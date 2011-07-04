#include "generator.h"


namespace KumirCodeGenerator {

Generator::Generator(QObject *parent) :
    QObject(parent)
{
    m_ast = 0;
    m_bc = 0;
}

void Generator::reset(const AST::Data *ast, Bytecode::Data *bc)
{
    m_ast = ast;
    m_bc = bc;
    l_constants.clear();
    l_externs.clear();
    l_loopBreaks.clear();
    l_funcBreaks.clear();
}

void Generator::generateConstantTable()
{
    for (int i=l_constants.size()-1; i>=0; i--) {
        QPair<Bytecode::ValueType, QVariant> cons = l_constants[i];
        Bytecode::TableElem e;
        e.type = Bytecode::EL_CONST;
        e.vtype = cons.first;
        e.id = i;
        e.constantValue = cons.second;
        m_bc->d.prepend(e);
    }
}

void Generator::generateExternTable()
{
    for (int i=l_externs.size()-1; i>=0; i--) {
        QPair<quint8, quint16> ext = l_externs[i];
        Bytecode::TableElem e;
        e.type = Bytecode::EL_EXTERN;
        e.module = ext.first;
        e.algId = e.id = ext.second;
        e.moduleName = m_ast->modules[ext.first]->header.cReference.nameSpace;
        e.name = m_ast->modules[ext.first]->header.algorhitms[ext.second]->header.name;
        m_bc->d.prepend(e);
    }
}

Bytecode::ValueType Generator::valueType(AST::VariableBaseType t)
{
    if (t==AST::TypeInteger)
        return Bytecode::VT_int;
    else if (t==AST::TypeReal)
        return Bytecode::VT_float;
    else if (t==AST::TypeBoolean)
        return Bytecode::VT_bool;
    else if (t==AST::TypeString)
        return Bytecode::VT_string;
    else if (t==AST::TypeCharect)
        return Bytecode::VT_char;
    else
        return Bytecode::VT_void;
}

Bytecode::ValueKind Generator::valueKind(AST::VariableAccessType t)
{
    if (t==AST::AccessArgumentIn)
        return Bytecode::VK_In;
    else if (t==AST::AccessArgumentOut)
        return Bytecode::VK_Out;
    else if (t==AST::AccessArgumentInOut)
        return Bytecode::VK_InOut;
    else
        return Bytecode::VK_Plain;
}

Bytecode::InstructionType Generator::operation(AST::ExpressionOperator op)
{
    if (op==AST::OpSumm)
        return Bytecode::SUM;
    else if (op==AST::OpSubstract)
        return Bytecode::SUB;
    else if (op==AST::OpMultiply)
        return Bytecode::MUL;
    else if (op==AST::OpDivision)
        return Bytecode::DIV;
    else if (op==AST::OpPower)
        return Bytecode::POW;
    else if (op==AST::OpNot)
        return Bytecode::NEG;
    else if (op==AST::OpAnd)
        return Bytecode::AND;
    else if (op==AST::OpOr)
        return Bytecode::OR;
    else if (op==AST::OpEqual)
        return Bytecode::EQ;
    else if (op==AST::OpLess)
        return Bytecode::LS;
    else if (op==AST::OpGreater)
        return Bytecode::GT;
    else if (op==AST::OpLessOrEqual)
        return Bytecode::LEQ;
    else if (op==AST::OpGreaterOrEqual)
        return Bytecode::GEQ;
    else
        return Bytecode::NOP;
}

void Generator::addModule(const AST::Module *mod)
{
    if (!mod->header.enabled)
        return;
    int id = m_ast->modules.indexOf(const_cast<AST::Module*>(mod));
    if (mod->header.type==AST::ModTypeExternal) {

    }
    else {
        addKumirModule(id, mod);
    }
}



void Generator::addKumirModule(int id, const AST::Module *mod)
{
    for (int i=0; i<mod->impl.globals.size(); i++) {
        const AST::Variable * var = mod->impl.globals[i];
        Bytecode::TableElem glob;
        glob.type = Bytecode::EL_GLOBAL;
        glob.module = quint8(id);
        glob.id = quint16(i);
        glob.name = var->name;
        glob.dimension = quint8(var->dimension);
        glob.vtype = valueType(var->baseType);
        glob.refvalue = valueKind(var->accessType);
        m_bc->d.append(glob);
    }
    Bytecode::TableElem initElem;
    initElem.type = Bytecode::EL_INIT;
    initElem.module = quint8(id);
    initElem.instructions = instructions(id, -1, 0, mod->impl.initializerBody).toVector();
    m_bc->d.append(initElem);
    for (int i=0; i<mod->impl.algorhitms.size() ; i++) {
        const AST::Algorhitm * alg = mod->impl.algorhitms[i];
        Bytecode::ElemType ft = Bytecode::EL_FUNCTION;
        if (mod->header.name.isEmpty() && i==0) {
            ft = Bytecode::EL_MAIN;
        }
        if (alg->header.specialType==AST::AlgorhitmTypeTesting) {
            ft = Bytecode::EL_TESTING;
        }
        addFunction(i, id, ft, alg);
    }
}

void Generator::shiftInstructions(QList<Bytecode::Instruction> &instrs, int offset)
{
    for (int i=0; i<instrs.size(); i++) {
        Bytecode::InstructionType t = instrs.at(i).type;
        if (t==Bytecode::JNZ || t==Bytecode::JZ || t==Bytecode::JUMP) {
            instrs[i].arg += offset;
        }
    }
}

void Generator::addFunction(int id, int moduleId, Bytecode::ElemType type, const AST::Algorhitm *alg)
{
    for (int i=0; i<alg->impl.locals.size(); i++) {
        const AST::Variable * var = alg->impl.locals[i];
        Bytecode::TableElem loc;
        loc.type = Bytecode::EL_LOCAL;
        loc.module = moduleId;
        loc.algId = id;
        loc.id = i;
        loc.name = var->name;
        loc.dimension = var->dimension;
        loc.vtype = valueType(var->baseType);
        loc.refvalue = valueKind(var->accessType);
        m_bc->d.append(loc);
    }
    Bytecode::TableElem func;
    func.type = type;
    func.module = moduleId;
    func.algId = func.id = id;
    func.name = alg->header.name;
    QList<Bytecode::Instruction> argHandle;

    for (int i=alg->header.arguments.size()-1; i>=0; i--) {
        Bytecode::Instruction store;
        store.type = Bytecode::STORE;
        const AST::Variable * var = alg->header.arguments[i];
        findVariable(moduleId, id, var, store.scope, store.arg);
        argHandle << store;
        Bytecode::Instruction pop;
        pop.type = Bytecode::POP;
        pop.registerr = 0;
        argHandle << pop;
    }

    for (int i=0; i<alg->header.arguments.size(); i++) {
        const AST::Variable * var = alg->header.arguments[i];
        if (var->dimension>0) {
            for (int i=var->dimension-1; i>=0; i--) {
                argHandle << calculate(moduleId, id, 0, var->bounds[i].second);
                argHandle << calculate(moduleId, id, 0, var->bounds[i].first);
            }
            Bytecode::Instruction bounds;
            bounds.type = Bytecode::SETARR;
            findVariable(moduleId, id, var, bounds.scope, bounds.arg);
            argHandle << bounds;
        }
        if (var->accessType==AST::AccessArgumentOut) {
            Bytecode::Instruction init;
            init.type = Bytecode::INIT;
            findVariable(moduleId, id, var, init.scope, init.arg);
            argHandle << init;
        }
    }

    QList<Bytecode::Instruction> pre = instructions(moduleId, id, 0, alg->impl.pre);
    QList<Bytecode::Instruction> body = instructions(moduleId, id, 0, alg->impl.body);
    QList<Bytecode::Instruction> post = instructions(moduleId, id, 0, alg->impl.post);

    int offset = argHandle.size() + pre.size();
    shiftInstructions(body, offset);
    shiftInstructions(post, offset);

    QList<Bytecode::Instruction> ret;

    int retIp = argHandle.size() + pre.size() + body.size() + post.size();

    for (int i=0; i<l_funcBreaks.size(); i++) {
        quint16 ip = l_funcBreaks[i] + offset;
        body[ip].arg = retIp;
    }
    l_funcBreaks.clear();

    Bytecode::Instruction line;
    line.type = Bytecode::LINE;
    line.arg = alg->impl.endLexems[0]->lineNo;
    ret << line;

    const AST::Variable * retval = returnValue(alg);
    if (retval) {
        Bytecode::Instruction loadRetval;
        loadRetval.type = Bytecode::LOAD;
        findVariable(moduleId, id, retval, loadRetval.scope, loadRetval.arg);
        ret << loadRetval;

    }

    Bytecode::Instruction retturn;
    retturn.type = Bytecode::RET;
    ret << retturn;

    QList<Bytecode::Instruction> instrs = argHandle + pre + body + post + ret;
    func.instructions = instrs.toVector();
    m_bc->d.append(func);
}

QList<Bytecode::Instruction> Generator::instructions(
    int modId, int algId, int level,
    const QList<AST::Statement *> &statements)
{
    QList<Bytecode::Instruction> result;
    for (int i=0; i<statements.size(); i++) {
        const AST::Statement * st = statements[i];
        switch (st->type) {
        case AST::StError:
            ERROR(modId, algId, level, st, result);
            break;
        case AST::StAssign:
            ASSIGN(modId, algId, level, st, result);
            break;
        case AST::StAssert:
            ASSERT(modId, algId, level, st, result);
            break;
        case AST::StVarInitialize:
            INIT(modId, algId, level, st, result);
            break;
        case AST::StInput:
            CALL_SPECIAL(modId, algId, level, st, result);
            break;
        case AST::StOutput:
            CALL_SPECIAL(modId, algId, level, st, result);
            break;
        case AST::StFileInput:
            CALL_SPECIAL(modId, algId, level, st, result);
            break;
        case AST::StFileOutput:
            CALL_SPECIAL(modId, algId, level, st, result);
            break;
        case AST::StLoop:
            LOOP(modId, algId, level+1, st, result);
            break;
        case AST::StIfThenElse:
            IFTHENELSE(modId, algId, level, st, result);
            break;
        case AST::StSwitchCaseElse:
            SWITCHCASEELSE(modId, algId, level, st, result);
            break;
        case AST::StBreak:
            BREAK(modId, algId, level, st, result);
        default:
            break;
        }
    }
    return result;
}

quint16 Generator::constantValue(Bytecode::ValueType type, const QVariant &value)
{
    QPair<Bytecode::ValueType, QVariant> c(type, value);
    if (!l_constants.contains(c)) {
        l_constants << c;
        return l_constants.size()-1;
    }
    else {
        return l_constants.indexOf(c);
    }
}

void Generator::ERROR(int , int , int , const AST::Statement * st, QList<Bytecode::Instruction>  & result)
{
    int lineNo = st->lexems[0]->lineNo;
    Bytecode::Instruction l;
    l.type = Bytecode::LINE;
    l.arg = lineNo;
    const QString error = ErrorMessages::message("KumirAnalizer", QLocale::Russian, st->error);
    Bytecode::Instruction e;
    e.type = Bytecode::ERROR;
    e.scope = Bytecode::CONST;
    e.arg = constantValue(Bytecode::VT_string, error);
    result << l << e;
}

void Generator::findVariable(int modId, int algId, const AST::Variable * var, Bytecode::VariableScope & scope, quint16 & id) const
{
    const AST::Module * mod = m_ast->modules.at(modId);
    for (quint16 i=0; i<mod->impl.globals.size(); i++) {
        if (mod->impl.globals.at(i)==var) {
            scope = Bytecode::GLOBAL;
            id = i;
            return;
        }
    }
    const AST::Algorhitm * alg = mod->impl.algorhitms[algId];
    for (quint16 i=0; i<alg->impl.locals.size(); i++) {
        if (alg->impl.locals.at(i)==var) {
            scope = Bytecode::LOCAL;
            id = i;
            return;
        }
    }
}

void Generator::findFunction(const AST::Algorhitm *alg, quint8 &module, quint16 &id)
{
    for (quint8 i=0; i<m_ast->modules.size(); i++) {
        const AST::Module * mod = m_ast->modules[i];
        QList<AST::Algorhitm*> table;
        if (mod->header.type==AST::ModTypeExternal)
            table = mod->header.algorhitms;
        else
            table = mod->impl.algorhitms;
        for (quint16 j=0; j<table.size(); j++) {
            if (alg==table[j]) {
                module = i;
                id = j;
                if (mod->header.type==AST::ModTypeExternal) {
                    QPair<quint8,quint16> ext(module, id);
                    if (!l_externs.contains(ext))
                        l_externs << ext;
                }
                return;
            }
        }
    }
}

void Generator::ASSIGN(int modId, int algId, int level, const AST::Statement *st, QList<Bytecode::Instruction> & result)
{
    int lineNo = st->lexems[0]->lineNo;
    Bytecode::Instruction l;
    l.type = Bytecode::LINE;
    l.arg = lineNo;
    result << l;

    const AST::Expression * rvalue = st->expressions[0];
    result << calculate(modId, algId, level, rvalue);

    if (st->expressions.size()>1) {
        const AST::Expression * lvalue = st->expressions[1];
        Bytecode::Instruction store;
        findVariable(modId, algId, lvalue->variable, store.scope, store.arg);
        if (lvalue->kind==AST::ExprArrayElement) {
            for (int i=lvalue->operands.size(); i>=0 ;i--) {
                result << calculate(modId, algId, level, lvalue->operands[i]);
            }
            store.type = Bytecode::STOREARR;
        }
        else {
            store.type = Bytecode::STORE;
        }
        result << store;
        Bytecode::Instruction pop;
        pop.type = Bytecode::POP;
        pop.registerr = 0;
        result << pop;
    }
}

QList<Bytecode::Instruction> Generator::calculate(int modId, int algId, int level, const AST::Expression* st)
{
    QList<Bytecode::Instruction> result;
    if (st->kind==AST::ExprConst) {
        int constId = constantValue(valueType(st->baseType), st->constant);
        Bytecode::Instruction instr;
        instr.type = Bytecode::LOAD;
        instr.scope = Bytecode::CONST;
        instr.arg = constId;
        result << instr;
    }
    else if (st->kind==AST::ExprVariable) {
        Bytecode::Instruction instr;
        instr.type = Bytecode::LOAD;
        findVariable(modId, algId, st->variable, instr.scope, instr.arg);
        result << instr;
    }
    else if (st->kind==AST::ExprArrayElement) {
        for (int i=st->operands.size()-1; i>=0; i--) {
            result << calculate(modId, algId, level, st->operands[i]);
        }
        Bytecode::Instruction instr;
        instr.type = Bytecode::LOADARR;
        findVariable(modId, algId, st->variable, instr.scope, instr.arg);
        result << instr;
    }
    else if (st->kind==AST::ExprFunctionCall) {
        const AST::Algorhitm * alg = st->function;
        if (alg->header.implType==AST::AlgorhitmExternal) {
            // Push to stack references (and them count) and arguments (and them count)

            // Push references to stack
            Bytecode::Instruction b;
            int refsCount = 0;
            for (int i=st->operands.size()-1; i>=0; i--) {
                AST::VariableAccessType t = alg->header.arguments[i]->accessType;
                if (t==AST::AccessArgumentOut||t==AST::AccessArgumentInOut) {
                    Bytecode::Instruction ref;
                    ref.type = Bytecode::REF;
                    findVariable(modId, algId, st->operands[i]->variable, ref.scope, ref.arg);
                    result << ref;
                    refsCount ++;
                }
            }
            b.type = Bytecode::LOAD;
            b.scope = Bytecode::CONST;
            b.arg = constantValue(Bytecode::VT_int, refsCount);
            result << b;
            // Push calculable arguments to stack
            int argsCount = 0;
            for (int i=st->operands.size()-1; i>=0; i--) {
                AST::VariableAccessType t = alg->header.arguments[i]->accessType;
                bool arr = alg->header.arguments[i]->dimension>0;
                if (t==AST::AccessArgumentIn && !arr)
                    result << calculate(modId, algId, level, st->operands[i]);
                else if (t==AST::AccessArgumentIn && arr) {
                    // load the whole array into stack
                    Bytecode::Instruction load;
                    load.type = Bytecode::LOAD;
                    findVariable(modId, algId, st->operands[i]->variable, load.scope, load.arg);
                    result << load;
                }
                if (t==AST::AccessArgumentIn)
                    argsCount ++;
            }
            b.arg = constantValue(Bytecode::VT_int, argsCount);
            result << b;
        }
        else {
            // Push to stack just arguments if reverse order
            for (int i=st->operands.size()-1; i>=0; i--) {
                AST::VariableAccessType t = alg->header.arguments[i]->accessType;
                bool arr = alg->header.arguments[i]->dimension>0;
                if (t==AST::AccessArgumentIn && !arr)
                    result << calculate(modId, algId, level, st->operands[i]);
                else if (t==AST::AccessArgumentIn && arr) {
                    // load the whole array into stack
                    Bytecode::Instruction load;
                    load.type = Bytecode::LOAD;
                    findVariable(modId, algId, st->operands[i]->variable, load.scope, load.arg);
                    result << load;
                }
                else if (t==AST::AccessArgumentOut||t==AST::AccessArgumentInOut) {
                    Bytecode::Instruction ref;
                    ref.type = Bytecode::REF;
                    findVariable(modId, algId, st->operands[i]->variable, ref.scope, ref.arg);
                    result << ref;
                }
            }
        }
        Bytecode::Instruction instr;
        instr.type = Bytecode::CALL;
        findFunction(st->function, instr.module, instr.arg);
        result << instr;
    }
    else if (st->kind==AST::ExprSubexpression) {
        for (int i=0; i<st->operands.size(); i++) {
            result << calculate(modId, algId, level, st->operands[i]);
        }
        Bytecode::Instruction instr;
        instr.type = operation(st->operatorr);
        result << instr;
    }
    return result;
}

void Generator::ASSERT(int modId, int algId, int level, const AST::Statement * st, QList<Bytecode::Instruction> & result)
{
    int lineNo = st->lexems[0]->lineNo;
    Bytecode::Instruction l;
    l.type = Bytecode::LINE;
    l.arg = lineNo;
    result << l;

    for (int i=0; i<st->expressions.size(); i++) {
        result << calculate(modId, algId, level, st->expressions[i]);
        Bytecode::Instruction pop;
        pop.type = Bytecode::POP;
        pop.registerr = 0;
        result << pop;
        int ip = result.size(); // pointing to next of calculation (i.e. JNZ instruction)
        int targetIp = ip + 2;
        Bytecode::Instruction jnz;
        jnz.type = Bytecode::JNZ;
        jnz.registerr = 0;
        jnz.arg = targetIp;
        result << jnz;
        Bytecode::Instruction err;
        err.type = Bytecode::ERROR;
        err.scope = Bytecode::CONST;
        err.arg = constantValue(Bytecode::VT_string, tr("Assertion false"));
        result << err;
    }
}

void Generator::INIT(int modId, int algId, int level, const AST::Statement * st, QList<Bytecode::Instruction> & result)
{
    int lineNo = st->lexems[0]->lineNo;
    Bytecode::Instruction l;
    l.type = Bytecode::LINE;
    l.arg = lineNo;
    result << l;

    for (int i=0; i<st->variables.size(); i++) {
        const AST::Variable * var = st->variables[i];
        if (var->dimension > 0) {
            for (int i=var->dimension-1; i>=0 ; i--) {
                result << calculate(modId, algId, level, var->bounds[i].second);
                result << calculate(modId, algId, level, var->bounds[i].first);
            }
            Bytecode::Instruction bounds;
            bounds.type = Bytecode::SETARR;
            findVariable(modId, algId, var, bounds.scope, bounds.arg);
            result << bounds;
        }
        Bytecode::Instruction init;
        init.type = Bytecode::INIT;
        findVariable(modId, algId, var, init.scope, init.arg);
        result << init;
    }
}

void Generator::CALL_SPECIAL(int modId, int algId, int level, const AST::Statement * st, QList<Bytecode::Instruction> & result)
{
    int lineNo = st->lexems[0]->lineNo;
    Bytecode::Instruction l;
    l.type = Bytecode::LINE;
    l.arg = lineNo;
    result << l;

    quint16 argsCount;

    if (st->type==AST::StOutput || st->type==AST::StFileOutput) {
        for (int i=st->expressions.size()-1; i>=0; i--) {
            result << calculate(modId, algId, level, st->expressions[i]);
        }
        argsCount = st->expressions.size();
    }
    else {
        QString format;
        for (int i=st->expressions.size()-1; i>=0; i--) {
            AST::Expression * expr = st->expressions[i];
            Bytecode::Instruction ref;
            findVariable(modId, algId, expr->variable, ref.scope, ref.arg);
            if (expr->kind==AST::ExprVariable) {
                ref.type = Bytecode::REF;
            }
            else if (expr->kind==AST::ExprArrayElement) {
                ref.type = Bytecode::REFARR;
                for (int j=expr->operands.size()-1; j>=0; j--) {
                    result << calculate(modId, algId, level, expr->operands[j]);
                }
            }
            if (expr->baseType==AST::TypeBoolean)
                format += "%b";
            else if (expr->baseType==AST::TypeCharect)
                format += "%c";
            else if (expr->baseType==AST::TypeInteger)
                format += "%d";
            else if (expr->baseType==AST::TypeReal)
                format += "%f";
            else
                format += "%s";
            result << ref;
        }
        Bytecode::Instruction fmt;
        fmt.type = Bytecode::LOAD;
        fmt.scope = Bytecode::CONST;
        fmt.arg = constantValue(Bytecode::VT_string, format);
        argsCount = st->expressions.size() + 1;
        result << fmt;
    }

    Bytecode::Instruction pushCount;
    pushCount.type = Bytecode::LOAD;
    pushCount.scope = Bytecode::CONST;
    pushCount.arg = constantValue(Bytecode::VT_int, argsCount);
    result << pushCount;

    Bytecode::Instruction call;
    call.type = Bytecode::CALL;
    call.module = 0xFF;
    if (st->type==AST::StInput)
        call.arg = 0x0000;
    else if (st->type==AST::StOutput)
        call.arg = 0x0001;
    if (st->type==AST::StFileInput)
        call.arg = 0x0002;
    else if (st->type==AST::StFileOutput)
        call.arg = 0x0003;
    result << call;
}


void Generator::IFTHENELSE(int modId, int algId, int level, const AST::Statement * st, QList<Bytecode::Instruction> &result)
{
    int lineNo = st->lexems[0]->lineNo;
    Bytecode::Instruction l;
    l.type = Bytecode::LINE;
    l.arg = lineNo;
    result << l;

    result << calculate(modId, algId, level, st->conditionals[0].condition);

    Bytecode::Instruction pop;
    pop.type = Bytecode::POP;
    pop.registerr = 0;
    result << pop;

    int jzIP = result.size();
    Bytecode::Instruction jz;
    jz.type = Bytecode::JZ;
    jz.registerr = 0;
    result << jz;

    result << instructions(modId, algId, level, st->conditionals[0].body);
    result[jzIP].arg = result.size();

    if (st->conditionals.size()>1) {
        int jumpIp = result.size();
        Bytecode::Instruction jump;
        jump.type = Bytecode::JUMP;
        result << jump;
        result[jzIP].arg = result.size();
        result << instructions(modId, algId, level, st->conditionals[1].body);
        result[jumpIp].arg = result.size();
    }
}

void Generator::SWITCHCASEELSE(int modId, int algId, int level, const AST::Statement *st, QList<Bytecode::Instruction> & result)
{
    Bytecode::Instruction l;
    l.type = Bytecode::LINE;

    int lastJzIp = -1;
    QList<int> jumpIps;

    for (int i=0; i<st->conditionals.size(); i++) {
        if (lastJzIp!=-1) {
            result[lastJzIp].arg = result.size();
            lastJzIp = -1;
        }
        int lineNo = st->conditionals[i].lexems[0]->lineNo;
        l.arg = lineNo;
        result << l;
        if (st->conditionals[i].condition) {
            result << calculate(modId, algId, level, st->conditionals[i].condition);
            Bytecode::Instruction pop;
            pop.type = Bytecode::POP;
            pop.registerr = 0;
            result << pop;
            Bytecode::Instruction jz;
            jz.type = Bytecode::JZ;
            jz.registerr = 0;
            lastJzIp = result.size();
            result << jz;
        }
        result << instructions(modId, algId, level, st->conditionals[i].body);
        if (i<st->conditionals.size()-1) {
            Bytecode::Instruction jump;
            jump.type = Bytecode::JUMP;
            jumpIps << result.size();
            result << jump;
        }
    }
    if (lastJzIp!=-1)
        result[lastJzIp].arg = result.size();
    for (int i=0; i<jumpIps.size(); i++) {
        result[jumpIps[i]].arg = result.size();
    }
}

const AST::Variable * Generator::returnValue(const AST::Algorhitm * alg)
{
    const QString name = alg->header.name;
    for (int i=0; i<alg->impl.locals.size(); i++) {
        if (alg->impl.locals[i]->name == name)
            return alg->impl.locals[i];
    }
    return 0;
}

void Generator::BREAK(int , int , int level,
                      const AST::Statement * st,
                      QList<Bytecode::Instruction> & result)
{
    int lineNo = st->lexems[0]->lineNo;
    Bytecode::Instruction l;
    l.type = Bytecode::LINE;
    l.arg = lineNo;
    result << l;

    Bytecode::Instruction jump;
    jump.type = Bytecode::JUMP;

    if (level==0) {
        l_funcBreaks << result.size();
    }
    else {
        l_loopBreaks << QPair<quint8,quint16>(level, result.size());
    }

    result << jump;
}

void Generator::LOOP(int modId, int algId,
                     int level,
                     const AST::Statement *st,
                     QList<Bytecode::Instruction> &result)
{


    int lineNo = st->lexems[0]->lineNo;
    Bytecode::Instruction l;
    l.type = Bytecode::LINE;
    l.arg = lineNo;
    result << l;

    int beginIp = result.size();
    int jzIp = -1;


    if (st->loop.type==AST::LoopWhile) {
        // pass
    }
    else if (st->loop.type==AST::LoopTimes) {
        // Calculate times value
        result << calculate(modId, algId, level, st->loop.timesValue);
        Bytecode::Instruction a;

        // First time increase by 1
        a.type = Bytecode::LOAD;
        a.scope = Bytecode::CONST;
        a.arg = constantValue(Bytecode::VT_int, 1);
        result << a;

        a.type = Bytecode::SUM;
        result << a;

        // Store value in register
        a.type = Bytecode::POP;
        a.registerr = level;
        result << a;

        // Make begin
        beginIp = result.size();
        result << l;

        // Decrease value in register
        a.type = Bytecode::PUSH;
        a.registerr = level;
        result << a;
        a.type = Bytecode::LOAD;
        a.scope = Bytecode::CONST;
        a.arg = constantValue(Bytecode::VT_int, 1);
        result << a;
        a.type = Bytecode::SUB;
        result << a;
        a.type = Bytecode::POP;
        a.registerr = level;
        result << a;

        // Check if register value > 0
        jzIp = result.size();
        a.type = Bytecode::JZ;
        a.registerr = level;
        result << a;
    }
    else if (st->loop.type==AST::LoopFor) {

        // Calculate initial value
        result << calculate(modId, algId, level, st->loop.fromValue);
        if (st->loop.forVariable->dimension>0) {
            for (int i=st->loop.forVariable->dimension-1; i>=0; i--) {
                result << calculate(modId, algId, level, st->expressions[i]);
            }
        }
        Bytecode::Instruction a;
        a.type = st->loop.forVariable->dimension>0 ? Bytecode::STOREARR : Bytecode::STORE;
        findVariable(modId, algId, st->loop.forVariable, a.scope, a.arg);
        result << a;

        // Skip first value change by passing JUMP
        int jumpId = result.size();
        a.type = Bytecode::JUMP;
        result << a;

        // Begin
        beginIp = result.size();
        result[jumpId].arg = beginIp;
        result << l;

        // Change variable
        if (st->loop.forVariable->dimension>0) {
            for (int i=st->loop.forVariable->dimension-1; i>=0; i--) {
                result << calculate(modId, algId, level, st->expressions[i]);
            }
        }
        a.type = st->loop.forVariable->dimension>0 ? Bytecode::LOADARR : Bytecode::LOAD;
        findVariable(modId, algId, st->loop.forVariable, a.scope, a.arg);
        result << a;

        result << calculate(modId, algId, level, st->loop.stepValue);
        a.type = Bytecode::SUM;
        result << a;

        if (st->loop.forVariable->dimension>0) {
            for (int i=st->loop.forVariable->dimension-1; i>=0; i--) {
                result << calculate(modId, algId, level, st->expressions[i]);
            }
        }
        a.type = st->loop.forVariable->dimension>0 ? Bytecode::STOREARR : Bytecode::STORE;
        findVariable(modId, algId, st->loop.forVariable, a.scope, a.arg);
        result << a;

        result << calculate(modId, algId, level, st->loop.toValue);
        a.type = Bytecode::LEQ;
        result << a;

        a.type = Bytecode::POP;
        a.registerr = level;
        result << a;

        // Check if register value > 0
        jzIp = result.size();
        a.type = Bytecode::JZ;
        a.registerr = level;
        result << a;
    }

    result << instructions(modId, algId, level, st->loop.body);

    if (st->loop.endCondition) {
        lineNo = st->loop.endLexems[0]->lineNo;
        l.arg = lineNo;
        result << l;
        result << calculate(modId, algId, level, st->loop.endCondition);
        Bytecode::Instruction e;
        e.type = Bytecode::POP;
        e.registerr = 0;
        result << e;
        e.type = Bytecode::JZ;
        e.registerr = 0;
        e.arg = beginIp;
        result << e;
    }
    else {
        lineNo = st->loop.endLexems[0]->lineNo;
        l.arg = lineNo;
        result << l;
        Bytecode::Instruction e;
        e.type = Bytecode::JUMP;
        e.registerr = 0;
        e.arg = beginIp;
        result << e;
    }

    // Found end of loop
    result[jzIp].arg = result.size();

    // If loop has break statements -- set proper jump for them
    QList< QPair<quint8,quint16> > toRemove;
    for (int i=0; i<l_loopBreaks.size(); i++) {
        QPair<quint8,quint16> breakIp = l_loopBreaks[i];
        quint8 lev = breakIp.first;
        quint16 ip = breakIp.second;
        if (lev==level) {
            result[ip].arg = result.size();
            toRemove << breakIp;
        }
    }
    for (int i=0; i<toRemove.size(); i++) {
        l_loopBreaks.removeAll(toRemove[i]);
    }

}

} // namespace KumirCodeGenerator
