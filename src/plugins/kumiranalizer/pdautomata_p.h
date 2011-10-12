#ifndef PDAUTOMATA_P_H
#define PDAUTOMATA_P_H

#include <QtCore>
#include "statement.h"
#include "dataformats/ast.h"

using namespace Shared;

namespace KumirAnalizer {

struct Script {
    QMetaMethod method;
    QList<QVariant> arguments;
};

typedef QList<Script> * ScriptListPtr;

struct RuleRightPart {
    QStringList nonTerminals;
    ScriptListPtr script;
    bool isEpsilon;
    int priority;
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
        int priority;
};

class PDAutomataPrivate:
        public QObject
{
    Q_OBJECT;
public:

    void loadRules(const QString &rulesRoot);
    class PDAutomata * q;
    AST::Data * ast;
    AST::Algorhitm * algorhitm;
    bool teacherMode;

    Matrix matrix;
    QList<Statement*> source;
    bool allowSkipParts;

    int currentPosition;
    QVector<ScriptListPtr> scripts;
    QStack<PDStackElem> stack;

    QStack<int> history_currentPosition;
    QStack< QVector<ScriptListPtr> > history_scripts;
    QStack< QStack<PDStackElem> > history_stack;

    QVector<ScriptListPtr> fixedScripts;
    QVector<int> nextPointers;

    AST::Module * currentModule;
    AST::Algorhitm * currentAlgorhitm;
    QStack< QList<AST::Statement*> * > currentContext;

    int maxPriorityValue;

    void matchScript(const QString &text, ScriptListPtr & scripts);

    int errorsCount;

public slots:
    void addDummyAlgHeader();
    void setCurrentError(const QString & value);
    void setModuleBeginError(const QString & value);
    void setCurrentIndentRank(int start, int end);
    void processCorrectEndOfLoop();
    void processAlgEndInsteadOfLoopEnd();
    void processCorrectCase();
    void processCorrectIf();
    void processCorrectThen();
    void processCorrectFi();
    void processCorrectElse();
    void processCorrectSwitch();
    void processCorrectBeginOfLoop();
    void processCorrectAlgHeader();
    void processCorrectAlgBegin();
    void processCorrectDocLine();
    void processCorrectRestrictionLine();
    void processCorrectAlgEnd();
    void processCorrectModuleBegin();
    void processCorrectModuleEnd();
    void processCorrectLoad();
    void appendSimpleLine();
    void setGarbageAlgError();
    void setGarbageIfThenError();
    void setGarbageSwitchCaseError();
    void setCorrespondingIfBroken();
    void setExtraOpenKeywordError(const QString & kw);
    void setExtraCloseKeywordError(const QString & kw);

    void setTooManyErrors();
    void finalizeIterativeRule(const PDStackElem & stackElem);
    void nextStep();
    void saveData();
    void restoreData();
    void popHistory();
    void clearDataHistory();

};

}

#endif // PDAUTOMATA_P_H
