#include "ast_algorhitm.h"

#include <QtCore>

namespace AST {

Algorhitm::Algorhitm()
{
    header.specialType = AlgorhitmTypeRegular;
    header.returnType = TypeNone;
    header.implType = AlgorhitmCompiled;
}

Algorhitm::Algorhitm(const Algorhitm *src)
{
    header.specialType = src->header.specialType;
    header.returnType = src->header.returnType;
    header.implType = src->header.implType;
    header.name = src->header.name;
    header.error = src->header.error;

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
        return "\"standart\"";
    }
}

extern QString dumpLexem(const struct Lexem *lx);

QString Algorhitm::dump() const
{
    QString result = "{\n";
    result += "\t\"header\": {\n";
    result += "\t\t\"name\": \""+header.name+"\",\n";
    result += "\t\t\"returnType\": "+AST::dump(header.returnType)+",\n";
    result += "\t\t\"specialType\": "+AST::dump(header.specialType);
    if (!header.arguments.isEmpty()) {
    result += ",\n\t\t\"arguments\": [ ";
        for (int i=0; i<header.arguments.size(); i++) {
            if (i>0) result += ", ";
            result += "\""+header.arguments[i]->name+"\"";
        }
        result += " ]"; // end arguments
    }
    if (!header.error.isEmpty()) {
        result += ",\n\t\t\"error\": \""+header.error+"\"";
    }
    result += "\n";
    result += "\t},\n";
    result += "\t\"implementation\": {\n";
//    if (!impl.headerLexems.isEmpty()) {
//        result += "\t\t\"headerLexems\": [\n";
//        for (int i=0; i<impl.headerLexems.size(); i++) {
//            result += "\t\t\t"+dumpLexem(impl.headerLexems[i]);
//            if (i<impl.headerLexems.size()-1)
//                result += ",";
//            result += "\n";
//        }
//        result += "\t\t],\n";
//    }
//    if (!impl.beginLexems.isEmpty()) {
//        result += "\t\t\"beginLexems\": [\n";
//        for (int i=0; i<impl.beginLexems.size(); i++) {
//            result += "\t\t\t"+dumpLexem(impl.beginLexems[i]);
//            if (i<impl.beginLexems.size()-1)
//                result += ",";
//            result += "\n";
//        }
//        result += "\t\t],\n";
//    }
    result += "\t\t\"locals\": [\n";
    for (int i=0; i<impl.locals.size(); i++) {
        result += AST::addIndent(impl.locals[i]->dump(), 3);
        if (i<impl.locals.size()-1) {
            result += ",";
        }
        result += "\n";
    }
    result += "\t\t] /* end algorhitm '"+header.name+"' locals */,\n";
    if (!impl.pre.isEmpty()) {
        result += "\t\tpre: [\n";
        for (int i=0; i<impl.pre.size(); i++) {
            result += AST::addIndent(impl.pre[i]->dump(), 3);
            if (i<impl.pre.size()-1) {
                result += ",";
            }
            result += "\n";
        }
        result += "\t\t] /* end algorhitm '"+header.name+"' pre-statements */,\n";
    }
    result += "\t\t\"body\": [\n";
    for (int i=0; i<impl.body.size(); i++) {
        result += AST::addIndent(impl.body[i]->dump(), 3);
        if (i<impl.body.size()-1) {
            result += ",";
        }
        result += "\n";
    }
    result += "\t\t] /* end algorhitm '"+header.name+"' body */";
    if (!impl.post.isEmpty()) {
        result += ",\n\t\t\"post\": [\n";
        for (int i=0; i<impl.post.size(); i++) {
            result += AST::addIndent(impl.post[i]->dump(), 3);
            if (i<impl.post.size()-1) {
                result += ",";
            }
            result += "\n";
        }
        result += "\t\t] /* end algorhitm '"+header.name+"' post-statements */";
    }
//    if (!impl.endLexems.isEmpty()) {
//        result += ",\n\t\t\"endLexems\": [\n";
//        for (int i=0; i<impl.endLexems.size(); i++) {
//            result += "\t\t\t"+dumpLexem(impl.endLexems[i]);
//            if (i<impl.endLexems.size()-1)
//                result += ",";
//            result += "\n";
//        }
//        result += "\t\t]";
//    }
    result += "\n\t} /* end algorhitm '"+header.name+"' implementation */\n";
    result += "} /* end algorhitm '"+header.name+"' */\n";
    return result;
}

}
