#ifndef STATEMENT_H
#define STATEMENT_H

#include <QtCore>
#include "interfaces/lexemtype.h"
#include "dataformats/lexem.h"
#include "interfaces/error.h"

namespace AST {
struct Statement;
struct Module;
struct Algorhitm;
struct Variable;
}

using AST::Lexem;

namespace KumirAnalizer {

struct Statement
{
    inline Statement(Shared::LexemType lt) { type=lt; alg=0; mod=0; statement=0; conditionalIndex=0; }
    QList<Lexem*> data;
    QPoint indentRank;
    Shared::LexemType type;
    struct AST::Statement * statement;
    struct AST::Algorhitm * alg;
    struct AST::Module * mod;
    QList<AST::Variable*> variables;
    int conditionalIndex;
    bool hasError() const;
    void setError(const QString & err,
                  const Lexem::ErrorStage stage,
                  const Lexem::ErrorRaisePosition raise
                  );
    explicit Statement();
    QPair<QString,quint32> suggestedClosingBracket;
};

}

#endif // STATEMENT_H
