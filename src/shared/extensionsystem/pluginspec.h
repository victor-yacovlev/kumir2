#ifndef PLUGINSPEC_H
#define PLUGINSPEC_H

#include <QString>
#include <QList>
#include <QStringList>
// #include <QtScript/QScriptEngine>

#include <string>
#include <deque>
#include <wchar.h>

namespace ExtensionSystem {


struct PluginSpec
{
    QString name;
    QString version;
    QString copyright;
    QString license;
    bool gui;
    QString libraryFileName;
    QString specFileName;
    QStringList dependencies;
    QStringList provides;
};

// extern QString readSpecFromFile(const QString &fileName, PluginSpec & spec, QScriptEngine * engine);
extern QString readSpecFromFile(const QString & fileName, PluginSpec & spec);

} // namespace ExtensionSystem

#endif // PLUGINSPEC_H
