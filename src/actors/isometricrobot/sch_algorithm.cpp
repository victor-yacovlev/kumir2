#include "sch_algorithm.h"

namespace Schema {

extern bool parceJSON(const QScriptValue &value, Algorhitm &algorhitm)
{
    if (!value.isObject() ||
            !value.property("width").isNumber() ||
            !value.property("height").isNumber())
    {
        return false;
    }
    int w = value.property("width").toInteger();
    int h = value.property("height").toInteger();
    algorhitm.size = QSize(w,h);
    algorhitm.commands = QVector<Command>(w*h, CmdNone);
    if (value.property("hint").isArray()) {
        int len = value.property("hint").property("length").toInteger();
        for (int i=0; i<len; i++) {
            algorhitm.hints << value.property("hint").property(i).toString();
        }
    }
    if (value.property("hint").isString()) {
        algorhitm.hints << value.property("hint").toString();
    }
    if (value.property("repeater").isBool()) {
        algorhitm.repeater = value.property("repeater").toBool();
    }
    else {
        algorhitm.repeater = false;
    }
    if (value.property("condition").isBool()) {
        algorhitm.condition = value.property("condition").toBool();
    }
    else {
        algorhitm.condition = false;
    }

    if (value.property("data").isArray()) {
        int len = value.property("data").property("length").toInteger();
        len = qMin(algorhitm.commands.size(), len);
        for (int i=0; i<len; i++) {
            Command cmd;
            if ( parceJSON(value.property("data").property(i), cmd) )
                algorhitm.commands[i] = cmd;
        }
    }
    algorhitm.repeaterCommand = CmdNone;
    algorhitm.conditionCommand = CmdNone;
    if (value.property("repeater_data").isNumber()) {
        algorhitm.repeaterCommand = Command(value.property("repeater_data").toUInt32());
    }
    if (value.property("condition_data").isNumber()) {
        algorhitm.repeaterCommand = Command(value.property("condition_data").toUInt32());
    }
    return true;
}


extern QString generateJSON(const Algorhitm &algorhitm)
{
    const QString template0 = "{\n\t\"width\": %1,\n\t\"height\": %2,\n\t\"condition\": %3,\n\t\"condition_data\": %4,\n\t\"repeater\": %5,\n\t\"repeater_data\": %6,\n\t\"hint\": %7,\n\t\"data\": %8\n}";
    int width = algorhitm.size.width();
    int height = algorhitm.size.height();
    bool cond = algorhitm.condition;
    bool rep = algorhitm.repeater;
    QString condData = generateJSON(algorhitm.conditionCommand);
    QString repData = generateJSON(algorhitm.repeaterCommand);
    QString hint;
    if (algorhitm.hints.size()==0) {
        hint = "null";
    }
    if (algorhitm.hints.size()==1) {
        hint = "\""+algorhitm.hints.first()+"\"";
    }
    if (algorhitm.hints.size()>1) {
        hint += "[ ";
        for (int i=0; i<algorhitm.hints.size(); i++) {
            hint += "\""+algorhitm.hints[i]+"\"";
            if (i<algorhitm.hints.size()-1) {
                hint += ", ";
            }
        }
        hint += " ]";
    }
    QString data = "[ ";
    if (!data.isEmpty())
        data += "\n\t\t";
    for (int i=0; i<algorhitm.commands.size(); i++) {
        Command cmd = algorhitm.commands[i];
        data += generateJSON(cmd);
        if (i<algorhitm.commands.size()-1) {
            data += "\n\t\t,";
        }
    }
    if (!data.isEmpty())
        data += "\n\t";
    data += " ]";
    QString all = template0
            .arg(width)
            .arg(height)
            .arg(cond? "true" : "false")
            .arg(condData)
            .arg(rep? "true" : "false")
            .arg(repData)
            .arg(hint)
            .arg(data);
    return all;
}

extern bool isEqual(const Algorhitm &a, const Algorhitm &b)
{
    if (a.size!=b.size)
        return false;
    if (a.repeater!=b.repeater)
        return false;
    if (a.condition!=b.condition)
        return false;
    if (a.repeaterCommand!=b.repeaterCommand)
        return false;
    if (a.conditionCommand!=b.conditionCommand)
        return false;
    if (a.commands.size()!=b.commands.size())
        return false;
    for (int i=0; i<a.commands.size(); i++) {
        if (a.commands[i]!=b.commands[i])
            return false;
    }
    return true;
}

} // namespace Schema
