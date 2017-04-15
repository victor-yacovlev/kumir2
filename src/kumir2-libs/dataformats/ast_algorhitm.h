#ifndef AST_ALGORHITM_H
#define AST_ALGORHITM_H

#include <QString>
#include <QList>

#include "ast_variabletype.h"
#include "ast_statement.h"
#include "ast_variable.h"
#include "ast_type.h"

#include "lexem.h"

#undef ABSTRACTSYNTAXTREE_EXPORT
#ifdef DATAFORMATS_LIBRARY
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_EXPORT
#else
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_IMPORT
#endif

namespace AST {

typedef QSharedPointer<struct Algorithm> AlgorithmPtr;

typedef QSharedPointer<struct Variable> VariablePtr;

typedef QSharedPointer<struct Statement> StatementPtr;

typedef QSharedPointer<struct Data> DataPtr;

/** Specific algorhitm types */
enum AlgorhitmType {

    /** Regular algorhitm */
    AlgorithmTypeRegular,

    /** Available to write only by teacher */
    AlgorithmTypeTeacher,

    /** Testing mode entry point */
    AlgorithmTypeTesting
};

/** Algorhitm implementation type */
enum AlgorhitmImplementationType {

    /** Kumir-compiled algorhitm */
    AlgorhitmCompiled,

    /** External or builtin algorhitm */
    AlgorhitmExternal

};

/** External reference for out-of module algorhitm */
struct AlgorhitmExternalReference {
    QString moduleName;
    QByteArray moduleAsciiName;
    QByteArray algorithmAsciiName;
    quint16 id;
};

/** Algorhitm public header */
struct AlgorhitmHeader {

    /** Algorhitm name, may be empty (for first algorhitm) */
    QString name;

    /** Algorhitm special type */
    enum AlgorhitmType specialType;

    /** Algorhitm implementation type*/
    enum AlgorhitmImplementationType implType;

    /** Algorhitm external reference, if defined */
    struct AlgorhitmExternalReference external;

    /** Return type */
    Type returnType;

    /** Formal arguments, represented as
      * references to corresponding local variables */
    QList<VariablePtr> arguments;

    /** Not empty if algorhitm header contains error,
      * but algorhitm name still reachable */
    QString error;

    /** True if algorithm declaration wrong */
    bool broken;
};

/** Algorhitm body (private to other components) */
struct AlgorhitmImplementation {

    /** Local variables and constants table */
    QList<VariablePtr> locals;

    /** Statements of pre-condition block (russian: dano) */
    QList<StatementPtr> pre;

    /** Statements of pre-condition block (russian: nado) */
    QList<StatementPtr> post;

    /** Statements of algorhitm body */
    QList<StatementPtr> body;

    /** Header lexems */
    QList<LexemPtr> headerLexems;

    /** Begin lexems */
    QList<LexemPtr> beginLexems;

    /** End lexems */
    QList<LexemPtr> endLexems;

    QString headerRuntimeError;
    int headerRuntimeErrorLine;
};

/** Algorhitm representation */
struct ABSTRACTSYNTAXTREE_EXPORT Algorithm {

    /** Public header for use in module etc. */
    struct AlgorhitmHeader header;

    /** Internal representation */
    struct AlgorhitmImplementation impl;

    explicit Algorithm();
    explicit Algorithm(const AlgorithmPtr src);
    void updateReferences(const Algorithm * src,
                          const struct Data * srcData,
                          const struct Data * data);

};

}

#endif // AST_ALGORHITM_H
