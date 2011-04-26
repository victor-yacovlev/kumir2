#ifndef ANALIZER_P_H
#define ANALIZER_P_H

#include "lexem.h"
#include "statement.h"
#include "interfaces/lexemtype.h"
#include "interfaces/error.h"
#include "interfaces/lineprop.h"
#include "abstractsyntaxtree/ast.h"

#include <QtCore>



typedef AST::Data AST_Data;

namespace KumirAnalizer {

struct AnalizerPrivate
{

    explicit AnalizerPrivate(class Analizer *);
    ~AnalizerPrivate();

    enum AnalizeSubject {
        SubjWholeText,
        SubjAlgorhtitm,
        SubjStatements
    };

    class Analizer * q;
    class Lexer * lexer;
    class PDAutomata * pdAutomata;
    AST_Data * ast;

    QString sourceText;
    QList<Statement> statements;
    QList<Shared::Error> errors;
    QList<Shared::LineProp> lineProperties;
    QList<int> lineIndents;

    AnalizeSubject analizeSubject(const QList<Statement> & statements) const;
    AnalizeSubject analizeSubject(const QList<Lexem> & lexems, int startLineNo) const;
    AnalizeSubject analizeSubject(const QList<Shared::LexemType> & lexemTypes, int startLineNo) const;

    void doCompilation(const AnalizeSubject & whatToCompile);

};

extern AnalizerPrivate::AnalizeSubject operator * ( const AnalizerPrivate::AnalizeSubject &first
                                                  , const AnalizerPrivate::AnalizeSubject &second );


}

#endif // ANALIZER_P_H
