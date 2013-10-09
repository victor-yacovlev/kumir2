#ifndef LLVMGENERATOR_H
#define LLVMGENERATOR_H

#include "nametranslator.h"

#include "dataformats/ast.h"
#include "dataformats/ast_expression.h"
#include "dataformats/ast_type.h"


#include <llvm/Module.h>
#include <llvm/LLVMContext.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/IRBuilder.h>

#include <cstdint>

namespace LLVMCodeGenerator {

class NameTranslator;

class LLVMGenerator
{
public:
    explicit LLVMGenerator();
    void reset(bool linkStdLibModule);
    llvm::Module* createModule(const AST::ModulePtr kmod);
private:
    void createStdLibModule();

    void addGlobalVariable(const AST::VariablePtr kvar, bool constant);
    void addFunction(const AST::AlgorithmPtr kfunc, bool createBody);
    void addFunctionBody(const QList<AST::StatementPtr> & statements, const AST::AlgorithmPtr & alg);
    void createMainFunction(const AST::AlgorithmPtr & entryPoint);

    void createExternsTable();
    void createInternalExternsTable();

    void createVarInitialize(llvm::IRBuilder<> & builder, const AST::StatementPtr & st);
    llvm::Value* createVarInitialize(llvm::IRBuilder<> & builder, const AST::VariablePtr & var, const QString &overrideName);
    void createAssign(llvm::IRBuilder<> & builder, const AST::StatementPtr & st, const AST::AlgorithmPtr & alg);
    void createAssert(llvm::IRBuilder<> & builder, const AST::StatementPtr & st, const AST::AlgorithmPtr & alg);
    void createOutput(llvm::IRBuilder<> & builder, const AST::StatementPtr & st, const AST::AlgorithmPtr & alg);
    llvm::Value* calculate(llvm::IRBuilder<> & builder, const AST::ExpressionPtr & ex, bool lvalue);
    llvm::Value* createConstant(llvm::IRBuilder<> & builder, const AST::Type kty, const QVariant & value);    
    llvm::Value* createFunctionCall(llvm::IRBuilder<> & builder, const AST::AlgorithmPtr & alg, const QList<AST::ExpressionPtr> & arguments);
    llvm::Value* createSubExpession(llvm::IRBuilder<> & builder, const AST::ExpressionPtr & ex);
    void createFreeTempScalars(llvm::IRBuilder<> & builder);

    llvm::Type * findOrRegisterType(const AST::Type &bt, const bool reference, const uint8_t dim);
    llvm::Type * findStandardType(const AST::VariableBaseType bt, const bool reference, const uint8_t dim);

    llvm::Module* currentModule_;
    llvm::Function* currentFunction_;
    llvm::LLVMContext* context_;
    QScopedPointer<NameTranslator> nameTranslator_;
    llvm::Module* stdlibModule_;
    QScopedPointer<llvm::MemoryBuffer> stdlibContents_;
    QStack<llvm::BasicBlock*> currentBlock_;


    llvm::Function* kumirInitStdLib_;
    llvm::Function* kumirInitScalar_;
    llvm::Function* kumirCreateInt_;
    llvm::Function* kumirCreateReal_;
    llvm::Function* kumirCreateBool_;
    llvm::Function* kumirCreateChar_;
    llvm::Function* kumirCreateString_;
    llvm::Function* kumirAssignScalarToScalar_;
    llvm::Function* kumirFreeScalar_;

    llvm::Function* kumirOutputStdoutII_;
    llvm::Function* kumirOutputStdoutSI_;
    llvm::Function* kumirOutputStdoutIS_;
    llvm::Function* kumirOutputStdoutSS_;

    llvm::Function* kumirOutputFileII_;
    llvm::Function* kumirOutputFileSI_;
    llvm::Function* kumirOutputFileIS_;
    llvm::Function* kumirOutputFileSS_;

    std::list<llvm::Value*> tempValsToFree_;
    bool linkStdLibModule_;


    /**
     * @brief buildCXXName Converts function C++ name to C/LLVM name
     * @param ns C++ namespace
     * @param name C++ name
     * @param params list of parameter type names
     * @return extern "C" name
     */
    static CString buildCXXName(const QString & ns,
                                const QString & clazz,
                                const QString & name,
                                const QStringList & params
                                );
};

}

#endif // LLVMGENERATOR_H
