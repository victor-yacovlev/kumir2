#include "kumirllvmcompilerplugin.h"

#include <QtCore>
#include "kumiranalizer/analizer.h"
#include "kumiranalizer/kumiranalizerplugin.h"
#include "interfaces/generatorinterface.h"
#include "dataformats/kumfile.h"

#include <iostream>

using namespace KumirLLVMCompiler;
using namespace KumirAnalizer;


KumirLLVMCompilerPlugin::KumirLLVMCompilerPlugin()
{


}


KumirLLVMCompilerPlugin::~KumirLLVMCompilerPlugin()
{

}


QString KumirLLVMCompilerPlugin::initialize(const QStringList &arguments)
{
    Q_UNUSED(arguments)
    m_analizer = qobject_cast<Shared::AnalizerInterface*>(myDependency("KumirAnalizer"));
    if (m_analizer)
        return "";
    else
        return "Could not find KumirAnalizer interface";
}

#include <iostream>


void KumirLLVMCompilerPlugin::start()
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
            QDataStream ts(&f);
            KumFile::Data kumFile;
            ts >> kumFile;
            f.close();

            int id = m_analizer->newDocument();
            m_analizer->setSourceText(id, kumFile.visibleText);
            if (kumFile.hasHiddenText) {
                m_analizer->setHiddenText(id, kumFile.hiddenText, -1);
            }
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
                std::cerr << "Error: " <<
                             QFileInfo(filename).fileName().toLocal8Bit().data() <<
                             ":" << e.line+1 <<
                             ":" << e.start+1 << "-" << e.start+e.len <<
                             ": " << e.code.toLocal8Bit().data() << std::endl;
            }
            Shared::GeneratorInterface * generator =
                    qobject_cast<Shared::GeneratorInterface*>(myDependency("Generator"));
            Q_CHECK_PTR(generator);

            QString outKodFileName = QFileInfo(filename).dir().absoluteFilePath(baseName+".o");
            foreach (QString arg, qApp->arguments()) {
                if (arg.startsWith("-o=")) {
                    outKodFileName = arg.mid(3);
                }
            }

            if (!outKodFileName.endsWith(".o"))
                outKodFileName += ".o";

            QFile binOut(outKodFileName);
            QPair<QString,QString> res = generator->generateExecuable(ast, &binOut);
            if (!res.first.isEmpty()) {
                std::cerr << "Error generating execuable: " << res.first.toStdString() << std::endl;
            }
            qApp->setProperty("returnCode", errors.isEmpty() && res.first.isEmpty()? 0 : 1);
        }
    }
    else {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "\t" << qApp->argv()[0] << " [-J] [-o=OUTFILE.o] FILENAME.kum" << std::endl;
        std::cerr << std::endl;
        std::cerr << "\t-J\t\tIf present, generates internal AST in JSON file" << std::endl;
        std::cerr << "\t-o OUTFILE.o\tOutput file name (default: FILENAME.o)" << std::endl;
        std::cerr << "\tFILENAME.kum\tKumir program input file name" << std::endl;
        qApp->setProperty("returnCode", 127);
    }
}

void KumirLLVMCompilerPlugin::stop()
{

}


Q_EXPORT_PLUGIN(KumirLLVMCompilerPlugin)
