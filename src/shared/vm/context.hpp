#ifndef CONTEXT_H
#define CONTEXT_H

#include <vector>
#include "variant.hpp"
#include "vm_instruction.hpp"
#include "vm_tableelem.hpp"

namespace VM {

enum ContextRunMode {
    CRM_UntilReturn,
    CRM_ToEnd,
    CRM_OneStep
};

struct Context {
    inline Context() {
        IP = -1; type = Bytecode::EL_FUNCTION;
        runMode = CRM_ToEnd; lineNo = -1;
        algId = -1;
    }

    int registers[201];
    int IP;
    std::vector<Variable> locals;
    const std::vector<Bytecode::Instruction> * program;
    Bytecode::ElemType type;
    ContextRunMode runMode;
    uint8_t moduleId;
    int algId;
    int lineNo;
};

}

#endif
