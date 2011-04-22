#ifndef AST_ALGORHITM_H
#define AST_ALGORHITM_H

#include <QString>
#include <QList>

#include "ast_variabletype.h"
#include "ast_statement.h"
#include "ast_variable.h"

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

/** Algorhitm public header */
struct AlgorhitmHeader {

    /** Algorhitm name, may be empty (for first algorhitm) */
    QString name;

    /** Algorhitm special type */
    enum AlgorhitmType specialType;

    /** Return type */
    enum VariableBaseType returnType;

    /** Formal arguments, represented as
      * references to corresponding local variables */
    QList<struct Variable *> arguments;
};

/** Algorhitm body (private to other components) */
struct AlgorhitmImplementation {

    /** Local variables and constants table */
    QList<struct Variable> localVariables;

    /** Statements of pre-condition block (russian: dano) */
    QList<struct Statement> preStatements;

    /** Statements of pre-condition block (russian: nado) */
    QList<struct Statement> postStatements;

    /** Statements of algorhitm body */
    QList<struct Statement> bodyStatements;
};

/** Algorhitm representation */
struct Algorhitm {

    /** Public header for use in module etc. */
    struct AlgorhitmHeader header;

    /** Internal representation */
    struct AlgorhitmImplementation impl;
};

}

#endif // AST_ALGORHITM_H
