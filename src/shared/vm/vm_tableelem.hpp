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
        algId = id = 0;
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



inline std::string vtypeToString(ValueType t, uint8_t dim)
{
    std::string result;
    if (t==VT_int)
        result = "int";
    else if (t==VT_real)
        result = "real";
    else if (t==VT_char)
        result = "char";
    else if (t==VT_string)
        result = "string";
    else if (t==VT_bool)
        result = "bool";
    else
        result = "";
    if (result.length()>0) {
        for (uint8_t i=0; i<dim; i++) {
            result += "[]";
        }
    }
    return result;
}

inline void vtypeFromString(const std::string &ss, ValueType &t, uint8_t &dim)
{
    size_t brackPos = ss.find_first_of('[');
    std::string s = Kumir::Core::toLowerCase(ss.substr(0, brackPos));
    Kumir::StringUtils::trim<std::string,char>(s);
    if (s=="int")
        t = VT_int;
    else if (s=="real")
        t = VT_real;
    else if (s=="char")
        t = VT_char;
    else if (s=="string")
        t = VT_string;
    else if (s=="bool")
        t = VT_bool;
    else
        t = VT_void;
    dim = 0;
    if (brackPos!=std::string::npos) {
        std::string brackets = s.substr(brackPos);
        for (size_t i=0; i<brackets.length(); i++) {
            if (brackets[i]=='[')
                dim++;
        }
    }

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

inline std::string constantToTextStream(const TableElem & e)
{
    std::ostringstream os;
    os << ".constant id=" << e.id << " type=" << vtypeToString(e.vtype, e.dimension) << " value=";
    if (e.vtype==VT_int) {
        const int32_t val = e.initialValue.toInt();
        os << val;
    }
    else if (e.vtype==VT_real) {
        const double val = e.initialValue.toDouble();
        os << val;
    }
    else if (e.vtype==VT_bool) {
        const bool val = e.initialValue.toBool();
        os << val? "true" : "false";
    }
    else {
        os << "\"" << Kumir::Coder::encode(Kumir::UTF8, screenString(e.initialValue.toString())) << "\"";
    }
    return os.str();
}

inline std::string localToTextStream(const TableElem & e)
{
    std::ostringstream os;
    os << ".local kind=" << kindToString(e.refvalue) << " type=" << vtypeToString(e.vtype, e.dimension) << " ";
    os << "module=" << int(e.module) << " algorithm=" << e.algId << " id=" << e.id;
    if (e.name.length()>0) {
        os << " name=\"" << Kumir::Coder::encode(Kumir::UTF8, screenString(e.name)) << "\"";
    }
    return os.str();
}

inline std::string globalToTextStream(const TableElem & e)
{
    std::ostringstream os;
    os << ".global type:" << vtypeToString(e.vtype, e.dimension) << " ";
    os << "module=" << int(e.module) << " id=" << e.id;
    if (e.name.length()>0) {
        os << " name=\"" << Kumir::Coder::encode(Kumir::UTF8, screenString(e.name)) << "\"";
    }
    return os.str();
}

inline std::string functionToTextStream(const TableElem & e)
{
    std::ostringstream os;
    os << elemTypeToString(e.type) << " ";
    os << "module=" << int(e.module) << " id=" << e.id << " size=" << e.instructions.size();
    if (e.name.length()>0) {
        os << " name=\"" << Kumir::Coder::encode(Kumir::UTF8, screenString(e.name)) << "\"";
    }
    os << "\n";
    for (size_t i=0; i<e.instructions.size(); i++) {
        os << i << ":\t";
        os << instructionToString(e.instructions[i]);
        os << "\n";
    }
    return os.str();
}

inline std::string externToTextStream(const TableElem & e)
{
    std::ostringstream os;
    os << ".extern ";
    os << "module=";
    os << "\"" << Kumir::Coder::encode(Kumir::UTF8, screenString(e.moduleName)) << "\"";
    os << " function=";
    os << "\"" << Kumir::Coder::encode(Kumir::UTF8, screenString(e.name)) << "\"";
}

inline void tableElemToTextStream(std::ostream &ts, const TableElem &e)
{
    if (e.type==EL_CONST)
        ts << constantToTextStream(e);
    else if (e.type==EL_EXTERN)
        ts << externToTextStream(e);
    else if (e.type==EL_LOCAL)
        ts << localToTextStream(e);
    else if (e.type==EL_GLOBAL)
        ts << globalToTextStream(e);
    else
        ts << functionToTextStream(e);
    ts << "\n";
}

inline std::map<std::string,std::string> attributesFromTextStream(std::istream & is)
{
    std::string lexem;
    std::map<std::string, std::string> result;
    while(1) {
        is >> lexem;
        if (is.fail())
            break;
        size_t eq_pos = lexem.find_first_of('=');
        if (eq_pos!=std::string::npos) {
            std::string key = lexem.substr(0, eq_pos);
            std::string val = lexem.substr(eq_pos+1);
            Kumir::StringUtils::trim<std::string,char>(key);
            Kumir::StringUtils::trim<std::string,char>(val);
            result[key] = val;
        }
    }
    return result;
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
    if (is.fail())
        throw std::string("Empty table element type: "+header);
    std::map<std::string, std::string> attrs = attributesFromTextStream(is);
    Kumir::Converter::ParseError error;
    if (attrs.count("id")==0 && e.type!=EL_EXTERN)
        throw std::string("No id specified: "+header);
    e.id = static_cast<uint16_t>(Kumir::Converter::parseInt(Kumir::Core::fromAscii(attrs["id"]),0,error));
    if (error!=Kumir::Converter::NoError) throw std::string("Error parsing numeric value");
    if (e.type==EL_LOCAL ||
            e.type==EL_GLOBAL ||
            e.type==EL_MAIN ||
            e.type==EL_BELOWMAIN ||
            e.type==EL_FUNCTION ||
            e.type==EL_TESTING
            )
    {
        if (attrs.count("name"))
            e.name = unscreenString(Kumir::Coder::decode(Kumir::UTF8, attrs["name"].substr(1,attrs["name"].length()-2)));
    }
    if (e.type==EL_LOCAL) {
        if (attrs.count("module")==0)
            throw std::string("No module id specified for local variable");
        if (error!=Kumir::Converter::NoError) throw std::string("Error parsing numeric value");
        if (attrs.count("algorithm")==0)
            throw std::string("No algorithm id specified for local variable");
        e.algId = Kumir::Converter::parseInt(Kumir::Core::fromAscii(attrs["algorithm"]),0,error);
        if (error!=Kumir::Converter::NoError) throw std::string("Error parsing numeric value");
        e.refvalue = VK_Plain;
        if (attrs.count("kind"))
            e.refvalue = kindFromString(attrs["kind"]);
        if (attrs.count("type")==0)
            throw std::string("No type specified for local variable");
        vtypeFromString(attrs["type"], e.vtype, e.dimension);
        if (e.vtype==VT_void)
            throw std::string("Illegal variable type");
    }
    if (e.type==EL_GLOBAL) {
        if (attrs.count("module")==0)
            throw std::string("No module id specified for global variable");
        if (error!=Kumir::Converter::NoError) throw std::string("Error parsing numeric value");
        e.refvalue = VK_Plain;
        if (attrs.count("type")==0)
            throw std::string("No type specified for global variable");
        vtypeFromString(attrs["type"], e.vtype, e.dimension);
        if (e.vtype==VT_void)
            throw std::string("Illegal variable type");
    }
    if (e.type==EL_EXTERN) {
        if (attrs.count("module")==0)
            throw std::string("No module name specified for external algorithm");
        e.moduleName = unscreenString(Kumir::Coder::decode(Kumir::UTF8, attrs["module"].substr(1,attrs["module"].length()-2)));
        if (attrs.count("algorithm")==0)
            throw std::string("No algorithm name specified for external reference");
        e.name = unscreenString(Kumir::Coder::decode(Kumir::UTF8,attrs["algorithm"].substr(1,attrs["algorithm"].length()-2)));
    }
    if (e.type==EL_CONST) {
        if (attrs.count("type")==0)
            throw std::string("No type specified for constant");
        vtypeFromString(attrs["type"], e.vtype, e.dimension);
        if (attrs.count("value")==0)
            throw std::string("No value specified for constant");
        if (e.vtype==VT_int) {
            int32_t val = Kumir::Converter::parseInt(Kumir::Core::fromUtf8(attrs["value"]),0,error);
            if (error!=Kumir::Converter::NoError) throw std::string("Error parsing numeric value");
            e.initialValue = Variable(val);
        }
        else if (e.vtype==VT_real) {
            double val;
            val = Kumir::Converter::parseReal(Kumir::Core::fromAscii(attrs["value"]),0,error);
            if (error!=Kumir::Converter::NoError) throw std::string("Error parsing numeric value");
            e.initialValue = Variable(val);
        }
        else if (e.vtype==VT_bool) {
            const std::string lexem = Kumir::Core::toLowerCase(attrs["value"]);
            if (lexem=="true" || lexem=="1")
                e.initialValue = Variable(bool(true));
            else if (lexem=="false" || lexem=="0")
                e.initialValue = Variable(bool(false));
            else
                throw std::string("Wrong boolean constant: "+header);
        }
        else {
            const std::string lexem = attrs["value"];
            if (lexem.length()<2) {
                throw std::string("Wrong literal constant: "+header);
            }
            e.initialValue = Variable(unscreenString(Kumir::Coder::decode(Kumir::UTF8, lexem.substr(1, lexem.length()-2))));
        }
    }
    if (e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_TESTING || e.type==EL_BELOWMAIN || e.type==EL_INIT) {
        int size = -1;
        if (attrs.count("size")) {
            size = Kumir::Converter::parseInt(Kumir::Core::fromAscii(attrs["size"]),0,error);
            if (error!=Kumir::Converter::NoError) throw std::string("Error parsing numeric value");
        }
        if (size==-1)
            e.instructions.resize(500);
        else
            e.instructions.resize(size);
        int index = 0;
        while (!ts.eof()) {
            if (size!=-1 && index>=size)
                break;
            std::string line;
            std::getline(ts, line);
            Kumir::StringUtils::trim<std::string,char>(line);
            if (line.length()==0 || line.at(0)=='#') {
                if (size==-1 && line.length()==0)
                    break;
                else
                    continue;
            }
            size_t colonPos = line.find(":");
            line = line.substr(colonPos+1);
            e.instructions[index] = instructionFromString(line);
            index++;
        }
        if (size==-1)
            e.instructions.resize(index);
    }
}


} // namespace Bytecode

#endif // BYTECODE_TABLEELEM_H
