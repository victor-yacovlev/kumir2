#include "ast_algorhitm.h"

#include <QtCore>

namespace AST {

Algorithm::Algorithm()
{
    header.specialType = AlgorithmTypeRegular;
    header.returnType.kind = TypeNone;
    header.implType = AlgorhitmCompiled;
    header.broken = false;
}

Algorithm::Algorithm(const AlgorithmPtr src)
{
    header.specialType = src->header.specialType;
    header.returnType = src->header.returnType;
    header.implType = src->header.implType;
    header.name = src->header.name;
    header.error = src->header.error;
    header.broken = src->header.broken;

    for (int i=0; i<src->impl.locals.size(); i++) {
        impl.locals << src->impl.locals[i];
    }
    for (int i=0; i<src->impl.pre.size(); i++) {
        impl.pre << src->impl.pre[i];
    }
    for (int i=0; i<src->impl.post.size(); i++) {
        impl.post << src->impl.post[i];
    }
    for (int i=0; i<src->impl.body.size(); i++) {
        impl.body << src->impl.body[i];
    }
}

void Algorithm::updateReferences(const Algorithm * src, const struct Data * srcData, const struct Data *data)
{
    for (int i=0; i<impl.locals.size(); i++) {
        impl.locals[i]->updateReferences(src->impl.locals[i].data(), srcData, data);
    }
    for (int i=0; i<impl.pre.size(); i++) {
        impl.pre[i]->updateReferences(src->impl.pre[i].data(), srcData, data);
    }
    for (int i=0; i<impl.post.size(); i++) {
        impl.post[i]->updateReferences(src->impl.post[i].data(), srcData, data);
    }
    for (int i=0; i<impl.body.size(); i++) {
        impl.body[i]->updateReferences(src->impl.body[i].data(), srcData, data);
    }

    for (int i=0; i<src->header.arguments.size(); i++) {
        int index = -1;
        for (int j=0; j<src->impl.locals.size(); j++) {
            if (src->impl.locals[j]==src->header.arguments[i]) {
                index = j;
                break;
            }
        }
        Q_ASSERT(index != -1);
        header.arguments << impl.locals[index];
    }
}



}
