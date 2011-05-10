#ifndef LEXEM_H
#define LEXEM_H

#include <QtCore>
#include "interfaces/lexemtype.h"

namespace AST {

    struct Lexem {
        Shared::LexemType type;
        QString data;
        int lineNo;
        int linePos;
        int length;
        QString error;
    };

}

#endif // LEXEM_H
