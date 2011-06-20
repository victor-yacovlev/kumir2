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

class EXTENSIONSYSTEM_EXPORT KPlugin : public QObject
{
    friend class PluginManager;
    Q_OBJECT
public:
    enum State { Disabled, Loaded, Initialized, Started, Stopped };
    KPlugin();
    PluginSpec pluginSpec() const;
    State state() const;
    inline virtual SettingsEditorPage settingsEditorPage() { return SettingsEditorPage(); }
    virtual ~KPlugin();
protected:
    inline virtual void changeCurrentDirectory(const QString & path) { Q_UNUSED(path); }
    inline virtual QString initialize(const QStringList &arguments) { Q_UNUSED(arguments); return ""; }
    inline virtual void start() {}
    inline virtual void stop() {}
    KPlugin * myDependency(const QString & name) const;

    QSettings * mySettings() const;

    QList<KPlugin*> loadedPlugins(const QString &pattern = "*");

};

} // namespace ExtensionSystem

#endif // KPLUGIN_H
