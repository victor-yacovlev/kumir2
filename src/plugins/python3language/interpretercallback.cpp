#include "interpretercallback.h"
#include "actorshandler.h"
#include "pyutils.h"
#include "pythonrunthread.h"

namespace Python3Language {

InterpreterCallback::InterpreterCallback(QObject *parent)
    : QObject(parent)
    , mutex_(new QMutex)
    , overridenStdIn_(0)
    , overridenStdOut_(0)
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
        { "get_output_buffer", get_output_buffer, METH_VARARGS, "Get output buffer text" },
        { "simulate_input", simulate_input, METH_VARARGS, "Pushes a value to be read from stdin" },
        { "force_global_variable_value", force_global_variable_value, METH_VARARGS, "Overrides global variable value and protects from user change" },
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
    self->mutex_->lock();
    self->outputBuffer_ += message;
    self->mutex_->unlock();
    if (self->overridenStdOut_) {
        *self->overridenStdOut_ << message;
    }
    else {
        Q_EMIT self->outputMessageRequest(message);
    }
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
    bool hasSimulatingInput = self->simulatingInputBuffer_.size() > 0;
    self->mutex_->unlock();
    PyObject* result = 0;
    if (hasSimulatingInput) {
        self->mutex_->lock();
        QString line = self->simulatingInputBuffer_.front();
        self->simulatingInputBuffer_.pop_front();
        line += "\n";
        self->mutex_->unlock();
        result = QStringToPyUnicode(line);
    }
    else if (self->overridenStdIn_) {
        const QString line = self->overridenStdIn_->readLine();
        result = QStringToPyUnicode(line);
    }
    else {
        Q_EMIT self->inputRequest();
        self->mutex_->lock();
        result = QStringToPyUnicode(self->inputString_);
        self->mutex_->unlock();
    }
    Py_INCREF(result);
    return result;
}

PyObject* InterpreterCallback::get_output_buffer(PyObject *, PyObject *)
{
    self->mutex_->lock();
    PyObject* result = QStringToPyUnicode(self->outputBuffer_);
    self->mutex_->unlock();
    Py_INCREF(result);
    return result;
}

PyObject* InterpreterCallback::simulate_input(PyObject *, PyObject *args)
{
    QMutexLocker l(self->mutex_);
    size_t count = PyTuple_Size(args);
    for (size_t i=0; i<count; i++) {
        PyObject * val = PyTuple_GetItem(args, i);
        PyObject * repr = PyObject_Str(val);
        QString line = PyUnicodeToQString(repr);
        self->simulatingInputBuffer_.push_back(line);
    }
    Py_RETURN_NONE;
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

PyObject* InterpreterCallback::force_global_variable_value(PyObject *, PyObject *args)
{
    if (PyTuple_Size(args)==2) {
        PyObject * pyName = PyTuple_GetItem(args, 0);
        PyObject * pyValue = PyTuple_GetItem(args, 1);
        if (PyUnicode_Check(pyName)) {
            Py_INCREF(pyValue);
            const QByteArray qName = PyUnicodeToQString(pyName).toLatin1();
            PythonRunThread::instance()->forceGlobalVariableValue(qName, pyValue);
        }
    }
    Py_RETURN_NONE;
}


} // namespace Python3Language
