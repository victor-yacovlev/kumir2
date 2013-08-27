#ifndef ANALIZER_COMPILERINTERFACE_H
#define ANALIZER_COMPILERINTERFACE_H

#include "dataformats/ast.h"
#include "error.h"

#include <QString>
#include <QList>

#include <string>

namespace Shared { namespace Analizer {


class ASTCompilerInterface {
public:
    virtual const AST::DataPtr abstractSyntaxTree() const = 0;
};


}}

Q_DECLARE_INTERFACE(Shared::Analizer::ASTCompilerInterface,
                    "kumir2.Analizer.CompilerInterface")

#endif // ANALIZER_COMPILERINTERFACE_H
