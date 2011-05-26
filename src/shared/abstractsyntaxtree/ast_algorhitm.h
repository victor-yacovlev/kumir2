#ifndef AST_ALGORHITM_H
#define AST_ALGORHITM_H

#include <QString>
#include <QList>

#include "ast_variabletype.h"
#include "ast_statement.h"
#include "ast_variable.h"

#undef ABSTRACTSYNTAXTREE_EXPORT
#ifdef ABSTRACTSYNTAXTREE_LIBRARY
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_EXPORT
#else
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_IMPORT
#endif

namespace AST {

/** Specific algorhitm types */
enum AlgorhitmType {

    /** Regular algorhitm */
    AlgorhitmTypeRegular,

    /** Available to write only by teacher */
    AlgorhitmTypeTeacher,

    /** Testing mode entry point */
    AlgorhitmTypeTesting
};

/** Algorhitm implementation type */
enum AlgorhitmImplementationType {

    /** Kumir-compiler algorhitm */
    AlgorhitmCompiled,

    /** External or builtin algorhitm */
    AlgorhitmExternal

};

/** External reference for out-of module algorhitm */
struct AlgorhitmExternalReference {
    QString moduleName;
    int id;
};

/** Algorhitm public header */
struct AlgorhitmHeader {

    /** Algorhitm name, may be empty (for first algorhitm) */
    QString name;

    QString cHeader;

    QString pascalHeader;

    /** Algorhitm special type */
    enum AlgorhitmType specialType;

    /** Algorhitm implementation type*/
    enum AlgorhitmImplementationType implType;

    /** Algorhitm external reference, if defined */
    struct AlgorhitmExternalReference external;

    /** Return type */
    enum VariableBaseType returnType;

    /** Formal arguments, represented as
      * references to corresponding local variables */
    QList<struct Variable *> arguments;

    /** Not empty if algorhitm header contains error,
      * but algorhitm name still reachable */
    QString error;
};

/** Algorhitm body (private to other components) */
struct AlgorhitmImplementation {

    /** Local variables and constants table */
    QList<struct Variable *> locals;

    /** Statements of pre-condition block (russian: dano) */
    QList<struct Statement *> pre;

    /** Statements of pre-condition block (russian: nado) */
    QList<struct Statement *> post;

    /** Statements of algorhitm body */
    QList<struct Statement *> body;

    /** Header lexems */
    QList<struct Lexem*> headerLexems;

    /** Begin lexems */
    QList<struct Lexem*> beginLexems;

    /** End lexems */
    QList<struct Lexem*> endLexems;
};

/** Algorhitm representation */
struct ABSTRACTSYNTAXTREE_EXPORT Algorhitm {

    /** Public header for use in module etc. */
    struct AlgorhitmHeader header;

    /** Internal representation */
    struct AlgorhitmImplementation impl;

    explicit Algorhitm();
    explicit Algorhitm(const struct Algorhitm * src);
    void updateReferences(const struct Algorhitm * src,
                          const struct Data * srcData,
                          const struct Data * data);
    QString dump() const;
    ~Algorhitm();
};

}

#endif // AST_ALGORHITM_H
