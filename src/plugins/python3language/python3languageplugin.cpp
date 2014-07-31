#include "python3languageplugin.h"
#include "analizerinstance.h"
#include "pythonrunthread.h"
#include "interpretercallback.h"

namespace Python3Language {

Python3LanguagePlugin::Python3LanguagePlugin()
    : ExtensionSystem::KPlugin()
    , runner_(0)
    , pyMain_(0)
{    
}

QString Python3LanguagePlugin::initialize(const QStringList &, const ExtensionSystem::CommandLine &)
{
    PyImport_AppendInittab("_kumir", &InterpreterCallback::__init__);
    Py_Initialize();
    PyEval_InitThreads();
    pyMain_ = PyEval_SaveThread();
    PyEval_AcquireThread(pyMain_);
    PyEval_ReleaseLock();
    runner_ = PythonRunThread::instance(this, myResourcesDir().absolutePath());
    connectRunThreadSignals();
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


bool Python3LanguagePlugin::loadProgram(const QString &fileName, const QByteArray &source, const SourceInfo &)
{
    runner_->loadProgram(fileName, QString::fromUtf8(source));
    loadedProgramVersion_ = QDateTime::currentDateTime();
    return true;
}

bool Python3LanguagePlugin::canStepOut() const
{
    return false;
}

bool Python3LanguagePlugin::isTestingRun() const
{
    return "__testing__" == runner_->mainModuleName();
}

bool Python3LanguagePlugin::hasMoreInstructions() const
{
    return false;
}

bool Python3LanguagePlugin::hasTestingEntryPoint() const
{
    return true;
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
    return 0;
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
    runner_->startOrContinue(RM_StepOver); // TODO implement me
}

void Python3LanguagePlugin::runToEnd()
{
    runner_->startOrContinue(RM_StepOver); // TODO implement me
}

void Python3LanguagePlugin::runTesting()
{
    runner_->setMainModuleName("__testing__");
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

