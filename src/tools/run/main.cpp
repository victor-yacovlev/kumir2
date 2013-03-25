#include <iostream>
#include <fstream>
#include "stdlib/kumirstdlib.hpp"
#include "vm/vm_abstract_handlers.h"
#include "vm/vm_console_handlers.hpp"
#include "vm/variant.hpp"
#include "vm/vm_bytecode.hpp"
#include "vm/vm.hpp"

#include <algorithm>

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#endif

using namespace Kumir;

static Encoding LOCALE;

static void do_output(const String &s)
{
    const std::string localstring = Coder::encode(LOCALE, s);
    std::cout << localstring;
}

static void do_output(const std::string & s)
{
    do_output(Core::fromUtf8(s));
}

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

int showErrorMessage(const String & message, int code) {
    bool toHttp = false;
#if !defined(WIN32) && !defined(_WIN32)
    char * REQUEST_METHOD = getenv("REQUEST_METHOD");
    char * QUERY_STRING = getenv("QUERY_STRING");
    toHttp = (REQUEST_METHOD!=0 && QUERY_STRING!=0);
#endif
    if (!toHttp) {
        const std::string localMessage = Coder::encode(LOCALE, message);
        std::cerr << localMessage << std::endl;
        return code;
    }
    else {
        const std::string localMessage = Coder::encode(UTF8, message);
        std::cout << "Content-type: text/html;charset=utf-8\n\n\n";
        std::cout << "<html><head><title>An error occured on server</title></head>\n";
        std::cout << "<body>\n";
        std::cout << localMessage << std::endl;
        std::cout << "</body></html>\n";
        return 0;
    }
}

bool IsPluginExtern(const Bytecode::TableElem & e) {
    bool isExtern = e.type==Bytecode::EL_EXTERN;
    bool isKumirModule = e.fileName.length()>4 &&
            e.fileName.substr(e.fileName.length()-4)==Kumir::Core::fromAscii(".kod");
    return isExtern && !isKumirModule;
}

int runKumirXRun(int argc, char* argv[]) {

#if defined(WIN32) || defined(_WIN32)
    // Win32 implementation
#else
    // Posix implementation
    std::string kumir2run = std::string(argv[0]);
    if (kumir2run.at(0)!='/') {
        char buf[2048];
        getcwd(buf, 2048);
        std::string cwd(buf);
        cwd.push_back('/');
        kumir2run = cwd + kumir2run;
    }
    size_t lastSlash = kumir2run.find_last_of('/');
    const std::string basePath = kumir2run.substr(0, lastSlash+1);
    const std::string kumir2xrun = basePath + std::string("kumir2-xrun");
    return execv(kumir2xrun.c_str(), argv);
#endif
    return 127;
}

int main(int argc, char *argv[])
{
//    sleep(15); // for remote debugger
    // Look at arguments
#if defined(WIN32) || defined(_WIN32)
    IO::LOCALE_ENCODING = LOCALE = CP866;
#else
    IO::LOCALE_ENCODING = LOCALE = UTF8;
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
                IO::LOCALE_ENCODING = LOCALE = CP1251;
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
    catch (String e) {
        std::cerr << "Can't load program file: " << Coder::encode(LOCALE, e) << std::endl;
        return 2;
    }
    catch (...) {
        std::cerr << "Can't load program file" << std::endl;
        return 2;
    }
    programFile.close();

    // Check if it's possible to run using regular runtime
    bool hasPluginDependency =
            std::count_if(programData.d.begin(), programData.d.end(), IsPluginExtern);

    if (hasPluginDependency) {
        return runKumirXRun(argc, argv);
    }

    // Prepare runner
    VM::KumirVM vm;

    VM::Console::InputFunctor inputFunctor;
    VM::Console::OutputFunctor outputFunctor;
    VM::Console::GetMainArgumentFunctor getMainArgumentFunctor;
    VM::Console::ReturnMainValueFunctor returnMainValueFunctor;

    inputFunctor.setLocale(LOCALE);
    outputFunctor.setLocale(LOCALE);
    getMainArgumentFunctor.setLocale(LOCALE);
    returnMainValueFunctor.setLocale(LOCALE);

    getMainArgumentFunctor.init(argc, argv);

    vm.setFunctor(&inputFunctor);
    vm.setFunctor(&outputFunctor);
    vm.setFunctor(&getMainArgumentFunctor);
    vm.setFunctor(&returnMainValueFunctor);

    String programPath = Files::getAbsolutePath(Coder::decode(LOCALE, programName));
    size_t slashPos = programPath.find_last_of(Char('/'));
    String programDir;
    if (slashPos!=String::npos) {
        programDir = programPath.substr(0, slashPos);
    }

    vm.setProgramDirectory(programDir);

    static const String LOAD_ERROR = Core::fromUtf8("ОШИБКА ЗАГРУЗКИ ПРОГРАММЫ: ");

    try {
        vm.setProgram(programData, true, Coder::decode(LOCALE, programName));
    }
    catch (String & msg) {
        String message = LOAD_ERROR + msg;
        return showErrorMessage(message, 11);
    }
    catch (std::string & msg) {
        String message = LOAD_ERROR + Core::fromAscii(msg);
        return showErrorMessage(message, 11);
    }
    catch (...) {
        String message = LOAD_ERROR;
        return showErrorMessage(message, 11);
    }

    vm.reset();
    vm.setDebugOff(true);


    // Main loop
    while (vm.hasMoreInstructions()) {
        vm.evaluateNextInstruction();
        if (vm.error().length()>0) {
            static const String RUNTIME_ERROR = Core::fromUtf8("ОШИБКА ВЫПОЛНЕНИЯ: ");
            static const String RUNTIME_ERROR_AT = Core::fromUtf8("ОШИБКА ВЫПОЛНЕНИЯ В СТРОКЕ ");
            static const String COLON = Core::fromAscii(": ");
            String message;
            if (vm.effectiveLineNo()!=-1) {
                message = RUNTIME_ERROR_AT+
                        Converter::sprintfInt(vm.effectiveLineNo()+1,10,0,0)+
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
