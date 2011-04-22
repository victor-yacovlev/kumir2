#ifndef PDAUTOMATA_P_H
#define PDAUTOMATA_P_H

#include <QtCore>
#include "statement.h"
#include "abstractsyntaxtree/ast.h"

using namespace Shared;

namespace KumirAnalizer {

struct Script {
    QMetaMethod method;
    QList<QVariant> arguments;
};

struct RuleRightPart {
    QStringList nonTerminals;
    QList<Script> scripts;
    bool isEpsilon;
    int priority;
    inline bool operator< (const RuleRightPart &other) const {
            return priority < other.priority;
    }
    int ruleLine;
};

typedef QList<RuleRightPart> Rules;

typedef QMap<QString,Rules> Matrix;

class PDAutomataPrivate:
        public QObject
{
    Q_OBJECT;
public:

    void loadRules(const QString &rulesRoot);
    class PDAutomata * q;
    AST::AST * ast;
    AST::Algorhitm * algorhitm;

    Matrix matrix;
    QList<Statement> * source;

    int i_maxPriorityValue;

    void matchScript(const QString &text, QList<Script> & scripts);


public slots:
    void setError(int value);
    void processCorrentEndOfLoop();
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
    void processCorrectInputRestrictionLine();
    void processCorrectOutputRestrictionLine();
    void processCorrectAlgEnd();
    void processCorrectModuleBegin();
    void processCorrectModuleEnd();
    void processCorrectLoad();
    void appendSimpleLine();
    void setGarbageAlgError();
    void setGarbageIfThenError();
    void setGarbageSwitchCaseError();
    void setCorrespondingIfBroken();
    void setExtraOpenKeywordError(int kw);
    void setExtraCloseKeywordError(int kw);

};

}

#endif // PDAUTOMATA_P_H
