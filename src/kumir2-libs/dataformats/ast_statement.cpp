#include "ast.h"
#include "ast_statement.h"
#include "ast_variable.h"
#include "ast_algorhitm.h"

namespace AST {

Statement::Statement()
    : type(StError)
    , skipErrorEvaluation(false)
{
    error = "";
    loop.type = LoopWhile;
}


Statement::Statement(const StatementPtr src)
{
    type = src->type;
    error = src->error;
    loop.type = src->loop.type;
    parent = src->parent;
    if (src->loop.toValue)
        loop.toValue = src->loop.toValue;
    if (src->loop.fromValue)
        loop.fromValue = src->loop.fromValue;
    if (src->loop.stepValue)
        loop.stepValue = src->loop.stepValue;

    if (src->loop.timesValue)
        loop.timesValue = src->loop.timesValue;

    if (src->loop.endCondition)
        loop.endCondition = src->loop.endCondition;


    for (int i=0; i<src->loop.body.size(); i++) {
        loop.body << src->loop.body[i];
    }

    for (int i=0; i<src->expressions.size(); i++) {
        expressions << src->expressions[i];
    }

    for (int i=0; i<src->conditionals.size(); i++) {
        struct ConditionSpec c;
        if (src->conditionals[i].condition)
            c.condition = src->conditionals[i].condition;

        for (int j=0; j<src->conditionals[i].body.size(); i++) {
            c.body << src->conditionals[i].body[j];
        }
        conditionals << c;
    }
}

void Statement::updateReferences(const Statement * src, const struct Data * srcData, const struct Data *data)
{
    if (loop.forVariable)
        loop.forVariable.data()->updateReferences(src->loop.forVariable.data(), srcData, data);
    if (loop.fromValue)
        loop.fromValue->updateReferences(src->loop.fromValue.data(), srcData, data);
    if (loop.toValue)
        loop.toValue->updateReferences(src->loop.toValue.data(), srcData, data);
    if (loop.stepValue)
        loop.stepValue->updateReferences(src->loop.stepValue.data(), srcData, data);
    if (loop.timesValue)
        loop.timesValue->updateReferences(src->loop.timesValue.data(), srcData, data);
    if (loop.endCondition)
        loop.endCondition->updateReferences(src->loop.endCondition.data(), srcData, data);
    for (int i=0; i<loop.body.size(); i++) {
        loop.body[i]->updateReferences(src->loop.body[i].data(), srcData, data);
    }
    for (int i=0; i<expressions.size(); i++) {
        expressions[i]->updateReferences(src->expressions[i].data(), srcData, data);
    }
    for (int i=0; i<conditionals.size(); i++) {
        if (conditionals[i].condition)
            conditionals[i].condition->updateReferences(src->conditionals[i].condition.data(), srcData, data);
        for (int j=0; j<conditionals[i].body.size(); j++) {
            conditionals[i].body[j]->updateReferences(src->conditionals[i].body[j].data(), srcData, data);
        }
    }
    for (int i=0; i<src->variables.size(); i++) {
        int modId = -1;
        int algId = -1;
        int varId = -1;
        for (int a=0; a<srcData->modules.size(); a++) {
            ModulePtr mod = srcData->modules[a];
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
                AlgorithmPtr alg = mod->impl.algorhitms[b];
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
