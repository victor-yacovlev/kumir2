#include <QtCore>
#include <iostream>

#include "shared/dataformats/bytecode.h"

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

int assemble(const QString & inFileName, const QString & outFileName)
{
    QFile inFile(inFileName);
    if (inFile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&inFile);
        Bytecode::Data data;
        ts >> data;
        inFile.close();
        QFile out(outFileName);
        if (out.open(QIODevice::WriteOnly)) {
            QDataStream ds(&out);
            ds << data;
            out.close();
            QFile::Permissions perms = out.permissions();
            perms |= QFile::ExeOther | QFile::ExeOwner | QFile::ExeUser | QFile::ExeGroup;
            out.setPermissions(perms);
        }
        else {
            std::cerr << "Can't open " << outFileName.toLocal8Bit().data() << std::endl;
            return 2;
        }
    }
    else {
        std::cerr << "Can't open " << inFileName.toLocal8Bit().data() << std::endl;
        return 1;
    }
    return 0;
}

int disassemble(const QString & inFileName, const QString & outFileName)
{
    QFile inFile(inFileName);
    if (inFile.open(QIODevice::ReadOnly)) {
        QDataStream ds(&inFile);
        Bytecode::Data data;
        ds >> data;
        inFile.close();
        QFile out(outFileName);
        if (out.open(QIODevice::WriteOnly)) {
            QTextStream ts(&out);
            ts << data;
            out.close();
        }
        else {
            std::cerr << "Can't open " << outFileName.toLocal8Bit().data() << std::endl;
            return 2;
        }
    }
    else {
        std::cerr << "Can't open " << inFileName.toLocal8Bit().data() << std::endl;
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    QString inFileName;
    QString outFileName;
    bool disassembler = false;
    QString suffix = ".kod";
    QString inSuffix = ".ks";
    for (int i=1; i<argc; i++) {
        QString arg = QString::fromLocal8Bit(argv[i]);
        if (arg.startsWith("-o=")) {
            outFileName= arg.mid(3);
        }
        if (!arg.startsWith("-"))
            inFileName = arg;
        if (arg.startsWith("-d") || arg.startsWith("-D")) {
            disassembler = true;
            suffix = ".ks";
            inSuffix = ".kod";
        }
    }
    if (inFileName.isEmpty())
        return usage(argv[0]);
    if (outFileName.isEmpty()) {
        if (inFileName.endsWith(inSuffix)) {
            outFileName = inFileName.left(inFileName.length()-inSuffix.length()) + suffix;
        }
        else {
            outFileName = inFileName+suffix;
        }
    }
    if (disassembler) {
        return disassemble(inFileName, outFileName);
    }
    else {
        return assemble(inFileName, outFileName);
    }
}
