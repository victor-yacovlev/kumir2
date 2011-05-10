#ifndef KPLUGIN_H
#define KPLUGIN_H

#include <QtCore>
#include "pluginspec.h"

#ifdef EXTENSIONSYSTEM_LIBRARY
#define EXTENSIONSYSTEM_EXPORT Q_DECL_EXPORT
#else
#define EXTENSIONSYSTEM_EXPORT Q_DECL_IMPORT
#endif

namespace ExtensionSystem {

class EXTENSIONSYSTEM_EXPORT KPlugin : public QObject
{
    friend class PluginManager;
    Q_OBJECT
public:
    enum State { Disabled, Loaded, Initialized, Started, Stopped };
    KPlugin();
    PluginSpec pluginSpec() const;
    State state() const;
    virtual ~KPlugin();
protected:
    virtual QString initialize(const QStringList &arguments) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    KPlugin * myDependency(const QString & name) const;

    QSettings * mySettings() const;

};

} // namespace ExtensionSystem

#endif // KPLUGIN_H
