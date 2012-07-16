#include "vm.h"
#include "dataformats/bc_instruction.h"
#include "stdlib/integeroverflowchecker.h"
#include "stdlib/doubleoverflowchecker.h"
#include "stdlib/genericinputoutput.h"
#include "extensionsystem/kplugin.h"
#include "stdlib/kumstdlib.h"

#define EPSILON 0.0000001
#define MAX_RECURSION_SIZE 4000

using namespace Bytecode;

namespace KumirCodeRun {

VM::VM(QObject *parent) :
    QObject(parent)
{
    e_entryPoint = EP_Main;
    b_blindMode = false;
    m_dontTouchMe = new QMutex;
    stack_values.init(100, 100);
    stack_contexts.init(100, 100);
}

QString extractCorrectAlgName(const QString & kumirHeader) {
    QString t = kumirHeader;
    t = t.remove(0,3);
    t = t.trimmed();
    if (t.startsWith(QString::fromUtf8("цел"))||
            t.startsWith(QString::fromUtf8("вещ"))||
            t.startsWith(QString::fromUtf8("лог"))||
            t.startsWith(QString::fromUtf8("сим"))||
            t.startsWith(QString::fromUtf8("лит"))) {
        t = t.remove(0,3);
        t = t.trimmed();
    }
    int end = t.indexOf("(");
    if (end!=-1)
        t = t.left(end);
    return t.simplified();
}

void VM::setAvailableActors(const QList<ActorInterface *> &actors)
{
    availableExternalMethods.clear();
    foreach (ActorInterface * a, actors) {
        QStringList algorhitms = a->funcList();
        const QString name = a->name();
        for(int i=0; i<algorhitms.size(); i++) {
            const QString aName = extractCorrectAlgName(algorhitms[i]);
            const quint32 m = (quint32)i;
            QPair<QString,QString> key;
            key.first = name;
            key.second = aName;
            QPair<ActorInterface*,quint32> value;
            value.first = a;
            value.second = m;
            availableExternalMethods.insert(key, value);
        }
    }
}

void VM::updateStFunctError()
{
    if (s_error.length()==0)
        s_error = __get_error__st_funct();
}

void VM::setNextCallInto()
{
    b_nextCallInto = true;
}

void VM::setNextCallOut()
{
    if (stack_contexts.size()==0)
        return;
    stack_contexts.top().runMode = CRM_UntilReturn;
}

void VM::setNextCallToEnd()
{
    for (int i=0; i<stack_contexts.size(); i++) {
        stack_contexts.at(i).runMode = CRM_ToEnd;
    }
}

void VM::setNextCallStepOver()
{
    if (stack_contexts.size()==0)
        return;
    stack_contexts.top().runMode = CRM_OneStep;
}

int VM::currentLineNo() const
{
    QMutexLocker l(m_dontTouchMe);
    if (stack_contexts.size()==0)
        return -1;
    else
        return stack_contexts.top().lineNo;
}

void VM::setBlindMode(bool bl)
{
    b_blindMode = bl;
}

void VM::reset()
{
    last_context = Context();
    b_blindMode = false;
    b_nextCallInto = false;
    i_backtraceSkip = 0;
    s_error = "";
    register0 = QVariant::Invalid;
    Variant::error = "";
    Variant::ignoreUndefinedError = false;
//    stack_values.clear();
    stack_values.reset();
//    stack_contexts.clear();
    stack_contexts.reset();
    mainProgram.instructions.clear();
    testingProgram.instructions.clear();

    QSet<QString> externModules;
    for (int i=0; i<functions.values().size(); i++) {
        const TableElem e = functions.values()[i];
        if (e.type==EL_MAIN || e.type==EL_BELOWMAIN)
            mainProgram = functions.values()[i];
        if (e.type==EL_TESTING)
            testingProgram = functions.values()[i];
    }
    for (int i=0; i<externs.values().size(); i++) {
        const TableElem e = externs.values()[i];
        externModules.insert(e.moduleName);
    }

    for (int i=0; i<externModules.toList().size(); i++) {
        const QString moduleName = externModules.toList()[i];
        emit resetModuleRequest(moduleName);
    }


    if (e_entryPoint==EP_Main && (mainProgram.type==EL_MAIN || mainProgram.type==EL_BELOWMAIN) ) {
        Context c;
        quint32 mod = mainProgram.module;
        quint32 alg = mainProgram.algId;
        quint32 key = (mod << 16) | alg;
        c.locals = cleanLocalTables[key];
        c.program = &(mainProgram.instructions);
        c.type = mainProgram.type;
        c.runMode = CRM_ToEnd;
        c.algId = mainProgram.algId;
        c.moduleId = mainProgram.module;
        stack_contexts.push(c);
    }

    if (e_entryPoint==EP_Testing && testingProgram.type==EL_TESTING) {
        Context c;
        quint32 mod = testingProgram.module;
        quint32 alg = testingProgram.algId;
        quint32 key = (mod << 16) | alg;
        c.locals = cleanLocalTables[key];
        c.program = &(testingProgram.instructions);
        c.type = EL_TESTING;
        c.runMode = CRM_ToEnd;
        c.algId = testingProgram.algId;
        c.moduleId = testingProgram.module;
        stack_contexts.push(c);
    }

    for (int i=inits.size()-1; i>=0; i--) {
        quint8 key = inits.keys()[i];
        if (inits[key].instructions.size()>0) {
            Context c;
            c.program = &(inits[key].instructions);
            c.type = EL_INIT;
            c.runMode = CRM_ToEnd;
            c.moduleId = inits[key].module;
            c.algId = -1;
            stack_contexts.push(c);
        }
    }
    nextIP(); // Change from -1 to 0
}

void VM::nextIP()
{
    if (stack_contexts.size()>0) {
        stack_contexts.top().IP ++;
    }
}

Variant fromTableElem(const TableElem & e)
{
    Variant r;
    r.setDimension(e.dimension);
    r.setBaseType(e.vtype);
    r.setName(e.name);
    if (e.type==EL_CONST)
        r.setValue(e.constantValue);
    return r;
}

void VM::loadProgram(const Data & program)
{
    externs.clear();
    externalMethods.clear();
    functions.clear();
    cleanLocalTables.clear();
    inits.clear();
    QMap< quint64, QList<Variant> > locals;
    for (int i=0; i<program.d.size(); i++) {
        const TableElem e = program.d[i];
        if (e.type==EL_GLOBAL) {
            globals[QPair<quint8,quint16>(e.module,e.id)] = fromTableElem(e);
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
            QPair<QString,QString> qualifedKey;
            qualifedKey.first = e.moduleName;
            qualifedKey.second = e.name;
            Q_ASSERT(availableExternalMethods.contains(qualifedKey));
            externalMethods.insert(key, availableExternalMethods.value(qualifedKey));
        }
        else if (e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_BELOWMAIN || e.type==EL_TESTING) {
            quint32 key = 0x00000000;
            quint32 alg = e.algId;
            quint32 mod = e.module;
            mod = mod << 16;
            key = mod | alg;
            functions[key] = e;
        }
        else if (e.type==EL_INIT ) {
            quint8 key = e.module;
            inits[key] = e;
        }
    }
    for (int i=0; i<functions.keys().size(); i++) {
        quint32 key = functions.keys()[i];
        const QString algName = functions[key].name;
        if (locals.contains(key)) {
            for (int j=0; j<locals[key].size(); j++) {
                locals[key][j].setAlgorhitmName(algName);
            }
        }
    }
    for (int i=0; i<locals.keys().size(); i++) {
        quint32 key = locals.keys()[i];
        cleanLocalTables[key] = locals[key].toVector();
    }
//    reset();
}

QStringList VM::usedActors() const
{
    QStringList result;
    for (int i=0; i<externs.values().size(); i++) {
        const QString moduleName = externs.values()[i].moduleName;
        result << moduleName;
    }
    return result;
}

void VM::setLocalVariableValue(int localId, const QVariant &value)
{
    QMutexLocker l(m_dontTouchMe);
    Q_ASSERT(stack_contexts.size());
    Q_ASSERT(localId < stack_contexts.top().locals.size());
    stack_contexts.top().locals[localId].setValue(value);
}

void VM::evaluateNextInstruction()
{

    int ip = stack_contexts.top().IP;
    const QVector<Instruction> * program = stack_contexts.top().program;
    const Instruction instr = program->at(ip);
//    qDebug() << "Evaluating " << ip << ": " << instructionToString(instr);
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
    case UPDARR:
        do_updarr(instr.scope, instr.arg);
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
    case REFARR:
        do_refarr(instr.scope, instr.arg);
        break;
    case SETREF:
        do_setref(instr.scope, instr.arg);
        break;
    case CTL:
        do_ctl(instr.module, instr.arg);
        break;
    case INRANGE:
        do_inrange();
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
    case SHOWREG:
        do_showreg(instr.registerr);
        break;
    case CLEARMARG:
        do_clearmarg(instr.arg);
        break;
    case PAUSE:
        do_pause(instr.arg);
        break;
    case HALT:
        do_halt(instr.arg);
        break;
    default:
        nextIP();
        break;
    }

}

void VM::do_pause(quint16 lineNo)
{
    QMutexLocker l(m_dontTouchMe);
    stack_contexts.top().runMode = CRM_OneStep;
    emit lineNoChanged(lineNo);
    emit pauseRequest();
    emit lineNoChanged(lineNo);
    nextIP();
}

void VM::do_halt(quint16)
{
    QMutexLocker l(m_dontTouchMe);
//    if (lineNo==65535)
        emit outputRequest("\n"+tr("STOP."));
//    else
//        emit outputRequest("\n"+tr("STOP AT LINE %1.").arg(lineNo));
    stack_contexts.reset();
}

int VM::effectiveLineNo() const
{
    if (stack_contexts.size()==0)
        return -1;
    int index = qMax(0, int(stack_contexts.size()-1 - i_backtraceSkip));
    return stack_contexts.at(index).lineNo;
}

void VM::do_ctl(quint8 parameter, quint16 value)
{
    if (parameter==0x00) {
        Variant::ignoreUndefinedError = value>0;
    }
    else if (parameter==0x01) {
        i_backtraceSkip = value;
    }
    nextIP();
}

void VM::do_inrange()
{
    Q_ASSERT(stack_values.size()>=4);
    Variant value = stack_values.pop();
    Variant to = stack_values.pop();
    Variant from = stack_values.pop();
    Variant step = stack_values.pop();

    int iValue = value.toInt();
    int iStep = step.toInt();
    int iFrom = from.toInt();
    int iTo = to.toInt();
    bool res = iStep>=0
            ? (iFrom <= iValue) && (iValue <= iTo)
            : (iFrom >= iValue) && (iValue >= iTo);
    register0.setValue(res);
//    stack_contexts.top().registers[0].setValue(QVariant(res));
    nextIP();
}

void VM::do_call(quint8 mod, quint16 alg)
{
    quint32 module = mod << 16;
    quint32 algorhitm = alg;
    quint32 p = module | algorhitm;

    if (mod==0xFF) {
        int argsCount = stack_values.pop().toInt();
        // Special calls
        if (alg==0x00) {
            // Input
            const QString format = stack_values.pop().toString();
            QList<quintptr> references;
            QList<int> indeces;
            for (int i=1; i<argsCount; i++) {
                Variant ref = stack_values.pop();
                references << quintptr(ref.reference());
                indeces << ref.referenceIndeces();
            }
            if (f_input.isOpen()) {
                StdLib::GenericInputOutput * inp = StdLib::GenericInputOutput::instance();
                inp->doInput(format);
                QSet<QPair<int,int> > errpos;
                QString error;
                QList<QVariant> result;
                if (f_input.atEnd()) {
                    s_error = tr("File %1 reached end").arg(f_input.fileName());
                    return;
                }
                QString line = QString::fromLocal8Bit(f_input.readLine()).trimmed();
                if (line.isEmpty()) {
                    s_error = tr("Current line of file %1 is empty").arg(f_input.fileName());
                    return;
                }
                bool ok = inp->tryFinishInput(line, result, errpos, false, error);
                if (!ok) {
                    s_error = tr("Input from file %1 error: %2")
                            .arg(f_input.fileName())
                            .arg(error);
                }
                else {
                    if (result.size()>references.size()) {
                        result = result.mid(0,references.size());
                    }
                    setResults("",references,indeces,result);
                }

            }
            else {
                emit inputRequest(format, references, indeces);
            }
        }
        if (alg==0x01) {
            // Output
            QString output;
            QVariantList outArgs;
            std::string format;
            for (int i=0; i<argsCount; i++) {
                Variant v = stack_values.pop();
                if (v.baseType()==VT_int) {
                    if (i%3==0) format += 'i';
                    outArgs << v.toInt();
                }
                else if (v.baseType()==VT_float) {
                    if (i%3==0) format += 'r';
                    outArgs << v.toReal();
                }
                else if (v.baseType()==VT_bool) {
                    if (i%3==0) format += 'b';
                    outArgs << v.toBool();
                }
                else if (v.baseType()==VT_string) {
                    if (i%3==0) format += 's';
                    outArgs << v.toString();
                }
                else if (v.baseType()==VT_char) {
                    if (i%3==0) format += 'c';
                    outArgs << v.toString();
                }
            }
            output = StdLib::GenericInputOutput::prepareOutput(format, outArgs, s_error);
            if (s_error.isEmpty()) {
                if (f_output.isOpen()) {
                    f_output.write(output.toLocal8Bit());
                }
                else {
                    emit outputRequest(output);
                }
            }
        }
        else if (alg==0x02) {
            // File input
            QString format = stack_values.pop().toString();
            int handle = stack_values.pop().toInt();
            format.remove("%");
            QList<quintptr> references;
            QList<int> indeces;
            for (int i=0; i<format.length(); i++) {
                Variant ref = stack_values.pop();
                references << quintptr(ref.reference());
                indeces << ref.referenceIndeces();
            }
            Q_ASSERT(references.count()==format.length());
            QList<QVariant> result;
            for (int i=0; i<format.length(); i++) {
                if (format[i]=='s') {
                    const wchar_t * r = __finput2_string__st_funct(handle);
                    result << QVariant(QString::fromWCharArray(r));
                }
                else if (format[i]=='c')
                    result << QVariant(QChar(__finput2_char__st_funct(handle)));
                else {
                    union IntRealBool r = __finput2_int_real_bool__st_funct(handle, format[i].toAscii());
                    if (format[i]=='b')
                        result << QVariant(bool(r.boolValue));
                    else if (format[i]=='d')
                        result << QVariant(r.intValue);
                    else
                        result << QVariant(r.realValue);
                }
                s_error = __get_error__st_funct();
                if (s_error.length()>0)
                    break;
            }
            if (s_error.length()==0) {
                if (result.size()>references.size()) {
                    result = result.mid(0,references.size());
                }
                setResults("",references,indeces,result);
            }
        }
        else if (alg==0x03) {
            int handle = stack_values.pop().toInt();
            QString output;
            QVariantList outArgs;
            std::string format;
            for (int i=0; i<argsCount-1; i++) {
                Variant v = stack_values.pop();
                if (v.baseType()==VT_int) {
                    if (i%3==0) format += 'i';
                    outArgs << v.toInt();
                }
                else if (v.baseType()==VT_float) {
                    if (i%3==0) format += 'r';
                    outArgs << v.toReal();
                }
                else if (v.baseType()==VT_bool) {
                    if (i%3==0) format += 'b';
                    outArgs << v.toBool();
                }
                else if (v.baseType()==VT_string) {
                    if (i%3==0) format += 's';
                    outArgs << v.toString();
                }
                else if (v.baseType()==VT_char) {
                    if (i%3==0) format += 'c';
                    outArgs << v.toString();
                }
            }
            output = StdLib::GenericInputOutput::prepareOutput(format, outArgs, s_error);
            wchar_t * buffer = (wchar_t*)calloc(output.length()+1, sizeof(wchar_t));
            output.toWCharArray(buffer);
            buffer[output.length()] = L'\0';
            if (s_error.isEmpty())
                __foutput2__st_funct(handle, buffer);
            free(buffer);

        }
        else if (alg==0x04) {
            // Get char from string
            Variant second = stack_values.pop();
            Variant first = stack_values.pop();
            int index = second.value().toInt();
            QString s = first.value().toString();
            s_error = Variant::error;
            if (s_error.isEmpty()) {
                if (index<1 || index>s.length()) {
                    s_error = tr("Index out of string");
                }
                else {
                    QChar result = s[index-1];
                    Variant r(result);
                    stack_values.push(r);
                }
            }
        }
        else if (alg==0x05) {
            // Set char in string
            Variant third = stack_values.pop();
            Variant second = stack_values.pop();
            Variant first = stack_values.pop();
            int index = third.value().toInt();
            QString source = second.value().toString();
            QString ch = first.value().toString();
            s_error = Variant::error;
            if (s_error.isEmpty()) {
                if (index<1 || index>source.length()) {
                    s_error = tr("Index out of string");
                }
                else {
                    source = source.mid(0,index-1)+ch+source.mid(index);
                    Variant r(source);
                    stack_values.push(r);
                }
            }
        }
        else if (alg==0x06) {
            // Get slice from string
            Variant third = stack_values.pop();
            Variant second = stack_values.pop();
            Variant first = stack_values.pop();
            int start = second.value().toInt();
            int end   = third.value().toInt();
            QString s = first.value().toString();
            s_error = Variant::error;
            if (s_error.isEmpty()) {
                if (start<1 || start>s.length()) {
                    s_error = tr("Index out of string");
                }
                else if (end<1 || end>s.length()) {
                    s_error = tr("Index out of string");
                }
                else if (end<start) {
                    s_error = tr("Invalid string slice");
                }
                else {
                    QString result = s.mid(start-1, end-start+1);
                    Variant r(result);
                    stack_values.push(r);
                }
            }
        }
        else if (alg==0x07) {
            // Set slice in string
            Variant fourth = stack_values.pop();
            Variant third = stack_values.pop();
            Variant second = stack_values.pop();
            Variant first = stack_values.pop();
            int end = fourth.value().toInt();
            int start = third.value().toInt();
            QString source = second.value().toString();
            QString ch = first.value().toString();
            s_error = Variant::error;
            if (s_error.isEmpty()) {
                if (start<1 || start>source.length()) {
                    s_error = tr("Index out of string");
                }
                else if (end<1 || end>source.length()) {
                    s_error = tr("Index out of string");
                }
                else if (end<start) {
                    s_error = tr("Invalid string slice");
                }
                else {
                    source = source.mid(0,start-1)+ch+source.mid(end);
                    Variant r(source);
                    stack_values.push(r);
                }
            }
        }
        else if (alg==0x0A01) {
            Variant sn = stack_values.pop();
            Variant fn = stack_values.pop();
            const QString fileName = fn.toString();
            const QString streamName = sn.toString().toLower().trimmed();
            if (streamName=="stdin") {
                f_input.close();
                if (!fileName.isEmpty()) {
                    f_input.setFileName(fileName);
                    if (!f_input.exists()) {
                        s_error = tr("File not exists: %1")
                                .arg(QFileInfo(f_input).absoluteFilePath());
                    }
                    else {
                        if (!f_input.open(QIODevice::ReadOnly)) {
                            s_error = tr("Can't open file %1: Access denied")
                                    .arg(QFileInfo(f_input).absoluteFilePath());
                        }
                    }
                }
            }
            else if (streamName=="stdout") {
                f_output.close();
                if (!fileName.isEmpty()) {
                    QDir d = QFileInfo(f_output).dir();
                    if (!d.exists()) {
                        if (!QDir::root().mkpath(d.absolutePath())) {
                            s_error = tr("Can't create directory %1: Access denied")
                                    .arg(d.absolutePath());
                        }
                    }
                    if (d.exists()) {
                        if (!f_output.open(QIODevice::WriteOnly)) {
                            s_error = tr("Can't open file %1 for writing: Access denied")
                                    .arg(QFileInfo(f_output).absoluteFilePath());
                        }
                    }
                }
            }
        }
        else if (alg==0xBB01) {
            m_dontTouchMe->lock();
            // Input argument
            int localId = argsCount; // Already removed from stack
            Q_ASSERT (localId < stack_contexts.top().locals.size());
            const QString & varName = stack_contexts.top().locals[localId].name();
            QString varFormat;
            Bytecode::ValueType baseType = stack_contexts.top().locals[localId].baseType();
            if (baseType==Bytecode::VT_int)
                varFormat = "%d";
            else if (baseType==Bytecode::VT_char)
                varFormat = "%c";
            else if (baseType==Bytecode::VT_float)
                varFormat = "%f";
            else if (baseType==Bytecode::VT_string)
                varFormat = "%s";
            else if (baseType==Bytecode::VT_bool)
                varFormat = "%b";
            QList<int> bounds = stack_contexts.top().locals[localId].bounds();
            m_dontTouchMe->unlock();
            emit inputArgumentRequest(localId, varName, varFormat, bounds);
        }
        else if (alg==0xBB02) {
            m_dontTouchMe->lock();
            // Output argument or return value
            int localId = argsCount; // Already removed from stack
            Q_ASSERT (localId < stack_contexts.top().locals.size());
            const QString & varName = stack_contexts.top().locals[localId].name();
            QList<int> bounds = stack_contexts.top().locals[localId].bounds();
            QVariant value = QVariant::Invalid;
            if (stack_contexts.top().locals[localId].hasValue())
                value = stack_contexts.top().locals[localId].value();
            m_dontTouchMe->unlock();
            emit outputArgumentRequest(value, varName, bounds);
        }
    }
    else if (externalMethods.contains(p)) {
        QPair<ActorInterface*,quint32> methodValue = externalMethods[p];
        call_externalMethod(methodValue.first, methodValue.second);
//        QList<quintptr> references;
//        QVariantList arguments;
//        QList<int> indeces;
//        int argsCount = stack_values.pop().toInt();
//        for (int i=0; i<argsCount; i++) {
//            arguments << stack_values.pop().value();
//        }
//        int refsCount = stack_values.pop().toInt();
//        for (int i=0; i<refsCount; i++) {
//            references << quintptr(stack_values.pop().reference());
//        }
//        const TableElem exportElem = externs[p];
//        const QString pluginName = exportElem.moduleName;
//        const QString algName = exportElem.name;
//        emit invokeExternalFunction(pluginName, algName, arguments, references, indeces);

    }
    else if (functions.contains(p)) {
        if (stack_contexts.size()>=MAX_RECURSION_SIZE) {
            s_error = tr("Too deep recursion");
        }
        else {
            m_dontTouchMe->lock();
            Context c;
            c.program = & (functions[p].instructions );
            c.locals = cleanLocalTables[p];
            c.type = functions[p].type;
            if (b_nextCallInto)
                c.runMode = CRM_OneStep;
            else if (stack_contexts.top().type==EL_BELOWMAIN && c.type==EL_MAIN)
                c.runMode = stack_contexts.top().runMode;
            else
                c.runMode = CRM_ToEnd;
            c.moduleId = functions[p].module;
            c.algId = functions[p].algId;
            stack_contexts.push(c);
            b_nextCallInto = false;
            m_dontTouchMe->unlock();
        }
    }
    else {
        s_error = tr("Internal error: don't know what is 'call %1 %2'").arg(mod).arg(alg);
    }
    nextIP();
}


void VM::call_externalMethod(ActorInterface *act, quint32 method)
{
    Q_CHECK_PTR(act);
    m_dontTouchMe->lock();
    int argsCount = stack_values.pop().toInt();
    QVariantList args;
    for (int i=0; i<argsCount; i++) {
        args << stack_values.pop().value();
    }
    m_dontTouchMe->unlock();
    emit beforeExternalFunction();
    EvaluationStatus status = act->evaluate(method, args);
    m_dontTouchMe->lock();
    int refsCount = stack_values.pop().toInt();
    QList<quintptr> references;
    QList<int> indeces;
    for (int i=0; i<refsCount; i++) {
        references << quintptr(stack_values.pop().reference());
    }
    m_dontTouchMe->unlock();
    if (status==ES_Error) {
        s_error = act->errorText();
        return;
    }
    if (status==ES_StackResult || status==ES_StackRezResult) {
        m_dontTouchMe->lock();
        pushValueToStack(act->result());
        m_dontTouchMe->unlock();
    }
    if (status==ES_RezResult || status==ES_StackRezResult) {
        setResults("", references, indeces, act->algOptResults());
    }
    if (status==ES_Async) {
        emit invokeExternalFunction(references);
    }
}

void VM::do_init(quint8 s, quint16 id)
{
    QMutexLocker l(m_dontTouchMe);
    if (VariableScope(s)==LOCAL) {
        stack_contexts.top().locals[id].init();
    }
    else if (VariableScope(s)==GLOBAL) {
        globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].init();
    }
    else {
        s_error = tr("Internal error: don't know what is 'init %1 %2'").arg(s).arg(id);
    }
    nextIP();
}

void VM::do_setarr(quint8 s, quint16 id)
{
    QMutexLocker l(m_dontTouchMe);
    int dim = 0;
    QList<int> bounds;
    if (VariableScope(s)==LOCAL) {
        dim = stack_contexts.top().locals[id].dimension();
    }
    else if (VariableScope(s)==GLOBAL) {
        dim = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].dimension();
    }
    else {
        s_error = tr("Internal error: don't know what is 'init %1 %2'").arg(s).arg(id);
    }
    if (dim>0) {
        QString name;
        for (int i=0; i<dim*2; i++) {
            bounds << stack_values.pop().toInt();
        }
        if (VariableScope(s)==LOCAL) {
            stack_contexts.top().locals[id].setBounds(bounds);
            name = stack_contexts.top().locals[id].name();
        }
        else if (VariableScope(s)==GLOBAL) {
            globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].setBounds(bounds);
            name = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].name();
        }
        s_error = Variant::error;
        if (!b_blindMode && s_error.isEmpty()) {

        }
        const int lineNo = stack_contexts.top().lineNo;
        if (lineNo!=-1 &&
//                (stack_contexts.top().runMode==CRM_OneStep || stack_contexts.top().type==EL_MAIN) &&
                !b_blindMode &&
                stack_contexts.top().type != EL_BELOWMAIN
                )
        {
            QString boundsText;
            for (int i=0; i<dim; i++) {
                boundsText += QString::number(bounds[i*2]);
                boundsText += ":";
                boundsText += QString::number(bounds[i*2+1]);
                if (i<dim-1) {
                    boundsText += ",";
                }
            }
            emit valueChangeNotice(lineNo, name+"["+boundsText+"]");
        }
    }
    nextIP();
}

void VM::do_updarr(quint8 s, quint16 id)
{
    QMutexLocker l(m_dontTouchMe);
    int dim = 0;
    QList<int> bounds;
    if (VariableScope(s)==LOCAL) {
        dim = stack_contexts.top().locals[id].dimension();
    }
    else if (VariableScope(s)==GLOBAL) {
        dim = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].dimension();
    }
    else {
        s_error = tr("Internal error: don't know what is 'init %1 %2'").arg(s).arg(id);
    }
    if (dim>0) {
        QString name;
        QList<int> effectiveBounds;
        for (int i=0; i<dim*2; i++) {
            bounds << stack_values.pop().toInt();
        }
        if (VariableScope(s)==LOCAL) {
            stack_contexts.top().locals[id].updateBounds(bounds);
            name = stack_contexts.top().locals[id].myName();
            effectiveBounds = stack_contexts.top().locals[id].effectiveBounds();
        }
        else if (VariableScope(s)==GLOBAL) {
            globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].updateBounds(bounds);
            name = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].myName();
            effectiveBounds = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].effectiveBounds();
        }
        s_error = Variant::error;
        if (!b_blindMode && s_error.isEmpty()) {

        }
        const int lineNo = stack_contexts.top().lineNo;
        if (lineNo!=-1 &&
                //(stack_contexts.top().runMode==CRM_OneStep || stack_contexts.top().type==EL_MAIN) &&
                !b_blindMode &&
                stack_contexts.top().type != EL_BELOWMAIN
                )
        {
            QString boundsText;

            for (int i=0; i<dim; i++) {
                boundsText += QString::number(effectiveBounds[i*2]);
                boundsText += ":";
                boundsText += QString::number(effectiveBounds[i*2+1]);
                if (i<dim-1) {
                    boundsText += ",";
                }
            }
            emit valueChangeNotice(lineNo, name+"["+boundsText+"]");
        }
    }
    nextIP();
}

void VM::do_store(quint8 s, quint16 id)
{
    QMutexLocker l(m_dontTouchMe);
    const Variant val = stack_values.top();
    QString name;
    QString svalue;
    const int lineNo = stack_contexts.top().lineNo;
    ValueType t = VT_void;
    Variant * reference = 0;
    if (VariableScope(s)==LOCAL) {
//        stack_contexts.top().locals[id].setBaseType(val.baseType());
        if (stack_contexts.top().locals[id].isReference())
            reference = stack_contexts.top().locals[id].reference();
        stack_contexts.top().locals[id].setBounds(val.bounds());
        stack_contexts.top().locals[id].setValue(val.value());
        if (lineNo!=-1 && !b_blindMode) {
            name = stack_contexts.top().locals[id].myName();
            svalue = stack_contexts.top().locals[id].toString();
        }
        t = stack_contexts.top().locals[id].baseType();
    }
    else if (VariableScope(s)==GLOBAL) {
//        globals[id].setBaseType(val.baseType());
        if (globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].isReference())
            reference = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].reference();
        globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].setBounds(val.bounds());
        globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].setValue(val.value());
        if (lineNo!=-1 && !b_blindMode) {
            name = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].myName();
            svalue = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].toString();
        }
        t = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].baseType();
    }
    else {
        s_error = tr("Internal error: don't know what is 'store %1 %2'").arg(s).arg(id);
    }
    if (lineNo!=-1 &&
//            (stack_contexts.top().runMode==CRM_OneStep || stack_contexts.top().type==EL_MAIN) &&
            !b_blindMode &&
            stack_contexts.top().type != EL_BELOWMAIN &&
            val.dimension()==0
            )
    {
        if (t==VT_string)
            svalue = "\""+svalue+"\"";
        if (t==VT_char)
            svalue = "'"+svalue+"'";
        QString qn;
        if (reference && !reference->algorhitmName().startsWith("@")) {
            qn = reference->algorhitmName().isEmpty()
                            ? reference->name()
                            : reference->algorhitmName()+"::"+reference->name();
            qn += "=";
        }
        emit valueChangeNotice(lineNo, qn+name+"="+svalue);
    }
    nextIP();
}


void VM::do_load(quint8 s, quint16 id)
{
    QMutexLocker l(m_dontTouchMe);
    Variant val;
    if (VariableScope(s)==LOCAL) {
        val.setBaseType(stack_contexts.top().locals[id].baseType());
        val.setBounds(stack_contexts.top().locals[id].bounds());
        val.setValue(stack_contexts.top().locals[id].value());
    }
    else if (VariableScope(s)==GLOBAL) {
        val.setBaseType(globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].baseType());
        val.setBounds(globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].bounds());
        val.setValue(globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].value());
    }
    else if (VariableScope(s)==CONST) {
        val.setBaseType(constants[id].baseType());
        val.setBounds(constants[id].bounds());
        val.setValue(constants[id].value());
    }
    else {
        s_error = tr("Internal error: don't know what is 'load %1 %2'").arg(s).arg(id);
    }
    bool isRetVal = VariableScope(s)==LOCAL
            && stack_contexts.top().locals[id].algorhitmName()==stack_contexts.top().locals[id].name();
    if (!Variant::error.isEmpty() && !isRetVal)
        s_error = Variant::error;
    stack_values.push(val);
    if (val.dimension()==0)
//        stack_contexts.top().registers[0] = val.value();
        register0 = val.value();
    nextIP();
}

void VM::do_storearr(quint8 s, quint16 id)
{
    QMutexLocker l(m_dontTouchMe);
    int dim = 0;
    QString name;
    QString svalue;
    const int lineNo = stack_contexts.top().lineNo;
    QString sindeces;
    if (VariableScope(s)==LOCAL) {
        dim = stack_contexts.top().locals[id].dimension();
        name = stack_contexts.top().locals[id].name();
    }
    else if (VariableScope(s)==GLOBAL) {
        dim = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].dimension();
        name = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].name();
    }
    else {
        s_error = tr("Internal error: don't know what is 'storearr %1 %2'").arg(s).arg(id);
    }
    if (dim>0) {
        QList<int> indeces;
        for (int i=0; i<dim; i++) {
            indeces << stack_values.pop().toInt();
            if (!sindeces.isEmpty())
                sindeces += ",";
            sindeces += QString::number(indeces.last());
        }
        const Variant val = stack_values.top();
        ValueType t = VT_void;
        svalue = val.toString();
        if (VariableScope(s)==LOCAL) {
            stack_contexts.top().locals[id].setValue(indeces, val.value());
            t = stack_contexts.top().locals[id].baseType();
            if (lineNo!=-1 && !b_blindMode)
                svalue = stack_contexts.top().locals[id].toString(indeces);
        }
        else if (VariableScope(s)==GLOBAL) {
            globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].setValue(indeces, val.value());
            t = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].baseType();
            if (lineNo!=-1 && !b_blindMode)
                svalue = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].toString(indeces);
        }
        if (t==VT_string)
            if (lineNo!=-1 && !b_blindMode)
                svalue = "\""+svalue+"\"";
        if (t==VT_char)
            if (lineNo!=-1 && !b_blindMode)
                svalue = "'"+svalue+"'";
        if (!Variant::error.isEmpty())
            s_error = Variant::error;
    }
    if (lineNo!=-1 &&
//            (stack_contexts.top().runMode==CRM_OneStep || stack_contexts.top().type==EL_MAIN) &&
            !b_blindMode
            )
        emit valueChangeNotice(lineNo, name+"["+sindeces+"]="+svalue);
    nextIP();
}

void VM::do_loadarr(quint8 s, quint16 id)
{
    QMutexLocker l(m_dontTouchMe);
    int dim = 0;
    ValueType vt = VT_void;
    if (VariableScope(s)==LOCAL) {
        dim = stack_contexts.top().locals[id].dimension();
        vt = stack_contexts.top().locals[id].baseType();
    }
    else if (VariableScope(s)==GLOBAL) {
        dim = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].dimension();
        vt = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].baseType();
    }
    else if (VariableScope(s)==CONST) {
        dim = constants[id].dimension();
        vt = constants[id].baseType();
    }
    else {
        s_error = tr("Internal error: don't know what is 'loadarr %1 %2'").arg(s).arg(id);
    }

    if (dim>0 || vt==VT_string) {
        QList<int> indeces;
        for (int i=0; i<dim; i++) {
            indeces << stack_values.pop().toInt();
        }
        Variant val;
        QVariant vv;
        if (VariableScope(s)==LOCAL) {
            val.setBaseType(stack_contexts.top().locals[id].baseType());
            vv = stack_contexts.top().locals[id].value(indeces);
        }
        else if (VariableScope(s)==GLOBAL) {
            val.setBaseType(globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].baseType());
            vv = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].value(indeces);
        }
        else if (VariableScope(s)==CONST) {
            val.setBaseType(constants[id].baseType());
            vv = constants[id].value(indeces);
        }
        if (vv.isValid()) {
            val.setValue(vv);
            stack_values.push(val);
            if (val.baseType()==VT_int)
                register0 = val.toInt();
            else if (val.baseType()==VT_float)
                register0 = val.toReal();
            else if (val.baseType()==VT_char)
                register0 = val.toChar();
            else if (val.baseType()==VT_string)
                register0 = val.toString();
            else if (val.baseType()==VT_bool)
                register0 = val.toBool();
        }
    }
    if (!Variant::error.isEmpty())
        s_error = Variant::error;
    nextIP();
}

void VM::do_ref(quint8 s, quint16 id)
{
    QMutexLocker l(m_dontTouchMe);
    Variant ref;
    if (VariableScope(s)==LOCAL) {
        ref = stack_contexts.top().locals[id].toReference();
    }
    else if (VariableScope(s)==GLOBAL) {
        ref = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].toReference();
    }
    else {
        s_error = tr("Internal error: don't know what is 'ref %1 %2'").arg(s).arg(id);
    }
    if (ref.isReference()) {
        stack_values.push(ref);
    }
    nextIP();
}

void VM::do_setref(quint8 s, quint16 id)
{
    QMutexLocker l(m_dontTouchMe);
    Variant ref = stack_values.top();
    QString name;
    if (!ref.isReference()) {
        s_error = tr("Internal error: trying to setref not a reference: 'setref %1 %2'").arg(s).arg(id);
    }
    else if (VariableScope(s)==LOCAL) {
        name = stack_contexts.top().locals[id].name();
        stack_contexts.top().locals[id].setReference(ref.reference(), ref.effectiveBounds());
    }
    else if (VariableScope(s)==GLOBAL) {
        name = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].name();
        globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].setReference(ref.reference(), ref.effectiveBounds());
    }
    else {
        s_error = tr("Internal error: trying to setref to constant: 'setref %1 %2'").arg(s).arg(id);
    }
    const int lineNo = stack_contexts.top().lineNo;
    if (lineNo!=-1 &&
            //(stack_contexts.top().runMode==CRM_OneStep || stack_contexts.top().type==EL_MAIN) &&
            !b_blindMode &&
            stack_contexts.top().type != EL_BELOWMAIN
            )
    {
        const QString qn = ref.algorhitmName().isEmpty()
                ? ref.name()
                : ref.algorhitmName()+"::"+ref.name();
        if (!qn.startsWith("@"))
            emit valueChangeNotice(lineNo, name+"=&"+qn);
    }
    nextIP();
}

void VM::do_refarr(quint8 s, quint16 id)
{
    QMutexLocker l(m_dontTouchMe);
    int dim = 0;
    if (VariableScope(s)==LOCAL) {
        dim = stack_contexts.top().locals[id].dimension();
    }
    else if (VariableScope(s)==GLOBAL) {
        dim = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].dimension();
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
        Variant ref;
        if (VariableScope(s)==LOCAL) {
            ref = stack_contexts.top().locals[id].toReference(indeces);
        }
        else if (VariableScope(s)==GLOBAL) {
            ref = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].toReference(indeces);
        }
        else if (VariableScope(s)==CONST) {
            s_error = tr("Internal error: don't know what is 'ref %1 %2'").arg(s).arg(id);
        }
        stack_values.push(ref);
    }
    nextIP();
}

void VM::do_jump(quint16 ip)
{
    stack_contexts.top().IP = ip;
}

void VM::do_jnz(quint8 r, quint16 ip)
{
    bool v = false;
    if (r==0)
        v = register0.toBool();
    else
        v = stack_contexts.top().registers[r] > 0;
//    bool v = stack_contexts.top().registers[r].toBool();
    if (v) {
        stack_contexts.top().IP = ip;
    }
    else {
        nextIP();
    }
}


void VM::do_jz(quint8 r, quint16 ip)
{
//    bool v = stack_contexts.top().registers[r].toBool();
    bool v = false;
    if (r==0)
        v = register0.toBool();
    else
        v = stack_contexts.top().registers[r] > 0;
    if (v) {
        nextIP();
    }
    else {
        stack_contexts.top().IP = ip;
    }
}

void VM::do_push(quint8 r)
{
    QVariant v;
    if (r==0)
        v = register0;
    else
        v = stack_contexts.top().registers[r];
    if (v.type()==QVariant::Int) {
        stack_values.push(Variant(v.toInt()));
    }
    else if (v.type()==QVariant::Double) {
        stack_values.push(Variant(v.toDouble()));
    }
    else if (v.type()==QVariant::Bool) {
        stack_values.push(Variant(v.toBool()));
    }
    else if (v.type()==QVariant::Char) {
        stack_values.push(Variant(v.toChar()));
    }
    else if (v.type()==QVariant::String) {
        stack_values.push(Variant(v.toString()));
    }
    nextIP();
}

void VM::do_pop(quint8 r)
{
    Variant v = stack_values.pop();
    if (r==0) {
        if (v.baseType()==VT_int) {
            register0 = v.toInt();
        }
        else if (v.baseType()==VT_float) {
            register0 = v.toReal();
        }
        else if (v.baseType()==VT_bool) {
            register0 = v.toBool();
        }
        else if (v.baseType()==VT_char) {
            register0 = v.toChar();
        }
        else if (v.baseType()==VT_string) {
            register0 = v.toString();
        }
    }
    else {
        stack_contexts.top().registers[r] = v.toInt();
    }
    nextIP();
}

void VM::do_showreg(quint8 regNo) {
    if (!b_blindMode) {
        const int lineNo = stack_contexts.top().lineNo;
        if (lineNo!=-1 &&
//                (stack_contexts.top().runMode==CRM_OneStep || stack_contexts.top().type==EL_MAIN) &&
                !b_blindMode
                )
        {
            QVariant val;
            if (regNo==0)
                val = register0;
            else
                val = stack_contexts.top().registers[regNo];
            if (val.type()==QVariant::Bool) {
                emit valueChangeNotice(lineNo, val.toBool()? tr("true") : tr("false"));
            }
            else {
                emit valueChangeNotice(lineNo, val.toString());
            }
        }
    }
    nextIP();
}

void VM::do_clearmarg(quint16 toLine)
{
    const int lineNo = stack_contexts.top().lineNo;
    if (!b_blindMode && lineNo!=-1
//            (stack_contexts.top().runMode==CRM_OneStep || stack_contexts.top().type==EL_MAIN)
            )
    {
        emit clearMargin(lineNo, toLine);
    }
    nextIP();
}

void VM::do_ret()
{
    if (stack_contexts.top().runMode==CRM_UntilReturn) {
        emit retInstruction(stack_contexts.top().lineNo);
        stack_contexts.top().runMode=CRM_ToEnd;
    }
    else {
        last_context = stack_contexts.pop();
        if (last_context.type==Bytecode::EL_INIT
                && last_context.runMode == CRM_OneStep
                )
        {
            if (stack_contexts.size()>0) {
                stack_contexts.top().runMode = CRM_OneStep;
            }
        }
        if (stack_contexts.size()>0) {
            nextIP();
        }
    }
}

void VM::do_error(quint8 s, quint16 id)
{
    if (VariableScope(s)==LOCAL) {
        s_error = stack_contexts.top().locals[id].toString();
    }
    else if (VariableScope(s)==GLOBAL) {
        s_error = globals[QPair<quint8,quint16>(stack_contexts.top().moduleId,id)].toString();
    }
    else if (VariableScope(s)==CONST) {
        s_error = constants[id].toString();
    }
}

void VM::do_line(quint16 no)
{
    if (!b_blindMode && stack_contexts.top().runMode==CRM_OneStep) {
        if (stack_contexts.top().lineNo!=no)
            emit lineNoChanged(no);
    }
    stack_contexts.top().lineNo = no;
    nextIP();
//    qDebug() << "LINE " << no;
//    qDebug() << "STACK SIZE " << stack_values.size();
//    qDebug() << "ACCUMULATOR " << register0;
}

void VM::do_sum()
{
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (a.baseType()==VT_int && b.baseType()==VT_int) {
        Variant r(a.toInt()+b.toInt());
        stack_values.push(r);
        if (!StdLib::IntegerOverflowChecker::checkSumm(a.toInt(), b.toInt())) {
            s_error = tr("Integer overflow while trying to summ");
        }
    }
    else if (a.baseType()==VT_float || b.baseType()==VT_float) {
        Variant r(a.toReal()+b.toReal());
        stack_values.push(r);
        if (!StdLib::DoubleOverflowChecker::checkSumm(a.toReal(), b.toReal())) {
            s_error = tr("Double overflow while trying to summ");
        }
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
    if (a.baseType()==VT_int && b.baseType()==VT_int) {
        Variant r(a.toInt()-b.toInt());
        stack_values.push(r);
        if (!StdLib::IntegerOverflowChecker::checkDiff(a.toInt(), b.toInt())) {
            s_error = tr("Integer overflow while trying to subtract");
        }
    }
    else if (a.baseType()==VT_float || b.baseType()==VT_float) {
        Variant r(a.toReal()-b.toReal());
        stack_values.push(r);
        if (!StdLib::DoubleOverflowChecker::checkDiff(a.toReal(), b.toReal())) {
            s_error = tr("Double overflow while trying to subtract");
        }
    }
    nextIP();
}

void VM::do_mul()
{
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (a.baseType()==VT_int && b.baseType()==VT_int) {
        Variant r(a.toInt()*b.toInt());
        stack_values.push(r);
        if (!StdLib::IntegerOverflowChecker::checkProd(a.toInt(), b.toInt())) {
            s_error = tr("Integer overflow while trying to multiply");
        }
    }
    else if (a.baseType()==VT_float || b.baseType()==VT_float) {
        Variant r(a.toReal()*b.toReal());
        stack_values.push(r);
        if (!StdLib::DoubleOverflowChecker::checkProd(a.toReal(), b.toReal())) {
            s_error = tr("Double overflow while trying to multiply");
        }
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
    if (a.baseType()==VT_int && b.baseType()==VT_int) {
        if (s_error.isEmpty() && !StdLib::IntegerOverflowChecker::checkPower(a.toInt(), b.toInt())) {
            s_error = tr("Integer overflow while trying to power");
        }
    }
    else {
        if (s_error.isEmpty() && !StdLib::DoubleOverflowChecker::checkProd(a.toReal(), b.toReal())) {
            s_error = tr("Double overflow while trying to power");
        }
    }
//    if (b.toReal()<-EPSILON && a.toReal()<-EPSILON) {
//        s_error = tr("Power to less than zero");
//    }
//    else {
    Variant r;
    double fvalue = pow(a.toReal(), b.toReal());
    if (a.baseType()==VT_int && b.baseType()==VT_int)
        r = Variant(int(fvalue));
    else
        r = Variant(fvalue);

        stack_values.push(r);
//    }
    if (a.baseType()==VT_int && b.baseType()==VT_int) {
        if (s_error.isEmpty() && !StdLib::IntegerOverflowChecker::checkPower(a.toInt(), b.toInt())) {
            s_error = tr("Integer overflow while trying to power");
        }
    }
    else {
        if (s_error.isEmpty() && !StdLib::DoubleOverflowChecker::checkProd(a.toReal(), b.toReal())) {
            s_error = tr("Double overflow while trying to power");
        }
    }
    nextIP();
}

void VM::do_neg()
{
    Variant a = stack_values.pop();
    if (a.baseType()==VT_bool) {
        Variant r(!a.toBool());
        stack_values.push(r);
//        stack_contexts.top().registers[0] = QVariant(!a.toBool());
        register0 = QVariant(!a.toBool());
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

#define CHAROFSTRING(x) ( x.length()>0? x[0].unicode() : 0x0000 )

void VM::do_eq()
{
    bool result = false;
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (b.baseType()==VT_int && a.baseType()==VT_int) {
        result = a.toInt()==b.toInt();
    }
    if (b.baseType()==VT_float || a.baseType()==VT_float) {
        result = fabs(a.toReal()-b.toReal())<EPSILON;
    }
    if (b.baseType()==VT_bool && a.baseType()==VT_bool) {
        result = a.toBool()==b.toBool();
    }
    if (a.baseType()==VT_string || b.baseType()==VT_string)
    {
        result = a.toString()==b.toString();
    }
    if (a.baseType()==VT_char && b.baseType()==VT_char) {
        result = CHAROFSTRING(a.toString()) == CHAROFSTRING(b.toString());
    }

    Variant r(result);
    stack_values.push(r);
//    stack_contexts.top().registers[0] = QVariant(result);
    register0 = QVariant(result);
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
    else if (b.baseType()==VT_float && a.baseType()==VT_float) {
        result = fabs(a.toReal()-b.toReal())<EPSILON;
    }
    else if (a.baseType()==VT_float || b.baseType()==VT_float) {
        result = fabs(a.toReal()-b.toReal())<EPSILON;
    }
    if (b.baseType()==VT_bool && a.baseType()==VT_bool) {
        result = a.toBool()==b.toBool();
    }
    if (a.baseType()==VT_string || b.baseType()==VT_string)
    {
        result = a.toString()==b.toString();
    }
    if (a.baseType()==VT_char && b.baseType()==VT_char) {
        result = CHAROFSTRING(a.toString()) == CHAROFSTRING(b.toString());
    }
    Variant r(!result);
    stack_values.push(r);
//    stack_contexts.top().registers[0] = QVariant(result);
    register0 = QVariant(!result);
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
    if (a.baseType()==VT_string || b.baseType()==VT_string)
    {
        result = a.toString()<b.toString();
    }
    if (a.baseType()==VT_char && b.baseType()==VT_char) {
        result = CHAROFSTRING(a.toString()) < CHAROFSTRING(b.toString());
    }
    Variant r(result);
    stack_values.push(r);
//    stack_contexts.top().registers[0] = QVariant(result);
    register0 = QVariant(result);
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
    if (a.baseType()==VT_string || b.baseType()==VT_string)
    {
        result = a.toString()>b.toString();
    }
    if (a.baseType()==VT_char && b.baseType()==VT_char) {
        result = CHAROFSTRING(a.toString()) > CHAROFSTRING(b.toString());
    }
    Variant r(result);
    stack_values.push(r);
//    stack_contexts.top().registers[0] = QVariant(result);
    register0 = QVariant(result);
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
    if (a.baseType()==VT_string || b.baseType()==VT_string)
    {
        result = a.toString()<=b.toString();
    }
    if (a.baseType()==VT_char && b.baseType()==VT_char) {
        result = CHAROFSTRING(a.toString()) <= CHAROFSTRING(b.toString());
    }
    Variant r(result);
    stack_values.push(r);
//    stack_contexts.top().registers[0] = QVariant(result);
    register0 = QVariant(result);
    nextIP();
}

void VM::do_geq()
{
    bool result = false;
    Variant b = stack_values.pop();
    Variant a = stack_values.pop();
    if (b.baseType()==VT_int && a.baseType()==VT_int) {
        result = a.toInt()>=b.toInt();
    }
    if (b.baseType()==VT_float || a.baseType()==VT_float) {
        result = a.toReal()>=b.toReal();
    }
    if (b.baseType()==VT_bool && a.baseType()==VT_bool) {
        result = a.toInt()>=b.toInt();
    }
    if (a.baseType()==VT_string || b.baseType()==VT_string)
    {
        result = a.toString()>=b.toString();
    }
    if (a.baseType()==VT_char && b.baseType()==VT_char) {
        result = CHAROFSTRING(a.toString()) >= CHAROFSTRING(b.toString());
    }
    Variant r(result);
    stack_values.push(r);
//    stack_contexts.top().registers[0] = QVariant(result);
    register0 = QVariant(result);
    nextIP();
}

ElemType VM::topStackType() const
{
    if (stack_contexts.size()==0)
        return EL_NONE;
    else
        return stack_contexts.top().type;
}


void VM::pushValueToStack(const QVariant &value)
{
    if (value.isValid()) {
        if (value.type()==QVariant::Int)
            stack_values.push(Variant(value.toInt()));
        else if (value.type()==QVariant::Double)
            stack_values.push(Variant(value.toDouble()));
        else if (value.type()==QVariant::Bool)
            stack_values.push(Variant(value.toBool()));
        else if (value.type()==QVariant::Char)
            stack_values.push(Variant(value.toChar()));
        else if (value.type()==QVariant::String)
            stack_values.push(Variant(value.toString()));
    }
}

void VM::setResults(
    const QString & error,
    const QList<quintptr> &references,
                    const QList<int> &indeces,
                    const QList<QVariant> &values
                    )
{
    QMutexLocker l(m_dontTouchMe);
    s_error = error;
    if (!error.isEmpty())
        return;
    int indecesStart = 0;
    QStringList marginText;
    Q_ASSERT(references.size()==values.size());
    for (int i=0; i<references.size(); i++) {
        quintptr ptr = references[i];
        QVariant value = values[i];
        Variant * v = (Variant*)(ptr);
        QList<int> inds = indeces.mid(indecesStart, v->dimension());
        QString mt = v->name();
        if (inds.size()>0) {
            mt += "[";
            for (int j=0; j<inds.size(); j++) {
                mt += QString::number(inds[j]);
                if (j<inds.size()-1)
                    mt+=",";
            }
            mt += "]";
        }
        if (value.type()==QVariant::Bool) {
            mt += "="+(value.toBool()? tr("true") : tr("false"));
        }
        else {
            mt += "="+value.toString();
        }
        marginText << mt;
        Q_CHECK_PTR(v);
        v->setValue(inds,value);
        indecesStart += inds.size();
    }
    s_error = Variant::error;
    int lineNo = stack_contexts.top().lineNo;
    if (lineNo!=-1 &&
            (stack_contexts.top().runMode==CRM_OneStep || stack_contexts.top().type==EL_MAIN)
            && !b_blindMode
            )
        emit valueChangeNotice(lineNo, marginText.join(", "));
}

int VM::contextByIds(int moduleId, int algorhitmId) const
{
    for (int i=stack_contexts.size()-1; i>=0; i--) {
        if (stack_contexts.at(i).algId==algorhitmId && stack_contexts.at(i).moduleId==moduleId)
            return i;
    }
    if (last_context.algId==algorhitmId && last_context.moduleId==moduleId)
        return -2;
    return -1;
}

QVariant VM::value(int moduleId, int algorhitmId, int variableId) const
{
    QMutexLocker l(m_dontTouchMe);
    QVariant result;
    if (algorhitmId==-1 && moduleId!=-1 && variableId!=-1) {
        QPair<quint8,quint16> index;
        index.first = moduleId;
        index.second = variableId;
        if (globals.contains(index)) {
            if (globals[index].hasValue())
                result = globals[index].value();
        }
    }
    else if (algorhitmId!=-1 && moduleId!=-1 && variableId!=-1) {
        int context = contextByIds(moduleId, algorhitmId);

        if (context>-1 &&
                variableId < stack_contexts.at(context).locals.size() &&
                stack_contexts.at(context).locals[variableId].hasValue()
                ) {
            result = stack_contexts.at(context).locals[variableId].value();
        }
        else if (context==-2 &&
                 variableId < last_context.locals.size() &&
                 last_context.locals[variableId].hasValue()) {
            result = last_context.locals[variableId].value();
        }
    }
    return result;
}

QList<int> VM::bounds(int moduleId, int algorhitmId, int variableId) const
{
    QMutexLocker l(m_dontTouchMe);
    QList<int> result;
    if (algorhitmId==-1 && moduleId!=-1 && variableId!=-1) {
        QPair<quint8,quint16> index;
        index.first = moduleId;
        index.second = variableId;
        if (globals.contains(index)) {
            result = globals[index].bounds();
        }
    }
    else if (algorhitmId!=-1 && moduleId!=-1 && variableId!=-1) {
        int context = contextByIds(moduleId, algorhitmId);

        if (context>-1) {
            if (variableId<stack_contexts.at(context).locals.size()) {
                result = stack_contexts.at(context).locals[variableId].bounds();
            }
        }
        else if (context==-2) {
            if (variableId<last_context.locals.size()) {
                result = last_context.locals[variableId].bounds();
            }
        }

    }
    return result;
}

QList<int> VM::reference(int moduleId, int algorhitmId, int variableId) const
{
    QMutexLocker l(m_dontTouchMe);
    QList<int> result;
    // TODO implement me
    return result;
}

QVariantList VM::remainingValues() const
{
    QVariantList result;
    for (int i=0; i<stack_values.size(); i++) {
        if (stack_values.at(i).hasValue())
            result << stack_values.at(i).value();
        else
            result << QVariant::Invalid;
    }
    return result;
}

bool VM::hasMoreInstructions() const
{
    if (stack_contexts.size()>0) {
        const QVector<Bytecode::Instruction> * program = stack_contexts.at(0).program;
        int IP = stack_contexts.at(0).IP;
        return IP < program->size();
    }
    else {
        return false;
    }
}

} // namespace KumirCodeRun
