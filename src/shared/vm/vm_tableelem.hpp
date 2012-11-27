#ifndef BYTECODE_TABLEELEM_H
#define BYTECODE_TABLEELEM_H

#define DO_NOT_DECLARE_STATIC
#include "stdlib/kumirstdlib.hpp"

#include <string>
#include <sstream>
#include <vector>
#include "variant.hpp"
#include "vm_instruction.hpp"
#include "vm_enums.h"

namespace Bytecode {

using Kumir::real;
using Kumir::String;
using VM::Variable;


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

    String name; // variable or function name

    String moduleName; // external module name
    Variable initialValue; // constant value
    std::vector<Instruction> instructions; // for local defined function

    inline TableElem() {
        type = EL_NONE;
        vtype = VT_void;
        dimension = 0;
        refvalue = VK_Plain;
        module = 0;
        algId = 0;
        id = 0;
    }
};

inline bool isLittleEndian() {
    uint16_t test = 0xFF00;
    char * buf = reinterpret_cast<char*>(&test);
    return buf[0]==0x00;
}

template <typename T> inline void valueToDataStream(std::list<char> & stream, T value)
{
    static const bool le = isLittleEndian();
    const char * buf = reinterpret_cast<char*>(&value);
    if (le) {
        for (int i=sizeof(T)-1; i>=0; i--) {
            stream.push_back(buf[i]);
        }
    }
    else {
        for (int i=0; i<sizeof(T); i++) {
            stream.push_back(buf[i]);
        }
    }
}

template <typename T> inline void valueFromDataStream(std::list<char> & stream, T &value)
{
    char buf[sizeof(T)];
    static const bool le = isLittleEndian();
    if (le) {
        for (int i=sizeof(value)-1; i>=0 ; i--) {
            buf[i] = stream.front();
            stream.pop_front();
        }
    }
    else {
        for (int i=0; i<sizeof(value); i++) {
            buf[i] = stream.front();
            stream.pop_front();
        }
    }
    const T * t = reinterpret_cast<T*>(&buf);
    value = *t;
}

inline void stdStringToDataStream(std::list<char>& stream, const std::string & str)
{
    uint16_t size = uint16_t(str.length());
    valueToDataStream(stream, size);
    for (int i=0; i<str.length(); i++) {
        stream.push_back(str[i]);
    }
}

inline void stringToDataStream(std::list<char>& stream, const String & str) {
    const std::string utf = Kumir::Coder::encode(Kumir::UTF8, str);
    stdStringToDataStream(stream, utf);
}

inline void stdStringFromDataStream(std::list<char>& stream, std::string & str)
{
    uint16_t u16size;
    valueFromDataStream(stream, u16size);
    size_t size = size_t(u16size);
    str.resize(size);
    for (size_t i=0; i<size; i++) {
        str[i] = stream.front();
        stream.pop_front();
    }
}

inline void stringFromDataStream(std::list<char>& stream, String & str)
{
    std::string utf;
    stdStringFromDataStream(stream, utf);
    str = Kumir::Coder::decode(Kumir::UTF8, utf);
}

inline void scalarConstantToDataStream(std::list<char> & stream, ValueType type, const Variable & val) {
    switch (type) {
    case VT_int: {
        const int32_t ival = val.toInt();
        valueToDataStream(stream, ival);
        break;
    }
    case VT_real: {
        const double rval = val.toReal();
        valueToDataStream(stream, rval);
        break;
    }
    case VT_bool: {
        const uint8_t bval = val.toBool()? 1 : 0;
        valueToDataStream(stream, bval);
        break;
    }
    case VT_char: {
        const String & cval = val.toString();
        stringToDataStream(stream, cval);
        break;
    }
    case VT_string: {
        const String & sval = val.toString();
        stringToDataStream(stream, sval);
        break;
    }
    default:
        break;
    }
}

inline void constantToDataStream(std::list<char> & stream, ValueType baseType, const Variable & val, uint8_t dimension) {
    if (dimension==0)
        scalarConstantToDataStream(stream, baseType, val);
    else {
        int bounds[7];
        val.getBounds(bounds);
        for (int i=0; i<7; i++) {
            int32_t bound32 = static_cast<int32_t>(bounds[i]);
            valueToDataStream(stream, bound32);
        }
        int32_t rawSize = val.rawSize();
        valueToDataStream(stream, rawSize);
        for (size_t i=0; i<val.rawSize(); i++) {
            VM::AnyValue v = val[i];
            uint8_t defined = v.isValid()? 1 : 0;
            valueToDataStream(stream, defined);
            if (defined==1)
                scalarConstantToDataStream(stream, baseType, VM::Variable(v));
        }
    }
}

inline void tableElemToBinaryStream(std::list<char> & ds, const TableElem &e)
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
        constantToDataStream(ds, e.vtype, e.initialValue, e.dimension);
    }
    else if (e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_TESTING || e.type==EL_BELOWMAIN || e.type==EL_INIT) {
        valueToDataStream(ds, uint16_t(e.instructions.size()));
        for (int i=0; i<e.instructions.size(); i++) {
            valueToDataStream(ds, toUint32(e.instructions[i]));
        }
    }
}

inline void scalarConstantFromDataStream(std::list<char> & stream, ValueType type, Variable & val)
{
    switch (type) {
    case VT_int: {
        int32_t ival;
        valueFromDataStream(stream, ival);
        val = Variable(ival);
        break;
    }
    case VT_real: {
        double rval;
        valueFromDataStream(stream, rval);
        val = Variable(rval);
        break;
    }
    case VT_bool: {
        uint8_t bval;
        valueFromDataStream(stream, bval);
        val = Variable(bool(bval>0? true : false));
        break;
    }
    case VT_char: {
        String cval;
        stringFromDataStream(stream, cval);
        val = Variable(Kumir::Char(cval.at(0)));
        break;
    }
    case VT_string: {
        Kumir::String sval;
        stringFromDataStream(stream, sval);
        val = Variable(sval);
        break;
    }
    default:
        break;
    }
}

inline void constantFromDataStream(std::list<char> & stream,
                                   ValueType baseType,
                                   Variable & val,
                                   uint8_t dimension )
{
    if (dimension==0)
        scalarConstantFromDataStream(stream, baseType, val);
    else {
        val.setDimension(dimension);
        int bounds[7];
        for (int i=0; i<7; i++) {
            int32_t bounds32;
            valueFromDataStream(stream, bounds32);
            bounds[i] = static_cast<int>(bounds32);
        }
        val.setBounds(bounds);
        val.init();
        int32_t rawSize32;
        valueFromDataStream(stream, rawSize32);
        size_t rawSize = static_cast<size_t>(rawSize32);
        for (size_t i=0; i<rawSize; i++) {
            uint8_t defined;
            valueFromDataStream(stream, defined);
            if (defined==1) {
                Variable element;
                scalarConstantFromDataStream(stream, baseType, element);
                val[i] = element.value();
            }
        }
    }
}

inline void tableElemFromBinaryStream(std::list<char> & ds, TableElem &e)
{
    uint8_t t;
    uint8_t v;
    uint8_t d;
    uint8_t r;
    uint8_t m;
    uint16_t a;
    uint16_t id;
    String s;
    valueFromDataStream(ds, t);
    valueFromDataStream(ds, v);
    valueFromDataStream(ds, d);
    valueFromDataStream(ds, r);
    valueFromDataStream(ds, m);
    valueFromDataStream(ds, a);
    valueFromDataStream(ds, id);
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
        constantFromDataStream(ds, e.vtype, e.initialValue, e.dimension);
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

inline ElemType elemTypeFromString(const std::string &ss)
{
    const std::string s = Kumir::Core::toLowerCase(ss);
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
    else if (t==VT_real)
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

inline ValueType vtypeFromString(const std::string &ss)
{
    const std::string s = Kumir::Core::toLowerCase(ss);
    if (s=="int")
        return VT_int;
    else if (s=="float")
        return VT_real;
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

inline ValueKind kindFromString(const std::string &ss)
{
    const std::string s = Kumir::Core::toLowerCase(ss);
    if (s=="in")
        return VK_In;
    else if (s=="inout")
        return VK_InOut;
    else if (s=="out")
        return VK_Out;
    else
        return VK_Plain;
}

inline void replaceAll(String &str, const String & from, const String & to)
{
    size_t fromSize = from.length();
    size_t startPos = 0;
    while ((startPos = str.find(from, startPos))!=String::npos) {
        str.replace(startPos, fromSize, to);
        startPos += fromSize;
    }
}

inline String screenString(String s)
{
    replaceAll(s, Kumir::Core::fromAscii("\\"), Kumir::Core::fromAscii("\\\\"));
    replaceAll(s, Kumir::Core::fromAscii("\n"), Kumir::Core::fromAscii("\\n"));
    replaceAll(s, Kumir::Core::fromAscii("\""), Kumir::Core::fromAscii("\\\""));
    replaceAll(s, Kumir::Core::fromAscii(" "), Kumir::Core::fromAscii("\\s"));
    replaceAll(s, Kumir::Core::fromAscii("\t"), Kumir::Core::fromAscii("\\t"));
    return s;
}

inline String unscreenString(String s)
{
    replaceAll(s, Kumir::Core::fromAscii("\\n"), Kumir::Core::fromAscii("\n"));
    replaceAll(s, Kumir::Core::fromAscii("\\\""), Kumir::Core::fromAscii("\""));
    replaceAll(s, Kumir::Core::fromAscii("\\s"), Kumir::Core::fromAscii(" "));
    replaceAll(s, Kumir::Core::fromAscii("\\t"), Kumir::Core::fromAscii("\t"));
    replaceAll(s, Kumir::Core::fromAscii("\\\\"), Kumir::Core::fromAscii("\\"));
    return s;
}

inline void tableElemToTextStream(std::ostream &ts, const TableElem &e)
{
    ts << elemTypeToString(e.type) << " ";
    if (e.type==EL_LOCAL || e.type==EL_GLOBAL || e.type==EL_CONST) {
        ts << vtypeToString(e.vtype) << " ";
        ts << int(e.dimension) <<" ";
    }
    if (e.type==EL_LOCAL || e.type==EL_GLOBAL)
        ts << kindToString(e.refvalue) << " ";

    if (e.type==EL_LOCAL || e.type==EL_GLOBAL || e.type==EL_EXTERN || e.type==EL_FUNCTION || e.type==EL_MAIN ||e.type==EL_BELOWMAIN|| e.type==EL_INIT || e.type==EL_TESTING) {
        ts << int(e.module) << " ";
    }
    if (e.type==EL_LOCAL) {
        ts << int(e.algId) << " ";
    }
    if (e.type!=EL_INIT)
        ts << int(e.id) << " ";
    if (e.type==EL_EXTERN)
        ts << "\"" << Kumir::Coder::encode(Kumir::UTF8, screenString(e.moduleName)) << "\" ";
    else if (e.type==EL_CONST) {
        if (e.vtype==VT_int) {
            const int32_t val = e.initialValue.toInt();
            ts << val;
        }
        else if (e.vtype==VT_real) {
            const double val = e.initialValue.toDouble();
            ts << val;
        }
        else if (e.vtype==VT_bool) {
            const bool val = e.initialValue.toBool();
            ts << val? "true" : "false";
        }
        else {
            ts << "\"" << Kumir::Coder::encode(Kumir::UTF8, screenString(e.initialValue.toString())) << "\"";
        }
    }
    if (e.type==EL_LOCAL||e.type==EL_GLOBAL||e.type==EL_GLOBAL||e.type==EL_FUNCTION||e.type==EL_MAIN||e.type==EL_BELOWMAIN||e.type==EL_FUNCTION||e.type==EL_EXTERN) {
        ts << "\"" << Kumir::Coder::encode(Kumir::UTF8, screenString(e.name)) << "\" ";
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
        e.moduleName = unscreenString(Kumir::Coder::decode(Kumir::UTF8, lexem.substr(1, lexem.length()-2)));
    }
    if (e.type==EL_CONST) {
        if (e.vtype==VT_int) {
            int32_t val;
            is >> val;
            if (is.fail())
                throw std::string("Wrong integer constant: "+header);
            e.initialValue = Variable(val);
        }
        else if (e.vtype==VT_real) {
            double val;
            is >> val;
            if (is.fail())
                throw std::string("Wrong dobule-precision floating point constant: "+header);
            e.initialValue = Variable(val);
        }
        else if (e.vtype==VT_bool) {
            is >> lexem;
            if (is.fail())
                throw std::string("Boolean constant value is empty: "+header);
            lexem = Kumir::Core::toLowerCase(lexem);
            if (lexem=="true" || lexem=="1")
                e.initialValue = Variable(bool(true));
            else if (lexem=="false" || lexem=="0")
                e.initialValue = Variable(bool(false));
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
            e.initialValue = Variable(unscreenString(Kumir::Coder::decode(Kumir::UTF8, lexem.substr(1, lexem.length()-2))));
        }
    }
    else if (e.type==EL_LOCAL||e.type==EL_GLOBAL||e.type==EL_GLOBAL||e.type==EL_FUNCTION||e.type==EL_MAIN||e.type==EL_TESTING||e.type==EL_BELOWMAIN||e.type==EL_EXTERN) {
        is >> lexem;
        if (is.fail())
            throw std::string("Name is empty: "+header);
        if (lexem.length()<2) {
            throw std::string("Wrong name: "+header);
        }
        e.name = unscreenString(Kumir::Coder::decode(Kumir::UTF8, lexem.substr(1,lexem.length()-2)));
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
