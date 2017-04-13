#include "ast_variable.h"
#include "ast_type.h"
#include "ast_algorhitm.h"

namespace AST {

typedef QSharedPointer<struct Variable> VariablePtr;
typedef QWeakPointer<struct Variable> VariableWPtr;

Variable::Variable()
{
    baseType.kind = TypeNone;
    accessType = AccessRegular;
    dimension = 0;
}

Variable::Variable(const VariablePtr src)
{
    baseType = src->baseType;
    accessType = src->accessType;
    dimension = src->dimension;
    name = src->name;
    initialValue = src->initialValue;
    for (int i=0; i<src->bounds.size(); i++)
        bounds << src->bounds[i];
}

void Variable::updateReferences(const Variable * src, const struct Data * srcData, const struct Data *data)
{
    for (int i=0; i<src->bounds.size(); i++) {
        bounds[i].first->updateReferences(src->bounds[i].first.data(), srcData, data);
        bounds[i].second->updateReferences(src->bounds[i].second.data(), srcData, data);
    }
}

}

