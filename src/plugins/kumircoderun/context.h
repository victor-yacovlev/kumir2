#ifndef KUMIRCODERUN_CONTEXT_H
#define KUMIRCODERUN_CONTEXT_H

#include <QtCore>

#include "variant.h"
#include "bytecode/instruction.h"
#include "bytecode/tableelem.h"

namespace KumirCodeRun {

struct Context {
    QVariant registers[256];
    quint16 IP;
    QVector<Variant> locals;
    QVector<Bytecode::Instruction> program;
    Bytecode::ElemType type;
    int lineNo;
};

} // namespace KumirCodeRun

#endif // KUMIRCODERUN_CONTEXT_H
