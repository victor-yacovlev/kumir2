#ifndef KUMIRCODERUN_CONTEXT_H
#define KUMIRCODERUN_CONTEXT_H

#include <QtCore>

#include "variant.h"
#include "bytecode/instruction.h"

namespace KumirCodeRun {

struct Context {
    QVariant registers[256];
    quint16 IP;
    QVector<Variant> locals;
    QVector<Bytecode::Instruction> * program;
};

} // namespace KumirCodeRun

#endif // KUMIRCODERUN_CONTEXT_H
