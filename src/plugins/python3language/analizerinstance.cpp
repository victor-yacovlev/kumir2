#include "analizerinstance.h"
#include "python3languageplugin.h"
#include "pyutils.h"

namespace Python3Language {

PythonAnalizerInstance::PythonAnalizerInstance(Python3LanguagePlugin *parent,
                                               const QString & extraPythonPath)
    : QObject(parent)
    , plugin_(parent)
    , py_(0)
{ 
#ifdef Q_OS_WIN32
    if (_Py_atomic_load_relaxed(&_PyThreadState_Current))
#else
    if (PyThreadState_GET())
#endif
        ::PyEval_AcquireLock();
    else
        ::PyGILState_Ensure();
    py_ = ::Py_NewInterpreter();
#ifdef Q_OS_WIN32
    Q_UNUSED(extraPythonPath);
    prepareBundledSysPath();
#else
    appendToSysPath(extraPythonPath);
#endif
    initializePyAnalizer();
    ::PyEval_ReleaseThread(py_);
}

PythonAnalizerInstance::~PythonAnalizerInstance()
{
    stopPythonInterpreter();
}

void PythonAnalizerInstance::stopPythonInterpreter()
{
    if (py_) {
        ::PyEval_AcquireThread(py_);
        ::Py_EndInterpreter(py_);
        ::PyEval_ReleaseLock();
    }
    py_ = 0;
}

void PythonAnalizerInstance::initializePyAnalizer()
{
    py_analizerInstance = ::PyImport_ImportModule("analizer_instance");
    if (!py_analizerInstance) {
        printPythonTraceback();
        return;
    }
    py_setSourceDirName = ::PyObject_GetAttrString(
                py_analizerInstance, "set_source_dir_name"
                );
    if (!py_setSourceDirName) printPythonTraceback();
    py_setSourceText = ::PyObject_GetAttrString(
                py_analizerInstance, "set_source_text"
                );
    if (!py_setSourceText) printPythonTraceback();
    py_getErrors = ::PyObject_GetAttrString(
                py_analizerInstance, "get_errors"
                );
    if (!py_getErrors) printPythonTraceback();
    py_getLineProperties = ::PyObject_GetAttrString(
                py_analizerInstance, "get_line_properties"
                );
    if (!py_getLineProperties) printPythonTraceback();
    py_getLineRanks = ::PyObject_GetAttrString(
                py_analizerInstance, "get_line_ranks"
                );
    if (!py_getLineRanks) printPythonTraceback();
    py_getLineProperty = ::PyObject_GetAttrString(
                py_analizerInstance, "get_line_property"
                );
    if (!py_getLineProperty) printPythonTraceback();
}

Shared::AnalizerInterface * PythonAnalizerInstance::plugin()
{
    return plugin_;
}

void PythonAnalizerInstance::setSourceDirName(const QString &path)
{
    if (!py_) return;
    Q_ASSERT(py_setSourceDirName);
    callPythonFunction(py_, py_setSourceDirName, path);
}

void PythonAnalizerInstance::setSourceText(const QString &plainText)
{
    if (!py_) return;

    Q_ASSERT(py_setSourceText);
    currentSourceText_ = plainText;
    callPythonFunction(py_, py_setSourceText, plainText);

    queryErrors();
    queryLineProperties();
    queryLineRanks();
}

std::string PythonAnalizerInstance::rawSourceData() const
{
    return std::string(currentSourceText_.toUtf8().constData());
}

QList<Error> PythonAnalizerInstance::errors() const
{
    return errors_;
}

void PythonAnalizerInstance::queryErrors() {
    Q_ASSERT(py_getErrors);
    const QVariant py_result = callPythonFunction(py_, py_getErrors);
    Q_ASSERT(py_result.type() == QVariant::List);
    const QVariantList alist = py_result.toList();
    QList<Error> result;
    Q_FOREACH(const QVariant & item, alist) {
        Q_ASSERT(item.type() == QVariant::Map);
        const QMap<QString,QVariant> classDict = item.toMap();
        Q_ASSERT(classDict.contains("line_no"));
        Q_ASSERT(classDict.contains("start_pos"));
        Q_ASSERT(classDict.contains("length"));
        Q_ASSERT(classDict.contains("message"));
        Error error;
        error.line = classDict["line_no"].toInt();
        error.start = classDict["start_pos"].toInt();
        error.len = classDict["length"].toInt();
        error.code = classDict["message"].toString();
        result.append(error);
    }
    errors_ = result;
}

QList<LineProp> PythonAnalizerInstance::lineProperties() const
{
    return lineProperties_;
}

void PythonAnalizerInstance::queryLineProperties() {
    Q_ASSERT(py_getLineProperties);
    const QVariant py_result = callPythonFunction(py_, py_getLineProperties);
    Q_ASSERT(py_result.type() == QVariant::List);
    const QVariantList alist = py_result.toList();
    QList<LineProp> result;
    Q_FOREACH(const QVariant & item, alist) {
        Q_ASSERT(item.type() == QVariant::List);
        const QVariantList py_prop = item.toList();
        LineProp prop(py_prop.size());
        for (int i=0; i<py_prop.size(); i++) {
            prop[i] = (LexemType) py_prop[i].toUInt();
        }
        result.append(prop);
    }
    lineProperties_ = result;
}

QList<QPoint> PythonAnalizerInstance::lineRanks() const
{
    return lineRanks_;
}

void PythonAnalizerInstance::queryLineRanks() {
    Q_ASSERT(py_getLineRanks);
    const QVariant py_result = callPythonFunction(py_, py_getLineRanks);
    Q_ASSERT(py_result.type() == QVariant::List);
    const QVariantList alist = py_result.toList();
    QList<QPoint> result;
    Q_FOREACH(const QVariant & item, alist) {
        Q_ASSERT(item.type() == QVariant::List);
        const QVariantList point = item.toList();
        result.append(QPoint(point.at(0).toInt(), point.at(1).toInt()));
    }
    lineRanks_ = result;
}

LineProp PythonAnalizerInstance::lineProp(int lineNo, const QString &text) const
{
    if (!py_) {
        LineProp fake(text.length());
        fake.fill((LexemType)0);
        return fake;
    }
    Q_ASSERT(py_getLineProperty);
    const QVariant py_result = callPythonFunction(py_, py_getLineProperty, lineNo, text);
    Q_ASSERT(py_result.type() == QVariant::List);
    const QVariantList alist = py_result.toList();
    LineProp result(alist.size());
    for (int i=0; i<alist.size(); i++) {
        result[i] = (LexemType) alist.at(i).toUInt();
    }
    return result;
}

} // namespace Python3Language
