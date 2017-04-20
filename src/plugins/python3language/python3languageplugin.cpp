extern "C" {
#include <Python.h>
}

#include "python3languageplugin.h"
#include "analizerinstance.h"
#include "pythonrunthread.h"
#include "interpretercallback.h"
#include "pyfilehandler.h"
#include "pyutils.h"
#include "sandboxwidget.h"
#include "syntaxchecksettingspage.h"


namespace Python3Language {

Python3LanguagePlugin::Python3LanguagePlugin()
    : ExtensionSystem::KPlugin()
    , _fileHandler(new PyFileHandler(this))
    , runner_(0)
    , _sandboxWidget(0)
    , _syntaxCheckSettingsPage(0)
    , _interpreterForAnalizers(0)
{

}

Python3LanguagePlugin::~Python3LanguagePlugin()
{
    // Delete analizers first, then interpreters
    Q_FOREACH(PythonAnalizerInstance *a, _analizerInstances) {
        delete a;
    }
    _interpreterForAnalizers->deleteLater();
}

QList<QWidget *> Python3LanguagePlugin::settingsEditorPages()
{
    QList<QWidget*> result;
    if (!_syntaxCheckSettingsPage) {
        _syntaxCheckSettingsPage = new SyntaxCheckSettingsPage(mySettings());
        connect (_syntaxCheckSettingsPage, SIGNAL(settingsChanged(QStringList)),
                 this, SLOT(updateSettings(QStringList)));
        result.append(_syntaxCheckSettingsPage);
    }
    return result;
}

Analizer::SourceFileInterface * Python3LanguagePlugin::sourceFileHandler()
{
    return _fileHandler;
}

QString Python3LanguagePlugin::initialize(const QStringList &, const ExtensionSystem::CommandLine &)
{
    qDebug() << "Registering metatypes";
    qRegisterMetaType<Python3Language::ValueRepresentation>("ValueRepresentation");
    qRegisterMetaType<QList<Python3Language::ValueRepresentation> >("QList<ValueRepresentation>");
    qDebug() << "Append _kumit to inittab";
    PyImport_AppendInittab("_kumir", &InterpreterCallback::__init__);

#ifdef Q_OS_WIN32
    qDebug() << "Setting initial PYTHONPATH";
    static const QString KumirRoot =
            QDir::toNativeSeparators(
                QDir::cleanPath(
                    QCoreApplication::applicationDirPath() + "/../"
                    )
                );
    static const QStringList PathItems = QStringList()
            << KumirRoot + "\\share\\kumir2\\python3language"
            << KumirRoot + "\\python\\Lib"
            << KumirRoot + "\\python\\DLLs"
            << KumirRoot + "\\python\\Lib\\site-packages"
               ;
    static const QString PythonPath = PathItems.join(";");
    static wchar_t wPythonPath[4096];
    wPythonPath[PythonPath.length()+1] = L'\0';
    PythonPath.toWCharArray(wPythonPath);
    qDebug() << "Calling _wputenv";
    _wputenv_s(L"PYTHONPATH", wPythonPath);
#endif
    Py_Initialize();
    PyEval_InitThreads();
    runner_ = PythonRunThread::instance(this, myResourcesDir().absolutePath());
    connectRunThreadSignals();
    _sandboxWidget = new SandboxWidget(myResourcesDir().absolutePath(), 0);
    _interpreterForAnalizers = PyInterpreterProcess::create(true, this);

    return QString();
}

void Python3LanguagePlugin::connectRunThreadSignals()
{
    connect(runner_, SIGNAL(errorOutputRequest(QString)),
            this, SIGNAL(errorOutputRequest(QString)));
    connect(runner_, SIGNAL(outputRequest(QString)),
            this, SIGNAL(outputRequest(QString)));
    connect(runner_, SIGNAL(stopped(int)),
            this, SIGNAL(stopped(int)));
    connect(runner_, SIGNAL(updateStepsCounter(quint64)),
            this, SIGNAL(updateStepsCounter(quint64)));
    connect(runner_, SIGNAL(lineChanged(int,quint32,quint32)),
            this, SIGNAL(lineChanged(int,quint32,quint32)));
    connect(runner_, SIGNAL(inputRequest(QString)),
            this, SIGNAL(inputRequest(QString)));
    connect(this, SIGNAL(finishInput(QVariantList)),
            runner_, SLOT(setInputResult(QVariantList)));
}

void Python3LanguagePlugin::stop()
{
    QCoreApplication::instance()->processEvents();
}

void Python3LanguagePlugin::updateSettings(const QStringList & keys)
{
    if (_syntaxCheckSettingsPage) {
        _syntaxCheckSettingsPage->setSettingsObject(mySettings());
    }
    if (mySettings() && keys.contains(SyntaxCheckSettingsPage::UsePep8Key)) {
        Q_FOREACH(PythonAnalizerInstance * analizer, _analizerInstances) {
            analizer->setUsePep8(
                        mySettings()->value(
                            SyntaxCheckSettingsPage::UsePep8Key,
                            SyntaxCheckSettingsPage::UsePep8DefaultValue
                            ).toBool()
                        );
        }
    }
}


Analizer::InstanceInterface * Python3LanguagePlugin::createInstance()
{
    _analizerInstances.append(new PythonAnalizerInstance(this, _interpreterForAnalizers));
    _analizerInstances.last()->setUsePep8(
                mySettings()->value(
                    SyntaxCheckSettingsPage::UsePep8Key,
                    SyntaxCheckSettingsPage::UsePep8DefaultValue
                    ).toBool()
                );
    return _analizerInstances.last();
}

void Python3LanguagePlugin::setStdInTextStream(QTextStream *stream)
{
    runner_->setStdInStream(stream);
}

void Python3LanguagePlugin::setStdOutTextStream(QTextStream *stream)
{
    runner_->setStdOutStream(stream);
}

QWidget *Python3LanguagePlugin::startPageWidget()
{
    return _sandboxWidget;
}

QString Python3LanguagePlugin::startPageTitle() const
{
    return tr("Python Sandbox");
}

QList<QAction *> Python3LanguagePlugin::startPageActions() const
{
    return QList<QAction*>();
}

void Python3LanguagePlugin::setStartPageTitleChangeHandler(const QObject *, const char *)
{

}

RunInterface::RunMode Python3LanguagePlugin::currentRunMode() const
{
    return runner_->currentRunMode();
}


bool Python3LanguagePlugin::loadProgram(const RunnableProgram & program)
{
    const QString programSource = _fileHandler->toString(_fileHandler->fromBytes(program.executableData));
    const QString preRunSource = extractFunction(programSource, "__pre_run__");
    const QString postRunSource = extractFunction(programSource, "__post_run__");
    const QString preTestSource = extractFunction(programSource, "__pre_test__");
    const QString postTestSource = extractFunction(programSource, "__post_test__");
    runner_->loadProgram(program.sourceFileName, programSource, preRunSource, postRunSource, preTestSource, postTestSource);
    loadedProgramVersion_ = QDateTime::currentDateTime();
    return true;
}

QString Python3LanguagePlugin::extractFunction(const QString &source, const QString &funcName)
{
    QString result;
    QStringList sourceLines = source.split("\n", QString::KeepEmptyParts);
    int lineStart = -1;
    int lineCount = 0;
    for (int i=0; i<sourceLines.size(); i++) {
        if (-1==lineStart) {
            QString line = sourceLines[i].simplified();
            if (line.startsWith("def "+funcName)) {
                lineStart = i;
                lineCount = 1;
            }
        }
        else {
            QString line = sourceLines[i];
            if (line.isEmpty() || line.startsWith(' ') || line.startsWith('\t')) {
                lineCount ++;
            }
            else {
                break;
            }
        }
    }
    if (-1!=lineStart) {
        QStringList midLines = sourceLines.mid(lineStart, lineCount);
        result = midLines.join("\n");
    }
    return result;
}

QStringList Python3LanguagePlugin::extraPaths() const
{
    QStringList result;
    result.append(myResourcesDir().absolutePath());
    QDir thirdPartyRoot = QDir(myResourcesDir().absolutePath() + "/3rd-party");
    QStringList thirdPartyItems = thirdPartyRoot.entryList(QDir::Dirs);
    Q_FOREACH(const QString & item, thirdPartyItems) {
        const QString &itemPath = thirdPartyRoot.absoluteFilePath(item);
        result.append(itemPath);
        if (QDir(itemPath+"/src").exists()) {
            result.append(itemPath + "/src");
        }
    }
    return result;
}

bool Python3LanguagePlugin::canStepOut() const
{
    return runner_->canStepOut();
}

bool Python3LanguagePlugin::isTestingRun() const
{
    return runner_->isTestingMode();
}

bool Python3LanguagePlugin::hasMoreInstructions() const
{
    return false;
}

bool Python3LanguagePlugin::hasTestingEntryPoint() const
{
    return runner_->hasPostRunSource();
}

int Python3LanguagePlugin::currentLineNo() const
{
    return runner_->currentLineNumber();
}

QPair<quint32,quint32> Python3LanguagePlugin::currentColumn() const
{
    return QPair<quint32,quint32>(0, 0);
}

QString Python3LanguagePlugin::error() const
{
    return runner_->errorText();
}

QVariant Python3LanguagePlugin::valueStackTopItem() const
{
    return runner_->testingResult();
}

unsigned long int Python3LanguagePlugin::stepsCounted() const
{
    return runner_->stepsCounted();
}

QAbstractItemModel * Python3LanguagePlugin::debuggerVariablesViewModel() const
{
    return runner_->variablesModel();
}

void Python3LanguagePlugin::runBlind()
{
    runner_->startOrContinue(Shared::RunInterface::RM_Blind);
}

void Python3LanguagePlugin::runContinuous()
{
    runner_->startOrContinue(Shared::RunInterface::RM_Regular);
}

void Python3LanguagePlugin::runStepOver()
{
    runner_->startOrContinue(Shared::RunInterface::RM_StepOver);
}

void Python3LanguagePlugin::runStepInto()
{
    runner_->startOrContinue(Shared::RunInterface::RM_StepIn); // TODO implement me
}

void Python3LanguagePlugin::runToEnd()
{
    runner_->startOrContinue(Shared::RunInterface::RM_StepOut); // TODO implement me
}

void Python3LanguagePlugin::runTesting()
{
    runner_->setTestingMode(true);
    runner_->startOrContinue(Shared::RunInterface::RM_Blind);
}

void Python3LanguagePlugin::terminate()
{
    runner_->terminate();
}

void Python3LanguagePlugin::terminateAndWaitForStopped()
{
    terminate();
}

void Python3LanguagePlugin::removeAllBreakpoints()
{
    qDebug() << "Remove all breakpoints";
    runner_->removeAllBreakpoints();
}

void Python3LanguagePlugin::insertSingleHitBreakpoint(const QString &fileName, quint32 lineNo)
{
    BreakpointLocation location(fileName, lineNo);
    runner_->insertSingleHitBreakpoint(location);
}

void Python3LanguagePlugin::insertOrChangeBreakpoint(bool enabled, const QString &fileName, quint32 lineNo, quint32 ignoreCount, const QString &condition)
{
    qDebug() << "Insert breakpoint: " << fileName << ":" << lineNo;
    BreakpointLocation location(fileName, lineNo);
    BreakpointData data; data.condition = condition; data.ignoreCount = ignoreCount;
    runner_->addOrChangeBreakpoint(location, data);
}

void Python3LanguagePlugin::removeBreakpoint(const QString &fileName, quint32 lineNo)
{
    qDebug() << "Remove breakpoint: " << fileName << ":" << lineNo;
    BreakpointLocation location(fileName, lineNo);
    runner_->removeBreakpoint(location);
}

void Python3LanguagePlugin::createPluginSpec()
{
    _pluginSpec.name = "Python3Language";
    _pluginSpec.provides.append("Analizer");
    _pluginSpec.provides.append("Generator");
    _pluginSpec.provides.append("Runner");
}


} // namespace Python3Language



#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN(Python3Language::Python3LanguagePlugin)
#endif



