#ifndef AST_MODULE_H
#define AST_MODULE_H

#include "ast_type.h"

#include <QString>
#include <QList>
#include <QSet>
#include <QStringList>
#include <QSharedPointer>
#include <QWeakPointer>

#undef ABSTRACTSYNTAXTREE_EXPORT
#ifdef DATAFORMATS_LIBRARY
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_EXPORT
#else
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_IMPORT
#endif

namespace AST {

typedef QSharedPointer<struct Module> ModulePtr;
typedef QWeakPointer<struct Module> ModuleWPtr;

typedef QSharedPointer<struct Algorithm> AlgorithmPtr;

typedef QSharedPointer<struct Variable> VariablePtr;

typedef QSharedPointer<struct Statement> StatementPtr;

typedef QSharedPointer<struct Data> DataPtr;

enum ModuleType {

    /** User module */
    ModTypeUser,

    /** Hidden (e.g. teacher) pseudo-module */
    ModTypeTeacher,

    /** User module in another file */
    ModTypeCached,

    /** External or system built-in module */
    ModTypeExternal

};


/** Module public header */
struct ModuleHeader {

    /** Module name, may be empty (for program itself)*/
    QString name;

    /** Module type */
    enum ModuleType type;

    /** List of public (i.e. not underscore-starting named) algorithms,
      * represented as references to corresponding functions */
    QList<AlgorithmPtr> algorhitms;

    /** List of overrided operators,
     * each represented as algorithm */
    QList<AlgorithmPtr> operators;

    /** Module custom types */
    QList<struct Type> types;

    /** True, if module is enabled to use */
    QList<ModuleWPtr> usedBy;
};

/** Module body (private to other modules) */
struct ModuleImplementation {

    /** Global variables and constants table */
    QList<VariablePtr> globals;

    /** Module algorhitms table */
    QList<AlgorithmPtr> algorhitms;

    /** Body of module initializer (i.e. statements before
      * first algorhitm declaration) */
    QList<StatementPtr> initializerBody;

    /** Module begin lexems */
    QList<struct Lexem*> beginLexems;

    /** Module end lexems */
    QList<struct Lexem*> endLexems;

};


/** Module representation */
struct ABSTRACTSYNTAXTREE_EXPORT Module {

    /** Public interface of module */
    struct ModuleHeader header;

    quint8 builtInID;

    /** Module internal representation */
    struct ModuleImplementation impl;

    explicit Module();
    explicit Module(const ModulePtr src);
    void updateReferences(const Module * src,
                          const struct Data* srcData,
                          const struct Data * data);
    bool isEnabledFor(const ModulePtr reference) const;

};

}

#endif // AST_MODULE_H
