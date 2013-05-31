#include "python3languageplugin.h"
#include "Python.h"
#include <iostream>

namespace Python3Language {

Python3LanguagePlugin::Python3LanguagePlugin()
    : ExtensionSystem::KPlugin()
    , analizerModule_(NULL)
    , runModule_(NULL)
{

}

static Python3LanguagePlugin * PLUGIN = nullptr;

static ::PyObject* _kumir_debug(PyObject *, PyObject *args)
{
    ::PyObject * msg = PyTuple_GetItem(args, 0);
    wchar_t * wcs = PyUnicode_AsWideCharString(msg, NULL);
    QString qs = QString::fromWCharArray(wcs);
    qDebug() << qs;
    PyMem_Free(wcs);
    Py_INCREF(Py_None);
    return Py_None;
}

static ::PyObject* _kumir_write_to_stdout(PyObject *, PyObject *args)
{
    ::PyObject * msg = PyTuple_GetItem(args, 0);
    QString qmsg;
    if (PyUnicode_Check(msg)) {
        wchar_t * buffer = PyUnicode_AsWideCharString(msg, 0);
        qmsg = QString::fromWCharArray(buffer);
        PyMem_Free(buffer);
    }
    else if (PyBytes_Check(msg)) {
        char * buffer = PyBytes_AsString(msg);
        qmsg = QString::fromLocal8Bit(buffer);
        PyMem_Free(buffer);
    }
    if (PLUGIN) {
        PLUGIN->handlePythonOutput(qmsg);
    }
    else {
        qDebug() << qmsg;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static ::PyObject* _kumir_write_to_stderr(PyObject *, PyObject *args)
{
    ::PyObject * msg = PyTuple_GetItem(args, 0);
    QString qmsg;
    if (PyUnicode_Check(msg)) {
        wchar_t * buffer = PyUnicode_AsWideCharString(msg, 0);
        qmsg = QString::fromWCharArray(buffer);
        PyMem_Free(buffer);
    }
    else if (PyBytes_Check(msg)) {
        char * buffer = PyBytes_AsString(msg);
        qmsg = QString::fromLocal8Bit(buffer);
        PyMem_Free(buffer);
    }
    if (PLUGIN) {
        PLUGIN->handlePythonError(qmsg);
    }
    else {
        qDebug() << qmsg;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static ::PyObject* _kumir_notify_line_changed(PyObject *, PyObject *args)
{
    ::PyObject * msg = PyTuple_GetItem(args, 0);
    int lineNo = PyLong_AsLong(msg);
    if (PLUGIN) {
        PLUGIN->handlePythonLineChanged(lineNo);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static ::PyMethodDef _kumirMethods[] = {
    { "debug", _kumir_debug, METH_VARARGS, ""},
    { "write_to_stdout", _kumir_write_to_stdout, METH_VARARGS, ""},
    { "write_to_stderr", _kumir_write_to_stderr, METH_VARARGS, ""},
    { "notify_line_changed", _kumir_notify_line_changed, METH_VARARGS, ""},
    { NULL, NULL, 0, NULL }
};

static ::PyModuleDef _kumirModule = {
    PyModuleDef_HEAD_INIT, "_kumir", NULL, -1, _kumirMethods,
    NULL, NULL, NULL, NULL
};

static ::PyObject* PyInit__kumir(void)
{
    return PyModule_Create(&_kumirModule);
}

QString Python3LanguagePlugin::initialize(const QStringList &)
{    
    static const QString pyLibPath = qApp->property("sharePath").toString()+"/python3language";
    static const QByteArray analizer_py_Path = pyLibPath.toLocal8Bit();
    char * analizer_py = (char*)calloc(analizer_py_Path.size(), sizeof(char));
    strcpy(analizer_py, analizer_py_Path.constData());
    PyImport_AppendInittab("_kumir", &PyInit__kumir);
//    PyImport_AppendInittab("__builtins__", &PyEval_GetBuiltins);
    Py_Initialize();    
    ::PyObject * sysPath = PySys_GetObject("path");
    ::PyObject * kumirPath = PyUnicode_FromString(analizer_py);
    PyList_Insert(sysPath, 0, kumirPath);
    analizerModule_ = PyImport_ImportModule("analizer");
    if (!analizerModule_) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
        return "Can't import python module 'analizer' using path " + pyLibPath;
    }
    runModule_ = PyImport_ImportModule("run");
    if (!runModule_) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
        return "Can't import python module 'run' using path " + pyLibPath;
    }
    PLUGIN = this;
    return "";
}

int Python3LanguagePlugin::newDocument()
{    
    ::PyObject* py_new_document = PyObject_GetAttrString(analizerModule_,
                                                    "new_document");
    if (!py_new_document) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
    }
    ::PyObject* py_id = PyObject_CallFunction(py_new_document, 0);
    if (!py_id) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
    }
    int result = PyLong_AsLong(py_id);
    Py_DecRef(py_id);
    return result;
}

void Python3LanguagePlugin::setSourceText(int documentId, const QString &text)
{    
    ::PyObject * py_set_source_text = PyObject_GetAttrString(analizerModule_, "set_source_text");
    ::PyObject * args = PyTuple_New(2);
    std::wstring ws = text.toStdWString();
    ::PyObject * id = PyLong_FromLong(documentId);
    ::PyObject * txt = PyUnicode_FromWideChar(ws.c_str(), ws.length());
    PyTuple_SetItem(args, 0, id);
    PyTuple_SetItem(args, 1, txt);
    ::PyObject *pRes = PyObject_CallObject(py_set_source_text, args);
    Py_DECREF(args);
    if (!pRes) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
    }
}

void Python3LanguagePlugin::setHiddenText(int documentId, const QString &text, int baseLine)
{

}

void Python3LanguagePlugin::setHiddenTextBaseLine(int documentId, int baseLine)
{

}

void Python3LanguagePlugin::changeSourceText(int documentId, const QList<ChangeTextTransaction> &changes)
{

}

QList<Error> Python3LanguagePlugin::errors(int documentId) const
{
    QList<Error> result;
    ::PyObject * py_errors = PyObject_GetAttrString(analizerModule_, "errors");
    ::PyObject * args = PyTuple_New(1);
    ::PyObject * id = PyLong_FromLong(documentId);
    PyTuple_SetItem(args, 0, id);
    ::PyObject *pRes = PyObject_CallObject(py_errors, args);
    Py_DECREF(args);
    if (!pRes) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
        return result;
    }
    int sz = PyList_Size(pRes);
    for (int i=0; i<sz; i++) {
        ::PyObject * item = PyList_GetItem(pRes, i);
        ::PyObject * lineNo = PyTuple_GetItem(item, 0);
        ::PyObject * offset = PyTuple_GetItem(item, 1);
        ::PyObject * length = PyTuple_GetItem(item, 2);
        ::PyObject * text = PyTuple_GetItem(item, 3);
        Error error;
        error.line = PyLong_AsLong(lineNo);
        error.start = PyLong_AsLong(offset);
        error.len = PyLong_AsLong(length);
        wchar_t * buffer = PyUnicode_AsWideCharString(text, 0);
        error.code = QString::fromWCharArray(buffer);
        if (error.code.length() > 0)
            error.code[0] = error.code[0].toUpper();
        PyMem_Free(buffer);
        result.push_back(error);
    }
    return result;
}

QList<LineProp> Python3LanguagePlugin::lineProperties(int documentId) const
{
    QList<LineProp> result;
    ::PyObject * py_line_prop = PyObject_GetAttrString(analizerModule_, "line_properties");
    ::PyObject * args = PyTuple_New(1);
    ::PyObject * id = PyLong_FromLong(documentId);
    PyTuple_SetItem(args, 0, id);
    ::PyObject *pRes = PyObject_CallObject(py_line_prop, args);
    Py_DECREF(args);
    if (!pRes) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
        return result;
    }
    int propsCount = PyList_Size(pRes);
    for (int i=0; i<propsCount; i++) {
        ::PyObject * item = PyList_GetItem(pRes, i);
        int propLength = PyList_Size(item);
        LineProp lp(propLength);
        for (int j=0; j<propLength; j++) {
            ::PyObject * propElem = PyList_GetItem(item, j);
            unsigned long val = PyLong_AsUnsignedLong(propElem);
            lp[j] = (Shared::LexemType)val;
        }
        result.push_back(lp);
    }
    return result;
}

QList<QPoint> Python3LanguagePlugin::lineRanks(int documentId) const
{
    return QList<QPoint>();
}

QStringList Python3LanguagePlugin::imports(int documentId) const
{
    return QStringList();
}

LineProp Python3LanguagePlugin::lineProp(int documentId, int lineNo, const QString &text) const
{
    LineProp lp(text.size());
    lp.fill(LxTypeEmpty);
    if (text.length()==0)
        return lp;
    ::PyObject * py_line_prop = PyObject_GetAttrString(analizerModule_, "line_prop");
    ::PyObject * args = PyTuple_New(3);
    std::wstring ws = text.toStdWString();
    ::PyObject * txt = PyUnicode_FromWideChar(ws.c_str(), ws.length());
    ::PyObject * id = PyLong_FromLong(documentId);
    ::PyObject * line_no = PyLong_FromLong(lineNo);
    PyTuple_SetItem(args, 0, id);
    PyTuple_SetItem(args, 1, line_no);
    PyTuple_SetItem(args, 2, txt);
    ::PyObject *pRes = PyObject_CallObject(py_line_prop, args);
    Py_DECREF(args);
    if (!pRes) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
        return lp;
    }
    int sz = PyList_Size(pRes);
    for (int i=0; i<sz; i++) {
        ::PyObject * item = PyList_GetItem(pRes, i);
        unsigned long val = PyLong_AsUnsignedLong(item);
        lp[i] = (Shared::LexemType)val;
    }
    return lp;
}

std::string Python3LanguagePlugin::rawSourceData(int documentId) const
{
    return "";
}

QString Python3LanguagePlugin::createImportStatementLine(const QString &importName) const
{
    return "import " + importName;
}

void Python3LanguagePlugin::dropDocument(int documentId)
{

}

bool Python3LanguagePlugin::loadProgram(const QString &fileName, const QByteArray & source, ProgramFormat )
{
    ::PyObject * py_load_program = PyObject_GetAttrString(runModule_,
                                                          "load_program");
    ::PyObject * args = PyTuple_New(2);
    std::wstring ws_fileName = fileName.toStdWString();
    ::PyObject * py_file_name =
            PyUnicode_FromWideChar(ws_fileName.c_str(), ws_fileName.length());
    ::PyObject * py_source = PyBytes_FromStringAndSize(source.data(), source.size());
    PyTuple_SetItem(args, 0, py_file_name);
    PyTuple_SetItem(args, 1, py_source);
    ::PyObject * py_result = PyObject_CallObject(py_load_program, args);
    Py_DECREF(args);
    if (!py_result) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
        return false;
    }
    bool result = PyLong_AsLong(py_result) != 0;
    return result;
}

QDateTime Python3LanguagePlugin::loadedProgramVersion() const
{
    return QDateTime::currentDateTime();
}

void Python3LanguagePlugin::setSourceDirName(int documentId, const QString &dirPath)
{

}

bool Python3LanguagePlugin::canStepOut() const
{
    return false;
}

void Python3LanguagePlugin::runBlind()
{
    ::PyObject * py_function = PyObject_GetAttrString(runModule_,
                                                       "run_blind");
    ::PyObject * args = PyTuple_New(0);
    ::PyObject * py_result = PyObject_CallObject(py_function, args);
    Py_DECREF(args);
    if (!py_result) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
    }
    using namespace Shared;
    if (error().length() > 0) {
        Q_EMIT stopped(RunInterface::SR_Error);
    }
    else {
        Q_EMIT stopped(RunInterface::SR_Done);
    }
}

void Python3LanguagePlugin::runContinuous()
{
    ::PyObject * py_function = PyObject_GetAttrString(runModule_,
                                                       "run_continuous");
    ::PyObject * args = PyTuple_New(0);
    ::PyObject * py_result = PyObject_CallObject(py_function, args);
    Py_DECREF(args);
    if (!py_result) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
    }
    using namespace Shared;
    if (error().length() > 0) {
        Q_EMIT stopped(RunInterface::SR_Error);
    }
    else {
        Q_EMIT stopped(RunInterface::SR_Done);
    }
}

void Python3LanguagePlugin::runStepOver()
{
    ::PyObject * py_function = PyObject_GetAttrString(runModule_,
                                                       "run_step_over");
    ::PyObject * args = PyTuple_New(0);
    ::PyObject * py_result = PyObject_CallObject(py_function, args);
    Py_DECREF(args);
    if (!py_result) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
    }
    using namespace Shared;
    if (error().length() > 0) {
        Q_EMIT stopped(RunInterface::SR_Error);
    }
    else {
        Q_EMIT stopped(RunInterface::SR_Done);
    }
}

void Python3LanguagePlugin::runStepInto()
{
    ::PyObject * py_function = PyObject_GetAttrString(runModule_,
                                                       "run_step_into");
    ::PyObject * args = PyTuple_New(0);
    ::PyObject * py_result = PyObject_CallObject(py_function, args);
    Py_DECREF(args);
    if (!py_result) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
    }
    using namespace Shared;
    if (error().length() > 0) {
        Q_EMIT stopped(RunInterface::SR_Error);
    }
    else {
        Q_EMIT stopped(RunInterface::SR_Done);
    }
}

void Python3LanguagePlugin::runStepOut()
{
    ::PyObject * py_function = PyObject_GetAttrString(runModule_,
                                                       "run_step_out");
    ::PyObject * args = PyTuple_New(0);
    ::PyObject * py_result = PyObject_CallObject(py_function, args);
    Py_DECREF(args);
    if (!py_result) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
    }
    using namespace Shared;
    if (error().length() > 0) {
        Q_EMIT stopped(RunInterface::SR_Error);
    }
    else {
        Q_EMIT stopped(RunInterface::SR_Done);
    }
}

void Python3LanguagePlugin::runTesting()
{
    Q_EMIT stopped(RunInterface::SR_Error);
}

void Python3LanguagePlugin::terminate()
{

}

bool Python3LanguagePlugin::hasMoreInstructions() const
{
    ::PyObject * py_function = PyObject_GetAttrString(runModule_,
                                                       "has_more_instructions");
    ::PyObject * args = PyTuple_New(0);
    ::PyObject * py_result = PyObject_CallObject(py_function, args);
    Py_DECREF(args);
    if (!py_result) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
        return false;
    }
    bool result = PyLong_AsLong(py_result) != 0;
    return result;
}

int Python3LanguagePlugin::currentLineNo() const
{
    ::PyObject * py_function = PyObject_GetAttrString(runModule_,
                                                       "current_line_no");
    ::PyObject * args = PyTuple_New(0);
    ::PyObject * py_result = PyObject_CallObject(py_function, args);
    Py_DECREF(args);
    if (!py_result) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
        return false;
    }
    int result = PyLong_AsLong(py_result);
    return result;
}

QString Python3LanguagePlugin::error() const
{
    ::PyObject * py_get_error = PyObject_GetAttrString(runModule_,
                                                       "get_error");
    ::PyObject * args = PyTuple_New(0);
    ::PyObject * py_result = PyObject_CallObject(py_get_error, args);
    Py_DECREF(args);
    if (!py_result) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
        return "";
    }
    wchar_t * buffer = PyUnicode_AsWideCharString(py_result, 0);
    const QString result = QString::fromWCharArray(buffer);
    PyMem_Free(buffer);
    return result;
}

QVariant Python3LanguagePlugin::valueStackTopItem() const
{
    return QVariant();
}

QMap<QString,QVariant> Python3LanguagePlugin::getScalarLocalValues(int frameNo) const
{
    return QMap<QString,QVariant>();
}

QMap<QString,QVariant> Python3LanguagePlugin::getScalarGlobalValues(const QString & moduleName) const
{
    return QMap<QString,QVariant>();
}

QVariantList Python3LanguagePlugin::getLocalTableValues(int frameNo, int maxCount, const QString & name,
                                 const QList< QPair<int,int> > & ranges, bool & complete) const
{
    return QVariantList();
}

QVariantList Python3LanguagePlugin::getGlobalTableValues(const QString & moduleName, int maxCount, const QString & name,
                                  const QList< QPair<int,int> > & ranges, bool & complete) const
{
    return QVariantList();
}

QVariant Python3LanguagePlugin::getLocalTableValue(int frameNo, const QString & name, const QList<int> & indeces) const
{
    return QVariant();
}

QVariant Python3LanguagePlugin::getGlobalTableValue(const QString & moduleName, const QString & name, const QList<int> & indeces) const
{
    return QVariant();
}

void Python3LanguagePlugin::handlePythonOutput(const QString &message)
{
    Q_EMIT outputRequest(message);
}

void Python3LanguagePlugin::handlePythonError(const QString &message)
{
    Q_EMIT errorOutputRequest(message);
}

void Python3LanguagePlugin::handlePythonLineChanged(int lineNo)
{
    Q_EMIT lineChanged(lineNo);
}


} // namespace

Q_EXPORT_PLUGIN(Python3Language::Python3LanguagePlugin)
