#ifndef SCH_COMMAND_H
#define SCH_COMMAND_H

#include <QtCore>
#include <QtScript>

namespace Schema {
    enum Command {
        CmdNone = 0,
        CmdTurnLeft = 1,
        CmdGoForward = 2,
        CmdTurnRight = 3,
        CmdDoAction = 4,
        CmdCall1 = 5,
        CmdCall2 = 6,
        CmdCall3 = 7,
        CmdCall4 = 8,
        CmdCall5 = 9,
        CmdCall6 = 10,
        CondWasAction = 11,
        CondNotAction = 12,
        CondIsWall = 13,
        CondNoWall = 14,
        RepForever = 15,
        Rep2 = 16,
        Rep3 = 17,
        Rep4 = 18,
        Rep5 = 19,
        Rep6 = 20
    };

    extern bool parceJSON(const QScriptValue &value, Command &command);
    extern bool parceAGKXML(const QString &shortcut, Command &command);
    extern QString generateJSON(const Command &command);
}


#endif // SCH_COMMAND_H
