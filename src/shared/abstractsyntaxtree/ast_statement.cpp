#include "ast.h"
#include "ast_statement.h"
#include "ast_variable.h"
#include "ast_algorhitm.h"

namespace AST {

Statement::Statement()
{
    type = StError;
    error = "";
    loop.type = LoopWhile;
    loop.forVariable = NULL;
    loop.toValue = loop.fromValue = loop.whileCondition = loop.timesValue = loop.endCondition = loop.stepValue = 0;
}

Statement::~Statement()
{
    for (int i=0; i<expressions.size(); i++) {
        if (expressions[i])
            delete expressions[i];
    }
//    for (int i=0; i<variables.size(); i++) {
//        if (variables[i])
//            delete variables[i];
//    }
    for (int i=0; i<conditionals.size(); i++) {
        if (conditionals[i].condition)
            delete conditionals[i].condition;
        for (int j=0; j<conditionals[i].body.size(); j++)
            if (conditionals[i].body[j])
                delete conditionals[i].body[j];
    }
    if (loop.forVariable)
        delete loop.forVariable;
    if (loop.fromValue)
        delete loop.fromValue;
    if (loop.toValue)
        delete loop.toValue;
    if (loop.stepValue)
        delete loop.stepValue;
    if (loop.whileCondition)
        delete loop.whileCondition;
    if (loop.timesValue)
        delete loop.timesValue;
    if (loop.endCondition)
        delete loop.endCondition;
    for (int i=0; i<loop.body.size(); i++) {
        delete loop.body[i];
    }
}

Statement::Statement(const Statement *src)
{
    type = src->type;
    error = src->error;
    loop.type = src->loop.type;
    if (src->loop.toValue)
        loop.toValue = new Expression(src->loop.toValue);
    else
        loop.toValue = 0;
    if (src->loop.fromValue)
        loop.fromValue = new Expression(src->loop.fromValue);
    else
        loop.fromValue = 0;
    if (src->loop.stepValue)
        loop.stepValue = new Expression(src->loop.stepValue);
    else
        loop.stepValue = 0;
    if (src->loop.timesValue)
        loop.timesValue = new Expression(src->loop.timesValue);
    else
        loop.timesValue = 0;
    if (src->loop.endCondition)
        loop.endCondition = new Expression(src->loop.endCondition);
    else
        loop.endCondition = 0;

    for (int i=0; i<src->loop.body.size(); i++) {
        loop.body << new Statement(src->loop.body[i]);
    }

    for (int i=0; i<src->expressions.size(); i++) {
        expressions << new Expression(src->expressions[i]);
    }

    for (int i=0; i<src->conditionals.size(); i++) {
        struct ConditionSpec c;
        if (src->conditionals[i].condition)
            c.condition = new Expression(src->conditionals[i].condition);
        else
            c.condition = 0;
        for (int j=0; j<src->conditionals[i].body.size(); i++) {
            c.body << new Statement(src->conditionals[i].body[j]);
        }
        conditionals << c;
    }
}

void Statement::updateReferences(const Statement *src, const Data *srcData, const Data *data)
{
    if (loop.forVariable)
        loop.forVariable->updateReferences(src->loop.forVariable, srcData, data);
    if (loop.fromValue)
        loop.fromValue->updateReferences(src->loop.fromValue, srcData, data);
    if (loop.toValue)
        loop.toValue->updateReferences(src->loop.toValue, srcData, data);
    if (loop.stepValue)
        loop.stepValue->updateReferences(src->loop.stepValue, srcData, data);
    if (loop.timesValue)
        loop.timesValue->updateReferences(src->loop.timesValue, srcData, data);
    if (loop.endCondition)
        loop.endCondition->updateReferences(src->loop.endCondition, srcData, data);
    for (int i=0; i<loop.body.size(); i++) {
        loop.body[i]->updateReferences(src->loop.body[i], srcData, data);
    }
    for (int i=0; i<expressions.size(); i++) {
        expressions[i]->updateReferences(src->expressions[i], srcData, data);
    }
    for (int i=0; i<conditionals.size(); i++) {
        if (conditionals[i].condition)
            conditionals[i].condition->updateReferences(src->conditionals[i].condition, srcData, data);
        for (int j=0; j<conditionals[i].body.size(); j++) {
            conditionals[i].body[j]->updateReferences(src->conditionals[i].body[j], srcData, data);
        }
    }
    for (int i=0; i<src->variables.size(); i++) {
        int modId = -1;
        int algId = -1;
        int varId = -1;
        for (int a=0; a<srcData->modules.size(); a++) {
            struct Module * mod = srcData->modules[a];
            modId = a;
            algId = -1;
            for (int b=0; b<mod->impl.globals.size(); b++) {
                if (mod->impl.globals[b]==src->variables[i]) {
                    varId = b;
                    break;
                }
            }
            if (varId>-1)
                break;
            for (int b=0; b<mod->impl.algorhitms.size(); b++) {
                struct Algorhitm * alg = mod->impl.algorhitms[b];
                algId = b;
                for (int c=0; c<alg->impl.locals.size(); c++) {
                    if (alg->impl.locals[c]==src->variables[i]) {
                        varId = c;
                        break;
                    }
                }
                if (varId>-1) {
                    break;
                }
            }
            if (varId>-1)
                break;
        }
        Q_ASSERT(modId>-1);
        Q_ASSERT(varId>-1);
        variables << ( algId>-1
                ? data->modules[modId]->impl.algorhitms[algId]->impl.locals[varId]
                : data->modules[modId]->impl.globals[varId] );
    }
}

extern QString addIndent(const QString & source, int count);
extern QString dumpLexem(const struct Lexem *lx);

QString dumpStatementType(const enum StatementType t)
{
    if (t==StAssign)
        return "assignment";
    else if (t==StAssert)
        return "assertion";
    else if (t==StVarInitialize)
        return "variableInitialization";
    else if (t==StInput)
        return "input";
    else if (t==StOutput)
        return "output";
    else if (t==StFileInput)
        return "fileInput";
    else if (t==StFileOutput)
        return "fileOutput";
    else if (t==StLoop)
        return "loop";
    else if (t==StIfThenElse)
        return "ifThenElse";
    else if (t==StSwitchCaseElse)
        return "switchCaseElse";
    else if (t==StBreak)
        return "break";
    else
        return "error";
}

QString dumpLoopSpec(const struct LoopSpec & spec)
{
    QString result = "{\n";
    if (spec.type==LoopFor)
        result += "\ttype: \"for\",\n";
    else if (spec.type==LoopTimes)
        result += "\ttype: \"times\",\n";
    else
        result += "\ttype: \"while\",\n";
    if (spec.type==LoopFor){
        if (spec.forVariable)
            result += "\tforVariable: \""+spec.forVariable->name+"\",\n";
        if (spec.fromValue)
            result += "\tfromValue: "+addIndent(spec.fromValue->dump(), 1)+",\n";
        if (spec.toValue)
            result += "\ttoValue: "+addIndent(spec.toValue->dump(), 1)+",\n";
        if (spec.stepValue)
            result += "\tstepValue: "+addIndent(spec.stepValue->dump(), 1)+",\n";
    }
    else if (spec.type==LoopTimes)
        result += "\ttimesValue: "+addIndent(spec.timesValue->dump(), 1)+",\n";
    else if (spec.type==LoopWhile && spec.whileCondition)
        result += "\twhileCondition: "+addIndent(spec.whileCondition->dump(), 1)+",\n";
    if (spec.endCondition)
        result += "\tendCondition: "+addIndent(spec.endCondition->dump(), 1)+",\n";
    result += "\tbody: [\n";
    for (int i=0; i<spec.body.size(); i++) {
        result += addIndent(spec.body[i]->dump(), 1);
        if (i<spec.body.size()-1)
            result += ",";
        result += "\n";
    }
    result += "\t]";
    if (!spec.endLexems.isEmpty()) {
        result += ",\n";
        result += "\tendLexems: [\n";
        for (int i=0; i<spec.endLexems.size(); i++) {
            result += "\t\t"+dumpLexem(spec.endLexems[i]);
            if (i<spec.endLexems.size()-1)
                result += ",";
            result += "\n";
        }
        result += "\t]\n";
    }
    else {
        result += "\n";
    }
    result += "}";
    return result;
}

QString dumpConditionSpec(const struct ConditionSpec & spec)
{
    QString result = "{\n";
    if (!spec.lexems.isEmpty()) {
        result += "\tlexems: [\n";
        for (int i=0; i<spec.lexems.size(); i++) {
            result += "\t\t"+dumpLexem(spec.lexems[i]);
            if (i<spec.lexems.size()-1)
                result += ",";
            result += "\n";
        }
        result += "\t],\n";
    }
    if (spec.condition) {
        result += "\tcondition: "+addIndent(spec.condition->dump(), 1)+",\n";
    }
    result += "\tbody: [\n";
    for (int i=0; i<spec.body.size(); i++) {
        result += addIndent(spec.body[i]->dump(), 1);
        if (i<spec.body.size()-1)
            result += ",";
        result += "\n";
    }
    result += "\t] /* end conditional body */ \n";
    result += "} /* end conditional element */ ";
    return result;
}



QString Statement::dump() const
{
    QString result = "{\n";
    if (!lexems.isEmpty()) {
        result += "\tlexems: [\n";
        for (int i=0; i<lexems.size(); i++) {
            result += "\t\t"+dumpLexem(lexems[i]);
            if (i<lexems.size()-1)
                result += ",";
            result += "\n";
        }
        result += "\t],\n";
    }
    result += "\ttype: \""+dumpStatementType(type)+"\"";
    if (type==StError) {
        result += ",\n\terror: \""+error+"\"\n";
    }
    else if (type==StLoop) {
        result += ",\n\tloop: "+addIndent(dumpLoopSpec(loop),1)+"\n";
    }
    else if (type==StIfThenElse || type==StSwitchCaseElse) {
        result += ",\n\tconditionals: [\n";
        for (int i=0; i<conditionals.size(); i++) {
            result += addIndent(dumpConditionSpec(conditionals[i]), 2);
            if (i<conditionals.size()-1)
                result += ",";
            result += "\n";
        }
        result += "\t]\n";
    }
    else if (type==StVarInitialize) {
        result += ",\n\tvariables: [ ";
        for (int i=0; i<variables.size(); i++) {
            result += "\""+variables[i]->name+"\"";
            if (i<variables.size()-1)
                result += ", ";
        }
        result += " ]\n";
    }
    else if (type==StBreak) {
        result += "\n";
    }
    else {
        result += ",\n\texpressions: [\n";
        for (int i=0; i<expressions.size(); i++) {
            result += addIndent(expressions[i]->dump(), 2);
            if (i<expressions.size()-1)
                result += ",";
            result += "\n";
        }
        result += "\t]\n";
    }
    result += "} /* end statement of type '"+dumpStatementType(type)+"' */";
    return result;
}

}
