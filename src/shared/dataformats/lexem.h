#ifndef LEXEM_H
#define LEXEM_H

#include <QtCore>
#include "interfaces/lexemtype.h"

namespace AST {

    struct Lexem {
        enum ErrorStage { NoError, Lexer, PDAutomata, Tables, Semantics } errorStage;
        inline Lexem() {
            type = Shared::LxTypeEmpty;
            lineNo = -1; linePos = 0; length = 0; errorStage = NoError;
        }
        Shared::LexemType type;
        QString data;
        int lineNo;
        int linePos;
        int length;
        QString error;
    };

}

#endif // LEXEM_H
