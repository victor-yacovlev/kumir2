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

extern QDomElement dumpMacro(const Macro &m, QDomDocument &document, QDomElement &root)
{
    QDomElement macroElement = document.createElement("macro");
    macroElement.setAttribute("title", m.title);
    if (!m.key.isNull())
        macroElement.setAttribute("key", m.key);
    for (int i=0; i<m.commands.size(); i++) {
        const KeyCommand & cmd = m.commands[i];
        QDomElement commandElement = document.createElement("command");
        commandElement.setAttribute("name", dumpKeyCommandType(cmd.type));
        if (cmd.text.length() > 0)
            commandElement.setAttribute("text", screenString(cmd.text));
        macroElement.appendChild(commandElement);
    }
    root.appendChild(macroElement);
    return macroElement;
}

extern bool loadMacro(const QDomElement &v, Macro &m)
{
    m.action = 0;
    m.commands.clear();
    m.title = v.attribute("title");
    const QString key = v.attribute("key").trimmed();
    if (key.length()==1)
        m.key = key[0];    
    else
        m.key = QChar(QChar::Null);
    if ( key.toLower() == "up" )
        m.extKey = Qt::Key_Up;
    else if ( key.toLower() == "down" )
        m.extKey = Qt::Key_Down;
    else if ( key.toLower() == "left" )
        m.extKey = Qt::Key_Left;
    else if ( key.toLower() == "right" )
        m.extKey = Qt::Key_Right;
    else if ( key.toLower() == "space" )
        m.extKey = Qt::Key_Space;

    const QDomNodeList commands = v.elementsByTagName("command");
    for (int i=0; i<commands.count(); i++) {
        const QDomElement cmd = commands.at(i).toElement();
        QString text = cmd.attribute("text");
        text = text.replace(QString("\\n"), QString("\n"));
        text = text.replace(QString("\\\""), QString("\""));
        text = text.replace(QString("\\\\"), QString("\\"));
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
    QDomDocument document("macros");
    QDomElement root = document.createElement("macros");
    document.appendChild(root);
    for (int i=0; i<macros.size(); i++) {
        const Macro & macro = macros[i];
        dumpMacro(macro, document, root);
    }
    QFile f(fileName);
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QTextStream ts(&f);
        document.save(ts, 4);
        f.close();
        return true;
    }
    else {
        return false;
    }
}

}
