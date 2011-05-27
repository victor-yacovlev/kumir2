#include "kumircppgeneratorplugin.h"
#include "abstractsyntaxtree/ast_module.h"
#include "abstractsyntaxtree/ast_algorhitm.h"
#include "abstractsyntaxtree/ast_variable.h"
#include "abstractsyntaxtree/ast_expression.h"
#include "nameprovider.h"

#include <QtCore>
#include <iostream>


namespace KumirCppGenerator {


struct Module {
    QString name;
    QString cNamespace;
    QString cLibrary;
    QString headerData;
    QString sourceData;
    QMap<QString,QString> algorhitmHeaders;
};

struct KumirCppGeneratorPrivate {
    QList<Module*> modules;
    NameProvider * nameProvider;
    const AST::Data * ast;
    void addModule(const AST::Module * module);
    void createModuleHeader(const AST::Module * module);
    void createModuleSource(const AST::Module * module);
    QString createAlgorhitmHeader(const AST::Algorhitm * algorhitm,
                                  const QString &moduleName);
    QString makeExpression(const AST::Expression * expr,
                           const AST::Algorhitm * algorhitm,
                           const AST::Module * module) const;
    QString makeConstant(AST::VariableBaseType type,
                         const QVariant & value) const;
    QString makeArrayElement(const AST::Variable * var,
                             const QList<AST::Expression *> & expr,
                             const AST::Algorhitm * algorhitm,
                             const AST::Module * module) const;
    QString makeFunctionCall(const AST::Algorhitm * function,
                             const QList<AST::Expression *> & args,
                             const AST::Algorhitm * algorhitm,
                             const AST::Module * module) const;
    QString makeSubExpression(const QList<AST::Expression *> & operands,
                              AST::ExpressionOperator op,
                              const AST::Algorhitm * algorhitm,
                              const AST::Module * module) const;
    QString makeAlgorhitm(const AST::Algorhitm * alg,
                          const AST::Module * modName);
    QString makeInitializator(const AST::Module * module);
    QString makeMain() const;
    QString makeBody(
        const QList<AST::Statement*> &statements,
        int deep,
        const AST::Algorhitm * algorhitm,
        const AST::Module * module);

    QString makeStError(const QString &text, int lineNo) const;
    QString makeStAssign(const QList<AST::Expression*> & exprs,
                         const AST::Algorhitm * algorhitm,
                         const AST::Module * module) const;
    QString makeStAssert(const QList<AST::Expression*> & exprs,
                         const AST::Algorhitm * algorhitm,
                         const AST::Module * module) const;
    QString makeStInput(const QList<AST::Expression*> & exprs,
                        const AST::Algorhitm * algorhitm,
                        const AST::Module * module) const;
    QString makeStOutput(const QList<AST::Expression*> & exprs,
                         const AST::Algorhitm * algorhitm,
                         const AST::Module * module) const;
    QString makeStLoop(const AST::LoopSpec &loop,
                       int deep,
                       const AST::Algorhitm * algorhitm,
                       const AST::Module * module);
    QString makeStIfThenElse(const QList<AST::ConditionSpec> &conds,
                             int deep,
                             const AST::Algorhitm * algorhitm,
                             const AST::Module * module);
    QString makeStBreak(int deep, const AST::Algorhitm * algorhitm,
                        const AST::Module * module) const;
    QString makeStVarInitialize(
        const QList<AST::Variable*> vars,
        const AST::Algorhitm * algorhitm,
        const AST::Module * module) const;

};

KumirCppGeneratorPlugin::KumirCppGeneratorPlugin()
{
    d = new KumirCppGeneratorPrivate;
    d->nameProvider = new NameProvider;
}

KumirCppGeneratorPlugin::~KumirCppGeneratorPlugin()
{
    for (int i=0; i<d->modules.size(); i++)
        delete d->modules[i];
    delete d;
}

QString KumirCppGeneratorPlugin::initialize(const QStringList &arguments)
{
    Q_UNUSED(arguments)
    return "";
}

void KumirCppGeneratorPrivate::addModule(const AST::Module *module)
{
    Module * mod = new Module;
    mod->name = module->header.name;
    if (module->header.type==AST::ModTypeExternal) {
        mod->cNamespace = module->header.cReference.nameSpace;
        mod->cLibrary = module->header.cReference.libraryBaseName;
    }
    else {
        if (!module->header.name.isEmpty()) {
            mod->cNamespace = nameProvider->addName(module->header.name, "");
        }
    }
    modules << mod;
}

QString KumirCppGeneratorPrivate::createAlgorhitmHeader(const AST::Algorhitm *algorhitm, const QString &moduleName)
{
    const QString algName = algorhitm->header.name;
    const QString cName = nameProvider->addName(algName, moduleName);
    const QString returnType = nameProvider->baseTypeName(algorhitm->header.returnType);
    QStringList args;
    foreach (const AST::Variable * var, algorhitm->header.arguments) {
        QString argType = nameProvider->baseTypeName(var->baseType);
        QString argMod;
        if ( (var->accessType==AST::AccessArgumentInOut || var->accessType==AST::AccessArgumentOut)
             && var->dimension==0 )
        {
            argMod = "*";
        }
        QString argName = nameProvider->addName(var->name, moduleName+"::"+algorhitm->header.name);
        if (var->accessType==AST::AccessArgumentIn && var->dimension>0) {
            argName = QString("__src_of_%1__").arg(argName);
        }
        if (var->dimension>0) {
            argType = QString("__array__<%1>").arg(argType);
        }
        args << argType + " " + argMod + argName;
    }
    return returnType+" "+cName+"("+args.join(", ")+")";
}

QString KumirCppGeneratorPrivate::makeExpression(
    const AST::Expression *expr,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module) const
{
    const QString modName = module->header.name;
    const QString algName = algorhitm? algorhitm->header.name : "";
    QString result;
    if (expr->kind==AST::ExprVariable) {
        result = nameProvider->findVariable(modName, algName, expr->variable->name);
    }
    else if (expr->kind==AST::ExprConst) {
        result = makeConstant(expr->baseType, expr->constant);
    }
    else if (expr->kind==AST::ExprArrayElement) {
        result = makeArrayElement(expr->variable, expr->operands, algorhitm, module);
    }
    else if (expr->kind==AST::ExprFunctionCall) {
        result = makeFunctionCall(expr->function, expr->operands, algorhitm, module);
    }
    else if (expr->kind==AST::ExprSubexpression) {
        result = makeSubExpression(expr->operands, expr->operatorr, algorhitm, module);
    }

    return result;
}

QString screenString(QString s)
{
    QString result;
    for (int i=0; i<s.length(); i++) {
        if (s[i]=='\n') {
            result += "\\n";
        }
        else if (s[i]=='\\') {
            result += "\\\\";
        }
        else if (s[i]=='\'') {
            result += "\\'";
        }
        else if (s[i]=='"') {
            result += "\\\"";
        }
        else if (s[i].unicode()>127) {
            result += "\\0x"+QString::number(s[i].unicode(), 16);
        }
        else {
            result += s[i];
        }
    }
    return result;
}

QString KumirCppGeneratorPrivate::makeConstant(
    AST::VariableBaseType type,
    const QVariant &value) const
{
    QString result;
    if (type==AST::TypeInteger || type==AST::TypeReal) {
        result = value.toString();
    }
    else if (type==AST::TypeBoolean) {
        result = value.toBool()? "1" : "0";
    }
    else if (type==AST::TypeCharect) {
        result = "L'"+screenString(value.toString())+"'";
    }
    else if (type==AST::TypeString) {
        result = "L\""+screenString(value.toString())+"\"";
    }
    return result;
}

QString KumirCppGeneratorPrivate::makeArrayElement(
    const AST::Variable *var,
    const QList<AST::Expression *> & expr,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module) const
{
    const QString modName = module->header.name;
    const QString algName = algorhitm? algorhitm->header.name : "";
    QString cName = nameProvider->findVariable(modName, algName, var->name);
    QString cType = nameProvider->baseTypeName(var->baseType);
    QStringList indeces;
    for (int i=0; i<3; i++) {
        if (i<expr.size())
            indeces << makeExpression(expr[i], algorhitm, module);
        else
            indeces << "-1";
    }
    QString result = QString("(*(%1*)__element__(&%2, %3, %4, %5))")
            .arg(cType)
            .arg(cName)
            .arg(indeces[0])
            .arg(indeces[1])
            .arg(indeces[2]);
    return result;
}

QString KumirCppGeneratorPrivate::makeFunctionCall(
    const AST::Algorhitm *function,
    const QList<AST::Expression *> &args,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module) const
{
    const QString modName = module->header.name;
    const QString cName = nameProvider->name(function->header.name, modName);
    QStringList arguments;
    for (int i=0; i<args.size(); i++) {
        arguments << makeExpression(args[i], algorhitm, module);
    }
    return cName+"("+arguments.join(", ")+")";
}

QString KumirCppGeneratorPrivate::makeSubExpression(
    const QList<AST::Expression *> &operands,
    AST::ExpressionOperator op,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module) const
{
    QString result;
    Q_ASSERT(operands.size()==1 || operands.size()==2);
    if (operands.size()==1) {
        Q_ASSERT(op==AST::OpNot || op==AST::OpSubstract);
        result += "(";
        result += nameProvider->operatorName(op);
        result += makeExpression(operands[0], algorhitm, module);
        result += ")";
    }
    else {
        if (op==AST::OpPower) {
            if (operands[0]->baseType==AST::TypeReal || operands[0]->baseType==AST::TypeReal) {
                result += "__real_power__(";
            }
            else {
                result += "__integer_power__(";
            }
            result += makeExpression(operands[0], algorhitm, module);
            result += ", ";
            result += makeExpression(operands[1], algorhitm, module);
            result += ")";
        }
        else if (op==AST::OpSumm && (operands[0]->baseType==AST::TypeCharect || operands[0]->baseType==AST::TypeString)) {
            result += "__concatenate__(";
            result += makeExpression(operands[0], algorhitm, module);
            result += ", ";
            result += makeExpression(operands[1], algorhitm, module);
            result += ")";
        }
        else {
            result += "(";
            result += makeExpression(operands[0], algorhitm, module);
            result += nameProvider->operatorName(op);
            result += makeExpression(operands[1], algorhitm, module);
            result += ")";
        }
    }
    return result;
}


void KumirCppGeneratorPrivate::createModuleHeader(const AST::Module *module)
{
    Module * mod = 0;
    foreach (Module* m, modules) {
        if (m->name==module->header.name) {
            mod = m;
            break;
        }
    }
    Q_CHECK_PTR(mod);
    mod->headerData += QString("#ifndef %1_H\n#define %1_H\n\n")
            .arg(mod->cNamespace.isEmpty()? "MAIN" : mod->cNamespace.toUpper());
    if (!mod->cNamespace.isEmpty()) {
//        mod->headerData += QString("namespace %1 {\n\n").arg(mod->cNamespace);
        mod->headerData += "extern void __init__"+mod->cNamespace+"();\n";
    }
    foreach (const AST::Algorhitm * alg, module->header.algorhitms) {
        if (module->header.type==AST::ModTypeExternal) {
            mod->headerData += "extern "+alg->header.cHeader+";\n";
        }
        else {
            QString cHeader = createAlgorhitmHeader(alg, mod->name);
            mod->algorhitmHeaders[alg->header.name] = cHeader;
            mod->headerData += "extern "+cHeader+";\n";
        }
    }

    if (!mod->cNamespace.isEmpty()) {
//        mod->headerData += "\n}\n";
    }
    mod->headerData += "\n#endif\n";
}

void KumirCppGeneratorPrivate::createModuleSource(const AST::Module *module)
{
    Module * mod = 0;
    foreach (Module* m, modules) {
        if (m->name==module->header.name) {
            mod = m;
            break;
        }
    }
    Q_CHECK_PTR(mod);
    mod->sourceData += QString("#include \"__kumir__.h\"\n");
    foreach (Module *m, modules) {
        if (m->cNamespace.isEmpty()) {
            mod->sourceData += "#include \"main.h\"\n";
        }
        else {
            mod->sourceData += "#include \""+m->cNamespace+".h\"\n";
        }
    }


    mod->sourceData += "\n";
//    if (!mod->cNamespace.isEmpty()) {
//        mod->sourceData+="namespace "+mod->cNamespace+" {\n";
//    }

    for (int i=0; i<module->impl.globals.size(); i++) {
        QString cName = nameProvider->addName(mod->cNamespace+"::"+module->impl.globals[i]->name
                                              , module->header.name);
        QString cType = nameProvider->baseTypeName(module->impl.globals[i]->baseType);
        if (module->impl.globals[i]->dimension>0) {
            cType = QString("__array__<%1>").arg(cType);
        }
        mod->sourceData += QString("static %1 %3;\n")
                .arg(cType).arg(cName)+";\n";
    }

    mod->sourceData += "\n";

    foreach (const AST::Algorhitm * alg, module->impl.algorhitms) {
        mod->sourceData += makeAlgorhitm(alg, module);
    }

    mod->sourceData += makeInitializator(module);

    if (!mod->cNamespace.isEmpty()) {
//        mod->sourceData+="\n}";
    }
    else {
        mod->sourceData += makeMain();
    }

    mod->sourceData += "\n";
}

QString KumirCppGeneratorPrivate::makeMain() const
{
    QString result;
    result += "#include <locale.h>\n";
    result += "int main(int argc, char** argv) {\n";
    result += "  setlocale(LC_CTYPE, \"ru_RU.UTF-8\");\n";
    QString firstAlgName;
    for (int i=0; i<modules.size(); i++) {
        Module * m = modules[i];
        if (m->cNamespace.isEmpty()) {
            result += "  __init__();\n";
        }
        else {
            result += "  __init__"+m->cNamespace+"();\n";
        }
    }
    QString entryPoint;
    for (int i=0; i<ast->modules.size(); i++) {
        if (ast->modules[i]->header.type==AST::ModTypeUser &&
                ast->modules[i]->header.name=="" &&
                ast->modules[i]->header.enabled)
        {
            if (ast->modules[i]->impl.algorhitms.size()>0) {
                entryPoint = nameProvider->name(ast->modules[i]->impl.algorhitms[0]->header.name, "");
            }
        }
    }
    if (!entryPoint.isEmpty()) {
        result += "  "+entryPoint+"();\n";
    }
    result += "  return 0;\n";
    result += "}\n";
    return result;
}

QString addIndent(const QString &s, int size = 1)
{
    QStringList lines = s.split("\n");
    for (int i=0; i<lines.size(); i++) {
        for (int j=0; j<size; j++) {
            lines[i] = "  "+lines[i];
        }
    }
    return lines.join("\n");
}

QString KumirCppGeneratorPrivate::makeAlgorhitm(
    const AST::Algorhitm *alg
    , const AST::Module * module)
{
    Module * mod = 0;
    foreach (Module* m, modules) {
        if (m->name==module->header.name) {
            mod = m;
            break;
        }
    }
    Q_CHECK_PTR(mod);
    QString header;
    QString modName = module->header.name;
    if (!mod->algorhitmHeaders.contains(alg->header.name)) {
        header = createAlgorhitmHeader(alg, modName);
    }
    else {
        header = "extern "+mod->algorhitmHeaders[alg->header.name];
    }
    QString result = header + "\n{\n";
    for (int i=0; i<alg->impl.locals.size(); i++) {
        QString locName = alg->impl.locals[i]->name;
        if (locName==alg->header.name) {
            locName = "__retval_of_"+locName;
        }
        const QString cName = nameProvider->addName(locName, modName+"::"+alg->header.name);
        QString cType = nameProvider->baseTypeName(alg->impl.locals[i]->baseType);
        if (alg->impl.locals[i]->dimension>0) {
            cType = QString("__array__<%1>").arg(cType);
        }
        if (alg->impl.locals[i]->accessType==AST::AccessRegular ||
                (alg->impl.locals[i]->accessType==AST::AccessArgumentIn &&
                 alg->impl.locals[i]->dimension>0
                 ))
        {
            result += "  "+cType+" "+cName+";\n";
        }
    }
    result += addIndent(makeBody(alg->impl.pre , 0, alg, module))+"\n";
    result += addIndent(makeBody(alg->impl.body, 0, alg, module))+"\n";
    result += addIndent(makeBody(alg->impl.post, 0, alg, module))+"\n";
    result += "}\n";
    return result;
}

QString KumirCppGeneratorPrivate::makeInitializator(const AST::Module *module)
{
    QString result = "extern void __init__()\n{\n";
    result += addIndent(makeBody(module->impl.initializerBody, 0, 0, module));
    result += "\n}\n";
    return result;
}

QString KumirCppGeneratorPrivate::makeBody(
    const QList<AST::Statement *> &statements,
    int deep,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module)
{
    QString result;
    for (int i=0; i<statements.size(); i++) {
        if (statements[i]->type==AST::StError)
            result += makeStError(statements[i]->error, -1);
        else if (statements[i]->type==AST::StAssign)
            result += makeStAssign(statements[i]->expressions, algorhitm, module);
        else if (statements[i]->type==AST::StAssert)
            result += makeStAssert(statements[i]->expressions, algorhitm, module);
        else if (statements[i]->type==AST::StVarInitialize)
            result += makeStVarInitialize(statements[i]->variables, algorhitm, module);
        else if (statements[i]->type==AST::StInput)
            result += makeStInput(statements[i]->expressions, algorhitm, module);
        else if (statements[i]->type==AST::StOutput)
            result += makeStOutput(statements[i]->expressions, algorhitm, module);
        else if (statements[i]->type==AST::StLoop)
            result += makeStLoop(statements[i]->loop, deep+1, algorhitm, module);
        else if (statements[i]->type==AST::StIfThenElse)
            result += makeStIfThenElse(statements[i]->conditionals, deep+1, algorhitm, module);
        else if (statements[i]->type==AST::StSwitchCaseElse)
            result += makeStIfThenElse(statements[i]->conditionals, deep+1, algorhitm, module);
        else if (statements[i]->type==AST::StBreak)
            result += makeStBreak(deep, algorhitm, module);
    }
    return result;
}

QString KumirCppGeneratorPrivate::makeStError(const QString &text, int lineNo) const
{
    const QString textConst = makeConstant(AST::TypeString, text);
    return "__abort__("+textConst+", "+QString::number(lineNo)+");\n";
}

QString KumirCppGeneratorPrivate::makeStAssign(
    const QList<AST::Expression *> &exprs,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module) const
{
    Q_ASSERT(exprs.size()==1 || exprs.size()==2);
    const QString right = makeExpression(exprs[0], algorhitm, module);
    QString result;
    if (exprs.size()==2) {
        const QString left = makeExpression(exprs[1], algorhitm, module);
        result = left + " = " + right + ";\n";
    }
    else {
        result = right + ";\n";
    }
    return result;
}

QString KumirCppGeneratorPrivate::makeStAssert(
    const QList<AST::Expression *> &exprs,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module) const
{
    QString result;
    for (int i=0; i<exprs.size(); i++) {
        const QString e = makeExpression(exprs[i], algorhitm, module);
        const QString text = QObject::tr("Assertion false");
        int lineNo = -1;
        result += "if ( !("+e+") ) {\n";
        result += "  "+makeStError(text, lineNo);
        result += "}\n";
    }
    return result;
}

QString KumirCppGeneratorPrivate::makeStInput(
    const QList<AST::Expression *> &exprs,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module) const
{
    QString result;
    QString format;
    QStringList es;
    for (int i=0; i<exprs.size(); i++) {
        if (exprs[i]->baseType==AST::TypeInteger)
            format += "%d";
        else if (exprs[i]->baseType==AST::TypeReal)
            format += "%f";
        else if (exprs[i]->baseType==AST::TypeBoolean)
            format += "%d";
        else if (exprs[i]->baseType==AST::TypeCharect)
            format += "%c";
        else if (exprs[i]->baseType==AST::TypeString)
            format += "%s";
        es << makeExpression(exprs[i], algorhitm, module);
    }
    result = "wscanf(" + makeConstant(AST::TypeString, format)+", "+
            es.join(", ")+");\n";
    return result;
}

QString KumirCppGeneratorPrivate::makeStOutput(
    const QList<AST::Expression *> &exprs,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module) const
{
    QString result;
    QString format;
    QStringList es;
    for (int i=0; i<exprs.size(); i++) {
        if (exprs[i]->baseType==AST::TypeInteger)
            format += "%d";
        else if (exprs[i]->baseType==AST::TypeReal)
            format += "%f";
        else if (exprs[i]->baseType==AST::TypeBoolean)
            format += "%d";
        else if (exprs[i]->baseType==AST::TypeCharect)
            format += "%c";
        else if (exprs[i]->baseType==AST::TypeString)
            format += "%s";
        es << makeExpression(exprs[i], algorhitm, module);
    }
    result = "wprintf(" + makeConstant(AST::TypeString, format)+", "+
            es.join(", ")+");\n";
    return result;
}

QString KumirCppGeneratorPrivate::makeStLoop(
    const AST::LoopSpec &loop,
    int deep,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module)
{
    QString result;
    if (loop.type==AST::LoopTimes) {
        result += QString("for (int __counter_%1__=0; __counter_%1__<%2; __counter_%1__++) {\n")
                .arg(deep)
                .arg(makeExpression(loop.timesValue, algorhitm, module));
    }
    else if (loop.type==AST::LoopFor) {
        const QString modName = module->header.name;
        const QString algName = algorhitm? algorhitm->header.name : "";
        QString var = nameProvider->findVariable(modName, algName, loop.forVariable->name);
        QString from = makeExpression(loop.fromValue, algorhitm, module);
        QString to = makeExpression(loop.toValue, algorhitm, module);
        QString step = loop.stepValue? makeExpression(loop.stepValue, algorhitm, module) : "1";
        result += QString("for (%1=%2; %4>=0? %1<=%3 : %1>=%3; %1+=%4 ) {\n")
                .arg(var).arg(from).arg(to).arg(step);
    }
    else if (loop.type==AST::LoopWhile) {
        QString cond = makeExpression(loop.whileCondition, algorhitm, module);
        result += QString("while (%1) {\n").arg(cond);
    }
    result += addIndent(makeBody(loop.body, deep+1, algorhitm, module))+"\n";
    if (loop.endCondition) {
        QString cond = makeExpression(loop.endCondition, algorhitm, module);
        result += "  if (%1) break;\n";
    }
    result += "}\n";
    return result;
}

QString KumirCppGeneratorPrivate::makeStIfThenElse(
    const QList<AST::ConditionSpec> &conds,
    int deep,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module)
{
    QString result;
    for (int i=0; i<conds.size(); i++) {
        const QString c = conds[i].condition? makeExpression(conds[i].condition, algorhitm, module) : "";
        if (i==0) {
            result += "if ("+c+") {\n";
        }
        else if (!c.isEmpty()) {
            result += "else if ("+c+") {\n";
        }
        else {
            result += "else {\n";
        }
        result += addIndent(makeBody(conds[i].body, deep+1, algorhitm, module));
        result += "}\n";
    }
    return result;
}

QString KumirCppGeneratorPrivate::makeStBreak(
    int deep,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module) const
{
    if (deep==0) {
        if (algorhitm->header.returnType==AST::TypeNone)
            return "return;\n";
        else
            return "return "
                    +nameProvider->findVariable(
                        module->header.name,
                        algorhitm->header.name,
                        "__retval_of_"+algorhitm->header.name)
                    +";\n";
    }
    else {
        return "break;\n";
    }
}

QString KumirCppGeneratorPrivate::makeStVarInitialize(
    const QList<AST::Variable *> vars,
    const AST::Algorhitm *algorhitm,
    const AST::Module *module) const
{
    QString result;
    foreach (const AST::Variable * var, vars) {
        if (var->accessType==AST::AccessRegular && var->dimension>0) {
            const QString cName = nameProvider->findVariable(
                        module->header.name,
                        algorhitm? algorhitm->header.name : "",
                        var->name);
            QString t;
            if (var->baseType==AST::TypeInteger)
                t = "i";
            else if (var->baseType==AST::TypeReal)
                t = "r";
            else if (var->baseType==AST::TypeBoolean)
                t = "b";
            else if (var->baseType==AST::TypeCharect)
                t = "c";
            else if (var->baseType==AST::TypeString)
                t = "s";
            result += QString("%1.type = '%2';\n%1.dim = %3;\n")
                    .arg(cName)
                    .arg(t)
                    .arg(var->dimension);
            for (int i=0; i<var->dimension; i++) {
                result += QString("%1.lefts[%2] = %3;\n%1.rights[%2] = %4;\n")
                        .arg(cName)
                        .arg(i)
                        .arg(makeExpression(var->bounds[i].first, algorhitm, module))
                        .arg(makeExpression(var->bounds[i].second, algorhitm, module));
            }
            result += "__allocate_array__(&"+cName+");\n";
        }
        else if (var->accessType==AST::AccessArgumentIn && var->dimension>0) {
            const QString cName = nameProvider->findVariable(
                        module->header.name,
                        algorhitm? algorhitm->header.name : "",
                        var->name);
            for (int i=0; i<var->dimension; i++) {
                result += QString("%1.lefts[%2] = %3;\n%1.rights[%2] = %4;\n")
                        .arg(cName)
                        .arg(i)
                        .arg(makeExpression(var->bounds[i].first, algorhitm, module))
                        .arg(makeExpression(var->bounds[i].second, algorhitm, module));
            }
            result += "__copy_array__(&"+cName+", &__src_of_"+cName+"__);";

        }
    }
    return result;
}


void KumirCppGeneratorPlugin::start()
{

}

void KumirCppGeneratorPlugin::stop()
{

}



Shared::GeneratorType KumirCppGeneratorPlugin::generateExecuable(
    const AST::Data *tree
    , QIODevice *out)
{
    d->ast = tree;
    for (int i=0; i<tree->modules.size(); i++) {
        if (tree->modules[i]->header.enabled)
            d->addModule(tree->modules[i]);
    }
    for (int i=0; i<tree->modules.size(); i++) {
        if (tree->modules[i]->header.enabled)
            d->createModuleHeader(tree->modules[i]);
    }
    for (int i=0; i<tree->modules.size(); i++) {
        if (tree->modules[i]->header.enabled && tree->modules[i]->header.type!=AST::ModTypeExternal)
            d->createModuleSource(tree->modules[i]);
    }
    if (out && out->metaObject()->className()==QString("QFile")) {
        QFile * f = qobject_cast<QFile*>(out);
        const QString fileName = QFileInfo(*f).fileName();
        const QString buildDir = "build-"+fileName;
        QDir::current().mkdir(buildDir);
        QDir::setCurrent(buildDir);
    }
    QSet<QString> cFiles;
    QSet<QString> hFiles;
    QSet<QString> libs;
    for (int i=0; i<d->modules.size(); i++) {
        const QString hFileName = d->modules[i]->cNamespace.isEmpty()
                ? QString("main.h")
                : d->modules[i]->cNamespace+".h";
        const QString cFileName = d->modules[i]->cNamespace.isEmpty()
                ? QString("main.c")
                : d->modules[i]->cNamespace+".c";
        QFile h(hFileName);
        if (h.open(QIODevice::WriteOnly | QIODevice::Text)) {
            h.write(d->modules[i]->headerData.toLocal8Bit());
            h.close();
            hFiles.insert(hFileName);
        }
        QFile c(cFileName);
        if (c.open(QIODevice::WriteOnly | QIODevice::Text)) {
            c.write(d->modules[i]->sourceData.toLocal8Bit());
            c.close();
            cFiles.insert(cFileName);
        }

        if (!d->modules[i]->cLibrary.isEmpty()) {
            QStringList libNames = d->modules[i]->cLibrary.split(QRegExp("\\s+"));
            foreach (QString libName, libNames) {
#ifdef Q_OS_MAC
#ifndef QT_NO_DEBUG
                    libName += "_debug";
#endif
#endif
#ifdef Q_OS_WIN32
#ifndef QT_NO_DEBUG
                    libName += "d";
#endif
#endif
                qDebug() << libName;
                libs.insert(libName);
            }
        }
    }
    cFiles.insert("__kumir__.c");
    hFiles.insert("__kumir__.h");

    const QString includePath = qApp->property("sharePath").toString()+"/kumircppgenerator/";
    QString gccOutName;
#ifdef Q_OS_WIN32
    gccOutName = "out.exe";
#else
    gccOutName = "out.bin";
#endif

    QStringList ldPaths;

    ldPaths << QDir::cleanPath(QFileInfo(pluginSpec().libraryFileName).absoluteDir().path());
    ldPaths << QDir::cleanPath(QFileInfo(pluginSpec().libraryFileName).absoluteDir().path()+"/../");

//#ifdef Q_OS_MAC
    const QString frameworksPath = QDir::cleanPath(qApp->applicationDirPath()+"/../Frameworks/");
    const QDir frameworksDir(frameworksPath);
    QStringList frameworksOpts;
    foreach (QString fw, frameworksDir.entryList()) {
        if (fw=="." || fw=="..")
            continue;
        ldPaths << frameworksPath+"/"+fw+"/Versions/4";
        fw.remove(".framework");
        frameworksOpts << " -framework "+fw;
    }

//#endif


    QFile::remove("__kumir__.h");
    QFile::remove("__kumir__.c");
    QFile::copy(includePath+"/__kumir__.h", "__kumir__.h");
    QFile::copy(includePath+"/__kumir__.c", "__kumir__.c");
    QString command = "gcc";
#ifdef Q_OS_WIN32
    command = "mingw32-gcc";
#endif
#ifdef Q_OS_MAC
    command += " -F"+frameworksPath;
    command += " -L"+frameworksPath;
    command += " -Wl,-install_name="+ldPaths.join(":");
    command += frameworksOpts.join(" ");
#else
    command += " -Wl,-rpath="+ldPaths.join(":");
#endif
    command += " -o "+gccOutName;
    command += " --std=c99";
    command += " -Werror";
    foreach (const QString L, ldPaths) {
        command += " -L"+L;
    }
    foreach (const QString lib, libs.toList()) {
        command += " "+lib;
    }
    command += " "+QStringList(cFiles.toList()).join(" ");
    if (qApp->arguments().contains("-V")) {
        std::cout << command.toLocal8Bit().data() << std::endl;
    }
    QProcess::execute(command);
    Shared::GeneratorType result = Shared::GenError;
    if (out && QFile::exists(gccOutName)) {
        QFile outF(gccOutName);
        if (outF.open(QIODevice::ReadOnly)) {
            out->write(outF.readAll());
            outF.close();

        }
        result = Shared::GenNativeExecuable;
        QFile::remove(gccOutName);
    }
    if (!qApp->arguments().contains("-S")) {
        foreach (QString fn, cFiles.toList()) {
            QFile::remove(fn);
        }
        foreach (QString fn, hFiles.toList()) {
            QFile::remove(fn);
        }
    }
    if (out && out->metaObject()->className()==QString("QFile")) {
        QDir::setCurrent("..");
        if (!qApp->arguments().contains("-S")) {
            QFile * f = qobject_cast<QFile*>(out);
            const QString fileName = QFileInfo(*f).fileName();
            const QString buildDir = "build-"+fileName;
            QDir::current().rmdir(buildDir);
        }
    }
    return result;
}

}

Q_EXPORT_PLUGIN(KumirCppGenerator::KumirCppGeneratorPlugin)
