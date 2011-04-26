#include "ast.h"
#include "ast_statement.h"
#include "ast_variable.h"
#include "ast_algorhitm.h"

namespace AST {

Statement::Statement()
{
    type = StError;
    error = 0;
    lineNo = -1;
    loop.type = LoopWhile;
    loop.forVariable = NULL;
    loop.toValue = loop.fromValue = loop.whileCondition = loop.timesValue = loop.endCondition = 0;
    loop.endLineNo = -1;
}

Statement::Statement(const Statement *src)
{
    type = src->type;
    error = src->error;
    lineNo = src->lineNo;
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
    loop.endLineNo = src->loop.endLineNo;

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
        c.startLineNo = src->conditionals[i].startLineNo;
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


}
