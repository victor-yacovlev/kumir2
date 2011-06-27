#include "vm.h"
#include "bytecode/instruction.h"

using namespace Bytecode;

namespace KumirCodeRun {

VM::VM(QObject *parent) :
    QObject(parent)
{
}

void VM::reset()
{
    stack_values.clear();
    stack_contexts.clear();
    globals.clear();
    constants.clear();
    externs.clear();
}


void VM::evaluateNextInstruction()
{
    int ip = stack_contexts.last().IP;
    Instruction instr = stack_contexts.last().program->at(ip);
    switch (instr.type) {
    case CALL:
        do_call(instr.module, instr.arg);
        break;
    case INIT:
        do_init(instr.scope, instr.arg);
        break;
    case SETARR:
        do_setarr(instr.scope, instr.arg);
        break;
    case STORE:
        do_store(instr.scope, instr.arg);
        break;
    case STOREARR:
        do_storearr(instr.scope, instr.arg);
        break;
    case LOAD:
        do_load(instr.scope, instr.arg);
        break;
    case LOADARR:
        do_loadarr(instr.scope, instr.arg);
        break;
    case JUMP:
        do_jump(instr.arg);
        break;
    case JNZ:
        do_jnz(instr.registerr, instr.arg);
        break;
    case JZ:
        do_jz(instr.registerr, instr.arg);
        break;
    case POP:
        do_pop(instr.registerr);
        break;
    case PUSH:
        do_push(instr.registerr);
        break;
    case RET:
        do_ret();
        break;
    case ERROR:
        do_error(instr.scope, instr.arg);
        break;
    case LINE:
        do_line(instr.arg);
        break;
    case REF:
        do_ref(instr.scope, instr.arg);
        break;
    case SUM:
        do_sum();
        break;
    case SUB:
        do_sub();
        break;
    case MUL:
        do_mul();
        break;
    case DIV:
        do_div();
        break;
    case POW:
        do_pow();
        break;
    case NEG:
        do_neg();
        break;
    case AND:
        do_and();
        break;
    case OR:
        do_or();
        break;
    case EQ:
        do_eq();
        break;
    case NEQ:
        do_neq();
        break;
    case LS:
        do_ls();
        break;
    case GT:
        do_gt();
        break;
    case LEQ:
        do_leq();
        break;
    case GEQ:
        do_geq();
        break;
    default:
        nextIP();
        break;
    }

}

void VM::do_call(quint8 mod, quint16 alg)
{
    const QPair<quint8, quint16> p(mod, alg);
    if (mod==255) {
        int argsCount = stack_values.pop().toInt();
        VariantList args;
        for (int i=0; i<argsCount; i++) {
            args << stack_values.pop();
        }
        // Special calls
        if (alg==0) {
            // Input
            // TODO implement me
        }
        if (alg==1) {
            // Output
            if (m_outputHandler.first) {
                m_outputHandler.second.invoke(
                            m_outputHandler.first,
                            Q_ARG(VariantList, args)
                            );
            }
        }
        if (alg==2) {
            // File input
            // TODO implement me
        }
        if (alg==3) {
            // File output
            if (m_foutputHandler.first) {
                m_foutputHandler.second.invoke(
                            m_foutputHandler.first,
                            Q_ARG(VariantList, args)
                            );
            }
        }
    }
    else if (externs.contains(p)) {
        int argsCount = stack_values.pop().toInt();
        VariantList args;
        for (int i=0; i<argsCount; i++) {
            args << stack_values.pop();
        }
        const TableElem exportElem = externs[p];
        const QString pluginName = exportElem.moduleName;
        const QString algName = exportElem.name;
        VariantList retval;
        if (m_externalFuncHandler.first) {
            m_externalFuncHandler.second.invoke(
                        m_externalFuncHandler.first,
                        Qt::DirectConnection,
                        Q_RETURN_ARG(VariantList, retval),
                        Q_ARG(QString, pluginName),
                        Q_ARG(QString, algName)
                        );
        }
        for (int i=0; i<retval.size(); i++) {
            stack_values.push(retval[i]);
        }
    }
    else if (functions.contains(p)) {
        Context c;
        c.IP = -1;
        c.program = & ( externs[p].instructions );
        c.locals = cleanLocalTables[p];
        stack_contexts.push(c);
    }
    else {
        s_error = tr("Internal error: don't know what is 'call %1 %2'").arg(mod).arg(alg);
    }
    nextIP();
}

void VM::do_init(quint8 s, quint16 id)
{
    if (VariableScope(s)==LOCAL) {
        stack_contexts[stack_contexts.size()-1].locals[id].init();
    }
    else if (VariableScope(s)==GLOBAL) {
        globals[id].init();
    }
    else {
        s_error = tr("Internal error: don't know what is 'init %1 %2'").arg(s).arg(id);
    }
    nextIP();
}

void VM::do_setarr(quint8 s, quint16 id)
{
    int dim = 0;
    QList<int> bounds;
    if (VariableScope(s)==LOCAL) {
        dim = stack_contexts[stack_contexts.size()-1].locals[id].dimension();
    }
    else if (VariableScope(s)==GLOBAL) {
        dim = globals[id].dimension();
    }
    else {
        s_error = tr("Internal error: don't know what is 'init %1 %2'").arg(s).arg(id);
    }
    if (dim>0) {
        for (int i=0; i<dim*2; i++) {
            bounds << stack_values.pop().toInt();
        }
        if (VariableScope(s)==LOCAL) {
            stack_contexts[stack_contexts.size()-1].locals[id].setBounds(bounds);
        }
        else if (VariableScope(s)==GLOBAL) {
            globals[id].setBounds(bounds);
        }
        s_error = Variant::error;
    }
    nextIP();
}

void VM::do_store(quint8 s, quint16 id)
{
    const Variant val = stack_values.top();
    if (VariableScope(s)==LOCAL) {
        stack_contexts[stack_contexts.size()-1].locals[id].setBaseType(val.baseType());
        stack_contexts[stack_contexts.size()-1].locals[id].setBounds(val.bounds());
        stack_contexts[stack_contexts.size()-1].locals[id].setValue(val.value());
    }
    else if (VariableScope(s)==GLOBAL) {
        globals[id].setBaseType(val.baseType());
        globals[id].setBounds(val.bounds());
        globals[id].setValue(val.value());
    }
    else {
        s_error = tr("Internal error: don't know what is 'store %1 %2'").arg(s).arg(id);
    }
    nextIP();
}

void VM::do_load(quint8 s, quint16 id)
{
    Variant val;
    if (VariableScope(s)==LOCAL) {
        val.setBaseType(stack_contexts[stack_contexts.size()-1].locals[id].baseType());
        val.setBounds(stack_contexts[stack_contexts.size()-1].locals[id].bounds());
        val.setValue(stack_contexts[stack_contexts.size()-1].locals[id].value());
    }
    else if (VariableScope(s)==GLOBAL) {
        val.setBaseType(globals[id].baseType());
        val.setBounds(globals[id].bounds());
        val.setValue(globals[id].value());
    }
    else if (VariableScope(s)==CONST) {
        val.setBaseType(constants[id].baseType());
        val.setBounds(constants[id].bounds());
        val.setValue(constants[id].value());
    }
    else {
        s_error = tr("Internal error: don't know what is 'load %1 %2'").arg(s).arg(id);
    }
    stack_values.push(val);
    nextIP();
}

void VM::do_storearr(quint8 s, quint16 id)
{
    int dim = 0;
    if (VariableScope(s)==LOCAL) {
        dim = stack_contexts[stack_contexts.size()-1].locals[id].dimension();
    }
    else if (VariableScope(s)==GLOBAL) {
        dim = globals[id].dimension();
    }
    else {
        s_error = tr("Internal error: don't know what is 'storearr %1 %2'").arg(s).arg(id);
    }
    if (dim>0) {
        QList<int> indeces;
        for (int i=0; i<dim; i++) {
            indeces << stack_values.pop().toInt();
        }
        const Variant val = stack_values.top();
        if (VariableScope(s)==LOCAL) {
            stack_contexts[stack_contexts.size()-1].locals[id].setValue(indeces, val.value());
        }
        else if (VariableScope(s)==GLOBAL) {
            globals[id].setValue(indeces, val.value());
        }
    }
    nextIP();
}

void VM::do_loadarr(quint8 s, quint16 id)
{
    int dim = 0;
    if (VariableScope(s)==LOCAL) {
        dim = stack_contexts[stack_contexts.size()-1].locals[id].dimension();
    }
    else if (VariableScope(s)==GLOBAL) {
        dim = globals[id].dimension();
    }
    else if (VariableScope(s)==CONST) {
        dim = constants[id].dimension();
    }
    else {
        s_error = tr("Internal error: don't know what is 'loadarr %1 %2'").arg(s).arg(id);
    }
    if (dim>0) {
        QList<int> indeces;
        for (int i=0; i<dim; i++) {
            indeces << stack_values.pop().toInt();
        }
        Variant val;
        if (VariableScope(s)==LOCAL) {
            val.setBaseType(stack_contexts[stack_contexts.size()-1].locals[id].baseType());
            val.setValue(stack_contexts[stack_contexts.size()-1].locals[id].value(indeces));
        }
        else if (VariableScope(s)==GLOBAL) {
            val.setBaseType(globals[id].baseType());
            val.setValue(globals[id].value(indeces));
        }
        else if (VariableScope(s)==CONST) {
            val.setBaseType(constants[id].baseType());
            val.setValue(constants[id].value(indeces));
        }
        stack_values.push(val);
    }
    nextIP();
}

void VM::do_ref(quint8 s, quint16 id)
{
    Variant ref;
    if (VariableScope(s)==LOCAL) {
        ref = stack_contexts[stack_contexts.size()-1].locals[id].toReference();
    }
    else if (VariableScope(s)==GLOBAL) {
        ref = globals[id].toReference();
    }
    else {
        s_error = tr("Internal error: don't know what is 'ref %1 %2'").arg(s).arg(id);
    }
    if (ref.isReference()) {
        stack_values.push(ref);
    }
    nextIP();
}

void VM::do_jump(quint16 ip)
{
    stack_contexts[stack_contexts.size()-1].IP = ip;
}

void VM::do_jnz(quint8 r, quint16 ip)
{
    bool v = stack_contexts[stack_contexts.size()-1].registers[r].toBool();
    if (v) {
        stack_contexts[stack_contexts.size()-1].IP = ip;
    }
    else {
        nextIP();
    }
}


void VM::do_jz(quint8 r, quint16 ip)
{
    bool v = stack_contexts[stack_contexts.size()-1].registers[r].toBool();
    if (v) {
        nextIP();
    }
    else {
        stack_contexts[stack_contexts.size()-1].IP = ip;
    }
}

void VM::do_push(quint8 r)
{
    QVariant v = stack_contexts[stack_contexts.size()-1].registers[r];
    if (v.type()==QVariant::Int) {
        stack_values << Variant(v.toInt());
    }
    else if (v.type()==QVariant::Double) {
        stack_values << Variant(v.toDouble());
    }
    else if (v.type()==QVariant::Bool) {
        stack_values << Variant(v.toBool());
    }
    else if (v.type()==QVariant::Char) {
        stack_values << Variant(v.toChar());
    }
    else if (v.type()==QVariant::String) {
        stack_values << Variant(v.toString());
    }
    nextIP();
}

void VM::do_pop(quint8 r)
{
    Variant v = stack_values.pop();
    if (v.baseType()==VT_int) {
        stack_contexts[stack_contexts.size()-1].registers[r] = v.toInt();
    }
    else if (v.baseType()==VT_float) {
        stack_contexts[stack_contexts.size()-1].registers[r] = v.toReal();
    }
    else if (v.baseType()==VT_bool) {
        stack_contexts[stack_contexts.size()-1].registers[r] = v.toBool();
    }
    else if (v.baseType()==VT_char) {
        stack_contexts[stack_contexts.size()-1].registers[r] = v.toChar();
    }
    else if (v.baseType()==VT_string) {
        stack_contexts[stack_contexts.size()-1].registers[r] = v.toString();
    }
    nextIP();
}

void VM::do_ret()
{
    stack_contexts.pop();
    if (!stack_contexts.isEmpty())
        nextIP();
}

void VM::do_error(quint8 s, quint16 id)
{
    if (VariableScope(s)==LOCAL) {
        s_error = stack_contexts[stack_contexts.size()-1].locals[id].toString();
    }
    else if (VariableScope(s)==GLOBAL) {
        s_error = globals[id].toString();
    }
    else if (VariableScope(s)==CONST) {
        s_error = constants[id].toString();
    }
}

void VM::do_line(quint16 no)
{
    // TODO implement me
}


} // namespace KumirCodeRun
