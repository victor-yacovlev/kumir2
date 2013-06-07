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

typedef QList<AST::StatementPtr> * LAS;

struct AnalizerPrivate
{

    explicit AnalizerPrivate(class KumirAnalizerPlugin * plugin, class Analizer *);
    ~AnalizerPrivate();

    std::vector<Shared::ActorInterface*> builtinModules;
    class Analizer * q;
    class Lexer * lexer;
    class PDAutomata * pdAutomata;
    class SyntaxAnalizer * analizer;
    AST::DataPtr ast;

    static QStringList AlwaysAvailableModulesName;

    void createModuleFromActor(const Shared::ActorInterface * actor, quint8 forcedId);
    QStringList gatherExtraTypeNames(const AST::ModulePtr currentModule) const;


    QStringList sourceText;
    QList<TextStatementPtr> statements;

    QString teacherText;
    int hiddenBaseLine;

    void removeAllVariables(const AST::VariablePtr var);

    void setHiddenBaseLine(int lineNo);
    void setHiddenText(const QString & text, int baseLineNo);

    static QLocale::Language nativeLanguage;



    /** Find algorhitm in AST by real line number */
    static AST::AlgorithmPtr findAlgorhitmByPos(AST::DataPtr data, int pos);

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
    static bool findInstructionsBlock(AST::DataPtr data
                                      , const QList<TextStatement*> statements
                                      , LAS & lst
                                      , int & begin
                                      , int & end
                                      , AST::ModulePtr & mod
                                      , AST::AlgorithmPtr & alg
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
    static bool findInstructionsBlock(AST::DataPtr data
                                      , const QList<TextStatement*> statements
                                      , int pos
                                      , LAS & lst
                                      , int & outPos
                                      , AST::ModulePtr & mod
                                      , AST::AlgorithmPtr & alg
                                      );

    enum CompilationStage {
        CS_StructureAndNames, CS_Contents
    };

    void doCompilation(QList<TextStatementPtr> & allStatements, CompilationStage stage);


    struct ModuleStatementsBlock {
        QList<TextStatementPtr> statements;
        TextStatementPtr begin;
        TextStatementPtr end;
        bool teacher;

        inline ModuleStatementsBlock(): teacher(false) {}
        inline operator bool() const { return statements.size() > 0; }
    };

    static QList<struct ModuleStatementsBlock> splitIntoModules(const QList<TextStatementPtr> & statements);

};


}

#endif // ANALIZER_P_H
