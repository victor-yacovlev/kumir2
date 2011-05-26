#ifndef GENERATOR_INTERFACE_H
#define GENERATOR_INTERFACE_H

#include "abstractsyntaxtree/ast.h"

namespace Shared {

enum GeneratorType {
    GenError,
    GenNativeExecuable,
    GenNotNativeExecuable
};

class GeneratorInterface {
public:
    virtual enum GeneratorType generateExecuable(
            const AST::Data * tree
            , QIODevice * out) = 0;
};

}

Q_DECLARE_INTERFACE(Shared::GeneratorInterface, "kumir2.generator")

#endif
