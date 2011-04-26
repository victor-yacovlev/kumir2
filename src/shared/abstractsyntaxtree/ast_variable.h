#ifndef AST_VARIABLE_H
#define AST_VARIABLE_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QPair>

#include "ast_variabletype.h"
#include "ast_expression.h"

namespace AST {

/** Variable element of (locals|globals) table */
struct Variable {

    /** One array dimension is calculable by two expressions */
    typedef QPair<struct Expression *, struct Expression *> Bound;

    /** Variable name, may be empty (for constant) */
    QString name;

    /** Variable base type */
    enum VariableBaseType baseType;

    /** Variable access type */
    enum VariableAccessType accessType;

    /** Array dimension; set to 0 for non-array variables */
    quint8 dimension;

    /** List of array bounds, empty for non-array variables */
    QList<Bound> bounds;

    /** Constant or pre-initialized initial value */
    QVariant initialValue;

    explicit Variable();
    explicit Variable(const struct Variable * src);
    void updateReferences(const struct Variable * src,
                          const struct Data * srcData,
                          const struct Data * data);
    ~Variable();
};



}


#endif // AST_VARIABLE_H
