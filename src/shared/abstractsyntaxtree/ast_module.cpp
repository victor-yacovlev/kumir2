#include "ast.h"
#include "ast_module.h"
#include "ast_algorhitm.h"

namespace AST {

Module::Module()
{
    header.type = ModTypeUser;
}

Module::Module( const struct Module * src )
{
    for (int i=0; i<src->impl.globals.size(); i++) {
        impl.globals << new Variable(src->impl.globals[i]);
    }

    for (int i=0; i<src->impl.algorhitms.size(); i++) {
        impl.algorhitms << new Algorhitm(src->impl.algorhitms[i]);
    }

    for (int i=0; i<src->impl.initializerBody.size(); i++) {
        impl.initializerBody << new Statement(src->impl.initializerBody[i]);
    }

    header.type = src->header.type;
    header.name = src->header.name;
    header.uses = src->header.uses;
}

void Module::updateReferences(const Module *src, const Data *srcData, const Data *data)
{
    for (int i=0; i<impl.globals.size(); i++) {
        impl.globals[i]->updateReferences(src->impl.globals[i], srcData, data);
    }
    for (int i=0; i<impl.algorhitms.size(); i++) {
        impl.algorhitms[i]->updateReferences(src->impl.algorhitms[i], srcData, data);
    }
    for (int i=0; i<impl.initializerBody.size(); i++) {
        impl.initializerBody[i]->updateReferences(src->impl.initializerBody[i], srcData, data);
    }
    if (header.type==ModTypeExternal) {
        header.algorhitms = src->header.algorhitms;
    }
    else {
        for (int i=0; i<src->header.algorhitms.size(); i++) {
            struct Algorhitm * alg = NULL;
            for (int j=0; j<src->impl.algorhitms.size(); j++) {
                if (src->impl.algorhitms[j]==src->header.algorhitms[i]) {
                    alg = src->impl.algorhitms[j];
                    break;
                }
            }
            Q_CHECK_PTR(alg);
            header.algorhitms << alg;
        }
    }
}

Module::~Module()
{
    for (int i=0; i<impl.algorhitms.size(); i++) {
        if (impl.algorhitms[i])
            delete impl.algorhitms[i];
    }
    for (int i=0; i<impl.globals.size(); i++) {
        if (impl.globals[i])
            delete impl.globals[i];
    }
    for (int i=0; i<impl.initializerBody.size(); i++) {
        if (impl.globals[i])
            delete impl.initializerBody[i];
    }
}

}
