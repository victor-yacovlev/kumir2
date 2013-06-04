#include "ast.h"
#include "ast_type.h"
#include "ast_module.h"
#include "ast_algorhitm.h"
#include "lexem.h"

#include <QtCore>

namespace AST {

Module::Module()
{
    builtInID = 0;
    header.type = ModTypeUser;
    header.enabled = false;
}

Module::Module(const ModulePtr src)
{
    header.enabled = src->header.enabled;
    for (int i=0; i<src->impl.globals.size(); i++) {
        impl.globals << src->impl.globals[i];
    }

    for (int i=0; i<src->impl.algorhitms.size(); i++) {
        impl.algorhitms << src->impl.algorhitms[i];
    }

    for (int i=0; i<src->impl.initializerBody.size(); i++) {
        impl.initializerBody << src->impl.initializerBody[i];
    }

    header.type = src->header.type;
    header.name = src->header.name;
    header.uses = src->header.uses;
    header.types = src->header.types;

}

void Module::updateReferences(const Module * src, const Data* srcData, const Data* data)
{
    for (int i=0; i<impl.globals.size(); i++) {
        impl.globals[i]->updateReferences(src->impl.globals[i].data(), srcData, data);
    }
    for (int i=0; i<impl.algorhitms.size(); i++) {
        impl.algorhitms[i]->updateReferences(src->impl.algorhitms[i].data(), srcData, data);
    }
    for (int i=0; i<impl.initializerBody.size(); i++) {
        impl.initializerBody[i]->updateReferences(src->impl.initializerBody[i].data(), srcData, data);
    }
    if (header.type==ModTypeExternal) {
        header.algorhitms = src->header.algorhitms;
    }
    else {
        for (int i=0; i<src->header.algorhitms.size(); i++) {
            int index = -1;
            for (int j=0; j<src->impl.algorhitms.size(); j++) {
                if (src->impl.algorhitms[j]==src->header.algorhitms[i]) {
                    index = j;
                    break;
                }
            }
            Q_ASSERT(index != -1);
            header.algorhitms << impl.algorhitms[index];
        }
    }
}

}
