#ifndef AST_MODULE_TYPE_H
#define AST_MODULE_TYPE_H

#include <QString>
#include "ast_variabletype.h"

#undef ABSTRACTSYNTAXTREE_EXPORT
#ifdef DATAFORMATS_LIBRARY
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_EXPORT
#else
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_IMPORT
#endif

namespace AST {
    struct Type {
        /** Base type */
        VariableBaseType kind;

        /** Type name (if user defined)*/
        QString name;

        /** Data size (if user defined)*/
        size_t size;
        bool operator==(const Type & other) const;
        bool operator!=(const Type & other) const;
        bool operator==(const VariableBaseType & otherType) const;
        inline Type() { kind = TypeNone; size = 0; }
        inline Type(AST::VariableBaseType bt) { kind = bt; size = 0; }
    };
}

#endif
