#ifndef BYTECODEGENERATOR_INSTRUCTION_H
#define BYTECODEGENERATOR_INSTRUCTION_H

#include <QtCore>

#ifdef DATAFORMATS_LIBRARY
#   define BYTECODE_EXPORT Q_DECL_EXPORT
#else
#   define BYTECODE_EXPORT Q_DECL_IMPORT
#endif

namespace Bytecode {

enum InstructionType {
    NOP         = 0x00,
    CALL        = 0x0A, // Call compiled function
    INIT        = 0x0C, // Initialize variable
    SETARR      = 0x0D, // Set array bounds
    STORE       = 0x0E, // Store value in variable
    STOREARR    = 0x0F, // Store value in array
    LOAD        = 0x10, // Get value from variable
    LOADARR     = 0x11, // Get value froma array
    SETMON      = 0x12, // Set variable monitor
    UNSETMON    = 0x13, // Unset variable monitor
    JUMP        = 0x14, // Unconditional jump
    JNZ         = 0x15, // Conditional jump if non-zero value in specified register
    JZ          = 0x16, // Conditional jump if zero value in specified register
    POP         = 0x18, // Pop from stack to register
    PUSH        = 0x19, // Push to stack from register
    RET         = 0x1B, // Return from function
    PAUSE       = 0x1D, // Force pause
    ERROR       = 0x1E, // Abort evaluation
    LINE        = 0x1F, // Emit line number
    REF         = 0x20, // Get reference to variable
    REFARR      = 0x21, // Get reference to array element
    SHOWREG     = 0x22, // Show register value at margin
    CLEARMARG   = 0x23, // Clear margin text from current line to specified


    // Common operations -- no comments need

    SUM         = 0xF1,
    SUB         = 0xF2,
    MUL         = 0xF3,
    DIV         = 0xF4,
    POW         = 0xF5,
    NEG         = 0xF6,
    AND         = 0xF7,
    OR          = 0xF8,
    EQ          = 0xF9,
    NEQ         = 0xFA,
    LS          = 0xFB,
    GT          = 0xFC,
    LEQ         = 0xFD,
    GEQ         = 0xFE
};

enum VariableScope {
    UNDEF = 0x00, // Undefined if not need
    CONST = 0x01, // Value from constants table
    LOCAL = 0x02, // Value from locals table
    GLOBAL= 0x03  // Value from globals table
};

/* Instruction has optimal (aka serialized) size of 32 bit:
  - first byte is Instruction Type
  - second byte is Module Number (for CALL),
    register number (for POP, PUSH, JZ and JNZ)
    variable scope (for variable-access instructions) or
    undefined (e.g. value is 0x00)
  - last two bytes is instruction argument value
    (or 0x0000 if not defined)
*/

struct Instruction {
    InstructionType type;
    union {
        VariableScope scope;
        quint8 module;
        quint8 registerr;
    };
    quint16 arg;
};


extern BYTECODE_EXPORT QString instructionToString(const Instruction &);
extern BYTECODE_EXPORT Instruction instructionFromString(const QString &);

extern BYTECODE_EXPORT quint32 toUint32(const Instruction &);
extern BYTECODE_EXPORT Instruction fromUint32(quint32);

}

#endif // BYTECODEGENERATOR_INSTRUCTION_H
