#ifndef PDAUTOMATA_H
#define PDAUTOMATA_H

#include <QtCore>
#include "statement.h"
#include "interfaces/error.h"
#include "dataformats/ast.h"
#include "dataformats/ast_algorhitm.h"
#include "dataformats/ast_statement.h"

using namespace Shared;

typedef AST::Data AST_Data;
typedef AST::Algorhitm AST_Algorhitm;
typedef AST::Statement AST_Statement;

namespace KumirAnalizer {


class PDAutomata : public QObject
{
    Q_OBJECT
public:
    explicit PDAutomata(QObject *parent = 0);

    /** Automata initialization.
      * @param statements reference to source text
      * @param ast reference to existing AST
      * @param algorhitm if not NULL, then reference to
      *        analize subject algorhitn, otherwise analizes to
      *        whole AST
      */
    void init(bool teacherMode, const QList<Statement*> & statements
              , AST_Data * ast
              , AST_Algorhitm *algorhitm);

    /** Direct automata stage.
      * @returns 0 on allow; 1 on deny; 2 on limit exception
      */
    int process();

    /** Post-process automata stage */
    void postProcess();

    /** List of syntax errors */
    QList<Error> errors() const;

    static AST_Statement * createSimpleAstStatement(Statement * st);

private:
    class PDAutomataPrivate * d;

};

} // namespace KumirAnalizer

#endif // PDAUTOMATA_H
