#ifndef KUMIRCODERUN_VM_H
#define KUMIRCODERUN_VM_H

#include <QtCore>
#include "variant.h"
#include "context.h"
#include "faststack.h"
#include "dataformats/bc_tableelem.h"
#include "dataformats/bytecode.h"
#include "shared/interfaces/actorinterface.h"

namespace KumirCodeRun {

using namespace Shared;

class VM : public QObject
{
    friend class Plugin;
    Q_OBJECT
    Q_ENUMS(RunEntryPoint)
public:
    enum RunEntryPoint { EP_Main, EP_Testing };
    explicit VM(QObject *parent = 0);
    void reset();
    int effectiveLineNo() const;
    void setAvailableActors(const QList<ActorInterface*> & actors);
    ElemType topStackType() const;
    void evaluateNextInstruction();
    void updateStFunctError();
    QStringList usedActors() const;
    inline bool isBlindMode() const { return b_blindMode; }
    void setBlindMode(bool bl);
    bool hasMoreInstructions() const;
    inline QString error() const { return s_error; }
    void loadProgram(const Data & program);
    inline int deep() const { return stack_contexts.size(); }
    inline void setEntryPoint(RunEntryPoint e) { e_entryPoint = e; }
    void pushValueToStack(const QVariant & value);
    void setResults(const QString & error,
        const QList<quintptr> & references,
        const QList<int> & indeces,
        const QList<QVariant> & values);
    void setLocalVariableValue(int localId, const QVariant & value);
    void setNextCallInto();
    void setNextCallOut();
    void setNextCallToEnd();
    void setNextCallStepOver();
    int currentLineNo() const;
    QVariantList remainingValues() const;

    QVariant value(int moduleId, int algorhitmId, int variableId) const;

    QList<int> bounds(int moduleId, int algorhitmId, int variableId) const;

    QList<int> reference(int moduleId, int algorhitmId, int variableId) const;
signals:
    void retInstruction(int lineNo);
    void lineNoChanged(int lineNo);
    void inputRequest(const QString & format, const QList<quintptr> & references, const QList<int> & indeces);
    void outputRequest(const QString & out);
    void pauseRequest();
    void contextAboutToPop();
    void beforeExternalFunction();
    void resetModuleRequest(const QString & actorPluginName);
    void invokeExternalFunction(const QString & actorPluginName,
                                const QString & functionName,
                                const QVariantList & arguments,
                                const QList<quintptr> & references,
                                const QList<int> & indeces
                                );
    void invokeExternalFunction(const QList<quintptr> & references);
    void valueChangeNotice(int lineNo, const QString & text);
    void inputArgumentRequest(int localId,
                              const QString & varName,
                              const QString & baseFormat,
                              const QList<int> & bounds);
    void outputArgumentRequest(const QVariant & value,
                               const QString & varName,
                               const QList<int> & bounds);

    void clearMargin(int fromLine, int toLine);
private:
    int contextByIds(int moduleId, int algorhitmId) const;
    QVariant register0;
    FastStack<Variant> stack_values;
    FastStack<Context> stack_contexts;
    Context last_context;
    QMap< QPair<quint8,quint16>, Variant> globals;
    QMap<quint16, Variant> constants;

    QMap< quint32, Bytecode::TableElem > externs;
    QMap< quint32, QPair<ActorInterface*,quint32> > externalMethods;

    QMap< quint32, Bytecode::TableElem > functions;
    Bytecode::TableElem mainProgram;
    Bytecode::TableElem testingProgram;
    QMap< quint8, Bytecode::TableElem > inits;
    QMap< quint32, QVector<Variant> > cleanLocalTables;
    QString s_error;
    bool b_nextCallInto;
    unsigned int i_backtraceSkip;

    RunEntryPoint e_entryPoint;

    void nextIP();

    QMap< QPair<QString,QString>, QPair<ActorInterface*,quint32> > availableExternalMethods;


    void call_externalMethod(ActorInterface * actor, quint32 method);


    void do_call(quint8, quint16);
    void do_init(quint8, quint16);
    void do_setarr(quint8, quint16);
    void do_updarr(quint8, quint16);
    void do_store(quint8, quint16);
    void do_storearr(quint8, quint16);
    void do_load(quint8, quint16);
    void do_loadarr(quint8, quint16);
    void do_jump(quint16);
    void do_jz(quint8, quint16);
    void do_jnz(quint8, quint16);
    void do_pop(quint8);
    void do_push(quint8);
    void do_ret();
    void do_error(quint8, quint16);
    void do_line(quint16);
    void do_ref(quint8, quint16);
    void do_setref(quint8, quint16);
    void do_refarr(quint8, quint16);
    void do_showreg(quint8);
    void do_clearmarg(quint16);
    void do_pause(quint16);
    void do_halt(quint16);
    void do_ctl(quint8 parameter, quint16 value);
    void do_inrange();

    void do_sum();
    void do_sub();
    void do_mul();
    void do_div();
    void do_pow();
    void do_neg();
    void do_and();
    void do_or();
    void do_eq();
    void do_neq();
    void do_ls();
    void do_gt();
    void do_leq();
    void do_geq();

    bool b_blindMode;
    QMutex * m_dontTouchMe;

    QFile f_input;
    QFile f_output;

};

} // namespace KumirCodeRun

#endif // KUMIRCODERUN_VM_H
