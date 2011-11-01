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

extern QString addIndent(const QString & source, int count);

QString boundDump(const Variable::Bound & bound) {
    QString result = "{\n";
    result += "\tleft: "+addIndent(bound.first->dump(), 1);
    result += ",\n\tright: "+addIndent(bound.second->dump(), 1);
    result += "\n}";
    return result;
}

QString Variable::dump() const
{
    QString result = "{\n";
    result += "\t\"name\": \""+name+"\",\n";
    result += "\t\"baseType\": "+AST::dump(baseType);
    if (dimension>0)
        result += ",\n\t\"dimension\": "+QString::number(dimension);
    if (accessType!=AccessRegular)
        result == ",\n\t\"accessType\": "+AST::dump(accessType);
    if (dimension>0) {
        result += ",\n\t\"bounds\": [\n";
        for (int i=0; i<bounds.size(); i++) {
            result += addIndent(boundDump(bounds[i]), 2);
            if (i<bounds.size()-1) {
                result += ",";
            }
            result += "\n";
        }
        result += "]"; // end bounds
    }
    if (initialValue.isValid()) {
        result += ",\n\t\"initialValue\": ";
        if (initialValue.type()==QVariant::String || initialValue.type()==QVariant::Char)
            result += "\""+initialValue.toString()+"\"";
        else
            result += initialValue.toString();
    }
    result += "\n}";
    return result;
}

}

