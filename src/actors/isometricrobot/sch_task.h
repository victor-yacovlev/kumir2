#ifndef SCH_TASK_H
#define SCH_TASK_H

#include "sch_environment.h"
#include "sch_program.h"

#include <QImage>

namespace Schema {

struct Task {
    /* Task title */
    QString title;
    /* Hint image and its type, if present */
    QByteArray hintData;
    QString hintMimeType;
    /* Actor environment */
    Environment environment;
    /* Program */
    Program program;
};

extern bool parceJSON(const QScriptValue &value, Task &task);
extern QString generateJSON(const Task &task);

extern bool isEqual(const Task &a, const Task &b);

} // namespace Schema

#endif // SCH_TASK_H
