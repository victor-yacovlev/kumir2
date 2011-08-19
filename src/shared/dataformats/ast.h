#ifndef AST_H
#define AST_H

#include <QList>
#include <QDateTime>

#include "ast_module.h"

#undef ABSTRACTSYNTAXTREE_EXPORT
#ifdef DATAFORMATS_LIBRARY
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_EXPORT
#else
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_IMPORT
#endif

namespace AST {

struct ABSTRACTSYNTAXTREE_EXPORT Data
{
    QList<struct Module*> modules;
    QDateTime lastModified;

    explicit Data();
    explicit Data(const Data * src);
    ~Data();
    QString dump() const;
};

}

#endif // AST_H
