#include "python3languageplugin.h"
#include "Python.h"

namespace Python3Language {

Python3LanguagePlugin::Python3LanguagePlugin()
    : ExtensionSystem::KPlugin()
    , analizerModule_(NULL)
{

}

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

static ::PyMethodDef _kumirMethods[] = {
    { "debug", _kumir_debug, METH_VARARGS, "Debug via qDebug routines"},
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
    PyImport_AppendInittab("__builtins__", &PyEval_GetBuiltins);
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
        return "Can't import python module 'analizer'";
    }

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

bool Python3LanguagePlugin::loadProgram(const QString &fileName, const QByteArray & source, ProgramFormat format)
{
    return false;
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

}

void Python3LanguagePlugin::runContinuous()
{

}

void Python3LanguagePlugin::runStepOver()
{

}

void Python3LanguagePlugin::runStepInto()
{

}

void Python3LanguagePlugin::runStepOut()
{

}

void Python3LanguagePlugin::runTesting()
{

}

void Python3LanguagePlugin::terminate()
{

}

bool Python3LanguagePlugin::hasMoreInstructions() const
{
    return false;
}

int Python3LanguagePlugin::currentLineNo() const
{
    return 0;
}

QString Python3LanguagePlugin::error() const
{
    return "";
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




} // namespace

Q_EXPORT_PLUGIN(Python3Language::Python3LanguagePlugin)
