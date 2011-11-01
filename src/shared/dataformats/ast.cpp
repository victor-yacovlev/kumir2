#include "ast.h"
#include "ast_module.h"

namespace AST {

Data::Data()
{

}

Data::Data(const Data *src)
{
    for (int i=0; i<src->modules.size(); i++) {
        modules << new Module(src->modules[i]);
    }
    for (int i=0; i<src->modules.size(); i++) {
        modules[i]->updateReferences(src->modules[i], src, this);
    }
}

Data::~Data()
{
    for (int i=0; i<modules.size(); i++) {
        if (modules[i]) {
            delete modules[i];
        }
    }
}

extern QString addIndent(const QString & source, int count);

QString Data::dump() const
{
    QString result = "[\n";
    for (int i=0; i<modules.size(); i++) {
        if (modules[i]->header.type==ModTypeExternal)
            continue;
        result += addIndent(modules[i]->dump(), 1);
        if (i<modules.size()-1) {
            result += ",";
        }
        result += "\n";
    }
    result += "]";
    return result;
}


}
