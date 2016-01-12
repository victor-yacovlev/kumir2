#ifndef BYTECODEGENERATOR_INSTRUCTION_H
#define BYTECODEGENERATOR_INSTRUCTION_H

#include <string>
#include <sstream>
#include <set>
#include <algorithm>
#define DO_NOT_DECLARE_STATIC
#include "stdlib/kumirstdlib.hpp"

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
    ERRORR       = 0x1E, // Abort evaluation
    LINE        = 0x1F, // Emit line number
    REF         = 0x20, // Get reference to variable
    REFARR      = 0x21, // Get reference to array element
    SHOWREG     = 0x22, // Show register value at margin
    CLEARMARG   = 0x23, // Clear margin text from current line to specified
    SETREF      = 0x24, // Set reference value to variable
    HALT        = 0x26, // Terminate
    CTL         = 0x27, // Control VM behaviour
    INRANGE     = 0x28, // Pops 4 values ... a, b, c, x from stack and returns c>=0? a<x<=b : a<=x<b
    UPDARR      = 0x29, // Updates array bounds

    CSTORE      = 0x30, // Copy value from stack head and push it to cache
    CLOAD       = 0x31, // Pop value from cache to push it to main stack
    CDROPZ      = 0x32, // Drop cache value in case of zero value in specified register

    CACHEBEGIN  = 0x33, // Push begin marker into cache
    CACHEEND    = 0x34, // Clear cache until marker


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
    CONSTT = 0x01, // Value from constants table
    LOCAL = 0x02, // Value from locals table
    GLOBAL= 0x03  // Value from globals table
};

enum LineSpecification {
    LINE_NUMBER = 0x00,
    COLUMN_START_AND_END = 0x80
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
        LineSpecification lineSpec;
        VariableScope scope;
        uint8_t module;
        uint8_t registerr;
    };
    uint16_t arg;
};


inline std::string typeToString(const InstructionType & t)
{
    if (t==NOP) return ("nop");
    else if (t==CALL) return ("call");
    else if (t==INIT) return ("init");
    else if (t==SETARR) return ("setarr");
    else if (t==STORE) return ("store");
    else if (t==STOREARR) return ("storearr");
    else if (t==LOAD) return ("load");
    else if (t==LOADARR) return ("loadarr");
    else if (t==SETMON) return ("setmon");
    else if (t==UNSETMON) return ("unsetmon");
    else if (t==JUMP) return ("jump");
    else if (t==JNZ) return ("jnz");
    else if (t==JZ) return ("jz");
    else if (t==POP) return ("pop");
    else if (t==PUSH) return ("push");
    else if (t==RET) return ("return");
    else if (t==PAUSE) return ("pause");
    else if (t==ERRORR) return ("error");
    else if (t==SUM) return ("sum");
    else if (t==SUB) return ("sub");
    else if (t==MUL) return ("mul");
    else if (t==DIV) return ("div");
    else if (t==POW) return ("pow");
    else if (t==NEG) return ("neg");
    else if (t==AND) return ("and");
    else if (t==OR) return ("or");
    else if (t==EQ) return ("eq");
    else if (t==NEQ) return ("neq");
    else if (t==LS) return ("ls");
    else if (t==GT) return ("gt");
    else if (t==LEQ) return ("leq");
    else if (t==GEQ) return ("geq");
    else if (t==REF) return ("ref");
    else if (t==REFARR) return ("refarr");
    else if (t==LINE) return ("line");
    else if (t==SHOWREG) return ("showreg");
    else if (t==CLEARMARG) return ("clearmarg");
    else if (t==SETREF) return ("setref");
    else if (t==PAUSE) return ("pause");
    else if (t==HALT) return ("halt");
    else if (t==CTL) return ("ctl");
    else if (t==INRANGE) return ("inrange");
    else if (t==UPDARR) return ("updarr");
    else if (t==CLOAD) return ("cload");
    else if (t==CSTORE) return ("cstore");
    else if (t==CDROPZ) return ("cdropz");
    else if (t==CACHEBEGIN) return ("cachebegin");
    else if (t==CACHEEND) return ("cacheend");
    else return "nop";
}

inline InstructionType typeFromString(const std::string & ss)
{
    std::string s = Kumir::Core::toLowerCase(ss);
    if (s=="nop") return NOP;
    else if (s=="call") return CALL;
    else if (s=="init") return INIT;
    else if (s=="setarr") return SETARR;
    else if (s=="store") return STORE;
    else if (s=="storearr") return STOREARR;
    else if (s=="load") return LOAD;
    else if (s=="loadarr") return LOADARR;
    else if (s=="setmon") return SETMON;
    else if (s=="unsetmon") return UNSETMON;
    else if (s=="jump") return JUMP;
    else if (s=="jnz") return JNZ;
    else if (s=="jz") return JZ;
    else if (s=="pop") return POP;
    else if (s=="push") return PUSH;
    else if (s=="return") return RET;
    else if (s=="pause") return PAUSE;
    else if (s=="error") return ERRORR;
    else if (s=="sum") return SUM;
    else if (s=="sub") return SUB;
    else if (s=="mul") return MUL;
    else if (s=="div") return DIV;
    else if (s=="pow") return POW;
    else if (s=="neg") return NEG;
    else if (s=="and") return AND;
    else if (s=="or") return OR;
    else if (s=="eq") return EQ;
    else if (s=="neq") return NEQ;
    else if (s=="ls") return LS;
    else if (s=="gt") return GT;
    else if (s=="leq") return LEQ;
    else if (s=="geq") return GEQ;
    else if (s=="ref") return REF;
    else if (s=="refarr") return REFARR;
    else if (s=="line") return LINE;
    else if (s=="showreg") return SHOWREG;
    else if (s=="clearmarg") return CLEARMARG;
    else if (s=="setref") return SETREF;
    else if (s=="pause") return PAUSE;
    else if (s=="halt") return HALT;
    else if (s=="ctl") return CTL;
    else if (s=="inrange") return INRANGE;
    else if (s=="updarr") return UPDARR;
    else if (s=="cload") return CLOAD;
    else if (s=="cstore") return CSTORE;
    else if (s=="cdropz") return CDROPZ;
    else if (s=="cachebegin") return CACHEBEGIN;
    else if (s=="cacheend") return CACHEEND;
    else return NOP;
}

typedef std::pair<uint32_t, uint16_t> AS_Key;
    // module | algorithm, id
typedef std::map<AS_Key, std::string> AS_Values;
struct AS_Helpers {
    AS_Values globals;
    AS_Values locals;
    AS_Values constants;
    AS_Values algorithms;
};

inline bool extractColumnPositionsFromLineInstruction(const Instruction & instr,
                                                      uint32_t &from,
                                                      uint32_t &to)
{
    if (instr.type == LINE && (instr.scope & COLUMN_START_AND_END) != 0) {
        uint32_t value = ((instr.lineSpec & 0x3F) << 16) | instr.arg;
        from = value >> 11;
        to = value & 0x7FF;
        return true;
    }
    else {
        from = to = 0u;
        return false;
    }
}

inline Instruction& setColumnPositionsToLineInstruction(Instruction & instr,
                                                        uint32_t from,
                                                        uint32_t to)
{
    uint32_t value = (from << 11) | to;
    uint16_t arg = value & 0xFFFF;
    uint8_t scope = (value >> 16) & 0xFF;
    scope = scope | COLUMN_START_AND_END;
    instr.type = LINE;
    instr.lineSpec = LineSpecification(scope);
    instr.arg = arg;
    return instr;
}

inline std::string instructionToString(const Instruction &instr, const AS_Helpers & helpers, uint8_t moduleId, uint16_t algId)
{
    static std::set<InstructionType> VariableInstructions;
    VariableInstructions.insert(INIT);
    VariableInstructions.insert(SETARR);
    VariableInstructions.insert(STORE);
    VariableInstructions.insert(STOREARR);
    VariableInstructions.insert(LOAD);
    VariableInstructions.insert(LOADARR);
    VariableInstructions.insert(SETMON);
    VariableInstructions.insert(UNSETMON);
    VariableInstructions.insert(REF);
    VariableInstructions.insert(REFARR);
    VariableInstructions.insert(SETREF);
    VariableInstructions.insert(UPDARR);

    static std::set<InstructionType> ModuleNoInstructions;
    ModuleNoInstructions.insert(CALL);
    ModuleNoInstructions.insert(CTL);
    
    static std::set<InstructionType> RegisterNoInstructions;
    RegisterNoInstructions.insert(POP);
    RegisterNoInstructions.insert(PUSH);
    RegisterNoInstructions.insert(JZ);
    RegisterNoInstructions.insert(JNZ);
    RegisterNoInstructions.insert(SHOWREG);    

    static std::set<InstructionType> HasValueInstructions;
    HasValueInstructions.insert(CALL);
    HasValueInstructions.insert(INIT);
    HasValueInstructions.insert(SETARR);
    HasValueInstructions.insert(STORE);
    HasValueInstructions.insert(STOREARR);
    HasValueInstructions.insert(LOAD);
    HasValueInstructions.insert(LOADARR);
    HasValueInstructions.insert(SETMON);
    HasValueInstructions.insert(UNSETMON);
    HasValueInstructions.insert(JUMP);
    HasValueInstructions.insert(JNZ);
    HasValueInstructions.insert(JZ);
    HasValueInstructions.insert(ERRORR);
    HasValueInstructions.insert(LINE);
    HasValueInstructions.insert(REF);
    HasValueInstructions.insert(REFARR);
    HasValueInstructions.insert(CLEARMARG);
    HasValueInstructions.insert(SETREF);
    HasValueInstructions.insert(HALT);
    HasValueInstructions.insert(PAUSE);
    HasValueInstructions.insert(CTL);
    HasValueInstructions.insert(UPDARR);

    std::stringstream result;
    result.setf(std::ios::hex,std::ios::basefield);
    result.setf(std::ios::showbase);
    InstructionType t = instr.type;
    result << typeToString(t);
    if (ModuleNoInstructions.count(t)) {
        result << " " << int(instr.module);
    }
    if (VariableInstructions.count(t)) {
        VariableScope s = instr.scope;
        if (s==GLOBAL)
            result << " global";
        else if (s==LOCAL)
            result << " local";
        else if (s==CONSTT)
            result << " constant";
    }
    if (t == LINE) {
        uint32_t from, to;
        result.unsetf(std::ios::basefield);
        result.unsetf(std::ios::showbase);
        if (extractColumnPositionsFromLineInstruction(instr, from, to)) {
            result << " col " << int(from) << "-" << int(to);
        }
        else {
            result << " no " << int(instr.arg);
        }
    }
    if (HasValueInstructions.count(t) && t!=LINE) {
        if (t==JUMP || t==JNZ || t==JZ) {
            result.unsetf(std::ios::basefield);
            result.unsetf(std::ios::showbase);
        }
        result << " " << int(instr.arg);
    }
    if (RegisterNoInstructions.count(t)) {
        result << " " << int(instr.registerr);
    }
    std::string r = result.str();

    if (VariableInstructions.count(t)) {
        VariableScope s = instr.scope;
        AS_Key akey;
        const AS_Values * vals = nullptr;
        akey.first = 0;
        if (s==LOCAL) {
            akey.first = (moduleId << 16) | algId;
            akey.second = instr.arg;
            vals = &(helpers.locals);
        }
        else if (s==GLOBAL) {
            akey.first = (moduleId << 16);
            akey.second = instr.arg;
            vals = &(helpers.globals);
        }
        else if (s==CONSTT) {
            akey.first = 0;
            akey.second = instr.arg;
            vals = &(helpers.constants);
        }
        if (vals && vals->count(akey)) {
            while (r.length()<25)
                r.push_back(' ');
            r += std::string("# ") + vals->at(akey);
        }
    }
    else if (t==CALL) {
        AS_Key akey (instr.module << 16, instr.arg);
        const AS_Values * vals = &(helpers.algorithms);
        if (vals && vals->count(akey)) {
            while (r.length()<25)
                r.push_back(' ');
            r += std::string("# ") + vals->at(akey);
        }
    }
    return r;
}

inline uint32_t toUint32(const Instruction &instr)
{
    static std::set<InstructionType> ModuleNoInstructions;
    ModuleNoInstructions.insert(CALL);
    ModuleNoInstructions.insert(CTL);

    static std::set<InstructionType> RegisterNoInstructions;
    RegisterNoInstructions.insert(POP);
    RegisterNoInstructions.insert(PUSH);
    RegisterNoInstructions.insert(JZ);
    RegisterNoInstructions.insert(JNZ);
    RegisterNoInstructions.insert(SHOWREG);

    uint32_t first = uint8_t(instr.type);
    first = first << 24;
    uint32_t second = 0u;
    if (ModuleNoInstructions.count(instr.type))
        second = uint8_t(instr.module);
    else if (RegisterNoInstructions.count(instr.type))
        second = uint8_t(instr.registerr);
    else
        second = uint8_t(instr.scope);
    if (instr.type == LINE) {
        second = uint8_t(instr.lineSpec);
    }
    second = second << 16;
    uint32_t last = instr.arg;
    last = last << 16; // Ensure first two bytes are 0x0000
    last = last >> 16;
    uint32_t result = first | second | last;
    return result;
}

inline Instruction fromUint32(uint32_t value)
{
    static std::set<InstructionType> ModuleNoInstructions;
    ModuleNoInstructions.insert(CALL);
    ModuleNoInstructions.insert(CTL);

    static std::set<InstructionType> RegisterNoInstructions;
    RegisterNoInstructions.insert(POP);
    RegisterNoInstructions.insert(PUSH);
    RegisterNoInstructions.insert(JZ);
    RegisterNoInstructions.insert(JNZ);
    RegisterNoInstructions.insert(SHOWREG);

    uint32_t first  = value & 0xFF000000;
    uint32_t second = value & 0x00FF0000;
    uint32_t last   = value & 0x0000FFFF;
    first = first >> 24;
    second = second >> 16;
    Instruction i;
    i.type = InstructionType(first);
    if (ModuleNoInstructions.count(i.type))
        i.module = uint8_t(second);
    else if (RegisterNoInstructions.count(i.type))
        i.registerr = uint8_t(second);
    else
        i.scope = VariableScope(second);
    if (i.type == LINE) {
        i.lineSpec = LineSpecification(second);
    }
    i.arg = uint16_t(last);
    return i;
}


} // end namespace Bytecode

#endif // BYTECODEGENERATOR_INSTRUCTION_H
