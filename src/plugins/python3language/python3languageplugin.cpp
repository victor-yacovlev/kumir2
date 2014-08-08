#include "python3languageplugin.h"
#include "analizerinstance.h"
#include "pythonrunthread.h"
#include "interpretercallback.h"
#include "pyfilehandler.h"
#include "pyutils.h"

namespace Python3Language {

Python3LanguagePlugin::Python3LanguagePlugin()
    : ExtensionSystem::KPlugin()
    , fileHandler_(new PyFileHandler(this))
    , runner_(0)
    , pyMain_(0)
{    
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
    Py_Finalize();
}

Analizer::InstanceInterface * Python3LanguagePlugin::createInstance()
{
    static const QString ExtraPythonPath = myResourcesDir().absolutePath();
    analizerInstances_.append(new PythonAnalizerInstance(this, ExtraPythonPath));
    return analizerInstances_.last();
}


bool Python3LanguagePlugin::loadProgram(const RunnableProgram & program)
{
    const QString programSource = fileHandler_->toString(fileHandler_->fromBytes(program.executableData));
    const QString preRunSource = extractFunction(programSource, "__pre_run__");
    const QString postRunSource = extractFunction(programSource, "__post_run__");
    runner_->loadProgram(program.sourceFileName, programSource, preRunSource, postRunSource);
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
    runner_->startOrContinue(RM_Blind);
}

void Python3LanguagePlugin::runContinuous()
{
    runner_->startOrContinue(RM_Regular);
}

void Python3LanguagePlugin::runStepOver()
{
    runner_->startOrContinue(RM_StepOver);
}

void Python3LanguagePlugin::runStepInto()
{
    runner_->startOrContinue(RM_StepIn); // TODO implement me
}

void Python3LanguagePlugin::runToEnd()
{
    runner_->startOrContinue(RM_StepOut); // TODO implement me
}

void Python3LanguagePlugin::runTesting()
{
    runner_->setTestingMode(true);
    runner_->startOrContinue(RM_Blind);
}

void Python3LanguagePlugin::terminate()
{
    runner_->terminate();
}

void Python3LanguagePlugin::terminateAndWaitForStopped()
{
    terminate();
}


} // namespace Python3Language

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN(Python3Language::Python3LanguagePlugin)
#endif

