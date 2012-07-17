#include "python3languageplugin.h"
#include "python3.2mu/Python.h"

namespace Python3Language {

struct Python3Module
{
    QString text;
};

Python3LanguagePlugin::Python3LanguagePlugin()
{
    m_documents.fill(0,128);
    py_analizer = 0;
}

QString Python3LanguagePlugin::initialize(const QStringList &)
{
    static const QString pyLibPath = qApp->property("sharePath").toString()+"/python3language/";
    static const QByteArray analizer_py_Path = pyLibPath.toLocal8Bit();
    char * analizer_py = (char*)calloc(analizer_py_Path.size(), sizeof(char));
    strcpy(analizer_py, analizer_py_Path.constData());

    Py_Initialize();
    ::PyObject * sysPath = PySys_GetObject("path");
    ::PyObject * kumirPath = PyUnicode_FromString(analizer_py);
    PyList_Insert(sysPath, 0, kumirPath);
    PyObject_Print(sysPath, stderr, 0);
    py_analizer = PyImport_ImportModule("analizer");
//    ::PyObject * ptype, *pvalue, *ptraceback;
//    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
//    PyObject_Print(pvalue, stderr,0);
    return "";
}

int Python3LanguagePlugin::newDocument()
{
    int index = m_documents.indexOf(0);
    m_documents[index] = new Python3Module;
    return index;
}

void Python3LanguagePlugin::setSourceText(int documentId, const QString &text)
{
    m_documents[documentId]->text = text;
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
    return QList<Error>();
}

QList<LineProp> Python3LanguagePlugin::lineProperties(int documentId) const
{
    return QList<LineProp>();
}

QList<QPoint> Python3LanguagePlugin::lineRanks(int documentId) const
{
    return QList<QPoint>();
}

QStringList Python3LanguagePlugin::imports(int documentId) const
{
    return QStringList();
}

LineProp Python3LanguagePlugin::lineProp(int documentId, const QString &text) const
{
    ::PyObject * py_line_prop = PyObject_GetAttrString(py_analizer, "line_prop");
    ::PyObject * args = PyTuple_New(1);
    std::wstring ws = text.toStdWString();
    ::PyObject * txt = PyUnicode_FromWideChar(ws.c_str(), ws.length());
    PyTuple_SetItem(args, 0, txt);
    ::PyObject *pRes = PyObject_CallObject(py_line_prop, args);
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
    Py_DECREF(args);
    LineProp lp(text.size());
    lp.fill(LxTypeEmpty);
    int sz = PyList_Size(pRes);
    for (int i=0; i<sz; i++) {
        ::PyObject * item = PyList_GetItem(pRes, i);
        unsigned long val = PyLong_AsUnsignedLong(item);
        lp[i] = (Shared::LexemType)val;
//        qDebug() << val;
    }
    return lp;
}

std::string Python3LanguagePlugin::rawSourceData(int documentId) const
{
    if (m_documents[documentId])
        return m_documents[documentId]->text.toStdString();
    else
        return "";
}

QStringList Python3LanguagePlugin::algorhitmsAvailableFor(int documentId, int lineNo) const
{
    return QStringList();
}

QStringList Python3LanguagePlugin::globalsAvailableFor(int documentId, int lineNo) const
{
    return QStringList();
}

QStringList Python3LanguagePlugin::localsAvailableFor(int documentId, int lineNo) const
{
    return QStringList();
}


void Python3LanguagePlugin::dropDocument(int documentId)
{
    if (m_documents[documentId]) {
        delete m_documents[documentId];
        m_documents[documentId] = 0;
    }
}

bool Python3LanguagePlugin::loadProgram(QIODevice *source, ProgramFormat format)
{
    return false;
}

QDateTime Python3LanguagePlugin::loadedProgramVersion() const
{
    return QDateTime::currentDateTime();
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

QVariantList Python3LanguagePlugin::remainingValues() const
{
    return QVariantList();
}

QVariant Python3LanguagePlugin::value(int moduleId, int algorhitmId, int variableId) const
{
    return QVariant();
}

QList<int> Python3LanguagePlugin::bounds(int moduleId, int algorhitmId, int variableId) const
{
    return QList<int>();
}

QList<int> Python3LanguagePlugin::reference(int moduleId, int algorhitmId, int variableId) const
{
    return QList<int>();
}

void Python3LanguagePlugin::finishInput(const QVariantList & message)
{

}

void Python3LanguagePlugin::finishExternalFunctionCall(
    const QString & error,
    const QVariant & retval,
    const QVariantList & results)
{

}

} // namespace

Q_EXPORT_PLUGIN(Python3Language::Python3LanguagePlugin)
