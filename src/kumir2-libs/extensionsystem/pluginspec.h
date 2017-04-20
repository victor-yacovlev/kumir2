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
    bool gui = false;
    QByteArray name;
    QList<QByteArray> dependencies;
    QList<QByteArray> provides;

    // Fields filled by launcher
    bool main = false;
    QByteArray libraryFileName;
    QStringList arguments;
};


extern QString readSpecFromFile(const QString & fileName, PluginSpec & spec);

} // namespace ExtensionSystem

#endif // PLUGINSPEC_H
