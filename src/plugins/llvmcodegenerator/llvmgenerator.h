#ifndef LLVMGENERATOR_H
#define LLVMGENERATOR_H

#include "dataformats/ast.h"

#include <llvm/Module.h>


namespace LLVMCodeGenerator {

class LLVMGenerator
{
public:
    explicit LLVMGenerator();
    void reset();

};

}

#endif // LLVMGENERATOR_H
