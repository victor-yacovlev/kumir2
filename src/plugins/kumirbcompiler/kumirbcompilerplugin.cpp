#include "kumirbcompilerplugin.h"

#include <QtCore>
#include "kumiranalizer/analizer.h"
#include "kumiranalizer/kumiranalizerplugin.h"
#include "interfaces/generatorinterface.h"
#include "dataformats/bytecode.h"

#include <iostream>

using namespace KumirBytecodeCompiler;
using namespace KumirAnalizer;


KumirBytecodeCompilerPlugin::KumirBytecodeCompilerPlugin()
{


}


KumirBytecodeCompilerPlugin::~KumirBytecodeCompilerPlugin()
{

}


QString KumirBytecodeCompilerPlugin::initialize(const QStringList &arguments)
{
    Q_UNUSED(arguments)
    m_analizer = qobject_cast<Shared::AnalizerInterface*>(myDependency("KumirAnalizer"));
    if (m_analizer)
        return "";
    else
        return "Could not find KumirAnalizer interface";
}

#include <iostream>


void KumirBytecodeCompilerPlugin::start()
{
    QString filename;
    for (int i=1; i<qApp->argc(); i++) {
        const QString arg = qApp->arguments()[i];
        if ( !arg.startsWith("-") && !arg.startsWith("[") && arg.endsWith(".kum")) {
            filename = arg;
        }
    }
    if (!filename.isEmpty() && !qApp->arguments().contains("-h") && !qApp->arguments().contains("-help") && !qApp->arguments().contains("--help") && !qApp->arguments().contains("/?")) {
        filename = QFileInfo(filename).absoluteFilePath();
        QFile f(filename);
        if (f.open(QIODevice::ReadOnly)) {
            QTextStream ts(&f);
            ts.setCodec("UTF-16");
            QString data = ts.readAll();
            f.close();

            int id = m_analizer->newDocument();
            m_analizer->setSourceText(id, data);
            QList<Shared::Error> errors = m_analizer->errors(id);
            const AST::Data * ast = m_analizer->abstractSyntaxTree(id);
            const QString baseName = QFileInfo(filename).baseName();
            const QString outFileName = QFileInfo(filename).dir().absoluteFilePath(
                        baseName+".dump.json");
            QFile ff(outFileName);
            if (qApp->arguments().contains("-J") && ff.open(QIODevice::WriteOnly|QIODevice::Text)) {
                QTextStream ots(&ff);
                ots.setCodec("UTF-8");
                ots.setGenerateByteOrderMark(true);
                QString modJs = ast->dump();
                modJs.replace("\t", "  ");
                ots << modJs;
                ff.close();;
            }
            for (int i=0; i<errors.size(); i++) {
                Shared::Error e = errors[i];
                std::cerr << filename.toLocal8Bit().data() << ":" << e.line << ": Error " << e.code.toLocal8Bit().data() << std::endl;
            }
            Shared::GeneratorInterface * generator =
                    qobject_cast<Shared::GeneratorInterface*>(myDependency("Generator"));
            Q_CHECK_PTR(generator);

            QString outKodFileName = QFileInfo(filename).dir().absoluteFilePath(baseName+".kod");
            foreach (QString arg, qApp->arguments()) {
                if (arg.startsWith("-o=")) {
                    outKodFileName = arg.mid(3);
                }
            }

            if (!outKodFileName.endsWith(".kod"))
                outKodFileName += ".kod";

            QFile binOut(outKodFileName);
            binOut.open(QIODevice::WriteOnly);
            Shared::GeneratorResult res = generator->generateExecuable(ast, &binOut);
            binOut.close();
            if (res.type==Shared::GenNotNativeExecuable && QFile::exists(outKodFileName)) {
                QFile::Permissions ps = binOut.permissions();
                ps |= QFile::ExeGroup | QFile::ExeOwner | QFile::ExeOther;
                QFile::setPermissions(outKodFileName, ps);
                if (qApp->arguments().contains("-S")) {
                    binOut.open(QIODevice::ReadOnly);
                    QDataStream ds(&binOut);
                    Bytecode::Data data;
                    ds >> data;
                    binOut.close();
                    const QString SfileName = outKodFileName.mid(0,outKodFileName.length()-4)+".ks";
                    QFile Sfile(SfileName);
                    if (Sfile.open(QIODevice::WriteOnly)) {
                        QTextStream ts(&Sfile);
                        ts << data;
                        Sfile.close();
                    }
                }
            }
            qApp->setProperty("returnCode", errors.isEmpty()? 0 : 1);
        }
    }
    else {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "\t" << qApp->argv()[0] << " [-J] [-S] [-o=OUTFILE.kod] FILENAME.kum" << std::endl;
        std::cerr << std::endl;
        std::cerr << "\t-J\t\tIf present, generates internal AST in JSON file" << std::endl;
        std::cerr << "\t-S\t\tIf present, generates human-readable form of code" << std::endl;
        std::cerr << "\t-o OUTFILE.kod\tOutput file name (default: FILENAME.kod)" << std::endl;
        std::cerr << "\tFILENAME.kum\tKumir program input file name" << std::endl;
        qApp->setProperty("returnCode", 127);
    }
}

void KumirBytecodeCompilerPlugin::stop()
{

}


Q_EXPORT_PLUGIN(KumirBytecodeCompilerPlugin)
