#ifndef STATEMENT_H
#define STATEMENT_H

#include <QtCore>
#include "interfaces/lexemtype.h"
#include "lexem.h"
#include "interfaces/error.h"

namespace AST {
struct Statement;
struct Module;
struct Algorhitm;
}

namespace KumirAnalizer {

struct Statement
{
    QList<Lexem*> data;
    int realLineNumber;
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
