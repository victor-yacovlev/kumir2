#ifndef AST_MODULE_H
#define AST_MODULE_H

#include <QString>
#include <QList>
#include <QSet>


#undef ABSTRACTSYNTAXTREE_EXPORT
#ifdef ABSTRACTSYNTAXTREE_LIBRARY
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_EXPORT
#else
#define ABSTRACTSYNTAXTREE_EXPORT Q_DECL_IMPORT
#endif

namespace AST {

enum ModuleType {

    /** User module */
    ModTypeUser,

    /** User/teacher module in another file */
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

    /** List of public (i.e. not underscore-starting named) algorhitms,
      * represented as references to corresponding functions */
    QList<struct Algorhitm *> algorhitms;

    /** List of dependent modules */
    QSet<QString> uses;

    /** True, if module is enabled to use */
    bool enabled;
};

/** Module body (private to other modules) */
struct ModuleImplementation {

    /** Global variables and constants table */
    QList<struct Variable *> globals;

    /** Module algorhitms table */
    QList<struct Algorhitm *> algorhitms;

    /** Body of module initializer (i.e. statements before
      * first algorhitm declaration) */
    QList<struct Statement *> initializerBody;

    /** Module begin lexems */
    QList<struct Lexem*> beginLexems;

    /** Module end lexems */
    QList<struct Lexem*> endLexems;
};


/** Module representation */
struct ABSTRACTSYNTAXTREE_EXPORT Module {

    /** Public interface of module */
    struct ModuleHeader header;

    /** Module internal representation */
    struct ModuleImplementation impl;

    explicit Module();
    explicit Module(const struct Module * src);
    void updateReferences(const struct Module * src,
                          const struct Data * srcData,
                          const struct Data * data);

    QString dump() const;
    bool load(const QString &data);
    ~Module();

};

}

#endif // AST_MODULE_H
