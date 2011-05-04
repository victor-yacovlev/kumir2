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


}
