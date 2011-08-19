#include "kumirnativegeneratorplugin.h"
#include "dataformats/ast_module.h"
#include "dataformats/ast_algorhitm.h"
#include "dataformats/ast_variable.h"
#include "dataformats/ast_expression.h"
#include "nameprovider.h"

#include <QtCore>
#include <iostream>


namespace KumirNativeGenerator {


struct Module {
    QString name;
    QString cNamespace;
    QStringList cLibraries;
    QStringList qtLibraries;
    QString headerData;
    QString sourceData;
    QMap<QString,QString> algorhitmHeaders;
    bool requireGui;
};

struct KumirNativeGeneratorPrivate {
    QList<Module*> modules;
    NameProvider * nameProvider;
    bool requireGui;
    const AST::Data * ast;
    void addModule(const AST::Module * module);
    void createModuleHeader(const AST::Module * module);
    void createModuleSource(const AST::Module * module);
    QString createAlgorhitmHeader(const AST::Algorhitm * algorhitm,
                                  const QString &moduleName);
    QString makeExpression(const AST::Expression * expr,
                           const AST::Algorhitm * algorhitm,
                           const AST::Module * module,
                           bool lvalue
                           ) const;
    QString makeConstant(AST::VariableBaseType type,
                         const QVariant & value) const;
    QString makeArrayElement(const AST::Variable * var,
                             const QList<AST::Expression *> & expr,
                             const AST::Algorhitm * algorhitm,
                             const AST::Module * module,
                             bool lvalue) const;
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

KumirNativeGeneratorPlugin::KumirNativeGeneratorPlugin()
{
    d = new KumirNativeGeneratorPrivate;
    d->nameProvider = new NameProvider;
}

KumirNativeGeneratorPlugin::~KumirNativeGeneratorPlugin()
{
    for (int i=0; i<d->modules.size(); i++)
        delete d->modules[i];
    delete d;
}

QString KumirNativeGeneratorPlugin::initialize(const QStringList &arguments)
{
    Q_UNUSED(arguments)
    return "";
}

void KumirNativeGeneratorPrivate::addModule(const AST::Module *module)
{
    if (!module->header.enabled)
        return;
    Module * mod = new Module;
    mod->name = module->header.name;
    if (module->header.type==AST::ModTypeExternal) {
        mod->cNamespace = module->header.cReference.nameSpace;
        mod->cLibraries = module->header.cReference.moduleLibraries;
        mod->qtLibraries = module->header.cReference.usedQtLibraries;
        mod->requireGui = module->header.cReference.requiresGuiEventLoop;
        requireGui |= mod->requireGui;
    }
    else {
        mod->requireGui = false;
        if (!module->header.name.isEmpty()) {
            mod->cNamespace = nameProvider->addName(module->header.name, "");
        }
    }
    modules << mod;
}

QString KumirNativeGeneratorPrivate::createAlgorhitmHeader(const AST::Algorhitm *algorhitm, const QString &moduleName)
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

QString KumirNativeGeneratorPrivate::makeExpression(
    const AST::Expression *expr,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module,
    bool lvalue
    ) const
{
    bool addEqual = lvalue;
    const QString modName = module->header.name;
    const QString algName = algorhitm? algorhitm->header.name : "";
    QString result;
    if (expr->kind==AST::ExprVariable) {
        if (expr->variable->name==algName)
            result = "__retval_of_"+nameProvider->name(algName, "");
        else {
            result = nameProvider->findVariable(modName, algName, expr->variable->name);
            if (expr->variable->accessType==AST::AccessArgumentInOut || expr->variable->accessType==AST::AccessArgumentOut)
                result = "(*"+result+")";
        }
    }
    else if (expr->kind==AST::ExprConst) {
        result = makeConstant(expr->baseType, expr->constant);
    }
    else if (expr->kind==AST::ExprArrayElement) {
        int dimVar = expr->variable->dimension;
        int dimReal = expr->operands.size();
        int diff = dimReal - dimVar;
        QList<AST::Expression*> arrOperands = expr->operands.mid(0,dimVar);
        QString arrayElement;
        if (dimVar>0)
            arrayElement = makeArrayElement(expr->variable, arrOperands, algorhitm, module, lvalue);
        else {
            if (expr->variable->name==algName)
                arrayElement = "__retval_of_"+nameProvider->name(algName, "");
            else
                arrayElement = nameProvider->findVariable(modName, algName, expr->variable->name);
        }
        if (expr->variable->accessType==AST::AccessArgumentInOut || expr->variable->accessType==AST::AccessArgumentOut)
            arrayElement = "(*"+arrayElement+")";
        Q_ASSERT(diff==0 || diff==1 || diff==2);
        if (diff==2) {

            // string slice
            if (lvalue) {
                result = "__set_slice_%2__(&"+arrayElement+", "
                        +makeExpression(expr->operands[expr->operands.size()-2], algorhitm, module, false)
                        +", "
                        +makeExpression(expr->operands[expr->operands.size()-1], algorhitm, module, false)
                        +", %1)";
            }
            else {
                addEqual = false;
                result = "__get_slice__("+arrayElement+", "
                        +makeExpression(expr->operands[expr->operands.size()-2], algorhitm, module, false)
                        +", "
                        +makeExpression(expr->operands[expr->operands.size()-1], algorhitm, module, false)
                        +")";
            }
        }
        else if (diff==1) {
            // string element
            if (lvalue) {
                addEqual = false;
                result = "__set_char_at__("+arrayElement+", "
                        +makeExpression(expr->operands[expr->operands.size()-1], algorhitm, module, false)
                        +", %1)";

            }
            else {
                result = "__get_char_at__("+arrayElement+", "
                        +makeExpression(expr->operands[expr->operands.size()-1], algorhitm, module, false)
                        +")";
            }
        }
        else {
            // regular table access
            result = arrayElement;
        }
    }
    else if (expr->kind==AST::ExprFunctionCall) {
        result = makeFunctionCall(expr->function, expr->operands, algorhitm, module);
    }
    else if (expr->kind==AST::ExprSubexpression) {
        result = makeSubExpression(expr->operands, expr->operatorr, algorhitm, module);
    }

    if (lvalue) {
        if (addEqual)
            result += " = %3 %1 %4;";
        else
            result += ";";
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
            result += "\\x"+QString::number(s[i].unicode(), 16);
        }
        else {
            result += s[i];
        }
    }
    return result;
}

QString KumirNativeGeneratorPrivate::makeConstant(
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
        result = "__string_from_constant__(L\""+screenString(value.toString())+"\")";
    }
    return result;
}

QString KumirNativeGeneratorPrivate::makeArrayElement(
    const AST::Variable *var,
    const QList<AST::Expression *> & expr,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module,
    bool lvalue) const
{
    Q_UNUSED(lvalue);
    const QString modName = module->header.name;
    const QString algName = algorhitm? algorhitm->header.name : "";
    QString cName = nameProvider->findVariable(modName, algName, var->name);
    QString cType = nameProvider->baseTypeName(var->baseType);
    QStringList indeces;
    for (int i=0; i<3; i++) {
        if (i<expr.size())
            indeces << makeExpression(expr[i], algorhitm, module, false);
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

QString KumirNativeGeneratorPrivate::makeFunctionCall(
    const AST::Algorhitm *function,
    const QList<AST::Expression *> &args,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module) const
{
    const QString modName = module->header.name;
    QString cName = nameProvider->name(function->header.name, modName);
    if (function->header.implType==AST::AlgorhitmExternal) {
        QString cHeader = function->header.cHeader;
        Q_ASSERT_X(!cHeader.isEmpty(),
                   "C-generator: makeFunctionCall",
                   QString("Has no C-header for %1:%2")
                   .arg(function->header.external.moduleName)
                   .arg(function->header.name)
                   .toLocal8Bit().data());
        int spacePos = cHeader.indexOf(" ");
        int brPos = cHeader.indexOf("(");
        Q_ASSERT_X(spacePos!=-1 && brPos!=-1 && brPos > spacePos,
                   "C-generator: makeFunctionCall",
                   QString("Invalid C-header for %1:%2")
                   .arg(function->header.external.moduleName)
                   .arg(function->header.name)
                   .toLocal8Bit().data());
        cName = cHeader.mid(spacePos+1, brPos-spacePos-1);
    }
    QStringList arguments;
    for (int i=0; i<args.size(); i++) {
        QString a = makeExpression(args[i], algorhitm, module, false);
        if (function->header.arguments[i]->accessType==AST::AccessArgumentInOut||
                function->header.arguments[i]->accessType==AST::AccessArgumentOut)
            a = "&("+a+")";
        arguments << a;
    }
    return cName+"("+arguments.join(", ")+")";
}

QString KumirNativeGeneratorPrivate::makeSubExpression(
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
        result += makeExpression(operands[0], algorhitm, module, false);
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
            result += makeExpression(operands[0], algorhitm, module, false);
            result += ", ";
            result += makeExpression(operands[1], algorhitm, module, false);
            result += ")";
        }
        else if (op==AST::OpSumm && (operands[0]->baseType==AST::TypeCharect || operands[0]->baseType==AST::TypeString)) {
            QString ft = operands[0]->baseType==AST::TypeCharect? "c" : "s";
            QString st = operands[1]->baseType==AST::TypeCharect? "c" : "s";
            result += "__concatenate_"+ft+st+"__(";
            result += makeExpression(operands[0], algorhitm, module, false);
            result += ", ";
            result += makeExpression(operands[1], algorhitm, module, false);
            result += ")";
        }

        else if (op==AST::OpDivision) {
            if (operands[0]->baseType==AST::TypeReal && operands[1]->baseType==AST::TypeInteger) {
                result += "__safe_div_ri__(";
            }
            if (operands[0]->baseType==AST::TypeReal && operands[1]->baseType==AST::TypeReal) {
                result += "__safe_div_rr__(";
            }
            if (operands[0]->baseType==AST::TypeInteger && operands[1]->baseType==AST::TypeReal) {
                result += "__safe_div_ir__(";
            }
            if (operands[0]->baseType==AST::TypeInteger && operands[1]->baseType==AST::TypeInteger) {
                result += "__safe_div_ii__(";
            }
            result += makeExpression(operands[0], algorhitm, module, false);
            result += ", ";
            result += makeExpression(operands[1], algorhitm, module, false);
            result += ")";
        }
        else {
            result += "(";
            result += makeExpression(operands[0], algorhitm, module, false);
            result += nameProvider->operatorName(op);
            result += makeExpression(operands[1], algorhitm, module, false);
            result += ")";
        }
    }
    return result;
}


void KumirNativeGeneratorPrivate::createModuleHeader(const AST::Module *module)
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
        mod->headerData += "extern void __create__"+mod->cNamespace+"();\n";
        mod->headerData += "extern void __init__"+mod->cNamespace+"();\n";
    }
    for (int i=0; i<module->header.algorhitms.size(); i++) {
        const AST::Algorhitm * alg = module->header.algorhitms[i];
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

void KumirNativeGeneratorPrivate::createModuleSource(const AST::Module *module)
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
        QString cName = nameProvider->addName(module->impl.globals[i]->name
                                              , module->header.name);
        QString cType = nameProvider->baseTypeName(module->impl.globals[i]->baseType);
        if (module->impl.globals[i]->dimension>0) {
            cType = QString("struct __array__");
        }
        mod->sourceData += QString("static %1 %3;\n")
                .arg(cType).arg(cName);
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

QString KumirNativeGeneratorPrivate::makeMain() const
{
    QString result;
    result += "#include <locale.h>\n";
    if (!requireGui) {
        result += "extern void __try_connect_to_kumir__(int argc, char** argv);\n";
        result += "int main(int argc, char** argv) {\n";
        result += "  __try_connect_to_kumir__(argc, argv);\n";
    }
    else
        result += "void __main_program__() {\n";
    result += "  __init_garbage_collector__();\n";
    result += "#ifndef WIN32\n";
    result += "  setlocale(LC_CTYPE, \"ru_RU.UTF-8\");\n";
    result += "#else\n";
    result += "  setlocale(LC_CTYPE, \".1251\");\n";
    result += "#endif\n";
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
    result += "  __free_garbage_collector__();\n";
    result += "  __wait_for_output_queue_flushed__();\n";
    if (!requireGui)
        result += "  return 0;\n";
    result += "}\n";
    if (requireGui) {
        result += "\n";
        result += "void __prepare__gui__() {\n";
        for (int i=0; i<modules.size(); i++) {
            Module * m = modules[i];
            if (m->requireGui) {
                result += "  __create__"+m->cNamespace+"();\n";
            }
        }
        result += "}\n\n";
        result += "extern void __main_gui__( int argc, char *argv[], void(*creat)(), void(*func)() );\n\n";
        result += "int main(int argc, char *argv[]) {\n";
        result += "  __main_gui__(argc, argv, __prepare__gui__, __main_program__);\n";
        result += "  return 0;\n";
        result += "}\n";
    }

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

QString KumirNativeGeneratorPrivate::makeAlgorhitm(
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
    result += "  __garbage_collector_begin_algorhitm__();\n";
    for (int i=0; i<alg->impl.locals.size(); i++) {
        if (alg->impl.locals[i]->accessType==AST::AccessRegular) {
            QString locName = alg->impl.locals[i]->name;
            if (locName==alg->header.name) {
                locName = "__retval_of_"+locName;
            }
            const QString cName = nameProvider->addName(locName, modName+"::"+alg->header.name);
            QString cType = nameProvider->baseTypeName(alg->impl.locals[i]->baseType);
            if (alg->impl.locals[i]->dimension>0) {
                cType = QString("struct __array__");
            }
            if (alg->impl.locals[i]->accessType==AST::AccessRegular ||
                    (alg->impl.locals[i]->accessType==AST::AccessArgumentIn &&
                     alg->impl.locals[i]->dimension>0
                     ))
            {
                result += "  "+cType+" "+cName+";\n";
            }
        }
    }
    result += addIndent(makeBody(alg->impl.pre , 0, alg, module))+"\n";
    result += addIndent(makeBody(alg->impl.body, 0, alg, module))+"\n";
    result += addIndent(makeBody(alg->impl.post, 0, alg, module))+"\n";
    if (alg->header.returnType==AST::TypeString)
        result += "  __garbage_collector_set_return_value__(__retval_of_"+nameProvider->name(alg->header.name,"")+");\n";
    for (int i=0; i<alg->header.arguments.size(); i++) {
        AST::Variable * var = alg->header.arguments[i];
        if (var->accessType==AST::AccessArgumentOut || var->accessType==AST::AccessArgumentInOut) {
            const QString & varName = nameProvider->findVariable(module->header.name, alg->header.name, var->name);
            if (var->baseType==AST::TypeString && var->dimension==0)
                result += "  __garbage_collector_set_return_value__(*"+varName+");\n";
            if (var->dimension>0)
                result += "  __garbage_collector_set_return_array__("+varName+");\n";
        }
    }
    result += "  __garbage_collector_end_algorhitm__();\n";
    if (alg->header.returnType!=AST::TypeNone)
        result += "  return __retval_of_"+nameProvider->name(alg->header.name,"")+";\n";
    result += "}\n";
    return result;
}

QString KumirNativeGeneratorPrivate::makeInitializator(const AST::Module *module)
{
    QString result = "extern void __init__()\n{\n";
    result += addIndent(makeBody(module->impl.initializerBody, 0, 0, module));
    result += "\n}\n";
    return result;
}

QString KumirNativeGeneratorPrivate::makeBody(
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

QString KumirNativeGeneratorPrivate::makeStError(const QString &text, int lineNo) const
{
    const QString textConst = makeConstant(AST::TypeString, text);
    return "__abort__("+textConst+", "+QString::number(lineNo)+");\n";
}

QString KumirNativeGeneratorPrivate::makeStAssign(
    const QList<AST::Expression *> &exprs,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module) const
{
    Q_ASSERT(exprs.size()==1 || exprs.size()==2);
    const QString right = makeExpression(exprs[0], algorhitm, module, false);
    QString rt;
    if (exprs[0]->baseType==AST::TypeString)
        rt = "s";
    else if (exprs[0]->baseType==AST::TypeCharect)
        rt = "c";
    else if (exprs[0]->baseType==AST::TypeBoolean)
        rt = "b";
    else if (exprs[0]->baseType==AST::TypeInteger)
        rt = "i";
    else if (exprs[0]->baseType==AST::TypeReal)
        rt = "r";
    QString result;
    if (exprs.size()==2) {
        const QString left = makeExpression(exprs[1], algorhitm, module, true);
        result = left.arg(right)+"\n";
        if (result.contains("%2"))
            result.replace("%2", rt);
        if (exprs[0]->baseType==AST::TypeCharect && exprs[1]->baseType==AST::TypeString) {
            result.replace("%3", "__string_of_char__(");
            result.replace("%4", ")");
        }
        result.replace("%3", "");
        result.replace("%4", "");


    }
    else {
        result = right + ";\n";
    }
    return result;
}

QString KumirNativeGeneratorPrivate::makeStAssert(
    const QList<AST::Expression *> &exprs,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module) const
{
    QString result;
    for (int i=0; i<exprs.size(); i++) {
        const QString e = makeExpression(exprs[i], algorhitm, module, false);
        const QString text = QObject::tr("Assertion false");
        int lineNo = -1;
        result += "if ( !("+e+") ) {\n";
        result += "  "+makeStError(text, lineNo);
        result += "}\n";
    }
    return result;
}

QString KumirNativeGeneratorPrivate::makeStInput(
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
            format += "%b";
        else if (exprs[i]->baseType==AST::TypeCharect)
            format += "%c";
        else if (exprs[i]->baseType==AST::TypeString)
            format += "%s";
        es << "&("+makeExpression(exprs[i], algorhitm, module, false)+")";
    }
    result = "__input__st_funct(\""+format+"\", "+QString::number(es.size())+", "+
            es.join(", ")+");\n";
    return result;
}

QString KumirNativeGeneratorPrivate::makeStOutput(
    const QList<AST::Expression *> &exprs,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module) const
{
    QString result;
    QString format;
    QStringList es;
    for (int i=0; i<exprs.size(); i++) {
        if (exprs[i]->baseType==AST::TypeInteger)
            format += "i";
        else if (exprs[i]->baseType==AST::TypeReal)
            format += "r";
        else if (exprs[i]->baseType==AST::TypeBoolean)
            format += "b";
        else if (exprs[i]->baseType==AST::TypeCharect)
            format += "c";
        else if (exprs[i]->baseType==AST::TypeString)
            format += "s";
        es << makeExpression(exprs[i], algorhitm, module, false);
    }
    result = "__output__st_funct(\""+format+"\", "+QString::number(es.size())+", "+
            es.join(", ")+");\n";
    return result;
}

QString KumirNativeGeneratorPrivate::makeStLoop(
    const AST::LoopSpec &loop,
    int deep,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module)
{
    QString result;
    if (loop.type==AST::LoopTimes) {
        result += QString("for (int __counter_%1__=0; __counter_%1__<%2; __counter_%1__++) {\n")
                .arg(deep)
                .arg(makeExpression(loop.timesValue, algorhitm, module, false));
    }
    else if (loop.type==AST::LoopFor) {
        const QString modName = module->header.name;
        const QString algName = algorhitm? algorhitm->header.name : "";
        QString var = nameProvider->findVariable(modName, algName, loop.forVariable->name);
        QString from = makeExpression(loop.fromValue, algorhitm, module, false);
        QString to = makeExpression(loop.toValue, algorhitm, module,false);
        QString step = loop.stepValue? makeExpression(loop.stepValue, algorhitm, module,false) : "1";
        result += QString("for (%1=%2; %4>=0? %1<=%3 : %1>=%3; %1+=%4 ) {\n")
                .arg(var).arg(from).arg(to).arg(step);
    }
    else if (loop.type==AST::LoopWhile) {
        QString cond = makeExpression(loop.whileCondition, algorhitm, module,false);
        result += QString("while (%1) {\n").arg(cond);
    }
    result += addIndent(makeBody(loop.body, deep+1, algorhitm, module))+"\n";
    if (loop.endCondition) {
        QString cond = makeExpression(loop.endCondition, algorhitm, module,false);
        result += QString("  if (%1) break;\n").arg(cond);
    }
    result += "}\n";
    return result;
}

QString KumirNativeGeneratorPrivate::makeStIfThenElse(
    const QList<AST::ConditionSpec> &conds,
    int deep,
    const AST::Algorhitm * algorhitm,
    const AST::Module * module)
{
    QString result;
    for (int i=0; i<conds.size(); i++) {
        const QString c = conds[i].condition? makeExpression(conds[i].condition, algorhitm, module, false) : "";
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

QString KumirNativeGeneratorPrivate::makeStBreak(
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

QString KumirNativeGeneratorPrivate:: makeStVarInitialize(
    const QList<AST::Variable *> vars,
    const AST::Algorhitm *algorhitm,
    const AST::Module *module) const
{
    QString result;
    foreach (const AST::Variable * var, vars) {
        if (var->initialValue.isValid()) {
            const QString cName = nameProvider->findVariable(
                        module->header.name,
                        algorhitm? algorhitm->header.name : "",
                        var->name);
            result += cName + " = "
                    + makeConstant(var->baseType, var->initialValue)
                    + ";\n";

        }
        else if (var->accessType==AST::AccessRegular && var->dimension>0) {
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
                        .arg(makeExpression(var->bounds[i].first, algorhitm, module, false))
                        .arg(makeExpression(var->bounds[i].second, algorhitm, module, false));
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
                        .arg(makeExpression(var->bounds[i].first, algorhitm, module, false))
                        .arg(makeExpression(var->bounds[i].second, algorhitm, module, false));
            }
            result += "__copy_array__(&"+cName+", &__src_of_"+cName+"__);";

        }
    }
    return result;
}


void KumirNativeGeneratorPlugin::start()
{

}

void KumirNativeGeneratorPlugin::stop()
{

}



Shared::GeneratorResult KumirNativeGeneratorPlugin::generateExecuable(
    const AST::Data *tree
    , QIODevice *out)
{
    d->ast = tree;
    d->requireGui = false;
    d->nameProvider->reset();
    for (int i=0; i<d->modules.size(); i++) {
        delete d->modules[i];
    }
    d->modules.clear();
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
        const QString buildDir = ".build-"+fileName;
        QDir::current().mkdir(buildDir);
        QDir::setCurrent(buildDir);
    }
    QSet<QString> cFiles;
    QSet<QString> hFiles;
    QSet<QString> libs;
    QSet<QString> qtLibs;
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
        if (!d->modules[i]->sourceData.isEmpty() && c.open(QIODevice::WriteOnly | QIODevice::Text)) {
            c.write(d->modules[i]->sourceData.toLocal8Bit());
            c.close();
            cFiles.insert(cFileName);
        }
        if (!d->modules[i]->cLibraries.isEmpty()) {
            QStringList libNames = d->modules[i]->cLibraries;

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
                libs.insert(libName);
            }
            foreach (QString qtLibName, d->modules[i]->qtLibraries) {
                qtLibs.insert(qtLibName);
            }
        }
    }

    hFiles.insert("__kumir__.h");

    const QString includePath = qApp->property("sharePath").toString()+"/kumirnativegenerator/";
    QString gccOutName;
    gccOutName = "out.bin";
#ifdef Q_OS_WIN32
    gccOutName = "out.exe";
#endif


    QStringList ldPaths;

    ldPaths << QDir::cleanPath(QFileInfo(pluginSpec().libraryFileName).absoluteDir().path()+"/../");

    QStringList frameworksOpts;
    foreach (QString fw, qtLibs) {
        frameworksOpts << " -framework "+fw;
    }

    QStringList dlls;
    foreach (QString fw, qtLibs) {
#ifndef QT_NO_DEBUG
        dlls << fw+"4d.dll";
#else
        dlls << fw+"4.dll";
#endif
    }
    foreach (QString l, libs.toList()) {
#ifndef QT_NO_DEBUG
        dlls << l+"d.dll";
#else
        dlls << l+".dll";
#endif
    }

    dlls << "mingwm10.dll" << "libgcc_s_dw2-1.dll";


    QFile::remove("__kumir__.h");

    QFile::copy(includePath+"/__kumir__.h", "__kumir__.h");

    QString command = "gcc";
#ifdef Q_OS_WIN32
    command += ".exe";
#endif

#ifdef Q_OS_MAC
    QString frameworksPath;
    command += " -F"+frameworksPath;
    command += " -L"+frameworksPath;
//    command += " -Wl,-install_name="+ldPaths.join(":");
    command += frameworksOpts.join(" ");
#else
    command += " -Wl,-rpath="+ldPaths.join(":");
#endif
    command += " -o "+gccOutName;
    command += " --std=c99";
//    command += " -Werror";
    command += " -g";
#ifdef Q_OS_WIN32
    command += " -DWIN32";
    command += " -Wl,-enable-auto-import";
    command += " -Wl,-enable-runtime-pseudo-reloc";
    command += " -mthreads";
    command += " -L"+QCoreApplication::applicationDirPath();
#endif

    foreach (const QString L, ldPaths) {
        command += " -L"+L;
    }

    command += " "+QStringList(cFiles.toList()).join(" ");

//    if (d->requireGui) {
//        QString libName = "KumirGuiRunner";
//#ifndef QT_NO_DEBUG
//#ifdef Q_OS_MAC
//        libName += "_debug";
//#endif
//#ifdef Q_OS_WIN32
//        libName += "d";
//#endif
//#endif
//        command += " -l"+libName;
//    }

    foreach (const QString lib, libs.toList()) {
        command += " -l"+lib;
#ifndef QT_NO_DEBUG
#ifdef Q_OS_WIN32
        command += "d";
#endif
#ifdef Q_OS_MAC
        command += "_debug";
#endif
#endif
    }
//#ifdef Q_OS_WIN32
//    foreach (const QString lib, qtLibs.toList()) {
//        command += " -l"+lib+"4";
//#ifndef QT_NO_DEBUG
//        command += "d";
//#endif
//    }
//#endif
    command += " -lm";

    if (qApp->arguments().contains("-V")) {
        std::cout << command.toLocal8Bit().data() << std::endl;
    }
    QProcess * proc = new QProcess(this);
    proc->setProcessChannelMode(QProcess::SeparateChannels);
#ifdef Q_OS_WIN32
    const QString mingwBinPath = QDir::toNativeSeparators(QDir::cleanPath(qApp->applicationDirPath()+"/../mingw/bin"));
//    std::cout << mingwBinPath.toLocal8Bit().data() << std::endl;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PATH", env.value("Path")+";"+mingwBinPath);
    command = mingwBinPath+"\\"+command;
    proc->setProcessEnvironment(env);
#endif

    proc->start(command);

    proc->waitForFinished(10000);
    proc->waitForReadyRead();

    if (qApp->arguments().contains("-V") || qApp->arguments()[0]!="kumir2-cc") {
//        std::cout << proc->errorString().toLocal8Bit().data();
        std::cout << proc->readAllStandardError().data();
        std::cout << proc->readAllStandardOutput().data();
    }

    Shared::GeneratorResult result;
    result.type = Shared::GenError;
    if (out && QFile::exists(gccOutName)) {
        QFile outF(gccOutName);
        if (outF.open(QIODevice::ReadOnly)) {
            out->write(outF.readAll());
            outF.flush();

        }
        result.type = Shared::GenNativeExecuable;
        if (!qApp->arguments().contains("-S"))
            QFile::remove(gccOutName);
    }
    if (!qApp->arguments().contains("-S")) {
//    if (false) {
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
            const QString buildDir = ".build-"+fileName;
            QDir::current().rmdir(buildDir);
        }
    }
    proc->deleteLater();
    result.usedLibs = libs.toList();
    result.usedQtLibs = qtLibs.toList();
#ifdef Q_OS_WIN32
    result.usedQtLibs += "libgcc_s_dw2-1";
    result.usedQtLibs += "mingwm10";
#endif
//    qDebug() << result.usedQtLibs;
    return result;
}

}

Q_EXPORT_PLUGIN(KumirNativeGenerator::KumirNativeGeneratorPlugin)
