#include "analizerinstance.h"
#include "python3languageplugin.h"
#include "pyutils.h"

namespace Python3Language {

PythonAnalizerInstance::PythonAnalizerInstance(Python3LanguagePlugin *parent,
                                               const QString & extraPythonPath)
    : QObject(parent)
    , plugin_(parent)
    , py_(0)
    , py_analizerInstance(0)
    , py_setSourceDirName(0)
    , py_setSourceText(0)
    , py_getErrors(0)
    , py_getLineProperties(0)
    , py_getLineRanks(0)
    , py_getLineProperty(0)
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

    if (!py_setSourceDirName) { printError("'set_source_dir_name' not implemented"); }
    py_setSourceText = ::PyObject_GetAttrString(
                py_analizerInstance, "set_source_text"
                );
    if (!py_setSourceText) { printError("'set_source_text' not implemented"); }
    py_getErrors = ::PyObject_GetAttrString(
                py_analizerInstance, "get_errors"
                );
    if (!py_getErrors) { printError("'get_errors' not implemented"); }
    py_getLineProperties = ::PyObject_GetAttrString(
                py_analizerInstance, "get_line_properties"
                );
    if (!py_getLineProperties) { printError("'get_line_properties' not implemented"); }
    py_getLineRanks = ::PyObject_GetAttrString(
                py_analizerInstance, "get_line_ranks"
                );
    if (!py_getLineRanks) { printError("'get_line_ranks' not implemented"); }
    py_getLineProperty = ::PyObject_GetAttrString(
                py_analizerInstance, "get_line_property"
                );
    if (!py_getLineProperty) { printError("'get_line_property' not implemented"); }
}

Shared::AnalizerInterface * PythonAnalizerInstance::plugin()
{
    return plugin_;
}

void PythonAnalizerInstance::setSourceDirName(const QString &path)
{
    if (!py_) { printError("interpreter not initialized"); return; }
    if (!py_setSourceDirName) { printError("'set_source_dir_name' not loaded"); return; }
    callPythonFunction(py_, py_setSourceDirName, path);
}

void PythonAnalizerInstance::setSourceText(const QString &plainText)
{
    if (!py_) { printError("interpreter not initialized"); return; }

    if (!py_setSourceText) { printError("'set_source_text' not loaded"); return; }
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
    if (!py_getErrors) { printError("'get_errors' not loaded"); return; }
    const QVariant py_result = callPythonFunction(py_, py_getErrors);    
    if (py_result.type() != QVariant::List) {
        printError("Result type of 'get_errors' is not a list");
        return;
    }
    const QVariantList alist = py_result.toList();
    QList<Error> result;
    for (int i=0; i<alist.size(); i++) {
        const QVariant & item = alist[i];
        if (item.type() != QVariant::Map) {
            printError(QString("Item %1 in 'get_errors' result is not a 'Error' class instance").arg(i));
            return;
        }
        const QMap<QString,QVariant> classDict = item.toMap();
        if (!classDict.contains("line_no")) {
            printError(QString("Item %1 in 'get_errors' result do not have 'line_no' property").arg(i));
            return;
        }
        if (!classDict.contains("start_pos")) {
            printError(QString("Item %1 in 'get_errors' result do not have 'start_pos' property").arg(i));
            return;
        }
        if (!classDict.contains("length")) {
            printError(QString("Item %1 in 'get_errors' result do not have 'length' property").arg(i));
            return;
        }
        if (!classDict.contains("message")) {
            printError(QString("Item %1 in 'get_errors' result do not have 'message' property").arg(i));
            return;
        }
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
    if (!py_getLineProperties) { printError("'get_line_properties' not loaded"); return; }
    const QVariant py_result = callPythonFunction(py_, py_getLineProperties);
    if (py_result.type() != QVariant::List) {
        printError("'get_line_properties' result is not a list");
        return;
    }
    const QVariantList alist = py_result.toList();
    QList<LineProp> result;
    QStringList sourceLines = currentSourceText_.split('\n');
    for (int z=0; z<alist.size(); z++) {
        const QVariant & item = alist[z];
        if (item.type() != QVariant::List) {
            printError(QString("Item %1 in 'get_line_properties' result is not a list of integers:\n%2").arg(z).arg(item.toString()));
            return;
        }
        const QVariantList py_prop = item.toList();
        LineProp prop(py_prop.size());
        for (int i=0; i<py_prop.size(); i++) {
            prop[i] = (LexemType) py_prop[i].toUInt();
        }
        if (prop.isEmpty()) {
            // in case if not implemented in Python-side
            prop.resize(sourceLines[alist.indexOf(item)].length());
            prop.fill(LxTypeEmpty, prop.size());
        }
        result.append(prop);
    }
    if (result.isEmpty()) {
        // in case if not implemented in Python-side
        Q_FOREACH(const QString & line, sourceLines) {
            LineProp fake(line.length());
            fake.fill(LxTypeEmpty, line.length());
            result.append(fake);
        }
    }
    lineProperties_ = result;
}

QList<QPoint> PythonAnalizerInstance::lineRanks() const
{
    return lineRanks_;
}

void PythonAnalizerInstance::queryLineRanks() {
    if (!py_getLineProperties) { printError("'get_line_ranks' not loaded"); return; }
    const QVariant py_result = callPythonFunction(py_, py_getLineRanks);
    if (py_result.type() != QVariant::List) {
        printError("'get_line_ranks' result is not a list");
        return;
    }
    const QVariantList alist = py_result.toList();
    QList<QPoint> result;
    for (int z=0; z<alist.size(); z++) {
        const QVariant & item = alist[z];
        if (item.type() != QVariant::List) {
            printError(QString("Item %1 in 'get_line_ranks' result is not a pair of two integers:\n%2").arg(z).arg(item.toString()));
            return;
        }
        const QVariantList point = item.toList();
        result.append(QPoint(point.at(0).toInt(), point.at(1).toInt()));
    }
    lineRanks_ = result;
}

LineProp PythonAnalizerInstance::lineProp(int lineNo, const QString &text) const
{
    LineProp fake(text.length());
    fake.fill((LexemType)0);
    return fake;
    if (!py_) {
        return fake;
    }
    if (!py_getLineProperties) { printError("'get_line_property' not loaded"); return fake; }
    const QVariant py_result = callPythonFunction(py_, py_getLineProperty, lineNo, text);
    if (py_result.type() != QVariant::List) {
        printError("'get_line_property' result is not a list of numbers");
        return fake;
    }
    const QVariantList alist = py_result.toList();
    LineProp result(alist.size());
    for (int i=0; i<alist.size(); i++) {
        result[i] = (LexemType) alist.at(i).toUInt();
    }
    return result;
}

} // namespace Python3Language
