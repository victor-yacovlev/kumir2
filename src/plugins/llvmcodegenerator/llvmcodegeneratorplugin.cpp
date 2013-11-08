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
    , linkStdLib_(false)
    , debugLevel_(LinesOnly)
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
    result << CommandLineParameter(
                  false,
                  'g', "debuglevel",
                  tr("Generate code with debug level from 0 (nothing) to 2 (maximum debug information)"),
                  QVariant::Int, false
                  );
//    result << CommandLineParameter(
//                  false,
//                  'l', "linkstdlib",
//                  tr("[only with -c] Link standard Kumir library into bitcode")
//                  );
    // TODO implement it
    return result;
}

void LLVMCodeGeneratorPlugin::setDebugLevel(DebugLevel debugLevel)
{
    debugLevel_ = debugLevel;
}

void LLVMCodeGeneratorPlugin::generateExecuable(
            const AST::DataPtr tree,
            QByteArray & out,
            QString & mimeType,
            QString & fileSuffix
            )
{
    d->reset(!compileOnly_ || linkStdLib_, debugLevel_);

    const QList<AST::ModulePtr> & modules = tree->modules;
    QList<AST::ModulePtr> kmodules;

    AST::ModulePtr userModule, teacherModule;
    AST::ModulePtr startupKumirModule = AST::ModulePtr(new AST::Module);
    startupKumirModule->header.type = AST::ModTypeUserMain;
    for (int i=0; i<modules.size(); i++) {
        AST::ModulePtr mod = modules[i];
        if (mod->header.type == AST::ModTypeUserMain) {
            userModule = mod;
        }
        else if (mod->header.type == AST::ModTypeTeacherMain) {
            teacherModule = mod;
        }        
        else if (mod->header.type != AST::ModTypeExternal) {
            kmodules.push_back(mod);
        }
    }
    startupKumirModule->impl.globals = userModule->impl.globals;
    startupKumirModule->impl.initializerBody = userModule->impl.initializerBody;
    startupKumirModule->impl.algorhitms = userModule->impl.algorhitms;
    startupKumirModule->header.algorhitms = userModule->header.algorhitms;
    if (teacherModule) {
        startupKumirModule->impl.globals += teacherModule->impl.globals;
        startupKumirModule->impl.initializerBody += teacherModule->impl.initializerBody;
        startupKumirModule->impl.algorhitms += teacherModule->impl.algorhitms;
        startupKumirModule->header.algorhitms += teacherModule->header.algorhitms;
    }
    kmodules.push_back(startupKumirModule);

    foreach (AST::ModulePtr kmod, kmodules) {
        d->addKumirModule(kmod);
    }

    foreach (AST::ModulePtr kmod, kmodules) {
        d->createKumirModuleImplementation(kmod);
    }

    llvm::Module * lmainModule = d->getResult();
    std::string buf;
    llvm::raw_string_ostream ostream(buf);


    lmainModule->print(ostream, 0);
    buf = ostream.str();
    const QByteArray bufData(buf.c_str(), buf.size());
    if (compileOnly_ && textForm_) {
        out = bufData;
        mimeType = "text/llvm";
        fileSuffix = ".ll";
        return;
    }

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
    if (compileOnly_) {
        out = binBufData;
        mimeType = "binary/llvm";
        fileSuffix = ".bc";
        return;
    }

    out = runExternalToolsToGenerateExecutable(binBufData);
    mimeType = "executable";
#ifndef Q_OS_WIN32
    fileSuffix = "";
#else
    fileSuffix = ".exe";
#endif

}

QByteArray LLVMCodeGeneratorPlugin::runExternalToolsToGenerateExecutable(const QByteArray &bitcode)
{
    static const QString CLang = findUtil("clang");

    // ====== Compile bitcote to machine assembly code

    QTemporaryFile bitcodeFile(QDir::tempPath() + "/XXXXXX.bc");

    bitcodeFile.open();
    bitcodeFile.write(bitcode);
    bitcodeFile.close();

    const QString bcFileName = QFileInfo(bitcodeFile).absoluteFilePath();
    const QString exeFileName = bcFileName.left(bcFileName.length()-2) + "exe";

    // ====== Use CLang toolchain to produce executable

    const QStringList clangArguments = QStringList()
            << "-O0"
            << "-o" << exeFileName << bcFileName << "-lstdc++" << "-lm";

    const int clang_status = QProcess::execute(CLang, clangArguments);
    QString errorMessage;
    if (clang_status == -2) {
        errorMessage = QString("Can't start CLang: %1 %2")
                .arg(CLang).arg(clangArguments.join(" "));
    }
    else if (clang_status == -1) {
        errorMessage = QString("CLang crashed while executing: %1 %2")
                .arg(CLang).arg(clangArguments.join(" "));
    }
    else if (clang_status != 0) {
        errorMessage = QString("Command exited with status %3: %1 %2")
                .arg(CLang).arg(clangArguments.join(" ")).arg(clang_status);
    }
    if (errorMessage.length() > 0) {
        std::cerr << errorMessage.toStdString() << std::endl;
        qApp->setProperty("returnCode", 5);
        qApp->quit();
    }

    // ====== Read result
    QFile executableFile(exeFileName);
    executableFile.open(QIODevice::ReadOnly);
    const QByteArray result = executableFile.readAll();
    executableFile.close();    
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
        const QStringList &,
        const ExtensionSystem::CommandLine &runtimeArguments)
{
    compileOnly_ = runtimeArguments.hasFlag('c');
    textForm_ = runtimeArguments.hasFlag('S');
    linkStdLib_ = true;
    DebugLevel debugLevel = LinesOnly;
    if (runtimeArguments.value('g').isValid()) {
        int level = runtimeArguments.value('g').toInt();
        level = qMax(0, level);
        level = qMin(2, level);
        debugLevel = DebugLevel(level);
    }
    setDebugLevel(debugLevel);
//    linkStdLib_ = runtimeArguments.hasFlag('l');
    // TODO implement it
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
