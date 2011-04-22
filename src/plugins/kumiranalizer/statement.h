#ifndef STATEMENT_H
#define STATEMENT_H

#include <QtCore>
#include "interfaces/lexemtype.h"
#include "lexem.h"
#include "interfaces/error.h"

namespace KumirAnalizer {

struct Statement
{
    QList<Lexem> data;
    int realLineNumber;
    Shared::Error error;
    QPoint indentRank;
    Shared::LexemType type;
};

}

#endif // STATEMENT_H
