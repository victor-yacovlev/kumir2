#ifndef LLVMCODEGENERATOR_EXTERNGENERATOR_H
#define LLVMCODEGENERATOR_EXTERNGENERATOR_H

#include <string>

#include <QScopedPointer>
#include <QString>
#include <QByteArray>
#include <QScopedPointer>
#include <QList>
#include <QMap>
#include <QSet>

#include <llvm/Config/llvm-config.h>
#if LLVM_VERSION_MINOR >= 3
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#else
#include <llvm/Module.h>
#include <llvm/LLVMContext.h>
#if LLVM_VERSION_MINOR == 0
#include <llvm/Support/IRBuilder.h>
#else
#include <llvm/IRBuilder.h>
#endif
#endif


#include "dataformats/ast_module.h"
#include "dataformats/ast_algorhitm.h"
#include "dataformats/ast_variable.h"

#include "nametranslator.h"

namespace LLVMCodeGenerator {

typedef std::string CString;

class ExternGenerator
{
public:
    explicit ExternGenerator();

    void reset(const llvm::Module * stdlibModule);
    void addFunction(const AST::ModulePtr & module,
                     const AST::AlgorithmPtr & algorithm,
                     llvm::Function * function);

    void generateBody(llvm::LLVMContext & ctx, llvm::Module * module, bool useWChar);

private:

    struct FunctionSpec {
        llvm::Function * targetFunction;
        AST::Type returnType;
        QList<AST::VariablePtr> arguments;
        QString kumirSignature;
        CString rtypeName;
        QList<CString> argnames;
        QList<CString> argtypes;
    };

    static QString kumirBaseTypeName(const AST::VariableBaseType t);
    static QString kumirTypeName(const AST::Type & t);
    llvm::Type * llvmBaseType(llvm::LLVMContext & ctx, const AST::VariableBaseType t, bool useWChar);
    llvm::Type * llvmType(llvm::LLVMContext & ctx, const AST::Type & t, bool useWChar);
    static QString createKumirSignature(const QString & moduleName,
                                        const AST::AlgorithmPtr & algorithm);

    void initConversionFunctions(const llvm::Module * stdlib);

    void generateFunction(const CString &name, FunctionSpec & spec, llvm::LLVMContext &ctx, llvm::Module *module, bool useWChar);

    QScopedPointer<NameTranslator> nameTranslator_;
    QMap<CString,FunctionSpec> externFunctions_;
    QSet<QByteArray> extraTypesUsed_;
    bool boolTypeUsed_;
    bool wcharTypeUsed_;
    bool int32TypeUsed_;

    llvm::Function * kumirCreateInt_;
    llvm::Function * kumirCreateReal_;
    llvm::Function * kumirCreateBool_;
    llvm::Function * kumirCreateCharFromChar_;
    llvm::Function * kumirCreateCharFromWChar_;
    llvm::Function * kumirCreateStringFromCs_;
    llvm::Function * kumirCreateStringFromWcs_;
    llvm::Function * kumirCreateUndefinedScalar_;
    llvm::Function * kumirCreateUndefinedArray_;

    llvm::StructType * kumirScalarType_;
    llvm::StructType * kumirArrayType_;
    llvm::PointerType * cFilePtrType_;

};

} // namespace LLVMCodeGenerator

#endif // LLVMCODEGENERATOR_EXTERNGENERATOR_H
