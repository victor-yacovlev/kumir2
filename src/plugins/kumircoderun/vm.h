#ifndef KUMIRCODERUN_VM_H
#define KUMIRCODERUN_VM_H

#include <QtCore>
#include "variant.h"
#include "context.h"
#include "bytecode/tableelem.h"
#include "bytecode/data.h"

namespace KumirCodeRun {

class VM : public QObject
{
    Q_OBJECT
    Q_ENUMS(RunEntryPoint)
public:
    enum RunEntryPoint { EP_Main, EP_Testing };
    explicit VM(QObject *parent = 0);
    void reset();
    ElemType topStackType() const;
    bool canStepInto() const;
    void evaluateNextInstruction();
    QStringList usedActors() const;
    inline bool hasMoreInstructions() const { return !stack_contexts.isEmpty() && stack_contexts[0].IP < stack_contexts[0].program.size(); }
    inline QString error() const { return s_error; }
    void loadProgram(const Data & program);
    inline int deep() const { return stack_contexts.size(); }
    inline void setEntryPoint(RunEntryPoint e) { e_entryPoint = e; }
    void pushValueToStack(const QVariant & value);
    void setResults(const QString & error,
        const QList<quintptr> & references,
        const QList<int> & indeces,
        const QList<QVariant> & values);
    void setNextCallInto();
    void setNextCallOut();
    void setNextCallToEnd();
    void setNextCallStepOver();
    int currentLineNo() const;
signals:
    void lineNoChanged(int lineNo);
    void inputRequest(const QString & format, const QList<quintptr> & references, const QList<int> & indeces);
    void outputRequest(const QString & out);
    void resetModuleRequest(const QString & actorPluginName);
    void invokeExternalFunction(const QString & actorPluginName,
                                const QString & functionName,
                                const QVariantList & arguments,
                                const QList<quintptr> & references,
                                const QList<int> & indeces
                                );
    void valueChangeNotice(int lineNo, const QString & text);
    void clearMargin(int fromLine, int toLine);
private:
    QStack<Variant> stack_values;
    QStack<Context> stack_contexts;
    QMap<quint16, Variant> globals;
    QMap<quint16, Variant> constants;
    QMap< quint32, Bytecode::TableElem > externs;
    QMap< quint32, Bytecode::TableElem > functions;
    QMap< quint32, QVector<Variant> > cleanLocalTables;
    QString s_error;
    bool b_nextCallInto;

    RunEntryPoint e_entryPoint;

    inline void nextIP() { stack_contexts[stack_contexts.size()-1].IP ++; }

    void do_call(quint8, quint16);
    void do_init(quint8, quint16);
    void do_setarr(quint8, quint16);
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
    void do_refarr(quint8, quint16);
    void do_showreg(quint8);
    void do_clearmarg(quint16);

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

};

} // namespace KumirCodeRun

#endif // KUMIRCODERUN_VM_H
