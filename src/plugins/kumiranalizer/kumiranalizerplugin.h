#ifndef KUMIRANALIZERPLUGIN_H
#define KUMIRANALIZERPLUGIN_H

#include <kumir2-libs/extensionsystem/kplugin.h>
#include <kumir2/analizerinterface.h>
#include <kumir2-libs/extensionsystem/pluginspec.h>

#include <QtCore/QLocale>
#include <QtCore/QStringList>

namespace KumirAnalizer {

class Analizer;
class KumFileHandler;
class QuickReferenceWidget;

class KumirAnalizerPlugin
        : public ExtensionSystem::KPlugin
        , public Shared::AnalizerInterface

{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "kumir2.KumirAnalizer" FILE "")
#endif
    Q_INTERFACES(Shared::AnalizerInterface)
    friend class AnalizerPrivate;
public:
    Shared::Analizer::InstanceInterface * createInstance();
    Shared::Analizer::SourceFileInterface * sourceFileHandler();

    KumirAnalizerPlugin();
    ~KumirAnalizerPlugin();

    inline QList<ExtensionSystem::CommandLineParameter> acceptableCommandLineParameters() const {
        return QList<ExtensionSystem::CommandLineParameter>();
    }

    inline bool primaryAlphabetIsLatin() const { return false; }
    inline bool caseInsensitiveGrammatic() const { return false; }
    inline bool supportPartialCompiling() const { return true; }
    inline IndentsBehaviour indentsBehaviour() const { return HardIndents; }
    inline SyntaxHighlightBehaviour syntaxHighlightBehaviour() const { return IndependentLinesHighlight; }

    QWidget * languageQuickReferenceWidget();

    inline QString defaultDocumentFileNameSuffix() const { return "kum"; }
    inline QString languageName() const { return QString::fromUtf8("Кумир"); }    
    inline QByteArray asciiLanguageIdentifier() const { return "kumir"; }
    inline void updateSettings(const QStringList &) {}

protected:
    void createPluginSpec();
    QString initialize(const QStringList &configurationArguments,
                       const ExtensionSystem::CommandLine &runtimeArguments);
    void start();
    void stop();
private:
    QVector<Analizer*> analizers_;
    bool teacherMode_;
    KumFileHandler* kumFileHandler_;
    QuickReferenceWidget* _quickReferenceWidget;

};

}

#endif
