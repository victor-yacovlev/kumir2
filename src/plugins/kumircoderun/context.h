#ifndef KUMIRCODERUN_CONTEXT_H
#define KUMIRCODERUN_CONTEXT_H

#include <QtCore>

#include "variant.h"
#include "bytecode/instruction.h"
#include "bytecode/tableelem.h"

namespace KumirCodeRun {

enum ContextRunMode {
    CRM_ToEnd,
    CRM_OneStep
};

struct Context {
    inline Context() {
        IP = 0; type = Bytecode::EL_FUNCTION;
        runMode = CRM_ToEnd; lineNo = -1;
    }

    QVariant registers[256];
    quint16 IP;
    QVector<Variant> locals;
    QVector<Bytecode::Instruction> program;
    Bytecode::ElemType type;
    ContextRunMode runMode;
    int lineNo;
};

} // namespace KumirCodeRun

#endif // KUMIRCODERUN_CONTEXT_H
