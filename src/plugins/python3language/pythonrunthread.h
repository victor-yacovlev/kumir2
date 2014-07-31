#ifndef PYTHON3LANGUAGE_PYTHONRUNTHREAD_H
#define PYTHON3LANGUAGE_PYTHONRUNTHREAD_H

#include <QtCore>
extern "C" {
#include <Python.h>
#include <frameobject.h>
}

#include "runmode.h"

namespace Python3Language {

class InterpreterCallback;
class ActorsHandler;

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
    inline void setMainModuleName(const QByteArray & name)
    { QMutexLocker l(mutex_); mainModuleName_ = name; }
    inline QByteArray mainModuleName() const { QMutexLocker l(mutex_); return mainModuleName_; }

Q_SIGNALS:
    void errorOutputRequest(const QString &);
    void outputRequest(const QString & output);
    void inputRequest(const QString & format);
    void stopped(int reason);
    void updateStepsCounter(quint64);
    void lineChanged(int lineNo, quint32 colStart, quint32 colEnd);

public Q_SLOTS:
    inline void loadProgram(const QString & fileName, const QString & source)
    { sourceProgramPath_ = fileName; sourceProgram_ = source; }

    void startOrContinue(const RunMode runMode);
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
    void releaseSemaphores();


private /*fields*/:
    static PythonRunThread* self;
    RunMode runMode_;
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
    QSemaphore * runPauseSemaphore_;
    QSemaphore * runInputSemaphore_;
    QVariant testingResult_;
};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_PYTHONRUNTHREAD_H
