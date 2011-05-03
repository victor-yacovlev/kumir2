#ifndef LEXEM_H
#define LEXEM_H

#include "interfaces/analizerinterface.h"
#include <QtCore>

namespace KumirAnalizer {

    struct Lexem {
        Shared::LexemType type;
        QString data;
        int pos;
        int size;
        QString error;
    };

}

#endif // LEXEM_H
