#ifndef SCH_PROGRAM_H
#define SCH_PROGRAM_H

#include "sch_algorithm.h"

namespace Schema {

typedef QList<Algorhitm> Program;


extern bool parceJSON(const QScriptValue &value, Program &program);
extern QString generateJSON(const Program &program);

extern bool isEqual(const Program &a, const Program &b);

} // namespace Schema

#endif // PROGRAM_H
