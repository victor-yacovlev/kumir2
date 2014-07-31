#ifndef PYTHON3LANGUAGE_INTERPRETERCALLBACK_H
#define PYTHON3LANGUAGE_INTERPRETERCALLBACK_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>

extern "C" {
#include <Python.h>
}

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

    inline void reset() { QMutexLocker l(mutex_); inputString_.clear();}
    inline void setInputString(const QString & text) { QMutexLocker l(mutex_); inputString_ = text; }

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
};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_INTERPRETERCALLBACK_H
