#include "syntaxanalizer.h"
#include "lexer.h"
#include <limits>
#include <deque>
#include <iostream>
#include <fstream>

#include "dataformats/ast_variable.h"
#include "dataformats/ast_type.h"
#include "errormessages/errormessages.h"
#include "extensionsystem/pluginmanager.h"
#include "interfaces/actorinterface.h"
#include "vm/vm_bytecode.hpp"

#define BADNAME_KEYWORD TN_BAD_NAME_3
#define BADNAME_OPERATOR TN_BAD_NAME_1

using namespace Shared;

namespace KumirAnalizer {

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

struct SyntaxAnalizerPrivate
{
    Lexer * lexer;
    AST::Data * ast;
    AST::Algorhitm * algorhitm;
    QList<Statement> statements;
    QSet<QString> unresolvedImports;
    QStringList alwaysEnabledModules;

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

    bool tryInputOperatorAlgorithm(
            const QString & lexem,
            AST::Type & type,
            QVariant & constantValue
            ) const;
    bool findConversionAlgorithm(const AST::Type & from
                                 , const AST::Type & to
                                 , AST::Module * mod
                                 , AST::Algorhitm * alg) const;
    AST::Expression * makeCustomBinaryOperation(const QString & operatorName
                            , AST::Expression * leftExpression
                            , AST::Expression * rightExpression
                            );
    AST::Expression * makeCustomUnaryOperation(const QString & operatorName
                                               , AST::Expression * argument);
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
    bool findUserType(const QString & name, AST::Type &type, AST::Module * module);
    QList<AST::Variable*> parseVariables(int statementIndex, VariablesGroup & group,
                                         AST::Module * mod,
                                         AST::Algorhitm * alg, bool algHeader);
    QVariant parseConstant(const std::list<Lexem*> &constant
                           , const AST::VariableBaseType pt
                           , int& maxDim
                           ) const;
    AST::VariableBaseType testConst(const std::list<Lexem *> & lxs, int &err) const;
    QVariant createConstValue(const QString &str, const AST::VariableBaseType type) const;
    AST::Expression * parseFunctionCall(const QList<Lexem*> &lexems, const AST::Module * mod, const AST::Algorhitm * alg);

    AST::Expression * parseSimpleName(const std::list<Lexem*> &lexems, const AST::Module * mod, const AST::Algorhitm * alg);
    void updateSliceDSCall(AST::Expression * expr, AST::Variable * var);
    AST::Expression * parseElementAccess(const QList<Lexem*> &lexems, const AST::Module * mod, const AST::Algorhitm * alg);
    AST::Expression * makeExpressionTree(const QList<SubexpressionElement> & s);
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
    int currentPosition;

    QList<Statement*> originalStatements;

};

SyntaxAnalizer::SyntaxAnalizer(Lexer * lexer,
                               const QStringList & alwaysEnabledModules,
                               QObject *parent) :
    QObject(parent)
{
    d = new SyntaxAnalizerPrivate;
    d->ast = 0;
    d->algorhitm = 0;
    d->lexer = lexer;
    d->currentPosition = -1;
    d->alwaysEnabledModules = alwaysEnabledModules;
}

void SyntaxAnalizer::syncStatements()
{
    Q_ASSERT(d->originalStatements.size()==d->statements.size());
    for (int i=0; i<d->originalStatements.size(); i++) {
        Q_CHECK_PTR(d->originalStatements[i]);
        d->originalStatements[i]->variables = d->statements[i].variables;
    }
}

void SyntaxAnalizer::init(
    QList<Statement*> &statements,
    AST_Data *ast,
    AST_Algorhitm *algorhitm)
{
    d->originalStatements = statements;
    d->currentPosition = statements.size()-1;
    d->ast = ast;
    d->algorhitm = algorhitm;
    d->statements.clear();
    for (int i=0; i<statements.size(); i++) {
        Statement * st = statements[i];
        Q_CHECK_PTR(st);
//        if (st->type!=LxTypeComment) {
            Statement sst;
            sst.type = st->type;
            sst.statement = st->statement;
            sst.alg = st->alg;
            sst.mod = st->mod;
            sst.conditionalIndex = st->conditionalIndex;
            for (int j=0; j<st->data.size(); j++) {
                Lexem * lx = st->data[j];
                Q_CHECK_PTR(lx);
                if (lx->type!=LxTypeComment)
                    sst.data << lx;
            }

            d->statements << sst;
//        }
    }

    d->unresolvedImports.clear();
}


Lexem * SyntaxAnalizerPrivate::findLexemByType(const QList<Lexem*> lxs, LexemType type)
{
    foreach (Lexem * lx , lxs) {
        if (lx->type==type)
            return lx;
    }
    return 0;
}

AST::Type typeFromSignature(QString s) {
    AST::Type result;
    if (s.startsWith("void"))
        result.kind = AST::TypeNone;
    else if (s.startsWith("int"))
        result.kind = AST::TypeInteger;
    else if (s.startsWith("real"))
        result.kind = AST::TypeReal;
    else if (s.startsWith("bool"))
        result.kind = AST::TypeBoolean;
    else if (s.startsWith("char"))
        result.kind = AST::TypeCharect;
    else if (s.startsWith("string"))
        result.kind = AST::TypeString;
    else if (s.startsWith("record ")) {
        result.kind = AST::TypeUser;
        s.remove(0, 7);
        int br = s.indexOf("{");
        result.name = s.left(br);
        s.remove(0, br+1);
        int lbr = s.lastIndexOf("}");
        s = s.left(lbr);
        QStringList fields = s.split(";");
        for (int i=0; i<fields.size(); i++) {
            AST::Type fieldType = typeFromSignature(fields[i]);
            AST::Field field(fields[i], fieldType);
            result.userTypeFields.append(field);
        }
    }
    return result;
}

void SyntaxAnalizer::buildTables(bool isInternalBuild)
{
//    if (d->algorhitm)
//        return; // Nothing to build if we analize just one algorhitm

    for (int i=0; i<d->statements.size(); i++) {
        const Statement & st = d->statements[i];
        bool wasError = st.hasError();
        if (st.type==LxPriModule) {
            d->parseModuleHeader(i);
        }
        if (st.type==LxPriImport) {
            d->parseImport(i);
        }
        if (!wasError && d->statements[i].hasError()) {
            foreach (Lexem * lx, d->statements[i].data) {
                if (!lx->error.isEmpty())
                    lx->errorStage = Lexem::Tables;
            }
        }
    }

    if (!isInternalBuild)
    for (int i=0; i<d->ast->modules.size(); i++) {
        AST::Module * mod = d->ast->modules[i];
        if (mod->header.type==AST::ModTypeExternal) {
            if (mod->builtInID!=0xF0 &&
                    !d->alwaysEnabledModules.contains(mod->header.name))
            {
                mod->header.enabled = false;
            }
        }
    }

    for (int i=0; i<d->ast->modules.size(); i++) {
        if (d->ast->modules[i]->header.type==AST::ModTypeUser || d->ast->modules[i]->header.type==AST::ModTypeHidden)
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

    // Find and load unresolved imports
    for (int i=0; i<d->unresolvedImports.size(); i++) {
        const QString name = d->unresolvedImports.toList()[i];
        QString error;
        if (name.endsWith(".kod")) {
            QString canonicalName = name;
            if (canonicalName.startsWith(QDir::currentPath())) {
                canonicalName.remove(0, QDir::currentPath().length());
                if (canonicalName.startsWith("/"))
                    canonicalName.remove(0,1);
            }
            QFileInfo kodFile(name);
            QString kodFilePath = QDir::toNativeSeparators(kodFile.absoluteFilePath());
            const char * programName = kodFilePath.toLocal8Bit().constData();
            std::ifstream programFile(programName);
            Bytecode::Data programData;
            if (!programFile.is_open()) {
                error = _("Can't open module file");
            }
            else {
                try {
                    Bytecode::bytecodeFromDataStream(programFile, programData);
                }
                catch (...) {
                    error = _("Module file is damaged");
                }
            }
            programFile.close();
            if (error.length()==0) {
                AST::Module * module = new AST::Module;
                module->header.type = AST::ModTypeCached;
                module->header.name = canonicalName;
                module->header.enabled = true;
                d->ast->modules.push_back(module);
                for (size_t e=0; e<programData.d.size(); e++) {
                    const Bytecode::TableElem & elem = programData.d.at(e);
                    if (elem.type==Bytecode::EL_FUNCTION || elem.type==Bytecode::EL_MAIN) {
                        const QString algName = QString::fromStdWString(elem.name);
                        if (algName.length()>0 && !algName.startsWith("_")) {
                            AST::Algorhitm * alg = new AST::Algorhitm;
                            alg->header.name = algName;
                            alg->header.implType = AST::AlgorhitmExternal;
                            alg->header.external.moduleName = canonicalName;
                            alg->header.external.id = elem.id;
                            const QString signature = QString::fromStdWString(elem.signature);
                            QStringList algSig = signature.split(":");
                            alg->header.returnType = typeFromSignature(algSig[0]);
                            if (algSig.size()>1) {
                                QStringList argSignatures = algSig[1].split(",");
                                for (int argNo=0; argNo<argSignatures.size(); argNo++) {
                                    AST::Variable * var = new AST::Variable;
                                    QStringList sigPair = argSignatures[argNo].split(" ");
                                    if (sigPair[0]=="in")
                                        var->accessType = AST::AccessArgumentIn;
                                    else if (sigPair[0]=="out")
                                        var->accessType = AST::AccessArgumentOut;
                                    else if (sigPair[0]=="inout")
                                        var->accessType = AST::AccessArgumentInOut;
                                    var->baseType = typeFromSignature(sigPair[1]);
                                    var->dimension = sigPair[1].count("[]");
                                    alg->header.arguments.push_back(var);
                                }
                            }
                            module->header.algorhitms.push_back(alg);
                        }
                    }
                }
            }

        }
    }

    // Set errors for imports that could not be resolved
    for (int i=0; i<d->unresolvedImports.size(); i++) {
        const QString name = d->unresolvedImports.toList()[i];

        for (int j=0; j<d->statements.size(); j++) {
            if (d->statements[j].type==Shared::LxPriImport && !d->statements[j].hasError()) {
                QString nn;
                for (int k=1; k<d->statements[j].data.size(); k++) {
                    Lexem * lx = d->statements[j].data[k];
                    if (!nn.isEmpty())
                        nn += " ";
                    nn += lx->data;
                }
                if (nn==name) {
                    const QString error = _("No such module");
                    for (int k=1; k<d->statements[j].data.size(); k++) {
                        Lexem * lx = d->statements[j].data[k];
                        lx->error = error;
                    }
                    if (d->statements[j].mod) {
                        AST::Module * mod = d->statements[j].mod;
                        AST::Statement * errStatement = new AST::Statement;
                        errStatement->type = AST::StError;
                        errStatement->error = error;
                        errStatement->lexems = d->statements[j].data;
                        mod->impl.initializerBody.prepend(errStatement);
                    }
                }
            }
        }
    }

    for (int i=0; i<d->statements.size(); i++) {
        const Statement & st = d->statements[i];
        bool wasError = st.hasError();
        if (st.type==LxPriAlgHeader) {
            d->parseAlgHeader(i, true, isInternalBuild);
        }
        if (!wasError && d->statements[i].hasError()) {
            foreach (Lexem * lx, d->statements[i].data) {
                if (!lx->error.isEmpty())
                    lx->errorStage = Lexem::Tables;
            }
        }
    }

    for (int i=0; i<d->statements.size(); i++) {
        const Statement & st = d->statements[i];
        bool wasError = st.hasError();
        if (st.type==LxNameClass && st.alg==0) {
            d->parseVarDecl(i);
        }
        if (st.type==LxPriAlgHeader) {
            d->parseAlgHeader(i, false, isInternalBuild);
        }
        if (!wasError && d->statements[i].hasError()) {
            foreach (Lexem * lx, d->statements[i].data) {
                if (!lx->error.isEmpty())
                    lx->errorStage = Lexem::Tables;
            }
        }
    }
    d->ast->modules[0]->header.enabled = true;
}

void SyntaxAnalizer::processAnalisys()
{
    for (int i=0; i<d->statements.size(); i++) {
        if (d->statements[i].hasError()) {
            foreach (Lexem * lx, d->statements[i].data) {
                if (lx->errorStage == AST::Lexem::Semantics) {
                    lx->error.clear();
                    lx->errorStage = AST::Lexem::NoError;
                }
            }
        }
    }
    for (int i=0; i<d->statements.size(); i++) {
        d->currentPosition = i;
        Statement & st = d->statements[i];
        // Fix unmatched modules first
        if (!st.mod) {
            for (int j=0; j<d->ast->modules.size(); j++) {
                if (d->ast->modules[j]->header.type==AST::ModTypeUser &&
                        d->ast->modules[j]->header.name.isEmpty()) {
                    st.mod = d->ast->modules[j];
                    break;
                }
            }
        }
        if (!st.mod) {
            // Add dummy module for this statement
            // (it is possible to reuse dummy module later)
            AST::Module * dummyModule = new AST::Module;
            dummyModule->header.type = AST::ModTypeUser;
            d->ast->modules.push_back(dummyModule);
            st.mod  = dummyModule;
        }
        bool wasError = st.hasError();
        if (st.statement) {
            for (int j=0; j<st.statement->expressions.size(); j++) {
                delete d->statements[i].statement->expressions[j];
            }
            d->statements[i].statement->expressions.clear();
        }
        if (st.type==LxPriAssign) {
            d->parseAssignment(i);
        }
        else if (st.type==LxNameClass && st.alg) {
            d->parseVarDecl(i);
        }
        else if (
                 st.type==LxPriAssert
                 || st.type==LxPriPre
                 || st.type==LxPriPost
                 )
        {
            d->parseAssertPrePost(i);
        }
        else if (st.type==LxPriInput) {
            d->parseInput(i);
        }
        else if (
                 st.type==LxPriOutput
                 )
        {
            d->parseOutput(i);
        }

        else if (st.type==LxPriEndModule
                 ||st.type==LxPriAlgBegin
                 ||st.type==LxPriAlgEnd
                 ||st.type==LxPriThen
                 ||st.type==LxPriElse
                 ||st.type==LxPriFi
                 ||st.type==LxPriSwitch
                 ||st.type==LxPriExit
                 ||st.type==LxPriPause
                 ||st.type==LxPriHalt
                 )
        {
            d->parseOneLexemInstruction(i);
        }
        else if (st.type==LxPriEndLoop) {
            d->parseEndLoop(i);
        }
        else if (st.type==LxPriIf||st.type==LxPriCase) {
            d->parseIfCase(i);
        }
        else if (st.type==LxPriLoop) {
            d->parseLoopBegin(i);
        }
        for (int j=0; j<st.data.size(); j++) {
            if (!st.data[j]->error.isEmpty()) {
                if (st.statement) {
                    if (st.type==LxPriLoop || st.type==LxPriSwitch) {
                        st.statement->beginBlockError = st.data[j]->error;
                    }
                    else if (st.type==LxPriEndLoop || st.type==LxPriFi) {
                        st.statement->endBlockError = st.data[j]->error;
                    }
                    else if (st.type==LxPriCase || st.type==LxPriElse) {
                        if (st.conditionalIndex < st.statement->conditionals.size()) {
                            st.statement->conditionals[st.conditionalIndex].conditionError = st.data[j]->error;
                        }
                    }
                    else {
                        st.statement->type = AST::StError;
                        st.statement->error = st.data[j]->error;
                    }
                    break;
                }
            }
        }
        if (!wasError && d->statements[i].hasError()) {
            foreach (Lexem * lx, d->statements[i].data) {
                if (lx->errorStage==AST::Lexem::NoError && !lx->error.isEmpty())  {
                    lx->errorStage = AST::Lexem::Semantics;
                }

            }
        }
    }
}

void SyntaxAnalizerPrivate::parseImport(int str)
{
    if (statements[str].hasError())
        return;
    Statement & st = statements[str];
    if (st.data.size()<2) {
        Q_ASSERT(st.data.size()==1);
        st.data[0]->error = _("No module name");
        return;
    }
    if (st.data.size()>2) {
        for (int i=2; i<st.data.size(); i++) {
            st.data[i]->error = _("Garbage afrer module name");
        }
        return;
    }
    if (st.data[1]->data.isEmpty()) {
        st.data[1]->error = _("No module name");
        return;
    }
    QString name;
    if (st.data[1]->type==LxConstLiteral) {
        name = st.data[1]->data.trimmed();
        if (name.isEmpty()) {
            st.data[1]->error = _("Must be Kumir program file name");
            return;
        }
        if (!name.endsWith(".kum") && !name.endsWith(".kod")) {
            name += ".kum";
        }
        QString kumName, binName;
        QFileInfo binFile, kumFile;
        if (name.endsWith(".kum")) {
            kumName = name;
            name = binName = name.left(name.length()-4)+".kod";
            binFile = QFileInfo (QDir::current().absoluteFilePath(binName));
            kumFile = QFileInfo (QDir::current().absoluteFilePath(kumName));
            QString kumir2bc = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("kumir2-bc");
#ifdef Q_OS_WIN32
            kumir2bc += ".exe";
#endif
            if (kumFile.exists()) {
                if (!binFile.exists() || binFile.lastModified()<kumFile.lastModified()) {
                    // Run kumir2-bc to create (or update) binary module
                    int status = QProcess::execute(kumir2bc, QStringList()
                                                   << "--debuglevel=2" << kumFile.absoluteFilePath());
                    if (status==-2) {
                        st.data[1]->error = _("Can't run kumir2-bc to compile this module");
                        return;
                    }
                    else if (status==-1) {
                        st.data[1]->error = _("kumir2-bc crashed while compiling this module");
                        return;
                    }
                    binFile = QFileInfo (QDir::current().absoluteFilePath(binName));
                    if (!binFile.exists()) {
                        st.data[1]->error = _("Error compiling this module");
                        return;
                    }
                }
            }
        }
        else {
            binName = name;
            binFile = QFileInfo (QDir::current().absoluteFilePath(binName));
        }
        if (!kumFile.exists() && !binFile.exists()) {
            st.data[1]->error = _("No such file");
            return;
        }
        name = binFile.absoluteFilePath();
    }
    else {
        QString localError = lexer->testName(st.data[1]->data);
        if (localError.size()>0) {
            st.data[1]->error = localError;
            return;
        }
        name = st.data[1]->data.simplified();
        st.data[1]->type = LxNameModule;
    }
    AST::Module * mod = st.mod;
    Q_CHECK_PTR(mod);
    mod->header.uses.insert(name);
}

void SyntaxAnalizerPrivate::parseModuleHeader(int str)
{
    if (statements[str].hasError())
        return;
    const Statement & st = statements[str];
    if (st.data.size()<2) {
        Q_ASSERT(st.data.size()==1);
        st.data[0]->error = _("No module name");
        return;
    }
    if (st.data.size()>2) {
        for (int i=2; i<st.data.size(); i++) {
            st.data[i]->error = _("Garbage afrer module name");
        }
        return;
    }
    if (st.data[1]->data.isEmpty()) {
        st.data[1]->error = _("No module name");
        return;
    }
    QString localError = lexer->testName(st.data[1]->data);
    if (localError.size()>0) {
        st.data[1]->error = localError;
        return;
    }
    AST::Module * mod = st.mod;
    Q_CHECK_PTR(mod);
    st.data[1]->type = LxNameModule;
    mod->header.name = st.data[1]->data.simplified();
}


void SyntaxAnalizerPrivate::parseVarDecl(int str)
{
    const Statement & st = statements[str];
    if (st.hasError())
        return;
    AST::Algorhitm * alg = st.alg;
    AST::Module * mod = st.mod;
    VariablesGroup group;
    group.access = AST::AccessRegular;
    for (int i=0; i<st.data.size(); i++) {
        if (st.data[i]->type & LxTypeComment)
            break;
        group.lexems << st.data[i];
    }
    QList<AST::Variable*> vars = parseVariables(str, group, mod, alg, false);
    QString error;
    for (int i=0; i<group.lexems.size()-1; i++) {
        if (group.lexems[i]->error.size()>0) {
            error = group.lexems[i]->error;
            break;
        }
    }
    if (!error.isEmpty()) {
        st.statement->type = AST::StError;
        st.statement->error = error;
    }
    else {
        st.statement->type = AST::StVarInitialize;
        st.statement->variables = vars;
    }
}

void SyntaxAnalizerPrivate::parseOutput(int str)
{
    const Statement & st = statements[str];
    if (st.hasError()) {
        return;
    }
    if (st.data.size()==1) {
        QString err = _("What to output?");
        st.data[0]->error = err;
        return;
    }
    if (st.data.last()->type==LxOperComa) {
        st.data.last()->error = _("Statement ends with coma");
        return;
    }
    QList< QList<Lexem*> > groups;
    QList<Lexem*> comas;
    splitLexemsByOperator(st.data.mid(1), LxOperComa, groups, comas);

    AST::Expression * fileHandle = 0;

    for (int i=0 ; i<groups.size(); i++) {
        QString err;

        QList< QList<Lexem*> > subgroups;
        QList< Lexem* > colons;
        splitLexemsByOperator(groups[i], LxOperColon, subgroups, colons);
        AST::Expression * expr = parseExpression(subgroups[0], st.mod, st.alg);
        if (!expr) {
            return;
        }
        int maxSubgroups;
        if (expr->baseType.kind==AST::TypeReal)
            maxSubgroups = 3;
        else if (expr->baseType.kind==AST::TypeInteger)
            maxSubgroups = 2;
        else
            maxSubgroups = 1;
        Q_ASSERT(colons.size() == subgroups.size()-1);
        if (subgroups.size()>maxSubgroups) {
            delete expr;
            err = _("Extra format parameter");
            for (int j=maxSubgroups-1 ; j<colons.size(); j++) {
                colons[j]->error = err;
            }
            for (int j=maxSubgroups; j<subgroups.size(); j++) {
                foreach (Lexem * lx, subgroups[j]) {
                    lx->error = err;
                }
            }
            return;
        }
        if (!fileHandle && expr->baseType.kind==AST::TypeUser && expr->baseType.name==QString::fromUtf8("файл")) {
            fileHandle = expr;
            continue;
        }
        AST::Expression * expr2 = 0, *expr3 = 0;
        if (subgroups.size()>1) {
            expr2 = parseExpression(subgroups[1], st.mod, st.alg);
            if (!expr2) {
                delete expr;
                return;
            }
            if (expr2->baseType.kind!=AST::TypeInteger) {
                err = _("Format parameter not integer");
                foreach (Lexem * lx, subgroups[1])
                    lx->error = err;
                delete expr;
                delete expr2;
                return;
            }
        }
        else {
            expr2 = new AST::Expression;
            expr2->baseType.kind = AST::TypeInteger;
            expr2->dimension = 0;
            expr2->kind = AST::ExprConst;
            expr2->constant = QVariant(0);
        }
        if (subgroups.size()>2) {
            expr3 = parseExpression(subgroups[2], st.mod, st.alg);
            if (!expr3) {
                delete expr;
                delete expr2;
                return;
            }
            if (expr3->baseType.kind!=AST::TypeInteger) {
                err = _("Format parameter not integer");
                foreach (Lexem * lx, subgroups[2])
                    lx->error = err;
                delete expr;
                delete expr2;
                delete expr3;
                return;
            }
        }
        else {
            expr3 = new AST::Expression;
            expr3->baseType.kind = AST::TypeInteger;
            expr3->dimension = 0;
            expr3->kind = AST::ExprConst;
            expr3->constant = QVariant(6);
        }
        if (expr->baseType.kind==AST::TypeUser) {
            AST::Expression * helperCall = makeCustomUnaryOperation(lexer->outputLexemName(), expr);
            if (helperCall) {
                expr = helperCall;
            }
            else {
                err = _("Can't output value of type %1", expr->baseType.name);
                foreach (Lexem * lx, subgroups[0])
                    lx->error = err;
                delete expr;
                delete expr2;
                delete expr3;
                return;
            }
        }
        st.statement->expressions << expr << expr2 << expr3;
    }
    if (fileHandle) {
        if (st.statement->expressions.size()==0) {
            delete fileHandle;
            QString err = _("What to output to file?");
            st.data.at(0)->error = err;
            return;
        }
        st.statement->expressions << fileHandle;
    }
}




void SyntaxAnalizerPrivate::parseInput(int str)
{
    const Statement & st = statements[str];
    if (st.hasError()) {
        return;
    }
    if (st.data.size()==1) {
        QString err = _("What to input?");
        st.data[0]->error = err;
        return;
    }
    if (st.data.last()->type==LxOperComa) {
        st.data.last()->error = _("Statement ends with coma");
        return;
    }
    QList< QList<Lexem*> > groups;
    QList<Lexem*> comas;
    splitLexemsByOperator(st.data.mid(1), LxOperComa, groups, comas);

    AST::Expression * fileHandle = 0;

    for (int i=0 ; i<groups.size(); i++) {
        QString err;

        AST::Expression * expr = parseExpression(groups[i], st.mod, st.alg);
        if (!expr) {
            for (int e=0; e<st.statement->expressions.size(); e++) {
                delete st.statement->expressions[e];
            }
            st.statement->expressions.clear();
            if (fileHandle)
                delete fileHandle;
            return;
        }

        if (!fileHandle && expr->baseType.kind==AST::TypeUser && expr->baseType.name==QString::fromUtf8("файл")) {
            fileHandle = expr;
            continue;
        }

        if (expr->baseType.kind==AST::TypeUser) {
            err = _("Can't input value of type %1",expr->baseType.name);
        }

        if (expr->kind==AST::ExprConst &&
                (expr->baseType!=AST::TypeCharect || expr->constant.toChar()!='\n'))
        {
            err = _("Can't input constant");
        }
        if (expr->kind==AST::ExprFunctionCall)
            err = _("Can't input function call");
        if (expr->kind==AST::ExprSubexpression)
            err = _("Can't input subexpression");
        if (expr->dimension>0)
            err = _("Can't input an array");


        if (err.length()>0) {
            foreach (Lexem * lx, groups[i])
                lx->error = err;
            delete expr;
            for (int e=0; e<st.statement->expressions.size(); e++) {
                delete st.statement->expressions[e];
            }
            st.statement->expressions.clear();
            if (fileHandle)
                delete fileHandle;
            return;
        }

        st.statement->expressions << expr;
    }
    if (fileHandle) {
        if (st.statement->expressions.size()==0) {
            delete fileHandle;
            QString err = _("What to input from file?");
            st.data.at(0)->error = err;
            return;
        }
        st.statement->expressions << fileHandle;
    }
}

void SyntaxAnalizerPrivate::parseAssertPrePost(int str)
{
    const Statement & st = statements[str];
    if (st.hasError()) {
        return;
    }
    if (st.data.size()==1) {
        QString err = _("No expressions after '%1'", st.data[0]->data);
        if (st.data[0]->type!=LxPriPre && st.data[0]->type!=LxPriPost)
            st.data[0]->error = err;
        return;
    }
    if (st.data.last()->type==LxOperComa) {
        st.data.last()->error = _("Statement ends with coma");
        return;
    }
    QList< QList<Lexem*> > groups;
    QList<Lexem*> comas;
    splitLexemsByOperator(st.data.mid(1), LxOperComa, groups, comas);

    for (int i=0 ; i<groups.size(); i++) {

        AST::Expression * expr = parseExpression(groups[i], st.mod, st.alg);
        if (!expr) {
            return;
        }
        QString err;
        if (expr->baseType.kind!=AST::TypeBoolean) {
            err = _("Condition is not boolean");
        }

        if (!err.isEmpty()) {
            for (int a=0; a<groups[i].size(); a++) {
                groups[i][a]->error = err;
            }
            delete expr;
            return;
        }
        st.statement->expressions << expr;
    }
}



void SyntaxAnalizerPrivate::parseOneLexemInstruction(int str)
{
    const Statement & st = statements[str];
    if (st.hasError()) {
        return;
    }
    for (int i=1; i<st.data.size(); i++) {
        st.data[i]->error = _("Garbage at end of line");
    }
}

void SyntaxAnalizerPrivate::parseEndLoop(int str)
{
    const Statement & st = statements[str];
    if (st.hasError()) {
        return;
    }
    if (st.data.size()>1 && st.data[1]->type!=LxSecIf) {
        for (int i=1; i<st.data.size(); i++) {
            st.data[i]->error = _("Garbage at end of line");
        }
        return;
    }
    if (st.data.size()==2 && st.data[1]->type==LxSecIf) {

        st.data[1]->error = _("No condition after 'end if'");
        if (st.data[1]->data.startsWith("_")) {
            // Mark "endloop_if" first lexem if it's written close to second
            st.data[0]->error = st.data[1]->error;
        }
        return;
    }
    if (st.data.size()>2) {
        QList<Lexem*> condLexems = st.data.mid(2);
        AST::Expression * expr = parseExpression(condLexems, st.mod, st.alg);
        if (expr) {
            if (expr->baseType.kind!=AST::TypeBoolean) {
                for (int i=2; i<st.data.size(); i++) {
                    st.data[i]->error = _("Condition is not boolean");
                }
                delete expr;
            }
            else {
                st.statement->loop.endCondition = expr;
            }
        }
    }

}

void SyntaxAnalizerPrivate::parseIfCase(int str)
{
    const Statement & st = statements[str];
    if (st.hasError()) {
        return;
    }
    if (st.type==LxPriIf && st.data.size()==1) {
        st.data[0]->error = _("No condition after 'if'");
        return;
    }
    if (st.type==LxPriCase && st.data.size()==1) {
        st.data[0]->error = _("No condition after 'case'");
        return;
    }
    if (st.type==LxPriCase && st.data.size()==2 && st.data[1]->type==LxOperColon) {
        st.data[0]->error = _("No condition between 'case' and colon");
        st.data[1]->error = _("No condition between 'case' and colon");
        return;
    }
    if (st.type==LxPriCase && st.data.last()->type!=LxOperColon) {
        for (int i=0; i<st.data.size(); i++) {
            st.data[i]->error = _("No colon after condition");
        }
        return;
    }
    QList<Lexem*> cond;
    if (st.type==LxPriCase)
        cond = st.data.mid(1, st.data.length()-2);
    else
        cond = st.data.mid(1);
    AST::Expression * expr = parseExpression(cond, st.mod, st.alg);
    if (expr) {
        if (expr->baseType.kind!=AST::TypeBoolean) {
            for (int i=0; i<cond.size(); i++) {
                cond[i]->error = _("Condition after '%1' not boolean", st.data[0]->data);
            }
            delete expr;
        }
        else {
            if (st.conditionalIndex < st.statement->conditionals.size()) {
                st.statement->conditionals[st.conditionalIndex].condition = expr;
            }
            else {
                delete expr;
            }
        }
    }
}

void SyntaxAnalizerPrivate::parseLoopBegin(int str)
{
    const Statement & st = statements[str];
    if (st.hasError() || !st.statement) {
        return;
    }
    AST::LoopType type = AST::LoopWhile;
    if (st.data.size()==1) {
        type = AST::LoopWhile;
    }
    else if (st.data.size()>1 && st.data[1]->type==LxSecFor) {
        type = AST::LoopFor;
    }
    else if (st.data.size()>1 && st.data[1]->type==LxSecWhile) {
        type = AST::LoopWhile;
    }
    else if (findLexemByType(st.data, LxSecTimes)) {
        type = AST::LoopTimes;
    }
    else {
        st.data[0]->error = _("Loop type not specified");
        return;
    }
    st.statement->loop.type = type;
    if (type==AST::LoopFor) {
        QList<Lexem*> forr;
        QList<Lexem*> from;
        QList<Lexem*> to;
        QList<Lexem*> step;
        Lexem * forLexem = st.data[1];
        Lexem * fromLexem = findLexemByType(st.data, LxSecFrom);
        Lexem * toLexem = findLexemByType(st.data, LxSecTo);
        Lexem * stepLexem = findLexemByType(st.data, LxSecStep);


        int forIndex = st.data.indexOf(forLexem);
        int fromIndex = st.data.indexOf(fromLexem);
        int toIndex = st.data.indexOf(toLexem);
        int stepIndex = st.data.indexOf(stepLexem);

        QString err =  "";

        if (fromIndex!=-1 && forIndex!=-1 && fromIndex-forIndex==1) {
            err =  _("No loop variable");
        }

        if (toIndex!=-1 && fromIndex>toIndex) {
            err = _("'to' earler then 'from'");
        }

        if (fromIndex==-1 && toIndex!=-1) {
            err = _("No 'from' before 'to'");
        }

        if (fromIndex==-1 && toIndex==-1) {
            err = _("No 'from'..'to'.. after variable");
        }

        if (!fromLexem) {
            err = _("No loop variable");
        }
        if (!fromLexem) {
            err = _("No loop 'from' value");
        }
        if (!toLexem) {
            err = _("No loop 'to' value");
        }

        if (err.length()) {
            for (int i=1; i<st.data.size(); i++) {
                st.data[i]->error = err;
            }
            return;
        }

        if (stepIndex==-1) {
            stepIndex = st.data.size();
        }
        for (int i=forIndex+1; i<fromIndex; i++ ) {
            forr << st.data[i];
        }
        for (int i=fromIndex+1; i<toIndex; i++) {
            from << st.data[i];
        }
        for (int i=toIndex+1; i<stepIndex; i++) {
            to << st.data[i];
        }
        for (int i=stepIndex+1; i<st.data.size(); i++) {
            step << st.data[i];
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
        if (findLocalVariable(name, st.alg, st.statement->loop.forVariable)) {

        }
        else if (findGlobalVariable(name, st.mod, st.statement->loop.forVariable)) {

        }
        else if (forr.size()==1 && forr[0]->type & LxTypeConstant) {
            foreach (Lexem *l, forr) l->error = _("Constant for-loop variable");
            return;
        }
        else {
            foreach (Lexem *l, forr) l->error = _("Variable not found");
            return;
        }
        if (st.statement->loop.forVariable->baseType.kind!=AST::TypeInteger
                || st.statement->loop.forVariable->dimension>0) {
            foreach (Lexem *l, forr) l->error = _("Not integer for-loop variable");
            return;
        }
        AST::Expression * fromExpr = parseExpression(from, st.mod, st.alg);
        if (!fromExpr) {
            return;
        }
        else if (fromExpr->baseType.kind!=AST::TypeInteger) {
            foreach (Lexem *l, from) l->error = _("Not integer from-value");
            delete fromExpr;
            return;
        }
        AST::Expression * toExpr = parseExpression(to, st.mod, st.alg);
        if (!toExpr) {
            delete fromExpr;
            return;
        }
        else if (toExpr->baseType.kind!=AST::TypeInteger) {
            foreach (Lexem *l, to) l->error = _("Not integer to-value");
            delete fromExpr;
            delete toExpr;
            return;
        }
        if (stepLexem && step.isEmpty()) {
            stepLexem->error = _("No step-value");
            delete fromExpr;
            delete toExpr;
            return;
        }
        AST::Expression * stepExpr = stepLexem? parseExpression(step, st.mod, st.alg) : 0;
        if (stepLexem && !stepExpr) {
            delete fromExpr;
            delete toExpr;
            return;
        }
        else if (stepExpr && stepExpr->baseType.kind!=AST::TypeInteger) {
            foreach (Lexem *l, step) l->error = _("Not integer step-value");
            delete fromExpr;
            delete toExpr;
            delete stepExpr;
            return;
        }
        st.statement->loop.fromValue = fromExpr;
        st.statement->loop.toValue = toExpr;
        st.statement->loop.stepValue = stepExpr;

    } // end if (type==AST::LoopFor)
    else if (type==AST::LoopTimes) {
        QList<Lexem*> times;
        bool timesFound = false;
        bool err = false;
        for (int i=1; i<st.data.size(); i++) {
            if (st.data[i]->type==LxSecTimes) {
                timesFound = true;
                if (times.isEmpty()) {
                    err = true;
                    st.data[i]->error = _("Times value not specified");
                    break;
                }
            }
            else {
                if (timesFound) {
                    err = true;
                    st.data[i]->error = _("Garbage at the end of statement");
                }
                else {
                    times << st.data[i];
                }
            }
        }
        if (err)
            return;
        AST::Expression * timesExpr = parseExpression(times, st.mod, st.alg);
        if (!timesExpr)
            return;
        else if (timesExpr->baseType.kind!=AST::TypeInteger) {
            foreach (Lexem * l, times) l->error = _("Not integer times-value");
            delete timesExpr;
            return;
        }
        st.statement->loop.timesValue = timesExpr;
    } // end if (type==AST::LoopTimes)
    else if (type==AST::LoopWhile) {
        if (st.data.size()==1) {
            // Forever loop
            AST::Expression * foreverr = new AST::Expression;
            foreverr->baseType.kind = AST::TypeBoolean;
            foreverr->kind = AST::ExprConst;
            foreverr->constant = QVariant(true);
            st.statement->loop.whileCondition = foreverr;
        }
        else {
            // While condition loop
            QList<Lexem*> cond = st.data.mid(2);
            if (cond.isEmpty()) {
                st.data[1]->error = _("No condition after 'loop while'");
                return;
            }
            AST::Expression * condExpr = parseExpression(cond, st.mod, st.alg);
            if (!condExpr)
                return;
            else if (condExpr->baseType.kind!=AST::TypeBoolean) {
                foreach (Lexem * l, cond) l->error = _("Condition is not boolean");
                delete condExpr;
                return;
            }
            st.statement->loop.whileCondition = condExpr;
        }
    } // end if (type==AST::LoopWhile)
}

void SyntaxAnalizerPrivate::parseAssignment(int str)
{
    const Statement & st = statements[str];
    if (st.hasError()) {
        return;
    }
    if (st.data.first()->type==LxSecFor) {
        st.data[0]->error = _("No 'loop' before 'for'");
        return;
    }
    if (st.data.first()->type==LxSecWhile) {
        st.data[0]->error = _("No 'loop' before 'while'");
        return;
    }
    if (st.data.first()->type==LxSecTimes) {
        st.data[0]->error = _("No 'loop' before 'times'");
        return;
    }
    AST::Algorhitm * alg = st.alg;
    AST::Module * mod = st.mod;
    Q_CHECK_PTR(mod);
    QList<Lexem*> left;
    QList<Lexem*> right;
    Lexem * assignOp = 0;
    int assignCount = 0;
    for (int i=0; i<st.data.size(); i++) {
        if (st.data[i]->type==LxPriAssign) {
            if (assignCount==1) {
                st.data[i]->error = _("Too many ':=' operators");
                return;
            }
            assignCount++;
            assignOp = st.data[i];
        }
        else {
            if (assignCount==0) {
                left << st.data[i];
            }
            else {
                right << st.data[i];
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
    if (left.isEmpty() && rightExpr->kind==AST::ExprFunctionCall &&
            rightExpr->baseType!=AST::TypeNone)
    {
        // TODO in 2.1:
        // allow this!
        QString err = _("Expression not assigned to anything");
        if (rightExpr->operatorr==AST::OpEqual)
        {
            // Possible = instead of :=
            err = _("'=' instead of ':='");
        }
        for (int i=0; i<right.size(); i++) {
            st.data[i]->error = err;
        }
        delete rightExpr;
        return;
    }
    if (left.isEmpty() && rightExpr->kind!=AST::ExprFunctionCall) {
        if (rightExpr->kind==AST::ExprSubexpression && rightExpr->operatorr==AST::OpEqual) {
            Lexem * lx = 0;
            for (int i=0; i<st.data.size(); i++) {
                if (st.data[i]->type==LxOperEqual) {
                    lx = st.data[i];
                    break;
                }
            }
            if (lx) {
                lx->error = _("'=' instead of ':='");
                delete rightExpr;
                return;
            }
        }
        const QString err = _("Expression not assigned to anything");
        for (int i=0; i<right.size(); i++) {
            st.data[i]->error = err;
        }
        delete rightExpr;
        return;
    }
    AST::Expression * leftExpr = 0;
    if (!left.isEmpty()) {
        if (rightExpr->baseType.kind==AST::TypeNone) {
            if (rightExpr->kind!=AST::ExprFunctionCall)
                assignOp->error = _("Assignment of void");
            else {
                const QString err = _("Assignment of non-returning algorithm");
                foreach (Lexem * lx, right) {
                    if (lx->type==LxOperLeftBr)
                        break;
                    lx->error = err;
                }
            }
            delete rightExpr;
            return;
        }
        leftExpr = parseExpression(left, mod, alg);
        if (!leftExpr) {
            delete rightExpr;
            return;
        }
        if (leftExpr->kind==AST::ExprFunctionCall) {
            assignOp->error = _("Assignment to function");
            delete rightExpr;
            delete leftExpr;
            return;
        }
        if (leftExpr->kind==AST::ExprSubexpression) {
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
            AST::VariableBaseType a = leftExpr->baseType.kind;
            AST::VariableBaseType b = rightExpr->baseType.kind;
            if (a==AST::TypeInteger) {
                if (b==AST::TypeReal) {
                    bool isRealConstant = false;
                    static const QString realSpecificSymbols =
                            QString::fromUtf8("еЕeE.");
                    if (rightExpr->kind==AST::ExprConst) {
                        QString strValue;
                        foreach (const AST::Lexem * lx, right) {
                            strValue += lx->data;
                        }
                        for (int s=0; s<realSpecificSymbols.length(); s++) {
                            const QChar ss = realSpecificSymbols[s];
                            if (strValue.contains(ss)) {
                                isRealConstant = true;
                                break;
                            }
                        }
                    }
                    if (!isRealConstant)
                    {
                        // Constant became real because of big integer representation
                        err = _("Integer constant too big");
                        foreach (Lexem * lx, right) { lx->error = err; }
                        delete leftExpr;
                        delete rightExpr;
                        return;
                    }
                    else {
                        err = _("Can't integer:=real");
                    }
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
                else if (b==AST::TypeUser) {
                    err = _("Can't %1:=%2", lexer->classNameByBaseType(leftExpr->baseType.kind), rightExpr->baseType.name);
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
                else if (b==AST::TypeUser) {
                    err = _("Can't %1:=%2", lexer->classNameByBaseType(leftExpr->baseType.kind), rightExpr->baseType.name);
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
                else if (b==AST::TypeUser) {
                    err = _("Can't %1:=%2", lexer->classNameByBaseType(leftExpr->baseType.kind), rightExpr->baseType.name);
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
                else if (b==AST::TypeUser) {
                    err = _("Can't %1:=%2", lexer->classNameByBaseType(leftExpr->baseType.kind), rightExpr->baseType.name);
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
                else if (b==AST::TypeUser) {
                    err = _("Can't %1:=%2", lexer->classNameByBaseType(leftExpr->baseType.kind), rightExpr->baseType.name);
                }
            }
            else if (a==AST::TypeUser) {
                if (b!=AST::TypeUser) {
                    err = _("Can't %1:=%2", leftExpr->baseType.name, lexer->classNameByBaseType(rightExpr->baseType.kind));
                }
                else if (b==AST::TypeUser) {
                    err = _("Can't %1:=%2", leftExpr->baseType.name, rightExpr->baseType.name);
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
    st.statement->type = AST::StAssign;
    st.statement->expressions << rightExpr;
    if (leftExpr)
        st.statement->expressions << leftExpr;
}

void SyntaxAnalizerPrivate::parseAlgHeader(int str, bool onlyName, bool allowOperatorsDeclaration)
{
    const Statement & st = statements[str];
    if (st.hasError() || !st.mod ||!st.alg)
        return;
    AST::Algorhitm * alg = st.alg;
    AST::Module * mod = st.mod;
    Q_CHECK_PTR(alg);
    Q_CHECK_PTR(mod);
    QString name;
    bool isOperator = false;
    bool isFirst = mod->header.name.isEmpty() && mod->impl.algorhitms.indexOf(alg)==0;
    int argsStartLexem = -1;
    int nameStartLexem = 1;
    if (st.data.size()>1 && st.data[1]->type==LxNameClass) {
        nameStartLexem = 2;
        AST::VariableBaseType bt = lexer->baseTypeByClassName(st.data[1]->data);
        if (bt==AST::TypeNone) {
            AST::Type tp;
            AST::Module * typeMod = 0;
            if (findUserType(st.data[1]->data, tp, typeMod)) {
                alg->header.returnType = tp;
            }
            else {
                alg->header.returnType.kind = AST::TypeNone;
            }
        }
        else {
            alg->header.returnType.kind = bt;
        }
        if (lexer->isArrayClassName(st.data[1]->data)) {
            alg->header.returnType.kind = AST::TypeNone;
            st.data[1]->error = _("Algorhitms can't return array");
            return;
        }

    }
    for (int i=nameStartLexem; i<st.data.size(); i++) {
        if (st.data[i]->type==LxNameClass) {
            st.data[i]->error = _("Name contains keyword");
            return;
        }
        else if (st.data[i]->type & LxTypeName) {
            if (i>nameStartLexem)
                name += " ";
            name += st.data[i]->data;
            st.data[i]->type=LxNameAlg;
        }
        else if (st.data[i]->type == LxOperLeftBr ) {
            argsStartLexem = i+1;
            break;
        }
        else if (allowOperatorsDeclaration) {
            if (i>nameStartLexem)
                name += " ";
            name += st.data[i]->data;
            isOperator = true;
        }
        else if  (st.data[i]->type==LxNameClass
                 || st.data[i]->type & LxTypePrimaryKwd
                 || st.data[i]->type & LxTypeSecondaryKwd)
        {
            st.data[i]->error = _("Keyword in name");
            return;
        }
        else {
            st.data[i]->error = _("Operator in name");
            return;
        }
    }

    if ( isFirst )
    {
        if ( name.isEmpty() )
        {
            if ( argsStartLexem!=-1 )
            {
                for (int i=argsStartLexem-1; i<st.data.size(); i++)
                    st.data[i]->error = _("Extra arguments");
                return;
            }
            if ( alg->header.returnType.kind!=AST::TypeNone )
            {
                st.data[1]->error = _("First unnamed algorithm should not return anything");
                alg->header.returnType.kind=AST::TypeNone;
                return;
            }
        }
    }
    else
    {
        // если алгоритм не первый, обязательно должно быть имя, иначе -- ошибка
        if ( name.isEmpty() )
        {
            for (int i=0; i<st.data.size(); i++) {
                st.data[i]->error = _("No algorithm name");
            }
            return ;
        }
    }

    // Проверяем на повторное описание алгоритма
    AST::Algorhitm * aa;
    if (!isOperator && findAlgorhitm(name,st.mod,aa) && aa!=alg)
    {
        for (int i=1; i<st.data.size(); i++) {
            if (st.data[i]->type==LxNameAlg)
                st.data[i]->error = _("The name is used by other algorithm");
        }
        return ;
    }

    // Проверяем на наличие переменной с таким же именем

    AST::Variable * vv;
    if (!isOperator && findGlobalVariable(name, st.mod, vv)) {
        for (int i=1; i<st.data.size(); i++) {
            if (st.data[i]->type==LxNameAlg)
                st.data[i]->error = _("The name is used by global variable");
        }
        return ;
    }

    if (alg->header.returnType.kind!=AST::TypeNone) {
        AST::Variable * retVar = new AST::Variable;
        retVar->accessType = AST::AccessRegular;
        retVar->baseType = alg->header.returnType;
        retVar->name = name;
        alg->impl.locals << retVar;
    }



    // Заносим алгоритм в таблицу функций
    alg->header.name = name;

    if (onlyName)
        return;

    // Make this algorhitm public available (if not private name)
    if (!name.isEmpty() && !name.startsWith("_")) {
        if (isOperator)
            mod->header.operators << alg;
        else
            mod->header.algorhitms << alg;
    }

    for (int i=nameStartLexem; i<argsStartLexem-1; i++) {
        st.data[i]->type = LxNameAlg;
    }

    if (alg->header.specialType==AST::AlgorhitmTypeTeacher && alg->header.name==lexer->testingAlgorhitmName()) {
        alg->header.specialType = AST::AlgorhitmTypeTesting;
    }

    // Если нет аргументов, то всё

    if ( argsStartLexem==-1 ) {
        return;
    }

    // =============== Argument list parsing


    QList<VariablesGroup> groups;
    if (st.data.last()->type!=LxOperRightBr) {
        alg->header.error = st.data[argsStartLexem-1]->error= _("Unpaired '('");
        return;
    }
    VariablesGroup currentGroup;
    currentGroup.access = AST::AccessArgumentIn;
    currentGroup.accessDefined = false;
    currentGroup.groupLexem = 0;
    for (int i=argsStartLexem; i<st.data.size()-1; i++) {
        if (st.data[i]->type==LxTypeComment)
            break;
        LexemType type = st.data[i]->type;
        if (type==LxSecIn || type==LxSecOut || type==LxSecInout) {
            if (!currentGroup.lexems.isEmpty() && currentGroup.lexems.last()->type==LxOperComa) {
                currentGroup.lexems.pop_back();
            }
            else if (!currentGroup.lexems.isEmpty()) {
                alg->header.error = st.data[i]->error = _("No coma before %1", st.data[i]->data);
                return;
            }
            if (!currentGroup.lexems.isEmpty()) {
                groups << currentGroup;
                currentGroup.lexems.clear();
                currentGroup.accessDefined = false;
            }
            if (type==LxSecIn) {
                if (currentGroup.accessDefined) {
                    alg->header.error = st.data[i]->error = _("Extra variable group in-specifier");
                }
                else {
                    currentGroup.accessDefined = true;
                    currentGroup.access = AST::AccessArgumentIn;
                }
            }
            else if (type==LxSecOut) {
                if (currentGroup.accessDefined) {
                    alg->header.error = st.data[i]->error = _("Extra variable group out-specifier");
                }
                else {
                    currentGroup.accessDefined = true;
                    currentGroup.access = AST::AccessArgumentOut;
                }
            }
            else if (type==LxSecInout) {
                if (currentGroup.accessDefined) {
                    alg->header.error = st.data[i]->error = _("Extra variable group inout-specifier");
                }
                else {
                    currentGroup.accessDefined = true;
                    currentGroup.access = AST::AccessArgumentInOut;
                }
            }
            currentGroup.groupLexem = st.data[i];
        }
        else if (type==LxPriAlgHeader) {
            alg->header.error = st.data[i]->error = _("'alg' instead of 'arg'");
            return;
        }
        else if (type==LxOperComa && currentGroup.lexems.isEmpty()) {
            if (currentGroup.groupLexem==0) {
                alg->header.error = st.data[i]->error = _("Extra ','");
                return;
            }
            else {
                alg->header.error = currentGroup.groupLexem->error = _("No variables declared after '%1'", currentGroup.groupLexem->data);
                return;
            }
        }
        else {
            currentGroup.lexems << st.data[i];
        }
    }
    if (!currentGroup.lexems.isEmpty())
        groups << currentGroup;

    QString localError;

    if (groups.isEmpty()) {
        const QString err = _("Extra brackets for algorithm without arguments");
        alg->header.error = st.data[argsStartLexem-1]->error = st.data.last()->error = err;
        return;
    }

    for (int i=0; i<groups.size(); i++) {
        QList<AST::Variable*> vars = parseVariables(str, groups[i], st.mod, st.alg, true);
        for (int j=0; j<vars.size(); j++) {
            alg->header.arguments << vars[j];
        }
        bool mustStop = false;
        for (int j=0; j<groups[i].lexems.size()-1; j++) {
            if (!groups[i].lexems[j]->error.isEmpty()) {
                localError = groups[i].lexems[j]->error;
                mustStop = true;
                break;
            }
        }
        if (mustStop)
            break;
    }

    alg->header.error = localError;
}

bool hasFunction(const AST::Expression * expr, QList<Lexem*> & lexems)
{
    if (expr->kind==AST::ExprFunctionCall) {
        lexems += expr->lexems;
        return true;
    }
    else if (expr->kind==AST::ExprSubexpression) {
        foreach (const AST::Expression * sub, expr->operands) {
            if (hasFunction(sub,lexems))
                return true;
        }
    }
    return false;
}

bool hasArrayElement(const AST::Expression * expr, QList<Lexem*> & lexems)
{
    if (expr->kind==AST::ExprArrayElement) {
        lexems += expr->lexems;
        return true;
    }
    else if (expr->kind==AST::ExprSubexpression) {
        foreach (const AST::Expression * sub, expr->operands) {
            if (hasArrayElement(sub,lexems))
                return true;
        }
    }
    return false;
}

QList<AST::Variable*> SyntaxAnalizerPrivate::parseVariables(int statementIndex, VariablesGroup &group, AST::Module *mod, AST::Algorhitm *alg, bool algHeader)
{
    // First -- check brackets

    QStack<Lexem *> openBrackets;
    QStack<Lexem *> openSqBrackets;
    QStack<Lexem *> openBraces;

    const QList<Lexem*> lexems = group.lexems;


    for (int i=0; i<lexems.size(); i++) {
        Lexem * lx = lexems.at(i);
        if (lx->type==LxOperLeftBr) {
            openBrackets.push_back(lx);
        }
        else if (lx->type==LxOperLeftSqBr) {
            openSqBrackets.push_back(lx);
        }
        else if (lx->type==LxOperLeftBrace) {
            openBraces.push_back(lx);
        }
        else if (lx->type==LxOperRightBr) {
            if (openBrackets.size()==0) {
                lx->error = _("Unpaired )");
                return QList<AST::Variable*>();
            }
            else {
                openBrackets.pop_back();
            }
        }
        else if (lx->type==LxOperRightSqBr) {
            if (openSqBrackets.size()==0) {
                lx->error = _("Unpaired ]");
                return QList<AST::Variable*>();
            }
            else {
                openSqBrackets.pop_back();
            }
        }
        else if (lx->type==LxOperRightBrace) {
            if (openBraces.size()==0) {
                lx->error = _("Unpaired }");
                return QList<AST::Variable*>();
            }
            else {
                openBraces.pop_back();
            }
        }
    }

    foreach (Lexem * lx, openBrackets) {
        lx->error = _("Unpaired (");
    }
    foreach (Lexem * lx, openSqBrackets) {
        lx->error = _("Unpaired [");
    }
    foreach (Lexem * lx, openBraces) {
        lx->error = _("Unpaired {");
    }

    if (openBrackets.size()+openSqBrackets.size()+openBraces.size() > 0) {
        return QList<AST::Variable*>();
    }

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
    AST::Type cType; // тип текущей величины
    cType.kind = AST::TypeNone;
    bool array = false;
    QString cName = ""; // имя текущей величины
    QList<Lexem*> cBound; // текущая граница массива
    QList<Lexem*> initValue; // значение инициализации
    QList<AST::Variable::Bound> bounds;
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
        if (group.lexems[curPos]->type==LxSecIn) {
            group.lexems[curPos]->error = _("Extra variable group in-specifier");
            return result;
        }
        else if (group.lexems[curPos]->type==LxSecOut) {
            group.lexems[curPos]->error = _("Extra variable group out-specifier");
            return result;
        }
        else if (group.lexems[curPos]->type==LxSecInout) {
            group.lexems[curPos]->error = _("Extra variable group inout-specifier");
            return result;
        }
        else if ( ( par == type ) || ( par == tn ) )
        {
            // Check if no coma before not first declaration
            if ( curPos>0
                 && group.lexems[curPos]->type!=LxOperComa
                 && group.lexems[curPos]->type!=LxOperSemicolon
                 && group.lexems[curPos]->type!=LxOperEqual
                 && group.lexems[curPos-1]->type!=LxOperComa
                 && group.lexems[curPos-1]->type!=LxOperSemicolon
                 )
            {
                group.lexems[curPos]->error = _("No coma before declaration");
                return result;
            }

            //          Pазбираемся с типом переменных
            if (group.lexems[curPos]->type==LxNameClass) {
                AST::VariableBaseType bt = lexer->baseTypeByClassName(group.lexems[curPos]->data);
                if (bt==AST::TypeNone) {
                    AST::Type userType;
                    AST::Module * userTypeModule = 0;
                    if (findUserType(group.lexems[curPos]->data, userType, userTypeModule)) {
                        cType = userType;
                    }
                }
                else {
                    cType.kind = bt;
                }
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
            if (group.lexems[curPos]->type == LxNameClass) {
                group.lexems[curPos]->error = cName.isEmpty()
                        ? _("Extra type")
                        : _("No coma before type");
                return result;
            }
            if (group.lexems[curPos]->data==QString::fromAscii("$") || group.lexems[curPos]->data==QString::fromAscii(".")) {
                group.lexems[curPos]->type = LxTypeEmpty;
                group.lexems[curPos]->error = _("Bad symbol in name");
                return result;
            }
            if (group.lexems[curPos]->type & LxTypeConstant) {
                group.lexems[curPos]->error = _("Constant can not be a name");
                return result;
            }
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
                        for (int a=curPos; a<group.lexems.size(); a++) {
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
                    int index = typePos;
                    if (nameStart-1>=0)
                        index = nameStart-1;
                    group.lexems[index]->error = _("Variable name is empty");
                    return result;
                }
                QString loc_err = lexer->testName(cName);
                if (!loc_err.isEmpty()) {
                    int endPos = curPos;
                    if (arrayStart>nameStart)
                        endPos = arrayStart;
                    for (int k=nameStart; k<endPos; k++)
                        group.lexems[k]->error = loc_err;
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
                AST::Variable * vv = 0;
                if (alg) {
                    if (findLocalVariable(cName, alg, vv)) {
                        group.lexems[nameStart]->error = _("Variable already declared");
                        return result; // переменная уже объявлена
                    }
                }
                else {
                    if (findGlobalVariable(cName, mod, vv)) {
                        group.lexems[nameStart]->error = _("Variable already declared");
                        return result; // переменная уже объявлена
                    }
                }

                AST::Algorhitm * aa;
                if (findAlgorhitm(cName, mod, aa)) {
                    group.lexems[nameStart]->error = _("Name is used by algorithm");
                }
                if (array && !massDeclared /*&& group.lexems[curPos]->type!=LxOperEqual*/)
                {
                    group.lexems[nameStart]->error = _("Array bounds no specified");
                    return result; // нет границ
                }
                var = new AST::Variable;
                var->name = cName;
                var->baseType = cType;
                var->accessType = group.access;
                var->dimension = array? dim : 0;
                var->bounds = bounds;
                result << var;
                statements[statementIndex].variables << var;
                if (alg)
                    alg->impl.locals << var;
                else
                    mod->impl.globals << var;
                cName = "";
                par = tn;
                dim = 0;
                massDeclared = false;
                nameStart = curPos+1;
                if (group.lexems[curPos]->type==LxOperEqual) {
                    par = value;
                    z = curPos+1;
                }
                bounds.clear();
            }
            else if (group.lexems[curPos]->type & LxTypePrimaryKwd ||
                     group.lexems[curPos]->type & LxTypeSecondaryKwd) {
                group.lexems[curPos]->error = _("Name contains keyword");
                return result;
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

                if (algHeader) {
                    QList<Lexem*> leftFunction;
                    QList<Lexem*> leftArray;

                    if (hasFunction(left, leftFunction)) {
                        foreach ( Lexem * a, leftFunction )
                            a->error = _("Function in array bound");
                        delete left;
                        return result;
                    }
                    if (hasArrayElement(left, leftArray)) {
                        foreach ( Lexem * a, leftArray )
                            a->error = _("Array element in array bound");
                        delete left;
                        return result;
                    }
                }

                if (left->baseType.kind!=AST::TypeInteger)
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
                QPair<AST::Expression*,AST::Expression*> bound(0, 0);
                bounds.append(bound);
                bounds.last().first = left;
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
                if (algHeader) {
                    QList<Lexem*> rightFunction;
                    QList<Lexem*> rightArray;

                    if (hasFunction(right, rightFunction)) {
                        foreach ( Lexem * a, rightFunction )
                            a->error = _("Function in array bound");
                        delete right;
                        return result;
                    }

                    if (hasArrayElement(right, rightArray)) {
                        foreach ( Lexem * a, rightArray )
                            a->error = _("Array element in array bound");
                        delete right;
                        return result;
                    }
                }


                if (right->baseType.kind!=AST::TypeInteger)
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
                bounds.last().second = right;
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

                if (algHeader) {
                    QList<Lexem*> rightFunction;
                    QList<Lexem*> rightArray;

                    if (hasFunction(right, rightFunction)) {
                        foreach ( Lexem * a, rightFunction )
                            a->error = _("Function in array bound");
                        delete right;
                        return result;
                    }

                    if (hasArrayElement(right, rightArray)) {
                        foreach ( Lexem * a, rightArray )
                            a->error = _("Array element in array bound");
                        delete right;
                        return result;
                    }
                }
                if (right->baseType.kind!=AST::TypeInteger)
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
                bounds.last().second = right;

            }
            else if (group.lexems[curPos]->type==LxOperSemicolon) {
                const QString err = _("No pairing ']'");

                 group.lexems[arrayStart]->error = err;

                return result;
            }
            else {
                cBound << group.lexems[curPos];
            }
        } // endif ( par == rb )
        else if ( par==value ) {
            if (group.access!=AST::AccessRegular) {
                group.lexems[z-1]->error = _("Initial values should not be specified in algorithm header");
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
                QVariant constValue = parseConstant(initValue.toStdList(), var->baseType.kind, maxDim);
                if (constValue==QVariant::Invalid) {
                    return result;
                }
                if (var->dimension==0 && lexer->isArrayClassName(group.lexems[typePos]->data)) {
                    if (maxDim>3) {
                        for (int a=z-1; a<=curPos; a++) {
                            group.lexems[a]->error = _("Table dimension > 3");
                            return result;
                        }
                    }
                    var->dimension = maxDim;
                }
                if (var->dimension>0) {
                    if (var->dimension!=maxDim) {
                        for (int a=z-1; a<=curPos; a++) {
                            group.lexems[a]->error = _("Unmatched array dimensions");
                            return result;
                        }
                    }
                }
                else if (maxDim>0) {
                    for (int a=z-1; a<=curPos; a++) {
                        group.lexems[a]->error = _("Must be a scalar constant");
                        return result;
                    }
                }
                var->initialValue = constValue;

                par = tn;
                initValue.clear();
            }
            else if (deepV!=0 && group.lexems[curPos]->type==LxOperSemicolon) {
                if (deepV>0) {
                    // extra open
                    for (int i=z; i<curPos; i++) {
                        if (group.lexems[i]->type==LxOperLeftBrace) {
                            group.lexems[i]->error = _("Extra open brace");
                            return result;
                        }
                    }
                }
                else {
                    // extra close
                    for (int i=curPos-1; i>z; i--) {
                        if (group.lexems[i]->type==LxOperRightBrace) {
                            group.lexems[i]->error = _("Extra close brace");
                            return result;
                        }
                    }
                }
            }
            else {
                initValue << group.lexems[curPos];
            }
        } // endif ( par==value )
    }
    return result;
}

QVariant SyntaxAnalizerPrivate::parseConstant(const std::list<Lexem*> &constant
                                              , const AST::VariableBaseType pt
                                              , int& maxDim
                                              ) const
{
    int localErr = 0;
    AST::VariableBaseType ct;
    maxDim = 0;
    if (constant.size()==0)
        return QVariant::Invalid;
    if (constant.size()==1 && constant.front()->data.trimmed()=="...") {
        constant.front()->error = _("Constant value not typed");
        return QVariant::Invalid;
    }
    bool array = false;
    if (constant.front()->type==LxOperLeftBrace) {
        if (constant.back()->type==LxOperRightBrace)
            array = true;
        else {
            constant.front()->error = _("Extra open brace");
            return QVariant::Invalid;
        }
    }
    if (constant.back()->type==LxOperRightBrace) {
        if (constant.front()->type==LxOperLeftBrace)
            array = true;
        else {
            constant.back()->error = _("Extra close brace");
            return QVariant::Invalid;
        }
    }
    if (!array) {
        maxDim = 0;
        ct = testConst(constant, localErr);
        if ( ct==AST::TypeNone ) {
            for (std::list<Lexem*>::const_iterator it = constant.begin(); it!=constant.end(); it++) {
                Lexem * lx = *it;
                lx->error = _("Not a constant value");
            }
            return QVariant::Invalid;
        }
        else if ( ( ct==pt ) ||
                  (ct==AST::TypeInteger && pt==AST::TypeReal) ||
                  (ct==AST::TypeCharect && pt==AST::TypeString))
        {
            QString val;
            for (std::list<Lexem*>::const_iterator it=constant.begin(); it!=constant.end(); it++) {
                val += (*it)->data;
            }
            bool integerOverflow = false;
            bool isHex = false;

            if (pt==AST::TypeInteger) {
                integerOverflow = !Kumir::Math::isCorrectIntegerConstant(val.toStdWString());
                isHex = val.startsWith("$") || val.startsWith("-$") || val.startsWith("0x") || val.startsWith("-0x");
            }
            if (ct==AST::TypeReal ||
                    (pt==AST::TypeReal || (integerOverflow && !isHex) )
                    )
            {
                ct = AST::TypeReal;
                Kumir::Converter::ParseError local_error = Kumir::Converter::NoError;
                Kumir::real value = Kumir::Converter::parseReal(val.toStdWString(), Kumir::Char('.'), local_error);
                QString err;
                if (local_error==Kumir::Converter::WrongReal) {
                    err = _("Wrong real number");
                }
                else if (local_error==Kumir::Converter::WrongExpForm) {
                    err = _("Wrong E-real number");
                }
                else if (local_error==Kumir::Converter::BadSymbol) {
                    err = _("Wrong real number");
                }
                else if (local_error==Kumir::Converter::Overflow) {
                    err = _("Too big real value");
                }
                if (err.length()>0) {
                    for (std::list<Lexem*>::const_iterator it = constant.begin(); it!=constant.end(); it++) {
                        Lexem * lx = * it;
                        lx->error = err;
                        return QVariant::Invalid;
                    }
                }
                return QVariant(value);
            }

            if (pt==AST::TypeInteger && integerOverflow) {
                if (val.startsWith("$")) {
                    for (std::list<Lexem*>::const_iterator it = constant.begin(); it!=constant.end(); it++) {
                        Lexem * lx = * it;
                        lx->error = _("Integer constant too big");
                    }
                    return QVariant::Invalid;
                }
            }
            return createConstValue(val, ct);
        }
        else {
            for (std::list<Lexem*>::const_iterator it = constant.begin(); it!=constant.end(); it++) {
                Lexem * lx = *it;
                lx->error = _("Constant type mismatch");
            }
            return QVariant::Invalid; // FIXME: type mismatch
        }
    }
    else {
        maxDim = 0;
        std::list<Lexem*> alist = constant;
        alist.pop_front();
        alist.pop_back();
        std::list< std::list<Lexem*> > values;
        typedef std::list< std::list<Lexem*> >::const_iterator values_iterator;
        std::list<Lexem*> operators;
        splitLexemsByOperator(alist, LxOperComa, values, operators);
        QVariantList result;
        QList<int> dimensions;
        for (values_iterator it=values.begin(); it!=values.end(); ++it) {
            int valDim = 0;
            QVariant value = QVariant::Invalid;
            if ( (*it).size()>0 ) {
                value = parseConstant(*it, pt, valDim);
                if (value==QVariant::Invalid)
                    return QVariant::Invalid;
                else {
                    result << value;
                    dimensions << valDim;
                }
            }
            else {
                result << QVariant::Invalid;
                dimensions << -1;
            }
        }
        if (dimensions.size()>0) {
            int firstDim = -1;
            std::list<Lexem*> firstDimLexems;
            Q_ASSERT(dimensions.size()==values.size());
            int i = 0;
            values_iterator itt = values.begin();
            for (i=0; i<dimensions.size(); i++, ++itt) {
                if (dimensions[i]!=-1) {
                    firstDim = dimensions[i];
                    firstDimLexems = (*itt);
                    break;
                }
            }
            i = 0;
            for (values_iterator it=values.begin(); it!=values.end(); ++it, i++) {
                if (dimensions[i]==-1)
                    continue;
                if (dimensions[i]!=firstDim) {
                    for (std::list<Lexem*>::const_iterator lx=it->begin(); lx!=it->end(); lx++) {
                        (*lx)->error = _("Table constant element of variant type");
                    }
                    for (std::list<Lexem*>::const_iterator lx=itt->begin(); lx!=itt->end(); lx++) {
                        (*lx)->error = _("Table constant element of variant type");
                    }
                    return QVariant::Invalid;
                }
            }
            if (firstDim==-1) {
                maxDim = 1;
            }
            else {
                maxDim = firstDim + 1;
            }
        }
        else {
            maxDim = 1;
        }
        return result;
    }
    return QVariant::Invalid;
}

AST::VariableBaseType SyntaxAnalizerPrivate::testConst(const std::list<Lexem*> &lxs, int &err) const
{
    err = 0;
    Lexem * lx = 0;
    if (lxs.size()==1) {
        lx = *lxs.begin();
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
    }
    else if (lxs.size()==2) {
        std::list<Lexem*>::const_iterator it = lxs.begin();
        Lexem * llx = *it;
        it++;
        lx = *it;
        if (llx->type==LxOperPlus || llx->type==LxOperMinus) {
            if (lx->type==LxConstInteger)
                return AST::TypeInteger;
            else if (lx->type==LxConstReal)
                return AST::TypeReal;
        }
    }

    // TODO implement overflow range checking
    return AST::TypeNone;
}

QVariant SyntaxAnalizerPrivate::createConstValue(const QString & str
                                                 , const AST::VariableBaseType type) const
{
    QVariant result = QVariant::Invalid;
    if (type==AST::TypeBoolean) {
        result = QVariant(lexer->boolConstantValue(str));
    }
    else if (type==AST::TypeCharect) {
        result = QVariant(QChar(str[0]));
    }
    else if (type==AST::TypeInteger) {
        bool ok;
        if (str.startsWith("$")) {
            result = QVariant(str.mid(1).toInt(&ok, 16));
        }
        else {
            result = QVariant(str.toInt());
        }
    }
    else if (type==AST::TypeReal) {
        result = QVariant(str.toDouble());
    }
    else if (type==AST::TypeString) {
        result = QVariant(str);
    }
    return result;
}

bool SyntaxAnalizerPrivate::findAlgorhitm(const QString &name, const AST::Module *module, AST::Algorhitm *&algorhitm)
{
    algorhitm = 0;
    for (int i=0; i<ast->modules.size(); i++) {
        if (ast->modules[i]->header.enabled || ast->modules[i]->header.type==AST::ModTypeHidden) {
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

bool SyntaxAnalizerPrivate::tryInputOperatorAlgorithm(
        const QString & lexem,
        AST::Type & type,
        QVariant & constantValue
        ) const
{

    AST::AlgorhitmExternalReference ref;
    for (int i=0; i<ast->modules.size(); i++) {
        const AST::Module * mod = ast->modules[i];
        if (!mod->header.enabled)
            continue;
        for (int j=0; j<mod->header.operators.size(); j++) {
            const AST::Algorhitm * alg = mod->header.operators[j];
            if (alg->header.arguments.size()==2
                    && alg->header.arguments[0]->baseType.kind==AST::TypeString
                    && alg->header.arguments[0]->dimension==0
                    && alg->header.arguments[1]->baseType.kind==AST::TypeBoolean
                    && alg->header.arguments[1]->dimension==0
                    && alg->header.arguments[1]->accessType==AST::AccessArgumentOut
                    && alg->header.returnType.kind!=AST::TypeNone
                    && alg->header.name==lexer->inputLexemName()
                    )
            {
                ref = alg->header.external;
                Shared::ActorInterface * actor = 0;
                QList<ExtensionSystem::KPlugin*> actors = ExtensionSystem::PluginManager::instance()->loadedPlugins("Actor*");
                for (int k=0; k<actors.size(); k++) {
                    actor = qobject_cast<Shared::ActorInterface*>(actors[k]);
                    if (!actor) continue;
                    if (actor->name()==mod->header.name)
                        break;
                    else
                        actor = 0;
                }
                if (!actor)
                    continue;
                QVariantList args;
                args << QVariant(lexem);
                Shared::EvaluationStatus es = actor->evaluate(ref.id, args);
                if (es==ES_StackRezResult) {
                    QVariant result = actor->result();
                    QVariantList retvals = actor->algOptResults();
                    if (retvals.size()==1 && retvals.at(0).toBool()) {
                        constantValue = result;
                        type = alg->header.returnType;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool SyntaxAnalizerPrivate::findConversionAlgorithm(const AST::Type & from
                             , const AST::Type & to
                             , AST::Module * mod
                             , AST::Algorhitm * alg) const
{
    for (int i=0; i<ast->modules.size(); i++) {
        mod = ast->modules[i];
        if (!mod->header.enabled)
            continue;
        for (int j=0; j<mod->header.operators.size(); j++) {
            alg = mod->header.operators[j];
            if (alg->header.arguments.size()==1) {
                if (alg->header.arguments[0]->baseType==from
                        && alg->header.arguments[0]->dimension==0)
                {
                    if (alg->header.returnType==to)
                        return true;
                }
            }
        }
    }
    mod = 0;
    alg = 0;
    return false;
}

AST::Expression * SyntaxAnalizerPrivate::makeCustomUnaryOperation(
        const QString &operatorName
        , AST::Expression *argument)
{
    QString argTypeName;
    if (argument->baseType.kind==AST::TypeUser)
        argTypeName = argument->baseType.name;
    else
        argTypeName = lexer->classNameByBaseType(argument->baseType.kind);
    for (int i=0; i<ast->modules.size(); i++) {
        AST::Module * mod = ast->modules[i];
        if (!mod->header.enabled)
            continue;
        for (int j=0; j<mod->header.operators.size(); j++) {
            AST::Algorhitm * alg = mod->header.operators[j];
            bool omatch = alg->header.name==operatorName;
            if (omatch && alg->header.arguments.size()==1) {
                AST::Type formaltype = alg->header.arguments[0]->baseType;
                AST::Type facttype   = argument->baseType;
                AST::Algorhitm * convAlg = 0;
                AST::Module * convMod = 0;
                bool typematch = formaltype.name==facttype.name
                        || findConversionAlgorithm(facttype, formaltype, convMod, convAlg);
                if (typematch) {
                    AST::Expression * res = new AST::Expression;
                    res->kind = AST::ExprFunctionCall;
                    res->baseType = alg->header.returnType;
                    res->function = alg;
                    if (convAlg) {
                        AST::Expression * convExpr = new AST::Expression;
                        convExpr->kind = AST::ExprFunctionCall;
                        convExpr->function = convAlg;
                        convExpr->operands << argument;
                        res->operands << convExpr;
                    }
                    else {
                        res->operands << argument;
                    }
                    return res;
                }
            }
        }
    }
    return 0;
}

AST::Expression * SyntaxAnalizerPrivate::makeCustomBinaryOperation(const QString & operatorName
                                               , AST::Expression * leftExpression
                                               , AST::Expression * rightExpression
                                               )
{
    QString headTypeName;
    if (leftExpression->baseType.kind==AST::TypeUser)
        headTypeName = leftExpression->baseType.name;
    else
        headTypeName = lexer->classNameByBaseType(leftExpression->baseType.kind);
    QString tailTypeName;
    if (rightExpression->baseType.kind==AST::TypeUser)
        tailTypeName = rightExpression->baseType.name;
    else
        tailTypeName = lexer->classNameByBaseType(rightExpression->baseType.kind);
    for (int i=0; i<ast->modules.size(); i++) {
        AST::Module * mod = ast->modules[i];
        if (!mod->header.enabled)
            continue;
        for (int j=0; j<mod->header.operators.size(); j++) {
            AST::Algorhitm * alg = mod->header.operators[j];
            bool omatch = alg->header.name==operatorName;
            if (omatch && alg->header.arguments.size()==2) {
                AST::Type ltype = alg->header.arguments[0]->baseType;
                AST::Type rtype = alg->header.arguments[1]->baseType;
                AST::Algorhitm * lconvAlg = 0;
                AST::Algorhitm * rconvAlg = 0;
                AST::Module * lconvMod = 0;
                AST::Module * rconvMod = 0;
                bool lmatch = ltype.name==headTypeName
                        || findConversionAlgorithm(leftExpression->baseType, ltype, lconvMod, lconvAlg);
                bool rmatch = rtype.name==tailTypeName
                        || findConversionAlgorithm(rightExpression->baseType, rtype, rconvMod, rconvAlg);
                if (lmatch && rmatch && omatch) {

                    AST::Expression * res = new AST::Expression;
                    res->kind = AST::ExprFunctionCall;
                    res->baseType = alg->header.returnType;
                    res->function = alg;
                    if (lconvAlg) {
                        AST::Expression * lconv = new AST::Expression;
                        lconv->kind = AST::ExprFunctionCall;
                        lconv->function = lconvAlg;
                        lconv->operands.append(leftExpression);
                        res->operands.append(lconv);
                    }
                    else {
                        res->operands.append(leftExpression);
                    }

                    if (rconvAlg) {
                        AST::Expression * rconv = new AST::Expression;
                        rconv->kind = AST::ExprFunctionCall;
                        rconv->function = rconvAlg;
                        rconv->operands.append(rightExpression);
                        res->operands.append(rconv);
                    }
                    else {
                        res->operands.append(rightExpression);
                    }

                    return res;
                }
            }
        }
    }
    return 0;
}

bool SyntaxAnalizerPrivate::findGlobalVariable(const QString &name, const AST::Module *module, AST::Variable *&var)
{
    var = 0;
    for (int i=0; i<module->impl.globals.size(); i++) {
        AST::Variable * v = module->impl.globals[i];
        if (v->name==name) {
            var = v;
            break;
        }
    }
    return var!=0;
}

bool SyntaxAnalizerPrivate::findLocalVariable(const QString &name, const AST::Algorhitm *alg, AST::Variable *&var)
{
    var = 0;
    for (int i=0; i<alg->impl.locals.size(); i++) {
        AST::Variable * v = alg->impl.locals[i];
        if (v->name==name) {
            var = v;
            break;
        }
    }
    return var!=0;
}

bool SyntaxAnalizerPrivate::findUserType(const QString &name, AST::Type &type, AST::Module *module)
{
    module = 0;
    for (int i=0; i<ast->modules.size(); i++) {
        AST::Module * mod = ast->modules[i];
        if (mod->header.enabled) {
            for (int j=0; j<mod->header.types.size(); j++) {
                AST::Type tp = mod->header.types[j];
                if (tp.name==name) {
                    module = mod;
                    type = tp;
                    return true;
                }
            }
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

#define IS_OPERATOR(x) (x & LxTypeOperator || x==LxOperGreaterOrEqual || x==LxSecOr || x==LxSecAnd || x==LxPriAssign/* || x==LxSecNot */)

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

    // First -- check brackets

    QStack<Lexem *> openBrackets;
    QStack<Lexem *> openSqBrackets;
    QStack<Lexem *> openBraces;


    for (int i=0; i<lexems.size(); i++) {
        Lexem * lx = lexems.at(i);
        if (lx->type==LxOperLeftBr) {
            openBrackets.push_back(lx);
        }
        else if (lx->type==LxOperLeftSqBr) {
            openSqBrackets.push_back(lx);
        }
        else if (lx->type==LxOperLeftBrace) {
            openBraces.push_back(lx);
        }
        else if (lx->type==LxOperRightBr) {
            if (openBrackets.size()==0) {
                lx->error = _("Unpaired )");
                return 0;
            }
            else {
                openBrackets.pop_back();
            }
        }
        else if (lx->type==LxOperRightSqBr) {
            if (openSqBrackets.size()==0) {
                lx->error = _("Unpaired ]");
                return 0;
            }
            else {
                openSqBrackets.pop_back();
            }
        }
        else if (lx->type==LxOperRightBrace) {
            if (openBraces.size()==0) {
                lx->error = _("Unpaired }");
                return 0;
            }
            else {
                openBraces.pop_back();
            }
        }
    }

    foreach (Lexem * lx, openBrackets) {
        lx->error = _("Unpaired (");
    }
    foreach (Lexem * lx, openSqBrackets) {
        lx->error = _("Unpaired [");
    }
    foreach (Lexem * lx, openBraces) {
        lx->error = _("Unpaired {");
    }

    if (openBrackets.size()+openSqBrackets.size()+openBraces.size() > 0) {
        return 0;
    }

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

        if (oper && oper->type==LxOperComa) {
            oper->error = _("Extra coma");
            return 0;
        }

        if (oper && oper->type==LxOperColon) {
            oper->error = _("Extra colon");
            return 0;
        }

        if (oper && oper->type==LxOperRightBr) {
            oper->error = _("Extra )");
            return 0;
        }

        if (oper && oper->type==LxOperRightSqBr) {
            oper->error = _("Extra ]");
            return 0;
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
//                if (operand->baseType!=AST::TypeBoolean) {
//                    delete operand;
//                    notFlag->error = _("Use of 'not' for non-boolean value");
//                    return 0;
//                }
                subexpression << notFlag;
            }
            curPos++;
            if (curPos>=lexems.size()) {
                oper = 0;
            }
            else if (IS_OPERATOR(lexems[curPos]->type)) {
                oper = lexems[curPos];
            }
            else {
                lexems[curPos]->error = _("Forgotten operator after bracket"); // FIXME garbage error
                delete operand;
                return 0;
            }
            subexpression << operand;
            if (oper)
                subexpression << oper;
            if (subexpression.size()>1 &&
                    operand->baseType.kind==AST::TypeNone &&
                    operand->kind==AST::ExprFunctionCall
            )
            {
                for (int i=0; i<block.size(); i++) {
                    block[i]->error = _("Assignment of non-returning algorithm");
                }
                delete operand;
                return 0;
            }
        } // end if (blockType==Function)
        else if (blockType==Simple) {
            if (block.isEmpty() && notFlag) {
                notFlag->error = _("Extra 'not'");
                return 0;
            }
            AST::Expression * operand = parseSimpleName(block.toStdList(), mod, alg);
            if (!operand) {
                return 0;
            }
            if (notFlag) {
//                if (operand->baseType!=AST::TypeBoolean) {
//                    delete operand;
//                    notFlag->error =  _("Use of 'not' for non-boolean value");
//                    return 0;
//                }
                subexpression << notFlag;
            }
            subexpression << operand;
            if (oper)
                subexpression << oper;
            if (subexpression.size()>1 &&
                    operand->baseType.kind==AST::TypeNone &&
                    operand->kind==AST::ExprFunctionCall
            )
            {
                for (int i=0; i<block.size(); i++) {
                    block[i]->error = _("Assignment of non-returning algorithm");
                }
                delete operand;
                return 0;
            }
        } // end if (blockType==Variable)
        else if (blockType==Element) {
            int deep = 0;
            int openBrPos = curPos;
            int closeBrPos = -1;
            for ( ; curPos < lexems.size(); curPos++) {
                Lexem * clx = lexems[curPos];
                if (deep==0 && IS_OPERATOR(clx->type) && clx->type!=LxOperLeftSqBr && clx->type!=LxOperRightSqBr) {
                    curPos--;
                    break;
                }
                block << clx;
                if (clx->type==LxOperLeftSqBr) {
                    if (deep==0)
                        openBrPos = curPos;
                    deep++;
                }
                if (clx->type==LxOperRightSqBr) {
                    deep--;
                    closeBrPos = curPos;
                }
            }
            if (deep>0) {
                lexems[openBrPos]->error = _("No pairing ']'");
                return 0;
            }
            else if (deep<0 && closeBrPos!=-1) {
                lexems[closeBrPos]->error= _("No pairing '['");
                return 0;
            }
            AST::Expression * operand = parseElementAccess(block, mod, alg);
            if (!operand) {
                return 0;
            }
            if (notFlag) {
//                if (operand->baseType!=AST::TypeBoolean) {
//                    delete operand;
//                    notFlag->error =  _("Use of 'not' for non-boolean value");
//                    return 0;
//                }
                subexpression << notFlag;
            }
            curPos++;
            if (curPos>=lexems.size()) {
                oper = 0;
            }
            else if (IS_OPERATOR(lexems[curPos]->type)) {
                oper = lexems[curPos];
            }
            else {
                lexems[curPos]->error = _("Forgotten operator after bracket"); // FIXME garbage error
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
//                    curPos++;
                    break;
                }
            }
            if (curPos>=lexems.size()) {
                oper = 0;
            }
//            else if (IS_OPERATOR(lexems[curPos]->type)) {
//                oper = lexems[curPos];
//            }
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
            if (block.first()->type==LxOperLeftBr && block.last()->type==LxOperRightBr) {
                block.pop_back(); // remove ")"
                block.pop_front(); // remove "("
            }
            AST::Expression * operand = parseExpression(block, mod, alg);
            if (!operand) {
                return 0;
            }
            if (notFlag) {
//                if (operand->baseType!=AST::TypeBoolean) {
//                    delete operand;
//                    notFlag->error =  _("Use of 'not' for non-boolean value");
//                    return 0;
//                }
                subexpression << notFlag;
            }
            curPos++;
            if (curPos==lexems.size()) {
                oper = 0;
            }
            else if (IS_OPERATOR(lexems[curPos]->type)) {
                oper = lexems[curPos];
                if (oper->type==LxOperLeftSqBr) {
                    oper->error = _("No name before [");
                    delete operand;
                    return 0;
                }
                if (oper->type==LxOperLeftBr) {
                    oper->error = _("No operator before (");
                    delete operand;
                    return 0;
                }
            }
            else {
                lexems[curPos]->error = _("Forgotten operator after bracket"); // FIXME garbage error
                delete operand;
                return 0;
            }
            if (oper && oper->type==LxOperComa) {
                oper->error = _("Extra ','");
                delete operand;
                return 0;
            }
            subexpression << operand;
            if (oper) {
                subexpression << oper;
            }
            operand->expressionIsClosed = true;
        } // end if (blockType==SubExpression)
        else if (blockType==None) {
            if (oper) {
                if (oper->type==LxOperPlus || oper->type==LxOperMinus) {
                    subexpression << oper;
                }
                else {
                    oper->error = _("Extra operator");
                    return 0;
    //                qDebug() << "Check me: " << __FILE__ << ", line: " << __LINE__;
                }
            }
            else {
                if (lexems.last()->type==LxOperRightBr) {
                    lexems.last()->error = _("No pairing '('");
                    return 0;
                }
                if (lexems.last()->type==LxOperRightSqBr) {
                    lexems.last()->error = _("No pairing '['");
                    return 0;
                }

            }
        }

    }
    for (int i=0; i<subexpression.size(); i++) {
        const SubexpressionElement & el = subexpression.at(i);
        if (el.o && (el.o->type==LxOperEqual || el.o->type==LxOperNotEqual) ) {
            bool isBooleanComparision =
                    i>0 && subexpression[i-1].e && subexpression[i-1].e->baseType.kind == AST::TypeBoolean
                    ||
                    i<subexpression.size()-1 && subexpression[i+1].e && subexpression[i+1].e->baseType.kind == AST::TypeBoolean;
            if (isBooleanComparision) {
                if (el.o->type = LxOperEqual)
                    el.o->type = LxOperBoolEqual;
                else
                    el.o->type = LxOperBoolNotEqual;
            }
        }
    }
    result = makeExpressionTree(subexpression);
    if (result)
        result->lexems = lexems;
    return result;
}


AST::Expression * SyntaxAnalizerPrivate::parseFunctionCall(const QList<Lexem *> &lexems, const AST::Module *mod, const AST::Algorhitm *alg)
{
    AST::Expression * result = 0;
    QString name;
    QList<Lexem*> nameLexems;
    Lexem * openBracket = 0;
    int openBracketIndex = -1;
    for (int i=0; i<lexems.size(); i++) {
        if (lexems[i]->type==LxNameClass || lexems[i]->type & LxTypePrimaryKwd || lexems[i]->type & LxTypeSecondaryKwd ) {
            lexems[i]->error = _("Keyword in name");
            return 0;
        }
        if (lexems[i]->type==LxOperLeftBr) {
            openBracket = lexems[i];
            openBracketIndex = i;
            break;
        }
        else {
            if (i>0) name += " ";
            name += lexems[i]->data;
            nameLexems << lexems[i];
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

    if (!function->header.error.isEmpty()) {
        foreach (Lexem * lx, nameLexems) {
            lx->error = _("This algorhitm is broken");
        }
        return 0;
    }

    foreach (Lexem * lx, nameLexems) lx->type = LxNameAlg;

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
    splitLexemsByOperator(argLine, LxOperComa, arguments, comas);
    int diff = arguments.size()-function->header.arguments.size();
    if (diff>0) {
        for (int i=0; i<diff; i++) {
            QList<Lexem*> lxs = arguments[arguments.size()-1-i];
            foreach (Lexem * lx, lxs) {
                lx->error = _("Extra algorithm arguments");
            }
        }
        return 0;
    }
    if (diff<0) {
        cb->error = _("Not enought algorithm arguments");
        return 0;
    }
    for (int i=0; i<arguments.size(); i++) {
        AST::Type extType = function->header.arguments[i]->baseType;
        int extDim = function->header.arguments[i]->dimension;
        bool allowTypeChange = function->header.arguments[i]->accessType==AST::AccessArgumentIn;
        AST::Expression * argument = 0;
        if (arguments[i].isEmpty()) {
            foreach (AST::Expression * a, realArguments) delete a;
            const QString err = _("Void argument");
            if (i>0)
                comas[i-1]->error = err;
            else
                ob->error = err;
            return 0;
        }
        if (extDim==0) {
            argument = parseExpression(arguments[i], mod, alg);
            if (!argument) {
                foreach (AST::Expression * a, realArguments) delete a;
                return 0;
            }
        }
        else {
            QString arrName;
            foreach (const Lexem * lx, arguments[i]) {
                if (!arrName.isEmpty())
                    arrName += " ";
                arrName += lx->data;
            }
            AST::Variable * var = 0;
            if (findVariable(arrName, mod, alg, var)) {
                argument = new AST::Expression;
                argument->kind = AST::ExprVariable;
                argument->dimension = var->dimension;
                argument->variable = var;
                argument->baseType = var->baseType;
            }
            else {
                foreach (AST::Expression * a, realArguments) delete a;
                const QString err = _("Array required here");
                foreach (Lexem * lx, arguments[i]) lx->error = err;
                return 0;
            }
        }
        realArguments << argument;
        AST::Type intType = argument->baseType;
        int intDim = argument->dimension;
        bool typesCompatible = ( intType.kind==AST::TypeInteger && extType.kind==AST::TypeReal )
                || ( intType.kind==AST::TypeCharect && extType.kind==AST::TypeString );
        QString err;
        if (intDim!=extDim) {
            if (intDim==0 || extDim==0) {
                err = _("Incompatible types");
            }
            else {
                err = _("Array dimension mismatch");
            }
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
    result->lexems = lexems;
    return result;
}

void SyntaxAnalizerPrivate::updateSliceDSCall(AST::Expression * expr, AST::Variable * var)
{
    static AST::Algorhitm * strlenAlg = 0;
    static AST::Module * stdlibMod = 0;
    if (!strlenAlg)
        findAlgorhitm(QString::fromUtf8("длин"), stdlibMod, strlenAlg);
    if (expr->kind==AST::ExprFunctionCall
            && expr->function==strlenAlg
            && expr->operands.size()==0)
    {
        AST::Expression * varExpr = new AST::Expression;
        varExpr->kind = AST::ExprVariable;
        varExpr->baseType = AST::TypeString;
        varExpr->dimension = 0;
        varExpr->variable = var;
        expr->operands.append(varExpr);
    }
    else foreach (AST::Expression * subExpr, expr->operands) {
        updateSliceDSCall(subExpr, var);
    }
}

AST::Expression * SyntaxAnalizerPrivate::parseElementAccess(const QList<Lexem *> &lexems, const AST::Module *mod, const AST::Algorhitm *alg)
{
    AST::Expression * result = 0;
    QString name;
    Lexem * openBracket = 0;
    int openBracketIndex = -1;
    bool nameIsNumericConstant = true;
    for (int i=0; i<lexems.size(); i++) {
        if (lexems[i]->type==LxOperLeftSqBr) {
            openBracket = lexems[i];
            openBracketIndex = i;
            break;
        }
        else {
            if (i>0) name += " ";
            name += lexems[i]->data;
            nameIsNumericConstant = nameIsNumericConstant &&
                    (lexems[i]->type==LxConstInteger
                     || lexems[i]->type==LxConstReal
                     || lexems[i]->type==LxConstBoolTrue
                     || lexems[i]->type==LxConstBoolFalse
                     );
        }
    }
    if (nameIsNumericConstant) {
        if (openBracketIndex==-1)
            openBracketIndex = lexems.size();
        for (int i=0; i<openBracketIndex; i++) {
            lexems[i]->error = _("Non-literal constant is not indexable");
        }
        return 0;
    }
    QList<AST::Expression*> realArguments;
    QList< QList<Lexem*> > arguments;
    QList<Lexem*> comas;

    AST::Variable * variable = 0;
    if (!findVariable(name, mod, alg, variable)) {
        if (lexer->isReturnVariable(name)) {
            if (!alg || alg->header.returnType.kind==AST::TypeNone) {
                if (openBracketIndex==-1)
                    openBracketIndex = lexems.size();
                for (int i=0; i<openBracketIndex; i++) {
                    if (!alg) {
                        lexems[i]->error = _("Access to return value outside of algorithm");
                    }
                    else {
                        lexems[i]->error = _("This algorithm has no return value");
                    }
                }
                return 0;
            }
            else {
                bool found = findLocalVariable(alg->header.name, alg, variable);
                Q_ASSERT(found);
                Q_UNUSED(found);
            }
        }
        else {
            AST::Algorhitm * a = 0;
            if (findAlgorhitm(name, mod,a)) {
                int a = qMax(0, openBracketIndex);
                for (int i=a; i<lexems.size(); i++) {
                    lexems[i]->error = _("'[...]' instead of '(...)'");
                }
                if (openBracketIndex==-1)
                    openBracketIndex = lexems.size();
                for (int i=0; i<openBracketIndex; i++) {
                    lexems[i]->type = LxNameAlg;
                }
            }
            else {
                if (openBracketIndex==-1)
                    openBracketIndex = lexems.size();
                for (int i=0; i<openBracketIndex; i++) {
                    lexems[i]->error = _("Variable not found");
                }
            }
            return 0;
        }
    }

    if (lexer->isReturnVariable(name) && alg->header.returnType.kind!=AST::TypeString) {
        if (openBracketIndex==-1)
            openBracketIndex = lexems.size();
        for (int i=0; i<openBracketIndex; i++) {
            lexems[i]->error = _("Return value is not indexable");
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
    int extraIndecesStart = -1;
    for (int i=0; i<argLine.size(); i++) {
        if (argLine[i]->type==LxOperLeftSqBr) {
            ob = argLine[i];
            if (deep==0 && i>0) {
                extraIndecesStart = i;
            }
            deep ++;
        }
        if (argLine[i]->type==LxOperRightSqBr) {
            cb = argLine[i];
            cbPos = i;
            deep --;
        }
    }
    if (deep>0) {
        ob->error = _("Unpaired '['");
        return 0;
    }
    if (deep<0) {
        for (int i=argLine.size()-1; i>=0; i--) {
            if (argLine[i]->type==LxOperRightSqBr) {
                deep--;
            }
            if (deep==0) {
                argLine[i]->error = _("Unpaired ']'");
                return 0;
            }
        }
    }
    if ((cbPos+1)<argLine.size()) {
        for (int i=cbPos+1; i<argLine.size(); i++) {
            argLine[i]->error = _("Garbage in expression");
        }
        return 0;
    }
    if (extraIndecesStart!=-1) {
        static const QString err = _("Indeces was specified before");
        for (int i=extraIndecesStart; i<argLine.size(); i++) {
            argLine[i]->error = err;
        }
        return 0;
    }
    argLine.pop_back();
    argLine.pop_front();
    if (argLine.isEmpty()) {
        openBracket->error = _("Nothing between '[' and ']'");
        cb->error = openBracket->error;
        return 0;
    }
    splitLexemsByOperator(argLine, LxOperComa, arguments, comas);
    for (int i=0; i<arguments.size(); i++) {
        if (arguments[i].isEmpty()) {
            const QString err = _("Void index");
            if (i>0)
                comas[i-1]->error = err;
            else
                ob->error = err;
            return 0;
        }
    }

    int varDimension = variable->dimension;
    if (variable->baseType.kind==AST::TypeString)
        varDimension ++;

    for (int i=0; i<arguments.size(); i++) {

        QList<Lexem*> colons;
        QList< QList<Lexem*> > slice;

        splitLexemsByOperator(arguments[i], LxOperColon, slice, colons);

        if (slice.size()>1 && variable->baseType.kind!=AST::TypeString) {
            foreach (Lexem * lx, arguments[i]) {
                lx->error = _("Not a simple index");
            }
            return 0;
        }
        if (slice.size()>1 && variable->baseType.kind==AST::TypeString && i<arguments.size()-1) {
            foreach (Lexem * lx, arguments[i]) {
                lx->error = _("Slice of string array, but not string");
            }
            return 0;
        }
        if (slice.size()>2) {
            QString err = _("Extra indeces in slice");
            for (int a=1; a<colons.size(); a++) {
                colons[a]->error = err;
            }
            for (int b=2; b<slice.size(); b++) {
                for (int a=0; a<slice[b].size(); a++) {
                    slice[b][a]->error = err;
                }
            }
            return 0;
        }
        for (int j=0 ; j<slice.size(); j++) {
            if (slice[j].isEmpty()) {
                const QString err = _("Void index");
                if (j>0) {
                    colons[j-1]->error = err;
                }
                else {
                    openBracket->error = err;
                }
                foreach (AST::Expression * a, realArguments) delete a;
                return 0;
            }
            AST::Expression * argument = parseExpression(slice[j], mod, alg);
            if (!argument) {
                foreach (AST::Expression * a, realArguments) delete a;
                return 0;
            }
            realArguments << argument;
            AST::Type intType = argument->baseType;
            int intDim = argument->dimension;
            if (intDim>0) {
                foreach (Lexem * lx, slice[j])
                    lx->error = _("Passing table as index");
                foreach (AST::Expression * a, realArguments) delete a;
                return 0;
            }
            if (intType.kind!=AST::TypeInteger) {
                foreach (Lexem * lx, slice[j])
                    lx->error = _("Index is not integer");
                foreach (AST::Expression * a, realArguments) delete a;
                return 0;
            }
            updateSliceDSCall(argument, variable);
        }
    } // end for arguments loop

    int diff = arguments.size()-varDimension;
    if (diff>0 && varDimension==0) {
        for (int i=openBracketIndex; i<lexems.size(); i++) {
            lexems[i]->error = _("Extra indeces");
        }
        if (result)
            delete result;
        return 0;
    }
    else if (diff>0) {
        int errorStartIndex = 0;
        for (int i=comas.size()-1; i>=0; i--) {
            deep--;
//            if (deep==0) {
                errorStartIndex = argLine.indexOf(comas[i])+1;
//            }
        }
        for (int i=errorStartIndex; i<cbPos-1; i++) {
            argLine[i]->error = _("Extra indeces");
        }
        if (result)
            delete result;
        return 0;
    }
    if ( ( diff<0 && variable->baseType.kind!=AST::TypeString )
            || ( diff<-1 && variable->baseType.kind==AST::TypeString )
            ) {
        cb->error = _("Not enought indeces");
        if (result)
            delete result;
        return 0;
    }

    result = new AST::Expression;
    result->kind = AST::ExprArrayElement;
    if (variable->baseType.kind==AST::TypeString && diff==0) {
        // Element of string: indeces count == array dimension + 1
        if (realArguments.size() > arguments.size()) {
            // String slice -> res type is string
            result->baseType.kind = AST::TypeString;
        }
        else {
            // String element -> res type is character
            result->baseType.kind = AST::TypeCharect;
        }
    }
    else {
        // Element of ordinary array
        result->baseType = variable->baseType;
    }
    result->dimension = 0;
    result->variable = variable;
    result->operands = realArguments;
    result->lexems = lexems;
    return result;
}


AST::Expression * SyntaxAnalizerPrivate::parseSimpleName(const std::list<Lexem *> &lexems, const AST::Module *mod, const AST::Algorhitm *alg)
{
    AST::Expression * result = 0;
    if (lexems.size()==1 && lexems.front()->type==LxSecCurrentStringLength) {
        result = new AST::Expression;
        result->kind = AST::ExprFunctionCall;
        result->baseType = AST::TypeInteger;
        result->dimension = 0;
        result->lexems = QList<Lexem*>::fromStdList(lexems);
        const AST::Module * dummy = 0;
        findAlgorhitm(QString::fromUtf8("длин"), dummy, result->function);
        return result;
    }
    if (lexems.size()==1 && lexems.front()->type & LxTypeConstant) {
        int err = 0;
        AST::VariableBaseType type = testConst(lexems, err);
        if (err) {
            lexems.front()->error = err;
            return 0;
        }
        else {
            result = new AST::Expression;
            result->kind = AST::ExprConst;
            result->baseType.kind = type;
            int maxDim = 0;
            result->constant = parseConstant(lexems, type, maxDim);
            if (result->constant.type()==QVariant::Double)
                result->baseType.kind = AST::TypeReal;
            return result;
        }
    }
    if (lexems.size()>1 && lexems.front()->type==LxConstLiteral) {
        bool allLiterals = true;
        for (std::list<Lexem*>::const_iterator it=lexems.begin(); it!=lexems.end(); it++) {
            allLiterals = allLiterals && (*it)->type==LxConstLiteral;
        }
        if (allLiterals) {
            const QString err = _("Many quotes in constant");
            for (std::list<Lexem*>::const_iterator it=lexems.begin(); it!=lexems.end(); it++) {
                (*it)->error = err;
            }
            return 0;
        }
    }
    if (lexems.size()==1 && lexems.front()->type == LxSecNewline) {
        result = new AST::Expression;
        result->kind = AST::ExprConst;
        result->baseType.kind = AST::TypeCharect;
        result->dimension = 0;
        result->constant = QVariant(QChar('\n'));
        return result;
    }
    else {
        // First -- try to parse as a constant of custom type
        QString longLexem;
        for (std::list<Lexem*>::const_iterator it=lexems.begin(); it!=lexems.end(); it++) {
            if (it!=lexems.begin())
                longLexem += " ";
            longLexem += (*it)->data;
        }
        AST::Type userConstType;
        QVariant userConstValue;
        if (tryInputOperatorAlgorithm(longLexem, userConstType, userConstValue)) {
            result = new AST::Expression;
            result->kind = AST::ExprConst;
            result->baseType = userConstType;
            result->dimension = 0;
            result->constant = userConstValue;
            for (std::list<Lexem*>::const_iterator it=lexems.begin(); it!=lexems.end(); it++) {
                Lexem * lx = (*it);
                lx->type = LxConstReal;
            }
            return result;
        }
        // If not a custom type constant -- try more...
        QString name;
        bool expCandidate = true;
        for (std::list<Lexem*>::const_iterator it=lexems.begin(); it!=lexems.end(); it++) {
            if (it!=lexems.begin())
                name += " ";
            name += (*it)->data;
            expCandidate = expCandidate && (*it)->type==LxConstReal;
            if ( (*it)->type==LxNameClass ) {
                (*it)->error = _("Keyword in name");
                return 0;
            }
        }
        QString err;
        if (expCandidate) {
            // If exp is correct, then it should be
            // caught above under condition lexems.size()==1
            err = _("Wrong E-real number");
        }
        else {
            err = lexer->testName(name);
        }
        bool retval = lexer->isReturnVariable(name);
        if (!err.isEmpty() && !retval) {
            for (std::list<Lexem*>::const_iterator it=lexems.begin(); it!=lexems.end(); it++) {
                (*it)->error = err;
            }
            return 0;
        }

        // Catch keyword in name
        if (!retval) {
            for (std::list<Lexem*>::const_iterator it=lexems.begin(); it!=lexems.end(); it++) {
                Lexem* lx = (*it);
                if (lx->type & LxTypePrimaryKwd || lx->type & LxTypeSecondaryKwd || lx->type==LxNameClass) {
                    lx->error = _("Keyword in name");
                    return 0;
                }
            }
        }

        // Catch abrakadabra like 'yes "Something"' or 'yes 3' or 'yes "Something"'

        Lexem * boolConst = 0;
        Lexem * numericConst = 0;
        Lexem * stringConst = 0;
//        LexemType prevType = LxTypeEmpty;
        for (std::list<Lexem*>::const_iterator it = lexems.begin(); it!=lexems.end(); it++) {
            Lexem * lx = (*it);
            LexemType curType = lx->type;
            if (curType==LxConstBoolTrue || curType==LxConstBoolFalse)
                if (!boolConst)
                    boolConst = lx;
            if (curType==LxConstInteger || curType==LxConstReal)
                if (!numericConst)
                    numericConst = lx;
            if (curType==LxConstLiteral)
                if (!stringConst)
                    stringConst = lx;
        }

        if (boolConst) {
            boolConst->error = _("'%1' can't be part of name", boolConst->data);
            return 0;
        }


        if (stringConst) {
            stringConst->error = _("Literal can't be part of name");
            return 0;
        }

        if (numericConst && numericConst==(*lexems.begin()) ) {
            numericConst->error = _("Name starts with digit");
            return 0;
        }

        AST::Algorhitm * a = 0;
        AST::Variable * v = 0;

        err = "";
        if (retval) {
            for (std::list<Lexem*>::const_iterator it=lexems.begin(); it!=lexems.end(); it++) {
                (*it)->type = LxTypeSecondaryKwd;
            }
            if (!alg) {
                err = _("Access to return value outside of algorithm");
            }
            if (alg && alg->header.returnType.kind==AST::TypeNone) {
                err = _("This algorithm has no return value");
            }
            if (err.isEmpty()) {
                bool found = findLocalVariable(alg->header.name, alg, v);
                Q_ASSERT(found);
                Q_UNUSED(found);
                result = new AST::Expression;
                result->kind = AST::ExprVariable;
                result->baseType = alg->header.returnType;
                result->variable = v;
                return result;
            }
        }
        else {
            if (findAlgorhitm(name, mod, a)) {
                foreach (Lexem *lx, lexems) lx->type = LxNameAlg;
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
            else {
                err = _("Name not declared");
            }
        }
        if (!err.isEmpty()) {
            for (std::list<Lexem*>::const_iterator it=lexems.begin(); it!=lexems.end(); it++) {
                (*it)->error = err;
            }
            return 0;
        }
    }
    return result;
}



int findOperatorByPriority(const QList<SubexpressionElement> & s)
{
    int result = -1;
    static const QList<QSet <LexemType> > Omega = QList<QSet<LexemType> >()
            << ( QSet<LexemType>() << LxSecOr )
            << ( QSet<LexemType>() << LxSecAnd )
            << ( QSet<LexemType>() << LxOperBoolEqual << LxOperBoolNotEqual )
            << ( QSet<LexemType>() << LxSecNot )
            << ( QSet<LexemType>() << LxOperNotEqual << LxOperEqual << LxOperGreater << LxOperGreaterOrEqual << LxOperLess << LxOperLessOrEqual << LxPriAssign)
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
        }
    }
    return result;
}

#define IS_NUMERIC(x) ( x==AST::TypeInteger || x==AST::TypeReal )
#define IS_LITERAL(x) ( x==AST::TypeCharect || x==AST::TypeString )

bool IS_NUMERIC_LIST(const QList<AST::Expression*> & list) {
    bool result = true;
    for (int i=0; i<list.size(); i++) {
        bool c = true;
        if (list[i]->baseType.kind==AST::TypeBoolean &&
                list[i]->kind==AST::ExprSubexpression)
        {
            c = IS_NUMERIC_LIST(list[i]->operands);
        }
        else {
            c = IS_NUMERIC(list[i]->baseType.kind);
        }
        result = result && c;
    }
    return result;
}

bool IS_LITERAL_LIST(const QList<AST::Expression*> & list) {
    bool result = true;
    for (int i=0; i<list.size(); i++) {
        result = result && IS_LITERAL(list[i]->baseType.kind);
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
    else if (lx->type==LxOperEqual || lx->type==LxOperBoolEqual)
        return AST::OpEqual;
    else if (lx->type==LxOperNotEqual || lx->type==LxOperBoolNotEqual)
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

AST::Type resType(const AST::Type & a
                              , const AST::Type & b
                              , AST::ExpressionOperator op)
{
    if(a==AST::TypeNone)
    {
        if (b==AST::TypeInteger && (op==AST::OpSumm || op==AST::OpSubstract) )
            return AST::TypeInteger;
        if (b==AST::TypeReal && (op==AST::OpSumm || op==AST::OpSubstract) )
            return AST::TypeReal;
        if (b==AST::TypeBoolean && op==AST::OpNot )
            return AST::TypeBoolean;
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

AST::Expression * findRightmostCNFSubexpression(AST::Expression * e)
{
    static const QSet<AST::ExpressionOperator> ComparisonOperators = QSet<AST::ExpressionOperator>()
            << AST::OpLess << AST::OpLessOrEqual << AST::OpEqual << AST::OpNotEqual << AST::OpGreaterOrEqual << AST::OpGreater;
    if (ComparisonOperators.contains(e->operatorr)) {
        return e->operands.last();
    }
    else {
        return findRightmostCNFSubexpression(e->operands.last());
    }
}

AST::Expression * SyntaxAnalizerPrivate::makeExpressionTree(const QList<SubexpressionElement> & s)
{
    if (s.isEmpty())
        return 0;
    int l = findOperatorByPriority(s);
    if (l==-1) {
        for (int i=0; i<s.size(); i++) {
            if (s[i].e)
                return s[i].e;
        }
    }
    else {
        if (s[l].o->type==LxPriAssign) {
            s[l].o->error = _("':=' or '=' ?");
            return 0;
        }
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
        AST::Type headType = headExpr? headExpr->baseType : AST::Type();
        AST::Type tailType = tailExpr->baseType;
        static const QSet<LexemType> ComparisonOperators = QSet<LexemType>()
                << LxOperLess << LxOperLessOrEqual << LxOperEqual << LxOperNotEqual << LxOperGreaterOrEqual << LxOperGreater << LxPriAssign;

        bool tailIsNumeric = IS_NUMERIC(tailType.kind);
        bool headIsBool = headType.kind==AST::TypeBoolean;
        bool hasHeadExpr = headExpr != NULL;
        bool headIsSubexpr = hasHeadExpr && headExpr->kind==AST::ExprSubexpression;
        bool isComparision = ComparisonOperators.contains(s[l].o->type);
        bool numericOperands = headExpr && IS_NUMERIC_LIST(headExpr->operands);
        bool tailIsLiteral = IS_LITERAL(tailType.kind);
        bool literalOperands = headExpr && IS_LITERAL_LIST(headExpr->operands);

        bool makeCNF = headIsBool && headIsSubexpr && isComparision && (
                    (tailIsNumeric && numericOperands)
                    ||
                    (tailIsLiteral && literalOperands)
                    );

        if (headExpr && headExpr->expressionIsClosed)
            makeCNF = false;

        if (makeCNF) {
            AST::Expression * res = new AST::Expression;
            res->kind = AST::ExprSubexpression;
            res->baseType.kind = AST::TypeBoolean;
            res->operands << headExpr;
            res->operatorr = AST::OpAnd;
            AST::Expression * subRes = new AST::Expression;
            subRes->kind = AST::ExprSubexpression;
            subRes->baseType.kind = AST::TypeBoolean;
//            subRes->operands << new AST::Expression(headExpr->operands.last());
            subRes->operands << findRightmostCNFSubexpression(new AST::Expression(headExpr));
            subRes->operands << tailExpr;
            subRes->operatorr = operatorByLexem(s[l].o);            
            res->operands << subRes;
            return res;
        }
        else if (!headExpr && tailExpr->kind==AST::ExprConst) {
            // Merge unary +, - and 'not' into constant
            if (s[l].o->type==LxOperPlus) {
                if (tailExpr->baseType.kind==AST::TypeInteger) {
                    // pass
                }
                else if (tailExpr->baseType.kind==AST::TypeReal) {
                    // pass
                }
                else {
                    s[l].o->error = _("Can't +%1", lexer->classNameByBaseType(tailExpr->baseType.kind));
                }
            }
            else if (s[l].o->type==LxOperMinus) {
                if (tailExpr->baseType.kind==AST::TypeInteger) {
                    tailExpr->constant = QVariant(0-tailExpr->constant.toInt());
                }
                else if (tailExpr->baseType.kind==AST::TypeReal) {
                    tailExpr->constant = QVariant(0.0-tailExpr->constant.toDouble());
                }
                else {
                    s[l].o->error = _("Can't -%1", lexer->classNameByBaseType(tailExpr->baseType.kind));
                }
            }
            else if (s[l].o->type==LxSecNot) {
                if (tailExpr->baseType.kind==AST::TypeBoolean) {
                    tailExpr->constant = QVariant( ! tailExpr->constant.toBool());
                }
                else {
                    s[l].o->error = _("Can't not %1", lexer->classNameByBaseType(tailExpr->baseType.kind));
                }
            }
            return tailExpr;
        }
        else {
            AST::ExpressionOperator operation = operatorByLexem(s[l].o);
            AST::Type rt = resType(headType, tailType, operation);
            if (rt.kind==AST::TypeNone && headExpr && tailExpr) {
                // Try to find custom-overriden operator
                const QString & operatorName = s[l].o->data;
                AST::Expression * customOperation = makeCustomBinaryOperation(operatorName, headExpr, tailExpr);
                if (customOperation) {
                    customOperation->operatorr = operation;
                    return customOperation;
                }
            }
            if (rt.kind==AST::TypeNone) {
                QString type1name, type2name;
                if (headType.kind!=AST::TypeUser)
                    type1name = lexer->classNameByBaseType(headType.kind);
                else
                    type1name = headType.name;
                if (tailType.kind!=AST::TypeUser)
                    type2name = lexer->classNameByBaseType(tailType.kind);
                else
                    type2name = tailType.name;
                s[l].o->error = _("Can't %1 %2 %3"
                                  ,type1name
                                  ,s[l].o->data
                                  ,type2name
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
            if (headExpr)
                res->operands << headExpr;
            res->operands << tailExpr;
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
