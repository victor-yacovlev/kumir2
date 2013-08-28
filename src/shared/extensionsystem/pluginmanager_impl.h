#ifndef EXTENSIONSYSTEM_PLUGINMANAGER_IMPL_H
#define EXTENSIONSYSTEM_PLUGINMANAGER_IMPL_H

#include "extensionsystem/pluginmanager.h"

#include <QString>
#include <QStringList>

namespace ExtensionSystem {

struct PluginRequest {
    QString name;
    bool start;
    QStringList arguments;
};

struct PluginManagerImpl {
    QList<KPlugin*> objects;
    QString path;
    QString sharePath;
    QList<PluginSpec> specs;
    QList<KPlugin::State> states;
    QList<SettingsPtr> settings;
    QList<PluginRequest> requests;
    QString mainPluginName;
    GlobalState globalState;

    SettingsPtr mySettings;
    QString workspacePath;


    QString parsePluginsRequest(const QString &templ, QList<PluginRequest> & plugins, QStringList & names);
    QString loadSpecs(const QStringList &names/*, QScriptEngine * engine*/);
    QString makeDependencies(const QString &entryPoint,
                             QStringList &orderedList);
    QString reorderSpecsAndCreateStates(const QStringList & orderedList);
    void createSettingsDialog();
    QString loadPlugins();
    void changeWorkingDirectory(const QString &path, bool saveChanges);
    bool extractRuntimeParametersForPlugin(const KPlugin * plugin, CommandLine & parameters);

    std::list<QString> namedProgramArguments;
    std::list<QString> unnamedProgramArguments;
};

} // namespace ExtensionSystem

#endif // EXTENSIONSYSTEM_PLUGINMANAGER_IMPL_H
