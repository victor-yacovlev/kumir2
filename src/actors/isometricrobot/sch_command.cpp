#include "sch_command.h"

namespace Schema {

extern bool parceJSON(const QScriptValue &value, Command &cmd)
{
    if (value.isNull()) {
        cmd = CmdNone;
        return true;
    }
    if (value.isString()) {
        cmd = CmdNone;
        QString cmdv = value.toString().trimmed();
        if ( cmdv=="CmdNone" ) cmd = CmdNone;
        if ( cmdv=="CmdTurnLeft" ) cmd = CmdTurnLeft;
        if ( cmdv=="CmdGoForward" ) cmd = CmdGoForward;
        if ( cmdv=="CmdTurnRight" ) cmd = CmdTurnRight;
        if ( cmdv=="CmdDoAction" ) cmd = CmdDoAction;
        if ( cmdv=="CmdCall1" ) cmd = CmdCall1;
        if ( cmdv=="CmdCall2" ) cmd = CmdCall2;
        if ( cmdv=="CmdCall3" ) cmd = CmdCall3;
        if ( cmdv=="CmdCall4" ) cmd = CmdCall4;
        if ( cmdv=="CmdCall5" ) cmd = CmdCall5;
        if ( cmdv=="CmdCall6" ) cmd = CmdCall6;
        if ( cmdv=="CondWasAction" ) cmd = CondWasAction;
        if ( cmdv=="CondNotAction" ) cmd = CondNotAction;
        if ( cmdv=="CondIsWall" ) cmd = CondIsWall;
        if ( cmdv=="CondNoWall" ) cmd = CondNoWall;
        if ( cmdv=="RepForever" ) cmd = RepForever;
        if ( cmdv=="Rep2" ) cmd = Rep2;
        if ( cmdv=="Rep3" ) cmd = Rep3;
        if ( cmdv=="Rep4" ) cmd = Rep4;
        if ( cmdv=="Rep5" ) cmd = Rep5;
        if ( cmdv=="Rep6" ) cmd = Rep6;
        return true;
    }
    else {
        return false;
    }
}

#define _(x) QString::fromUtf8(x)

extern bool parceAGKXML(const QString &shortcut, Command &command)
{
    if (shortcut.trimmed().isEmpty() || shortcut.trimmed()=="-") {
        command = CmdNone;
        return true;
    }
    if (shortcut.trimmed()==_("ч?")) {
        command = CondNotAction;
        return true;
    }
    if (shortcut.trimmed()==_("з?")) {
        command = CondWasAction;
        return true;
    }
    if (shortcut.trimmed()==_("св?")) {
        command = CondNoWall;
        return true;
    }
    if (shortcut.trimmed()==_("ст?")) {
        command = CondIsWall;
        return true;
    }
    if (shortcut.trimmed()==_("л")) {
        command = CmdTurnLeft;
        return true;
    }
    if (shortcut.trimmed()==_("п")) {
        command = CmdTurnRight;
        return true;
    }
    if (shortcut.trimmed()==_("в")) {
        command = CmdGoForward;
        return true;
    }
    if (shortcut.trimmed()==_("з")) {
        command = CmdDoAction;
        return true;
    }
    if (shortcut.trimmed()==_("forever")) {
        command = RepForever;
        return true;
    }
    bool ok;
    int rep = shortcut.toInt(&ok);
    command = Command(RepForever+rep-1);
    return ok;
}

extern QString generateJSON(const Command &command)
{
    if ( command==CmdTurnLeft) return "\"CmdTurnLeft\"";
    if ( command==CmdGoForward) return "\"CmdGoForward\"";
    if ( command==CmdTurnRight) return "\"CmdTurnRight\"";
    if ( command==CmdDoAction) return "\"CmdDoAction\"";
    if ( command==CmdCall1) return "\"CmdCall1\"";
    if ( command==CmdCall2) return "\"CmdCall2\"";
    if ( command==CmdCall3) return "\"CmdCall3\"";
    if ( command==CmdCall4) return "\"CmdCall4\"";
    if ( command==CmdCall5) return "\"CmdCall5\"";
    if ( command==CmdCall6) return "\"CmdCall6\"";
    if ( command==CondWasAction) return "\"CondWasAction\"";
    if ( command==CondNotAction) return "\"CondNotAction\"";
    if ( command==CondIsWall) return "\"CondIsWall\"";
    if ( command==CondNoWall) return "\"CondNoWall\"";
    if ( command==RepForever) return "\"RepForever\"";
    if ( command==Rep2) return "\"Rep2\"";
    if ( command==Rep3) return "\"Rep3\"";
    if ( command==Rep4) return "\"Rep4\"";
    if ( command==Rep5) return "\"Rep5\"";
    if ( command==Rep6) return "\"Rep6\"";
    return "\"CmdNone\"";
}

}
