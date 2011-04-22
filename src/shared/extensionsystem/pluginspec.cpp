#include "pluginspec.h"

#include <QtCore>
#include <QScriptValue>
#include <QScriptEngine>

namespace ExtensionSystem {

extern QString readSpecFromFile(const QString &fileName, PluginSpec &spec, QScriptEngine * engine)
{
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        return "Can't open plugin spec file "+fileName;
    }
    QString data = "data = "+QString::fromUtf8(f.readAll())+";\n";
    f.close();

    QScriptValue toplevel = engine->evaluate(data);
    if (toplevel.isError()) {
        return "Can't parse plugin spec file "+fileName+": "+toplevel.toString();
    }
    spec.gui = false;
    if (!toplevel.isObject()) {
        return "Plugin spec file is not valid: "+fileName;
    }
    if (toplevel.property("name").isString()) {
        spec.name = toplevel.property("name").toString().trimmed();
        spec.provides << spec.name;
    }
    else {
        return "Plugin spec file does not have string property 'name': "+fileName;
    }
    if (toplevel.property("version").isString()) {
        spec.version = toplevel.property("version").toString().trimmed();
    }
    else {
        return "Plugin spec file does not have string property 'version': "+fileName;
    }
    if (toplevel.property("copyright").isString()) {
        spec.copyright = toplevel.property("copyright").toString();
    }
    if (toplevel.property("license").isString()) {
        spec.license = toplevel.property("license").toString();
    }
    if (toplevel.property("requires").isString()) {
        // Depends by single module name of any version
        Dependency dep;
        dep.name = toplevel.property("depends").toString().trimmed();
        if (!dep.name.isEmpty()) {
            spec.dependencies << dep;
        }
    }
    else if (toplevel.property("requires").isArray()) {
        int len = toplevel.property("requires").property("length").toInteger();
        for (int i=0; i<len; i++) {
            QScriptValue depValue = toplevel.property("requires").property(i);
            if (depValue.isString()) {
                Dependency dep;
                dep.name = depValue.toString().trimmed();
                if (!dep.name.isEmpty()) {
                    spec.dependencies << dep;
                }
            }
            else if (depValue.isObject()) {
                Dependency dep;
                if (depValue.property("name").isString()) {
                    dep.name = depValue.property("name").toString();
                }
                if (depValue.property("minVersion").isString()) {
                    dep.minVersion = depValue.property("minVersion").toString();
                }
                if (depValue.property("maxVersion").isString()) {
                    dep.maxVersion = depValue.property("maxVersion").toString();
                }
                if (!dep.name.isEmpty()) {
                    spec.dependencies << dep;
                }
            }
        }
    }
    else if (toplevel.property("requires").isObject()) {
        // Depends by single module
        Dependency dep;
        if (toplevel.property("requires").property("name").isString()) {
            dep.name = toplevel.property("requires").property("name").toString();
        }
        if (toplevel.property("requires").property("minVersion").isString()) {
            dep.minVersion = toplevel.property("requires").property("minVersion").toString();
        }
        if (toplevel.property("requires").property("maxVersion").isString()) {
            dep.maxVersion = toplevel.property("requires").property("maxVersion").toString();
        }
        if (!dep.name.isEmpty()) {
            spec.dependencies << dep;
        }
    }
    if (toplevel.property("provides").isString()) {
        // Depends by single module name of any version
        QString pname;
        pname = toplevel.property("provides").toString().trimmed();
        if (!pname.isEmpty()) {
            spec.provides << pname;
        }
    }
    else if (toplevel.property("provides").isArray()) {
        int len = toplevel.property("provides").property("length").toInteger();
        for (int i=0; i<len; i++) {
            QScriptValue depValue = toplevel.property("provides").property(i);
            if (depValue.isString()) {
                QString pname;
                pname = depValue.toString().trimmed();
                if (!pname.isEmpty()) {
                    spec.provides << pname;
                }
            }
        }
    }
    if (toplevel.property("gui").isString()) {
        const QStringList trueStr( QStringList() << "true" << "1" << "TRUE" << "True" << "yes" << "YES" << "Yes");
        spec.gui = trueStr.contains(toplevel.property("gui").toString());
    }
    if (toplevel.property("gui").isBool()) {
        spec.gui = toplevel.property("gui").toBool();
    }
    if (toplevel.property("gui").isNumber()) {
        spec.gui = toplevel.property("gui").toInteger()!=0;
    }
    return "";
}

} // namespace ExtensionSystem
