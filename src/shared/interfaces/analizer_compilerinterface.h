#ifndef ANALIZER_COMPILERINTERFACE_H
#define ANALIZER_COMPILERINTERFACE_H

#include "dataformats/ast.h"
#include "error.h"

#include <QString>
#include <QList>

#include <string>

namespace Shared { namespace Analizer {


class ASTCompilerInterface
{
public:
    virtual const AST::DataPtr abstractSyntaxTree() const = 0;
};

class ExternalExecutableCompilerInterface
{
public:
    virtual void prepareToRun() = 0;
    virtual bool compiledWithoutErrors() const = 0;
    virtual QString executableFilePath() const = 0;
};


}}

Q_DECLARE_INTERFACE(Shared::Analizer::ASTCompilerInterface,
                    "kumir2.Analizer.ASTCompilerInterface")
Q_DECLARE_INTERFACE(Shared::Analizer::ExternalExecutableCompilerInterface,
                    "kumir2.Analizer.ExternalExecutableCompilerInterface")

#endif // ANALIZER_COMPILERINTERFACE_H
