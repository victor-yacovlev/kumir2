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
    static PluginManager * instance();

    /** Set path to search plugins */
    void setPluginPath(const QString &path);

    /** Set path to search settings etc. */
    void setSharePath(const QString &path);

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
    QList<KPlugin*> loadedPlugins(const QRegExp & rx = QRegExp("*", Qt::CaseSensitive, QRegExp::Wildcard));

public slots:

    /** Show settings for all modules
      * @param parent widget, which blocks until dialog is closed
      */
    void showSettingsDialog();

    /** Stops all plugins in reverse-order of creation */
    void shutdown();

private:
    static PluginManager * m_instance;
    struct PluginManagerPrivate * d;

};

} // namespace ExtensionSystem

#endif // PLUGINMANAGER_H
