#ifndef PLUGINSPEC_H
#define PLUGINSPEC_H

#include <QString>
#include <QList>
#include <QStringList>


#include <string>
#include <deque>
#include <wchar.h>

namespace ExtensionSystem {


struct PluginSpec
{
    // Mandatory fields filled by plugin itself
    bool gui;
    QByteArray name;
    QList<QByteArray> dependencies;
    QList<QByteArray> provides;

    // Fields filled by launcher
    bool main;
    QByteArray libraryFileName;
    QString nonStandardPluginDir;
    QStringList arguments;

    inline explicit PluginSpec() { gui = false; main = false; }
};


extern QString readSpecFromFile(const QString & fileName, PluginSpec & spec);

} // namespace ExtensionSystem

#endif // PLUGINSPEC_H
