#include "llvmcodegeneratorplugin.h"
#include "llvmgenerator.h"

#include <QtPlugin>

#include <llvm/Module.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Linker.h>
#include <llvm/Assembly/Parser.h>
#include <llvm/Support/SourceMgr.h>

#include <llvm/LLVMContext.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/PassManager.h>
#include <llvm/Target/TargetLibraryInfo.h>

#include <iostream>


namespace LLVMCodeGenerator {

LLVMCodeGeneratorPlugin::LLVMCodeGeneratorPlugin()
    : ExtensionSystem::KPlugin()
    , d(new LLVMGenerator)
    , compileOnly_(false)
    , textForm_(false)
{
}

LLVMCodeGeneratorPlugin::~LLVMCodeGeneratorPlugin()
{
    delete d;
}

QList<ExtensionSystem::CommandLineParameter>
LLVMCodeGeneratorPlugin::acceptableCommandLineParameters() const
{
    using ExtensionSystem::CommandLineParameter;
    QList<CommandLineParameter> result;
    result << CommandLineParameter(
                  false,
                  'c', "compile",
                  tr("Compile file to LLVM bitcode module, but not make executable")
                  );
    result << CommandLineParameter(
                  false,
                  'S', "assembly",
                  tr("[only with -c] Generate LLVM bitcode in text form")
                  );
    return result;
}

void LLVMCodeGeneratorPlugin::setDebugLevel(DebugLevel debugLevel)
{

}

void LLVMCodeGeneratorPlugin::generateExecuable(
            const AST::DataPtr tree,
            QByteArray & out,
            QString & mimeType,
            QString & fileSuffix
            )
{
    d->reset(true /*!compileOnly_ || (compileOnly_ && textForm_)*/);

    QList<AST::ModulePtr> & modules = tree->modules;

    AST::ModulePtr userModule, teacherModule;
    AST::ModulePtr linkedModule = AST::ModulePtr(new AST::Module);
    for (int i=0; i<modules.size(); i++) {
        AST::ModulePtr mod = modules[i];
        if (mod->header.type == AST::ModTypeUserMain) {
            userModule = mod;
        }
        else if (mod->header.type == AST::ModTypeTeacherMain) {
            teacherModule = mod;
        }        
    }
    linkedModule->impl.globals = userModule->impl.globals;
    linkedModule->impl.initializerBody = userModule->impl.initializerBody;
    linkedModule->impl.algorhitms = userModule->impl.algorhitms;
    linkedModule->header.algorhitms = userModule->header.algorhitms;
    modules.removeAll(userModule);
    if (teacherModule) {
        linkedModule->impl.globals += teacherModule->impl.globals;
        linkedModule->impl.initializerBody += teacherModule->impl.initializerBody;
        linkedModule->impl.algorhitms += teacherModule->impl.algorhitms;
        linkedModule->header.algorhitms += teacherModule->header.algorhitms;
        modules.removeAll(teacherModule);
    }
    modules.push_back(linkedModule);
    llvm::Module * lmainModule = d->createModule(linkedModule);
    modules.pop_back();
    modules.push_back(userModule);
    if (teacherModule) {
        modules.push_back(teacherModule);
    }

    std::string buf;
    llvm::raw_string_ostream ostream(buf);

    if (compileOnly_) {
        lmainModule->print(ostream, 0);
        buf = ostream.str();
        const QByteArray bufData(buf.c_str(), buf.size());
        if (textForm_) {
            out = bufData;
            mimeType = "text/llvm";
            fileSuffix = ".ll";
        }
        else {
            llvm::SMDiagnostic lerr;
            llvm::LLVMContext &lctx = llvm::getGlobalContext();
            llvm::Module * reparsedModule = llvm::ParseAssemblyString(buf.c_str(),
                                                                      0,
                                                                      lerr, lctx);
            if (reparsedModule == 0) {
                lerr.print("kumir2-llvmc", llvm::errs());
                qApp->quit();
            }
            buf.clear();
            llvm::raw_string_ostream bstream(buf);
            llvm::WriteBitcodeToFile(reparsedModule, bstream);
            bstream.flush();
            buf = bstream.str();
            const QByteArray binBufData(buf.c_str(), buf.size());
            out = binBufData;
            mimeType = "binary/llvm";
            fileSuffix = ".bc";
        }
    }
    else {
        lmainModule->print(ostream, 0);
        buf = ostream.str();
        const QByteArray bitcode(buf.c_str(), buf.size());
        out = runExternalToolsToGenerateExecutable(bitcode);
        mimeType = "executable";
#ifndef Q_OS_WIN32
        fileSuffix = "";
#else
        fileSuffix = ".exe";
#endif
    }
}

QByteArray LLVMCodeGeneratorPlugin::runExternalToolsToGenerateExecutable(const QByteArray &bitcode)
{
//    static const QString LLVM_AS = findUtil();
    static const QString LLC = findUtil("llc");
    static const QString CLang = findUtil("clang");

    // ====== Compile bitcote to machine assembly code

    QTemporaryFile bitcodeFile(QDir::tempPath() + "/XXXXXX.ll");

    bitcodeFile.open();
    bitcodeFile.write(bitcode);
    bitcodeFile.close();

    const QString bcFileName = QFileInfo(bitcodeFile).absoluteFilePath();
    const QString sFileName = bcFileName.left(bcFileName.length()-2) + "s";
    const QString exeFileName = bcFileName.left(bcFileName.length()-2) + "exe";

    const QStringList llcArguments = QStringList()
            << "-o" << sFileName << bcFileName;

    const int llc_status = QProcess::execute(LLC, llcArguments);
    QString errorMessage;
    if (llc_status == -2) {
        errorMessage = QString("Can't start LLVM bitcode to native compiler: %1 %2")
                .arg(LLC).arg(llcArguments.join(" "));
    }
    else if (llc_status == -1) {
        errorMessage = QString("LLVM bitcode to native compiler crashed while executing: %1 %2")
                .arg(LLC).arg(llcArguments.join(" "));
    }
    else if (llc_status != 0) {
        errorMessage = QString("Command exited with status %3: %1 %2")
                .arg(LLC).arg(llcArguments.join(" ")).arg(llc_status);
    }
    if (errorMessage.length() > 0) {
        std::cerr << errorMessage.toStdString() << std::endl;
        qApp->setProperty("exitStatus", 5);
        qApp->quit();
    }

    // ====== Link the code with libstdc++

    const QStringList clangArguments = QStringList()
            << "-o" << exeFileName << sFileName << "-lstdc++" << "-lm";

    const int clang_status = QProcess::execute(CLang, clangArguments);

    if (clang_status == -2) {
        errorMessage = QString("Can't start object linker: %1 %2")
                .arg(LLC).arg(llcArguments.join(" "));
    }
    else if (clang_status == -1) {
        errorMessage = QString("Object linker crashed while executing: %1 %2")
                .arg(LLC).arg(llcArguments.join(" "));
    }
    else if (clang_status != 0) {
        errorMessage = QString("Command exited with status %3: %1 %2")
                .arg(LLC).arg(llcArguments.join(" ")).arg(clang_status);
    }
    if (errorMessage.length() > 0) {
        QFile::remove(sFileName);
        std::cerr << errorMessage.toStdString() << std::endl;
        qApp->setProperty("exitStatus", 5);
        qApp->quit();
    }

    // ====== Read result
    QFile executableFile(exeFileName);
    executableFile.open(QIODevice::ReadOnly);
    const QByteArray result = executableFile.readAll();
    executableFile.close();
    QFile::remove(sFileName);
    QFile::remove(exeFileName);
    return result;
}

QString LLVMCodeGeneratorPlugin::findUtil(const QString &name)
{
    QString exec = name;
#ifdef Q_OS_WIN32
    exec += ".exe";
#endif
    QStringList paths;
    foreach (const QString & pathName, paths) {
        const QString fullPath = QDir(pathName).absoluteFilePath(exec);
        if (QFile(fullPath).exists()) {
            return fullPath;
        }
    }
    return exec;
}

//std::string LLVMCodeGeneratorPlugin::generateNativeExecutable(llvm::Module *mod)
//{
//    std::string outbuf;
//    using namespace llvm;

//    LLVMContext &context = getGlobalContext();
//    InitializeNativeTarget();
//    InitializeNativeTargetAsmPrinter();
//    InitializeNativeTargetAsmParser();

//    Triple triple = getNativeTriple();
//    mod->setTargetTriple(triple.str());
//    std::string err;
//    const Target * theTarget = TargetRegistry::lookupTarget(triple.str(), err);
//    if (!theTarget) {
//        std::cerr << err << std::endl;
//        return outbuf;
//    }

//    CodeGenOpt::Level OLvl = CodeGenOpt::Default;
//    std::string FeaturesStr;
//    TargetOptions Options;

//    TargetMachine * target = theTarget->createTargetMachine(
//                triple.getTriple(), // triple
//                "", // mcpu
//                FeaturesStr, // features
//                Options, // options
//                Reloc::Default,
//                CodeModel::Default,
//                OLvl // opt level
//                );

//    Q_ASSERT(target);

//    PassManager PM;
//    PM.add(new TargetLibraryInfo(triple));
//    PM.add(new TargetTransformInfo(target->getScalarTargetTransformInfo(),
//                                   target->getVectorTargetTransformInfo()));
//    PM.add(new DataLayout(*target->getDataLayout()));

//    raw_string_ostream stream(outbuf);
//    formatted_raw_ostream ostream(stream);

//    bool success = !target->addPassesToEmitFile(
//                PM,
//                ostream,
//                TargetMachine::CGFT_AssemblyFile
//                );

//    if (!success) {
//        return outbuf;
//    }

//    PM.run(*mod);

//    return stream.str();
//}

//llvm::Triple LLVMCodeGeneratorPlugin::getNativeTriple()
//{
//    llvm::Triple::ArchType arch;
//    if (sizeof(void*) == 8u) {
//        arch = llvm::Triple::x86_64;
//    }
//    else {
//        arch = llvm::Triple::x86;
//    }
//    llvm::Triple::VendorType vendor;
//    llvm::Triple::OSType os;
//#ifdef Q_OS_MAC
//    vendor = llvm::Triple::Apple;
//    os = llvm::Triple::MacOSX;
//#else
//    vendor = llvm::Triple::PC;
//#endif
//#ifdef Q_OS_WIN32
//    os = llvm::Triple::Win32;
//#endif
//#ifdef Q_OS_LINUX
//    os = llvm::Triple::Linux;
//#endif
//#ifdef Q_OS_FREEBSD
//    os = llvm::Triple::FreeBSD;
//#endif
//    return llvm::Triple(llvm::Triple::getArchTypeName(arch),
//                        llvm::Triple::getVendorTypeName(vendor),
//                        llvm::Triple::getOSTypeName(os));
//}

void LLVMCodeGeneratorPlugin::setOutputToText(bool flag)
{
    textForm_ = flag;
}

QString LLVMCodeGeneratorPlugin::initialize(
        const QStringList &configurationArguments,
        const ExtensionSystem::CommandLine &runtimeArguments)
{
    compileOnly_ = runtimeArguments.hasFlag('c');
    textForm_ = runtimeArguments.hasFlag('S');
    return "";
}

void LLVMCodeGeneratorPlugin::start()
{

}

void LLVMCodeGeneratorPlugin::stop()
{

}

}

Q_EXPORT_PLUGIN2(LLVMCodeGenerator, LLVMCodeGenerator::LLVMCodeGeneratorPlugin)
