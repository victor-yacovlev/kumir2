#ifndef AST_MODULE_H
#define AST_MODULE_H

#include <QString>
#include <QList>
#include <QStringList>

#include "ast_statement.h"
#include "ast_variable.h"
#include "ast_algorhitm.h"

namespace AST {

/** Module public header */
struct ModuleHeader {

    /** Module name, may be empty (for program itself)*/
    QString name;

    /** List of public (i.e. not underscore-starting named) algorhitms,
      * represented as references to corresponding functions */
    QList<struct AlgorhitmHeader *> algorhitms;

    /** List of dependent modules */
    QStringList uses;
};

/** Module body (private to other modules) */
struct ModuleImplementation {

    /** Global variables and constants table */
    QList<struct Variable> globalVariables;

    /** Module algorhitms table */
    QList<struct Algorhitm> algorhitms;

    /** Body of module initializer (i.e. statements before
      * first algorhitm declaration) */
    QList<struct Statement> initializerBody;
};


/** Module representation */
struct Module {

    /** Public interface of module */
    struct ModuleHeader header;

    /** Module internal representation */
    struct ModuleImplementation impl;
};

}

#endif // AST_MODULE_H
