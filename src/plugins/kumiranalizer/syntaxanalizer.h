#ifndef SYNTAXANALIZER_H
#define SYNTAXANALIZER_H

#include <QtCore>
#include "statement.h"

#include "dataformats/ast.h"
#include "dataformats/ast_algorhitm.h"
#include "interfaces/analizerinterface.h"
#include "lexer.h"

typedef AST::Data AST_Data;
typedef AST::Algorhitm AST_Algorhitm;

namespace KumirAnalizer {

using namespace AST;
using namespace Shared;

struct VariablesGroup {
    Lexem * groupLexem;
    QList<Lexem*> lexems;
    AST::VariableAccessType access;
    bool accessDefined;
};

struct SubexpressionElement {
    inline SubexpressionElement(AST::Expression * ee) { e = ee; o = 0; }
    inline SubexpressionElement(Lexem * oo) { e = 0; o = oo; }
    AST::Expression * e;
    Lexem * o;
};


class SyntaxAnalizer : public QObject
{
    Q_OBJECT
public:
    explicit SyntaxAnalizer(class Lexer * lexer_,
                            const QStringList & alwaysEnabledModules_,
                            bool teacherMode,
                            QObject *parent = 0);
    void init(QList<Statement*> & statements_
              , AST_Data * ast_
              , AST_Algorhitm *algorhitm_);
    void syncStatements();
    QStringList unresolvedImports() const;
    void setSourceDirName(const QString & dirName);
    void buildTables(bool allowOperatorsDeclaration);
    QList<Shared::Suggestion> suggestAutoComplete(const Statement * statementBefore,
                                                  const QList<Lexem*> lexemsAfter,
                                                  const AST::Module * contextModule,
                                                  const AST::Algorhitm * contextAlgorithm
                                                  ) const;
    void processAnalisys();
    ~SyntaxAnalizer();
private /*fields*/:

    Lexer * lexer_;
    AST::Data * ast_;
    AST::Algorhitm * algorhitm_;
    QList<Statement> statements_;
    QSet<QString> unresolvedImports_;
    QStringList alwaysEnabledModules_;
    QString sourceDirName_;
    int currentPosition_;
    QList<Statement*> originalStatements_;
    bool teacherMode_;

private /*methods*/:
    void parseImport(int str);
    void parseModuleHeader(int str);
    void parseAlgHeader(int str, bool onlyName, bool allowOperatorsDeclaration);
    void parseVarDecl(int str);
    void parseAssignment(int str);
    void parseInput(int str);
    void parseAssertPrePost(int str);
    void parseOutput(int str);
    void parseOneLexemInstruction(int str);
    void parseEndLoop(int str);
    void parseIfCase(int str);
    void parseLoopBegin(int str);

    const Statement & findSourceStatementByLexem(const Lexem* lexem) const;

    QList<Shared::Suggestion> suggestAssignmentAutoComplete(
            const Statement *statementBefore,
            const QList<Lexem *> lexemsAfter,
            const AST::Module *contextModule,
            const AST::Algorhitm *contextAlgorithm
            ) const;

    QList<Shared::Suggestion> suggestInputOutputAutoComplete(
            const Statement *statementBefore,
            const QList<Lexem *> lexemsAfter,
            const AST::Module *contextModule,
            const AST::Algorhitm *contextAlgorithm
            ) const;

    QList<Shared::Suggestion> suggestConditionAutoComplete(
            const Statement *statementBefore,
            const QList<Lexem *> lexemsAfter,
            const AST::Module *contextModule,
            const AST::Algorhitm *contextAlgorithm
            ) const;

    QList<Shared::Suggestion> suggestLoopBeginAutoComplete(
            const Statement *statementBefore,
            const QList<Lexem *> lexemsAfter,
            const AST::Module *contextModule,
            const AST::Algorhitm *contextAlgorithm
            ) const;

    QList<Shared::Suggestion> suggestImportAutoComplete(
            const Statement *statementBefore,
            const QList<Lexem *> lexemsAfter
            ) const;

    QList<Shared::Suggestion> suggestExpressionAutoComplete(
            const QList<Lexem*> lexemsBefore,
            const QList<Lexem*> lexemsAfter,
            const AST::Module *contextModule,
            const AST::Algorhitm *contextAlgorithm,
            bool typeIsKnown,
            AST::Type baseType,
            unsigned int dimension,
            AST::VariableAccessType accessType,
            AST::ExpressionType expressionKind
            ) const;

    QList<Shared::Suggestion> suggestValueAutoComplete(
            const QList<Lexem*> lexemsBefore,
            const QList<Lexem*> lexemsAfter,
            const AST::Module *contextModule,
            const AST::Algorhitm *contextAlgorithm,
            bool typeIsKnown,
            AST::Type baseType,
            unsigned int minimumDimension,
            AST::VariableAccessType accessType
            ) const;

    QList<Shared::Suggestion> suggestOperandAutoComplete(
            const QList<Lexem*> lexemsBefore,
            const QList<Lexem*> lexemsAfter,
            const AST::Module * contextModule,
            const AST::Algorhitm * contextAlgorithm,
            const AST::Expression * leftExpression,
            const LexemType operatorr,
            const AST::Type & targetBaseType
            ) const;

    bool tryInputOperatorAlgorithm(
            const QString & lexem,
            AST::Type & type,
            QVariant & constantValue
            ) const;
    bool findConversionAlgorithm(const AST::Type & from
                                 , const AST::Type & to
                                 , AST::Module * & mod
                                 , AST::Algorhitm * & alg) const;
    AST::Expression * makeCustomBinaryOperation(const QString & operatorName
                            , AST::Expression * leftExpression
                            , AST::Expression * rightExpression
                            ) const;
    template <typename TOut>
    TOut makeCustomUnaryOperation(const QString & operatorName
                                               , AST::Expression * argument) const;
    bool findAlgorhitm(const QString &name
                       , const AST::Module*  module
                       , AST::Algorhitm* & algorhitm_) const;
    bool findGlobalVariable(const QString &name, const AST::Module *module, AST::Variable* & var) const;
    bool findLocalVariable(const QString &name
                           , const AST::Algorhitm * alg
                           , AST::Variable* & var) const;
    AST::Expression * parseExpression(QList<Lexem*> lexems
                                      , const AST::Module * mod
                                      , const AST::Algorhitm * alg) const;
    bool findVariable(const QString &name
                      , const AST::Module * module
                      , const AST::Algorhitm * algorhitm_
                      , AST::Variable* & var) const;
    bool findUserType(const QString & name, AST::Type &type, AST::Module * module) const;
    QList<AST::Variable*> parseVariables(int statementIndex, VariablesGroup & group,
                                         AST::Module * mod,
                                         AST::Algorhitm * alg, bool algHeader);
    QVariant parseConstant(const std::list<Lexem*> &constant
                           , const AST::VariableBaseType pt
                           , int& maxDim
                           ) const;
    AST::VariableBaseType testConst(const std::list<Lexem *> & lxs, int &err) const;
    QVariant createConstValue(const QString &str, const AST::VariableBaseType type) const;
    AST::Expression * parseFunctionCall(const QList<Lexem*> &lexems, const AST::Module * mod, const AST::Algorhitm * alg) const;

    AST::Expression * parseSimpleName(const std::list<Lexem*> &lexems, const AST::Module * mod, const AST::Algorhitm * alg) const;
    void updateSliceDSCall(AST::Expression * expr, AST::Variable * var) const;
    AST::Expression * parseElementAccess(const QList<Lexem*> &lexems, const AST::Module * mod, const AST::Algorhitm * alg) const;
    AST::Expression * makeExpressionTree(const QList<SubexpressionElement> & s) const;
    template <typename List1, typename List2>
    inline static void splitLexemsByOperator(
            const List1 &s
            , const LexemType & op
            , List2 & result
            , List1 & comas
            )
    {
        result.clear();
        comas.clear();
        int deep = 0;
        if (s.size()>0)
            result.push_back(List1());
        typename List1::const_iterator it;
        for (it=s.begin(); it!=s.end(); ++it) {
            if ( (*it)->type==op && deep==0 ) {
                result.push_back(List1());
                comas.push_back((*it));
            }
            else {
                if ( (*it)->type==LxOperLeftBr || (*it)->type==LxOperLeftSqBr || (*it)->type==LxOperLeftBrace )
                    deep++;
                else if ( (*it)->type==LxOperRightBr || (*it)->type==LxOperRightSqBr || (*it)->type==LxOperRightBrace )
                    deep--;
                result.back().push_back(*it);
            }
        }
    }
    static Lexem * findLexemByType(const QList<Lexem*> lxs, LexemType type);    

};

} // namespace KumirAnalizer

#endif // SYNTAXANALIZER_H
