#include <QtCore>
#include <iostream>

#include "shared/bytecode/data.h"

int usage(const char * programName)
{
    std::cerr << "Usage: " << std::endl;
    std::cerr << "\t" << programName << " [-o=OUTFILE.kod] FILENAME.ks" << std::endl << std::endl;
    std::cerr << "\t-o=OUTFILE.kod\t\tKumir runtime code file name" << std::endl;
    std::cerr << "\tFILENAME.ks\t\tKumir assembler input file name" << std::endl;
    return 127;
}

int main(int argc, char *argv[])
{
    QString inFileName;
    QString outFileName;
    for (int i=1; i<argc; i++) {
        QString arg = QString::fromLocal8Bit(argv[i]);
        if (arg.startsWith("-o=")) {
            outFileName= arg.mid(3);
        }
        if (!arg.startsWith("-"))
            inFileName = arg;
    }
    if (inFileName.isEmpty())
        return usage(argv[0]);
    if (outFileName.isEmpty()) {
        if (inFileName.endsWith(".ks")) {
            outFileName = inFileName.left(inFileName.length()-3) + ".kod";
        }
        else {
            outFileName = inFileName+".kod";
        }
    }
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
