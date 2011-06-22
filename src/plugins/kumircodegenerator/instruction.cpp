#include "instruction.h"

namespace BytecodeGenerator {


extern QDataStream &operator<<(QDataStream &ds, const Instruction &instr)
{
    ds << quint32(instr.type);
    ds << instr.arg;
    return ds;
}

extern QDataStream &operator>>(QDataStream &ds, Instruction &instr)
{
    quint32 a;
    ds >> a;
    ds >> instr.arg;
    instr.type = InstructionType(a);
    return ds;
}

QString typeToString(InstructionType t) {
    if (t==Null) return QString::fromAscii("Null").toLower();
    else if (t==Call) return QString::fromAscii("Call").toLower();
    else if (t==CallInt) return QString::fromAscii("CallInt").toLower();
    else if (t==Init) return QString::fromAscii("Init").toLower();
    else if (t==SetArr) return QString::fromAscii("SetArr").toLower();
    else if (t==Store) return QString::fromAscii("Store").toLower();
    else if (t==StoreArr) return QString::fromAscii("StoreArr").toLower();
    else if (t==Get) return QString::fromAscii("Get").toLower();
    else if (t==GetArr) return QString::fromAscii("GetArr").toLower();
    else if (t==SetMon) return QString::fromAscii("SetMon").toLower();
    else if (t==UnsetMon) return QString::fromAscii("UnsetMon").toLower();
    else if (t==Jump) return QString::fromAscii("Jump").toLower();
    else if (t==Jnz) return QString::fromAscii("Jnz").toLower();
    else if (t==Jz) return QString::fromAscii("Jz").toLower();
    else if (t==Pop) return QString::fromAscii("Pop").toLower();
    else if (t==Push) return QString::fromAscii("Push").toLower();
    else if (t==Break) return QString::fromAscii("Break").toLower();
    else if (t==Return) return QString::fromAscii("Return").toLower();
    else if (t==PushC) return QString::fromAscii("PushC").toLower();
    else if (t==Pause) return QString::fromAscii("Pause").toLower();
    else if (t==Error) return QString::fromAscii("Error").toLower();
    else if (t==Sum) return QString::fromAscii("Sum").toLower();
    else if (t==Sub) return QString::fromAscii("Sub").toLower();
    else if (t==Mul) return QString::fromAscii("Mul").toLower();
    else if (t==Div) return QString::fromAscii("Div").toLower();
    else if (t==Pow) return QString::fromAscii("Pow").toLower();
    else if (t==Neg) return QString::fromAscii("Neg").toLower();
    else if (t==And) return QString::fromAscii("And").toLower();
    else if (t==Or) return QString::fromAscii("Or").toLower();
    else if (t==Eq) return QString::fromAscii("Eq").toLower();
    else if (t==Neq) return QString::fromAscii("Neq").toLower();
    else if (t==Ls) return QString::fromAscii("Ls").toLower();
    else if (t==Gt) return QString::fromAscii("Gt").toLower();
    else if (t==Leq) return QString::fromAscii("Leq").toLower();
    else if (t==Geq) return QString::fromAscii("Geq").toLower();
    else return "Null";
}

InstructionType typeFromString(const QString & s) {
    if (s.toLower()==QString::fromAscii("Null").toLower()) return Null;
    else if (s.toLower()==QString::fromAscii("Call").toLower()) return Call;
    else if (s.toLower()==QString::fromAscii("CallInt").toLower()) return CallInt;
    else if (s.toLower()==QString::fromAscii("Init").toLower()) return Init;
    else if (s.toLower()==QString::fromAscii("SetArr").toLower()) return SetArr;
    else if (s.toLower()==QString::fromAscii("Store").toLower()) return Store;
    else if (s.toLower()==QString::fromAscii("StoreArr").toLower()) return StoreArr;
    else if (s.toLower()==QString::fromAscii("Get").toLower()) return Get;
    else if (s.toLower()==QString::fromAscii("GetArr").toLower()) return GetArr;
    else if (s.toLower()==QString::fromAscii("SetMon").toLower()) return SetMon;
    else if (s.toLower()==QString::fromAscii("UnsetMon").toLower()) return UnsetMon;
    else if (s.toLower()==QString::fromAscii("Jump").toLower()) return Jump;
    else if (s.toLower()==QString::fromAscii("Jnz").toLower()) return Jnz;
    else if (s.toLower()==QString::fromAscii("Jz").toLower()) return Jz;
    else if (s.toLower()==QString::fromAscii("Pop").toLower()) return Pop;
    else if (s.toLower()==QString::fromAscii("Push").toLower()) return Push;
    else if (s.toLower()==QString::fromAscii("Break").toLower()) return Break;
    else if (s.toLower()==QString::fromAscii("Return").toLower()) return Return;
    else if (s.toLower()==QString::fromAscii("PushC").toLower()) return PushC;
    else if (s.toLower()==QString::fromAscii("Pause").toLower()) return Pause;
    else if (s.toLower()==QString::fromAscii("Error").toLower()) return Error;
    else if (s.toLower()==QString::fromAscii("Sum").toLower()) return Sum;
    else if (s.toLower()==QString::fromAscii("Sub").toLower()) return Sub;
    else if (s.toLower()==QString::fromAscii("Mul").toLower()) return Mul;
    else if (s.toLower()==QString::fromAscii("Div").toLower()) return Div;
    else if (s.toLower()==QString::fromAscii("Pow").toLower()) return Pow;
    else if (s.toLower()==QString::fromAscii("Neg").toLower()) return Neg;
    else if (s.toLower()==QString::fromAscii("And").toLower()) return And;
    else if (s.toLower()==QString::fromAscii("Or").toLower()) return Or;
    else if (s.toLower()==QString::fromAscii("Eq").toLower()) return Eq;
    else if (s.toLower()==QString::fromAscii("Neq").toLower()) return Neq;
    else if (s.toLower()==QString::fromAscii("Ls").toLower()) return Ls;
    else if (s.toLower()==QString::fromAscii("Gt").toLower()) return Gt;
    else if (s.toLower()==QString::fromAscii("Leq").toLower()) return Leq;
    else if (s.toLower()==QString::fromAscii("Geq").toLower()) return Geq;
    else return Null;
}

extern QTextStream &operator<<(QTextStream &ts, const Instruction &instr)
{
    ts << "\t" << typeToString(instr.type) << " " << QString::number(instr.arg) << "\n";
    return ts;
}

extern QTextStream &operator>>(QTextStream &ts, Instruction &instr)
{
    instr.type = Null;
    instr.arg = 0;
    const QString line = ts.readLine();
    const QStringList lexems = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if (lexems.size()>0)
        instr.type = typeFromString(lexems[0]);
    if (lexems.size()>1)
        instr.arg = quint32(lexems[1].toULong());
    return ts;
}


} // namespace BytecodeGenerator
