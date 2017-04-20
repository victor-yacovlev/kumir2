#ifndef KUMIRCODEGENERATORPLUGIN_H
#define KUMIRCODEGENERATORPLUGIN_H

#include <kumir2-libs/extensionsystem/kplugin.h>
#include <kumir2/generatorinterface.h>

#include <QObject>
#include <QtPlugin>

using namespace Shared;

namespace KumirCodeGenerator {

class KumirCodeGeneratorPlugin
        : public ExtensionSystem::KPlugin
        , public GeneratorInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "kumir2.KumirCodeGenerator" FILE "")
#endif
    Q_INTERFACES(Shared::GeneratorInterface)

public:
    KumirCodeGeneratorPlugin();

    QList<ExtensionSystem::CommandLineParameter> acceptableCommandLineParameters() const;

    void setDebugLevel(DebugLevel debugLevel);
    void generateExecutable(
                const AST::DataPtr tree,
                QByteArray & out,
                QString & mimeType,
                QString & fileSuffix
                );

    void setOutputToText(bool flag);
    inline void setVerbose(bool) {}
    inline void setTemporaryDir(const QString &, bool ) {}
    inline void updateSettings(const QStringList &) {}

protected:
    QString initialize(const QStringList &configurationArguments,
                       const ExtensionSystem::CommandLine &runtimeArguments);
    void start();
    void stop();
private:
    class Generator * d;
    bool textMode_;



};

}

#endif // KUMIRCODEGENERATORPLUGIN_H
