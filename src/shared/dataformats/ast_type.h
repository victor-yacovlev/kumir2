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

        inline bool operator==(const Type & other) const {
            if (kind!=TypeUser)
                return kind == other.kind;
            else
                return name == other.name;
        }
        inline bool operator!=(const Type & other) const {
            if (kind==TypeUser)
                return name!=other.name;
            else
                return kind!=other.kind;
        }
        inline bool operator==(const VariableBaseType & otherType) const {
            return kind!=TypeUser && kind==otherType;
        }
        inline Type() { kind = TypeNone; }
        inline Type(AST::VariableBaseType bt) { kind = bt; }
        inline Type(const QString & userTypeName) { kind = AST::TypeUser; name = userTypeName; }
    };

}

#endif
