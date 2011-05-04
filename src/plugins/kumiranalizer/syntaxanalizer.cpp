#include "syntaxanalizer.h"
#include "lexer.h"

#include "abstractsyntaxtree/ast_variable.h"
#include "errormessages/errormessages.h"

#define BADNAME_KEYWORD TN_BAD_NAME_3
#define BADNAME_OPERATOR TN_BAD_NAME_1

using namespace Shared;

namespace KumirAnalizer {

struct VariablesGroup {
    QList<Lexem*> lexems;
    AST::VariableAccessType access;
};

struct SubexpressionElement {
    inline SubexpressionElement(AST::Expression * ee) { e = ee; o = 0; }
    inline SubexpressionElement(Lexem * oo) { e = 0; o = oo; }
    AST::Expression * e;
    Lexem * o;
};

struct SyntaxAnalizerPrivate
{
    Lexer * lexer;
    AST::Data * ast;
    AST::Algorhitm * algorhitm;
    QList<Statement> * statements;
    QSet<QString> unresolvedImports;

    void parseImport(int str);
    void parseModuleHeader(int str);
    void parseAlgHeader(int str);
    void parseVarDecl(int str);
    void parseAssignment(int str);
    void parseInputOutputAssertPrePost(int str);
    void parseOneLexemInstruction(int str);
    void parseEndLoop(int str);
    void parseIfCase(int str);
    void parseLoopBegin(int str);

    bool findAlgorhitm(const QString &name
                       , const AST::Module*  module
                       , AST::Algorhitm* & algorhitm);
    bool findGlobalVariable(const QString &name, const AST::Module *module, AST::Variable* & var);
    bool findLocalVariable(const QString &name
                           , const AST::Algorhitm * alg
                           , AST::Variable* & var);
    AST::Expression * parseExpression(QList<Lexem*> lexems
                                      , const AST::Module * mod
                                      , const AST::Algorhitm * alg);
    bool findVariable(const QString &name
                      , const AST::Module * module
                      , const AST::Algorhitm * algorhitm
                      , AST::Variable* & var);
    QList<AST::Variable*> parseVariables(VariablesGroup & group,
                                         AST::Module * mod,
                                         AST::Algorhitm * alg);
    QVariant parseConstant(const QList<Lexem*> &constant
                           , const AST::VariableBaseType pt
                           , bool array
                           , int& maxDim
                           ) const;
    AST::VariableBaseType testConst(const Lexem * lx, int &err) const;
    QVariant createConstValue(const Lexem * lx, const AST::VariableBaseType type) const;
    AST::Expression * parseFunctionCall(const QList<Lexem*> &lexems, const AST::Module * mod, const AST::Algorhitm * alg);

    AST::Expression * parseSimpleName(const QList<Lexem*> &lexems, const AST::Module * mod, const AST::Algorhitm * alg);
    AST::Expression * parseElementAccess(const QList<Lexem*> &lexems, const AST::Module * mod, const AST::Algorhitm * alg);
    AST::Expression * makeExpressionTree(const QList<SubexpressionElement> & s);
    static void splitLexemsByComas(const QList<Lexem *> &s
                                   , QList< QList<Lexem*> > & result
                                   , QList< Lexem* > & comas);
    static Lexem * findLexemByType(const QList<Lexem*> lxs, LexemType type);


};

SyntaxAnalizer::SyntaxAnalizer(Lexer * lexer, QObject *parent) :
    QObject(parent)
{
    d = new SyntaxAnalizerPrivate;
    d->ast = 0;
    d->algorhitm = 0;
    d->statements = 0;
    d->lexer = lexer;
}

void SyntaxAnalizer::init(QList<Statement> *statements, AST_Data *ast, AST_Algorhitm *algorhitm)
{
    d->ast = ast;
    d->algorhitm = algorhitm;
    d->statements = statements;
    d->unresolvedImports.clear();
}

void SyntaxAnalizerPrivate::splitLexemsByComas(const QList<Lexem *> &s
                                               , QList< QList<Lexem*> > & result
                                               , QList< Lexem* > & comas)
{
    result.clear();
    comas.clear();
    int deep = 0;
    if (!s.isEmpty())
        result << QList<Lexem*>();
    for (int i=0; i<s.size(); i++) {
        if (s[i]->type==LxOperComa && deep==0) {
            result << QList<Lexem*>();
            comas << s[i];
        }
        else {
            if (s[i]->type==LxOperLeftBr || s[i]->type==LxOperLeftSqBr)
                deep++;
            if (s[i]->type==LxOperRightBr || s[i]->type==LxOperRightSqBr)
                deep--;
            result.last() << s[i];
        }
    }
}

void SyntaxAnalizer::buildTables()
{
    if (d->algorhitm)
        return; // Nothing to build if we analize just one algorhitm

    for (int i=0; i<d->statements->size(); i++) {
        const Statement & st = d->statements->at(i);
        if (st.type==LxPriModule) {
            d->parseModuleHeader(i);
        }
        if (st.type==LxPriImport) {
            d->parseImport(i);
        }
    }

    for (int i=0; i<d->ast->modules.size(); i++) {
        if (d->ast->modules[i]->header.type==AST::ModTypeUser)
            d->ast->modules[i]->header.enabled = true;
        if (d->ast->modules[i]->header.enabled) {
            d->unresolvedImports.unite(d->ast->modules[i]->header.uses);
        }
    }

    forever {
        bool nextBreak = true;
        for (int i=0; i<d->ast->modules.size(); i++) {
            AST::Module * mod = d->ast->modules[i];
            if (d->unresolvedImports.contains(mod->header.name)) {
                d->unresolvedImports.remove(mod->header.name);
                if (!mod->header.enabled) {
                    nextBreak = false;
                    d->unresolvedImports.unite(mod->header.uses);
                }
                mod->header.enabled = true;
            }
        }
        if (nextBreak)
            break;
    }

    for (int i=0; i<d->statements->size(); i++) {
        const Statement & st = d->statements->at(i);
        if (st.type==LxPriAlgHeader) {
            d->parseAlgHeader(i);
        }
        else if (st.type==LxNameClass) {
            d->parseVarDecl(i);
        }
    }
}

void SyntaxAnalizer::processAnalisys()
{
    for (int i=0; i<d->statements->size(); i++) {
        const Statement & st = d->statements->at(i);
        if (st.type==LxPriAssign) {
            d->parseAssignment(i);
        }
        else if (st.type==LxPriInput
                 || st.type==LxPriFinput
                 || st.type==LxPriOutput
                 || st.type==LxPriFoutput
                 || st.type==LxPriAssert
                 || st.type==LxPriPre
                 || st.type==LxPriPost
                 )
        {
            d->parseInputOutputAssertPrePost(i);
        }
        else if (st.type==LxPriEndModule
                 ||st.type==LxPriAlgBegin
                 ||st.type==LxPriAlgEnd
                 ||st.type==LxPriThen
                 ||st.type==LxPriElse
                 ||st.type==LxPriFi
                 ||st.type==LxPriSwitch
                 ||st.type==LxPriCase
                 ||st.type==LxPriExit
                 )
        {
            d->parseOneLexemInstruction(i);
        }
        else if (st.type==LxPriEndLoop) {
            d->parseEndLoop(i);
        }
        else if (st.type==LxPriIf) {
            d->parseIfCase(i);
        }
        else if (st.type==LxPriLoop) {
            d->parseLoopBegin(i);
        }
    }
}

void SyntaxAnalizerPrivate::parseImport(int str)
{
    if (statements->at(str).hasError())
        return;
    Statement * st = &( (*statements)[str] );
    if (st->data.size()<2) {
        Q_ASSERT(st->data.size()==1);
        st->data[0]->error = _("No module name");
        return;
    }
    if (st->data.size()>2) {
        for (int i=2; i<st->data.size(); i++) {
            st->data[i]->error = _("Garbage afrer module name");
        }
        return;
    }
    if (st->data[1]->data.isEmpty()) {
        st->data[1]->error = _("No module name");
        return;
    }
    QString localError = lexer->testName(st->data[1]->data);
    if (localError.size()>0) {
        st->data[1]->error = localError;
        return;
    }
    AST::Module * mod = st->mod;
    Q_CHECK_PTR(mod);
    st->data[1]->type = LxNameModule;
    mod->header.uses.insert(st->data[1]->data.simplified());
}

void SyntaxAnalizerPrivate::parseModuleHeader(int str)
{
    if (statements->at(str).hasError())
        return;
    Statement * st = &( (*statements)[str] );
    if (st->data.size()<2) {
        Q_ASSERT(st->data.size()==1);
        st->data[0]->error = _("No module name");
        return;
    }
    if (st->data.size()>2) {
        for (int i=2; i<st->data.size(); i++) {
            st->data[i]->error = _("Garbage afrer module name");
        }
        return;
    }
    if (st->data[1]->data.isEmpty()) {
        st->data[1]->error = _("No module name");
        return;
    }
    QString localError = lexer->testName(st->data[1]->data);
    if (localError.size()>0) {
        st->data[1]->error = localError;
        return;
    }
    AST::Module * mod = st->mod;
    Q_CHECK_PTR(mod);
    st->data[1]->type = LxNameModule;
    mod->header.name = st->data[1]->data.simplified();
}


void SyntaxAnalizerPrivate::parseVarDecl(int str)
{
    Statement * st = &( (*statements)[str] );
    if (st->hasError())
        return;
    AST::Algorhitm * alg = st->alg;
    AST::Module * mod = st->mod;
    VariablesGroup group;
    group.access = AST::AccessRegular;
    for (int i=0; i<st->data.size(); i++) {
        group.lexems << st->data[i];
    }
    QList<AST::Variable*> vars = parseVariables(group, mod, alg);
    QString error;
    for (int i=0; i<group.lexems.size()-1; i++) {
        if (group.lexems[i]->error.size()>0) {
            error = group.lexems[i]->error;
            break;
        }
    }
    if (!error.isEmpty()) {
        st->statement->type = AST::StError;
        st->statement->error = error;
    }
    else {
        st->statement->type = AST::StVarInitialize;
        st->statement->variables = vars;
    }
    if (alg) {
        alg->impl.locals.append(vars);
    }
    else {
        mod->impl.globals.append(vars);
    }
}

void SyntaxAnalizerPrivate::parseInputOutputAssertPrePost(int str)
{
    Statement * st = &( (*statements)[str] );
    if (st->hasError()) {
        return;
    }
    if (st->data.size()==1) {
        st->data[0]->error = _("No expressions after '%1'", st->data[0]->data);
        return;
    }
    if (st->data.last()->type==LxOperComa) {
        st->data.last()->error = _("Statement ends with coma");
        return;
    }
    QList< QList<Lexem*> > groups;
    QList<Lexem*> comas;
    splitLexemsByComas(st->data.mid(1), groups, comas);

    enum GroupType {
        Undefined,
        FileHandle,
        InputExpression,
        OutputExpression,
        AssertionExpression
    } groupType = Undefined;

    for (int i=0 ; i<groups.size(); i++) {

        if (i==0 && (st->type==LxPriFinput || st->type==LxPriFoutput))
            groupType = FileHandle;
        else if (st->type==LxPriInput || st->type==LxPriFinput)
            groupType = InputExpression;
        else if (st->type==LxPriOutput || st->type==LxPriFoutput)
            groupType = OutputExpression;

        AST::Expression * expr = parseExpression(groups[i], st->mod, st->alg);
        if (!expr) {
            return;
        }
        QString err;
        if (groupType==FileHandle) {
            // Expression is a file ID
            if (expr->baseType!=AST::TypeInteger) {
                err = _("File handle is not integer");
            }
        }
        else if (groupType==InputExpression) {
            // Expression is input variable

            if (expr->kind==AST::ExprVariable && expr->variable->dimension>0) {
                err = _("Can't input array");
            }
            if (expr->kind==AST::ExprConst) {
                err = _("Can't input constant value");
            }
            if (expr->kind==AST::ExprSubexpression) {
                err = _("Can't input complex expression");
            }
            if (expr->kind==AST::ExprArrayElement && expr->operands.size()>1) {
                err = _("Can't input string slice");
            }
            if (expr->kind==AST::ExprVariable && expr->variable->accessType==AST::AccessArgumentIn) {
                err = _("Can't input algorhitm in- argument");
            }
        }
        else if (groupType==OutputExpression) {
            // Expression is output variable -- nothing special to check
        }
        else if (groupType==AssertionExpression) {
            // Expression is assertion in anywhere

            if (expr->baseType!=AST::TypeBoolean) {
                err = _("Condition is not boolean");
            }
        }

        if (!err.isEmpty()) {
            for (int a=0; a<groups[i].size(); a++) {
                groups[i][a]->error = err;
            }
            delete expr;
            return;
        }
        st->statement->expressions << expr;
    }
}

void SyntaxAnalizerPrivate::parseOneLexemInstruction(int str)
{
    Statement * st = &( (*statements)[str] );
    if (st->hasError()) {
        return;
    }
    for (int i=1; i<st->data.size(); i++) {
        st->data[i]->error = _("Garbage at end of line");
    }
}

void SyntaxAnalizerPrivate::parseEndLoop(int str)
{
    Statement * st = &( (*statements)[str] );
    if (st->hasError()) {
        return;
    }
    if (st->data.size()>1 && st->data[1]->type!=LxSecIf) {
        for (int i=1; i<st->data.size(); i++) {
            st->data[i]->error = _("Garbage at end of line");
        }
        return;
    }
    if (st->data.size()==2 && st->data[1]->type==LxSecIf) {
        st->data[1]->error = _("No condition after 'end if'");
        return;
    }
    AST::Expression * expr = parseExpression(st->data.mid(2), st->mod, st->alg);
    if (expr) {
        if (expr->baseType!=AST::TypeBoolean) {
            for (int i=2; i<st->data.size(); i++) {
                st->data[i]->error = _("Condition is not boolean");
            }
            delete expr;
        }
        else {
            st->statement->loop.endCondition = expr;
        }
    }
}

void SyntaxAnalizerPrivate::parseIfCase(int str)
{
    Statement * st = &( (*statements)[str] );
    if (st->hasError()) {
        return;
    }
    if (st->type==LxPriIf && st->data.size()==1) {
        st->data[0]->error = _("No condition after 'if'");
        return;
    }
    if (st->type==LxPriCase && st->data.size()==1) {
        st->data[0]->error = _("No condition after 'case'");
        return;
    }
    if (st->type==LxPriCase && st->data.size()==2 && st->data[1]->type==LxOperColon) {
        st->data[0]->error = _("No condition between 'case' and colon");
        st->data[1]->error = _("No condition between 'case' and colon");
        return;
    }
    if (st->type==LxPriCase && st->data.last()->type!=LxOperColon) {
        for (int i=0; i<st->data.size(); i++) {
            st->data[i]->error = _("No colon after condition");
        }
        return;
    }
    QList<Lexem*> cond;
    if (st->type==LxPriCase)
        cond = st->data.mid(1, st->data.length()-2);
    else
        cond = st->data.mid(1);
    AST::Expression * expr = parseExpression(cond, st->mod, st->alg);
    if (expr) {
        if (expr->baseType!=AST::TypeBoolean) {
            for (int i=0; i<cond.size(); i++) {
                cond[i]->error = _("Condition if not boolean");
            }
            delete expr;
        }
        else {
            st->statement->conditionals[st->conditionalIndex].condition = expr;
        }
    }
}

void SyntaxAnalizerPrivate::parseLoopBegin(int str)
{
    Statement * st = &( (*statements)[str] );
    if (st->hasError()) {
        return;
    }
    AST::LoopType type = AST::LoopWhile;
    if (st->data.size()==1) {
        type = AST::LoopWhile;
    }
    else if (st->data.size()>1 && st->data[1]->type==LxSecFor) {
        type = AST::LoopFor;
    }
    else if (st->data.size()>1 && st->data[1]->type==LxSecWhile) {
        type = AST::LoopWhile;
    }
    else if (findLexemByType(st->data, LxSecTimes)) {
        type = AST::LoopTimes;
    }
    else {
        st->data[0]->error = _("Loop type not specified");
        return;
    }
    if (type==AST::LoopFor) {
        QList<Lexem*> forr;
        QList<Lexem*> from;
        QList<Lexem*> to;
        QList<Lexem*> step;
        Lexem * forLexem = st->data[1];
        Lexem * fromLexem = findLexemByType(st->data, LxSecFrom);
        Lexem * toLexem = findLexemByType(st->data, LxSecTo);
        Lexem * stepLexem = findLexemByType(st->data, LxSecStep);
        if (!fromLexem) {
            forLexem->error = _("No loop variable");
            return;
        }
        if (!fromLexem) {
            forLexem->error = _("No loop 'from' value");
            return;
        }
        if (!toLexem) {
            forLexem->error = _("No loop 'to' value");
        }
        int forIndex = st->data.indexOf(forLexem);
        int fromIndex = st->data.indexOf(fromLexem);
        int toIndex = st->data.indexOf(toLexem);
        int stepIndex = st->data.indexOf(stepLexem);
        if (stepIndex==-1) {
            stepIndex = st->data.size();
        }
        for (int i=forIndex+1; i<fromIndex; i++ ) {
            forr << st->data[i];
        }
        for (int i=fromIndex+1; i<toIndex; i++) {
            from << st->data[i];
        }
        for (int i=toIndex+1; i<stepIndex; i++) {
            to << st->data[i];
        }
        for (int i=stepIndex+1; i<st->data.size(); i++) {
            step << st->data[i];
        }
        if (forr.isEmpty()) {
            forLexem->error = _("No for-loop variable");
            return;
        }
        if (from.isEmpty()) {
            fromLexem->error = _("From-value not specified");
            return;
        }
        if (to.isEmpty()) {
            toLexem->error = _("Then-value not specified");
            return;
        }
        if (step.isEmpty() && stepLexem) {
            stepLexem->error = _("Step-value is empty");
        }
        QString name;
        for (int i=0; i<forr.size(); i++) {
            if (i>0) name += " "; name += forr[i]->data;
        }
        if (findLocalVariable(name, st->alg, st->statement->loop.forVariable)) {

        }
        else if (findGlobalVariable(name, st->mod, st->statement->loop.forVariable)) {

        }
        else if (forr.size()==1 && forr[0]->type & LxTypeConstant) {
            foreach (Lexem *l, forr) l->error = _("Constant for-loop variable");
            return;
        }
        else {
            foreach (Lexem *l, forr) l->error = _("Variable not found");
            return;
        }
        if (st->statement->loop.forVariable->baseType!=AST::TypeInteger
                || st->statement->loop.forVariable->dimension>0) {
            foreach (Lexem *l, forr) l->error = _("Not integer for-loop variable");
            return;
        }
        AST::Expression * fromExpr = parseExpression(from, st->mod, st->alg);
        if (!fromExpr) {
            return;
        }
        else if (fromExpr->baseType!=AST::TypeInteger) {
            foreach (Lexem *l, from) l->error = _("Not integer from-value");
            delete fromExpr;
            return;
        }
        AST::Expression * toExpr = parseExpression(to, st->mod, st->alg);
        if (!toExpr) {
            delete fromExpr;
            return;
        }
        else if (toExpr->baseType!=AST::TypeInteger) {
            foreach (Lexem *l, to) l->error = _("Not integer to-value");
            delete fromExpr;
            delete toExpr;
            return;
        }
        AST::Expression * stepExpr = stepLexem? parseExpression(step, st->mod, st->alg) : 0;
        if (stepLexem && !stepExpr) {
            delete fromExpr;
            delete toExpr;
            return;
        }
        else if (stepExpr && stepExpr->baseType!=AST::TypeInteger) {
            foreach (Lexem *l, step) l->error = _("Not integer step-value");
            delete fromExpr;
            delete toExpr;
            delete stepExpr;
            return;
        }
        st->statement->loop.fromValue = fromExpr;
        st->statement->loop.toValue = toExpr;
        st->statement->loop.stepValue = stepExpr;

    } // end if (type==AST::LoopFor)
    else if (type==AST::LoopTimes) {
        QList<Lexem*> times;
        bool timesFound = false;
        bool err = false;
        for (int i=1; i<st->data.size(); i++) {
            if (st->data[i]->type==LxSecTimes) {
                timesFound = true;
                if (times.isEmpty()) {
                    err = true;
                    st->data[i]->error = _("Times value not specified");
                    break;
                }
            }
            else {
                if (timesFound) {
                    err = true;
                    st->data[i]->error = _("Garbage at the end of statement");
                }
                else {
                    times << st->data[i];
                }
            }
        }
        if (err)
            return;
        AST::Expression * timesExpr = parseExpression(times, st->mod, st->alg);
        if (!timesExpr)
            return;
        else if (timesExpr->baseType!=AST::TypeInteger) {
            foreach (Lexem * l, times) l->error = _("Not integer times-value");
            delete timesExpr;
            return;
        }
        st->statement->loop.timesValue = timesExpr;
    } // end if (type==AST::LoopTimes)
    else if (type==AST::LoopWhile) {
        if (st->data.size()==1) {
            // Forever loop
            AST::Expression * foreverr = new AST::Expression;
            foreverr->baseType = AST::TypeBoolean;
            foreverr->kind = AST::ExprConst;
            foreverr->constant = QVariant(true);
            st->statement->loop.whileCondition = foreverr;
        }
        else {
            // While condition loop
            QList<Lexem*> cond = st->data.mid(2);
            if (cond.isEmpty()) {
                st->data[1]->error = _("No condition after 'loop while'");
                return;
            }
            AST::Expression * condExpr = parseExpression(cond, st->mod, st->alg);
            if (!condExpr)
                return;
            else if (condExpr->baseType!=AST::TypeInteger) {
                foreach (Lexem * l, cond) l->error = _("Condition is not boolean");
                delete condExpr;
                return;
            }
            st->statement->loop.whileCondition = condExpr;
        }
    } // end if (type==AST::LoopWhile)
}

void SyntaxAnalizerPrivate::parseAssignment(int str)
{
    Statement * st = &( (*statements)[str] );
    if (st->hasError()) {
        return;
    }
    AST::Algorhitm * alg = st->alg;
    AST::Module * mod = st->mod;
    Q_CHECK_PTR(mod);
    QList<Lexem*> left;
    QList<Lexem*> right;
    Lexem * assignOp = 0;
    int assignCount = 0;
    for (int i=0; i<st->data.size(); i++) {
        if (st->data[i]->type==LxPriAssign) {
            if (assignCount==1) {
                st->data[i]->error = _("Too many ':=' operators");
                return;
            }
            assignCount++;
            assignOp = st->data[i];
        }
        else {
            if (assignCount==0) {
                left << st->data[i];
            }
            else {
                right << st->data[i];
            }
        }
    }
    if (assignCount==1) {
        if (left.isEmpty()) {
            assignOp->error = _("No left value of assignment");
            return;
        }
        if (right.isEmpty()) {
            assignOp->error = _("No right value of assignment");
            return;
        }
    }
    else {
        right = left;
        left.clear();
    }
    AST::Expression * rightExpr = parseExpression(right, mod, alg);
    if (!rightExpr)
        return;
    AST::Expression * leftExpr = 0;
    if (!left.isEmpty()) {
        leftExpr = parseExpression(left, mod, alg);
        if (!leftExpr) {
            delete rightExpr;
            return;
        }
        if (leftExpr->kind==AST::ExprFunctionCall || leftExpr->kind==AST::ExprSubexpression) {
            assignOp->error = _("Assignment to complex expression");
            delete rightExpr;
            delete leftExpr;
            return;
        }
        if (leftExpr->kind==AST::ExprConst) {
            assignOp->error = _("Assignment to constant value");
            delete rightExpr;
            delete leftExpr;
            return;
        }
        if ( (leftExpr->kind==AST::ExprVariable) && (leftExpr->variable->dimension>0) ) {
            assignOp->error = _("Assignment to array");
            delete rightExpr;
            delete leftExpr;
            return;
        }
        if ( (rightExpr->kind==AST::ExprVariable) && (rightExpr->variable->dimension>0) ) {
            assignOp->error = _("Assignment of array");
            delete rightExpr;
            delete leftExpr;
            return;
        }
        if ( (leftExpr->kind==AST::ExprVariable) && (leftExpr->variable->accessType==AST::AccessArgumentIn) ) {
            assignOp->error = _("Assignment to in- argument");
            delete rightExpr;
            delete leftExpr;
            return;
        }
        QString err;
        if (leftExpr->baseType!=rightExpr->baseType) {
            AST::VariableBaseType a = leftExpr->baseType;
            AST::VariableBaseType b = rightExpr->baseType;
            if (a==AST::TypeInteger) {
                if (b==AST::TypeReal) {
                    err = _("Can't integer:=real");
                }
                else if (b==AST::TypeCharect) {
                    err = _("Can't integer:=charect");
                }
                else if (b==AST::TypeString) {
                    err = _("Can't integer:=string");
                }
                else if (b==AST::TypeBoolean) {
                    err = _("Can't integer:=boolean");
                }
            }
            else if (a==AST::TypeReal) {
                if (b==AST::TypeCharect) {
                    err = _("Can't real:=charect");
                }
                else if (b==AST::TypeString) {
                    err = _("Can't real:=string");
                }
                else if (b==AST::TypeBoolean) {
                    err = _("Can't real:=boolean");
                }
            }
            else if (a==AST::TypeCharect) {
                if (b==AST::TypeBoolean) {
                    err = _("Can't charect:=boolean");
                }
                else if (b==AST::TypeInteger) {
                    err = _("Can't charect:=integer");
                }
                else if (b==AST::TypeReal) {
                    err = _("Can't charect:=real");
                }
                else if (b==AST::TypeString) {
                    err = _("Can't charect:=string");
                }
            }
            else if (a==AST::TypeBoolean) {
                if (b==AST::TypeInteger) {
                    err = _("Can't boolean:=integer");
                }
                else if (b==AST::TypeReal) {
                    err = _("Can't boolean:=real");
                }
                else if (b==AST::TypeCharect) {
                    err = _("Can't boolean:=charect");
                }
                else if (b==AST::TypeString) {
                    err = _("Can't boolean:=string");
                }
            }
            else if (a==AST::TypeString) {
                if (b==AST::TypeBoolean) {
                    err = _("Can't string:=boolean");
                }
                else if (b==AST::TypeInteger) {
                    err = _("Can't string:=integer");
                }
                else if (b==AST::TypeReal) {
                    err = _("Can't string:=real");
                }
            }
        } // if (leftExpr->type!=rightExpr->type)
        if (!err.isEmpty()) {
            assignOp->error = err;
            delete leftExpr;
            delete rightExpr;
            return;
        }
    }
    st->statement->type = AST::StAssign;
    st->statement->expressions << rightExpr;
    if (leftExpr)
        st->statement->expressions << leftExpr;
}

void SyntaxAnalizerPrivate::parseAlgHeader(int str)
{
    Statement * st = &( (*statements)[str] );
    if (st->hasError())
        return;
    AST::Algorhitm * alg = st->alg;
    AST::Module * mod = st->mod;
    Q_CHECK_PTR(alg);
    Q_CHECK_PTR(mod);
    QString name;
    bool isFirst = mod->header.name.isEmpty() && mod->impl.algorhitms.indexOf(alg)==0;
    int argsStartLexem = -1;
    int nameStartLexem = 1;
    if (st->data.size()>1 && st->data[1]->type==LxNameClass) {
        nameStartLexem = 2;
        alg->header.returnType = lexer->baseTypeByClassName(st->data[1]->data);
        if (lexer->isArrayClassName(st->data[1]->data)) {
            st->data[1]->error = _("Algorhitms can't return array");
            return;
        }

    }
    for (int i=nameStartLexem; i<st->data.size(); i++) {
        if (st->data[i]->type == LxTypeName) {
            if (i>nameStartLexem)
                name += " ";
            name += st->data[i]->data;
            st->data[i]->type=LxNameAlg;
        }
        else if (st->data[i]->type == LxOperLeftBr ) {
            argsStartLexem = i+1;
            break;
        }
        else if (st->data[i]->type==LxNameClass
                 || st->data[i]->type & LxTypePrimaryKwd
                 || st->data[i]->type & LxTypeSecondaryKwd)
        {
            st->data[i]->error = _("Keyword in name");
            return;
        }
        else {
            st->data[i]->error = _("Operator in name");
            return;
        }
    }

    if ( isFirst )
    {
        if ( name.isEmpty() )
        {
            if ( argsStartLexem!=-1 )
            {
                for (int i=argsStartLexem-1; i<st->data.size(); i++)
                    st->data[i]->error = _("Extra arguments");
                return;
            }
            if ( alg->header.returnType!=AST::TypeNone )
            {
                st->data[0]->error = _("First unnamed algorhitm should not return anything");
                return;
            }
        }
    }
    else
    {
        // если алгоритм не первый, обязательно должно быть имя, иначе -- ошибка
        if ( name.isEmpty() )
        {
            for (int i=0; i<st->data.size(); i++) {
                st->data[i]->error = _("No algorhitm name");
            }
            return ;
        }
    }

    // Проверяем на повторное описание алгоритма
    AST::Algorhitm * aa;
    if (findAlgorhitm(name,st->mod,aa))
    {
        for (int i=1; i<st->data.size(); i++) {
            if (st->data[i]->type==LxNameAlg)
                st->data[i]->error = _("The name is used by other algorhitm");
        }
        return ;
    }

    // Проверяем на наличие переменной с таким же именем

    AST::Variable * vv;
    if (findGlobalVariable(name, st->mod, vv)) {
        for (int i=1; i<st->data.size(); i++) {
            if (st->data[i]->type==LxNameAlg)
                st->data[i]->error = _("The name is used by global variable");
        }
        return ;
    }

    if (alg->header.returnType!=AST::TypeNone) {
        AST::Variable * retVar = new AST::Variable;
        retVar->accessType = AST::AccessRegular;
        retVar->baseType = alg->header.returnType;
        retVar->name = name;
        alg->impl.locals << retVar;
    }



    // Заносим алгоритм в таблицу функций
    alg->header.name = name;

    // Make this algorhitm public available (if not private name)
    if (!name.isEmpty() && !name.startsWith("_")) {
        mod->header.algorhitms << alg;
    }

    // Если нет аргументов, то всё

    if ( argsStartLexem==-1 ) {
        return;
    }

    // =============== � АЗБО�  А� ГУМЕНТОВ


    QList<VariablesGroup> groups;
    if (st->data.last()->type!=LxOperRightBr) {
        st->data[0]->error= _("Unpaired '('");
        return;
    }
    VariablesGroup currentGroup;
    currentGroup.access = AST::AccessArgumentIn;
    for (int i=argsStartLexem; i<st->data.size()-1; i++) {
        LexemType type = st->data[i]->type;
        if (type==LxSecIn || type==LxSecOut || type==LxSecInout) {
            if (currentGroup.lexems.last()->type==LxOperComa) {
                currentGroup.lexems.pop_back();
            }
            else if (!currentGroup.lexems.isEmpty()) {
                st->data[i]->error = _("No coma before %1", st->data[i]->data);
                return;
            }
            if (type==LxSecIn)
                currentGroup.access = AST::AccessArgumentIn;
            else if (type==LxSecOut)
                currentGroup.access = AST::AccessArgumentOut;
            else if (type==LxSecInout)
                currentGroup.access = AST::AccessArgumentInOut;
            if (!currentGroup.lexems.isEmpty()) {
                groups << currentGroup;
                currentGroup.lexems.clear();
            }
        }
        else if (type==LxPriAlgHeader) {
            st->data[i]->error = _("'arg' instead of 'alg'");
            return;
        }
        else {
            currentGroup.lexems << st->data[i];
        }
    }
    if (!currentGroup.lexems.isEmpty())
        groups << currentGroup;

    QString localError;

    for (int i=0; i<groups.size(); i++) {
        QList<AST::Variable*> vars = parseVariables(groups[i], st->mod, st->alg);
        for (int j=0; j<vars.size(); j++) {
            alg->header.arguments << vars[j];
            alg->impl.locals << vars[j];
        }
        for (int j=0; j<groups[i].lexems.size()-1; j++) {
            if (!groups[i].lexems[j]->error.isEmpty()) {
                localError = groups[i].lexems[j]->error;
                break;
            }
        }
    }

    alg->header.error = localError;
}

QList<AST::Variable*> SyntaxAnalizerPrivate::parseVariables(VariablesGroup &group, AST::Module *mod, AST::Algorhitm *alg)
{
    //  Pежим работы автомата-парсера.
    //  type -- разбор типа
    //  name -- разбор имени переменной
    //  tn -- разбор типа или имени
    //  tab -- разбор массива
    //  lbN -- разбор левой границы
    //  lbN -- разбор правой границы
    //  value -- разбор константного значения

    enum Parcing { type, name, tn, lb, rb, value };

    //  Состояния автомата
    Parcing par = type; // режим разбора
    AST::VariableBaseType cType = AST::TypeNone; // тип текущей величины
    bool array = false;
    QString cName = ""; // имя текущей величины
    QList<Lexem*> cBound; // текущая граница массива
    QList<Lexem*> initValue; // значение инициализации
    int deepV = 0; // глубина вложенности {}
    int deep1 = 0; // глубина вложенности [] внутри правой границы
    int deep2 = 0; // глубина вложенности () внутри правой границы
    int dim = 0;
    bool massDeclared = false;
    AST::Variable * var = 0;
    QList<AST::Variable*> result;

    int z = 0;
    int nameStart = 0;
    int arrayStart = 0;
    int arrayBoundStart = 0;
    int typePos = 0;

    Lexem endMark;
    endMark.type = LxOperSemicolon;
    group.lexems << &endMark;

    for ( int curPos=0; curPos<group.lexems.size(); curPos++ )
    {

        //      Предполагается, что модификатор группы уже задан, поэтому
        //      если встречается арг, рез или аргрез, то это ошибка
        if (group.lexems[curPos]->type==LxSecIn || group.lexems[curPos]->type==LxSecOut || group.lexems[curPos]->type==LxSecInout) {
            group.lexems[curPos]->error = _("Extra variable group specifier: %1", group.lexems[curPos]->data);
            return result;
        }
        else if ( ( par == type ) || ( par == tn ) )
        {
            //          Pазбираемся с типом переменных
            if (group.lexems[curPos]->type==LxNameClass) {
                cType = lexer->baseTypeByClassName(group.lexems[curPos]->data);
                array = lexer->isArrayClassName(group.lexems[curPos]->data);
                //              Переходим в режим поиска имени
                par = name;
                cName = "";
                nameStart = curPos+1;
                typePos = curPos;
                continue;
            }
            else if ( par == tn ) {
                par = name;
            }
            else
            {
                // не указан тип переменной
                if ( typePos != 0 ) {
                    group.lexems[typePos]->error = _("Type not declared before");
                }
                else {
                    for (int a=0; a<curPos+1; a++) {
                        group.lexems[a]->error = _("Type not declared before");
                    }
                }
                return result;
            }
        } // endif ( par == type || par == tn )
        if ( par == name )
        {
            if (group.lexems[curPos]->type==LxOperRightSqBr)
            {
                group.lexems[curPos]->error = _("No pairing '['");
                return result;
            }
            if (group.lexems[curPos]->type==LxOperLeftSqBr)
            {
                if ( massDeclared )
                {
                    group.lexems[curPos]->error = _("Extra '['");
                    return result;
                }
                if ( cName.trimmed().isEmpty() )
                {
                    group.lexems[typePos]->error = _("No variable name");
                    return result; // нет имени переменной
                }
                else
                {
                    if ( !array )
                    {
                        for (int a=0; a<group.lexems.size(); a++) {
                            group.lexems[a]->error = _("Extra array bound");
                            if (group.lexems[a]->type==LxOperRightSqBr)
                                break;
                        }
                        return result; // границы не нужны
                    }
                    else
                    {
                        cBound.clear();
                        par = lb;
                        arrayStart = curPos;
                        arrayBoundStart = curPos;
                    }
                }
            } // if ( '[' )
            else if (group.lexems[curPos]->type==LxPriAssign)
            {
                group.lexems[curPos]->error = _("':=' should not be here");
                return result;
            }
            else if (group.lexems[curPos]->type==LxOperComa || group.lexems[curPos]->type==LxOperSemicolon || group.lexems[curPos]->type==LxOperEqual)
            {
                if ( cName.trimmed().isEmpty() )
                {
                    group.lexems[typePos]->error = _("Variable name is empty");
                    return result;
                }
                QString loc_err = lexer->testName(cName);
                if (!loc_err.isEmpty()) {
                    group.lexems[nameStart]->error = loc_err;
                    return result;
                }
                if ( cName.count("\"") || cName.count("'") )
                {
                    group.lexems[nameStart]->error = _("Constant instead of name");
                    return result;
                }
                cName = cName.trimmed();
                //                if ( cName == QObject::trUtf8("знач") )
                //                    return PV_ZNACH_NAME;
                AST::Variable * vv;
                if (findVariable(cName, mod, alg, vv)) {
                    group.lexems[nameStart]->error = _("Variable already declared");
                    return result; // переменная уже объявлена
                }
                AST::Algorhitm * aa;
                if (findAlgorhitm(cName, mod, aa)) {
                    group.lexems[nameStart]->error = _("Name is used by algorhitm");
                }
                if (array && !massDeclared)
                {
                    group.lexems[nameStart]->error = _("Array bounds no specified");
                    return result; // нет границ
                }
                var = new AST::Variable;
                var->name = cName;
                var->baseType = cType;
                var->accessType = group.access;
                var->dimension = array? dim : 0;
                result << var;
                cName = "";
                par = tn;
                dim = 0;
                massDeclared = false;
                nameStart = curPos+1;
                if (group.lexems[curPos]->type==LxOperEqual) {
                    par = value;
                    z = curPos+1;
                }
            }
            else
            {
                if (!cName.isEmpty())
                    cName += " ";
                cName += group.lexems[curPos]->data;
            }
        } // endif ( par == name || par == tn )
        else if ( par == lb )
        {
            if (group.lexems[curPos]->type==LxOperLeftSqBr)
            {
                deep1 ++;
                cBound << group.lexems[curPos];
            }
            else if (group.lexems[curPos]->type==LxOperLeftBr)
            {
                deep2 ++;
                cBound << group.lexems[curPos];
            }
            else if (group.lexems[curPos]->type==LxOperRightSqBr && deep1>0)
            {
                deep1 --;
                cBound << group.lexems[curPos];

            }
            else if (group.lexems[curPos]->type==LxOperRightBr)
            {
                deep2 --;
                cBound << group.lexems[curPos];
            }
            else if (group.lexems[curPos]->type==LxOperComa && deep2==0 && deep1==0)
            {
                if ( cBound.isEmpty() )
                {
                    for (int a=arrayStart; a<=curPos; a++) {
                        group.lexems[a]->error = _("No left array bound");
                    }
                    return result;
                }
                else
                {
                    group.lexems[curPos]->error = _("',' instead of ':'");
                    return result;
                }
            }
            //          Pазбор левой границы массива
            else if (group.lexems[curPos]->type==LxOperSemicolon)
            {
                group.lexems[arrayStart]->error = _("No pairing ']'");
                return result;
            }
            else if (group.lexems[curPos]->type==LxOperColon)
            {

                if ( cBound.isEmpty() )
                {
                    for (int a=arrayStart; a<=curPos; a++)
                        group.lexems[a]->error = _("Left array bound not specified");
                    return result;
                }

                AST::Expression * left = parseExpression(cBound, mod, alg);

                if ( left==0 ) // error
                {
                    return result; // ошибка разбора левой границы
                }
                if (left->baseType!=AST::TypeInteger)
                {
                    for (int a=arrayBoundStart; a<=curPos; a++) {
                        group.lexems[a]->error = _("Left array bound is not integer");
                    }
                    return result; // левая граница -- не целое число
                }

                QString pattern = "\\b"+cName+"\\b";
                if ( alg && !alg->header.name.isEmpty() )
                    pattern += "|\\b"+alg->header.name+"\\b";
                QRegExp rxRecursion = QRegExp(pattern);
                for (int a=0; a<cBound.size(); a++) {
                    if (rxRecursion.indexIn(cBound[a]->data)!=-1) {
                        cBound[a]->error = _("Recursive declaration of bound");
                        return result;
                    }
                }
                var->bounds.last().first = left;
                cBound.clear();
                par = rb;
                arrayBoundStart = curPos;
            }
            else if (group.lexems[curPos]->type==LxOperRightSqBr && deep1==0)
            {
                group.lexems[curPos]->error = _("Left array bound is empty");
                return result;
            }
            else
            {
                cBound << group.lexems[curPos];
            }
        } // endif ( par == lb )
        else if ( par == rb )
        {
            if (group.lexems[curPos]->type==LxOperLeftSqBr)
            {
                deep1 ++;
                cBound << group.lexems[curPos];
            }
            else if (group.lexems[curPos]->type==LxOperLeftBr)
            {
                deep2 ++;
                cBound << group.lexems[curPos];
            }
            else if (group.lexems[curPos]->type==LxOperRightSqBr && deep1>0)
            {
                deep1 --;
                cBound << group.lexems[curPos];

            }
            else if (group.lexems[curPos]->type==LxOperRightBr)
            {
                deep2 --;
                cBound << group.lexems[curPos];
            }
            else if (group.lexems[curPos]->type==LxOperComa && deep1==0 && deep2==0)
            {
                if ( dim > 2 )
                {
                    for (int a=arrayStart; a<group.lexems.size(); a++) {
                        group.lexems[a]->error = _("Array dimension must not be > 3");
                        if (group.lexems[a]->type==LxOperRightSqBr)
                            break;
                    }
                    return result; // размерность > 3
                }
                dim++;

                AST::Expression * right = parseExpression(cBound, mod, alg);
                if ( right==0 )
                {
                    return result; // ошибка разбора левой границы
                }
                if (right->baseType!=AST::TypeInteger)
                {
                    for (int a=arrayBoundStart; a<=curPos; a++) {
                        group.lexems[a]->error = _("Right array bound is not integer");
                    }
                    return result; // правая граница -- не целое число
                }

                QString pattern = "\\b"+cName+"\\b";
                if ( alg && !alg->header.name.isEmpty() )
                    pattern += "|\\b"+alg->header.name+"\\b";
                QRegExp rxRecursion = QRegExp(pattern);
                for (int a=0; a<cBound.size(); a++) {
                    if (rxRecursion.indexIn(cBound[a]->data)!=-1) {
                        cBound[a]->error = _("Recursive declaration of bound");
                        return result;
                    }
                }
                cBound.clear();
                par = lb;
                arrayBoundStart = curPos;
                var->bounds.last().second = right;
            }
            else if (group.lexems[curPos]->type==LxOperRightSqBr && deep1==0)
            {
                if ( dim > 2 )
                {
                    for (int a=arrayStart; a<group.lexems.size(); a++) {
                        group.lexems[a]->error = _("Array dimension must not be > 3");
                        if (group.lexems[a]->type==LxOperRightSqBr)
                            break;
                    }
                    return result;
                }

                dim++;

                if ( cBound.isEmpty() )
                {
                    for (int a=arrayBoundStart; a<=curPos; a++) {
                        group.lexems[a]->error = _("No right array bound");
                    }
                    return result;
                }

                AST::Expression * right = parseExpression(cBound, mod, alg);

                if (right==0) {
                    return result;
                }
                if (right->baseType!=AST::TypeInteger)
                {
                    for (int a=arrayBoundStart+1; a<curPos; a++) {
                        group.lexems[a]->error = _("Right array bound is not integer");
                    }
                    return result; // правая граница -- не целое число
                }
                QString pattern = "\\b"+cName+"\\b";
                if ( alg && !alg->header.name.isEmpty() )
                    pattern += "|\\b"+alg->header.name+"\\b";
                QRegExp rxRecursion = QRegExp(pattern);
                for (int a=0; a<cBound.size(); a++) {
                    if (rxRecursion.indexIn(cBound[a]->data)!=-1) {
                        cBound[a]->error = _("Recursive declaration of bound");
                        return result;
                    }
                }
                par = tn;
                massDeclared = true;
                var->bounds.last().second = right;

            }
            else if (group.lexems[curPos]->type==LxOperSemicolon) {
                group.lexems[curPos]->error = _("No pairing ']'");
                return result;
            }
            else {
                cBound << group.lexems[curPos];
            }
        } // endif ( par == rb )
        else if ( par==value ) {
            if (group.access==AST::AccessRegular) {
                group.lexems[z-1]->error = _("Initial values should not be specified in algorhitm header");
                return result;
            }
            if (group.lexems[curPos]->type==LxOperLeftBrace) {
                deepV ++;
            }
            else if  (group.lexems[curPos]->type==LxOperRightBrace) {
                deepV --;
            }
            if ( deepV==0 && ( group.lexems[curPos]->type==LxOperComa || group.lexems[curPos]->type==LxOperSemicolon )) {

                if (initValue.isEmpty()) {
                    group.lexems[z-1]->error = _("Initial value is empty");
                    return result;
                }
                int maxDim = 0;
                QVariant constValue = parseConstant(initValue, var->baseType, var->dimension>0, maxDim);
                if (constValue==QVariant::Invalid) {
                    return result;
                }
                if (var->dimension>0) {
                    if (var->dimension!=maxDim) {
                        for (int a=z-1; a<=curPos; a++) {
                            group.lexems[a]->error = _("Unmatched array dimensions");
                            return result;
                        }
                    }
                }
                var->initialValue = constValue;

                par = tn;
                initValue.clear();
            }
            else {
                initValue << group.lexems[curPos];
            }
        } // endif ( par==value )
    }
    return result;
}

QVariant SyntaxAnalizerPrivate::parseConstant(const QList<Lexem*> &constant
                                              , const AST::VariableBaseType pt
                                              , bool array
                                              , int& maxDim
                                              ) const
{
    int localErr = 0;
    AST::VariableBaseType ct;

    if (constant.size()==1 && constant[0]->data.trimmed()=="...") {
        constant[0]->error = _("Constant value not typed");
        return QVariant::Invalid;
    }
    if (!array) {
        maxDim = 0;
        ct = testConst(constant[0], localErr);
        if ( ct==AST::TypeNone ) {
            for (int a=0; a<constant.size(); a++) {
                constant[a]->error = _("Not a constant value");
            }
            return QVariant::Invalid;
        }
        else if ( ( ct==pt ) || (ct==AST::TypeInteger && pt==AST::TypeReal) )
        {
            return createConstValue(constant[0], ct);
        }
        else {
            for (int a=0; a<constant.size(); a++) {
                constant[a]->error = _("Constant type mismatch");
            }
            return QVariant::Invalid; // FIXME: type mismatch
        }
    }
    // TODO implement me
    return QVariant::Invalid;
}

AST::VariableBaseType SyntaxAnalizerPrivate::testConst(const Lexem *lx, int &err) const
{
    err = 0;
    if (lx->type==LxConstInteger)
        return AST::TypeInteger;
    if (lx->type==LxConstReal)
        return AST::TypeReal;
    if (lx->type==LxConstBoolTrue || lx->type==LxConstBoolFalse)
        return AST::TypeBoolean;
    if (lx->type==LxConstLiteral && lx->data.size()==1)
        return AST::TypeCharect;
    if (lx->type==LxConstLiteral)
        return AST::TypeString;
    // TODO implement overflow range checking
    return AST::TypeNone;
}

QVariant SyntaxAnalizerPrivate::createConstValue(const Lexem *lx
                                                 , const AST::VariableBaseType type) const
{
    QVariant result = QVariant::Invalid;
    if (type==AST::TypeBoolean) {
        result = QVariant(lexer->boolConstantValue(lx->data));
    }
    else if (type==AST::TypeCharect) {
        result = QVariant(QChar(lx->data[0]));
    }
    else if (type==AST::TypeInteger) {
        result = QVariant(lx->data.toInt());
    }
    else if (type==AST::TypeReal) {
        result = QVariant(lx->data.toDouble());
    }
    else if (type==AST::TypeString) {
        result = QVariant(lx->data);
    }
    return result;
}

bool SyntaxAnalizerPrivate::findAlgorhitm(const QString &name, const AST::Module *module, AST::Algorhitm *&algorhitm)
{
    algorhitm = 0;
    for (int i=0; i<ast->modules.size(); i++) {
        if (ast->modules[i]->header.enabled) {
            if (module && module==ast->modules[i]) {
                // The same module - find includes private members
                for (int j=0; j<ast->modules[i]->impl.algorhitms.size(); j++) {
                    AST::Algorhitm * alg = ast->modules[i]->impl.algorhitms[j];
                    if (alg->header.name==name) {
                        algorhitm = alg;
                        return true;
                    }
                }
            }
            else {
                // Find only public algorhitm
                for (int j=0; j<ast->modules[i]->header.algorhitms.size(); j++) {
                    AST::Algorhitm * alg = ast->modules[i]->header.algorhitms[j];
                    if (alg->header.name==name) {
                        algorhitm = alg;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool SyntaxAnalizerPrivate::findGlobalVariable(const QString &name, const AST::Module *module, AST::Variable *&var)
{
    var = 0;
    for (int i=0; i<module->impl.globals.size(); i++) {
        AST::Variable * v = module->impl.globals[i];
        if (v->name==name) {
            var = v;
            return true;
        }
    }
    return false;
}

bool SyntaxAnalizerPrivate::findLocalVariable(const QString &name, const AST::Algorhitm *alg, AST::Variable *&var)
{
    var = 0;
    for (int i=0; i<alg->impl.locals.size(); i++) {
        AST::Variable * v = alg->impl.locals[i];
        if (v->name==name) {
            var = v;
            return true;
        }
    }
    return false;
}

bool SyntaxAnalizerPrivate::findVariable(const QString &name
                                         , const AST::Module *module
                                         , const AST::Algorhitm *algorhitm
                                         , AST::Variable *&var)
{
    bool found = false;
    if (algorhitm) {
        found = findLocalVariable(name, algorhitm, var);
    }
    if (!found) {
        found = findGlobalVariable(name, module, var);
    }
    return found;
}

enum BlockType {
    None,
    Simple,
    Element,
    Function,
    SubExpression
} ;

#define IS_OPERATOR(x) (x & LxTypeOperator || x==LxSecOr || x==LxSecAnd || x==LxSecNot )

AST::Expression * SyntaxAnalizerPrivate::parseExpression(
    QList<Lexem *> lexems
    , const AST::Module *mod
    , const AST::Algorhitm *alg
    )
{
    AST::Expression * result = 0;
    BlockType blockType = None;

    Lexem * prevOper = 0;
    Lexem * oper = 0;
    QList<SubexpressionElement> subexpression;
    QList<Lexem*> block;

    int prevPos = 0;
    int curPos = -1;

    while (curPos < lexems.size()) {
        blockType = None;
        block.clear();
        prevOper = oper;
        oper = 0;
        prevPos = curPos;
        curPos ++;
        for ( ; curPos<lexems.size(); curPos++) {
            if (IS_OPERATOR(lexems[curPos]->type)) {
                oper = lexems[curPos];
                break;
            }
            block << lexems[curPos];
        }


        if (oper && oper->type==LxOperLeftBr) {
            if (block.size()>0 && block[0]->type!=LxSecNot)
                blockType = Function;
            else
                blockType = SubExpression;
        }
        else if (oper && oper->type==LxOperLeftSqBr) {
            if (block.isEmpty()) {
                lexems[curPos]->error = _("Array name not specified before '['");
                return 0;
            }
            blockType = Element;
        }
        else {
            if (block.size()>0) {
                blockType = Simple;
            }
            else if (prevOper && prevOper->type==LxOperComa && (!oper || (oper && oper->type!=LxOperPlus && oper->type!=LxOperMinus)) ) {
                prevOper->error = _("Extra ','");
                return 0;
            }
        }

        Lexem * notFlag = 0;
        if (block.size()>1 && block.last()->type==LxSecNot) {
            block.last()->error = _("Extra 'not'");
            return 0;
        }
        for (QList<Lexem*>::iterator it=block.begin(); it!=block.end(); ) {
            if ( (*it)->type==LxSecNot && notFlag==0 ) {
                notFlag = (*it);
                it = block.erase(it);
            }
            else if ( (*it)->type==LxSecNot && notFlag!=0 ) {
                (*it)->error = _("Too many 'not'");
                return 0;
            }
            else {
                it++;
            }
        }



        if (blockType==Function) {
            int deep = 0;
            int openBrPos = curPos;
            for ( ; curPos < lexems.size(); curPos++) {
                block << lexems[curPos];
                if (lexems[curPos]->type==LxOperLeftBr)
                    deep++;
                if (lexems[curPos]->type==LxOperRightBr)
                    deep--;
                if (deep==0) {
                    break;
                }
            }
            if (deep>0) {
                lexems[openBrPos]->error = _("No pairing ')'"); // FIXME error code for unmatched open bracket
                return 0;
            }
            AST::Expression * operand = parseFunctionCall(block, mod, alg);
            if (!operand) {
                return 0;
            }
            if (notFlag) {
                if (operand->baseType!=AST::TypeBoolean) {
                    delete operand;
                    notFlag->error = _("Use of 'not' for non-boolean value");
                    return 0;
                }
                subexpression << notFlag;
            }
            curPos++;
            if (curPos==lexems.size()) {
                oper = 0;
            }
            else if (IS_OPERATOR(lexems[curPos]->type)) {
                oper = lexems[curPos];
            }
            else {
                lexems[curPos]->error = _("Garbage in expression"); // FIXME garbage error
                delete operand;
                return 0;
            }
            subexpression << operand;
            if (oper)
                subexpression << oper;
        } // end if (blockType==Function)
        else if (blockType==Simple) {
            AST::Expression * operand = parseSimpleName(block, mod, alg);
            if (!operand) {
                return 0;
            }
            if (notFlag) {
                if (operand->baseType!=AST::TypeBoolean) {
                    delete operand;
                    notFlag->error =  _("Use of 'not' for non-boolean value");
                    return 0;
                }
                subexpression << notFlag;
            }
            subexpression << operand;
            if (oper)
                subexpression << oper;
        } // end if (blockType==Variable)
        else if (blockType==Element) {
            int deep = 0;
            int openBrPos = curPos;
            for ( ; curPos < lexems.size(); curPos++) {
                block << lexems[curPos];
                if (lexems[curPos]->type==LxOperLeftSqBr)
                    deep++;
                if (lexems[curPos]->type==LxOperRightSqBr)
                    deep--;
                if (deep==0) {
                    break;
                }
            }
            if (deep>0) {
                lexems[openBrPos]->error = _("No pairing ']'"); // FIXME error code for unmatched open square bracket
                return 0;
            }
            AST::Expression * operand = parseElementAccess(block, mod, alg);
            if (!operand) {
                return 0;
            }
            if (notFlag) {
                if (operand->baseType!=AST::TypeBoolean) {
                    delete operand;
                    notFlag->error =  _("Use of 'not' for non-boolean value");
                    return 0;
                }
                subexpression << notFlag;
            }
            curPos++;
            if (curPos==lexems.size()) {
                oper = 0;
            }
            else if (IS_OPERATOR(lexems[curPos]->type)) {
                oper = lexems[curPos];
            }
            else {
                lexems[curPos]->error = _("Garbage in expression"); // FIXME garbage error
                delete operand;
                return 0;
            }
            subexpression << operand;
            if (oper)
                subexpression << oper;
        } // end if (blockType==Element)
        else if (blockType==SubExpression) {
            int deep = 0;
            int openBrPos = curPos;
            for ( ; curPos < lexems.size(); curPos++) {
                block << lexems[curPos];
                if (lexems[curPos]->type==LxOperLeftBr)
                    deep++;
                if (lexems[curPos]->type==LxOperRightBr)
                    deep--;
                if (deep==0) {
                    break;
                }
            }
            if (deep>0) {
                lexems[openBrPos]->error = _("No pairing ')'"); // FIXME error code for unmatched open bracket
                return 0;
            }
            if (block.size()<3) {
                if (block.size()>0) {
                    block[0]->error = _("Nothing between '(' and ')'"); // FIXME error code: nothing between brackets
                }
                if (block.size()>1) {
                    block[1]->error = block[0]->error;
                }
                return 0;
            }
            block.pop_back(); // remove ")"
            block.pop_front(); // remove "("
            AST::Expression * operand = parseExpression(block, mod, alg);
            if (!operand) {
                return 0;
            }
            if (notFlag) {
                if (operand->baseType!=AST::TypeBoolean) {
                    delete operand;
                    notFlag->error =  _("Use of 'not' for non-boolean value");
                    return 0;
                }
                subexpression << notFlag;
            }
            curPos++;
            if (curPos==lexems.size()) {
                curPos = 0;
            }
            else if (IS_OPERATOR(lexems[curPos]->type)) {
                oper = lexems[curPos];
            }
            else {
                lexems[curPos]->error = _("Garbage in expression"); // FIXME garbage error
                delete operand;
                return 0;
            }
            subexpression << operand;
            if (oper)
                subexpression << oper;
        } // end if (blockType==SubExpression)
        else if (blockType==None) {
            qDebug() << "Check me: " << __FILE__ << ", line: " << __LINE__;
        }

    }


    result = makeExpressionTree(subexpression);


    return result;
}

AST::Expression * SyntaxAnalizerPrivate::parseElementAccess(const QList<Lexem *> &lexems, const AST::Module *mod, const AST::Algorhitm *alg)
{
    AST::Expression * result = 0;
    QString name;
    Lexem * openBracket = 0;
    int openBracketIndex = -1;
    for (int i=0; i<lexems.size(); i++) {
        if (lexems[i]->type==LxOperLeftSqBr) {
            openBracket = lexems[i];
            openBracketIndex = i;
            break;
        }
        else {
            if (i>0) name += " ";
            name += lexems[i]->data;
        }
    }
//    QList<AST::Expression*> realArguments;
//    QList< QList<Lexem*> > arguments;
//    QList<Lexem*> comas;
    if (name.isEmpty()) {
        openBracket->error = _("Array name not specified before '['");
        return 0;
    }
    AST::Variable * variable = 0;
    if (!findLocalVariable(name, alg, variable)) {
        if (!findGlobalVariable(name, mod, variable)) {
            for (int i=0; i<openBracketIndex; i++) {
                lexems[i]->error = _("Array variable not found");
            }
            return 0;
        }
    }
    return result;
}


AST::Expression * SyntaxAnalizerPrivate::parseFunctionCall(const QList<Lexem *> &lexems, const AST::Module *mod, const AST::Algorhitm *alg)
{
    AST::Expression * result = 0;
    QString name;
    Lexem * openBracket = 0;
    int openBracketIndex = -1;
    for (int i=0; i<lexems.size(); i++) {
        if (lexems[i]->type==LxOperLeftBr) {
            openBracket = lexems[i];
            openBracketIndex = i;
            break;
        }
        else {
            if (i>0) name += " ";
            name += lexems[i]->data;
        }
    }
    QList<AST::Expression*> realArguments;
    QList< QList<Lexem*> > arguments;
    QList<Lexem*> comas;

    AST::Algorhitm * function = 0;
    if (!findAlgorhitm(name, mod, function)) {
        if (openBracketIndex==-1)
            openBracketIndex = lexems.size();
        for (int i=0; i<openBracketIndex; i++) {
            lexems[i]->error = _("Algorhitm not found");
        }
        return 0;
    }

    if (!openBracket) {
        // No arguments
        // Do nothing, because of implemented as other
        // block type (see parseSimpleName);
        qWarning() << "!!! Something strange at " << __FILE__ << ", line: " << __LINE__;
        return 0;
    }

    //
    QList<Lexem*> argLine = lexems.mid(openBracketIndex);
    Lexem * ob = 0;
    Lexem * cb = 0;
    int deep = 0;
    int cbPos = -1;
    for (int i=0; i<argLine.size(); i++) {
        if (argLine[i]->type==LxOperLeftBr) {
            ob = argLine[i];
            deep ++;
        }
        if (argLine[i]->type==LxOperRightBr) {
            cb = argLine[i];
            cbPos = i;
            deep --;
        }
    }
    if (deep>0) {
        ob->error = _("Unpaired '('");
        return 0;
    }
    if (deep<0) {
        for (int i=argLine.size()-1; i>=0; i--) {
            if (argLine[i]->type==LxOperRightBr) {
                deep--;
            }
            if (deep==0) {
                argLine[i]->error = _("Unpaired ')'");
                return 0;
            }
        }
    }
    if ((cbPos+1)<argLine.size()) {
        for (int i=cbPos+1; i<argLine.size(); i++) {
            argLine[cbPos]->error = _("Garbage in expression");
        }
        return 0;
    }
    argLine.pop_back();
    argLine.pop_front();
    if (argLine.isEmpty()) {
        openBracket->error = _("Nothing between '(' and ')'");
        cb->error = _("Nothing between '(' and ')'");
        return 0;
    }
    splitLexemsByComas(argLine, arguments, comas);
    int diff = arguments.size()>function->header.arguments.size();
    if (diff>0) {
        int errorStartIndex = 0;
        for (int i=comas.size()-1; i>=0; i--) {
            deep--;
            if (deep==0) {
                errorStartIndex = argLine.indexOf(comas[i]);
            }
        }
        for (int i=errorStartIndex; i<cbPos; i++) {
            argLine[i]->error = _("Extra algorhitm arguments");
        }
        return 0;
    }
    if (diff<0) {
        cb->error = _("Not enought algorhitm arguments");
        return 0;
    }
    for (int i=0; i<arguments.size(); i++) {
        AST::Expression * argument = parseExpression(arguments[i], mod, alg);
        if (!argument) {
            foreach (AST::Expression * a, realArguments) delete a;
            return 0;
        }
        realArguments << argument;
        AST::VariableBaseType intType = argument->baseType;
        int intDim = argument->dimension;
        AST::VariableBaseType extType = function->header.arguments[i]->baseType;
        int extDim = function->header.arguments[i]->dimension;
        bool allowTypeChange = function->header.arguments[i]->accessType==AST::AccessArgumentIn;
        bool typesCompatible = ( intType==AST::TypeInteger && extType==AST::TypeReal )
                || ( intType==AST::TypeCharect && extType==AST::TypeString );
        QString err;
        if (intDim!=extDim) {
            err = _("Array dimension mismatch");
        }
        if (intType!=extType) {
            if (!allowTypeChange || !typesCompatible)
                err = _("Incompatible types");
        }
        if (argument->kind!=AST::ExprVariable
                && function->header.arguments[i]->accessType==AST::AccessArgumentOut)
        {
            err = _("Can't pass this expression as out-argument");
        }
        if (argument->kind!=AST::ExprVariable
                && function->header.arguments[i]->accessType==AST::AccessArgumentInOut)
        {
            err = _("Can't pass this expression as in/out-argumeny");
        }
        if (argument->kind==AST::ExprVariable
                && argument->variable->accessType==AST::AccessArgumentIn)
        {
            if (function->header.arguments[i]->accessType==AST::AccessArgumentOut)
            {
                err = _("Can't pass in-argument as out-argument");
            }
            if (function->header.arguments[i]->accessType==AST::AccessArgumentInOut)
            {
                err = _("Can't pass in-argument as in/out-argument");
            }
        }
        if (!err.isEmpty()) {
            foreach (Lexem * l, arguments[i]) l->error = err;
            foreach (AST::Expression * a, realArguments) delete a;
            return 0;
        }
    } // end for arguments loop
    result = new AST::Expression;
    result->kind = AST::ExprFunctionCall;
    result->baseType = function->header.returnType;
    result->dimension = 0;
    result->function = function;
    result->operands = realArguments;
    return result;
}

AST::Expression * SyntaxAnalizerPrivate::parseSimpleName(const QList<Lexem *> &lexems, const AST::Module *mod, const AST::Algorhitm *alg)
{
    AST::Expression * result = 0;
    if (lexems.size()==1 && lexems[0]->type & LxTypeConstant) {
        int err = 0;
        AST::VariableBaseType type = testConst(lexems[0], err);
        if (err) {
            lexems[0]->error = err;
            return 0;
        }
        else {
            result = new AST::Expression;
            result->kind = AST::ExprConst;
            result->baseType = type;
            int maxDim = 0;
            result->constant = parseConstant(lexems, type, false, maxDim);
            return result;
        }
    }
    if (lexems.size()==1 && lexems[0]->type == LxSecNewline) {
        result = new AST::Expression;
        result->kind = AST::ExprConst;
        result->baseType = AST::TypeCharect;
        result->dimension = 0;
        result->constant = QVariant(QChar('\n'));
        return result;
    }
    else {
        QString name;
        for (int i=0; i<lexems.size(); i++) {
            if (i>0)
                name += " ";
            name += lexems[i]->data;
        }
        QString err = lexer->testName(name);
        bool retval = lexer->isReturnVariable(name);
        if (!err.isEmpty() && !retval) {
            for ( int i=0; i<lexems.size(); i++ ) {
                lexems[i]->error = err;
            }
            return 0;
        }

        AST::Algorhitm * a = 0;
        AST::Variable * v = 0;

        err = "";
        if (retval) {
            if (!alg) {
                err = _("Access to return value outside of algorhitm");
            }
            if (alg && alg->header.returnType==AST::TypeNone) {
                err = _("This algorhitm has no return value");
            }
            if (err.isEmpty()) {
                bool found = findLocalVariable(alg->header.name, alg, v);
                Q_ASSERT(found);
                result = new AST::Expression;
                result->kind = AST::ExprVariable;
                result->baseType = alg->header.returnType;
                result->variable = v;
                return result;
            }
        }
        else {
            if (findAlgorhitm(name, mod, a)) {
                if (a->header.arguments.size()>0) {
                    err = _("No arguments");
                }
                else {
                    result = new AST::Expression;
                    result->kind = AST::ExprFunctionCall;
                    result->function = a;
                    result->baseType = a->header.returnType;
                    return result;
                }
            }
            else if (findVariable(name, mod, alg, v)) {
                if (v->dimension>0) {
                    err = _("Array without index"); // FIXME: error code for array in expression
                }
                else {
                    result = new AST::Expression;
                    result->kind = AST::ExprVariable;
                    result->variable = v;
                    result->baseType = v->baseType;
                    return result;
                }
            }
        }
        if (!err.isEmpty()) {
            for (int i=0; i<lexems.size(); i++) {
                lexems[i]->error = err;
            }
            return 0;
        }
    }
    return result;
}



int findOperatorByPriority(const QList<SubexpressionElement> & s, int &rank)
{
    int result = -1;
    static const QList<QSet <LexemType> > Omega = QList<QSet<LexemType> >()
            << ( QSet<LexemType>() << LxSecOr << LxSecAnd )
            << ( QSet<LexemType>() << LxOperNotEqual << LxOperEqual << LxOperGreater << LxOperGreaterOrEqual << LxOperLess << LxOperLessOrEqual )
            << ( QSet<LexemType>() << LxOperPlus << LxOperMinus )
            << ( QSet<LexemType>() << LxOperAsterisk << LxOperSlash )
            << ( QSet<LexemType>() << LxOperPower );
    for (int i=0; i<Omega.count(); i++) {
        for (int j=s.size()-1; j>=0; j--) {
            if (s[j].o && Omega[i].contains(s[j].o->type)) {
                result = j;
                break;
            }
        }
        if (result!=-1) {
            break;
            rank = i;
        }
    }
    return result;
}

#define IS_NUMERIC(x) ( x==AST::TypeInteger || x==AST::TypeReal )
#define IS_LITERAL(x) ( x==AST::TypeCharect || x==AST::TypeString )

bool IS_NUMERIC_LIST(const QList<AST::Expression*> & list) {
    bool result = true;
    for (int i=0; i<list.size(); i++) {
        result = result && IS_NUMERIC(list[i]->baseType);
    }
    return result;
}

bool IS_LITERAL_LIST(const QList<AST::Expression*> & list) {
    bool result = true;
    for (int i=0; i<list.size(); i++) {
        result = result && IS_LITERAL(list[i]->baseType);
    }
    return result;
}

AST::ExpressionOperator operatorByLexem(const Lexem * lx)
{
    if (lx->type==LxOperPlus)
        return AST::OpSumm;
    else if (lx->type==LxOperMinus)
        return AST::OpSubstract;
    else if (lx->type==LxOperAsterisk)
        return AST::OpMultiply;
    else if (lx->type==LxOperSlash)
        return AST::OpDivision;
    else if (lx->type==LxOperPower)
        return AST::OpPower;
    else if (lx->type==LxSecNot)
        return AST::OpNot;
    else if (lx->type==LxSecAnd)
        return AST::OpAnd;
    else if (lx->type==LxSecOr)
        return AST::OpOr;
    else if (lx->type==LxOperEqual)
        return AST::OpEqual;
    else if (lx->type==LxOperNotEqual)
        return AST::OpNotEqual;
    else if (lx->type==LxOperLess)
        return AST::OpLess;
    else if (lx->type==LxOperGreater)
        return AST::OpGreater;
    else if (lx->type==LxOperLessOrEqual)
        return AST::OpLessOrEqual;
    else if (lx->type==LxOperGreaterOrEqual)
        return AST::OpGreaterOrEqual;
    else
        return AST::OpNone;


}

AST::VariableBaseType resType(AST::VariableBaseType a
                              , AST::VariableBaseType b
                              , AST::ExpressionOperator op)
{
    if(a==AST::TypeNone)
    {
        return AST::TypeNone;
    }

    if(a==AST::TypeInteger)
    {
        if(b==AST::TypeInteger)
        {
            if(op==AST::OpSumm)
                return AST::TypeInteger;
            if(op==AST::OpSubstract)
                return AST::TypeInteger;
            if(op==AST::OpMultiply)
                return AST::TypeInteger;
            if(op==AST::OpDivision)
                return AST::TypeReal;
            if(op==AST::OpPower)
                return AST::TypeInteger;

            if(op==AST::OpGreater)

                return AST::TypeBoolean;
            if(op==AST::OpLess)

                return AST::TypeBoolean;
            if(op==AST::OpEqual)
                return AST::TypeBoolean;


            if(op==AST::OpNotEqual)

                return AST::TypeBoolean;
            if(op==AST::OpGreaterOrEqual)

                return AST::TypeBoolean;
            if(op==AST::OpLessOrEqual)

                return AST::TypeBoolean;

            if(op==AST::OpOr)
            {
                return AST::TypeNone;
            };
            if(op==AST::OpAnd)
            {
                return AST::TypeNone;
            };

        }

        if(b==AST::TypeReal)
        {
            if ( op==AST::OpSumm )
                return AST::TypeReal;
            if ( op==AST::OpSubstract )
                return AST::TypeReal;
            if ( op==AST::OpMultiply )
                return AST::TypeReal;
            if ( op==AST::OpDivision )
                return AST::TypeReal;
            if ( op==AST::OpPower )
                return AST::TypeReal;

            if(op==AST::OpNotEqual)

                return AST::TypeBoolean;
            if(op==AST::OpGreaterOrEqual)

                return AST::TypeBoolean;
            if(op==AST::OpLessOrEqual)

                return AST::TypeBoolean;

            if(op==AST::OpGreater)

                return AST::TypeBoolean;
            if(op==AST::OpLess)

                return AST::TypeBoolean;
            if(op==AST::OpEqual)
                return AST::TypeBoolean;

            if(op==AST::OpOr)
            {
                return AST::TypeNone;
            };
            if(op==AST::OpAnd)
            {
                return AST::TypeNone;
            };


        }

        if ( b == AST::TypeBoolean )
        {
            return AST::TypeNone;
        }

        if ( b == AST::TypeCharect )
        {
            return AST::TypeNone;
        }

        if ( b == AST::TypeString )
        {
            return AST::TypeNone;
        }
        return AST::TypeNone;

    } // end if (a==AST::TypeInteger)


    if(a==AST::TypeBoolean)
    {
        if(b==AST::TypeBoolean)
        {
            if(op==AST::OpOr)
            {
                return AST::TypeBoolean;
            };
            if(op==AST::OpAnd)
            {
                return AST::TypeBoolean;
            };

            if ( op == AST::OpNotEqual )
                return AST::TypeBoolean;
            if(op==AST::OpEqual)
                return AST::TypeBoolean;
            return AST::TypeNone;

        }
        if ( b==AST::TypeInteger )
        {
            return AST::TypeNone;
        }
        if ( b==AST::TypeReal )
        {
            return AST::TypeNone;
        }
        if ( b==AST::TypeCharect )
        {
            return AST::TypeNone;
        }
        if ( b==AST::TypeString )
        {
            return AST::TypeNone;
        }

        return AST::TypeNone;
    } // end if (a==AST::TypeBoolean)

    if (a==AST::TypeReal)
    {
        if(b==AST::TypeInteger)
        {
            if(op==AST::OpSumm)
                return AST::TypeReal;
            if(op==AST::OpSubstract)
                return AST::TypeReal;
            if(op==AST::OpMultiply)
                return AST::TypeReal;
            if(op==AST::OpDivision)
                return AST::TypeReal;
            if ( op==AST::OpPower )
                return AST::TypeReal;

            if(op==AST::OpGreater)
                return AST::TypeBoolean;
            if(op==AST::OpLess)
                return AST::TypeBoolean;
            if(op==AST::OpEqual)
                return AST::TypeBoolean;

            if(op==AST::OpNotEqual)
                return AST::TypeBoolean;
            if(op==AST::OpGreaterOrEqual)
                return AST::TypeBoolean;
            if(op==AST::OpLessOrEqual)
                return AST::TypeBoolean;

            return AST::TypeNone;

        }

        if (b==AST::TypeReal)
        {
            if(op==AST::OpSumm)
                return AST::TypeReal;
            if(op==AST::OpSubstract)
                return AST::TypeReal;
            if(op==AST::OpMultiply)
                return AST::TypeReal;
            if(op==AST::OpDivision)
                return AST::TypeReal;
            if ( op==AST::OpPower )
                return AST::TypeReal;

            if(op==AST::OpGreater)
                return AST::TypeBoolean;
            if(op==AST::OpLess)
                return AST::TypeBoolean;
            if(op==AST::OpEqual)
                return AST::TypeBoolean;

            if(op==AST::OpNotEqual)
                return AST::TypeBoolean;
            if(op==AST::OpGreaterOrEqual)
                return AST::TypeBoolean;
            if(op==AST::OpLessOrEqual)
                return AST::TypeBoolean;
            return AST::TypeNone;

        }

        if ( b==AST::TypeString )
        {
            return AST::TypeNone;
        }

        if ( b==AST::TypeCharect )
        {
            return AST::TypeNone;
        }

        if ( b==AST::TypeBoolean )
        {
            return AST::TypeNone;
        }
        return AST::TypeNone;

    } // end if (a==AST::TypeReal)

    if(a==AST::TypeString)
    {
        if ( b==AST::TypeString )
        {
            if ( op==AST::OpSumm )
                return AST::TypeString;
            if(op==AST::OpGreater)
                return AST::TypeBoolean;
            if(op==AST::OpLess)
                return AST::TypeBoolean;
            if(op==AST::OpEqual)
                return AST::TypeBoolean;

            if(op==AST::OpNotEqual)
                return AST::TypeBoolean;
            if(op==AST::OpGreaterOrEqual)
                return AST::TypeBoolean;
            if(op==AST::OpLessOrEqual)
                return AST::TypeBoolean;

            return AST::TypeNone;
        }
        if ( b==AST::TypeCharect )
        {
            if ( op==AST::OpSumm )
                return AST::TypeString;
            if(op==AST::OpGreater)
                return AST::TypeBoolean;
            if(op==AST::OpLess)
                return AST::TypeBoolean;
            if(op==AST::OpEqual)
                return AST::TypeBoolean;

            if(op==AST::OpNotEqual)
                return AST::TypeBoolean;
            if(op==AST::OpGreaterOrEqual)
                return AST::TypeBoolean;
            if(op==AST::OpLessOrEqual)
                return AST::TypeBoolean;

            return AST::TypeNone;
        }
        if ( b==AST::TypeBoolean )
        {
            return AST::TypeNone;
        }
        if ( b==AST::TypeReal )
        {
            return AST::TypeNone;
        }
        if ( b==AST::TypeInteger )
        {
            return AST::TypeNone;
        }
        return AST::TypeNone;
    } //string


    if(a==AST::TypeCharect)
    {
        if ( b==AST::TypeString )
        {
            if ( op==AST::OpSumm )
                return AST::TypeString;
            if(op==AST::OpGreater)
                return AST::TypeBoolean;
            if(op==AST::OpLess)
                return AST::TypeBoolean;
            if(op==AST::OpEqual)
                return AST::TypeBoolean;

            if(op==AST::OpNotEqual)
                return AST::TypeBoolean;
            if(op==AST::OpGreaterOrEqual)
                return AST::TypeBoolean;
            if(op==AST::OpLessOrEqual)
                return AST::TypeBoolean;
            return AST::TypeNone;
        }
        if ( b==AST::TypeCharect )
        {
            if ( op==AST::OpSumm )
                return AST::TypeString;
            if(op==AST::OpGreater)
                return AST::TypeBoolean;
            if(op==AST::OpLess)
                return AST::TypeBoolean;
            if(op==AST::OpEqual)
                return AST::TypeBoolean;

            if(op==AST::OpNotEqual)
                return AST::TypeBoolean;
            if(op==AST::OpGreaterOrEqual)
                return AST::TypeBoolean;
            if(op==AST::OpLessOrEqual)
                return AST::TypeBoolean;
            return AST::TypeNone;
        }
        if ( b==AST::TypeBoolean )
        {
            return AST::TypeNone;
        }
        if ( b==AST::TypeReal )
        {
            return AST::TypeNone;
        }
        if ( b==AST::TypeInteger )
        {
            return AST::TypeNone;
        }
        return AST::TypeNone;
    } //string

    return AST::TypeNone;

}

AST::Expression * SyntaxAnalizerPrivate::makeExpressionTree(const QList<SubexpressionElement> & s)
{
    if (s.isEmpty())
        return 0;
    int rank;
    int l = findOperatorByPriority(s, rank);
    if (l==-1) {
        for (int i=0; i<s.size(); i++) {
            if (s[i].e)
                return s[i].e;
        }
    }
    else {
        QList<SubexpressionElement> head = s.mid(0, l);
        QList<SubexpressionElement> tail = s.mid(l+1);
        if (head.isEmpty()) {
            static QSet<LexemType> UnaryOperations = QSet<LexemType>()
                    << LxOperPlus << LxOperMinus << LxSecNot;
            if (!UnaryOperations.contains(s[l].o->type)) {
                s[l].o->error = _("No operand before non-unary operator");
                return 0;
            }
        }
        if (head.isEmpty() && tail.isEmpty()) {
            s[l].o->error = _("No operands");
            return 0;
        }
        if (tail.isEmpty()) {
            s[l].o->error = _("Extra operator");
            return 0;
        }
        AST::Expression * headExpr = makeExpressionTree(head);
        AST::Expression * tailExpr = makeExpressionTree(tail);
        if (!head.isEmpty() && !headExpr)
            return 0;
        if (!tailExpr)
            return 0;
        AST::VariableBaseType headType = headExpr? headExpr->baseType : AST::TypeNone;
        AST::VariableBaseType tailType = tailExpr->baseType;
        static const QSet<LexemType> ComparisonOperators = QSet<LexemType>()
                << LxOperLess << LxOperLessOrEqual << LxOperEqual << LxOperNotEqual << LxOperGreaterOrEqual << LxOperGreater;
        bool makeCNF = false;
        if (IS_NUMERIC(tailType)
                && headType==AST::TypeBoolean
                && headExpr
                && headExpr->kind == AST::ExprSubexpression
                && ComparisonOperators.contains(s[l].o->type)
                && rank==1
                && IS_NUMERIC_LIST(headExpr->operands)
                )
        {
            makeCNF = true;
        }
        if (IS_LITERAL(tailType)
                && headType==AST::TypeBoolean
                && headExpr
                && headExpr->kind == AST::ExprSubexpression
                && ComparisonOperators.contains(s[l].o->type)
                && rank==1
                && IS_LITERAL_LIST(headExpr->operands)
                )
        {
            makeCNF = true;
        }
        if (makeCNF) {
            AST::Expression * res = new AST::Expression;
            res->kind = AST::ExprSubexpression;
            res->baseType = AST::TypeBoolean;
            res->operands << headExpr;
            res->operatorr = AST::OpAnd;
            AST::Expression * subRes = new AST::Expression;
            subRes->kind = AST::ExprSubexpression;
            subRes->baseType = AST::TypeBoolean;
            subRes->operands << new AST::Expression(headExpr->operands.last());
            subRes->operands << tailExpr;
            subRes->operatorr = operatorByLexem(s[l].o);
            res->operands << subRes;
            return res;
        }
        else {
            AST::ExpressionOperator operation = operatorByLexem(s[l].o);
            AST::VariableBaseType rt = resType(headType, tailType, operation);
            if (rt==AST::TypeNone) {
                s[l].o->error = _("Can't %1%2%3"
                                  ,lexer->classNameByBaseType(headType)
                                  ,s[l].o->data
                                  ,lexer->classNameByBaseType(tailType)
                                  );
                if (headExpr)
                    delete headExpr;
                if (tailExpr)
                    delete tailExpr;
                return 0;
            }
            AST::Expression * res = new AST::Expression;
            res->kind = AST::ExprSubexpression;
            res->baseType = rt;
            res->operands << headExpr << tailExpr;
            res->operatorr = operation;
            return res;
        }

    }
    return 0;
}



SyntaxAnalizer::~SyntaxAnalizer()
{
    delete d;
}

} // namespace KumirAnalizer
