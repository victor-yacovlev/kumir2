#include <iostream>
#include <fstream>
#include "stdlib/kumirstdlib.hpp"
#include "vm/variant.hpp"
#include "vm/vm_bytecode.hpp"
#include "vm/vm.hpp"

static Kumir::Encoding LOCALE;

int usage(const char * programName)
{
    using namespace Kumir;

    bool russianLanguage = false;

#if defined(WIN32) || defined(_WIN32)
    russianLanguage = true;
    Char _n = Char('\n');
#else
    char * env = getenv("LANG");
    if (env) {
        std::string LANG(env);
        russianLanguage = LANG.find("ru")!=std::string::npos;
    }
    Char _n = Char('\n');
#endif
    String message;
    if (russianLanguage) {
        message  = Core::fromUtf8("Вызов:");
        message.push_back(_n);
        message += Core::fromUtf8("\t")+Core::fromUtf8(std::string(programName));
        message += Core::fromUtf8(" [-ansi] ИМЯФАЙЛА.kod [ПАРАМ1 [ПАРАМ2 ... [ПАРАМn]]]");
        message.push_back(_n);
        message.push_back(_n);
        message += Core::fromUtf8("\t-ansi\t\tИспользовть кодировку 1251 вместо 866 в терминале (только для Windows)");
        message.push_back(_n);
        message += Core::fromUtf8("\tИМЯФАЙЛА.kod\tИмя выполнеяемой программы");
        message.push_back(_n);
        message += Core::fromUtf8("\tПАРАМ1...ПАРАМn\tАргументы главного алгоритма Кумир-программы");
        message.push_back(_n);
    }
    else {
        message  = Core::fromUtf8("Usage:");
        message.push_back(_n);
        message += Core::fromUtf8("\t")+Core::fromUtf8(std::string(programName));
        message += Core::fromUtf8(" [-ansi] FILENAME.kod [ARG1 [ARG2 ... [ARGn]]]");
        message.push_back(_n);
        message.push_back(_n);
        message += Core::fromUtf8("\t-ansi\t\tUse codepage 1251 instead of 866 in console (Windows only)");
        message.push_back(_n);
        message += Core::fromUtf8("\tFILENAME.kod\tKumir runtime file name");
        message.push_back(_n);
        message += Core::fromUtf8("\tARG1...ARGn\tKumir program main algorithm arguments");
        message.push_back(_n);
    }
    std::cerr << Coder::encode(LOCALE, message);
    return 127;
}

class InteractionHandler
        : public VM::AbstractInteractionHandler
{
public:
    explicit InteractionHandler(int argc, char *argv[]);
    bool makeInputArgument(VM::Variable & reference);
    bool makeOutputArgument(const VM::Variable & reference);
private:
    void output(const Kumir::String & s);
    inline void output(const std::string & s) { output(Kumir::Core::fromAscii(s)); }
    bool readScalarArgument(const Kumir::String & message, const Kumir::String & name, VM::ValueType type, VM::AnyValue & val);
    std::deque< Kumir::String > m_arguments;
    size_t i_currentArgument;
};

InteractionHandler::InteractionHandler(int argc, char *argv[])
{
    i_currentArgument = 0;
    bool argumentsScope = false;
    for (int i=1; i<argc; i++) {
        std::string arg(argv[i]);
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
            m_arguments.push_back(Kumir::Coder::decode(LOCALE, arg));
        }
    }
}

#define IS_HEX(x) ( (x>='0' && x<='9') || (x>='A' && x<='F') || (x>='a' && x<='f') )

Kumir::String decodeHttpStringValue(const std::string & s)
{
    Kumir::String result;
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
            int value = Kumir::Converter::stringToInt(Kumir::Coder::decode(Kumir::ASCII,hexcode), ok);
            char ch = (char)value;
            utf8string.push_back(ch);
            cpos += 3;
        }
        else {
            utf8string.push_back(s[cpos]);
            cpos += 1;
        }

    }
    result = Kumir::Coder::decode(Kumir::UTF8, utf8string);
    return result;
}

int showErrorMessage(const Kumir::String & message, int code) {
    bool toHttp = false;
#if !defined(WIN32) && !defined(_WIN32)
    char * REQUEST_METHOD = getenv("REQUEST_METHOD");
    char * QUERY_STRING = getenv("QUERY_STRING");
    toHttp = (REQUEST_METHOD!=0 && QUERY_STRING!=0);
#endif
    if (!toHttp) {
        const std::string localMessage = Kumir::Coder::encode(LOCALE, message);
        std::cerr << localMessage << std::endl;
        return code;
    }
    else {
        const std::string localMessage = Kumir::Coder::encode(Kumir::UTF8, message);
        std::cout << "Content-type: text/html;charset=utf-8\n\n\n";
        std::cout << "<html><head><title>An error occured on server</title></head>\n";
        std::cout << "<body>\n";
        std::cout << localMessage << std::endl;
        std::cout << "</body></html>\n";
        return 0;
    }
}

bool InteractionHandler::readScalarArgument(const Kumir::String &message, const Kumir::String &name, VM::ValueType type, VM::AnyValue &val)
{
    Kumir::IO::InputStream stream;
    bool foundValue = false;
#if !defined(WIN32) && !defined(_WIN32)
    char * REQUEST_METHOD = getenv("REQUEST_METHOD");
    char * QUERY_STRING = getenv("QUERY_STRING");
    if (REQUEST_METHOD && std::string(REQUEST_METHOD)==std::string("GET") && QUERY_STRING) {
        Kumir::String query_string = decodeHttpStringValue(std::string(QUERY_STRING));
        Kumir::StringList pairs = Kumir::Core::splitString(query_string, Kumir::Char('&'), true);
        for (size_t i=0; i<pairs.size(); i++) {
            Kumir::StringList apair = Kumir::Core::splitString(pairs[i], Kumir::Char('='), true);
            if (apair.size()==2) {
                Kumir::String aname = apair[0];
                Kumir::String avalue = apair[1];
                if (aname==name) {
                    stream = Kumir::IO::InputStream(avalue);
                    foundValue = true;
                    break;
                }
            }
        }
    }
#endif
    if (!foundValue) {
        if (i_currentArgument<m_arguments.size()) {
            stream = Kumir::IO::InputStream(m_arguments[i_currentArgument]);
            i_currentArgument ++;
            foundValue = true;
        }
    }
    if (!foundValue) {
        Kumir::IO::writeString(0, message);
        stream = Kumir::IO::InputStream(stdin, LOCALE);
    }
    if      (type==VM::VT_int)
        val = Kumir::IO::readInteger(stream);
    else if (type==VM::VT_real)
        val = Kumir::IO::readReal(stream);
    else if (type==VM::VT_bool)
        val = Kumir::IO::readBool(stream);
    else if (type==VM::VT_char)
        val = Kumir::IO::readChar(stream);
    else if (type==VM::VT_string)
        val = Kumir::IO::readString(stream);
    return Kumir::Core::getError().size()==0;
}

bool InteractionHandler::makeInputArgument(VM::Variable &reference)
{
    Kumir::String message = Kumir::Core::fromUtf8("Введите ")+reference.name();
    if (reference.dimension()==0) {
        message += Kumir::Core::fromAscii(": ");
        VM::AnyValue val;
        if (readScalarArgument(message, reference.name(), reference.baseType(), val))
            reference.setValue(val);
    }
    else if (reference.dimension()==1) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        for (int x=bounds[0]; x<=bounds[1]; x++) {
            VM::AnyValue val;
            message = Kumir::Core::fromUtf8("Введите ")+reference.name();
            message += Kumir::Core::fromAscii("[");
            message += Kumir::Converter::intToString(x);
            message += Kumir::Core::fromAscii("]: ");
            if (readScalarArgument(message, reference.name(), reference.baseType(),  val))
                reference.setValue(x,val);
            else
                return true;
        }
    }
    else if (reference.dimension()==2) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        for (int y=bounds[0]; y<=bounds[1]; y++) {
            for (int x=bounds[2]; x<=bounds[3]; x++) {
                VM::AnyValue val;
                message = Kumir::Core::fromUtf8("Введите ")+reference.name();
                message += Kumir::Core::fromAscii("[");
                message += Kumir::Converter::intToString(y);
                message += Kumir::Core::fromAscii(",");
                message += Kumir::Converter::intToString(x);
                message += Kumir::Core::fromAscii("]: ");
                if (readScalarArgument(message, reference.name(), reference.baseType(),  val))
                    reference.setValue(y,x,val);
                else
                    return true;
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
                    message = Kumir::Core::fromUtf8("Введите ")+reference.name();
                    message += Kumir::Core::fromAscii("[");
                    message += Kumir::Converter::intToString(y);
                    message += Kumir::Core::fromAscii(",");
                    message += Kumir::Converter::intToString(x);
                    message += Kumir::Core::fromAscii("]: ");
                    if (readScalarArgument(message, reference.name(), reference.baseType(),  val))
                        reference.setValue(z,y,x,val);
                    else
                        return true;
                }
            }
        }
    }
    return true;
}

bool InteractionHandler::makeOutputArgument(const VM::Variable & reference) {
    if (!reference.isValid())
        return true;
    Kumir::String repr;
    output(reference.name()+Kumir::Core::fromAscii(" = "));
    if (reference.dimension()==0) {
        if (reference.hasValue()) {
            repr = reference.value().toString();
            if (reference.baseType()==Bytecode::VT_string)
                repr = Kumir::Core::fromAscii("\"") + repr + Kumir::Core::fromAscii("\"");
            else if (reference.baseType()==Bytecode::VT_char)
                repr = Kumir::Core::fromAscii("'") + repr + Kumir::Core::fromAscii("'");
        }
        output(repr);
    }
    else if (reference.dimension()==1) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        output("{ ");
        for (int x=bounds[0]; x<=bounds[1]; x++) {
            repr.clear();
            if (reference.hasValue(x)) {
                repr = reference.value(x).toString();
                if (reference.baseType()==Bytecode::VT_string)
                    repr = Kumir::Core::fromAscii("\"") + repr + Kumir::Core::fromAscii("\"");
                else if (reference.baseType()==Bytecode::VT_char)
                    repr = Kumir::Core::fromAscii("'") + repr + Kumir::Core::fromAscii("'");
            }
            output(repr);
            if (x<bounds[1]) {
                output(", ");
            }
        }
        output(" }");
    }
    else if (reference.dimension()==2) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        output("{ ");
        for (int y=bounds[0]; y<=bounds[1]; y++) {
            output("{ ");
            for (int x=bounds[2]; x<=bounds[3]; x++) {
                repr.clear();
                if (reference.hasValue(y,x)) {
                    repr = reference.value(y,x).toString();
                    if (reference.baseType()==Bytecode::VT_string)
                        repr = Kumir::Core::fromAscii("\"") + repr + Kumir::Core::fromAscii("\"");
                    else if (reference.baseType()==Bytecode::VT_char)
                        repr = Kumir::Core::fromAscii("'") + repr + Kumir::Core::fromAscii("'");
                }
                output(repr);
                if (x<bounds[1]) {
                    output(", ");
                }
            }
            output(" }");
            if (y<bounds[1]) {
                output(", ");
            }
        }
        output(" }");
    }
    else if (reference.dimension()==3) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        output("{ ");
        for (int z=bounds[0]; z<=bounds[1]; z++) {
            output("{ ");
            for (int y=bounds[2]; y<=bounds[3]; y++) {
                output("{ ");
                for (int x=bounds[4]; x<=bounds[5]; x++) {
                    repr.clear();
                    if (reference.hasValue(z,y,x)) {
                        repr = reference.value(z,y,x).toString();
                        if (reference.baseType()==Bytecode::VT_string)
                            repr = Kumir::Core::fromAscii("\"") + repr + Kumir::Core::fromAscii("\"");
                        else if (reference.baseType()==Bytecode::VT_char)
                            repr = Kumir::Core::fromAscii("'") + repr + Kumir::Core::fromAscii("'");
                    }
                    output(repr);
                    if (x<bounds[1]) {
                        output(", ");
                    }
                }
                output(" }");
                if (y<bounds[1]) {
                    output(", ");
                }
            }
            output(" }");
            if (z<bounds[1]) {
                output(", ");
            }
        }
        output(" }");
    }
    output("\n");
    return true;
}

void InteractionHandler::output(const Kumir::String &s)
{
    const std::string localstring = Kumir::Coder::encode(LOCALE, s);
    std::cout << localstring;
}


int main(int argc, char *argv[])
{
//    sleep(15); // for remote debugger
    // Look at arguments
#if defined(WIN32) || defined(_WIN32)
    Kumir::IO::LOCALE_ENCODING = LOCALE = Kumir::CP866;
#else
    Kumir::IO::LOCALE_ENCODING = LOCALE = Kumir::UTF8;
#endif
    std::string programName;
    std::deque<std::string> args;
    bool forceTextForm = false;
    for (int i=1; i<argc; i++) {
        std::string  arg(argv[i]);
        if (arg.length()==0)
            continue;
        static const std::string minuss("-s");
        static const std::string minusS("-S");
        static const std::string minusansi("-ansi");
        if (programName.empty()) {
            if (arg==minuss || arg==minusS)
                forceTextForm = true;
            else if (arg==minusansi)
                Kumir::IO::LOCALE_ENCODING = LOCALE = Kumir::CP1251;
            else
                programName = arg;
        }
        else {
            args.push_back(arg);
        }
    }
    if (programName.empty())
        return usage(argv[0]);

    // Load a program
    std::ifstream programFile(programName.c_str(), std::ios::in|std::ios::binary);
    if (!programFile.is_open()) {
        std::cerr << "Can't open program file: " << programName << std::endl;
        return 1;
    }
    const std::string suffix = programName.substr(programName.length()-2);
    bool textForm = suffix=="ks" || forceTextForm;
    Bytecode::Data programData;
    try {
        if (textForm)
            Bytecode::bytecodeFromTextStream(programFile, programData);
        else
            Bytecode::bytecodeFromDataStream(programFile, programData);
    }
    catch (std::string e) {
        std::cerr << "Can't load program file: " << e << std::endl;
        return 2;
    }
    catch (Kumir::String e) {
        std::cerr << "Can't load program file: " << Kumir::Coder::encode(LOCALE, e) << std::endl;
        return 2;
    }
    catch (...) {
        std::cerr << "Can't load program file" << std::endl;
        return 2;
    }
    programFile.close();

    // Prepare runner
    VM::KumirVM vm;
    vm.setDebugOff(false);

    InteractionHandler interactionHandler(argc, argv);
    vm.setExternalHandler(&interactionHandler);

    Kumir::String programPath = Kumir::Files::getAbsolutePath(Kumir::Coder::decode(LOCALE, programName));
    size_t slashPos = programPath.find_last_of(Kumir::Char('/'));
    Kumir::String programDir;
    if (slashPos!=Kumir::String::npos) {
        programDir = programPath.substr(0, slashPos);
    }

    vm.setProgramDirectory(programDir);

    static const Kumir::String LOAD_ERROR = Kumir::Core::fromUtf8("ОШИБКА ЗАГРУЗКИ ПРОГРАММЫ: ");

    try {
        vm.setProgram(programData, true, Kumir::Coder::decode(LOCALE, programName));
    }
    catch (Kumir::String & msg) {
        Kumir::String message = LOAD_ERROR + msg;
        return showErrorMessage(message, 11);
    }
    catch (std::string & msg) {
        Kumir::String message = LOAD_ERROR + Kumir::Core::fromAscii(msg);
        return showErrorMessage(message, 11);
    }
    catch (...) {
        Kumir::String message = LOAD_ERROR;
        return showErrorMessage(message, 11);
    }

    vm.reset();


    // Main loop
    while (vm.hasMoreInstructions()) {
        vm.evaluateNextInstruction();
        if (vm.error().length()>0) {
            static const Kumir::String RUNTIME_ERROR = Kumir::Core::fromUtf8("ОШИБКА ВЫПОЛНЕНИЯ: ");
            static const Kumir::String RUNTIME_ERROR_AT = Kumir::Core::fromUtf8("ОШИБКА ВЫПОЛНЕНИЯ В СТРОКЕ ");
            static const Kumir::String COLON = Kumir::Core::fromAscii(": ");
            Kumir::String message;
            if (vm.effectiveLineNo()!=-1) {
                message = RUNTIME_ERROR_AT+
                        Kumir::Converter::sprintfInt(vm.effectiveLineNo()+1,10,0,0)+
                        COLON+
                        vm.error();
            }
            else {
                message = RUNTIME_ERROR + vm.error();
            }
            return showErrorMessage(message, 10);
            return 10;
        }
    }
    return 0;
}
