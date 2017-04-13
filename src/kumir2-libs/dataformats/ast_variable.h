#ifndef AST_VARIABLE_H
#define AST_VARIABLE_H

#include "ast_variabletype.h"
#include "ast_expression.h"
#include "ast_type.h"
#include "lexem.h"

#include <QString>
#include <QList>
#include <QVariant>
#include <QPair>
#include <QSharedPointer>
#include <QWeakPointer>

#undef ABSTRACTSYNTAXTREE_EXPORT
#ifdef DATAFORMATS_LIBRARY
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_EXPORT
#else
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_IMPORT
#endif

namespace AST {

typedef QSharedPointer<struct Variable> VariablePtr;

typedef QSharedPointer<struct Data> DataPtr;

/** Variable element of (locals|globals) table */
struct ABSTRACTSYNTAXTREE_EXPORT Variable {

    /** One array dimension is calculable by two expressions */
    typedef QPair<ExpressionPtr, ExpressionPtr> Bound;

    /** Variable name, may be empty (for constant) */
    QString name;

    /** Variable base type */
    struct Type baseType;

    /** Variable access type */
    enum VariableAccessType accessType;

    /** Array dimension; set to 0 for non-array variables */
    quint8 dimension;

    /** List of array bounds, empty for non-array variables */
    QList<Bound> bounds;

    /** Constant or pre-initialized initial value */
    QVariant initialValue;

    explicit Variable();
    explicit Variable(const VariablePtr src);
    void updateReferences(const struct Variable * src,
                          const struct Data * srcData,
                          const struct Data * data);

};



}


#endif // AST_VARIABLE_H
