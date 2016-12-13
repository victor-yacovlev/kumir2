#include "kplugin.h"
#include "pluginmanager.h"

namespace ExtensionSystem {

KPlugin::KPlugin()
    : QObject()
{
}


QList<KPlugin*> KPlugin::loadedPlugins(const QString &pattern)
{
    return PluginManager::instance()->loadedPlugins(pattern);
}

QList<const KPlugin*> KPlugin::loadedPlugins(const QString &pattern) const
{
    const QList<const KPlugin*> result = PluginManager::instance()->loadedConstPlugins(pattern);
    return result;
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

QByteArray KPlugin::pluginName() const
{
    const QMetaObject * meta = metaObject();
    const QByteArray className = QByteArray(meta->className());
    int separatorPos = className.lastIndexOf("::");
    QByteArray result;
    if (separatorPos != -1) {
        result = className.left(separatorPos);
    }
    const QString message = QString::fromLatin1(
                "KPlugin can't build name based on class %1 namespace. "
                "You must explicitly implement virtual method 'pluginName()' "
                "in this class."
                ).arg(QString::fromLatin1(className));
    Q_ASSERT_X(result.length() > 0,
               "KPlugin::pluginName()",
               message.toLatin1().constData()
               );
    return result;
}

void KPlugin::initialize(const QString & resourcesRootPath)
{
    settings_ = SettingsPtr(new Settings(QString::fromLatin1(pluginName())));
    resourcesDir_ = resourcesRootPath;
    initialize(QStringList(), CommandLine());
}

SettingsPtr KPlugin::mySettings() const
{
    if (settings_)
        return settings_;
    else
        return PluginManager::instance()->settingsByObject(this);
}

QDir KPlugin::myResourcesDir() const
{
    QString thisModulePath;
    if (PluginManager::instance()->sharePath().isEmpty()) {
        thisModulePath = resourcesDir_;
    }
    else {
        const QString sharePath = PluginManager::instance()->sharePath();
        QString relPath = QString::fromLatin1(pluginName()).toLower();
        if (relPath.startsWith("actor")) {
            relPath = "actors/" + relPath.mid(5);
        }
        thisModulePath = QDir::cleanPath(sharePath + "/" + relPath);
    }
    Q_ASSERT(! thisModulePath.isEmpty());
    QDir result = QDir(thisModulePath);
    QString message = QString::fromLatin1("The directory not exists or not readable: %1")
            .arg(thisModulePath);
    Q_ASSERT_X(result.exists() && result.isReadable(), "KPlugin::myResourcesDir",
               message.toLocal8Bit().constData()
               );
    return result;
}

KPlugin::~KPlugin()
{

}

} // namespace ExtensionSystem
