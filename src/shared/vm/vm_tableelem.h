#ifndef BYTECODE_TABLEELEM_H
#define BYTECODE_TABLEELEM_H

#include "vm_instruction.h"

#include <string>
#include <sstream>
#include <boost/any.hpp>
#include <vector>

namespace Bytecode {

enum ElemType {
    EL_NONE     = 0x00,     // stream delimeter
    EL_LOCAL    = 0x01,       // Local variable
    EL_GLOBAL   = 0x02,      // Global variable
    EL_CONST    = 0x03,       // Constant
    EL_FUNCTION = 0x04,    // Local defined function
    EL_EXTERN   = 0x05,      // External module name
    EL_INIT     = 0x06,     // Local defined module initializer
    EL_MAIN     = 0x07,     // Main (usually - first) function
    EL_TESTING  = 0x08,      // Testing function
    EL_BELOWMAIN= 0x09      // Function evaluated below main function
};

enum ValueType {
    VT_void     = 0x00,
    VT_int      = 0x01,
    VT_float    = 0x02,
    VT_char     = 0x03,
    VT_bool     = 0x04,
    VT_string   = 0x05
};

enum ValueKind {
    VK_Plain    = 0x00,
    VK_In       = 0x01,
    VK_InOut    = 0x02,
    VK_Out      = 0x03
};

struct TableElem {
    ElemType type; // Element type
    ValueType vtype; // Value type
    uint8_t dimension; // 0 for regular, [1..3] for arrays
    ValueKind refvalue; // 1 for in-argument,
                     // 2 for in/out-argument,
                     // 3 for out-argument
                     // 0 for regular variable
    uint8_t module;  // module id
    uint16_t algId; // algorhitm id
    uint16_t id; // element id

    std::string name; // variable or function name

    std::string moduleName; // external module name
    boost::any constantValue; // constant value
    std::vector<Instruction> instructions; // for local defined function

};

inline void stringToDataStream(std::ostream& stream, const std::string & str)
{
    uint16_t size = uint16_t(str.length());
    stream << size;
    for (size_t i=0; i<str.length(); i++)
        stream << str.at(i);
}

inline void stringFromDataStream(std::istream& stream, std::string & str)
{
    uint16_t u16size;
    stream >> u16size;
    if (stream.fail())
        throw std::string("Can't read sting value from data stream: first byte is not a string size");
    size_t size = size_t(u16size);
    str.resize(size);
    char ch = '\0';
    for (size_t i=0; i<size; i++) {
        stream >> ch;
        if (stream.fail())
            throw std::string("Can't read string value from data stream: stream ends before value read");
        str.at(i) = ch;
    }
}

template <typename T> inline void valueToDataStream(std::ostream& stream, T value)
{
    stream.write(reinterpret_cast<char*>(&value), sizeof(T));
}

template <typename T> inline void valueFromDataStream(std::istream & stream, T &value)
{
    stream.read(reinterpret_cast<char*>(&value), sizeof(T));
}

inline void tableElemToBinaryStream(std::ostream & ds, const TableElem &e)
{
    valueToDataStream(ds, uint8_t(e.type));
    valueToDataStream(ds, uint8_t(e.vtype));
    valueToDataStream(ds, uint8_t(e.dimension));
    valueToDataStream(ds, uint8_t(e.refvalue));
    valueToDataStream(ds, uint8_t(e.module));
    valueToDataStream(ds, uint16_t(e.algId));
    valueToDataStream(ds, uint16_t(e.id));
    stringToDataStream(ds, e.name);
    if (e.type==EL_EXTERN)
        stringToDataStream(ds, e.moduleName);
    else if (e.type==EL_CONST) {
        if (e.vtype==VT_int) {
            const int32_t val = boost::any_cast<int32_t>(e.constantValue);
            valueToDataStream(ds, val);
        }
        else if (e.vtype==VT_float) {
            const double val = boost::any_cast<double>(e.constantValue);
            valueToDataStream(ds, val);
        }
        else if (e.vtype==VT_bool) {
            const bool val = boost::any_cast<bool>(e.constantValue);
            valueToDataStream(ds, val? uint8_t(1) : uint8_t(0));
        }
        else {
            // character or string
            // NOTE: for character type always use 'string' type
            //       because of possible multibyte encoding
            const std::string val = boost::any_cast<std::string>(e.constantValue);
            stringToDataStream(ds, val);
        }
    }
    else if (e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_TESTING || e.type==EL_BELOWMAIN || e.type==EL_INIT) {
        valueToDataStream(ds, uint16_t(e.instructions.size()));
        for (int i=0; i<e.instructions.size(); i++) {
            valueToDataStream(ds, toUint32(e.instructions[i]));
        }
    }
}

inline void tableElemFromBinaryStream(std::istream & ds, TableElem &e)
{
    uint8_t t;
    uint8_t v;
    uint8_t d;
    uint8_t r;
    uint8_t m;
    uint16_t a;
    uint16_t id;
    std::string s;
    valueFromDataStream(ds, t);
    valueFromDataStream(ds, v);
    valueFromDataStream(ds, d);
    valueFromDataStream(ds, r);
    valueFromDataStream(ds, m);
    valueFromDataStream(ds, a);
    valueFromDataStream(ds, id);
    if (ds.fail())
        throw std::string("Can't read block from data stream");
    stringFromDataStream(ds, s);
    e.type = ElemType(t);
    e.vtype = ValueType(v);
    e.dimension = d;
    e.refvalue = ValueKind(r);
    e.module = m;
    e.algId = a;
    e.id = id;
    e.name = s;
    if (e.type==EL_EXTERN) {
        stringFromDataStream(ds, s);
        e.moduleName = s;
    }
    else if (e.type==EL_CONST) {
        if (e.vtype==VT_int) {
            int32_t iv;
            valueFromDataStream(ds, iv);
            e.constantValue = iv;
        }
        else if (e.vtype==VT_float) {
            double fv;
            valueFromDataStream(ds, fv);
            e.constantValue = fv;
        }
        else if (e.vtype==VT_bool) {
            uint8_t bv;
            valueFromDataStream(ds, bv);
            e.constantValue = bool(bv>0? true : false);
        }
        else {
            stringFromDataStream(ds, s);
            e.constantValue = s;
        }
    }
    else if (e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_TESTING || e.type==EL_BELOWMAIN || e.type==EL_INIT) {
        uint16_t u16sz;
        valueFromDataStream(ds, u16sz);
        size_t sz = size_t(u16sz);
        e.instructions.resize(sz);
        for (size_t i=0; i<sz; i++) {
            uint32_t instr;
            valueFromDataStream(ds, instr);
            e.instructions[i] = fromUint32(instr);
        }
    }
}

inline std::string elemTypeToString(ElemType t)
{
    if (t==EL_LOCAL)
        return ".local";
    else if (t==EL_GLOBAL)
        return ".global";
    else if (t==EL_CONST)
        return ".constant";
    else if (t==EL_FUNCTION)
        return ".function";
    else if (t==EL_EXTERN)
        return ".extern";
    else if (t==EL_INIT)
        return ".init";
    else if (t==EL_MAIN)
        return ".main";
    else if (t==EL_BELOWMAIN)
        return ".belowmain";
    else if (t==EL_TESTING)
        return ".testing";
    else  {
        return "";
    }
}

inline ElemType elemTypeFromString(std::string s)
{
    boost::algorithm::to_lower(s);
    if (s==".local")
        return EL_LOCAL;
    else if (s==".global")
        return EL_GLOBAL;
    else if (s==".constant")
        return EL_CONST;
    else if (s==".function")
        return EL_FUNCTION;
    else if (s==".extern")
        return EL_EXTERN;
    else if (s==".init")
        return EL_INIT;
    else if (s==".main")
        return EL_MAIN;
    else if (s==".belowmain")
        return EL_BELOWMAIN;
    else if (s==".testing")
        return EL_TESTING;
    else {
        return ElemType(0);
    }
}



inline std::string vtypeToString(ValueType t)
{
    if (t==VT_int)
        return "int";
    else if (t==VT_float)
        return "float";
    else if (t==VT_char)
        return "char";
    else if (t==VT_string)
        return "string";
    else if (t==VT_bool)
        return "bool";
    else
        return "";
}

inline ValueType vtypeFromString(std::string s)
{
    boost::algorithm::to_lower(s);
    if (s=="int")
        return VT_int;
    else if (s=="float")
        return VT_float;
    else if (s=="char")
        return VT_char;
    else if (s=="string")
        return VT_string;
    else if (s=="bool")
        return VT_bool;
    else
        return VT_void;
}

inline std::string kindToString(ValueKind k)
{
    if (k==VK_Plain)
        return "var";
    else if (k==VK_In)
        return "in";
    else if (k==VK_InOut)
        return "inout";
    else if (k==VK_Out)
        return "out";
    else  {
        throw std::string("Unknown type");
        return "";
    }
}

inline ValueKind kindFromString(std::string s)
{
    boost::algorithm::to_lower(s);
    if (s=="in")
        return VK_In;
    else if (s=="inout")
        return VK_InOut;
    else if (s=="out")
        return VK_Out;
    else
        return VK_Plain;
}

inline void replaceAll(std::string &str, const std::string & from, const std::string & to)
{
    size_t fromSize = from.length();
    size_t startPos = 0;
    while ((startPos = str.find(from, startPos))!=std::string::npos) {
        str.replace(startPos, fromSize, to);
        startPos += fromSize;
    }
}

inline std::string screenString(std::string s)
{
    replaceAll(s, "\\", "\\\\");
    replaceAll(s, "\n", "\\n");
    replaceAll(s, "\"", "\\\"");
    replaceAll(s, " ", "\\s");
    replaceAll(s, "\t", "\\t");
    return s;
}

inline std::string unscreenString(std::string s)
{
    replaceAll(s, "\\n", "\n");
    replaceAll(s, "\\\"", "\"");
    replaceAll(s, "\\s", " ");
    replaceAll(s, "\\t", "\t");
    replaceAll(s, "\\\\", "\\");
    return s;
}

inline void tableElemToTextStream(std::ostream &ts, const TableElem &e)
{
    ts << elemTypeToString(e.type) << " ";
    if (e.type==EL_LOCAL || e.type==EL_GLOBAL || e.type==EL_CONST) {
        ts << vtypeToString(e.vtype) << " ";
        ts << e.dimension <<" ";
    }
    if (e.type==EL_LOCAL || e.type==EL_GLOBAL)
        ts << kindToString(e.refvalue) << " ";

    if (e.type==EL_LOCAL || e.type==EL_GLOBAL || e.type==EL_EXTERN || e.type==EL_FUNCTION || e.type==EL_MAIN ||e.type==EL_BELOWMAIN|| e.type==EL_INIT || e.type==EL_TESTING) {
        ts << e.module << " ";
    }
    if (e.type==EL_LOCAL) {
        ts << e.algId << " ";
    }
    if (e.type!=EL_INIT)
        ts << e.id << " ";
    if (e.type==EL_EXTERN)
        ts << "\"" << screenString(e.moduleName) << "\" ";
    else if (e.type==EL_CONST) {
        if (e.vtype==VT_int) {
            const int32_t val = boost::any_cast<int32_t>(e.constantValue);
            ts << val;
        }
        else if (e.vtype==VT_float) {
            const double val = boost::any_cast<double>(e.constantValue);
            ts << val;
        }
        else if (e.vtype==VT_bool) {
            const bool val = boost::any_cast<bool>(e.constantValue);
            ts << val? "true" : "false";
        }
        else {
            // character or string
            // NOTE: for character type always use 'string' type
            //       because of possible multibyte encoding
            const std::string val = boost::any_cast<std::string>(e.constantValue);
            stringToDataStream(ts, val);
        }
    }
    if (e.type==EL_LOCAL||e.type==EL_GLOBAL||e.type==EL_GLOBAL||e.type==EL_FUNCTION||e.type==EL_MAIN||e.type==EL_BELOWMAIN||e.type==EL_FUNCTION||e.type==EL_EXTERN) {
        ts << "\"" << screenString(e.name) << "\" ";
    }
    if (e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_TESTING ||e.type==EL_BELOWMAIN|| e.type==EL_INIT) {
        ts << e.instructions.size();
        for (int i=0; i<e.instructions.size(); i++) {
            ts << "\n";
            ts << i << ":\t";
            ts << instructionToString(e.instructions[i]);
        }
    }
    ts << "\n";
}

inline void tableElemFromTextStream(std::istream& ts, TableElem& e)
{
    e.type = EL_NONE;
    std::string header;
    while (!ts.eof()) {
        std::string line;
        std::getline(ts, line);
        if (line.length()>0 && line.at(0)!='#') {
            header = line;
            break;
        }
    }
    if (header=="") {
        return;
    }
    std::istringstream is(header);
    std::string lexem;
    is >> lexem;
    if (is.fail())
        throw std::string("Empty table element header: "+header);
    e.type = elemTypeFromString(lexem);
    if (e.type==EL_LOCAL || e.type==EL_GLOBAL || e.type==EL_CONST) {
        is >> lexem;
        if (is.fail())
            throw std::string("Empty table element type: "+header);
        e.vtype = vtypeFromString(lexem);
        uint8_t dim;
        is >> dim;
        if (is.fail())
            throw std::string("Wrong table element dimension: "+header);
        e.dimension = dim;
    }
    if (e.type==EL_LOCAL || e.type==EL_GLOBAL)
        is >> lexem;
        if (is.fail())
            throw std::string("Empty table element scope: "+header);
        e.refvalue = kindFromString(lexem);

    if (e.type==EL_LOCAL || e.type==EL_GLOBAL || e.type==EL_EXTERN || e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_INIT ||e.type==EL_BELOWMAIN|| e.type==EL_TESTING) {
        uint8_t val;
        is >> val;
        if (is.fail())
            throw std::string("Wrong table element module ID: "+header);
        e.module = val;
    }
    if (e.type==EL_LOCAL) {
        uint16_t val;
        is >> val;
        if (is.fail())
            throw std::string("Wrong table element algorithm ID: "+header);
        e.algId = val;
    }
    if (e.type!=EL_INIT) {
        uint16_t val;
        is >> val;
        if (is.fail())
            throw std::string("Wrong table element ID: "+header);
        e.id = val;
    }
    if (e.type==EL_EXTERN) {
        is >> lexem;
        if (is.fail())
            throw std::string("Empty table element extern declaration: "+header);
        if (lexem.length()<3)
            throw std::string("Wrong table element extern declaration: "+header);
        e.moduleName = unscreenString(lexem.substr(1, lexem.length()-2));
    }
    if (e.type==EL_CONST) {
        if (e.vtype==VT_int) {
            int32_t val;
            is >> val;
            if (is.fail())
                throw std::string("Wrong integer constant: "+header);
            e.constantValue = val;
        }
        else if (e.vtype==VT_float) {
            double val;
            is >> val;
            if (is.fail())
                throw std::string("Wrong dobule-precision floating point constant: "+header);
            e.constantValue = val;
        }
        else if (e.vtype==VT_bool) {
            is >> lexem;
            if (is.fail())
                throw std::string("Boolean constant value is empty: "+header);
            boost::algorithm::to_lower(lexem);
            if (lexem=="true" || lexem=="1")
                e.constantValue = bool(true);
            else if (lexem=="false" || lexem=="0")
                e.constantValue = bool(false);
            else
                throw std::string("Wrong boolean constant: "+header);
        }
        else {
            is >> lexem;
            if (is.fail())
                throw std::string("Literal constant value is empty: "+header);
            if (lexem.length()<2) {
                throw std::string("Wrong literal constant: "+header);
            }
            e.constantValue = unscreenString(lexem.substr(1, lexem.length()-2));
        }
    }
    else if (e.type==EL_LOCAL||e.type==EL_GLOBAL||e.type==EL_GLOBAL||e.type==EL_FUNCTION||e.type==EL_MAIN||e.type==EL_TESTING||e.type==EL_BELOWMAIN||e.type==EL_EXTERN) {
        is >> lexem;
        if (is.fail())
            throw std::string("Name is empty: "+header);
        if (lexem.length()<2) {
            throw std::string("Wrong name: "+header);
        }
        e.name = unscreenString(lexem.substr(1,lexem.length()-2));
    }
    if (e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_TESTING || e.type==EL_BELOWMAIN || e.type==EL_INIT) {
        size_t size;
        is >> size;
        if (is.fail()) {
            throw std::string("Wrong element size: "+header);
        }
        e.instructions.resize(size);
        size_t index = 0;
        while (index<size && !ts.eof()) {
            std::string line;
            std::getline(ts, line);
            if (line.length()==0 || line.at(0)=='#')
                continue;
            size_t colonPos = line.find(":");
            line = line.substr(colonPos+1);
            e.instructions[index] = instructionFromString(line);
            index++;
        }
    }
}


} // namespace Bytecode

#endif // BYTECODE_TABLEELEM_H
