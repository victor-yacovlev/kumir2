#ifndef SCH_ALGORHITM_H
#define SCH_ALGORHITM_H

#include <QtCore>
#include <QtScript>

#include "sch_command.h"

namespace Schema {

struct Algorhitm
{
    /* Form size */
    QSize size;
    bool repeater;
    bool condition;
    QStringList hints;
    QVector<Command> commands;
    Command repeaterCommand;
    Command conditionCommand;
};

extern bool parceJSON(const QScriptValue &value, Algorhitm &algorhitm);
extern QString generateJSON(const Algorhitm &algorhitm);

extern bool isEqual(const Algorhitm &a, const Algorhitm &b);

} // namespace Schema

#endif // SCH_ALGORHITM_H
