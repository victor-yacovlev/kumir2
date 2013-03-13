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
    QString source;
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

class PDAutomataPrivate:
        public QObject
{
    Q_OBJECT
public:

    void loadRules(const QString &rulesRoot);    
    class PDAutomata * q;
    ~PDAutomataPrivate();
    AST::Data * ast;
    AST::Algorhitm * algorhitm;
    bool teacherMode;

    Matrix matrix;
    QList<Statement*> source;
    bool allowSkipParts;

    int currentPosition;
    QVector<ScriptListPtr> scripts;
    QVector<QString> acceptedRules;
    QStack<PDStackElem> stack;

    QStack<int> history_currentPosition;
    QStack< QVector<ScriptListPtr> > history_scripts;
    QStack< QStack<PDStackElem> > history_stack;
    QStack< QVector<int> > history_nextPointers;

    QVector<ScriptListPtr> fixedScripts;
    QVector<int> nextPointers;

    AST::Module * currentModule;
    AST::Algorhitm * currentAlgorhitm;
    QStack< QList<AST::Statement*> * > currentContext;

    qreal maxPriorityValue;

    void matchScript(const QString &text, ScriptListPtr & scripts);
    void addEpsilonRule(const QString &nonTerminal, const qreal prior, const QString & script );

    int errorsCount;

public slots:
    void addDummyAlgHeader();
    void setCurrentError(const QString & value);
    void setCurrentErrorRaisePosition(Lexem::ErrorRaisePosition pos);
    void setOutOfAlgError();
    void setModuleBeginError(const QString & value);
    void setCurrentIndentRank(int start, int end);
    void processCorrectEndOfLoop();
    void processAlgEndInsteadOfLoopEnd();
    void processModEndInsteadOfAlgEnd();
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
    void processCorrectModuleBegin();
    void processCorrectModuleEnd();
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

}

#endif // PDAUTOMATA_P_H
