#ifndef GENERATOR_INTERFACE_H
#define GENERATOR_INTERFACE_H

#include "abstractsyntaxtree/ast.h"

namespace Shared {

enum GeneratorType {
    GenNativeExecuable,
    GenNotNativeExecuable
};

class GeneratorInterface {
public:
    virtual void generateExecuable(const AST::Data * tree
                                   , QIODevice * out
                                   , enum GeneratorType & outType) = 0;
};

}

#endif
