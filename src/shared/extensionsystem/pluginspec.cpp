#include "pluginspec.h"

#include <QtCore>

#include "../../VERSION.h"

namespace ExtensionSystem {

extern QString readSpecFromFile(const QString &fileName, PluginSpec &spec)
{
    QFile f(fileName);
    if (!f.exists()) {
        //return "Can't open plugin spec file "+fileName;
        return "";
    }
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        spec.specFileName = fileName;
        QTextStream ts(&f);
        QStringList lines = ts.readAll().split("\n", QString::SkipEmptyParts);
        for (int i=0; i<lines.size(); i++) {
            QString line = lines[i];
            line = line.simplified();
            QStringList pair = line.split("=",QString::SkipEmptyParts);
            if (pair.size()==2) {
                QString key = pair[0].simplified();
                QString value = pair[1].simplified();
                if (key=="name") {
                    spec.name = value;
                    spec.provides << value;
                }
                else if (key=="provides") {
                    QStringList lst = value.split(",", QString::SkipEmptyParts);
                    for (int j=0; j<lst.size(); j++) {
                        QString v = lst[j].simplified();
                        spec.provides << v;
                    }
                }
                else if (key=="requires") {
                    QStringList lst = value.split(",", QString::SkipEmptyParts);
                    for (int j=0; j<lst.size(); j++) {
                        QString v = lst[j].simplified();
                        spec.dependencies << v;
                    }
                }
                else if (key=="gui") {
                    value = value.toLower();
                    spec.gui = value=="true" || value=="yes" || value=="1";
                }
            }
        }
        f.close();
    }
    return "";
}


} // namespace ExtensionSystem
