#ifndef STATEMENT_H
#define STATEMENT_H

#include <QtCore>
#include "interfaces/lexemtype.h"
#include "dataformats/lexem.h"
#include "interfaces/error.h"

namespace AST {
struct Statement;
struct Module;
struct Algorithm;
struct Variable;

typedef QSharedPointer<Statement> StatementPtr;
typedef QSharedPointer<Module> ModulePtr;
typedef QSharedPointer<Algorithm> AlgorithmPtr;
typedef QSharedPointer<Variable> VariablePtr;
}

using AST::Lexem;

namespace KumirAnalizer {

struct Statement
{
    inline Statement(Shared::LexemType lt) : type(lt), conditionalIndex(0) {}
    QList<Lexem*> data;
    QPoint indentRank;
    Shared::LexemType type;
    AST::StatementPtr statement;
    AST::AlgorithmPtr alg;
    AST::ModulePtr mod;
    QList<AST::VariablePtr> variables;
    int conditionalIndex;
    bool hasError() const;
    void setError(const QString & err,
                  const Lexem::ErrorStage stage,
                  const Lexem::ErrorRaisePosition raise
                  );
    explicit Statement();
    mutable QPair<QString,quint32> suggestedClosingBracket;
    mutable QStringList suggestedImportModuleNames;
};

}

#endif // STATEMENT_H
