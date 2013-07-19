#include <QtCore>
#include <QtGui>
#include <QtXml>

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

extern bool loadMacro(const QDomElement &v, Macro &m)
{
    m.action = 0;
    m.commands.clear();
    m.title = v.attribute("title");
    const QString key = v.attribute("key");
    if (key.length()!=1)
        return false;
    m.key = key[0];
    const QDomNodeList commands = v.elementsByTagName("command");
    for (int i=0; i<commands.count(); i++) {
        const QDomElement cmd = commands.at(i).toElement();
        QString text = cmd.attribute("text");
        text.replace("\\n", "\n");
        text.replace("\\\\", "\\");
        KeyCommand command(
                    loadKeyCommandType(cmd.attribute("name")),
                    text
                    );
        m.commands.push_back(command);
    }
    return m.commands.size() > 0;
}

extern QList<Macro> loadFromFile(const QString &fileName)
{
    QList<Macro> result;
    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QDomDocument doc;
        doc.setContent(&f);
        const QDomNodeList macros = doc.elementsByTagName("macro");
        for (int i=0; i<macros.count(); i++) {
            Macro macro;
            if (loadMacro(macros.at(i).toElement(), macro)) {
                result << macro;
            }
        }
        f.close();
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
