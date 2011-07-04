#ifndef KPLUGIN_H
#define KPLUGIN_H

#include <QtCore>
#include "pluginspec.h"

#ifdef EXTENSIONSYSTEM_LIBRARY
#define EXTENSIONSYSTEM_EXPORT Q_DECL_EXPORT
#else
#define EXTENSIONSYSTEM_EXPORT Q_DECL_IMPORT
#endif

class QWidget;
class QIcon;

namespace ExtensionSystem {

struct SettingsEditorPage {
    inline SettingsEditorPage() { settingsPage = 0; settingsGroupIcon = 0; }
    QWidget * settingsPage;
    QString settingsGroupName;
    QIcon * settingsGroupIcon;
};

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
    friend class PluginManagerPrivate;
    Q_OBJECT
public:
    enum State { Disabled, Loaded, Initialized, Started, Stopped };
    KPlugin();
    PluginSpec pluginSpec() const;
    State state() const;
    inline virtual SettingsEditorPage settingsEditorPage() { return SettingsEditorPage(); }
    virtual ~KPlugin();
protected:
    inline virtual QByteArray saveSession() const { return QByteArray(); }
    inline virtual void restoreSession(const QByteArray &) { }
    inline virtual void changeCurrentDirectory(const QString & path) { Q_UNUSED(path); }
    inline virtual void changeGlobalState(GlobalState old, GlobalState current) { Q_UNUSED(old); Q_UNUSED(current); }
    inline virtual QString initialize(const QStringList &arguments) { Q_UNUSED(arguments); return ""; }
    inline virtual void start() {}
    inline virtual void stop() {}
    KPlugin * myDependency(const QString & name) const;

    QSettings * mySettings() const;

    QList<KPlugin*> loadedPlugins(const QString &pattern = "*");

};

} // namespace ExtensionSystem

#endif // KPLUGIN_H
