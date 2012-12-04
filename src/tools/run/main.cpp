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
    vm.setProgram(programData);
    vm.reset();

    // Main loop
    while (vm.hasMoreInstructions()) {
        vm.evaluateNextInstruction();
        if (Kumir::Core::getError().length()>0) {
            static const Kumir::String RUNTIME_ERROR = Kumir::Core::fromUtf8("ОШИБКА ВЫПОЛНЕНИЯ: ");
            const Kumir::String message = RUNTIME_ERROR + Kumir::Core::getError();
            const std::string localMessage = Kumir::Coder::encode(LOCALE, message);
            std::cerr << localMessage << std::endl;
            return 10;
        }
    }
    return 0;
}
