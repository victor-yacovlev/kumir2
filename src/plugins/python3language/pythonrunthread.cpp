#include "pythonrunthread.h"
#include "interpretercallback.h"
#include "actorshandler.h"
#include "pyutils.h"
#include "interfaces/runinterface.h"

extern "C" {
#include <Python.h>
}

namespace Python3Language {

using namespace Shared;

PythonRunThread* PythonRunThread::self = 0;

PythonRunThread* PythonRunThread::instance(QObject *parent, const QString &extraPythonPath)
{
    if (!self) {
        self = new PythonRunThread(parent, extraPythonPath);
    }
    return self;
}

PythonRunThread::PythonRunThread(QObject *parent, const QString & extraPythonPath)
    : QThread(parent)
    , runMode_(RM_Idle)
    , callback_(InterpreterCallback::instance(this))
    , actorsHandler_(ActorsHandler::instance(this))
    , pythonPath_(extraPythonPath)
    , mutex_(new QMutex)
    , mainModuleName_("__main__")
    , runPauseSemaphore_(new QSemaphore(0))
    , runInputSemaphore_(new QSemaphore(0))
{
    connect(callback_, SIGNAL(errorMessageRequest(QString)),
            this, SIGNAL(errorOutputRequest(QString)),
            Qt::DirectConnection);
    connect(callback_, SIGNAL(outputMessageRequest(QString)),
            this, SIGNAL(outputRequest(QString)),
            Qt::DirectConnection);
    connect(callback_, SIGNAL(inputRequest()),
            this, SLOT(handlePythonInput()),
            Qt::DirectConnection);

    reset();
}

void PythonRunThread::reset()
{
    QMutexLocker l(mutex_);
    lineNumber_ = -1;
    errorText_.clear();
    testingResult_.clear();
    stepsCounted_ = 0;
    justStarted_ = true;
    stopping_ = false;
    releaseSemaphores();
    runPauseSemaphore_->acquire();
    runInputSemaphore_->acquire();
}

void PythonRunThread::releaseSemaphores()
{
    if (!runPauseSemaphore_->available()) {
        runPauseSemaphore_->release();
    }

    if (!runInputSemaphore_->available()) {
        runInputSemaphore_->release();
    }
}

void PythonRunThread::run()
{
    // Clear states
    reset();
    callback_->reset();
    actorsHandler_->reset();

    // Initialize interpreter in current thread
    PyGILState_Ensure();
    PyThreadState * py = Py_NewInterpreter();
    appendToSysPath(pythonPath_);   
    PyObject* py_run_wrapper = PyImport_ImportModule("run_wrapper");
    if (!py_run_wrapper) { printPythonTraceback(); return; }    
    PyEval_ReleaseThread(py);

    // Create actor 'modules'
    for (int i=0; i<actorsHandler_->size(); i++) {
        const QString source = actorsHandler_->moduleWrapper(i);
        const QString name = actorsHandler_->moduleName(i);
        createModuleFromSource(py, name, source);
    }

    // Prepare main program code
    mutex_->lock();
    PyEval_AcquireThread(py);
    PyObject * code = compileModule(
                sourceProgramPath_.isEmpty()
                ? QString("<program>") : sourceProgramPath_,
                sourceProgram_,
                &lineNumber_,
                &errorText_
                );
    PyEval_ReleaseThread(py);
    QByteArray mainModuleName = mainModuleName_;
    mutex_->unlock();

    if (!code) {
        // There is fatal syntax error in program
        Q_EMIT stopped(RunInterface::SR_Error);
    }
    else {
        PyEval_AcquireThread(py);
        // Set interpreter tracing
        PyEval_SetTrace(&python_trace_dispatch, 0);

        // Create main module
        // NOTE in this stage name must be '__main__' to ensure builtins available
        PyObject *mainModule = PyImport_AddModule("__main__");

        // Prepare globals
        PyObject *globals = PyModule_GetDict(mainModule);
        PyObject *pyModuleName = QStringToPyUnicode(mainModuleName);
        PyDict_SetItemString(globals, "__name__", pyModuleName);
        if ("__testing__"==mainModuleName) {
            PyObject *pyTestResult = PyLong_FromLong(0);
            PyDict_SetItemString(globals, "__test__", pyTestResult);
        }

        // Evaluate
        PyEval_EvalCode(code, globals, globals);

        // Get testing result if present
        if ("__testing__"==mainModuleName) {
            PyObject *pyTestResult = PyDict_GetItemString(globals, "__test__");
            if (pyTestResult) {
                int testingResult = PyLong_AsLong(pyTestResult);
                mutex_->lock();
                testingResult_ = QVariant(testingResult);
                qDebug() << "Testing result = " << testingResult;
                mutex_->unlock();
            }
        }

        // Unset interpreter tracing
        PyEval_SetTrace(0 ,0);
        PyEval_ReleaseThread(py);
    }

    // Finalize interpreter
    PyEval_AcquireThread(py);
    Py_EndInterpreter(py);
    py = 0;
    PyEval_ReleaseLock();

    Q_EMIT updateStepsCounter(stepsCounted_);

    RunInterface::StopReason exitStatus = RunInterface::SR_Done;
    mutex_->lock();
    if (stopping_)
        exitStatus = RunInterface::SR_UserTerminated;
    else if (errorText_.length() > 0)
        exitStatus = RunInterface::SR_Error;
    mutex_->unlock();
    Q_EMIT stopped(exitStatus);
    setMainModuleName("__main__");
}

int PythonRunThread::python_trace_dispatch(PyObject *, PyFrameObject *frame, int what, PyObject *arg)
{
    const QString fileName = PyUnicodeToQString(frame->f_code->co_filename);
    static const QString DummyFileName = QString::fromLatin1("<program>");
    const bool sameFile = DummyFileName==fileName || fileName==self->sourceProgramPath_;
    bool mustStop = false;

    if (!mustStop) {
        if (sameFile) {

            int lineNumber = PyFrame_GetLineNumber(frame) - 1;
            self->mutex_->lock();
            self->lineNumber_ = lineNumber;
            self->mutex_->unlock();

            if (PyTrace_LINE==what)
                self->dispatchLineChange();

            if (RM_StepOver==self->runMode_ || RM_StepIn==self->runMode_ || RM_StepOut==self->runMode_) {
                self->updateDebuggerVariablesModel(frame);
            }

            if (PyTrace_LINE==what) {
                if (RM_StepOver==self->runMode_ || RM_StepIn==self->runMode_) {
                    Q_EMIT self->stopped(RunInterface::SR_UserInteraction);
                    self->runPauseSemaphore_->acquire();
                }
            }

            if (PyTrace_EXCEPTION==what || PyTrace_C_EXCEPTION==what) {
                // Set error status
                PyObject * exception = PyTuple_GetItem(arg, 1);
                PyObject * message = PyObject_Str(exception);
                self->mutex_->lock();
                self->errorText_ = PyUnicodeToQString(message);
                self->mutex_->unlock();
                Py_XDECREF(message);
                Py_XDECREF(exception);
            }
        }
    }

    self->mutex_->lock();
    mustStop = self->stopping_;
    self->mutex_->unlock();

    return mustStop;
}

void PythonRunThread::terminate()
{
    mutex_->lock();
    stopping_ = true;
    mutex_->unlock();

    releaseSemaphores();
}

void PythonRunThread::dispatchLineChange()
{
    bool emitSteps = RM_StepOver==runMode_ || RM_StepIn==runMode_ || RM_StepOut==runMode_;
    bool justStarted = false;
    unsigned long int stepsCounter = 0;
    mutex_->lock();
    int lineNumber = lineNumber_;
    emitSteps = emitSteps || justStarted_;
    if (!justStarted_) {
        stepsCounted_++;
        stepsCounter = stepsCounted_;
        emitSteps = emitSteps || 0==stepsCounter%1000;
    }
    else {
        justStarted = true;
    }
    justStarted_ = false;
    mutex_->unlock();
    if (justStarted) {
        Q_EMIT updateStepsCounter(0);
    }
    else if (emitSteps) {
        Q_EMIT updateStepsCounter(stepsCounter);
    }

    // Highlight current line if need
    if (RM_StepOver==runMode_ || RM_StepIn==runMode_ || RM_StepOut==runMode_) {
        Q_EMIT lineChanged(lineNumber, 0, 0);
    }

}

void PythonRunThread::handlePythonInput()
{
    // Request raw input from GUI
    Q_EMIT inputRequest("s");

    // Wait until data written from GUI thread and semaphore reseased
    runInputSemaphore_->acquire();
}

void PythonRunThread::setInputResult(const QVariantList &results)
{
    Q_ASSERT(results.size()==0 || results.size()==1);
    const QString inputString = results.isEmpty() ? QString() : results.at(0).toString();

    callback_->setInputString(inputString);
    runInputSemaphore_->release();
}

void PythonRunThread::updateDebuggerVariablesModel(PyFrameObject *current_frame)
{
    // TODO implement me
}

void PythonRunThread::startOrContinue(const RunMode runMode)
{
    runMode_ = runMode;
    if (!isRunning()) {
        start();
    }
    else {
        runPauseSemaphore_->release();
    }
}

} // namespace Python3Language
