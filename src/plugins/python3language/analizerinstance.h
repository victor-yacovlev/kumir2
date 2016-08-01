#ifndef PYTHON3LANGUAGE_ANALIZERINSTANCE_H
#define PYTHON3LANGUAGE_ANALIZERINSTANCE_H


#include <QObject>
extern "C" {
#include <Python.h>
}

#include "interfaces/analizer_instanceinterface.h"

namespace Python3Language {

using namespace Shared::Analizer;

class Python3LanguagePlugin;

class PythonAnalizerInstance
        : public QObject
        , public InstanceInterface
{
    friend class Python3LanguagePlugin;
    Q_OBJECT
    Q_INTERFACES(Shared::Analizer::InstanceInterface)
public:
    Shared::AnalizerInterface * plugin();
    ~PythonAnalizerInstance();

    void setSourceDirName(const QString &path);
    void setSourceText(const QString &plainText);
    std::string rawSourceData() const;
    QList<Error> errors() const;
    QList<LineProp> lineProperties() const;
    QList<QPoint> lineRanks() const;
    LineProp lineProp(int lineNo, const QString &text) const;
    void setUsePep8(bool use);
    void connectUpdateRequest(QObject * receiver, const char * method);

Q_SIGNALS:
    void internallyReanalized();

protected /*methods*/:
    void stopPythonInterpreter();
    explicit PythonAnalizerInstance(Python3LanguagePlugin *parent,
                                    const QStringList & extraPythonPaths);
    void initializePyAnalizer(const QStringList & extraPythonPaths);

    void queryErrors();
    void queryLineProperties();
    void queryLineRanks();

private /*fields*/:
    Python3LanguagePlugin* plugin_;
    QString currentSourceText_;
    ::PyThreadState* py_;
    ::PyObject* py_analizerInstance;
    ::PyObject* py_setSourceDirName;
    ::PyObject* py_setSourceText;
    ::PyObject* py_getErrors;
    ::PyObject* py_getLineProperties;
    ::PyObject* py_getLineRanks;
    ::PyObject* py_getLineProperty;
    ::PyObject* py_setUsePep8;
    QList<Error> errors_;
    QList<LineProp> lineProperties_;
    QList<QPoint> lineRanks_;

};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_ANALIZERINSTANCE_H
