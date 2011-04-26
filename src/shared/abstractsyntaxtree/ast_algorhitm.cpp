#include "ast_algorhitm.h"

namespace AST {

Algorhitm::Algorhitm()
{
    header.specialType = AlgorhitmTypeRegular;
    header.returnType = TypeNone;
    header.implType = AlgorhitmCompiled;
    impl.lineNoStart = impl.lineNoEnd = -1;
}

Algorhitm::Algorhitm(const Algorhitm *src)
{
    header.specialType = src->header.specialType;
    header.returnType = src->header.returnType;
    header.implType = src->header.implType;
    header.name = src->header.name;
    impl.lineNoStart = src->impl.lineNoStart;
    impl.lineNoEnd = src->impl.lineNoEnd;

    for (int i=0; i<src->impl.locals.size(); i++) {
        impl.locals << new Variable(src->impl.locals[i]);
    }
    for (int i=0; i<src->impl.pre.size(); i++) {
        impl.pre << new Statement(src->impl.pre[i]);
    }
    for (int i=0; i<src->impl.post.size(); i++) {
        impl.post << new Statement(src->impl.post[i]);
    }
    for (int i=0; i<src->impl.body.size(); i++) {
        impl.body << new Statement(src->impl.body[i]);
    }
}

void Algorhitm::updateReferences(const Algorhitm *src, const Data *srcData, const Data *data)
{
    for (int i=0; i<impl.locals.size(); i++) {
        impl.locals[i]->updateReferences(src->impl.locals[i], srcData, data);
    }
    for (int i=0; i<impl.pre.size(); i++) {
        impl.pre[i]->updateReferences(src->impl.pre[i], srcData, data);
    }
    for (int i=0; i<impl.post.size(); i++) {
        impl.post[i]->updateReferences(src->impl.post[i], srcData, data);
    }
    for (int i=0; i<impl.body.size(); i++) {
        impl.body[i]->updateReferences(src->impl.body[i], srcData, data);
    }

    for (int i=0; i<src->header.arguments.size(); i++) {
        struct Variable * arg = NULL;
        for (int j=0; j<src->impl.locals.size(); j++) {
            if (src->impl.locals[j]==src->header.arguments[i]) {
                arg = impl.locals[j];
                break;
            }
        }
        Q_CHECK_PTR(arg);
        header.arguments << arg;
    }
}

Algorhitm::~Algorhitm()
{
    for (int i=0; i<impl.locals.size(); i++) {
        if (impl.locals[i])
            delete impl.locals[i];
    }
    for (int i=0; i<impl.pre.size(); i++) {
        if (impl.pre[i])
            delete impl.pre[i];
    }
    for (int i=0; i<impl.post.size(); i++) {
        if (impl.post[i])
            delete impl.post[i];
    }
    for (int i=0; i<impl.body.size(); i++) {
        if (impl.body[i])
            delete impl.body[i];
    }
}

}
