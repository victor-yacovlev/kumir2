#ifndef SCH_GAME_H
#define SCH_GAME_H

#include "sch_task.h"
#include "fp.h"

namespace Schema {

struct Game {
    /* title */
    QString title;
    /* legal */
    QStringList authors;
    QString copyright;
    QString license;
    QString homepage;
    /* content */
    QList<Task> tasks;
    /* entry point */
    int index;
};


extern bool parceJSON(const QScriptValue &value, Game &game);
extern QString generateJSON(const Game &game);

extern bool isEqual(const Game &a, const Game &b);

} // namespace Schema

#endif // SCH_GAME_H
