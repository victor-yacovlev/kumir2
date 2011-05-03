#ifndef LEXER_P_H
#define LEXER_P_H

#include "lexem.h"
#include "interfaces/lexemtype.h"
#include "statement.h"
#include "interfaces/error.h"
#include "abstractsyntaxtree/ast_variabletype.h"

#include <QtCore>

namespace KumirAnalizer {

struct LexerPrivate {
    class Lexer * q;

    /**
      * String "normalization"
      */
    void splitLineIntoLexems(const QString &text
                             , QList<Lexem*> & lexems
                             ) const;

    /**
      * SplitFStringList in the terms of Kumir 1.x
      */
    void groupLexemsByStatements(const QList<Lexem*> & lexems
                                 , QList<Statement> & statements
                                 ) const;

    static void initNormalizator(const QString &fileName);

    static QStringList keyWords;
    static QStringList operators;
    static QStringList typeNames;
    static QStringList constNames;
    static QStringList compounds;

    static QHash<QString,Shared::LexemType> kwdMap;
    static QHash<QString,AST::VariableBaseType> baseTypes;
    static QHash<QString,bool> boolConstantValues;
    static QSet<QString> arrayTypes;
    static QString retvalKeyword;
    static QRegExp rxCompound;
    static QRegExp rxKeyWords;
    static QRegExp rxConst;
    static QRegExp rxTypes;


};

}

#endif // LEXER_P_H
