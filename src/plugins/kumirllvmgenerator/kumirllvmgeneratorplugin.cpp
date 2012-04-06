#include <QtCore>
#include "kumirllvmgeneratorplugin.h"
#include "llvmnameprovider.h"

#include <llvm/Support/IRReader.h>
#include <llvm/Support/IRBuilder.h>

namespace KumirLLVMGenerator {


KumirLLVMGeneratorPlugin::KumirLLVMGeneratorPlugin()
    : KPlugin()
{
    nameProvider = new NameProvider(this);
}

KumirLLVMGeneratorPlugin::~KumirLLVMGeneratorPlugin()
{

}

QString KumirLLVMGeneratorPlugin::initialize(const QStringList &arguments)
{


}

QPair<QString,QString> KumirLLVMGeneratorPlugin::generateExecuable(
        const AST::Data *tree,
        QIODevice *out,
        QStringList *usedDlls)
{
    llvm::LLVMContext context;
    prepareContext(context);

    for (int i=0; i<tree->modules.size(); i++) {
        AST::Module * mod = tree->modules[i];
        addKumirModule(context, tree, mod);
    }

}

QString KumirLLVMGeneratorPlugin::modulesPath()
{
#ifdef Q_OS_WIN32
    return QCoreApplication::applicationDirPath()+"/modules/";
#endif
#ifdef Q_OS_UNIX
    return QDir::cleanPath(QCoreApplication::applicationDirPath()+"/../"+QString::fromLocal8Bit(IDE_LIBRARY_BASENAME)+"/kumir2/modules/");
#endif
}

void KumirLLVMGeneratorPlugin::prepareContext(llvm::LLVMContext &context)
{
    // load stdlib modules
    llvm::SMDiagnostic diag;
    const QDir modulesDir(modulesPath());
    const QStringList modFileNames = modulesDir.entryList(QStringList() << "*.llvm.o");
    foreach (const QString &modFileName, modFileNames) {
        const std::string qualifedFileName = QString(modulesPath()+"/"+modFileName).toStdString();
        llvm::Module * module = llvm::ParseIRFile(qualifedFileName, diag, context);
        if (!module) {
             qDebug() << QString::fromStdString(diag.getMessage());
        }
    }
}

void KumirLLVMGeneratorPlugin::addKumirModule(llvm::LLVMContext &context,
                                              const AST::Data *ast,
                                              const AST::Module *module)
{
    std::string modName;
    if (module->header.name.isEmpty())
        modName = "main";
    else
        modName = nameProvider->addName(module->header.name, "").toStdString();
    llvm::Module * lmodule = new llvm::Module(modName, context);

}

}
