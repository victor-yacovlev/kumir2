#include <iostream>
#include <fstream>
#include "stdlib/kumirstdlib.hpp"
#include "vm/variant.hpp"
#include "vm/vm_bytecode.hpp"
#include "vm/vm.hpp"

#if !defined(WIN32) && !defined(_WIN32)
#define LOCALE Kumir::UTF8
#else
#define LOCALE Kumir::CP866
#endif

int usage(const char * programName)
{
    std::cerr << "Usage: " << std::endl;
    std::cerr << "\t" << programName << " FILENAME.kod | FILENAME.ks [ARG1 [ARG2 ... [ARGn]]]" << std::endl << std::endl;
    std::cerr << "\tFILENAME.kod\tKumir runtime code file name" << std::endl;
    std::cerr << "\tFILENAME.ks\tKumir assembler input file name" << std::endl;
    std::cerr << "\tARG1...ARGn\tKumir program main algorithm arguments" << std::endl;
    return 127;
}

class InteractionHandler
        : public VM::AbstractInteractionHandler
{
public:
    explicit InteractionHandler(int argc, char *argv[]);
    bool makeInputArgument(VM::Variable & reference);
private:
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

int main(int argc, char *argv[])
{
    // Look at arguments
    std::string programName;
    std::deque<std::string> args;
    bool forceTextForm = false;
    for (int i=1; i<argc; i++) {
        std::string  arg(argv[i]);
        static const std::string minuss("-s");
        static const std::string minusS("-S");
        if (programName.empty()) {
            if (arg==minuss || arg==minusS)
                forceTextForm = true;
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
    std::ifstream programFile(programName.c_str());
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

    InteractionHandler interactionHandler(argc, argv);
    vm.setExternalHandler(&interactionHandler);

    vm.setProgram(programData);
    vm.reset();


    // Main loop
    while (vm.hasMoreInstructions()) {
        vm.evaluateNextInstruction();
        if (vm.error().length()>0) {
            static const Kumir::String RUNTIME_ERROR = Kumir::Core::fromUtf8("ОШИБКА ВЫПОЛНЕНИЯ: ");
            const Kumir::String message = RUNTIME_ERROR + vm.error();
            const std::string localMessage = Kumir::Coder::encode(LOCALE, message);
            std::cerr << localMessage << std::endl;
            return 10;
        }
    }
    return 0;
}
