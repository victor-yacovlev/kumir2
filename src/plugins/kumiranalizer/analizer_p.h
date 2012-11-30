#ifndef ANALIZER_P_H
#define ANALIZER_P_H

#include "dataformats/lexem.h"
#include "statement.h"
#include "interfaces/lexemtype.h"
#include "interfaces/error.h"
#include "interfaces/lineprop.h"
#include "dataformats/ast.h"
#include "interfaces/analizerinterface.h"
#include "interfaces/actorinterface.h"

#include <QtCore>

using AST::Lexem;

typedef AST::Data AST_Data;

namespace KumirAnalizer {

typedef QList<AST::Statement*> * LAS;

struct AnalizerPrivate
{

    explicit AnalizerPrivate(class KumirAnalizerPlugin * plugin, class Analizer *);
    ~AnalizerPrivate();

    enum AnalizeSubject {
        SubjWholeText,
        SubjAlgorhtitm,
        SubjStatements
    };

    std::vector<Shared::ActorInterface*> builtinModules;
    class Analizer * q;
    class Lexer * lexer;
    class PDAutomata * pdAutomata;
    class SyntaxAnalizer * analizer;
    AST_Data * ast;

    static QString StandartFunctionsModuleName;

    void createModuleFromActor(const Shared::ActorInterface * actor, uint8_t forcedId);


    QStringList sourceText;
    QList<Statement*> statements;

    QString teacherText;
    QList<Statement*> teacherStatements;
    int hiddenBaseLine;

    void removeAllVariables(AST::Variable * var);

    void setHiddenBaseLine(int lineNo);
    void setHiddenText(const QString & text, int baseLineNo);

    static QLocale::Language nativeLanguage;

    void compileTransaction(const Shared::ChangeTextTransaction &changes);

    /** Find algorhitm in AST by real line number */
    static AST::Algorhitm * findAlgorhitmByPos(AST::Data * data, int pos);

    /** Find begin/end iterators in AST instructions list,
      * containing provided lexem groups
      * @param IN data - AST
      * @param IN statements - list of lexem groups
      * @param OUT lst - list of AST instructions
      * @param OUT begin - begin iterator
      * @param OUT end - end iterator
      * @param OUT mod - module reference
      * @param OUT alg - algorhitm reference
      * @return true on found, false if not found
      */
    static bool findInstructionsBlock(AST::Data * data
                                      , const QList<Statement*> statements
                                      , LAS & lst
                                      , int & begin
                                      , int & end
                                      , AST::Module* & mod
                                      , AST::Algorhitm* & alg
                                      );
    /** Find context of AST instructions list,
      * containing provided lexem groups
      * @param IN data - AST
      * @param IN statements - list of lexem groups
      * @param IN pos - position, where lexems inserted
      * @param OUT lst - list of AST instructions
      * @param OUT outPos - position, where statements to be insert
      * @param OUT mod - module reference
      * @param OUT alg - algorhitm reference
      * @return true on found, false if not found
      */
    static bool findInstructionsBlock(AST::Data * data
                                      , const QList<Statement*> statements
                                      , int pos
                                      , LAS & lst
                                      , int & outPos
                                      , AST::Module* & mod
                                      , AST::Algorhitm* & alg
                                      );

    AnalizeSubject analizeSubject(const QList<Statement*> & statements) const;


    void doCompilation(AnalizeSubject whatToCompile
                       , QList<Statement*> & oldStatements
                       , QList<Statement*> & newStatements
                       , QList<Statement*> & allStatements
                       , int whereInserted
                       );

};

extern AnalizerPrivate::AnalizeSubject operator * ( const AnalizerPrivate::AnalizeSubject &first
                                                   , const AnalizerPrivate::AnalizeSubject &second );


}

#endif // ANALIZER_P_H
