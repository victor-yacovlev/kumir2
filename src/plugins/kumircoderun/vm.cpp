#include "vm.h"
#include "bytecode/instruction.h"

#define EPSILON 0.0000001

using namespace Bytecode;

namespace KumirCodeRun {

VM::VM(QObject *parent) :
    QObject(parent)
{
    e_entryPoint = EP_Main;
}


void VM::reset()
{
    s_error = "";
    stack_values.clear();
    stack_contexts.clear();
    QList<TableElem*> inits;
    TableElem * testing;
    TableElem * aMain;
    QSet<QString> externModules;
    for (int i=0; i<functions.values().size(); i++) {
        const TableElem e = functions.values()[i];
        if (e.type==EL_INIT)
            inits << &(functions.values()[i]);
        if (e.type==EL_MAIN)
            aMain = &(functions.values()[i]);
        if (e.type==EL_TESTING)
            testing = &(functions.values()[i]);
    }
    for (int i=0; i<externs.values().size(); i++) {
        const TableElem e = externs.values()[i];
        externModules.insert(e.moduleName);
    }

    for (int i=0; i<externModules.toList().size(); i++) {
        const QString moduleName = externModules.toList()[i];
        emit resetModuleRequest(moduleName);
    }


    if (e_entryPoint==EP_Main && aMain) {
        Context c;
        quint32 mod = aMain->module;
        quint32 alg = aMain->algId;
        quint32 key = (mod << 16) | alg;
        c.locals = cleanLocalTables[key];
        c.program = aMain->instructions;
        c.IP = 0;
        c.type = EL_MAIN;
        c.lineNo = -1;
        stack_contexts.push(c);
    }

    if (e_entryPoint==EP_Testing && testing) {
        Context c;
        quint32 mod = testing->module;
        quint32 alg = testing->algId;
        quint32 key = (mod << 16) | alg;
        c.locals = cleanLocalTables[key];
        c.program = testing->instructions;
        c.IP = 0;
        c.type = EL_TESTING;
        c.lineNo = -1;
        stack_contexts.push(c);
    }

    for (int i=0; i<inits.size(); i++) {
        if (inits[i]->instructions.size()>0) {
            Context c;
            quint32 mod = inits[i]->module;
            quint32 alg = inits[i]->algId;
            quint32 key = (mod << 16) | alg;
            c.locals = cleanLocalTables[key];
            c.program = inits[i]->instructions;
            c.IP = 0;
            c.type = EL_INIT;
            c.lineNo = -1;
            stack_contexts.push(c);
        }
    }

}

Variant fromTableElem(const TableElem & e)
{
    Variant r;
    r.setDimension(e.dimension);
    r.setBaseType(e.vtype);
    if (e.type==EL_CONST)
        r.setValue(e.constantValue);
    return r;
}

void VM::loadProgram(const Data & program)
{
    externs.clear();
    functions.clear();
    cleanLocalTables.clear();
    QMap< quint64, QList<Variant> > locals;
    for (int i=0; i<program.d.size(); i++) {
        const TableElem e = program.d[i];
        if (e.type==EL_GLOBAL) {
            globals[e.id] = fromTableElem(e);
        }
        else if (e.type==EL_CONST) {
            constants[e.id] = fromTableElem(e);
        }
        else if (e.type==EL_LOCAL) {
            quint32 key = 0x00000000;
            quint32 alg = e.algId;
            quint32 mod = e.module;
            mod = mod << 16;
            key = mod | alg;
            QList<Variant> lcs;
            if (locals.contains(key))
                lcs = locals[key];
            lcs << fromTableElem(e);
            locals[key] = lcs;
        }
        else if (e.type==EL_EXTERN) {
            quint32 key = 0x00000000;
            quint32 alg = e.algId;
            quint32 mod = e.module;
            mod = mod << 16;
            key = mod | alg;
            externs[key] = e;
        }
        else if (e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_TESTING) {
            quint32 key = 0x00000000;
            quint32 alg = e.algId;
            quint32 mod = e.module;
            mod = mod << 16;
            key = mod | alg;
            functions[key] = e;
        }
    }
    for (int i=0; i<locals.keys().size(); i++) {
        quint32 key = locals.keys()[i];
        cleanLocalTables[key] = locals[key].toVector();
    }
    reset();
}


void VM::evaluateNextInstruction()
{
    int ip = stack_contexts.last().IP;
    QVector<Instruction> program = stack_contexts[stack_contexts.size()-1].program;
    Instruction instr = program[ip];
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
    quint32 module = mod << 16;
    quint32 algorhitm = alg;
    quint32 p = module | algorhitm;

    if (mod==255) {
        int argsCount = stack_values.pop().toInt();
        VariantList args;
        for (int i=0; i<argsCount; i++) {
            args << stack_values.pop();
        }
        // Special calls
        if (alg==0) {
            // Input
            const QString format = args.first().toString();
            QList<quintptr> references;
            for (int i=1; i<args.size(); i++) {
                references << quintptr(args[i].reference());
            }
            emit inputRequest(format, references);
        }
        if (alg==1) {
            // Output
            QString output;
            for (int i=0; i<args.size(); i++) {
                output += args[i].toString();
            }
            emit outputRequest(output);
        }
        if (alg==2) {
            // File input
            // TODO implement me
        }
        if (alg==3) {
            // File output

        }
    }
    else if (externs.contains(p)) {
        QList<quintptr> references;
        QVariantList arguments;
        int argsCount = stack_values.pop().toInt();
        for (int i=0; i<argsCount; i++) {
            arguments << stack_values.pop().value();
        }
        int refsCount = stack_values.pop().toInt();
        for (int i=0; i<refsCount; i++) {
            references << quintptr(stack_values.pop().reference());
        }
        const TableElem exportElem = externs[p];
        const QString pluginName = exportElem.moduleName;
        const QString algName = exportElem.name;
        emit invokeExternalFunction(pluginName, algName, arguments, references);

    }
    else if (functions.contains(p)) {
        Context c;
        c.IP = -1;
        c.program = functions[p].instructions ;
        c.locals = cleanLocalTables[p];
        c.type = EL_FUNCTION;
        c.lineNo = -1;
        stack_contexts.push(c);
        emit functionEntered();
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
    if (!stack_contexts.isEmpty()) {
        nextIP();
        emit lineNoChanged(stack_contexts[stack_contexts.size()-1].lineNo);
    }
    emit functionLeaved();
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
    emit lineNoChanged(no);
    stack_contexts[stack_contexts.size()-1].lineNo = no;
    nextIP();
}

void VM::do_sum()
{
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (a.baseType()==VT_int && a.baseType()==VT_int) {
        Variant r(a.toInt()+b.toInt());
        stack_values.push(r);
    }
    else if (a.baseType()==VT_float || b.baseType()==VT_float) {
        Variant r(a.toReal()+b.toReal());
        stack_values.push(r);
    }
    else if (a.baseType()==VT_string || a.baseType()==VT_char) {
        Variant r(a.toString()+b.toString());
        stack_values.push(r);
    }
    nextIP();
}

void VM::do_sub()
{
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (a.baseType()==VT_int && a.baseType()==VT_int) {
        Variant r(a.toInt()-b.toInt());
        stack_values.push(r);
    }
    else if (a.baseType()==VT_float || b.baseType()==VT_float) {
        Variant r(a.toReal()-b.toReal());
        stack_values.push(r);
    }
    nextIP();
}

void VM::do_mul()
{
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (a.baseType()==VT_int && a.baseType()==VT_int) {
        Variant r(a.toInt()*b.toInt());
        stack_values.push(r);
    }
    else if (a.baseType()==VT_float || b.baseType()==VT_float) {
        Variant r(a.toReal()*b.toReal());
        stack_values.push(r);
    }
    nextIP();
}

void VM::do_div()
{
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (b.baseType()==VT_int && b.toInt()==0) {
        s_error = tr("Division by zero");
    }
    else if (b.baseType()==VT_float && fabs(b.toReal()) < EPSILON) {
        s_error = tr("Division by zero");
    }
    else {
        Variant r(a.toReal()/b.toReal());
        stack_values.push(r);
    }
    nextIP();
}

void VM::do_pow()
{
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (b.toReal()<-EPSILON) {
        s_error = tr("Power to less than zero");
    }
    else {
        Variant r(pow(a.toReal(),b.toReal()));
        stack_values.push(r);
    }
    nextIP();
}

void VM::do_neg()
{
    Variant a = stack_values.pop();
    if (a.baseType()==VT_bool) {
        Variant r(!a.toBool());
        stack_values.push(r);
    }
    else if (a.baseType()==VT_int) {
        Variant r(-a.toInt());
        stack_values.push(r);
    }
    else if (a.baseType()==VT_float) {
        Variant r(0.0-a.toReal());
        stack_values.push(r);
    }
    nextIP();
}

void VM::do_and()
{
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (a.baseType()==VT_bool && b.baseType()==VT_bool) {
        Variant r(a.toBool() && b.toBool());
        stack_values.push(r);
    }
    nextIP();
}

void VM::do_or()
{
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (a.baseType()==VT_bool && b.baseType()==VT_bool) {
        Variant r(a.toBool() || b.toBool());
        stack_values.push(r);
    }
    nextIP();
}

void VM::do_eq()
{
    bool result = false;
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (b.baseType()==VT_int && a.baseType()==VT_int) {
        result = a.toInt()==b.toInt();
    }
    if (b.baseType()==VT_float && a.baseType()==VT_float) {
        result = fabs(a.toReal()-b.toReal())<EPSILON;
    }
    if (b.baseType()==VT_bool && a.baseType()==VT_bool) {
        result = a.toBool()==b.toBool();
    }
    if ( (a.baseType()==VT_string || a.baseType()==VT_char)
         &&
         (b.baseType()==VT_string || b.baseType()==VT_char)
         )
    {
        result = a.toString()==b.toString();
    }
    Variant r(result);
    stack_values.push(r);
    nextIP();
}

void VM::do_neq()
{
    bool result = false;
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (b.baseType()==VT_int && a.baseType()==VT_int) {
        result = a.toInt()==b.toInt();
    }
    if (b.baseType()==VT_float && a.baseType()==VT_float) {
        result = fabs(a.toReal()-b.toReal())<EPSILON;
    }
    if (b.baseType()==VT_bool && a.baseType()==VT_bool) {
        result = a.toBool()==b.toBool();
    }
    if ( (a.baseType()==VT_string || a.baseType()==VT_char)
         &&
         (b.baseType()==VT_string || b.baseType()==VT_char)
         )
    {
        result = a.toString()==b.toString();
    }
    Variant r(!result);
    stack_values.push(r);
    nextIP();
}

void VM::do_ls()
{
    bool result = false;
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (b.baseType()==VT_int && a.baseType()==VT_int) {
        result = a.toInt()<b.toInt();
    }
    if (b.baseType()==VT_float || a.baseType()==VT_float) {
        result = a.toReal()<b.toReal();
    }
    if (b.baseType()==VT_bool && a.baseType()==VT_bool) {
        result = a.toInt()<b.toInt();
    }
    if ( (a.baseType()==VT_string || a.baseType()==VT_char)
         &&
         (b.baseType()==VT_string || b.baseType()==VT_char)
         )
    {
        result = a.toString()<b.toString();
    }
    Variant r(result);
    stack_values.push(r);
    nextIP();
}

void VM::do_gt()
{
    bool result = false;
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (b.baseType()==VT_int && a.baseType()==VT_int) {
        result = a.toInt()>b.toInt();
    }
    if (b.baseType()==VT_float || a.baseType()==VT_float) {
        result = a.toReal()>b.toReal();
    }
    if (b.baseType()==VT_bool && a.baseType()==VT_bool) {
        result = a.toInt()>b.toInt();
    }
    if ( (a.baseType()==VT_string || a.baseType()==VT_char)
         &&
         (b.baseType()==VT_string || b.baseType()==VT_char)
         )
    {
        result = a.toString()>b.toString();
    }
    Variant r(result);
    stack_values.push(r);
    nextIP();
}

void VM::do_leq()
{
    bool result = false;
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (b.baseType()==VT_int && a.baseType()==VT_int) {
        result = a.toInt()<=b.toInt();
    }
    if (b.baseType()==VT_float || a.baseType()==VT_float) {
        result = a.toReal()<=b.toReal();
    }
    if (b.baseType()==VT_bool && a.baseType()==VT_bool) {
        result = a.toInt()<=b.toInt();
    }
    if ( (a.baseType()==VT_string || a.baseType()==VT_char)
         &&
         (b.baseType()==VT_string || b.baseType()==VT_char)
         )
    {
        result = a.toString()<=b.toString();
    }
    Variant r(result);
    stack_values.push(r);
    nextIP();
}

void VM::do_geq()
{
    bool result = false;
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (b.baseType()==VT_int && a.baseType()==VT_int) {
        result = a.toInt()>b.toInt();
    }
    if (b.baseType()==VT_float || a.baseType()==VT_float) {
        result = a.toReal()>b.toReal();
    }
    if (b.baseType()==VT_bool && a.baseType()==VT_bool) {
        result = a.toInt()>b.toInt();
    }
    if ( (a.baseType()==VT_string || a.baseType()==VT_char)
         &&
         (b.baseType()==VT_string || b.baseType()==VT_char)
         )
    {
        result = a.toString()>b.toString();
    }
    Variant r(result);
    stack_values.push(r);
    nextIP();
}

ElemType VM::topStackType() const
{
    if (stack_contexts.isEmpty())
        return EL_NONE;
    else
        return stack_contexts[stack_contexts.size()-1].type;
}

bool VM::canStepInto() const
{
    if (stack_contexts.isEmpty())
        return false;
    bool result = false;
    int ip = stack_contexts[stack_contexts.size()-1].IP+1;
    for ( ; ip<stack_contexts[stack_contexts.size()-1].program.size(); ip++) {
        Instruction instr = stack_contexts[stack_contexts.size()-1].program[ip];
        if (instr.type==LINE || instr.type==JUMP || instr.type==JNZ || instr.type==JZ || instr.type==RET)
            break;
        if (instr.type==CALL) {
            quint32 mod = instr.module;
            quint32 alg = instr.arg;
            quint32 key = (mod << 16) | alg;
            if (functions.contains(key)) {
                result = true;
                break;
            }
        }
    }
    return result;
}

} // namespace KumirCodeRun
