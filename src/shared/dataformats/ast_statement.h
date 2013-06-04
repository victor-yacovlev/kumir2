#ifndef AST_STATEMENT_H
#define AST_STATEMENT_H

#include <QList>
#include <QSharedPointer>
#include <QWeakPointer>

#undef ABSTRACTSYNTAXTREE_EXPORT
#ifdef DATAFORMATS_LIBRARY
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_EXPORT
#else
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_IMPORT
#endif

namespace AST {

typedef QSharedPointer<struct Expression> ExpressionPtr;

typedef QSharedPointer<struct Algorithm> AlgorithmPtr;

typedef QSharedPointer<struct Variable> VariablePtr;

typedef QSharedPointer<struct Statement> StatementPtr;

typedef QSharedPointer<struct Data> DataPtr;

/** Defines statement (in terminology of Kumir 1.x: ProgaValue) type */
enum StatementType {

    /** No actual statement, but error code */
    StError,

    /** Assignment of function call
      * NOTE: function call is "assignment", where left part is void */
    StAssign,

    /** Assert, pre-condition of post-condition,
      * depending on context */
    StAssert,

    /** Array-variable initialization */
    StVarInitialize,

    /** Terminal input */
    StInput,

    /** Terminal output */
    StOutput,

    /** Loop block */
    StLoop,

    /** One or two if-then-else sub-blocks */
    StIfThenElse,

    /** At lease one switch-case-else sub-block */
    StSwitchCaseElse,

    /** Loop break of function return statement */
    StBreak,

    /** Pause evaluation until key pressed */
    StPause,

    /** Terminate evaluation unconditionally */
    StHalt
};

/** Loop clarification */
enum LoopType {

    /** For loop (russian: "nc dlja") */
    LoopFor,

    /** While or forever loop (russian: "nc" or "nc poka") */
    LoopWhile,

    /** Counter loop (russian "nc ? raz") */
    LoopTimes,

    /** Forever loop: a particular case of 'while (true)' */
    LoopForever
};

/** Loop-specific part of statement */
struct LoopSpec {

    /** Loop type */
    enum LoopType type;

    /** Referece to "for" loop variable */
    VariablePtr forVariable;

    /** "for" loop "from" value */
    ExpressionPtr fromValue;

    /** "for" loop "to" value */
    ExpressionPtr toValue;

    /** "for" loop "step" value */
    ExpressionPtr stepValue;

    /** "while" loop condition value */
    ExpressionPtr whileCondition;

    /** "times" loop value */
    ExpressionPtr timesValue;

    /** Optional condition for loop end */
    ExpressionPtr endCondition;

    /** Loop body */
    QList<StatementPtr> body;

    /** Loop end lexems */
    QList<struct Lexem*> endLexems;

};

/** "if-then", "case" or "else" sub-block of statement */
struct ConditionSpec {

    StatementPtr parent;

    /** Condition; empty for "else" block */
    ExpressionPtr condition;

    /** Conditional body */
    QList<StatementPtr> body;

    /** Condition source lexems */
    QList<struct Lexem*> lexems;

    QString conditionError;
};

/** Evaluable statement */
struct ABSTRACTSYNTAXTREE_EXPORT Statement {

    /** Statement kind */
    StatementType type;

    /** Tree parent */

    StatementPtr parent;

    /** Statement source lexems */
    QList<struct Lexem*> lexems;
    QList<struct Lexem*> endBlockLexems;

    /** Compile-time error */
    QString error;

    QString headerError;
    int headerErrorLine;
    QString beginBlockError;
    QString endBlockError;

    /** If error: add to generated code or not */
    bool skipErrorEvaluation;

    /** List of top-level expressions for all
      * statement kinds except StLoop, StIfThenElse and StSwitchCaseElse */
    QList<ExpressionPtr> expressions;

    /** Variable references list, defined only if type==StVarInitialize */
    QList<VariablePtr> variables;

    /** Loop specific-part, defined only if type==StLoop */
    struct LoopSpec loop;

    /** Conditional blocks, defined only if type==StIfThenElse or type==StSwitchCaseElse */
    QList<struct ConditionSpec> conditionals;

    explicit Statement();
    explicit Statement(const StatementPtr src);
    void updateReferences(const Statement * src,
                          const struct Data * srcData,
                          const struct Data * data);
};


}

#endif // AST_STATEMENT_H
