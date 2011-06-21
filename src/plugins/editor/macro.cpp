#include <QtCore>
#include <QtGui>
#include <QtScript>

#include "macro.h"

namespace Editor {

QString screenString(QString s)
{
    s.replace("\\", "\\\\");
    s.replace("\n","\\n");
    s.replace("\"", "\\\"");
    return s;
}

extern QString dumpMacro(Macro &m)
{
    QString result = "{\n";
    result += "  title: \""+screenString(m.title)+"\",\n";
    result += "  key: \""+screenString(QString(m.key).toUpper())+"\",\n";
    result += "  commands: [\n";
    for (int i=0; i<m.commands.size(); i++) {
        result += QString("    { command: \"%1\", text: \"%2\" }")
                .arg(screenString(m.commands[i].text))
                .arg(dumpKeyCommandType(m.commands[i].type));
        if (i<m.commands.size()-1)
            result += ",";
        result += "\n";
    }
    result += "  ]\n";
    result += "}";
    return result;
}

extern bool loadMacro(const QScriptValue &v, Macro &m)
{
    m.action = 0;
    m.commands.clear();
    m.title = v.property("title").toString();
    const QString key = v.property("key").toString();
    if (key.length()!=1)
        return false;
    m.key = key[0];
    quint32 commandsCount = v.property("commands").property("length").toUInt32();
    if (commandsCount==0)
        return false;
    for (quint32 i=0; i<commandsCount; i++) {
        QScriptValue cmd = v.property("commands").property(i);
        KeyCommand command(loadKeyCommandType(cmd.property("command").toString()),
                           cmd.property("text").toString()
                           );
        m.commands << command;
    }
    return true;
}

extern QList<Macro> loadFromFile(const QString &fileName)
{
    QList<Macro> result;
    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream ts(&f);
        ts.setCodec("UTF-8");;
        ts.setAutoDetectUnicode(true);
        const QString data = ts.readAll();
        f.close();
        QScriptEngine e;
        QScriptValue v = e.evaluate(data);
        if (v.isArray()) {
            quint32 length = v.property("length").toUInt32();
            for (quint32 i=0; i<length; i++) {
                QScriptValue vv = v.property(i);
                Macro m;
                if (loadMacro(vv, m))
                    result << m;
            }
        }
    }
    return result;
}

extern bool saveToFile(const QString &fileName, const QList<Macro> &macros)
{
    QFile f(fileName);
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QTextStream ts(&f);
        ts.setCodec("UTF-8");
        ts.setGenerateByteOrderMark(true);
        ts << "[\n";
        for (int i=0; i<macros.size(); i++) {
            Macro m = macros[i];
            QStringList lines = dumpMacro(m).split("\n");
            foreach (QString line, lines) {
                ts << "  ";
                ts << line;
                ts << "\n";
            }
            if (i<macros.size()-1) {
                ts << ", ";
            }
        }
        ts << "]\n";
        f.close();
        return true;
    }
    else {
        return false;
    }
}

}
