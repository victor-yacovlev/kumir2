#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QtCore>
#include "kplugin.h"

#ifdef EXTENSIONSYSTEM_LIBRARY
#define EXTENSIONSYSTEM_EXPORT Q_DECL_EXPORT
#else
#define EXTENSIONSYSTEM_EXPORT Q_DECL_IMPORT
#endif

namespace  ExtensionSystem {

class EXTENSIONSYSTEM_EXPORT PluginManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginManager();
    ~PluginManager();
    static PluginManager * instance();

    /** Set path to search plugins */
    void setPluginPath(const QString &path);

    /** Set path to search settings etc. */
    void setSharePath(const QString &path);

    bool showWorkspaceChooseOnLaunch() const;

    void switchGlobalState(GlobalState state);

    GlobalState currentGlobalState() const;

    /** Loads plugins by given template in form:
      *
      *     Plugin1(),Plugin2,*Plugin3,Plugin4(argument1,argument2)
      *
      * where *-marked plugin is entry point,
      * plugin arguments are in brackets (optional)
      *
      * @param templ template
      * @return error message or empty string on success */
    QString loadPluginsByTemplate(const QString &templ);

    /** Loads extra module (example: actor file on demand) by given canonical
     *  file name @param canonicalFileName.
     *
     *  @returns error message or empty string on success */
    QString loadExtraModule(const std::string & canonicalFileName);

    /** Returns true if at least one module requires gui event processing */
    bool isGuiRequired() const;

    /** Initializes plugings by parsing commandline arguments. Returns error message */
    QString initializePlugins();

    /** Starts entry point plugin. Returns error message */
    QString start();

    /** Returns plugin spec */
    PluginSpec specByObject(const KPlugin * p) const;

    /** Returns plugin state */
    KPlugin::State stateByObject(const KPlugin * p) const;

    /** Returns plugin settings */
    QSettings * settingsByObject(const KPlugin * p) const;

    /** Returns plugin dependency by name */
    KPlugin * dependentPlugin(const QString &name, const class KPlugin * p) const;

    /** Return all plugins matched by regexp */
    QList<KPlugin*> loadedPlugins(const QString & pattern = "*");
    QList<const KPlugin*> loadedConstPlugins(const QString & pattern = "*") const;

    /** Return plugin by specified name */
    KPlugin* loadedPlugin(const QString & name);

    /** Module to start */
    KPlugin * startupModule();

    template <class PluginInterface>
    PluginInterface* findPlugin() {
        QList<KPlugin*> plugins = loadedPlugins();
        PluginInterface* result = nullptr;
        for (int i=0; i<plugins.size(); i++) {
            KPlugin * plugin = plugins[i];
            result = qobject_cast<PluginInterface*>(plugin);
            if (result)
                break;
        }
        return result;
    }

public slots:

    /** Show settings for all modules
      * @param parent widget, which blocks until dialog is closed
      */
    void showSettingsDialog();

    void switchToDefaultWorkspace();

    bool showWorkspaceChooseDialog();

    /** Stops all plugins in reverse-order of creation */
    void shutdown();

private:

    static PluginManager * m_instance;
    struct PluginManagerPrivate * d;

};

} // namespace ExtensionSystem

#endif // PLUGINMANAGER_H
