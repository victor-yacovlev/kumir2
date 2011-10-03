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
    else if (op==AST::OpNotEqual)
        return Bytecode::NEQ;
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
    const AST::Module * mainMod = 0;
    const AST::Algorhitm * mainAlg = 0;
    int mainModId = -1;
    int mainAlgorhitmId = -1;
    for (int i=0; i<mod->impl.algorhitms.size() ; i++) {
        const AST::Algorhitm * alg = mod->impl.algorhitms[i];
        Bytecode::ElemType ft = Bytecode::EL_FUNCTION;
        if (mod->header.name.isEmpty() && i==0) {
            ft = Bytecode::EL_MAIN;
            if (!alg->header.arguments.isEmpty())
            {
                mainMod = mod;
                mainAlg = alg;
                mainModId = id;
                mainAlgorhitmId = i;
            }
        }
        if (alg->header.specialType==AST::AlgorhitmTypeTesting) {
            ft = Bytecode::EL_TESTING;
        }
        addFunction(i, id, ft, alg);
    }
    if (mainMod && mainAlg) {
        addInputArgumentsMainAlgorhitm(mainModId, mainAlgorhitmId, mainMod, mainAlg);
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

void Generator::addInputArgumentsMainAlgorhitm(int moduleId, int algorhitmId, const AST::Module *mod, const AST::Algorhitm *alg)
{
    // Generate hidden algorhitm, which will called before main to input arguments
    int algId = mod->impl.algorhitms.size();
    QList<Bytecode::Instruction> instrs;
    Bytecode::Instruction l;
    l.type = Bytecode::LINE;
    l.arg = alg->impl.headerLexems[0]->lineNo;
    instrs << l;
    QList<quint16> varsToOut;
    int locOffset = 0;

    // Add function return as local
    if (alg->header.returnType!=AST::TypeNone) {
        const AST::Variable * retval = returnValue(alg);
        Bytecode::TableElem loc;
        loc.type = Bytecode::EL_LOCAL;
        loc.module = moduleId;
        loc.algId = algId;
        loc.id = 0;
        loc.name = tr("Function return value");
        loc.dimension = 0;
        loc.vtype = valueType(retval->baseType);
        loc.refvalue = Bytecode::VK_Plain;
        m_bc->d.append(loc);
        varsToOut << constantValue(Bytecode::VT_int, 0);
        locOffset = 1;
    }

    // Add arguments as locals
    for (int i=0; i<alg->header.arguments.size(); i++) {
        const AST::Variable * var = alg->header.arguments[i];
        Bytecode::TableElem loc;
        loc.type = Bytecode::EL_LOCAL;
        loc.module = moduleId;
        loc.algId = algId;
        loc.id = locOffset+i;
        loc.name = var->name;
        loc.dimension = var->dimension;
        loc.vtype = valueType(var->baseType);
        loc.refvalue = Bytecode::VK_Plain;
        m_bc->d.append(loc);

    }

    for (int i=0; i<alg->header.arguments.size(); i++) {
        AST::Variable * var = alg->header.arguments[i];
        // Initialize argument
        if (var->dimension > 0) {
            for (int j=var->dimension-1; j>=0 ; j--) {
                QList<Bytecode::Instruction> initBounds;
                initBounds << calculate(moduleId, algorhitmId, 0, var->bounds[j].second);
                initBounds << calculate(moduleId, algorhitmId, 0, var->bounds[j].first);
                instrs << initBounds;
            }
            Bytecode::Instruction bounds;
            bounds.type = Bytecode::SETARR;
            bounds.scope = Bytecode::LOCAL;
            bounds.arg = quint16(i+locOffset);
            instrs << bounds;
        }
        Bytecode::Instruction init;
        init.type = Bytecode::INIT;
        init.scope = Bytecode::LOCAL;
        init.arg = quint16(i+locOffset);
        instrs << init;
        if (var->initialValue.isValid() && var->dimension==0) {
            Bytecode::Instruction load;
            load.type = Bytecode::LOAD;
            load.scope = Bytecode::CONST;
            load.arg = constantValue(valueType(var->baseType), var->initialValue);
            instrs << load;
            Bytecode::Instruction store = init;
            store.type = Bytecode::STORE;
            instrs << store;
            Bytecode::Instruction pop;
            pop.type = Bytecode::POP;
            pop.registerr = 0;
            instrs << pop;
        }

        // If IN of INOUT argument -- require input
        // This made by special function call
        if (var->accessType==AST::AccessArgumentIn || var->accessType==AST::AccessArgumentInOut)  {
            Bytecode::Instruction varId;
            varId.type = Bytecode::LOAD;
            varId.scope = Bytecode::CONST;
            varId.arg = constantValue(Bytecode::VT_int, i+locOffset);

            Bytecode::Instruction call;
            call.type = Bytecode::CALL;
            call.module = 0xFF;
            call.arg = 0xBB01;

            instrs << varId << call;
        }
        if (var->accessType==AST::AccessArgumentOut || var->accessType==AST::AccessArgumentInOut) {
            varsToOut << constantValue(Bytecode::VT_int, i+locOffset);
        }
    }

    // Call main (first) algorhitm:
    //  -- 1) Push arguments
    for (int i=alg->header.arguments.size()-1; i>=0; i--) {
        AST::VariableAccessType t = alg->header.arguments[i]->accessType;
        if (t==AST::AccessArgumentIn) {
            Bytecode::Instruction load;
            load.type = Bytecode::LOAD;
            load.scope = Bytecode::LOCAL;
            load.arg = quint16(i+locOffset);
            instrs << load;
        }
        else if (t==AST::AccessArgumentOut||t==AST::AccessArgumentInOut) {
            Bytecode::Instruction ref;
            ref.type = Bytecode::REF;
            ref.scope = Bytecode::LOCAL;
            ref.arg = quint16(i+locOffset);
            instrs << ref;
        }
    }
    //  -- 2) Call main (first) algorhitm
    Bytecode::Instruction callInstr;
    callInstr.type = Bytecode::CALL;
    findFunction(alg, callInstr.module, callInstr.arg);
    instrs << callInstr;
    //  -- 3) Store return value
    if (alg->header.returnType!=AST::TypeNone) {
        Bytecode::Instruction storeRetVal;
        storeRetVal.type = Bytecode::STORE;
        storeRetVal.scope = Bytecode::LOCAL;
        storeRetVal.arg = 0;
        Bytecode::Instruction pop;
        pop.type = Bytecode::POP;
        pop.registerr = 0;
        instrs << storeRetVal << pop;
    }

    // Show what in result...

    for (int i=0; i<varsToOut.size(); i++) {
        Bytecode::Instruction arg;
        arg.type = Bytecode::LOAD;
        arg.scope = Bytecode::CONST;
        arg.arg = varsToOut[i];
        instrs << arg;
        Bytecode::Instruction callShow;
        callShow.type = Bytecode::CALL;
        callShow.module = 0xFF;
        callShow.arg = 0xBB02;
        instrs << callShow;
    }

    Bytecode::TableElem func;
    func.type = Bytecode::EL_BELOWMAIN;
    func.algId = func.id = algId;
    func.module = moduleId;
    func.moduleName = mod->header.name;
    func.name = "@below_main";
    func.instructions = instrs.toVector();
    m_bc->d.append(func);

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

    Bytecode::Instruction l;
    l.type = Bytecode::LINE;
    l.arg = alg->impl.headerLexems[0]->lineNo;
    argHandle << l;

    Bytecode::Instruction clearmarg;
    clearmarg.type = Bytecode::CLEARMARG;
    clearmarg.arg = alg->impl.endLexems[0]->lineNo;
    argHandle << clearmarg;

    for (int i=0; i<alg->header.arguments.size(); i++) {
        Bytecode::Instruction store;
        const AST::Variable * var = alg->header.arguments[i];
        if (var->accessType==AST::AccessArgumentIn)
            store.type = Bytecode::STORE;
        else
            store.type = Bytecode::SETREF;
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

    l.arg = alg->impl.beginLexems[0]->lineNo;
    argHandle << l;

    QList<Bytecode::Instruction> pre = instructions(moduleId, id, 0, alg->impl.pre);
    QList<Bytecode::Instruction> body = instructions(moduleId, id, 0, alg->impl.body);
    QList<Bytecode::Instruction> post = instructions(moduleId, id, 0, alg->impl.post);

    int offset = argHandle.size() + pre.size();
    shiftInstructions(body, offset);
    shiftInstructions(post, offset);

    QList<Bytecode::Instruction> ret;

    int retIp = argHandle.size() + pre.size() + body.size() + post.size();

    setBreakAddress(body, 0, retIp);


    Bytecode::Instruction line;
    line.type = Bytecode::LINE;
    if (alg->impl.endLexems.size()>0) {
        line.arg = alg->impl.endLexems[0]->lineNo;
        ret << line;
    }

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
        case AST::StAssignFileStream:
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
            break;
        case AST::StPause:
            PAUSE_STOP(modId, algId, level, st, result);
            break;
        case AST::StHalt:
            PAUSE_STOP(modId, algId, level, st, result);
            break;
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

        int diff = lvalue->operands.size()-lvalue->variable->dimension;

        if (diff>0) {
            // Load source string
            Bytecode::Instruction load;
            findVariable(modId, algId, lvalue->variable, load.scope, load.arg);
            load.type = lvalue->variable->dimension>0? Bytecode::LOADARR : Bytecode::LOAD;
            for (int i=lvalue->variable->dimension-1; i>=0 ;i--) {
                result << calculate(modId, algId, level, lvalue->operands[i]);
            }
            result << load;
        }

        if (diff==1) {
            // Set character

            result << calculate(modId, algId, level,
                                lvalue->operands[lvalue->operands.count()-1]);
            Bytecode::Instruction argsCount;
            argsCount.type = Bytecode::LOAD;
            argsCount.scope = Bytecode::CONST;
            argsCount.arg = constantValue(Bytecode::VT_int, 3);
            result << argsCount;

            Bytecode::Instruction call;
            call.type = Bytecode::CALL;
            call.module = 0xff;
            call.arg = 0x05;
            result << call;
        }

        if (diff==2) {
            // Set slice

            result << calculate(modId, algId, level,
                                lvalue->operands[lvalue->operands.count()-2]);
            result << calculate(modId, algId, level,
                                lvalue->operands[lvalue->operands.count()-1]);
            Bytecode::Instruction argsCount;
            argsCount.type = Bytecode::LOAD;
            argsCount.scope = Bytecode::CONST;
            argsCount.arg = constantValue(Bytecode::VT_int, 4);
            result << argsCount;

            Bytecode::Instruction call;
            call.type = Bytecode::CALL;
            call.module = 0xff;
            call.arg = 0x07;
            result << call;
        }

        Bytecode::Instruction store;
        findVariable(modId, algId, lvalue->variable, store.scope, store.arg);
        store.type = lvalue->variable->dimension>0? Bytecode::STOREARR : Bytecode::STORE;
        if (lvalue->kind==AST::ExprArrayElement) {
            for (int i=lvalue->variable->dimension-1; i>=0 ;i--) {
                result << calculate(modId, algId, level, lvalue->operands[i]);
            }
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

        Bytecode::Instruction instr;
        findVariable(modId, algId, st->variable, instr.scope, instr.arg);
        instr.type = Bytecode::LOAD;
        if (st->variable->dimension>0) {
            for (int i=st->variable->dimension-1; i>=0; i--) {
                result << calculate(modId, algId, level, st->operands[i]);
            }
            instr.type = Bytecode::LOADARR;
        }
        result << instr;
        int diff = st->operands.size() - st->variable->dimension;
        Bytecode::Instruction argsCount;
        argsCount.type = Bytecode::LOAD;
        argsCount.scope = Bytecode::CONST;
        Bytecode::Instruction specialFunction;
        specialFunction.type = Bytecode::CALL;
        specialFunction.module = 0xff;
        if (diff==1) {
            // Get char
            result << calculate(modId, algId, level,
                                st->operands[st->operands.count()-1]);
            argsCount.arg = constantValue(Bytecode::VT_int, 2);
            result << argsCount;
            specialFunction.arg = 0x04;
            result << specialFunction;
        }
        else if (diff==2) {
            // Get slice
            result << calculate(modId, algId, level,
                                st->operands[st->operands.count()-2]);
            result << calculate(modId, algId, level,
                                st->operands[st->operands.count()-1]);
            argsCount.arg = constantValue(Bytecode::VT_int, 3);
            result << argsCount;
            specialFunction.arg = 0x06;
            result << specialFunction;
        }
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
        if (st->operatorr==AST::OpSubstract && st->operands.size()==1) {
            instr.type = Bytecode::NEG;
        }
        result << instr;
    }
    return result;
}

void Generator::PAUSE_STOP(int , int , int , const AST::Statement * st, QList<Bytecode::Instruction> & result)
{
    int lineNo = st->lexems[0]->lineNo;
    Bytecode::Instruction l;
    l.type = Bytecode::LINE;
    l.arg = lineNo;
    result << l;

    Bytecode::Instruction a;
    a.type = st->type==AST::StPause? Bytecode::PAUSE : Bytecode::HALT;
    a.arg = lineNo;
    result << a;
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
        if (var->initialValue.isValid() && var->dimension==0) {
            Bytecode::Instruction load;
            load.type = Bytecode::LOAD;
            load.scope = Bytecode::CONST;
            load.arg = constantValue(valueType(var->baseType), var->initialValue);
            result << load;
            Bytecode::Instruction store = init;
            store.type = Bytecode::STORE;
            result << store;
            Bytecode::Instruction pop;
            pop.type = Bytecode::POP;
            pop.registerr = 0;
            result << pop;
        }
        else {
            // TODO constant values for dimension > 0
        }
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
    else if (st->type==AST::StAssignFileStream) {
        for (int i=st->expressions.size()-1; i>=0; i--) {
            result << calculate(modId, algId, level, st->expressions[i]);
        }
        argsCount = st->expressions.size();
    }
    else {
        QString format;
        for (int i=0; i<st->expressions.size(); i++) {
            AST::Expression * expr = st->expressions[i];
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
        }
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
    else if (st->type==AST::StFileInput)
        call.arg = 0x0002;
    else if (st->type==AST::StFileOutput)
        call.arg = 0x0003;
    else if (st->type==AST::StAssignFileStream)
        call.arg = 0x0A01;

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

    Bytecode::Instruction showreg;
    showreg.type = Bytecode::SHOWREG;
    showreg.registerr = 0;
    result << showreg;

    int jzIP = result.size();
    Bytecode::Instruction jz;
    jz.type = Bytecode::JZ;
    jz.registerr = 0;
    result << jz;

    QList<Bytecode::Instruction> thenInstrs = instructions(modId, algId, level, st->conditionals[0].body);
    shiftInstructions(thenInstrs, result.size());
    result += thenInstrs;
    result[jzIP].arg = result.size();

    if (st->conditionals.size()>1) {
        int jumpIp = result.size();
        Bytecode::Instruction jump;
        jump.type = Bytecode::JUMP;
        result << jump;
        result[jzIP].arg = result.size();
        QList<Bytecode::Instruction> elseInstrs = instructions(modId, algId, level, st->conditionals[1].body);
        shiftInstructions(elseInstrs, result.size());
        result += elseInstrs;
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
            Bytecode::Instruction showreg;
            showreg.type = Bytecode::SHOWREG;
            showreg.registerr = 0;
            result << showreg;
            Bytecode::Instruction jz;
            jz.type = Bytecode::JZ;
            jz.registerr = 0;
            lastJzIp = result.size();
            result << jz;
        }
        QList<Bytecode::Instruction> instrs = instructions(modId, algId, level, st->conditionals[i].body);
        shiftInstructions(instrs, result.size());
        result += instrs;
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
//    jump.type = Bytecode::JUMP;
    jump.type = Bytecode::InstructionType(127);
    jump.registerr = level;

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

    Bytecode::Instruction swreg;
    swreg.type = Bytecode::SHOWREG;
    swreg.registerr = level * 2;

    Bytecode::Instruction clmarg;
    if (st->loop.endLexems.size()>0) {
        clmarg.type = Bytecode::CLEARMARG;
        clmarg.arg = st->loop.endLexems[0]->lineNo;
    }

    int beginIp = result.size();
    int jzIp = -1;


    if (st->loop.type==AST::LoopWhile) {
        // Highlight line and clear margin
        if (lineNo!=-1) {
            result << l;
        }

        if (st->loop.whileCondition) {
            // Calculate condition
            result << calculate(modId, algId, level, st->loop.whileCondition);

            // Check condition result
            Bytecode::Instruction a;
            a.type = Bytecode::POP;
            a.registerr = level * 2;
            result << a;

            if (lineNo!=-1) {
                result << swreg;
            }

            jzIp = result.size();
            a.type = Bytecode::JZ;
            a.registerr = level * 2;
            result << a;

            if (lineNo!=-1) {
                result << clmarg;
            }
        }
        else {
            if (lineNo!=-1) {
                result << clmarg;
            }
        }

    }
    else if (st->loop.type==AST::LoopTimes) {
        // Highlight line
        if (lineNo!=-1) {
            result << l;
        }

        // Calculate times value
        result << calculate(modId, algId, level, st->loop.timesValue);
        Bytecode::Instruction a;

        // Store value in register
        a.type = Bytecode::POP;
        a.registerr = level * 2 - 1;
        result << a;

        // Store initial value "0" in nearest register
        a.type = Bytecode::LOAD;
        a.scope = Bytecode::CONST;
        a.arg = constantValue(Bytecode::VT_int, 0);
        result << a;

        a.type = Bytecode::POP;
        a.registerr = level * 2;
        result << a;


        // Make begin
        beginIp = result.size();

        // Highlight line and clear margin
        if (lineNo!=-1) {
            result << l;
            result << clmarg;
        }

        // Increase value in register
        a.type = Bytecode::PUSH;
        a.registerr = level * 2;
        result << a;
        a.type = Bytecode::LOAD;
        a.scope = Bytecode::CONST;
        a.arg = constantValue(Bytecode::VT_int, 1);
        result << a;
        a.type = Bytecode::SUM;
        result << a;
        a.type = Bytecode::POP;
        a.registerr = level * 2;
        result << a;
        a.type = Bytecode::PUSH;
        a.registerr = level * 2;
        result << a;

        // Compare value to "times" value
        a.type = Bytecode::PUSH;
        a.registerr = level * 2 - 1;
        result << a;
        a.type = Bytecode::GT;
        result << a;
        a.type = Bytecode::POP;
        a.registerr = 0;
        result << a;

        // Check if register value > 0 (i.e. "counter" > "times")
        jzIp = result.size();
        a.type = Bytecode::JNZ;
        a.registerr = 0;
        result << a;

        // Show counter value at margin
        if (lineNo!=-1) {
            result << swreg;
        }
    }
    else if (st->loop.type==AST::LoopFor) {

        // Highlight line
        if (lineNo!=-1) {
            result << l;
        }

        // Calculate and forget initial value
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

        a.type = Bytecode::POP;
        a.registerr = 0;
        result << a;


        // First time: Load variable and 'to'-value
        if (st->loop.forVariable->dimension>0) {
            for (int i=st->loop.forVariable->dimension-1; i>=0; i--) {
                result << calculate(modId, algId, level, st->expressions[i]);
            }
        }
        a.type = st->loop.forVariable->dimension>0? Bytecode::LOADARR : Bytecode::LOAD;
        findVariable(modId, algId, st->loop.forVariable, a.scope, a.arg);
        result << a;

        result << calculate(modId, algId, level, st->loop.toValue);

        // First time: Compare values and go to end if neccessary

        a.type = Bytecode::LEQ;
        result << a;
        a.type = Bytecode::POP;
        a.registerr = 0;
        result << a;
        a.type = Bytecode::JZ;
        a.registerr = 0;
        jzIp = result.size();
        result << a;

        // Begin
        beginIp = result.size();
    }

    QList<Bytecode::Instruction> instrs = instructions(modId, algId, level, st->loop.body);
    shiftInstructions(instrs, result.size());
    result += instrs;

    int endJzIp = -1;

    if (st->loop.endCondition) {
        lineNo = st->loop.endLexems[0]->lineNo;
        l.arg = lineNo;
        result << l;
        result << calculate(modId, algId, level, st->loop.endCondition);
        Bytecode::Instruction e;
        e.type = Bytecode::POP;
        e.registerr = 0;
        result << e;
        // Show counter value at margin
        if (lineNo!=-1) {
            result << swreg;
        }
        endJzIp = result.size();
        e.type = Bytecode::JNZ;
        e.registerr = 0;
        result << e;
    }    
    else {

    }

    int jzIp2 = -1;

    if (st->loop.type==AST::LoopFor) {
        // Change loop variable must be done at end of loop!
        l.arg = st->lexems[0]->lineNo;
        result << l;

        // Change variable
        if (st->loop.forVariable->dimension>0) {
            for (int i=st->loop.forVariable->dimension-1; i>=0; i--) {
                result << calculate(modId, algId, level, st->expressions[i]);
            }
        }
        Bytecode::Instruction a;
        a.type = st->loop.forVariable->dimension>0 ? Bytecode::LOADARR : Bytecode::LOAD;
        findVariable(modId, algId, st->loop.forVariable, a.scope, a.arg);
        result << a;

        if (st->loop.stepValue) {
            result << calculate(modId, algId, level, st->loop.stepValue);
        }
        else {
            a.type = Bytecode::LOAD;
            a.scope = Bytecode::CONST;
            a.arg = constantValue(Bytecode::VT_int, 1);
            result << a;
        }
        a.type = Bytecode::SUM;
        result << a;
        a.type = Bytecode::POP;
        a.registerr = level * 2;
        result << a;
        a.type = Bytecode::PUSH;
        result << a;

        // Load 'to'-value
        result << calculate(modId, algId, level, st->loop.toValue);

        // Compare

        a.type = Bytecode::LEQ;
        result << a;
        a.type = Bytecode::POP;
        a.registerr = 0;
        result << a;
        a.type = Bytecode::JZ;
        a.registerr = 0;
        jzIp2 = result.size();
        result << a;

        // Clear margin
        if (lineNo!=-1) {
            result << clmarg;
        }

        // Store variable

        a.type = Bytecode::PUSH;
        a.registerr = level * 2;
        result << a;

        if (st->loop.forVariable->dimension>0) {
            for (int i=st->loop.forVariable->dimension-1; i>=0; i--) {
                result << calculate(modId, algId, level, st->expressions[i]);
            }
        }
        a.type = st->loop.forVariable->dimension>0 ? Bytecode::STOREARR : Bytecode::STORE;
        findVariable(modId, algId, st->loop.forVariable, a.scope, a.arg);
        result << a;
    }

    // Jump to loop begin
//    lineNo = st->loop.endLexems[0]->lineNo;
//    l.arg = lineNo;
//    result << l;
    Bytecode::Instruction e;
    e.type = Bytecode::JUMP;
    e.registerr = 0;
    e.arg = beginIp;
    result << e;


    // Found end of loop
    if (jzIp!=-1) {
        result[jzIp].arg = result.size();
    }
    if (endJzIp!=-1) {
        result[endJzIp].arg = result.size();
    }
    if (jzIp2!=-1) {
        result[jzIp2].arg = result.size();
    }

    setBreakAddress(result, level, result.size());

}

void Generator::setBreakAddress(QList<Bytecode::Instruction> &instrs,
                                int level,
                                int address)
{
    for (int i=0; i<instrs.size(); i++) {
        if (int(instrs[i].type)==127 && instrs[i].registerr==level) {
            instrs[i].type = Bytecode::JUMP;
            instrs[i].arg = address;
        }
    }
}

} // namespace KumirCodeGenerator
