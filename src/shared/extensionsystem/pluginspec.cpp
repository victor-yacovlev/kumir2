#include "pluginspec.h"

#include <QtCore>
//#include <QScriptValue>
//#include <QScriptEngine>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include "../../VERSION.h"

namespace ExtensionSystem {

extern QString readSpecFromFile(const QString &fileName, PluginSpec &spec)
{
    QFile f(fileName);
    if (!f.exists()) {
        //return "Can't open plugin spec file "+fileName;
        return "";
    }
    std::string fn = fileName.toStdString();
    using boost::property_tree::ptree;
    using boost::property_tree::json_parser::read_json;
    ptree pt;
    read_json(fn, pt);
    spec.name = QString::fromStdString(pt.get<std::string>("name"));
    spec.provides << spec.name;
    spec.version = QString::fromStdString(pt.get<std::string>("version",""));
    if (spec.version.length()==0)
        spec.version = QString::fromAscii("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_RELEASE);
    spec.copyright = QString::fromStdString(pt.get<std::string>("copyright",""));
    spec.license = QString::fromStdString(pt.get<std::string>("license",""));
    spec.gui = pt.get<int>("gui", false) > 0;

    if (pt.count("requires")>0)
    BOOST_FOREACH(ptree::value_type & v,
                  pt.get_child("requires"))
    {
        spec.dependencies << QString::fromStdString(v.second.data());
    }
    if (pt.count("provides")>0)
    BOOST_FOREACH(ptree::value_type & v,
                  pt.get_child("provides"))
    {
        spec.provides << QString::fromStdString(v.second.data());
    }

    return "";
}

/*
extern QString readSpecFromFile(const QString &fileName, PluginSpec &spec, QScriptEngine * engine)
{
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly|QIODevice::Text)) {
//        return "Can't open plugin spec file "+fileName;
        return "";
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
        spec.version = QString::fromAscii("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_RELEASE);
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
        dep.name = toplevel.property("requires").toString().trimmed();
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
*/

} // namespace ExtensionSystem
