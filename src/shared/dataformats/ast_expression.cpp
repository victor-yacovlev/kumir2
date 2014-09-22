#include "ast_expression.h"
#include "ast_module.h"
#include "ast_algorhitm.h"
#include "ast.h"

namespace AST {

Expression::Expression()
    : kind(ExprNone)
    , baseType(TypeNone)
    , expressionIsClosed(false)
    , isStringPart(false)
    , operatorr(AST::OpNone)
    , dimension(0)
    , keepInCache(false)
    , useFromCache(false)
    , clearCacheOnFailure(false)
{    
}

Expression::Expression(const ExpressionPtr src)
    : kind(src->kind)
    , baseType(src->baseType)
    , constant(src->constant)
    , variable(src->variable)
    , function(src->function)
    , expressionIsClosed(src->expressionIsClosed)
    , isStringPart(src->isStringPart)
    , operatorr(src->operatorr)
    , dimension(src->dimension)
    , keepInCache(src->keepInCache)
    , useFromCache(src->useFromCache)
    , clearCacheOnFailure(src->clearCacheOnFailure)
    , cacheReference(src->cacheReference)
{
    for (int i=0; i<src->operands.size(); i++) {
        operands << src->operands[i];
    }
}

void Expression::updateReferences(const Expression * src, const struct Data * srcData, const struct Data *data)
{
    if (src->variable) {
        int modId = -1;
        int varId = -1;
        int algId = -1;
        for (int i=0; i<srcData->modules.size(); i++) {
            modId = i;
            algId = -1;
            varId = -1;
            ModulePtr mod = srcData->modules[i];
            for (int j=0; j<mod->impl.globals.size(); j++) {
                if (mod->impl.globals[j]==src->variable) {
                    varId = j;
                    break;
                }
            }
            if (varId!=-1)
                break;
            for (int j=0; j<mod->impl.algorhitms.size(); j++) {
                AlgorithmPtr alg = mod->impl.algorhitms[j];
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
        variable.clear();
    }
    if (src->function) {
        int modId = -1;
        int algId = -1;
        for (int i=0; i<srcData->modules.size(); i++) {
            ModulePtr mod = srcData->modules[i];
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
        function.clear();
    }
    for (int i=0; i<operands.size(); i++) {
        operands[i]->updateReferences(src->operands[i].data(), srcData, data);
    }
}

}

