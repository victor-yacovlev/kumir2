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
    int conditionalIndex;
    bool hasError() const;
    explicit Statement();
};

}

#endif // STATEMENT_H
