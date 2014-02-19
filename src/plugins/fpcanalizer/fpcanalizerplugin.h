#ifndef FPC_ANALIZER_PLUGIN_H
#define FPC_ANALIZER_PLUGIN_H

#include "extensionsystem/kplugin.h"
#include "interfaces/analizerinterface.h"
#include "interfaces/analizer_instanceinterface.h"


namespace FpcAnalizer {

using namespace ExtensionSystem;
using namespace Shared;

class FpcAnalizerPlugin
        : public KPlugin
        , public AnalizerInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::AnalizerInterface)
public:
    QString initialize(const QStringList &, const CommandLine &);
    void updateSettings(const QStringList &keys);
    inline bool primaryAlphabetIsLatin() const { return true; }
    inline bool caseInsensitiveGrammatic() const { return true; }
    inline bool indentsSignificant() const { return true; }
    inline QString languageName() const { return "Pascal"; }
    inline QString defaultDocumentFileNameSuffix() const { return "pas"; }
    Analizer::InstanceInterface* createInstance();
    QMap<QString,QString> readCapitalizationHints() const;
    static FpcAnalizerPlugin* self();
    inline QDir resoursesDir() { return myResourcesDir(); }
private:
    uint lastInstanceIndex_;
    static FpcAnalizerPlugin* me_;
};


}

#endif
