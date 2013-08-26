#ifndef KPLUGIN_H
#define KPLUGIN_H

#include <QtCore>
#include "pluginspec.h"
#include "settings.h"
#include "commandlineparameter.h"

#ifdef EXTENSIONSYSTEM_LIBRARY
#define EXTENSIONSYSTEM_EXPORT Q_DECL_EXPORT
#else
#define EXTENSIONSYSTEM_EXPORT Q_DECL_IMPORT
#endif

class QWidget;
class QIcon;

namespace ExtensionSystem {

enum GlobalState {
    GS_Unlocked, // Edit mode
    GS_Observe, // Observe mode
    GS_Running, // Running mode
    GS_Input,  // User input required
    GS_Pause  // Running paused
};

class EXTENSIONSYSTEM_EXPORT KPlugin : public QObject
{
    friend class PluginManager;
    friend struct PluginManagerImpl;
    Q_OBJECT
public:
    enum State { Disabled, Loaded, Initialized, Started, Stopped };
    KPlugin();
    PluginSpec pluginSpec() const;
    State state() const;
    inline virtual bool isGuiRequired() const { return pluginSpec().gui; } // Can be overridden in special cases
    inline virtual QWidget* settingsEditorPage() { return 0; }
    SettingsPtr pluginSettings() const { return mySettings(); }
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

    inline virtual QString initialize(
            const QStringList & /*configurationArguments*/,
            const CommandLine & /*runtimeArguments*/
            ) = 0;

    inline virtual void start() {}
    inline virtual void stop() {}
    inline virtual void updateSettings() { }
    KPlugin * myDependency(const QString & name) const;

    SettingsPtr mySettings() const;

    QList<KPlugin*> loadedPlugins(const QString &pattern = "*");
    QList<const KPlugin*> loadedPlugins(const QString &pattern = "*") const;

};

} // namespace ExtensionSystem

#endif // KPLUGIN_H
