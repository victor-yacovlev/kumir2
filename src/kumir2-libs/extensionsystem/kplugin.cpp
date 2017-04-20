#include "kplugin.h"
#include "pluginmanager.h"

namespace ExtensionSystem {

KPlugin::KPlugin()
    : QObject()
{
}


QList<KPlugin*> KPlugin::loadedPlugins(const QByteArray &pattern)
{
    return PluginManager::instance()->loadedPlugins(pattern);
}

QList<const KPlugin*> KPlugin::loadedPlugins(const QByteArray &pattern) const
{
    const QList<const KPlugin*> result = PluginManager::instance()->loadedConstPlugins(pattern);
    return result;
}


const PluginSpec & KPlugin::pluginSpec() const
{
    return _pluginSpec;
}

KPlugin::State KPlugin::state() const
{
    return _state;
}

KPlugin * KPlugin::myDependency(const QByteArray &name) const
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
    _settings = SettingsPtr(new Settings(QString::fromLatin1(pluginName())));
    _resourcesDir = resourcesRootPath;
    initialize(QStringList(), CommandLine());
}

SettingsPtr KPlugin::mySettings() const
{
    return _settings;
}

QDir KPlugin::myResourcesDir() const
{
    QString thisModulePath;
    if (PluginManager::instance()->sharePath().isEmpty()) {
        thisModulePath = _resourcesDir;
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
//    Q_ASSERT_X(result.exists() && result.isReadable(), "KPlugin::myResourcesDir",
//               message.toLocal8Bit().constData()
//               );
    return result;
}

KPlugin::~KPlugin()
{

}

} // namespace ExtensionSystem
