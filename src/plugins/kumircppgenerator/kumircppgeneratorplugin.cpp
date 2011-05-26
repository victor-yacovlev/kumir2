#include "kumircppgeneratorplugin.h"
#include "abstractsyntaxtree/ast_module.h"
#include "abstractsyntaxtree/ast_algorhitm.h"
#include "abstractsyntaxtree/ast_variable.h"
#include "abstractsyntaxtree/ast_expression.h"
#include "nameprovider.h"

#include <QtCore>


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
    void addModule(const AST::Module * module);
    void createModuleHeader(const AST::Module * module);
    void createModuleSource(const AST::Module * module);
    QString createAlgorhitmHeader(const AST::Algorhitm * algorhitm,
                                  const QString &moduleName);
    QString makeExpression(const AST::Expression * expr,
                           const QString &modName,
                           const QString &algName) const;
    QString makeConstant(AST::VariableBaseType type,
                         const QVariant & value) const;
    QString makeArrayElement(const AST::Variable * var,
                             const QList<AST::Expression *> & expr,
                             const QString &modName,
                             const QString &algName) const;
    QString makeFunctionCall(const AST::Algorhitm * function,
                             const QList<AST::Expression *> & args,
                             const QString & modName,
                             const QString & algName) const;
    QString makeSubExpression(const QList<AST::Expression *> & operands,
                              AST::ExpressionOperator op,
                              const QString &modName,
                              const QString &algName) const;
    QString makeAlgorhitm(const AST::Algorhitm * alg,
                          const AST::Module * modName);
    QString makeInitializator(const AST::Module * module) const;
    QString makeBody(const QList<AST::Variable*> &vars,
                     const QList<AST::Statement*> &statements,
                     const QString & algName,
                     const QString & modName) const;

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
    const QString cName = nameProvider->addName(algorhitm->header.name, moduleName);
    const QString returnType = nameProvider->baseTypeName(algorhitm->header.returnType);
    QStringList args;
    foreach (const AST::Variable * var, algorhitm->header.arguments) {
        QString argType = nameProvider->baseTypeName(var->baseType);
        QString argMod = var->accessType!=AST::AccessRegular? "*" : "";
        QString argName = nameProvider->addName(var->name, moduleName+"::"+algorhitm->header.name);
        QString argBounds = "";
        for (int i=0; i<var->dimension; i++) {
            argBounds += "*";
        }
        args << argType + argBounds + " " + argMod + argName;
    }
    return returnType+" "+cName+"("+args.join(", ")+")";
}

QString KumirCppGeneratorPrivate::makeExpression(
    const AST::Expression *expr,
    const QString &modName,
    const QString &algName) const
{
    QString result;
    if (expr->kind==AST::ExprVariable) {
        result = nameProvider->findVariable(modName, algName, expr->variable->name);
    }
    else if (expr->kind==AST::ExprConst) {
        result = makeConstant(expr->baseType, expr->constant);
    }
    else if (expr->kind==AST::ExprArrayElement) {
        result = makeArrayElement(expr->variable, expr->operands, modName, algName);
    }
    else if (expr->kind==AST::ExprFunctionCall) {
        result = makeFunctionCall(expr->function, expr->operands, modName, algName);
    }
    else if (expr->kind==AST::ExprSubexpression) {
        result = makeSubExpression(expr->operands, expr->operatorr, modName, algName);
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
    const QString &modName,
    const QString &algName) const
{
    QString result = nameProvider->findVariable(modName, algName, var->name);
    for (int i=0; i<expr.size(); i++) {
        result += "[";
        result += makeExpression(expr[i], modName, algName);
        if (i<var->dimension) {
            result += "-";
            result += makeExpression(var->bounds[i].first, modName, algName);
        }
        else {
            // String element
            result += "-1";
        }
        result += "]";
    }
    return result;
}

QString KumirCppGeneratorPrivate::makeFunctionCall(
    const AST::Algorhitm *function,
    const QList<AST::Expression *> &args,
    const QString &modName,
    const QString &algName) const
{
    const QString cName = nameProvider->name(function->header.name, modName);
    QStringList arguments;
    for (int i=0; i<args.size(); i++) {
        arguments << makeExpression(args[i], modName, algName);
    }
    return cName+"("+arguments.join(", ")+")";
}

QString KumirCppGeneratorPrivate::makeSubExpression(
    const QList<AST::Expression *> &operands,
    AST::ExpressionOperator op,
    const QString &modName,
    const QString &algName) const
{
    QString result;
    Q_ASSERT(operands.size()==1 || operands.size()==2);
    if (operands.size()==1) {
        Q_ASSERT(op==AST::OpNot || op==AST::OpSubstract);
        result += "(";
        result += nameProvider->operatorName(op);
        result += makeExpression(operands[0], modName, algName);
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
            result += makeExpression(operands[0], modName, algName);
            result += ", ";
            result += makeExpression(operands[1], modName, algName);
            result += ")";
        }
        else if (op==AST::OpSumm && (operands[0]->baseType==AST::TypeCharect || operands[0]->baseType==AST::TypeString)) {
            result += "__concatenate__(";
            result += makeExpression(operands[0], modName, algName);
            result += ", ";
            result += makeExpression(operands[1], modName, algName);
            result += ")";
        }
        else {
            result += "(";
            result += makeExpression(operands[0], modName, algName);
            result += nameProvider->operatorName(op);
            result += makeExpression(operands[1], modName, algName);
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
        mod->headerData += QString("namespace %1 {\n\n").arg(mod->cNamespace);
    }
    foreach (const AST::Algorhitm * alg, module->header.algorhitms) {
        if (module->header.type==AST::ModTypeExternal) {
            mod->headerData += "extern "+alg->header.cHeader;
        }
        else {
            QString cHeader = createAlgorhitmHeader(alg, mod->name);
            mod->algorhitmHeaders[alg->header.name] = cHeader;
            mod->headerData += "extern "+cHeader+";\n";
        }
    }

    if (!mod->cNamespace.isEmpty()) {
        mod->headerData += "\n}\n";
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
    mod->sourceData += QString("#include \"%1.h\"\n")
            .arg(mod->cNamespace.isEmpty()? "main" : mod->cNamespace.toLower());

    mod->sourceData += "\n";
    if (!mod->cNamespace.isEmpty()) {
        mod->sourceData+="namespace "+mod->cNamespace+" {\n";
    }

    for (int i=0; i<module->impl.globals.size(); i++) {
        QString cName = nameProvider->addName(module->impl.globals[i]->name, module->header.name);
        QString cType = nameProvider->baseTypeName(module->impl.globals[i]->baseType);
        QString bounds;
        for (int j=0; j<module->impl.globals[i]->dimension; j++) {
            bounds += "*";
        }
        mod->sourceData += QString("static %1%2 %3;\n")
                .arg(cType).arg(bounds).arg(cName);
    }

    mod->sourceData += "\n";

    foreach (const AST::Algorhitm * alg, module->impl.algorhitms) {
        mod->sourceData += makeAlgorhitm(alg, module);
    }

    mod->sourceData += makeInitializator(module);

    if (!mod->cNamespace.isEmpty()) {
        mod->sourceData+="\n}";
    }

    mod->sourceData += "\n";
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
    for (int i=0; i<alg->impl.locals.size(); i++) {
        nameProvider->addName(alg->impl.locals[i]->name, modName+"::"+alg->header.name);
    }
    const QString algName = alg->header.name;
    QString result = header + "\n{\n";
    result += addIndent(makeBody(alg->impl.locals, alg->impl.pre, algName, modName))+"\n";
    result += addIndent(makeBody(alg->impl.locals, alg->impl.body, algName, modName))+"\n";
    result += addIndent(makeBody(alg->impl.locals, alg->impl.post, algName, modName))+"\n";
    result += "}\n";
    return result;
}

QString KumirCppGeneratorPrivate::makeInitializator(const AST::Module *module) const
{
    QString result = "extern __init__()\n{\n";
    result += addIndent(makeBody(module->impl.globals, module->impl.initializerBody, "", module->header.name));
    result += "\n}\n";
    return result;
}

QString KumirCppGeneratorPrivate::makeBody(
    const QList<AST::Variable *> &vars,
    const QList<AST::Statement *> &statements,
    const QString &algName,
    const QString &modName) const
{
    QString result;

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
        }
        QFile c(cFileName);
        if (c.open(QIODevice::WriteOnly | QIODevice::Text)) {
            c.write(d->modules[i]->sourceData.toLocal8Bit());
            c.close();
        }
    }
    Q_UNUSED(out);
    return Shared::GenNativeExecuable;
}

}

Q_EXPORT_PLUGIN(KumirCppGenerator::KumirCppGeneratorPlugin)
