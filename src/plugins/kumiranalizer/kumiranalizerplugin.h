#ifndef KUMIRANALIZERPLUGIN_H
#define KUMIRANALIZERPLUGIN_H

#include <extensionsystem/kplugin.h>
#include <interfaces/analizerinterface.h>

#include <QtCore/QLocale>
#include <QtCore/QStringList>

namespace KumirAnalizer {

class Analizer;

class KumirAnalizerPlugin
        : public ExtensionSystem::KPlugin
        , public Shared::AnalizerInterface

{
    Q_OBJECT
    Q_INTERFACES(Shared::AnalizerInterface)
    friend struct AnalizerPrivate;
public:
    Shared::Analizer::InstanceInterface * createInstance();

    KumirAnalizerPlugin();
    ~KumirAnalizerPlugin();

    inline QList<ExtensionSystem::CommandLineParameter> acceptableCommandLineParameters() const {
        return QList<ExtensionSystem::CommandLineParameter>();
    }

    inline bool primaryAlphabetIsLatin() const { return false; }
    inline bool caseInsensitiveGrammatic() const { return false; }
    inline bool supportPartialCompiling() const { return true; }
    inline bool indentsSignificant() const { return false; }

    inline QString defaultDocumentFileNameSuffix() const { return "kum"; }
    inline QString languageName() const { return QString::fromUtf8("Кумир"); }    
    inline void updateSettings(const QStringList &) {}

protected:
    QString initialize(const QStringList &configurationArguments,
                       const ExtensionSystem::CommandLine &runtimeArguments);
    void start();
    void stop();
private:
    QVector<Analizer*> analizers_;
    bool teacherMode_;
};

}

#endif
