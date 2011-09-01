#ifndef AST_EXPRESSION_H
#define AST_EXPRESSION_H

#include <QString>
#include <QList>
#include <QVariant>

#include "ast_variabletype.h"

#undef ABSTRACTSYNTAXTREE_EXPORT
#ifdef DATAFORMATS_LIBRARY
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_EXPORT
#else
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_IMPORT
#endif

namespace AST {

/** Type of expression structure */
enum ExpressionType {

    /** Dummy expression, when it optional
      * (for example, "step" value in for-loop)
      */
    ExprNone,

    /** Variable */
    ExprVariable,

    /** Constant */
    ExprConst,

    /** Array of string element, or slice */
    ExprArrayElement,

    /** Function call */
    ExprFunctionCall,

    /** Subexpression */
    ExprSubexpression
};

/** Operand delimeter for subexpressions */
enum ExpressionOperator {
    OpNone,
    OpSumm,
    OpSubstract,
    OpMultiply,
    OpDivision,
    OpPower,
    OpNot,
    OpAnd,
    OpOr,
    OpEqual,
    OpNotEqual,
    OpLess,
    OpGreater,
    OpLessOrEqual,
    OpGreaterOrEqual
};

/** Expression tree-based representation */
struct ABSTRACTSYNTAXTREE_EXPORT Expression {

    /** Kind of expression */
    enum ExpressionType kind;

    /** Resulting base type of expression */
    enum VariableBaseType baseType;

    /** Resulting dimension of base type */
    int dimension;

    /** Reference to variable in cases of
      * kind==StExprVariable or kind==StArrayElement or kind==StSlice;
      * set to NULL otherwise */
    struct Variable * variable;

    /** Constant value in case of kind==ExprConst */
    QVariant constant;

    /** Reference to algorhitm in case of kind==StFunctionCall;
      * set to NULL otherwise */
    struct Algorhitm * function;

    /** 1. Empty in case of kind==StVariable
      * 2. Array bounds expressions in case of kind==StArrayBound
      * 3. SliceBounds expressions in case of kind==StSlice
      * 4. Function arguments expressions in case of kind==StFunctionCall
      * 5. Expressions list of size len(operators)+1 in case of kind==StSubexpression
      */
    QList<struct Expression *> operands;

    /** Operator in case of kind==StSubexpression, emty otherwise */
    enum ExpressionOperator operatorr;

    /** Expression source lexems */
    QList<struct Lexem*> lexems;

    explicit Expression();
    explicit Expression(const struct Expression * src);
    void updateReferences(const struct Expression * src,
                          const struct Data * srcData,
                          const struct Data * data);
    QString dump() const;
    ~Expression();
};


}

#endif // AST_EXPRESSION_H
