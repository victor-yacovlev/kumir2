#include <QtCore>

#include <dataformats/ast_variable.h>
#include <dataformats/ast_algorhitm.h>
#include <dataformats/ast_expression.h>

#include "kumirllvmgeneratorplugin.h"
#include "llvmnameprovider.h"

#include <llvm/Support/IRReader.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/TargetFolder.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/raw_os_ostream.h>



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

    return "";
}

QPair<QString,QString> KumirLLVMGeneratorPlugin::generateExecuable(
        const AST::Data *tree,
        QIODevice *out,
        QStringList *usedDlls)
{
    llvm::LLVMContext context;
    prepareContext(context);

    std::list<llvm::Module*> kumModules;

    for (int i=0; i<tree->modules.size(); i++) {
        AST::Module * mod = tree->modules[i];
        if (mod->header.type==AST::ModTypeUser && mod->header.enabled)
            kumModules.push_back(addKumirModule(context, tree, mod));
    }

    llvm::Module * mainMod = kumModules.front();
    std::string mainText;
    llvm::raw_string_ostream mainTextStream(mainText);
    mainMod->print(mainTextStream, 0);
    out->write(mainText.c_str());
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

llvm::Type * KumirLLVMGeneratorPlugin::makeType(llvm::LLVMContext & context, AST::VariableBaseType base, int dimension)
{
    if (base!=AST::TypeNone) {

        /** Each value have "defined/undefined" flag, so
            even primitive types are structs
            struct variable<BASE> {
                int8_t defined;
                <BASE> value;
            };
        **/

        llvm::Type * _flag = llvm::Type::getInt8Ty(context);
        llvm::Type * _base = 0;
        if (base==AST::TypeInteger)
            _base = llvm::Type::getInt32Ty(context);
        else if (base==AST::TypeReal)
            _base = llvm::Type::getDoubleTy(context);
        else if (base==AST::TypeBoolean)
            _base = llvm::Type::getInt8Ty(context);
        else if (base==AST::TypeCharect)
            _base = llvm::Type::getInt32Ty(context);
        else if (base==AST::TypeString)
            _base = llvm::Type::getInt32PtrTy(context);
        llvm::StructType * _struct = llvm::StructType::create(context);
        _struct->setBody(_flag, _base, 0);
        if (dimension==0)
            return _struct;

        /** Each table is struct
            struct table<BASE> {
                int8_t  dimension;
                int32_t size0;
                int32_t size1;
                int32_t size2;
                int32_t start0;
                int32_t start1;
                int32_t start2;
                BASE*   data;
            }
        **/

        llvm::StructType * _table = llvm::StructType::create(context);
        llvm::Type * _dim = llvm::Type::getInt8Ty(context);
        llvm::Type * _size0 = llvm::Type::getInt32Ty(context);
        llvm::Type * _size1 = llvm::Type::getInt32Ty(context);
        llvm::Type * _size2 = llvm::Type::getInt32Ty(context);
        llvm::Type * _start0 = llvm::Type::getInt32Ty(context);
        llvm::Type * _start1 = llvm::Type::getInt32Ty(context);
        llvm::Type * _start2 = llvm::Type::getInt32Ty(context);
        llvm::Type * _data = llvm::PointerType::get(_struct, 0);
        _table->setBody(_dim,
                        _size0, _size1, _size2,
                        _start0, _start1, _start2,
                        _data,
                        0);
        return _table;
    }
    else {
        return llvm::Type::getVoidTy(context);
    }
}

llvm::Module * KumirLLVMGeneratorPlugin::addKumirModule(llvm::LLVMContext &context,
                                              const AST::Data *ast,
                                              const AST::Module *module)
{
    std::string modName;
    if (module->header.name.isEmpty())
        modName = nameProvider->addName("__main__", "").toStdString();
    else
        modName = nameProvider->addName(module->header.name, "").toStdString();
    llvm::Module * lmodule = new llvm::Module(modName, context);
    // Make global variables definitions
    foreach (const AST::Variable* var, module->impl.globals) {
        const std::string varName = nameProvider->addName(var->name, QString::fromStdString(modName)).toStdString();
        llvm::Type * ty = makeType(context, var->baseType, var->dimension);
        llvm::Constant* lvar = lmodule->getOrInsertGlobal(varName, ty);
    }
    // Make function definitions
    foreach (const AST::Algorhitm* alg, module->impl.algorhitms) {
        std::string algName;
        if (alg->header.name.isEmpty())
            algName = nameProvider->addName("__main__", QString::fromStdString(modName)).toStdString();
        else
            algName = nameProvider->addName(alg->header.name, QString::fromStdString(modName)).toStdString();
        llvm::Type * retType = makeType(context, alg->header.returnType, 0);
        std::vector<llvm::Type*> params(alg->header.arguments.size());
        for (int i=0; i<alg->header.arguments.size(); i++) {
            const AST::Variable * argument = alg->header.arguments[i];
            llvm::Type * argType = makeType(context, argument->baseType, argument->dimension);
            if (argument->accessType!=AST::AccessArgumentIn) {
                argType = llvm::PointerType::get(argType, 0);
            }
            params[i] = argType;
        }
        llvm::FunctionType * ft = llvm::FunctionType::get(retType, params, false);
        llvm::Constant* lfunc = lmodule->getOrInsertFunction(algName, ft);
    }
    // Make function implementations
    foreach (const AST::Algorhitm * alg, module->impl.algorhitms) {
        addAlgorhitmBody(context, lmodule, ast, module, alg);
    }
    return lmodule;
}

void KumirLLVMGeneratorPlugin::addAlgorhitmBody(
        llvm::LLVMContext &context, // LLVM context
        llvm::Module * lmodule,  // LLVM module
        const AST::Data *ast, // Complete AST tree
        const AST::Module *module, // Parent AST module
        const AST::Algorhitm *alg // Algorhitm to add
        )
{
    std::string modName;
    if (module->header.name.isEmpty())
        modName = nameProvider->addName("__main__", "").toStdString();
    else
        modName = nameProvider->addName(module->header.name, "").toStdString();
    std::string algName;
    if (alg->header.name.isEmpty())
        algName = nameProvider->addName("__main__", QString::fromStdString(modName)).toStdString();
    else
        algName = nameProvider->addName(alg->header.name, QString::fromStdString(modName)).toStdString();
    llvm::Function * lfunc = lmodule->getFunction(algName);
    Q_CHECK_PTR(lfunc);

}

}

Q_EXPORT_PLUGIN(KumirLLVMGenerator::KumirLLVMGeneratorPlugin)
