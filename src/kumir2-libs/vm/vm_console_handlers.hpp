#ifndef VM_CONSOLE_HANDLERS_HPP
#define VM_CONSOLE_HANDLERS_HPP

#define DO_NOT_DECLARE_STATIC
#include "variant.hpp"
#include "vm_abstract_handlers.h"
#include <kumir2-libs/stdlib/kumirstdlib.hpp>
#include <iostream>
#include <fstream>
#undef DO_NOT_DECLARE_STATIC

#ifndef _override
#if defined(_MSC_VER)
#   define _override
#else
#   define _override override
#endif
#endif

namespace VM {
namespace Console {

using namespace Kumir;

inline void do_output(const String &s, const Encoding locale)
{
    EncodingError encodingError;
    const std::string localstring = Coder::encode(locale, s, encodingError);
    std::cout << localstring;
    std::cout.flush();
}

inline void do_output(const std::string & s, const Encoding locale)
{
    do_output(Core::fromUtf8(s), locale);
}

class InputFunctor
        : public VM::InputFunctor
        , public Kumir::AbstractInputBuffer
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
        , stdin_(IO::makeInputStream(FileType(), true))
    {}
    inline bool operator() (VariableReferencesList alist, Kumir::String * error) _override;
    inline void setLocale(const Encoding loc) { locale_ = loc; }
    inline void setCustomTypeFromStringFunctor(VM::CustomTypeFromStringFunctor *f)
    {
        customTypeFromString_ = f;
    }
    inline bool readRawChar(Char &ch) _override;
    inline void pushLastCharBack() _override;
    inline void clear() _override {}
private:
    Encoding locale_;
    VM::CustomTypeFromStringFunctor * customTypeFromString_;
    IO::InputStream stdin_;
};

bool InputFunctor::operator() (VariableReferencesList alist, Kumir::String * error)
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
            var.setValue(VM::AnyValue(IO::readLine(stream)));
        else if (var.baseType()==VM::VT_record) {
            const String s = IO::readString(stream);
            if (!stream.hasError()) {
                VM::CustomTypeFromStringFunctor * f =
                        customTypeFromString_;
                if (! f) {
                    static VM::CustomTypeFromStringFunctor def;
                    f = &def;
                }
                const std::string & modAsciiName = var.recordModuleAsciiName();
                const String & modName = var.recordModuleLocalizedName();
                const String & clsName = var.recordClassLocalizedName();
                const std::string & asciiClsName = var.recordClassAsciiName();
                var.setValue((*f)(s, modAsciiName, modName, asciiClsName, clsName, error));
            }
        }

        if (stream.hasError()) {
            int a, b;
            String message;
            stream.getError(message, a, b);
            if (error) {
                error->assign(message);
            }
        }

        if (error && error->length() > 0) {
            break;
        }
    }
    return true;
}

bool InputFunctor::readRawChar(Char &ch)
{
    return stdin_.readRawChar(ch);
}

void InputFunctor::pushLastCharBack()
{
    stdin_.pushLastCharBack();
}

class OutputFunctor
        : public VM::OutputFunctor
        , public Kumir::AbstractOutputBuffer
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
    inline void operator ()(VariableReferencesList alist, FormatsList formats, Kumir::String * error) _override;
    inline void setLocale(const Encoding loc) { locale_ = loc; }
    inline void setCustomTypeToStringFunctor(VM::CustomTypeToStringFunctor * f)
    {
        customTypeToString_ = f;
    }
    inline void writeRawString(const String &) _override;

private:
    Encoding locale_;
    VM::CustomTypeToStringFunctor * customTypeToString_;
};

void OutputFunctor::operator ()(
        VariableReferencesList values,
        FormatsList formats, Kumir::String * error
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
            const String repr = (*f)(values[i], error);
            IO::writeString(os, repr, 0);
        }
        if (error && error->length() > 0) {
            return;
        }
    }    
    do_output(os.getBuffer(), locale_);
}

void OutputFunctor::writeRawString(const String &s)
{
    do_output(s, locale_);
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
        , quietMode_(false)
    {}
    inline void operator()(const VM::Variable & reference, Kumir::String * error) _override;
    inline void setLocale(const Encoding loc) { locale_ = loc; }
    inline void setCustomTypeToStringFunctor(VM::CustomTypeToStringFunctor * f)
    {
        customTypeToString_ = f;
    }
    inline void setQuietMode(bool v) { quietMode_ = v; }
private:
    Encoding locale_;
    VM::CustomTypeToStringFunctor * customTypeToString_;
    bool quietMode_;
};

class GetMainArgumentFunctor
        : public VM::GetMainArgumentFunctor
{
public:
    inline GetMainArgumentFunctor()
        : VM::GetMainArgumentFunctor()
        , currentArgument_(0)
    #if defined(WIN32) || defined(_WIN32)
        , locale_(CP866)
    #else
        , locale_(UTF8)
    #endif
        , customTypeFromString_(nullptr)
        , quietMode_(false)
    {}
    inline void operator()(VM::Variable & reference, Kumir::String * error) _override;
    inline void init(const std::deque<std::string> args);
    inline void init(int argc, char * argv[]);
    inline void setLocale(const Encoding loc) { locale_ = loc; }
    inline void setCustomTypeFromStringFunctor(VM::CustomTypeFromStringFunctor *f)
    {
        customTypeFromString_ = f;
    }
    inline void setQuietMode(bool v) { quietMode_ = v; }
private:
    inline bool readScalarArgument(const String & message,
                                   const String & name,
                                   VM::ValueType type,
                                   const std::string & customModuleAsciiName,
                                   const String & customModuleName,
                                   const std::string & customTypeAsciiName,
                                   const String & customTypeLocalizedName,
                                   VM::AnyValue & val, Kumir::String * error
                                   );
    inline static String decodeHttpStringValue(const std::string & s);
    std::deque< String > m_arguments;
    size_t currentArgument_;
    Encoding locale_;
    VM::CustomTypeFromStringFunctor * customTypeFromString_;
    bool quietMode_;
};

void GetMainArgumentFunctor::init(const std::deque<std::string> args)
{
    currentArgument_ = 0;
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
            EncodingError encodingError;
            m_arguments.push_back(Coder::decode(locale_, arg, encodingError));
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
    EncodingError encodingError;
    while (cpos<s.length()) {
        if (s[cpos]=='%'
                && cpos+2 < s.length()
                && IS_HEX(s[cpos+1])
                && IS_HEX(s[cpos+2])
                )
        {
            std::string hexcode = std::string("0x")+s.substr(cpos+1,2);
            bool ok;
            int value = Converter::stringToInt(Coder::decode(ASCII,hexcode,encodingError), ok);
            char ch = (char)value;
            utf8string.push_back(ch);
            cpos += 3;
        }
        else {
            utf8string.push_back(s[cpos]);
            cpos += 1;
        }

    }
    result = Coder::decode(UTF8, utf8string, encodingError);
    return result;
}

bool GetMainArgumentFunctor::readScalarArgument(
        const String &message,
        const String &name,
        VM::ValueType type,
        const std::string & customModuleAsciiName,
        const String & customModuleName,
        const std::string & customTypeAsciiName,
        const String & customTypeLocalizedName,
        VM::AnyValue &val, Kumir::String * error
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
        if (currentArgument_<m_arguments.size()) {
            stream = IO::InputStream(m_arguments[currentArgument_]);
            currentArgument_ ++;
            foundValue = true;
        }
    }
    if (!foundValue) {
        if (!quietMode_) {
            IO::writeString(0, message);
        }
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
        val = (*f)(s, customModuleAsciiName, customModuleName, customTypeAsciiName, customTypeLocalizedName, error);
    }
    if (Core::getError().length() > 0) {
        if (error) {
            error->assign(Core::getError());
        }
    }
    return error? error->length()==0: true; // Core::getError().size()==0;
}

void GetMainArgumentFunctor::operator()(VM::Variable &reference, Kumir::String * error)
{
    String message = Core::fromUtf8("Введите ")+reference.name();
    static const String errorMessage = Core::fromUtf8("Не все аргументы первого алгоритма введены корректно");
    if (reference.dimension()==0) {
        message += Core::fromAscii(": ");
        VM::AnyValue val;
        if (readScalarArgument(message,
                               reference.name(),
                               reference.baseType(),
                               reference.recordModuleAsciiName(),
                               reference.recordModuleLocalizedName(),
                               reference.recordClassAsciiName(),
                               reference.recordClassLocalizedName(),
                               val, error))
            reference.setValue(val);
        else {
            if (error) {
                error->assign(errorMessage);
            }
            return;
        }
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
                                   reference.recordModuleAsciiName(),
                                   reference.recordModuleLocalizedName(),
                                   reference.recordClassAsciiName(),
                                   reference.recordClassLocalizedName(),
                                   val, error))
                reference.setValue(x,val);
            else {
                if (error) {
                    error->assign(errorMessage);
                }
                return;
            }
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
                                       reference.recordModuleAsciiName(),
                                       reference.recordModuleLocalizedName(),
                                       reference.recordClassAsciiName(),
                                       reference.recordClassLocalizedName(),
                                       val, error))
                    reference.setValue(y,x,val);
                else {
                    if (error) {
                        error->assign(errorMessage);
                    }
                    return;
                }
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
                                           reference.recordModuleAsciiName(),
                                           reference.recordModuleLocalizedName(),
                                           reference.recordClassAsciiName(),
                                           reference.recordClassLocalizedName(),
                                           val, error))
                        reference.setValue(z,y,x,val);
                    else {
                        if (error) {
                            error->assign(errorMessage);
                        }
                        return;
                    }
                }
            }
        }
    }
}

void ReturnMainValueFunctor::operator()(const VM::Variable & reference, Kumir::String * /*error*/) {
    if (!reference.isValid())
        return;
    String repr;
    VM::CustomTypeToStringFunctor * f = customTypeToString_;
    if (reference.baseType()==VM::VT_record && !f) {
        static VM::CustomTypeToStringFunctor def;
        f = &def;
    }
    if (!quietMode_) {
        do_output(reference.name()+Core::fromAscii(" = "), locale_);
    }
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
    if (!quietMode_) {
        do_output("\n", locale_);
    }
}


}} // namespaces

#endif // VM_CONSOLE_HANDLERS_HPP
