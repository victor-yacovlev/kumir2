#ifndef PDAUTOMATA_H
#define PDAUTOMATA_H

#include <QtCore>
#include "statement.h"
#include "interfaces/error.h"
#include "dataformats/ast.h"
#include "dataformats/ast_algorhitm.h"
#include "dataformats/ast_statement.h"


namespace KumirAnalizer {


class PDAutomata : public QObject
{
    Q_OBJECT
public:
    explicit PDAutomata(QObject *parent = 0);
    ~PDAutomata();

    /** Automata initialization.
      * @param statements reference to source text
      * @param ast reference to existing AST
      * @param algorhitm if not NULL, then reference to
      *        analize subject algorhitn, otherwise analizes to
      *        whole AST
      */
    void init(
              const QList<TextStatementPtr> & statements
              , AST::ModulePtr module
              );

    /** Direct automata stage.
      * @returns 0 on allow; 1 on deny; 2 on limit exception
      */
    int process();

    /** Post-process automata stage */
    void postProcess();

    /** List of syntax errors */
    QList<Shared::Error> errors() const;

    static AST::StatementPtr createSimpleAstStatement(TextStatementPtr st);

private:

    struct Script {
        QMetaMethod method;
        QList<QVariant> arguments;
        QString source;
        QString scriptInfo;
    };

    typedef QList<Script> * ScriptListPtr;

    struct RuleRightPart {
        QStringList nonTerminals;
        ScriptListPtr script;
        bool isEpsilon;
        qreal priority;
        inline bool operator< (const RuleRightPart &other) const {
                return priority < other.priority;
        }
        int ruleLine;
    };

    typedef QList<RuleRightPart> Rules;

    typedef QMap<QString,Rules> Matrix;

    struct PDStackElem {
            QString nonTerminal;
            int iterateStart;
            qreal priority;
    };

    AST::StatementPtr findASTStatementBySourceStatement(const TextStatementPtr st) const;

    static void updateBackReferences(const QList<AST::StatementPtr> &alist);
    static void updateBackReferences(AST::StatementPtr root);

    void loadRules(const QString &rulesRoot);


    Matrix matrix_;
    QList<TextStatementPtr> source_;
    bool allowSkipParts_;

    int currentPosition_;
    QVector<ScriptListPtr> scripts_;
    QVector<QString> acceptedRules_;
    QStack<PDStackElem> stack_;

    QStack<int> history_errorsCount_;
    QStack<int> history_currentPosition_;
    QStack< QVector<ScriptListPtr> > history_scripts_;
    QStack< QStack<PDStackElem> > history_stack_;
    QStack< QVector<int> > history_nextPointers_;

    QVector<ScriptListPtr> fixedScripts_;
    QVector<int> nextPointers_;

    AST::ModulePtr currentModule_;
    AST::AlgorithmPtr currentAlgorhitm_;
    QStack< QList<AST::StatementPtr> * > currentContext_;

    qreal maxPriorityValue_;

    void matchScript(const QString &text, ScriptListPtr & scripts, const QString & scriptInfo);
    void addEpsilonRule(const QString &nonTerminal, const qreal prior, const QString & script );

    int errorsCount_;

private slots:
    void addDummyAlgHeader();
    void setCurrentError(const QString & value);
    void setCurrentErrorRaisePosition(Lexem::ErrorRaisePosition pos);
    void setOutOfAlgError();
    void setModuleBeginError(const QString & value);
    void setCurrentIndentRank(int start, int end);
    void processCorrectEndOfLoop();
    void processAlgEndInsteadOfLoopEnd();
    void processCorrectCase();
    void processCorrectIf();
    void processCorrectThen();
    void processCorrectThenIfNotExists();
    void processCorrectThenIfNotExists2();
    void processCorrectFi();
    void processCorrectElse();
    void processCorrectSwitch();
    void processCorrectBeginOfLoop();
    void processCorrectAlgHeader();
    void processCorrectAlgBegin();
    void processCorrectDocLine();
    void processCorrectRestrictionLine();
    void processCorrectAlgEnd();
    void processCorrectLoad();
    void appendSimpleLine();
    void processAlgWithNoBegin();
    void setGarbageAlgError();
    void setGarbageIfThenError();
    void setGarbageSwitchCaseError();
    void setCorrespondingIfBroken();
    void setExtraOpenKeywordError(const QString & kw);
    void setExtraCloseKeywordError(const QString & kw);
    void suggest(const QString & text, int moveCursorBackLinesCount);
    void setTooManyErrors();
    void finalizeIterativeRule(const PDStackElem & stackElem);
    void nextStep();
    void saveData();
    void restoreData();
    void popHistory();
    void clearDataHistory();

};

} // namespace KumirAnalizer

#endif // PDAUTOMATA_H
