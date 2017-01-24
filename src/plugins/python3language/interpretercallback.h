#ifndef PYTHON3LANGUAGE_INTERPRETERCALLBACK_H
#define PYTHON3LANGUAGE_INTERPRETERCALLBACK_H

extern "C" {
#include <Python.h>
}

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QStringList>
#include <QTextStream>
#include <QMap>
#include <QVariant>


namespace Python3Language {

class Python3LanguagePlugin;

class InterpreterCallback : public QObject
{
    Q_OBJECT
public:
    static InterpreterCallback * instance(QObject * parent = 0);

    // Functions exported to python
    static PyObject* __init__();
    static PyObject* debug(PyObject*, PyObject * args);
    static PyObject* write_output(PyObject *, PyObject * args);
    static PyObject* write_error(PyObject *, PyObject * args);
    static PyObject* read_input(PyObject *, PyObject *);
    static PyObject* actor_call(PyObject *, PyObject * args);
    static PyObject* get_output_buffer(PyObject *, PyObject *);
    static PyObject* simulate_input(PyObject*, PyObject *args);
    static PyObject* force_global_variable_value(PyObject*, PyObject *args);
    static PyObject* set_permanent_value(PyObject*, PyObject *args);
    static PyObject* get_permanent_value(PyObject*, PyObject *args);
    static PyObject* del_permanent_value(PyObject*, PyObject *args);
    static PyObject* set_test_run_count(PyObject*, PyObject *args);
    static PyObject* get_test_runs_left(PyObject*, PyObject *);

    inline void reset() {
        QMutexLocker l(mutex_);
        inputString_.clear();
        outputBuffer_.clear();
        simulatingInputBuffer_.clear();
    }
    inline void setInputString(const QString & text) { QMutexLocker l(mutex_); inputString_ = text; }
    inline void setStdInStream(QTextStream * stream) { QMutexLocker l(mutex_); overridenStdIn_ = stream; }
    inline void setStdOutStream(QTextStream * stream) { QMutexLocker l(mutex_); overridenStdOut_ = stream; }

Q_SIGNALS:
    void outputMessageRequest(const QString & text);
    void errorMessageRequest(const QString & text);
    void inputRequest();

private /*methods*/:
    explicit InterpreterCallback(QObject *parent = 0);

private /*fields*/:
    static InterpreterCallback * self;
    QMutex * mutex_;
    QString inputString_;
    QString outputBuffer_;
    QStringList simulatingInputBuffer_;
    QTextStream * overridenStdIn_;
    QTextStream * overridenStdOut_;
    QMap<QString, QVariant> permanentStorage_;
};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_INTERPRETERCALLBACK_H
