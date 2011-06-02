#include "kplugin.h"
#include "pluginmanager.h"

namespace ExtensionSystem {

KPlugin::KPlugin()
    : QObject()
{
}

QList<KPlugin*> KPlugin::loadedPlugins(const QRegExp &rx)
{
    return PluginManager::instance()->loadedPlugins(rx);
}

PluginSpec KPlugin::pluginSpec() const
{
    return PluginManager::instance()->specByObject(this);
}

KPlugin::State KPlugin::state() const
{
    return PluginManager::instance()->stateByObject(this);
}

KPlugin * KPlugin::myDependency(const QString &name) const
{
    return PluginManager::instance()->dependentPlugin(name, this);
}

QSettings * KPlugin::mySettings() const
{
    return PluginManager::instance()->settingsByObject(this);
}

KPlugin::~KPlugin()
{

}

} // namespace ExtensionSystem
