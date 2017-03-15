extern "C" {
#include <Python.h>
}


#include "analizerinstance.h"
#include "python3languageplugin.h"
#include "pyutils.h"
#include "tokenizerinstance.h"

namespace Python3Language {

PythonAnalizerInstance::PythonAnalizerInstance(Python3LanguagePlugin *parent,
                                               PyInterpreterProcess *interpreter)
    : QObject(parent)
    , _plugin(parent)
    , _py(interpreter)
    , _internalId(-1)
    , _tokenizer(new TokenizerInstance(this))
{
    QVariant id = _py->blockingCall("analyzer", "create", QVariantList());
    _internalId = id.toLongLong();
}

PythonAnalizerInstance::~PythonAnalizerInstance()
{
    if (_py) {
        _py->blockingCall("analyzer", "remote", QVariantList() << _internalId);
    }
}



Shared::AnalizerInterface * PythonAnalizerInstance::plugin()
{
    return _plugin;
}

void PythonAnalizerInstance::setSourceDirName(const QString &path)
{
    _py->blockingCall("analyzer", "set_source_dir_name",
                      QVariantList() << _internalId << path);
}

void PythonAnalizerInstance::setSourceText(const QString &plainText)
{
    _py->blockingCall("analyzer", "set_source_text",
                      QVariantList() << _internalId << plainText);

    queryErrors();
    _currentSourceText = plainText;
    _tokenizer->setSourceText(plainText);
}

std::string PythonAnalizerInstance::rawSourceData() const
{
    return std::string(_currentSourceText.toUtf8().constData());
}

QList<Error> PythonAnalizerInstance::errors() const
{
    return _errors;
}

void PythonAnalizerInstance::queryErrors() {

    const QVariant py_result =
            _py->blockingCall("analyzer", "get_errors",
                              QVariantList() << _internalId);


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
        if (!classDict.contains("id")) {
            printError(QString("Item %1 in 'get_errors' result do not have 'id' property").arg(i));
            return;
        }
        if (!classDict.contains("origin_name")) {
            printError(QString("Item %1 in 'get_errors' result do not have 'origin_name' property").arg(i));
            return;
        }
        Error error;
        error.line = classDict["line_no"].toInt();
        error.start = classDict["start_pos"].toInt();
        error.len = classDict["length"].toInt();
        error.message = classDict["message"].toString();
        error.msgid = classDict["id"].toString().toLatin1();
        error.origin = classDict["origin_name"].toString().toLatin1();
        // TODO translate error messages
        bool show_msg_id = true;
#if defined(NDEBUG) || defined(QT_NO_DEBUG)
        show_msg_id = false;
#endif
        if (show_msg_id && error.msgid.length() > 0) {
            error.message += " [" + error.msgid + "]";
        }
        if (error.origin.toLower().contains("pep-8") || error.origin.toLower().contains("pep8")) {
            error.message = "PEP-8: " + error.message;
        }
        result.append(error);
    }
    _errors = result;
}

QList<LineProp> PythonAnalizerInstance::lineProperties() const
{
    return _tokenizer->lineProperties();
}



QList<QPoint> PythonAnalizerInstance::lineRanks() const
{
    return _tokenizer->lineRanks();
}


LineProp PythonAnalizerInstance::lineProp(int lineNo, const QString &text) const
{
    return _tokenizer->lineProp(lineNo, text);
}

void PythonAnalizerInstance::setUsePep8(bool use)
{
    _py->blockingCall("analyzer", "set_use_pep8",
                      QVariantList() << use);
    setSourceText(_currentSourceText);  // Perform complete analisys again
    Q_EMIT internallyReanalized();
}

void PythonAnalizerInstance::connectUpdateRequest(QObject *receiver, const char *method)
{
    connect(this, SIGNAL(internallyReanalized()), receiver, method);
}

} // namespace Python3Language
