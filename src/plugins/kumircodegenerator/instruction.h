#ifndef BYTECODEGENERATOR_INSTRUCTION_H
#define BYTECODEGENERATOR_INSTRUCTION_H

#include <QtCore>

namespace BytecodeGenerator {

enum InstructionType {
    Null        = 0x0000,
    Call        = 0x000A, // Call compiled function
    CallInt     = 0x000B, // Call external function
    Init        = 0x000C, // Initialize variable
    SetArr      = 0x000D, // Set array bounds
    Store       = 0x000E, // Store value in variable
    StoreArr    = 0x000F, // Store value in array
    Get         = 0x0010, // Get value from variable
    GetArr      = 0x0011, // Get value froma array
    SetMon      = 0x0012, // Set variable monitor
    UnsetMon    = 0x0013, // Unset variable monitor
    Jump        = 0x0014, // Unconditional jump
    Jnz         = 0x0015, // Conditional jump if non-zero
    Jz          = 0x0016, // Conditional jump if zero
    Pop         = 0x0018, // Pop from stack to accumulator
    Push        = 0x0019, // Push to stack from accumulator
    Break       = 0x001A, // Break ???
    Return      = 0x001B, // Return from function
    PushC       = 0x001C, // Push value to stack
    Pause       = 0x001D, // Force pause ???
    Error       = 0x001E, // Abort evaluation

    // Common operations -- no comments need

    Sum         = 0xFF01,
    Sub         = 0xFF02,
    Mul         = 0xFF03,
    Div         = 0xFF04,
    Pow         = 0xFF05,
    Neg         = 0xFF06,
    And         = 0xFF07,
    Or          = 0xFF08,
    Eq          = 0xFF09,
    Neq         = 0xFF0A,
    Ls          = 0xFF0B,
    Gt          = 0xFF0C,
    Leq         = 0xFF0D,
    Geq         = 0xFF0E
};

struct Instruction {
    inline Instruction() { type = Null; arg = 0; }
    inline Instruction(InstructionType t, quint32 a) { type=t; arg=a; }
    inline Instruction(const Instruction &other) { type=other.type; arg=other.arg; }
    InstructionType type;
    quint32 arg;
};

extern QDataStream &operator<<(QDataStream &, const Instruction &);
extern QDataStream &operator>>(QDataStream &, Instruction &);

extern QTextStream &operator<<(QTextStream &, const Instruction &);
extern QTextStream &operator>>(QTextStream &, Instruction &);


} // namespace BytecodeGenerator

#endif // BYTECODEGENERATOR_INSTRUCTION_H
