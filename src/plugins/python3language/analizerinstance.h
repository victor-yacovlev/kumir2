#ifndef PYTHON3LANGUAGE_ANALIZERINSTANCE_H
#define PYTHON3LANGUAGE_ANALIZERINSTANCE_H

#include <QObject>
#include "pyinterpreterprocess.h"
#include "namecontext.h"
#include "interfaces/analizer_instanceinterface.h"

namespace Python3Language {

using namespace Shared::Analizer;

class Python3LanguagePlugin;
class TokenizerInstance;


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
    explicit PythonAnalizerInstance(Python3LanguagePlugin *parent,
                                    PyInterpreterProcess* interpreter);

    void queryErrors();
    void queryNamesExtract();

private /*fields*/:    
    Python3LanguagePlugin* _plugin;
    PyInterpreterProcess* _py;
    QString _currentSourceText;
    QList<Error> _errors;
    long long _internalId;
    NamesContext _globalNames;
    TokenizerInstance *_tokenizer;

};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_ANALIZERINSTANCE_H
