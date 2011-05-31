#ifndef GENERATOR_INTERFACE_H
#define GENERATOR_INTERFACE_H

#include "abstractsyntaxtree/ast.h"

namespace Shared {

enum GeneratorType {
    GenError,
    GenNativeExecuable,
    GenNotNativeExecuable
};

struct GeneratorResult {
    GeneratorType type;
    QStringList usedLibs;
    QStringList usedQtLibs;
};


class GeneratorInterface {
public:
    virtual GeneratorResult generateExecuable(
            const AST::Data * tree
            , QIODevice * out) = 0;
};

}

Q_DECLARE_INTERFACE(Shared::GeneratorInterface, "kumir2.generator")

#endif
