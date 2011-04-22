#ifndef AST_EXPRESSION_H
#define AST_EXPRESSION_H

#include <QString>
#include <QList>

#include "ast_variabletype.h"

namespace AST {

/** Type of expression structure */
enum ExpressionType {

    /** Dummy expression, when it optional
      * (for example, "step" value in for-loop)
      */
    ExprNone,

    /** Variable of constant */
    ExprVariable,

    /** Array of string element */
    ExprArrayElement,

    /** String (in future - may be array) slice */
    ExprSlice,

    /** Function call */
    ExprFunctionCall,

    /** Subexpression */
    ExprSubexpression
};

/** Operand delimeter for subexpressions */
enum ExpressionOperator {
    OpSumm,
    OpSubstract,
    OpMultiply,
    OpDivision,
    OpPower,
    OpNot,
    OpAnd,
    OpOr
};

/** Expression tree-based representation */
struct Expression {

    /** Kind of expression */
    enum ExpressionType kind;

    /** Resulting type of expression */
    enum VariableBaseType type;

    /** Reference to variable in cases of
      * kind==StExprVariable or kind==StArrayElement or kind==StSlice;
      * set to NULL otherwise */
    struct Variable * variable;

    /** Reference to algorhitm in case of kind==StFunctionCall;
      * set to NULL otherwise */
    struct Algorhitm * function;

    /** 1. Empty in case of kind==StVariable
      * 2. Array bounds expressions in case of kind==StArrayBound
      * 3. SliceBounds expressions in case of kind==StSlice
      * 4. Function arguments expressions in case of kind==StFunctionCall
      * 5. Expressions list of size len(operators)+1 in case of kind==StSubexpression
      */
    QList<struct Expression> operands;

    /** List of operators in case of kind==StSubexpression, emty otherwise */
    QList<enum ExpressionOperator> operators;
};

}

#endif // AST_EXPRESSION_H
