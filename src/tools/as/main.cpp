#include <iostream>
#include <fstream>
#include "stdlib/kumirstdlib.hpp"
#include "vm/variant.hpp"
#include "vm/vm_bytecode.hpp"

#if !defined(WIN32) && !defined(_WIN32)
#include <sys/stat.h>
#endif

int usage(const char * programName)
{
    std::cerr << "Usage: " << std::endl;
    std::cerr << "\t" << programName << " [-o=OUTFILE.kod] FILENAME.ks" << std::endl << std::endl;
    std::cerr << "\t-o=OUTFILE.kod\tKumir runtime code file name" << std::endl;
    std::cerr << "\tFILENAME.ks\tKumir assembler input file name" << std::endl;
    std::cerr << std::endl << "or:" << std::endl;
    std::cerr << "\t" << programName << " -d [-o=OUTFILE.ks] FILENAME.kod" << std::endl << std::endl;
    std::cerr << "\t-o=OUTFILE.ks\tKumir assembler text file name" << std::endl;
    std::cerr << "\tFILENAME.kod\tKumir runtime code file name" << std::endl;
    return 127;
}

int assemble(const std::string & inFileName, const std::string & outFileName)
{
    std::ifstream inFile(inFileName.c_str());
    if (!inFile.is_open()) {
        std::cerr << "Can't open " << inFileName << std::endl;
        return 1;
    }
    Bytecode::Data data;
    Bytecode::bytecodeFromTextStream(inFile, data);
    inFile.close();
    std::ofstream outFile(outFileName.c_str());
    if (!outFile.is_open()) {
        std::cerr << "Can't open " << outFileName << std::endl;
        return 2;
    }
    Bytecode::bytecodeToDataStream(outFile, data);
    outFile.close();
#if !defined(WIN32) && !defined(_WIN32)
    static const mode_t mode = S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH;
    chmod(outFileName.c_str(), mode);
#endif
    return 0;
}

int disassemble(const std::string & inFileName, const std::string & outFileName)
{
    std::ifstream inFile(inFileName.c_str());
    if (!inFile.is_open()) {
        std::cerr << "Can't open " << inFileName << std::endl;
        return 1;
    }
    Bytecode::Data data;
    Bytecode::bytecodeFromDataStream(inFile, data);
    inFile.close();
    std::ofstream outFile(outFileName.c_str());
    if (!outFile.is_open()) {
        std::cerr << "Can't open " << outFileName << std::endl;
        return 2;
    }
    Bytecode::bytecodeToTextStream(outFile, data);
    outFile.close();
#if !defined(WIN32) && !defined(_WIN32)
    static const mode_t mode = S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH;
    chmod(outFileName.c_str(), mode);
#endif
    return 0;
}

int main(int argc, char *argv[])
{
    std::string inFileName;
    std::string outFileName;
    bool disassembler = false;
    std::string suffix = ".kod";
    std::string inSuffix = ".ks";
    for (int i=1; i<argc; i++) {
        std::string  arg(argv[i]);
        if (arg.at(0)=='-') {
            if (arg.length()>3 && arg.at(1)=='o' && arg.at(2)=='=')
                outFileName = arg.substr(3);
            else if (arg.length()==2 && (arg.at(1)=='d' || arg.at(1)=='D') ) {
                disassembler = true;
                suffix = ".ks";
                inSuffix = ".kod";
            }
        }
        else
            inFileName = arg;
    }
    if (inFileName.empty())
        return usage(argv[0]);
    if (outFileName.empty()) {
        if (!disassembler && inFileName.substr(inFileName.length()-2)=="ks")
            outFileName = inFileName.substr(0,inFileName.length()-3)+suffix;
        else if (disassembler && inFileName.substr(inFileName.length()-3)=="kod")
            outFileName = inFileName.substr(0,inFileName.length()-4)+suffix;
        else
            outFileName = inFileName+suffix;
    }
    if (disassembler) {
        return disassemble(inFileName, outFileName);
    }
    else {
        return assemble(inFileName, outFileName);
    }
}
