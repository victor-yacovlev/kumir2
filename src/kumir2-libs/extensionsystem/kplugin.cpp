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
#ifdef Q_OS_UNIX
    // Something overrided by environment variable
    const QByteArray envName =
            QByteArray("KUMIR2_") +
            _pluginSpec.name.toUpper() +
            QByteArray("_RESOURCES_DIR");
    const char * envVal = ::getenv(envName.constData());
    if (envVal) {
        const QString path = QDir::cleanPath(QString::fromUtf8(envVal));
        const QDir result = QDir(path);
        if (result.exists()) {
            return result;
        }
    }
#endif

    // Non-standard plugin location
    if (_pluginSpec.nonStandardPluginDir.length() > 0) {
        const QString pluginDirName = QString::fromLatin1(_pluginSpec.name).toLower();
        const QStringList candidates = QStringList()
                << _pluginSpec.nonStandardPluginDir + "/../../share/" + pluginDirName
                << _pluginSpec.nonStandardPluginDir + "/" + pluginDirName
                << _pluginSpec.nonStandardPluginDir + "/resources"
                << _pluginSpec.nonStandardPluginDir + "/data"
                << _pluginSpec.nonStandardPluginDir + "/" + pluginDirName + "/resources"
                << _pluginSpec.nonStandardPluginDir + "/" + pluginDirName + "/data"
                   ;
        Q_FOREACH(const QString & dirName, candidates) {
            const QString path = QDir::cleanPath(dirName);
            const QDir result = QDir(path);
            if (result.exists()) {
                return result;
            }
        }
    }

    // Standard distribution
    const QString sharePath = PluginManager::instance()->sharePath();
    if(!sharePath.isEmpty()) {
        QString relPath = QString::fromLatin1(pluginName()).toLower();
        if (relPath.startsWith("actor")) {
            relPath = "actors/" + relPath.mid(5);
        }
        const QString path = QDir::cleanPath(sharePath + "/" + relPath);
        const QDir result = QDir(path);
        if (result.exists()) {
            return result;
        }
    }
    qDebug() << "Resources dir not found for plugin: " << _pluginSpec.name;
    return QDir::current();  // some fallback value
}

KPlugin::~KPlugin()
{

}

} // namespace ExtensionSystem
