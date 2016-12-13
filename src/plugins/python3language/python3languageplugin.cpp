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
    , fileHandler_(new PyFileHandler(this))
    , pyMain_(0)
    , runner_(0)
    , sandboxWidget_(0)
    , syntaxCheckSettingsPage_(0)
{    
}

QList<QWidget *> Python3LanguagePlugin::settingsEditorPages()
{
    QList<QWidget*> result;
    if (!syntaxCheckSettingsPage_) {
        syntaxCheckSettingsPage_ = new SyntaxCheckSettingsPage(mySettings());
        connect (syntaxCheckSettingsPage_, SIGNAL(settingsChanged(QStringList)),
                 this, SLOT(updateSettings(QStringList)));
        result.append(syntaxCheckSettingsPage_);
    }
    return result;
}

Analizer::SourceFileInterface * Python3LanguagePlugin::sourceFileHandler()
{
    return fileHandler_;
}

QString Python3LanguagePlugin::initialize(const QStringList &, const ExtensionSystem::CommandLine &)
{
    qDebug() << "Registering metatypes";
    qRegisterMetaType<Python3Language::ValueRepresentation>("ValueRepresentation");
    qRegisterMetaType<QList<Python3Language::ValueRepresentation> >("QList<ValueRepresentation>");
    qDebug() << "Append _kumit to inittab";
    PyImport_AppendInittab("_kumir", &InterpreterCallback::__init__);
    PyImport_AppendInittab("_kumir_sandbox", SandboxWidget::__init__);
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
    qDebug() << "Py_Initialize()";
    Py_Initialize();
    qDebug() << "Initializing threads";
    PyEval_InitThreads();
    qDebug() << "Saving main thread state";
    pyMain_ = PyEval_SaveThread();
    qDebug() << "Acquring main thread lock";
    PyEval_AcquireThread(pyMain_);
    qDebug() << "Releasing main thread lock";
    PyEval_ReleaseLock();
    qDebug() << "Creating runner instance";
    runner_ = PythonRunThread::instance(this, myResourcesDir().absolutePath());
    qDebug() << "Connecting signals/slots";
    connectRunThreadSignals();
    qDebug() << "Initialization done";

    sandboxWidget_ = SandboxWidget::instance(myResourcesDir().absolutePath(), 0);

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
    Q_FOREACH ( PythonAnalizerInstance* instance, analizerInstances_ )
        instance->stopPythonInterpreter();
    QCoreApplication::instance()->processEvents();
    PyEval_AcquireThread(pyMain_);
    //    Py_Finalize();
}

void Python3LanguagePlugin::updateSettings(const QStringList & keys)
{
    if (syntaxCheckSettingsPage_) {
        syntaxCheckSettingsPage_->setSettingsObject(mySettings());
    }
    if (mySettings() && keys.contains(SyntaxCheckSettingsPage::UsePep8Key)) {
        Q_FOREACH(PythonAnalizerInstance * analizer, analizerInstances_) {
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
    static const QStringList ExtraPythonPath = extraPaths();
    analizerInstances_.append(new PythonAnalizerInstance(this, ExtraPythonPath));
    analizerInstances_.last()->setUsePep8(
                mySettings()->value(
                    SyntaxCheckSettingsPage::UsePep8Key,
                    SyntaxCheckSettingsPage::UsePep8DefaultValue
                    ).toBool()
                );
    return analizerInstances_.last();
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
    return sandboxWidget_;
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
    const QString programSource = fileHandler_->toString(fileHandler_->fromBytes(program.executableData));
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


} // namespace Python3Language



#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN(Python3Language::Python3LanguagePlugin)
#endif



