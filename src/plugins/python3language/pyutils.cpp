#include "pyutils.h"

namespace Python3Language {

extern QString PyUnicodeToQString(PyObject * unicode)
{
    Q_ASSERT(PyUnicode_Check(unicode));
    wchar_t * buffer = PyUnicode_AsWideCharString(unicode, 0);
    QString result = QString::fromWCharArray(buffer);
    PyMem_Free(buffer);
    return result;
}

extern PyObject* QStringToPyUnicode(const QString & qstring)
{
    std::wstring wstring = qstring.toStdWString();
    PyObject * result = PyUnicode_FromWideChar(wstring.c_str(), -1);
    Q_ASSERT(PyUnicode_Check(result));
    return result;
}

extern void printPythonTraceback()
{
    PyObject * ptype, *pvalue, *ptraceback;
    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
    PyObject * pvalueRepr = PyObject_Repr(pvalue);
    PyObject * ptracebackRepr = PyObject_Repr(ptraceback);

    QString qvalue = PyUnicodeToQString(pvalueRepr);
    QString qtraceback = PyUnicodeToQString(ptracebackRepr);

    qDebug() << qvalue;
    qDebug() << qtraceback;
}

QMap<QString,QVariant> PyObjectToPropertyMap(PyObject *object)
{
    PyObject * __dict__ = PyObject_GenericGetDict(object, 0);
    Q_ASSERT(PyDict_Check(__dict__));
    PyObject * keys = PyDict_Keys(__dict__);
    size_t count = PyObject_Length(keys);
    QMap<QString,QVariant> result;
    for (size_t i=0; i<count; i++) {
        PyObject * py_key = PyList_GetItem(keys, i);
        const QString key = PyUnicodeToQString(py_key);
        if (!key.startsWith("_")) {
            PyObject * py_value = PyDict_GetItem(__dict__, py_key);
            const QVariant value = PyObjectToQVariant(py_value);
            result[key] = value;
        }
    }
    return result;
}

extern QVariant PyObjectToQVariant(PyObject *object)
{
    const QByteArray objectTypeName(object->ob_type->tp_name);
    if (objectTypeName == "NoneType") {
        return QVariant();
    }
    if (PyTuple_Check(object) || PyList_Check(object))
        return PyListToQVariantList(object);
    else if (PyLong_Check(object) && PyLong_AsLong(object) >= 0)
        return QVariant::fromValue(PyLong_AsUnsignedLong(object));
    else if (PyLong_Check(object))
        return QVariant::fromValue(PyLong_AsLong(object));
    else if (PyFloat_Check(object))
        return QVariant(PyFloat_AsDouble(object));
    else if (PyBool_Check(object))
        return QVariant(PyLong_AsLong(object) != 0);
    else if (PyUnicode_Check(object))
        return QVariant(PyUnicodeToQString(object));
    else
        return QVariant(PyObjectToPropertyMap(object));
}

extern PyObject* QVariantToPyObject(const QVariant & value)
{
    PyObject* result = 0;
    if (QVariant::Int==value.type())
        result = PyLong_FromLong(value.toInt());
    else if (QVariant::UInt==value.type())
        result = PyLong_FromUnsignedLong(value.toUInt());
    else if (QVariant::Double==value.type())
        result = PyFloat_FromDouble(value.toDouble());
    else if (QVariant::Bool==value.type())
        result = value.toBool() ? Py_True : Py_False;
    else if (QVariant::Invalid==value.type())
        result = Py_None;
    else if (QVariant::Char==value.type() || QVariant::String==value.type())
        result = QStringToPyUnicode(value.toString());
    else if (QVariant::List==value.type())
        result = QVariantListToPyList(value.toList(), false);
    // TODO support more types?
    Py_XINCREF(result);
    return result;
}

extern QVariantList PyListToQVariantList(PyObject *list)
{
    Q_ASSERT(PyTuple_Check(list) || PyList_Check(list));
    bool isTuple = PyTuple_Check(list);
    QVariantList result;
    size_t size = PyObject_Length(list);
    for (size_t i=0; i<size; i++) {
        PyObject * py_item = 0;
        if (isTuple)
            py_item = PyTuple_GetItem(list, i);
        else
            py_item = PyList_GetItem(list, i);
        result.append(PyObjectToQVariant(py_item));
    }
    return result;
}

extern PyObject* QVariantListToPyList(const QVariantList &list, bool toTuple)
{
    PyObject * result = 0;
    if (toTuple)
        result = PyTuple_New(list.size());
    else
        result = PyList_New(list.size());
    for (size_t i=0; i<list.size(); i++) {
        const QVariant & item = list.at(i);
        PyObject * py_item = QVariantToPyObject(item);
        if (toTuple)
            PyTuple_SetItem(result, i, py_item);
        else
            PyList_SetItem(result, i, py_item);
    }
    return result;
}

extern QVariant callPythonFunction(
        PyThreadState * interpreter,
        PyObject * callable,
        const QVariantList & args
        )
{
    Q_ASSERT(PyCallable_Check(callable));
    PyEval_AcquireThread(interpreter);
    PyObject * py_args = QVariantListToPyList(args, true);
    PyObject * py_result = PyObject_CallObject(callable, py_args);
    QVariant result = PyObjectToQVariant(py_result);
    PyEval_ReleaseThread(interpreter);
    return result;
}

extern QVariant callPythonFunction(PyThreadState * interpreter, PyObject * callable)
{
    return callPythonFunction(interpreter, callable, QVariantList());
}

extern QVariant callPythonFunction(
        PyThreadState * interpreter,
        PyObject * callable,
        const QVariant & arg1
        )
{
    return callPythonFunction(interpreter, callable, QVariantList() << arg1);
}

extern QVariant callPythonFunction(
        PyThreadState * interpreter,
        PyObject * callable,
        const QVariant & arg1,
        const QVariant & arg2
        )
{
    return callPythonFunction(interpreter, callable, QVariantList() << arg1 << arg2);
}

extern void appendToSysPath(const QString & path)
{
    PyObject * sysPath = PySys_GetObject("path");
    PyObject * extraPath = QStringToPyUnicode(path);
    PyList_Insert(sysPath, 0, extraPath);
}

extern PyObject* compileModule(
        const QString &fileName,
        const QString &source,
        int * errorLineNumber,
        QString * errorText
        )
{
    const std::string cfileName(fileName.toLocal8Bit().constData());
    const std::string csource(source.toLocal8Bit().constData());
    PyObject* result = Py_CompileString(csource.c_str(), cfileName.c_str(), Py_file_input);
    if (!result) {
        PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
    }
    return result;
}

extern PyObject* createModuleFromSource(
        PyThreadState *interpreter,
        const QString &moduleName,
        const QString &moduleSource
        )
{
    const std::string cname = moduleName.toStdString();
    PyObject* module = 0;
    PyEval_AcquireThread(interpreter);
    PyObject* code = compileModule("<generated>", moduleSource);
    if (!code) {
        qDebug() << "Error creating " << moduleName;
        printPythonTraceback();
        qDebug() << moduleSource;
    }
    else {
        module = PyImport_ExecCodeModule(const_cast<char*>(cname.c_str()), code);
        if (!module) {
            printPythonTraceback();
        }
    }
    PyEval_ReleaseThread(interpreter);
    Py_XINCREF(module);
    return module;
}

}

