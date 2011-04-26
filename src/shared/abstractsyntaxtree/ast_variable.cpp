#include "ast_variable.h"

namespace AST {

Variable::Variable()
{
    baseType = TypeNone;
    accessType = AccessRegular;
    dimension = 0;
}

Variable::Variable(const struct Variable *src)
{
    baseType = src->baseType;
    accessType = src->accessType;
    dimension = src->dimension;
    name = src->name;
    initialValue = src->initialValue;
    for (int i=0; i<src->bounds.size(); i++)
        bounds << Bound(NULL, NULL);
}

void Variable::updateReferences(const Variable *src, const Data *srcData, const Data *data)
{
    for (int i=0; i<src->bounds.size(); i++) {
        const struct Expression * left = src->bounds[i].first;
        const struct Expression * right = src->bounds[i].second;
        struct Expression * newLeft = new Expression(left);
        newLeft->updateReferences(left, srcData, data);
        struct Expression * newRight = new Expression(right);
        newRight->updateReferences(right, srcData, data);
        bounds[i] = Bound(newLeft, newRight);
    }
}

Variable::~Variable()
{
    for (int i=0; i<bounds.size(); i++) {
        delete bounds[i].first;
        delete bounds[i].second;
    }
}


}

