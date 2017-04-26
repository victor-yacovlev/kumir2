#ifndef EXTENSIONSYSTEM_PLUGINMANAGER_IMPL_H
#define EXTENSIONSYSTEM_PLUGINMANAGER_IMPL_H

#include "pluginmanager.h"

#include <QString>
#include <QStringList>
#include <QFont>

namespace ExtensionSystem {


struct PluginManagerImpl {
    QList<KPlugin*> objects;
    QString path;
    QString sharePath;

    QByteArray mainPluginName;
    GlobalState globalState;

    SettingsPtr mySettings;
    QString settingsWorkspacePath;


    QString parsePluginsRequest(const QByteArray &templ, QList<PluginSpec> & plugins);
    QString loadSpecs(const QStringList &names/*, QScriptEngine * engine*/);
    QString makeDependencies(const QString &entryPoint,
                             QStringList &orderedList);
    QString reorderSpecsAndCreateStates(const QStringList & orderedList);
    void createSettingsDialog();
    QString loadPlugin(PluginSpec spec, const QList<PluginSpec> & allSpecs);
    QString initializePlugin(KPlugin * plugin);
    bool isPluginLoaded(const QByteArray & name) const;
    void changeWorkingDirectory(const QString &path, bool saveChanges, bool workDirOnly);
    bool extractRuntimeParametersForPlugin(const KPlugin * plugin, CommandLine & parameters);

    std::list<QString> namedProgramArguments;
    std::list<QString> unnamedProgramArguments;

    QFont initialApplicationFont;
    QStringList additionalPluginPrefixes;
};

} // namespace ExtensionSystem

#endif // EXTENSIONSYSTEM_PLUGINMANAGER_IMPL_H
