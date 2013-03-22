#ifndef VM_CONSOLE_HANDLERS_HPP
#define VM_CONSOLE_HANDLERS_HPP

#define DO_NOT_DECLARE_STATIC
#include "variant.hpp"
#include "vm_abstract_handlers.h"
#include "stdlib/kumirstdlib.hpp"
#include <iostream>
#include <fstream>
#undef DO_NOT_DECLARE_STATIC

namespace VM {
namespace Console {

using namespace Kumir;

inline void do_output(const String &s, const Encoding locale)
{
    const std::string localstring = Coder::encode(locale, s);
    std::cout << localstring;
    std::cout.flush();
}

inline void do_output(const std::string & s, const Encoding locale)
{
    do_output(Core::fromUtf8(s), locale);
}

class InputFunctor
        : public VM::InputFunctor
{
public:
    inline InputFunctor()
        : VM::InputFunctor()
    #if defined(WIN32) || defined(_WIN32)
        , locale_(CP866)
    #else
        , locale_(UTF8)
    #endif
        , customTypeFromString_(nullptr)
    {}
    inline void operator() (VariableReferencesList alist);
    inline void setLocale(const Encoding loc) { locale_ = loc; }
    inline void setCustomTypeFromStringFunctor(VM::CustomTypeFromStringFunctor *f)
    {
        customTypeFromString_ = f;
    }
private:
    Encoding locale_;
    VM::CustomTypeFromStringFunctor * customTypeFromString_;
};

void InputFunctor::operator() (VariableReferencesList alist)
{
    IO::InputStream stream = IO::makeInputStream(FileType(), true);
    for (size_t i=0; i<alist.size(); i++) {
        VM::Variable & var = alist[i];
        if (var.baseType()==VM::VT_int)
            var.setValue(VM::AnyValue(IO::readInteger(stream)));
        else if (var.baseType()==VM::VT_real)
            var.setValue(VM::AnyValue(IO::readReal(stream)));
        else if (var.baseType()==VM::VT_bool)
            var.setValue(VM::AnyValue(IO::readBool(stream)));
        else if (var.baseType()==VM::VT_char)
            var.setValue(VM::AnyValue(IO::readChar(stream)));
        else if (var.baseType()==VM::VT_string)
            var.setValue(VM::AnyValue(IO::readString(stream)));
        else if (var.baseType()==VM::VT_record) {
            const String s = IO::readString(stream);
            if (!stream.hasError()) {
                VM::CustomTypeFromStringFunctor * f =
                        customTypeFromString_;
                if (! f) {
                    static VM::CustomTypeFromStringFunctor def;
                    f = &def;
                }
                const String & modName = var.recordModuleName();
                const String & clsName = var.recordClassName();
                var.setValue((*f)(s, modName, clsName));
            }
        }

        if (stream.hasError()) {
            int a, b;
            String message;
            stream.getError(message, a, b);
            throw message;
        }
    }
}

class OutputFunctor
        : public VM::OutputFunctor
{
public:
    inline OutputFunctor()
        : VM::OutputFunctor()
    #if defined(WIN32) || defined(_WIN32)
        , locale_(CP866)
    #else
        , locale_(UTF8)
    #endif
        , customTypeToString_(nullptr)
    {}
    inline void operator ()(VariableReferencesList alist, FormatsList formats);
    inline void setLocale(const Encoding loc) { locale_ = loc; }
    inline void setCustomTypeToStringFunctor(VM::CustomTypeToStringFunctor * f)
    {
        customTypeToString_ = f;
    }

private:
    Encoding locale_;
    VM::CustomTypeToStringFunctor * customTypeToString_;
};

void OutputFunctor::operator ()(
        VariableReferencesList values,
        FormatsList formats
        )
{
    IO::OutputStream os;
    for (int i=0; i<formats.size(); i++) {
        std::pair<int,int> format = formats[i];
        if (values[i].baseType()==VM::VT_int) {
            IO::writeInteger(os, values[i].toInt(), format.first);
        }
        else if (values[i].baseType()==VM::VT_real) {
            IO::writeReal(os, values[i].toDouble(), format.first, format.second);
        }
        else if (values[i].baseType()==VM::VT_bool) {
            IO::writeBool(os, values[i].toBool(), format.first);
        }
        else if (values[i].baseType()==VM::VT_char) {
            IO::writeChar(os, values[i].toChar(), format.first);
        }
        else if (values[i].baseType()==VM::VT_string) {
            IO::writeString(os, values[i].toString(), format.first);
        }
        else if (values[i].baseType()==VM::VT_record) {
            VM::CustomTypeToStringFunctor * f =
                    customTypeToString_;
            if (! f) {
                static VM::CustomTypeToStringFunctor def;
                f = &def;
            }
            const String repr = (*f)(values[i]);
            IO::writeString(os, repr, 0);
        }
    }
    do_output(os.getBuffer(), locale_);
}

class ReturnMainValueFunctor
        : public VM::ReturnMainValueFunctor
{
public:
    inline ReturnMainValueFunctor()
        : VM::ReturnMainValueFunctor()
    #if defined(WIN32) || defined(_WIN32)
        , locale_(CP866)
    #else
        , locale_(UTF8)
    #endif
        , customTypeToString_(nullptr)
    {}
    inline void operator()(const VM::Variable & reference);
    inline void setLocale(const Encoding loc) { locale_ = loc; }
    inline void setCustomTypeToStringFunctor(VM::CustomTypeToStringFunctor * f)
    {
        customTypeToString_ = f;
    }
private:
    Encoding locale_;
    VM::CustomTypeToStringFunctor * customTypeToString_;
};

class GetMainArgumentFunctor
        : public VM::GetMainArgumentFunctor
{
public:
    inline GetMainArgumentFunctor()
        : VM::GetMainArgumentFunctor()
        , i_currentArgument(0)
    #if defined(WIN32) || defined(_WIN32)
        , locale_(CP866)
    #else
        , locale_(UTF8)
    #endif
        , customTypeFromString_(nullptr)
    {}
    inline void operator()(VM::Variable & reference);
    inline void init(const std::deque<std::string> args);
    inline void init(int argc, char * argv[]);
    inline void setLocale(const Encoding loc) { locale_ = loc; }
    inline void setCustomTypeFromStringFunctor(VM::CustomTypeFromStringFunctor *f)
    {
        customTypeFromString_ = f;
    }
private:
    inline bool readScalarArgument(const String & message,
                                   const String & name,
                                   VM::ValueType type,
                                   const String & customModuleName,
                                   const String & customTypeName,
                                   VM::AnyValue & val
                                   );
    inline static String decodeHttpStringValue(const std::string & s);
    std::deque< String > m_arguments;
    size_t i_currentArgument;
    Encoding locale_;
    VM::CustomTypeFromStringFunctor * customTypeFromString_;
};

void GetMainArgumentFunctor::init(const std::deque<std::string> args)
{
    i_currentArgument = 0;
    bool argumentsScope = false;
    for (int i=1; i<args.size(); i++) {
        const std::string & arg = args[i];
        if (arg.length()==0)
            continue;
        if (!argumentsScope) {
            if (arg[0]!='-') {
                // Found not a switch -> it is a program,
                // so next arg is a program argument
                argumentsScope = true;
            }
        }
        else {
            m_arguments.push_back(Coder::decode(locale_, arg));
        }
    }
}

void GetMainArgumentFunctor::init(int argc, char *argv[])
{
    std::deque<std::string> stringlist;
    for (int i=0; i<argc; i++) {
        stringlist.push_back(std::string(argv[i]));
    }
    init(stringlist);
}

#define IS_HEX(x) ( (x>='0' && x<='9') || (x>='A' && x<='F') || (x>='a' && x<='f') )

String GetMainArgumentFunctor::decodeHttpStringValue(const std::string & s)
{
    String result;
    size_t cpos = 0;
    std::string utf8string;
    utf8string.reserve(s.length());
    while (cpos<s.length()) {
        if (s[cpos]=='%'
                && cpos+2 < s.length()
                && IS_HEX(s[cpos+1])
                && IS_HEX(s[cpos+2])
                )
        {
            std::string hexcode = std::string("0x")+s.substr(cpos+1,2);
            bool ok;
            int value = Converter::stringToInt(Coder::decode(ASCII,hexcode), ok);
            char ch = (char)value;
            utf8string.push_back(ch);
            cpos += 3;
        }
        else {
            utf8string.push_back(s[cpos]);
            cpos += 1;
        }

    }
    result = Coder::decode(UTF8, utf8string);
    return result;
}

bool GetMainArgumentFunctor::readScalarArgument(
        const String &message,
        const String &name,
        VM::ValueType type,
        const String & customModuleName,
        const String & customTypeName,
        VM::AnyValue &val
        )
{
    IO::InputStream stream;
    bool foundValue = false;
#if !defined(WIN32) && !defined(_WIN32)
    char * REQUEST_METHOD = getenv("REQUEST_METHOD");
    char * QUERY_STRING = getenv("QUERY_STRING");
    if (REQUEST_METHOD && std::string(REQUEST_METHOD)==std::string("GET") && QUERY_STRING) {
        String query_string = decodeHttpStringValue(std::string(QUERY_STRING));
        StringList pairs = Core::splitString(query_string, Char('&'), true);
        for (size_t i=0; i<pairs.size(); i++) {
            StringList apair = Core::splitString(pairs[i], Char('='), true);
            if (apair.size()==2) {
                String aname = apair[0];
                String avalue = apair[1];
                if (aname==name) {
                    stream = IO::InputStream(avalue);
                    foundValue = true;
                    break;
                }
            }
        }
    }
#endif
    if (!foundValue) {
        if (i_currentArgument<m_arguments.size()) {
            stream = IO::InputStream(m_arguments[i_currentArgument]);
            i_currentArgument ++;
            foundValue = true;
        }
    }
    if (!foundValue) {
        IO::writeString(0, message);
        stream = IO::InputStream(stdin, locale_);
    }
    if      (type==VM::VT_int)
        val = IO::readInteger(stream);
    else if (type==VM::VT_real)
        val = IO::readReal(stream);
    else if (type==VM::VT_bool)
        val = IO::readBool(stream);
    else if (type==VM::VT_char)
        val = IO::readChar(stream);
    else if (type==VM::VT_string)
        val = IO::readString(stream);
    else if (type==VM::VT_record) {
        const String s = IO::readString(stream);
        VM::CustomTypeFromStringFunctor * f = customTypeFromString_;
        if (!f) {
            static VM::CustomTypeFromStringFunctor def;
            f = &def;
        }
        try {
            val = (*f)(s, customModuleName, customTypeName);
        }
        catch (const String & message) {
            Core::abort(message);
        }
        catch (const std::string & message) {
            Core::abort(Core::fromUtf8(message));
        }
    }
    return Core::getError().size()==0;
}

void GetMainArgumentFunctor::operator()(VM::Variable &reference)
{
    String message = Core::fromUtf8("Введите ")+reference.name();
    static const String errorMessage = Core::fromUtf8("Не все аргументы первого алгоритма введены корректно");
    if (reference.dimension()==0) {
        message += Core::fromAscii(": ");
        VM::AnyValue val;
        if (readScalarArgument(message,
                               reference.name(),
                               reference.baseType(),
                               reference.recordModuleName(),
                               reference.recordClassName(),
                               val))
            reference.setValue(val);
        else
            throw errorMessage;
    }
    else if (reference.dimension()==1) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        for (int x=bounds[0]; x<=bounds[1]; x++) {
            VM::AnyValue val;
            message = Core::fromUtf8("Введите ")+reference.name();
            message += Core::fromAscii("[");
            message += Converter::intToString(x);
            message += Core::fromAscii("]: ");
            if (readScalarArgument(message,
                                   reference.name(),
                                   reference.baseType(),
                                   reference.recordModuleName(),
                                   reference.recordClassName(),
                                   val))
                reference.setValue(x,val);
            else
                throw errorMessage;
        }
    }
    else if (reference.dimension()==2) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        for (int y=bounds[0]; y<=bounds[1]; y++) {
            for (int x=bounds[2]; x<=bounds[3]; x++) {
                VM::AnyValue val;
                message = Core::fromUtf8("Введите ")+reference.name();
                message += Core::fromAscii("[");
                message += Converter::intToString(y);
                message += Core::fromAscii(",");
                message += Converter::intToString(x);
                message += Core::fromAscii("]: ");
                if (readScalarArgument(message,
                                       reference.name(),
                                       reference.baseType(),
                                       reference.recordModuleName(),
                                       reference.recordClassName(),
                                       val))
                    reference.setValue(y,x,val);
                else
                    throw errorMessage;
            }
        }
    }
    else if (reference.dimension()==3) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        for (int z=bounds[0]; z<=bounds[1]; z++) {
            for (int y=bounds[2]; y<=bounds[3]; y++) {
                for (int x=bounds[4]; x<=bounds[5]; x++) {
                    VM::AnyValue val;
                    message = Core::fromUtf8("Введите ")+reference.name();
                    message += Core::fromAscii("[");
                    message += Converter::intToString(y);
                    message += Core::fromAscii(",");
                    message += Converter::intToString(x);
                    message += Core::fromAscii("]: ");
                    if (readScalarArgument(message,
                                           reference.name(),
                                           reference.baseType(),
                                           reference.recordModuleName(),
                                           reference.recordClassName(),
                                           val))
                        reference.setValue(z,y,x,val);
                    else
                        throw errorMessage;
                }
            }
        }
    }
}

void ReturnMainValueFunctor::operator()(const VM::Variable & reference) {
    if (!reference.isValid())
        return;
    String repr;
    VM::CustomTypeToStringFunctor * f = customTypeToString_;
    if (reference.baseType()==VM::VT_record && !f) {
        static VM::CustomTypeToStringFunctor def;
        f = &def;
    }
    do_output(reference.name()+Core::fromAscii(" = "), locale_);
    if (reference.dimension()==0) {
        if (reference.hasValue()) {
            repr = reference.value().toString();
            if (reference.baseType()==Bytecode::VT_string)
                repr = Core::fromAscii("\"") + repr + Core::fromAscii("\"");
            else if (reference.baseType()==Bytecode::VT_char)
                repr = Core::fromAscii("'") + repr + Core::fromAscii("'");
        }
        do_output(repr, locale_);
    }
    else if (reference.dimension()==1) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        do_output("{ ", locale_);
        for (int x=bounds[0]; x<=bounds[1]; x++) {
            repr.clear();
            if (reference.hasValue(x)) {
                repr = reference.value(x).toString();
                if (reference.baseType()==Bytecode::VT_string)
                    repr = Core::fromAscii("\"") + repr + Core::fromAscii("\"");
                else if (reference.baseType()==Bytecode::VT_char)
                    repr = Core::fromAscii("'") + repr + Core::fromAscii("'");
            }
            do_output(repr, locale_);
            if (x<bounds[1]) {
                do_output(", ", locale_);
            }
        }
        do_output(" }", locale_);
    }
    else if (reference.dimension()==2) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        do_output("{ ", locale_);
        for (int y=bounds[0]; y<=bounds[1]; y++) {
            do_output("{ ", locale_);
            for (int x=bounds[2]; x<=bounds[3]; x++) {
                repr.clear();
                if (reference.hasValue(y,x)) {
                    repr = reference.value(y,x).toString();
                    if (reference.baseType()==Bytecode::VT_string)
                        repr = Core::fromAscii("\"") + repr + Core::fromAscii("\"");
                    else if (reference.baseType()==Bytecode::VT_char)
                        repr = Core::fromAscii("'") + repr + Core::fromAscii("'");
                }
                do_output(repr, locale_);
                if (x<bounds[1]) {
                    do_output(", ", locale_);
                }
            }
            do_output(" }", locale_);
            if (y<bounds[1]) {
                do_output(", ", locale_);
            }
        }
        do_output(" }", locale_);
    }
    else if (reference.dimension()==3) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        do_output("{ ", locale_);
        for (int z=bounds[0]; z<=bounds[1]; z++) {
            do_output("{ ", locale_);
            for (int y=bounds[2]; y<=bounds[3]; y++) {
                do_output("{ ", locale_);
                for (int x=bounds[4]; x<=bounds[5]; x++) {
                    repr.clear();
                    if (reference.hasValue(z,y,x)) {
                        repr = reference.value(z,y,x).toString();
                        if (reference.baseType()==Bytecode::VT_string)
                            repr = Core::fromAscii("\"") + repr + Core::fromAscii("\"");
                        else if (reference.baseType()==Bytecode::VT_char)
                            repr = Core::fromAscii("'") + repr + Core::fromAscii("'");
                    }
                    do_output(repr, locale_);
                    if (x<bounds[1]) {
                        do_output(", ", locale_);
                    }
                }
                do_output(" }", locale_);
                if (y<bounds[1]) {
                    do_output(", ", locale_);
                }
            }
            do_output(" }", locale_);
            if (z<bounds[1]) {
                do_output(", ", locale_);
            }
        }
        do_output(" }", locale_);
    }
    do_output("\n", locale_);
}


}} // namespaces

#endif // VM_CONSOLE_HANDLERS_HPP
