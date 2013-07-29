#include "kumircompilertoolplugin.h"
#include "extensionsystem/pluginmanager.h"

#include <QtCore>
#include "kumiranalizer/analizer.h"
#include "kumiranalizer/kumiranalizerplugin.h"
#include "interfaces/generatorinterface.h"
#include "dataformats/kumfile.h"
#include "stdlib/kumirstdlib.hpp"
#include "vm/variant.hpp"
#include "vm/vm_bytecode.hpp"
#include <iostream>
#include <fstream>

using namespace KumirCompilerTool;
using namespace KumirAnalizer;

typedef Shared::GeneratorInterface::DebugLevel DebugLevel;

KumirCompilerToolPlugin::KumirCompilerToolPlugin()
    : KPlugin()
    , analizer_(nullptr)
    , generator_(nullptr)
    , useAnsiWindowsOutput_(false)
{
}

QList<ExtensionSystem::CommandLineParameter>
KumirCompilerToolPlugin::acceptableCommandLineParameters() const
{
    using ExtensionSystem::CommandLineParameter;
    QList<CommandLineParameter> result;
    result << CommandLineParameter(
                  false,
                  'a', "ansi",
                  tr("Show error messages in console using CP1251 encoding instead of CP866 (Windows only)")
                  );
    result << CommandLineParameter(
                  false,
                  'e', "encoding",
                  tr("Explicitly set source file text encoding"),
                  QVariant::String, false
                  );
    result << CommandLineParameter(
                  false,
                  'o', "out",
                  tr("Explicitly set output file name"),
                  QVariant::String, false
                  );

    // Startup parameters

    result << CommandLineParameter(
                  false,
                  tr("PROGRAM.kum"),
                  tr("Source file name"),
                  QVariant::String,
                  true
                  );
    return result;
}


QString KumirCompilerToolPlugin::initialize(
        const QStringList & /*configurationArguments*/,
        const ExtensionSystem::CommandLine & runtimeArguments
        )
{
    using namespace Shared;
    using namespace ExtensionSystem;
    PluginManager * manager = PluginManager::instance();
    analizer_ = manager->findPlugin<AnalizerInterface>();
    generator_ = manager->findPlugin<GeneratorInterface>();

    for (int i=1; i<qApp->argc(); i++) {
        const QString arg = qApp->arguments()[i];
        if ( !arg.startsWith("-") && !arg.startsWith("[") && arg.endsWith(".kum")) {
            sourceFileName_ = arg;
        }
    }

    if (sourceFileName_.isEmpty()) {
        return tr("Error: source file name not specified.\nRun with --help parameter for more details");
    }

    useAnsiWindowsOutput_ = runtimeArguments.hasFlag('a');
    sourceFileEncoding_ = runtimeArguments.value('e').toString();
    outFileName_ = runtimeArguments.value('o').toString();

    return QString();
}

#include <iostream>


void KumirCompilerToolPlugin::start()
{

    QString filename = QFileInfo(filename).absoluteFilePath();
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly)) {
        QDataStream ts(&f);
        KumFile::Data kumFile;
        kumFile.sourceEncoding = sourceFileEncoding_;
        ts >> kumFile;
        f.close();

        int id = analizer_->newDocument();
        QString dirname = QFileInfo(filename).absoluteDir().absolutePath();
        analizer_->setSourceDirName(id, dirname);
        analizer_->setSourceText(id, kumFile.visibleText + "\n" + kumFile.hiddenText);
        QList<Shared::Error> errors = analizer_->errors(id);
        const AST::DataPtr ast = analizer_->abstractSyntaxTree(id);
        const QString baseName = QFileInfo(filename).baseName();

        for (int i=0; i<errors.size(); i++) {
            Shared::Error e = errors[i];
            QString errorMessage = tr("Error: ") +
                    QFileInfo(filename).fileName() +
                    ":" + QString::number(e.line+1) +
                    ":" + QString::number(e.start+1) + "-" + QString::number(e.start+e.len) +
                    ": " + e.code;
#ifdef Q_OS_WIN32
            QTextCodec * cp866 = QTextCodec::codecForName("CP866");
            if (useAnsiWindowsOutput_) {
                std::cerr << cp866->fromUnicode(errorMessage).data();
            }
            else {
                std::cerr << errorMessage.toLocal8Bit().data();
            }
#else
            std::cerr << errorMessage.toLocal8Bit().data();
#endif
            std::cerr << std::endl;
        }

        QString suffix;
        QString mimeType;
        QByteArray outData;
        generator_->generateExecuable(ast, outData, mimeType, suffix);

        QString outFileName = QFileInfo(filename).dir().absoluteFilePath(baseName+suffix);

        if (outFileName_.length() > 0)
            outFileName = outFileName_;


        if (!outFileName.endsWith(suffix))
            outFileName += suffix;

        QFile binOut(outFileName);

        binOut.open(QIODevice::WriteOnly);
        binOut.write(outData);
        binOut.close();
        if (!mimeType.contains("text") && QFile::exists(outFileName)) {
            QFile::Permissions ps = binOut.permissions();
            ps |= QFile::ExeGroup | QFile::ExeOwner | QFile::ExeOther;
            QFile::setPermissions(outFileName, ps);
        }
        qApp->setProperty("returnCode", errors.isEmpty()? 0 : 1);
    }
    else {
        const QString errorMessage = tr("Can't open file %1").arg(filename);
#ifdef Q_OS_WIN32
        QTextCodec * cp866 = QTextCodec::codecForName("CP866");
        if (useAnsiWindowsOutput_) {
            std::cerr << cp866->fromUnicode(errorMessage).data();
        }
        else {
            std::cerr << errorMessage.toLocal8Bit().data();
        }
#else
        std::cerr << errorMessage.toLocal8Bit().data();
#endif
        std::cerr << std::endl;
        qApp->setProperty("returnCode", 2);
    }
}

void KumirCompilerToolPlugin::stop()
{

}


Q_EXPORT_PLUGIN(KumirCompilerToolPlugin)
