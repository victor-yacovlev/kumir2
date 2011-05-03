#include "ast_expression.h"
#include "ast_module.h"
#include "ast_algorhitm.h"
#include "ast.h"

namespace AST {

Expression::Expression()
{
    kind = ExprNone;
    baseType = TypeNone;
    variable = NULL;
    function = NULL;
    constant = QVariant::Invalid;
    dimension = 0;
}

Expression::Expression(const struct Expression * src)
{
    kind = src->kind;
    baseType = src->baseType;
    constant = src->constant;
    dimension = src->dimension;
    for (int i=0; i<src->operands.size(); i++) {
        operands << new Expression(src->operands[i]);
    }
    operatorr = src->operatorr;
}

void Expression::updateReferences(const Expression *src, const Data *srcData, const Data *data)
{
    if (src->variable) {
        int modId = -1;
        int varId = -1;
        int algId = -1;
        for (int i=0; i<srcData->modules.size(); i++) {
            modId = i;
            algId = -1;
            varId = -1;
            struct Module * mod = srcData->modules[i];
            for (int j=0; j<mod->impl.globals.size(); j++) {
                if (mod->impl.globals[j]==src->variable) {
                    varId = j;
                    break;
                }
            }
            if (varId!=-1)
                break;
            for (int j=0; j<mod->impl.algorhitms.size(); j++) {
                struct Algorhitm * alg = mod->impl.algorhitms[j];
                algId = j;
                for (int k=0; k<alg->impl.locals.size(); k++) {
                    if (alg->impl.locals[k]==src->variable) {
                        varId = k;
                        break;
                    }
                }
                if (varId>-1)
                    break;
            }
            if (varId>-1)
                break;
        }
        Q_ASSERT(modId>-1);
        Q_ASSERT(varId>-1);
        variable = algId>-1
                ? data->modules[modId]->impl.algorhitms[algId]->impl.locals[varId]
                : data->modules[modId]->impl.globals[varId];
    }
    else {
        variable = NULL;
    }
    if (src->function) {
        int modId = -1;
        int algId = -1;
        for (int i=0; i<srcData->modules.size(); i++) {
            struct Module * mod = srcData->modules[i];
            modId = i;
            for (int j=0; j<mod->impl.algorhitms.size(); j++) {
                if (mod->impl.algorhitms[j]==src->function) {
                    algId = j;
                    break;
                }
            }
            if (algId>-1)
                break;
        }
        Q_ASSERT(modId>-1);
        Q_ASSERT(algId>-1);
        function = data->modules[modId]->impl.algorhitms[algId];
    }
    else {
        function = NULL;
    }
    for (int i=0; i<operands.size(); i++) {
        operands[i]->updateReferences(src->operands[i], srcData, data);
    }
}

Expression::~Expression()
{
    for (int i=0; i<operands.size(); i++) {
        if (operands[i])
            delete operands[i];
    }
}

extern QString addIndent(const QString & source, int count);
QString dumpKind(const enum ExpressionType & kind) {
    if (kind==ExprVariable) {
        return "\"variable\"";
    }
    else if (kind==ExprConst) {
        return "\"constant\"";
    }
    else if (kind==ExprArrayElement) {
        return "\"element\"";
    }
    else if (kind==ExprFunctionCall) {
        return "\"function\"";
    }
    else if (kind==ExprSubexpression) {
        return "\"subexpression\"";
    }
    else {
        return "";
    }
}

QString dumpOperator(const enum ExpressionOperator op)
{
    if (op==OpSumm)
        return "+";
    else if (op==OpSubstract)
        return "-";
    else if (op==OpMultiply)
        return "*";
    else if (op==OpDivision)
        return "/";
    else if (op==OpPower)
        return "**";
    else if (op==OpNot)
        return "!";
    else if (op==OpAnd)
        return "&&";
    else if (op==OpOr)
        return "||";
    else if (op==OpEqual)
        return "==";
    else if (op==OpNotEqual)
        return "!=";
    else if (op==OpLess)
        return "<";
    else if (op==OpGreater)
        return ">";
    else if (op==OpLessOrEqual)
        return "<=";
    else if (op==OpGreaterOrEqual)
        return ">=";
    else
        return "";
}

QString Expression::dump() const
{
    QString result = "{\n";
    result += "\tkind: "+dumpKind(kind);
    result += "\ttype: "+AST::dump(baseType);
    if (kind==ExprConst) {
        result += "\tconstant: ";
        if (constant.type()==QVariant::String || constant.type()==QVariant::Char)
            result += "\""+constant.toString()+"\"";
        else
            result += constant.toString();
        result += ";\n";
    }
    else if (kind==ExprVariable) {
        result +=" \tvariable: \""+variable->name+"\";\n";
    }
    else if (kind==ExprFunctionCall) {
        result += "\tfunction: \""+function->header.name+"\",\n";
    }
    else if (kind==ExprArrayElement) {
        result += "t\element: \""+variable->name+"\",\n";
    }
    else if (kind==ExprSubexpression) {
        result += "\toperator: \""+dumpOperator(operatorr)+"\",\n";
    }
    if (kind==ExprFunctionCall || kind==ExprArrayElement || kind==ExprSubexpression) {
        result += "\toperands: [\n";
        for (int i=0; i<operands.size(); i++) {
            result += addIndent(operands[i]->dump(), 2);
            if (i<operands.size()-1) {
                result += ",";
            }
            result += "\n";
        }
        result += "\t]\n";
    }
    result += "}"; // end expression
    return result;
}

}

