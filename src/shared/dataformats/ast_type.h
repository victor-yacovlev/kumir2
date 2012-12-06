#ifndef AST_MODULE_TYPE_H
#define AST_MODULE_TYPE_H

#include <QString>
#include <QPair>
#include "ast_variabletype.h"

#undef ABSTRACTSYNTAXTREE_EXPORT
#ifdef DATAFORMATS_LIBRARY
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_EXPORT
#else
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_IMPORT
#endif

namespace AST {

    typedef QPair<QString,struct Type>  Field;

    struct Type {
        /** Base type */
        VariableBaseType kind;

        /** Type name (if user defined)*/
        QString name;

        /** User type fields (if user defined)*/
        QList< Field > userTypeFields;

        bool operator==(const Type & other) const;
        bool operator!=(const Type & other) const;
        bool operator==(const VariableBaseType & otherType) const;
        inline Type() { kind = TypeNone; }
        inline Type(AST::VariableBaseType bt) { kind = bt; }
    };

}

#endif
