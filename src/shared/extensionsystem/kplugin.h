#ifndef KPLUGIN_H
#define KPLUGIN_H

#include <QtCore>
#include "pluginspec.h"
#include "settings.h"
#include "commandlineparameter.h"
#include "interfaces/plugininterface.h"

#ifdef EXTENSIONSYSTEM_LIBRARY
#define EXTENSIONSYSTEM_EXPORT Q_DECL_EXPORT
#else
#define EXTENSIONSYSTEM_EXPORT Q_DECL_IMPORT
#endif

class QWidget;
class QIcon;

namespace ExtensionSystem {

typedef Shared::PluginInterface::GlobalState GlobalState;

class EXTENSIONSYSTEM_EXPORT KPlugin
        : public QObject
        , public Shared::PluginInterface
{
    friend class PluginManager;
    friend struct PluginManagerImpl;
    Q_OBJECT
    Q_INTERFACES(Shared::PluginInterface)
public:
    virtual QByteArray pluginName() const;
    enum State { Disabled = 0x00, Loaded=0x01, Initialized=0x02, Started=0x03, Stopped=0x04 };
    KPlugin();
    PluginSpec pluginSpec() const;
    State state() const;
    inline virtual bool isGuiRequired() const { return pluginSpec().gui; } // Can be overridden in special cases
    inline virtual QWidget* settingsEditorPage() { return 0; }

    inline static SettingsPtr pluginSettings() { return self->mySettings(); }
    SettingsPtr mySettings() const;

    inline static QDir pluginResourcesDir() { return self->myResourcesDir(); }
    QDir myResourcesDir() const;

    virtual ~KPlugin();

protected:
    inline virtual void saveSession() const { }
    inline virtual void restoreSession() { }
    inline virtual void changeCurrentDirectory(const QString & path) { Q_UNUSED(path); }
    inline virtual void changeGlobalState(GlobalState old, GlobalState current) { Q_UNUSED(old); Q_UNUSED(current); }
    inline virtual QList<CommandLineParameter> acceptableCommandLineParameters() const
    {
        return QList<CommandLineParameter>();
    }
    void initialize(const QString & pluginResourcesRootPath);
    virtual QString initialize(
            const QStringList & /*configurationArguments*/,
            const CommandLine & /*runtimeArguments*/
            ) = 0;

    inline virtual bool isSafeToQuit() { return true; }
    inline virtual void start() {}
    inline virtual void stop() {}
    virtual void updateSettings(const QStringList & keys) = 0;
    KPlugin * myDependency(const QString & name) const;    

    QList<KPlugin*> loadedPlugins(const QString &pattern = "*");
    QList<const KPlugin*> loadedPlugins(const QString &pattern = "*") const;

private:
    SettingsPtr settings_;
    QString resourcesDir_;
    static KPlugin* self;

};

} // namespace ExtensionSystem

#endif // KPLUGIN_H
