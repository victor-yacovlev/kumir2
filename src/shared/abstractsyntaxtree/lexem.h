#ifndef LEXEM_H
#define LEXEM_H

#include <QtCore>
#include "interfaces/lexemtype.h"

namespace AST {

    struct Lexem {
        inline Lexem() {
            type = Shared::LxTypeEmpty;
            lineNo = -1; linePos = 0; length = 0; lexerError = false;
        }
        Shared::LexemType type;
        QString data;
        int lineNo;
        int linePos;
        int length;
        QString error;
        bool lexerError;
    };

}

#endif // LEXEM_H
