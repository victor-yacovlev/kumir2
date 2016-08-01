#ifndef PYTHON3LANGUAGE_PYTHONRUNTHREAD_H
#define PYTHON3LANGUAGE_PYTHONRUNTHREAD_H

#include <QtCore>
extern "C" {
#include <Python.h>
#include <frameobject.h>
}

#include "interfaces/runinterface.h"

namespace Python3Language {

class InterpreterCallback;
class ActorsHandler;
class VariablesModel;

typedef QPair<QString,quint32> BreakpointLocation;
struct BreakpointData {
    QString condition;
    quint32 ignoreCount;
    quint32 hitCount;
    inline explicit BreakpointData(): ignoreCount(0), hitCount(0) {}
};

class PythonRunThread : public QThread
{
    Q_OBJECT
public /*methods*/:
    static PythonRunThread * instance(QObject *parent = 0, const QString & extraPythonPath = QString());
    inline QString errorText() const { QMutexLocker l(mutex_); return errorText_; }
    inline QVariant testingResult() const { QMutexLocker l(mutex_); return testingResult_; }
    inline int currentLineNumber() const { QMutexLocker l(mutex_); return lineNumber_; }
    inline unsigned long int stepsCounted() const
    { QMutexLocker l(mutex_); return stepsCounted_; }
    inline void setTestingMode(bool v) { QMutexLocker l(mutex_); testingMode_ = v; }
    inline bool isTestingMode() const { QMutexLocker l(mutex_); return testingMode_; }
    inline bool hasPostRunSource() const { QMutexLocker l(mutex_); return postRunSource_.length() > 0; }
    QAbstractItemModel * variablesModel() const;
    inline bool canStepOut() const { QMutexLocker l(mutex_); return canStepOut_; }
    void setStdInStream(QTextStream * stream);
    void setStdOutStream(QTextStream * stream);

    Shared::RunInterface::RunMode currentRunMode() const;

    void forceGlobalVariableValue(const QByteArray & name, PyObject * value);
    inline void setTestRunCount(quint32 n) { QMutexLocker l(mutex_); testRunCount_ = n; }
    inline unsigned long testRunsLeft() const { QMutexLocker l(mutex_); return testRunCount_; }

    void removeAllBreakpoints();
    void insertSingleHitBreakpoint(const BreakpointLocation &location);
    void addOrChangeBreakpoint(const BreakpointLocation &location, const BreakpointData &data);
    void removeBreakpoint(const BreakpointLocation &location);

Q_SIGNALS:
    void errorOutputRequest(const QString &);
    void outputRequest(const QString & output);
    void inputRequest(const QString & format);
    void stopped(int reason);
    void updateStepsCounter(quint64);
    void lineChanged(int lineNo, quint32 colStart, quint32 colEnd);

public Q_SLOTS:
    inline void loadProgram(
            const QString & fileName,
            const QString & source,
            const QString & preRunSource,
            const QString & postRunSource,
            const QString & preTestSource,
            const QString & postTestSource
            )
    {
        sourceProgramPath_ = fileName;
        sourceProgram_ = source;
        preRunSource_ = preRunSource;
        postRunSource_ = postRunSource;
        preTestSource_ = preTestSource;
        postTestSource_ = postTestSource;
    }

    void startOrContinue(const Shared::RunInterface::RunMode runMode);
    void terminate();
    void setInputResult(const QVariantList &results);

private Q_SLOTS:
    void handlePythonInput();

private /*methods*/:
    explicit PythonRunThread(QObject *parent, const QString & extraPythonPath);
    void reset();
    void run();
    void updateDebuggerVariablesModel(PyFrameObject * current_frame);
    static int python_trace_dispatch(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg);
    void dispatchLineChange();
    bool checkForBreakpoint(const BreakpointLocation & location);
    void releaseSemaphores();


private /*fields*/:
    static PythonRunThread* self;
    QStack<Shared::RunInterface::RunMode> runMode_;
    InterpreterCallback * callback_;
    ActorsHandler* actorsHandler_;
    QString pythonPath_;
    PyObject * py_run_wrapper;
    QString sourceProgramPath_;
    QString sourceProgram_;    
    QMutex * mutex_;
    QString errorText_;
    int lineNumber_;
    unsigned long int stepsCounted_;
    bool justStarted_;
    bool stopping_;
    QByteArray mainModuleName_;
    bool testingMode_;
    QSemaphore * runPauseSemaphore_;
    QSemaphore * runInputSemaphore_;
    QVariant testingResult_;
    QString preRunSource_;
    QString postRunSource_;
    QString preTestSource_;
    QString postTestSource_;
    VariablesModel* variablesModel_;
    bool canStepOut_;
    QMap<QByteArray,PyObject*> forcedGlobalValues_;
    quint32 testRunCount_;
    QSet<BreakpointLocation> singleHits_;
    QMap<BreakpointLocation,BreakpointData> breakpoints_;
};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_PYTHONRUNTHREAD_H
