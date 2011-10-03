#include "bc_instruction.h"

namespace Bytecode {


QString typeToString(InstructionType t) {
    if (t==NOP) return QString::fromAscii("Nop").toLower();
    else if (t==CALL) return QString::fromAscii("Call").toLower();
    else if (t==INIT) return QString::fromAscii("Init").toLower();
    else if (t==SETARR) return QString::fromAscii("SetArr").toLower();
    else if (t==STORE) return QString::fromAscii("Store").toLower();
    else if (t==STOREARR) return QString::fromAscii("StoreArr").toLower();
    else if (t==LOAD) return QString::fromAscii("Load").toLower();
    else if (t==LOADARR) return QString::fromAscii("LoadArr").toLower();
    else if (t==SETMON) return QString::fromAscii("SetMon").toLower();
    else if (t==UNSETMON) return QString::fromAscii("UnsetMon").toLower();
    else if (t==JUMP) return QString::fromAscii("Jump").toLower();
    else if (t==JNZ) return QString::fromAscii("Jnz").toLower();
    else if (t==JZ) return QString::fromAscii("Jz").toLower();
    else if (t==POP) return QString::fromAscii("Pop").toLower();
    else if (t==PUSH) return QString::fromAscii("Push").toLower();
    else if (t==RET) return QString::fromAscii("Return").toLower();
    else if (t==PAUSE) return QString::fromAscii("Pause").toLower();
    else if (t==ERROR) return QString::fromAscii("Error").toLower();
    else if (t==SUM) return QString::fromAscii("Sum").toLower();
    else if (t==SUB) return QString::fromAscii("Sub").toLower();
    else if (t==MUL) return QString::fromAscii("Mul").toLower();
    else if (t==DIV) return QString::fromAscii("Div").toLower();
    else if (t==POW) return QString::fromAscii("Pow").toLower();
    else if (t==NEG) return QString::fromAscii("Neg").toLower();
    else if (t==AND) return QString::fromAscii("And").toLower();
    else if (t==OR) return QString::fromAscii("Or").toLower();
    else if (t==EQ) return QString::fromAscii("Eq").toLower();
    else if (t==NEQ) return QString::fromAscii("Neq").toLower();
    else if (t==LS) return QString::fromAscii("Ls").toLower();
    else if (t==GT) return QString::fromAscii("Gt").toLower();
    else if (t==LEQ) return QString::fromAscii("Leq").toLower();
    else if (t==GEQ) return QString::fromAscii("Geq").toLower();
    else if (t==GEQ) return QString::fromAscii("Geq").toLower();
    else if (t==REF) return QString::fromAscii("Ref").toLower();
    else if (t==REFARR) return QString::fromAscii("RefArr").toLower();
    else if (t==LINE) return QString::fromAscii("Line").toLower();
    else if (t==SHOWREG) return QString::fromAscii("ShowReg").toLower();
    else if (t==CLEARMARG) return QString::fromAscii("ClearMarg").toLower();
    else if (t==SETREF) return QString::fromAscii("SetRef").toLower();
    else if (t==PAUSE) return QString::fromAscii("Pause").toLower();
    else if (t==HALT) return QString::fromAscii("Halt").toLower();
    else return "nop";
}

InstructionType typeFromString(const QString & s) {
    if (s.toLower()==QString::fromAscii("Nop").toLower()) return NOP;
    else if (s.toLower()==QString::fromAscii("Call").toLower()) return CALL;
    else if (s.toLower()==QString::fromAscii("Init").toLower()) return INIT;
    else if (s.toLower()==QString::fromAscii("SetArr").toLower()) return SETARR;
    else if (s.toLower()==QString::fromAscii("Store").toLower()) return STORE;
    else if (s.toLower()==QString::fromAscii("StoreArr").toLower()) return STOREARR;
    else if (s.toLower()==QString::fromAscii("Load").toLower()) return LOAD;
    else if (s.toLower()==QString::fromAscii("LoadArr").toLower()) return LOADARR;
    else if (s.toLower()==QString::fromAscii("SetMon").toLower()) return SETMON;
    else if (s.toLower()==QString::fromAscii("UnsetMon").toLower()) return UNSETMON;
    else if (s.toLower()==QString::fromAscii("Jump").toLower()) return JUMP;
    else if (s.toLower()==QString::fromAscii("Jnz").toLower()) return JNZ;
    else if (s.toLower()==QString::fromAscii("Jz").toLower()) return JZ;
    else if (s.toLower()==QString::fromAscii("Pop").toLower()) return POP;
    else if (s.toLower()==QString::fromAscii("Push").toLower()) return PUSH;
    else if (s.toLower()==QString::fromAscii("Return").toLower()) return RET;
    else if (s.toLower()==QString::fromAscii("Pause").toLower()) return PAUSE;
    else if (s.toLower()==QString::fromAscii("Error").toLower()) return ERROR;
    else if (s.toLower()==QString::fromAscii("Sum").toLower()) return SUM;
    else if (s.toLower()==QString::fromAscii("Sub").toLower()) return SUB;
    else if (s.toLower()==QString::fromAscii("Mul").toLower()) return MUL;
    else if (s.toLower()==QString::fromAscii("Div").toLower()) return DIV;
    else if (s.toLower()==QString::fromAscii("Pow").toLower()) return POW;
    else if (s.toLower()==QString::fromAscii("Neg").toLower()) return NEG;
    else if (s.toLower()==QString::fromAscii("And").toLower()) return AND;
    else if (s.toLower()==QString::fromAscii("Or").toLower()) return OR;
    else if (s.toLower()==QString::fromAscii("Eq").toLower()) return EQ;
    else if (s.toLower()==QString::fromAscii("Neq").toLower()) return NEQ;
    else if (s.toLower()==QString::fromAscii("Ls").toLower()) return LS;
    else if (s.toLower()==QString::fromAscii("Gt").toLower()) return GT;
    else if (s.toLower()==QString::fromAscii("Leq").toLower()) return LEQ;
    else if (s.toLower()==QString::fromAscii("Geq").toLower()) return GEQ;
    else if (s.toLower()==QString::fromAscii("Ref").toLower()) return REF;
    else if (s.toLower()==QString::fromAscii("RefArr").toLower()) return REFARR;
    else if (s.toLower()==QString::fromAscii("Line").toLower()) return LINE;
    else if (s.toLower()==QString::fromAscii("ShowReg").toLower()) return SHOWREG;
    else if (s.toLower()==QString::fromAscii("ClearMarg").toLower()) return CLEARMARG;
    else if (s.toLower()==QString::fromAscii("SetRef").toLower()) return SETREF;
    else if (s.toLower()==QString::fromAscii("Pause").toLower()) return PAUSE;
    else if (s.toLower()==QString::fromAscii("Halt").toLower()) return HALT;
    else return NOP;
}

static const QSet<InstructionType> VariableInstructions = QSet<InstructionType>()
<< INIT << SETARR << STORE << STOREARR << LOAD << LOADARR << SETMON << UNSETMON << REF << REFARR << SETREF;

static const QSet<InstructionType> ModuleNoInstructions = QSet<InstructionType>()
<< CALL;

static const QSet<InstructionType> RegisterNoInstructions = QSet<InstructionType>()
<< POP << PUSH << JZ << JNZ << SHOWREG;

static const QSet<InstructionType> HasValueInstructions = QSet<InstructionType>()
<< CALL << INIT << SETARR << STORE << STOREARR << LOAD << LOADARR
   << SETMON << UNSETMON << JUMP << JNZ << JZ << ERROR << LINE << REF << REFARR << CLEARMARG << SETREF << HALT << PAUSE;



extern QString instructionToString(const Instruction &instr)
{
    QString result;
    InstructionType t = instr.type;
    result += typeToString(t);
    if (ModuleNoInstructions.contains(t)) {
        result += " "+QString::number( instr.module );
    }
    if (VariableInstructions.contains(t)) {
        VariableScope s = instr.scope;
        if (s==GLOBAL)
            result += " global";
        else if (s==LOCAL)
            result += " local";
        else if (s==CONST)
            result += " constant";
    }
    if (HasValueInstructions.contains(t)) {
        result += " " + QString::number(instr.arg);
    }
    if (RegisterNoInstructions.contains(t)) {
        result += " " + QString::number(instr.registerr);
    }
    return result;
}

extern quint32 toUint32(const Instruction &instr)
{
    quint32 first = quint8(instr.type);
    first = first << 24;
    quint32 second;
    if (ModuleNoInstructions.contains(instr.type))
        second = quint8(instr.module);
    else if (RegisterNoInstructions.contains(instr.type))
        second = quint8(instr.registerr);
    else
        second = quint8(instr.scope);
    second = second << 16;
    quint32 last = instr.arg;
    last = last << 16; // Ensure first two bytes are 0x0000
    last = last >> 16;
    quint32 result = first | second | last;
    return result;
}

extern Instruction instructionFromString(const QString &str)
{
    InstructionType t = NOP;
    quint8 mod = 0;
    quint8 reg = 0;
    VariableScope s = UNDEF;
    quint16 val = 0;
    const QStringList lexems = str.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if (lexems.size()>0)
        t = typeFromString(lexems[0]);
    if (lexems.size()>1) {
        if (VariableInstructions.contains(t)) {
            if (lexems[1].toLower().startsWith("g")) {
                s = GLOBAL;
            }
            else if (lexems[1].toLower().startsWith("l")) {
                s = LOCAL;
            }
            else if (lexems[1].toLower().startsWith("c")) {
                s = CONST;
            }
        }
        else if (ModuleNoInstructions.contains(t)) {
            mod = lexems[1].toUShort();
        }
        else if (RegisterNoInstructions.contains(t)) {
            reg = lexems[1].toUShort();
        }
    }
    if (lexems.size()>2) {
        if (HasValueInstructions.contains(t)) {
            val = lexems[2].toUInt();
        }
    }
    Instruction i;
    i.type = t;
    if (s!=UNDEF)
        i.scope = s;
    else if (ModuleNoInstructions.contains(t))
        i.module = mod;
    else
        i.registerr = reg;
    i.arg = val;
    return i;
}

extern Instruction fromUint32(quint32 value)
{
    quint32 first  = value & 0xFF000000;
    quint32 second = value & 0x00FF0000;
    quint32 last   = value & 0x0000FFFF;
    first = first >> 24;
    second = second >> 16;
    Instruction i;
    i.type = InstructionType(first);
    if (ModuleNoInstructions.contains(i.type))
        i.module = quint8(second);
    else if (RegisterNoInstructions.contains(i.type))
        i.registerr = quint8(second);
    else
        i.scope = VariableScope(second);
    i.arg = quint16(last);
    return i;
}


}
