#ifndef LLVMGENERATOR_H
#define LLVMGENERATOR_H

#include "nametranslator.h"

#include "dataformats/ast.h"
#include "dataformats/ast_expression.h"
#include "dataformats/ast_type.h"

#include "interfaces/generatorinterface.h"

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
    void reset(bool linkStdLibModule, Shared::GeneratorInterface::DebugLevel debugLevel);
    void addKumirModule(const AST::ModulePtr kmod);
    void createKumirModuleImplementation(const AST::ModulePtr kmod);
    llvm::Module * getResult();
private:
    void createStdLibModule();

    void addGlobalVariable(llvm::IRBuilder<> & builder, const AST::VariablePtr kvar, bool constant);
    void addFunction(const AST::AlgorithmPtr kfunc, bool createBody);
    void addFunctionBody(const QList<AST::StatementPtr> & statements, const AST::AlgorithmPtr & alg);
    void createMainFunction(const AST::AlgorithmPtr & entryPoint);

    void createExternsTable();
    void createInternalExternsTable();

    void createVarInitialize(llvm::IRBuilder<> & builder, const AST::StatementPtr & st, bool global);
    llvm::Value* createVarInitialize(llvm::IRBuilder<> & builder, const AST::VariablePtr & var, const QString &overrideName, bool global);
    void createAssign(llvm::IRBuilder<> & builder, const AST::StatementPtr & st, const AST::AlgorithmPtr & alg);
    void createAssert(llvm::IRBuilder<> & builder, const AST::StatementPtr & st, const AST::AlgorithmPtr & alg);
    void createOutput(llvm::IRBuilder<> & builder, const AST::StatementPtr & st, const AST::AlgorithmPtr & alg);
    void createHalt(llvm::IRBuilder<> & builder, const AST::StatementPtr & st, const AST::AlgorithmPtr & alg);
    void createInput(llvm::IRBuilder<> & builder, const AST::StatementPtr & st, const AST::AlgorithmPtr & alg);
    void createLoop(llvm::IRBuilder<> & builder, const AST::StatementPtr & st, const AST::AlgorithmPtr & alg);
    void createIfThenElse(llvm::IRBuilder<> & builder, const AST::StatementPtr & st, const AST::AlgorithmPtr & alg);
    void createSwitchCaseElse(llvm::IRBuilder<> & builder, const AST::StatementPtr & st, const AST::AlgorithmPtr & alg);
    void createBreak(llvm::IRBuilder<> & builder, const AST::StatementPtr & st, const AST::AlgorithmPtr & alg);
    void createError(llvm::IRBuilder<> & builder, const AST::StatementPtr & st);
    llvm::Value* calculate(llvm::IRBuilder<> & builder, const AST::ExpressionPtr & ex, bool isLvalue = false);
    llvm::Value* createConstant(llvm::IRBuilder<> & builder, const AST::Type kty, const QVariant & value);
    QByteArray createArray_0_ConstantData(const AST::VariableBaseType bt, const QVariant & value, bool addDefFlag);
    QByteArray createArray_1_ConstantData(const AST::VariableBaseType bt, const QVariantList & list);
    QByteArray createArray_2_ConstantData(const AST::VariableBaseType bt, const QVariantList & list);
    QByteArray createArray_3_ConstantData(const AST::VariableBaseType bt, const QVariantList & list);
    llvm::Value* createArrayConstant(llvm::IRBuilder<> & builder, const AST::VariableBaseType bt, const uint8_t dim, const QVariant & value);
    llvm::Value* createFunctionCall(llvm::IRBuilder<> & builder, const AST::AlgorithmPtr & alg, const QList<AST::ExpressionPtr> & arguments);
    llvm::Value* createSubExpession(llvm::IRBuilder<> & builder, const AST::ExpressionPtr & ex);
    llvm::Value* createShortCircuitOperation(llvm::IRBuilder<> & builder, const AST::ExpressionPtr & left, const AST::ExpressionPtr & right, const AST::ExpressionOperator op);
    llvm::Value* createArrayElementGet(llvm::IRBuilder<> & builder, const AST::ExpressionPtr & ex, bool isLvalue);
    llvm::Value* createStringSliceGet(llvm::IRBuilder<> & builder, const AST::ExpressionPtr & ex, bool isLvalue);
    llvm::Value* findVariableAtCurrentContext(const AST::VariablePtr & var);
    void createFreeTempScalars(llvm::IRBuilder<> & builder);

    llvm::Module* currentModule_;
    llvm::Function* currentFunction_;
    AST::AlgorithmPtr currentAlgorithm_;
    llvm::LLVMContext* context_;
    QScopedPointer<NameTranslator> nameTranslator_;
    llvm::Module* stdlibModule_;
    QScopedPointer<llvm::MemoryBuffer> stdlibContents_;
    llvm::BasicBlock* currentBlock_;
    llvm::BasicBlock* currentLoopEnd_;
    llvm::BasicBlock* currentFunctionExit_;
    uint32_t ifThenElseCounter_;
    uint32_t switchCaseCounter_;
    uint32_t loopCounter_;


    llvm::Function* kumirInitStdLib_;
    llvm::Function* kumirCreateUndefinedScalar_;
    llvm::Function* kumirCreateUndefinedArray_;
    llvm::Function* kumirCreateDefinedScalar_;
    llvm::Function* kumirLinkArray_;
    llvm::Function* kumirCreateArray1_;
    llvm::Function* kumirRefArray1_;
    llvm::Function* kumirCloneArray1_;
    llvm::Function* kumirCreateArray2_;
    llvm::Function* kumirRefArray2_;
    llvm::Function* kumirCloneArray2_;
    llvm::Function* kumirCreateArray3_;
    llvm::Function* kumirRefArray3_;
    llvm::Function* kumirCloneArray3_;
    llvm::Function* kumirFreeArray_;
    llvm::Function* kumirCleanUpArrayInShape_;

    llvm::Function* kumirFillArrayI_;
    llvm::Function* kumirFillArrayR_;
    llvm::Function* kumirFillArrayB_;
    llvm::Function* kumirFillArrayC_;
    llvm::Function* kumirFillArrayS_;
    llvm::Function* kumirCreateInt_;
    llvm::Function* kumirCreateReal_;
    llvm::Function* kumirCreateBool_;
    llvm::Function* kumirCreateChar_;
    llvm::Function* kumirCreateString_;
    llvm::Function* kumirAssignScalarToScalar_;
    llvm::Function* kumirAssignScalarToStringRef_;
    llvm::Function* kumirAssignScalarToArrayElement_;
    llvm::Function* kumirMoveScalar_;
    llvm::Function* kumirFreeScalar_;
    llvm::Function* kumirGetArray1Element_;
    llvm::Function* kumirGetArray2Element_;
    llvm::Function* kumirGetArray3Element_;
    llvm::Function* kumirGetStringElement_;
    llvm::Function* kumirGetStringSlice_;
    llvm::Function* kumirGetStringElementRef_;
    llvm::Function* kumirGetStringSliceRef_;

    llvm::Function* kumirLoopForFromToInitCounter_;
    llvm::Function* kumirLoopForFromToStepInitCounter_;

    llvm::Function* kumirLoopForFromToCheckCounter_;
    llvm::Function* kumirLoopForFromToStepCheckCounter_;

    llvm::Function* kumirLoopTimesInitCounter_;
    llvm::Function* kumirLoopTimesCheckCounter_;

    llvm::Function* kumirLoopEndCounter_;

    llvm::Function* kumirScalarAsBool_;

    llvm::Function* kumirOutputStdoutII_;
    llvm::Function* kumirOutputStdoutSI_;
    llvm::Function* kumirOutputStdoutIS_;
    llvm::Function* kumirOutputStdoutSS_;

    llvm::Function* kumirOutputFileII_;
    llvm::Function* kumirOutputFileSI_;
    llvm::Function* kumirOutputFileIS_;
    llvm::Function* kumirOutputFileSS_;

    llvm::Function* kumirInputStdin_;
    llvm::Function* kumirInputFile_;

    llvm::Function* kumirAssert_;
    llvm::Function* kumirAbortOnError_;
    llvm::Function* kumirSetCurrentLineNumber_;
    llvm::Function* kumirCheckValueDefined_;
    llvm::Function* kumirHalt_;

    llvm::Function* kumirOpEq_;
    llvm::Function* kumirOpNeq_;
    llvm::Function* kumirOpLs_;
    llvm::Function* kumirOpGt_;
    llvm::Function* kumirOpLq_;
    llvm::Function* kumirOpGq_;

    llvm::Function* kumirOpAdd_;
    llvm::Function* kumirOpSub_;
    llvm::Function* kumirOpMul_;
    llvm::Function* kumirOpDiv_;
    llvm::Function* kumirOpPow_;

    llvm::Function* kumirOpNeg_;

    llvm::StructType* kumirScalarType_;
    llvm::StructType* kumirArrayType_;
    llvm::StructType* kumirStringRefType_;


    std::list<llvm::Value*> tempValsToFree_;
    bool linkStdLibModule_;
    std::list<llvm::Function*> initFunctions_;

    Shared::GeneratorInterface::DebugLevel debugLevel_;


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
