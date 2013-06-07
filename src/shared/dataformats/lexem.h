#ifndef LEXEM_H
#define LEXEM_H

#include <QtCore>
#include "interfaces/lexemtype.h"

namespace AST {

    struct Lexem {
        enum ErrorStage { NoError, Lexer, BeforePDAutomata, PDAutomata, Tables, Semantics } errorStage;
        enum ErrorRaisePosition { AsIs, Header, Begin, End } errorRaisePosition;
        inline Lexem() {
            type = Shared::LxTypeEmpty;
            lineNo = -1; linePos = 0; length = 0;
            errorStage = NoError;
            errorRaisePosition = AsIs;
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
