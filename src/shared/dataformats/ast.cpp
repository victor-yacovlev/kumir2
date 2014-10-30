#include "ast.h"
#include "ast_module.h"
#include "ast_algorhitm.h"
#include "ast_variable.h"

namespace AST {

Data::Data()
{

}

Data::Data(const DataPtr src)
{
    for (int i=0; i<src->modules.size(); i++) {
        modules << src->modules[i];
    }
    for (int i=0; i<src->modules.size(); i++) {
        modules[i]->updateReferences(src->modules[i].data(), src.data(), this);
    }
}


}
