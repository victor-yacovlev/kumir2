#include "interpretercallback.h"
#include "actorshandler.h"
#include "pyutils.h"

namespace Python3Language {

InterpreterCallback::InterpreterCallback(QObject *parent)
    : QObject(parent)
    , mutex_(new QMutex)
{
}

InterpreterCallback* InterpreterCallback::self = 0;

InterpreterCallback* InterpreterCallback::instance(QObject * parent)
{
    if (!self)
        self = new InterpreterCallback(parent);
    return self;
}

PyObject* InterpreterCallback::__init__()
{
    static PyMethodDef methods[] = {
        { "debug", debug, METH_VARARGS, "Debug using qDebug" },
        { "write_output", write_output, METH_VARARGS, "Output regular text to I/O area" },
        { "write_error", write_output, METH_VARARGS, "Output error (red) text to I/O area" },
        { "read_input", read_input, METH_VARARGS, "Read text line from I/O area" },
        { "actor_call", actor_call, METH_VARARGS, "Call actor method" },
        { 0, 0, 0, 0 }
    };

    static PyModuleDef module = {
        PyModuleDef_HEAD_INIT,
        "_kumir", 0, -1, methods,
        0, 0, 0, 0
    };

    return PyModule_Create(&module);
}

PyObject* InterpreterCallback::debug(PyObject *, PyObject *args)
{
    PyObject * msg = PyTuple_GetItem(args, 0);
    QString message = PyUnicodeToQString(msg);
    qDebug() << message;
    Py_RETURN_NONE;
}

PyObject* InterpreterCallback::write_output(PyObject *, PyObject *args)
{
    PyObject * msg = PyTuple_GetItem(args, 0);
    QString message = PyUnicodeToQString(msg);
    Q_EMIT self->outputMessageRequest(message);
    Py_RETURN_NONE;
}

PyObject* InterpreterCallback::write_error(PyObject *, PyObject *args)
{
    PyObject * msg = PyTuple_GetItem(args, 0);
    QString message = PyUnicodeToQString(msg);
    Q_EMIT self->errorMessageRequest(message);
    Py_RETURN_NONE;
}

PyObject* InterpreterCallback::read_input(PyObject *, PyObject *)
{
    self->mutex_->lock();
    self->inputString_.clear();
    self->mutex_->unlock();
    Q_EMIT self->inputRequest();
    QMutexLocker l(self->mutex_);
    PyObject * result = QStringToPyUnicode(self->inputString_);
    Py_INCREF(result);
    return result;
}


PyObject* InterpreterCallback::actor_call(PyObject *, PyObject *args)
{
    PyObject* pyModId = PyTuple_GetItem(args, 0);
    PyObject* pyFunId = PyTuple_GetItem(args, 1);
    PyObject* pyArgs = PyTuple_GetItem(args, 2);
    int modId = PyLong_AsLong(pyModId);
    int funId = PyLong_AsLong(pyFunId);
    const QVariantList funcArgs = PyObjectToQVariant(pyArgs).toList();
    const QVariantList callResult = ActorsHandler::instance()->call(modId, funId, funcArgs).toList();
    PyObject* pyCallResult = PyTuple_New(2);
    const QString error = callResult.at(0).toString();
    PyObject* pyError = QStringToPyUnicode(error);
    PyTuple_SetItem(pyCallResult, 0, pyError);
    const QVariant result = callResult.at(1);
    PyObject* pyResult = 0;
    if (QVariant::List == result.type()) {
        pyResult = QVariantListToPyList(result.toList(), true);
    }
    else {
        pyResult = QVariantToPyObject(result);
    }
    PyTuple_SetItem(pyCallResult, 1, pyResult);
    return pyCallResult;
}


} // namespace Python3Language
