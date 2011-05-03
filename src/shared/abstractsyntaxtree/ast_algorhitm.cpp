#include "ast_algorhitm.h"

#include <QtCore>

namespace AST {

Algorhitm::Algorhitm()
{
    header.specialType = AlgorhitmTypeRegular;
    header.returnType = TypeNone;
    header.implType = AlgorhitmCompiled;
    impl.lineNoStart = impl.lineNoEnd = -1;
    header.broken = false;
}

Algorhitm::Algorhitm(const Algorhitm *src)
{
    header.specialType = src->header.specialType;
    header.returnType = src->header.returnType;
    header.implType = src->header.implType;
    header.name = src->header.name;
    header.broken = src->header.broken;
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

extern QString addIndent(const QString & source, int count);

QString dump(const enum AlgorhitmType & type)
{
    if (type==AlgorhitmTypeTeacher) {
        return "\"teacher\"";
    }
    else if (type==AlgorhitmTypeTesting) {
        return "\"testing\"";
    }
    else {
        return "standart";
    }
}

QString Algorhitm::dump() const
{
    QString result = "{\n";
    result += "\theader: {\n";
    result += "\t\tname: \""+header.name+"\",\n";
    result += "\t\treturnType: "+AST::dump(header.returnType)+",\n";
    result += "\t\tspecialType: "+AST::dump(header.specialType)+",\n";
    result += "\t\targuments: [";
    for (int i=0; i<header.arguments.size(); i++) {
        if (i>0) result += ", ";
        result += header.arguments[i]->name;
    }
    result += "],\n"; // end arguments
    result += QString("\t\tbroken: ")+(header.broken? "true" : "false")+"\n";
    result += "},\n"; // end header
    result += "\timplementation: {\n";
    result += "\t\tlocals: [\n";
    for (int i=0; i<impl.locals.size(); i++) {
        result += AST::addIndent(impl.locals[i]->dump(), 3);
        if (i<impl.locals.size()-1) {
            result += ",";
        }
        result += "\n";
    }
    result += "\t\t],\n"; // end locals
    result += "\t\tpre: [\n";
    for (int i=0; i<impl.pre.size(); i++) {
        result += AST::addIndent(impl.pre[i]->dump(), 3);
        if (i<impl.pre.size()-1) {
            result += ",";
        }
        result += "\n";
    }
    result += "\t\t],\n"; // end body
    result += "\t\tbody: [\n";
    for (int i=0; i<impl.body.size(); i++) {
        result += AST::addIndent(impl.body[i]->dump(), 3);
        if (i<impl.body.size()-1) {
            result += ",";
        }
        result += "\n";
    }
    result += "\t\tpost: [\n";
    for (int i=0; i<impl.post.size(); i++) {
        result += AST::addIndent(impl.post[i]->dump(), 3);
        if (i<impl.post.size()-1) {
            result += ",";
        }
        result += "\n";
    }
    result += "\t\t],\n"; // end body
    result += "\t}\n"; // end implementation
    result += "}\n";
    return result;
}

}
