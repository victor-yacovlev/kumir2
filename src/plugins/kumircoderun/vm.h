#ifndef KUMIRCODERUN_VM_H
#define KUMIRCODERUN_VM_H

#include <QtCore>
#include "variant.h"
#include "context.h"
#include "bytecode/tableelem.h"

namespace KumirCodeRun {

class VM : public QObject
{
    Q_OBJECT
public:
    explicit VM(QObject *parent = 0);
    void reset();
    void evaluateNextInstruction();
    inline bool hasMoreInstructions() const { return !stack_contexts.isEmpty(); }
    inline QString error() const { return s_error; }
private:
    QStack<Variant> stack_values;
    QStack<Context> stack_contexts;
    QVector<Variant> globals;
    QVector<Variant> constants;
    QMap< QPair<quint8,quint16>, Bytecode::TableElem > externs;
    QMap< QPair<quint8,quint16>, Bytecode::TableElem > functions;
    QMap< QPair<quint8,quint16>, QVector<Variant> > cleanLocalTables;
    QString s_error;

    QPair< QObject*, QMetaMethod > m_externalFuncHandler;
    QPair< QObject*, QMetaMethod > m_inputHandler;
    QPair< QObject*, QMetaMethod > m_outputHandler;
    QPair< QObject*, QMetaMethod > m_finputHandler;
    QPair< QObject*, QMetaMethod > m_foutputHandler;

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
