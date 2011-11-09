#ifndef SYNTAXANALIZER_H
#define SYNTAXANALIZER_H

#include <QtCore>
#include "statement.h"

#include "dataformats/ast.h"
#include "dataformats/ast_algorhitm.h"

typedef AST::Data AST_Data;
typedef AST::Algorhitm AST_Algorhitm;

namespace KumirAnalizer {

class SyntaxAnalizer : public QObject
{
    Q_OBJECT
public:
    explicit SyntaxAnalizer(class Lexer * lexer, QObject *parent = 0);
    void init(QList<Statement*> & statements
              , AST_Data * ast
              , AST_Algorhitm *algorhitm);
    void syncStatements();
    QStringList unresolvedImports() const;
    void buildTables();
    void processAnalisys();
    ~SyntaxAnalizer();
private:
    struct SyntaxAnalizerPrivate * d;
};

} // namespace KumirAnalizer

#endif // SYNTAXANALIZER_H
