#ifndef AST_EXPRESSION_H
#define AST_EXPRESSION_H


#include <QString>
#include <QList>
#include <QVariant>
#include <QSharedPointer>
#include <QWeakPointer>

#include "lexem.h"
#include "ast_variabletype.h"
#include "ast_type.h"

#undef ABSTRACTSYNTAXTREE_EXPORT
#ifdef DATAFORMATS_LIBRARY
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_EXPORT
#else
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_IMPORT
#endif

namespace AST {

typedef QSharedPointer<struct Module> ModulePtr;

typedef QSharedPointer<struct Algorithm> AlgorithmPtr;

typedef QSharedPointer<struct Variable> VariablePtr;

typedef QSharedPointer<struct Expression> ExpressionPtr;

typedef QSharedPointer<struct Statement> StatementPtr;

typedef QSharedPointer<struct Data> DataPtr;

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
    Type baseType;

    /** Resulting dimension of base type */
    int dimension;

    /** Reference to variable in cases of
      * kind==StExprVariable or kind==StArrayElement or kind==StSlice;
      * set to NULL otherwise */
    VariablePtr  variable;

    /** Constant value in case of kind==ExprConst */
    QVariant constant;

    /** Reference to algorhitm in case of kind==StFunctionCall;
      * set to NULL otherwise */
    AlgorithmPtr function;

    /** 1. Empty in case of kind==StVariable
      * 2. Array bounds expressions in case of kind==StArrayBound
      * 3. SliceBounds expressions in case of kind==StSlice
      * 4. Function arguments expressions in case of kind==StFunctionCall
      * 5. Expressions list of size len(operators)+1 in case of kind==StSubexpression
      */
    QList<ExpressionPtr> operands;

    /** True if expression () bounds.
      * This flag is used to avoid CNF-transofmations */
    bool expressionIsClosed;

    /** True if string element or slice */
    bool isStringPart;

    /** Operator in case of kind==StSubexpression, emty otherwise */
    enum ExpressionOperator operatorr;

    /** Expression source lexems */
    QList<LexemPtr> lexems;

    explicit Expression();
    explicit Expression(const ExpressionPtr src);
    void updateReferences(const Expression * src,
                          const struct Data * srcData,
                          const struct Data * data);
};


}

#endif // AST_EXPRESSION_H
