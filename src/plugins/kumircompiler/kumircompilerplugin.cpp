#include "kumircompilerplugin.h"

#include <QtCore>
#include "kumiranalizer/analizer.h"
#include "kumiranalizer/kumiranalizerplugin.h"
#include "interfaces/generatorinterface.h"

#include <iostream>

using namespace KumirCompiler;
using namespace KumirAnalizer;


KumirCompilerPlugin::KumirCompilerPlugin()
{


}


KumirCompilerPlugin::~KumirCompilerPlugin()
{

}


QString KumirCompilerPlugin::initialize(const QStringList &arguments)
{
    Q_UNUSED(arguments)
    m_analizer = qobject_cast<Shared::AnalizerInterface*>(myDependency("KumirAnalizer"));
    if (m_analizer)
        return "";
    else
        return "Could not find KumirAnalizer interface";
}

#include <iostream>


void KumirCompilerPlugin::start()
{
    QString suffix = ".bin";
#ifdef Q_OS_WIN32
    suffix = ".exe";
#endif
#ifdef Q_OS_MAC
    suffix = "";
#endif
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
//            data = QString::fromUtf8("алг \nнач\nвыбор \nпри 1=1  и да:\nпри да или нет:\nиначе\nвывод 1\nвсе \nкон\n");
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

            QString outBinFileName = QFileInfo(filename).dir().absoluteFilePath(baseName+suffix);
            foreach (QString arg, qApp->arguments()) {
                if (arg.startsWith("-o=")) {
                    outBinFileName = arg.mid(3);
                }
            }
#ifdef Q_OS_WIN32
            if (!outBinFileName.endsWith(".exe"))
                outBinFileName += ".exe";
#endif
#ifdef Q_OS_MAC
            if (!outBinFileName.endsWith(".app"))
                outBinFileName += ".app";
            QString bundleDir = outBinFileName;
            const QString frameworksPath = QDir::cleanPath(qApp->applicationDirPath()+"/../Frameworks/");
            const QString pluginsPath = QDir::cleanPath(qApp->applicationDirPath()+"/../PlugIns/");
            QDir::current().mkpath(bundleDir+"/Contents/MacOS");
//            QDir::current().mkpath(bundleDir+"/Contents/Frameworks");
//            QDir::current().mkpath(bundleDir+"/Contents/PlugIns");
            QProcess::execute("cp -R "+frameworksPath+" "+bundleDir+"/Contents/");
            QProcess::execute("cp -R "+pluginsPath+" "+bundleDir+"/Contents/");
            outBinFileName = baseName;
            foreach (QString arg, qApp->arguments()) {
                if (arg.startsWith("-o=")) {
                    outBinFileName = arg.mid(3);
                }
            }
            if (outBinFileName.endsWith(".app"))
                outBinFileName = outBinFileName.left(outBinFileName.length()-4);
            outBinFileName = bundleDir+"/Contents/MacOS/"+outBinFileName;
#endif
            QFile binOut(outBinFileName);
            binOut.open(QIODevice::WriteOnly);
            Shared::GeneratorResult res = generator->generateExecuable(ast, &binOut);
            binOut.close();
#ifdef Q_OS_WIN32
            foreach (QString qtLib, res.usedQtLibs) {
                QString dllName = qtLib+"4";
#ifndef QT_NO_DEBUG
                dllName += "d";
#endif
                dllName += ".dll";
                if (!QFile::exists(dllName)) {
                    QFile::copy(QCoreApplication::applicationDirPath()+"/"+dllName, dllName);
                }
            }
#endif
            if (res.type==Shared::GenNativeExecuable && QFile::exists(outBinFileName)) {
                QFile::Permissions ps = binOut.permissions();
                ps |= QFile::ExeGroup | QFile::ExeOwner | QFile::ExeOther;
                QFile::setPermissions(outBinFileName, ps);
            }
            qApp->setProperty("returnCode", errors.isEmpty()? 0 : 1);
        }
    }
    else {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "\t" << qApp->argv()[0] << " [-J] [-S] [-V] [-o=OUTFILE" << suffix.toLocal8Bit().data() <<"] FILENAME.kum" << std::endl;
        std::cerr << std::endl;
        std::cerr << "\t-J\t\tIf present, generates internal AST in JSON file" << std::endl;
        std::cerr << "\t-S\t\tIf present, keeps generated C-code on disk" << std::endl;
        std::cerr << "\t-V\t\tIf present, displays verbose information" << std::endl;
        std::cerr << "\t-o OUTFILE" << suffix.toLocal8Bit().data() << "\tOutput file name (default: FILENAME" << suffix.toLocal8Bit().data() << ")" << std::endl;
        std::cerr << "\tFILENAME.kum\tKumir program input file name" << std::endl;
        qApp->setProperty("returnCode", 127);
    }
    qApp->quit();
}

void KumirCompilerPlugin::stop()
{

}


Q_EXPORT_PLUGIN(KumirCompilerPlugin)
