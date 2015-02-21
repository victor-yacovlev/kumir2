#ifndef AST_H
#define AST_H

#include "ast_module.h"

#include <QList>
#include <QDateTime>
#include <QSharedPointer>
#include <QWeakPointer>

#undef ABSTRACTSYNTAXTREE_EXPORT
#ifdef DATAFORMATS_LIBRARY
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_EXPORT
#else
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_IMPORT
#endif

namespace AST {

typedef QSharedPointer<struct Data> DataPtr;

struct ABSTRACTSYNTAXTREE_EXPORT Data
{
    QList<ModulePtr> modules;
    QDateTime lastModified;

    explicit Data();
    explicit Data(const DataPtr src);


    AST::ModulePtr findModuleByType(const ModuleType moduleType) const;
};

}

#endif // AST_H
