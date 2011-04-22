#ifndef PLUGINSPEC_H
#define PLUGINSPEC_H

#include <QString>
#include <QList>
#include <QtScript/QScriptEngine>

namespace ExtensionSystem {

struct Dependency {
    QString name;
    QString minVersion;
    QString maxVersion;
};

struct PluginSpec
{
    QString name;
    QString version;
    QString copyright;
    QString license;
    bool gui;
    QString libraryFileName;
    QString specFileName;
    QList<Dependency> dependencies;
    QList<QString> provides;
};

extern QString readSpecFromFile(const QString &fileName, PluginSpec & spec, QScriptEngine * engine);

} // namespace ExtensionSystem

#endif // PLUGINSPEC_H
