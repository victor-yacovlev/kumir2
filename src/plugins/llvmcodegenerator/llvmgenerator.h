#ifndef LLVMGENERATOR_H
#define LLVMGENERATOR_H

#include "nametranslator.h"

#include "dataformats/ast.h"
#include "dataformats/ast_expression.h"
#include "dataformats/ast_type.h"

#include "interfaces/generatorinterface.h"
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
#include <llvm/Support/MemoryBuffer.h>

#include <cstdint>
#include <stack>

namespace LLVMCodeGenerator {

class NameTranslator;
class LLVMCodeGeneratorPlugin;

class LLVMGenerator
{
public:
    explicit LLVMGenerator();
    void initialize(const QDir & resourcesRoot);
    void reset(bool addMainEntryPoint, Shared::GeneratorInterface::DebugLevel debugLevel);
    typedef QPair<AST::AlgorithmPtr, llvm::Function*> FunctionTwine;
    QList<FunctionTwine> addKumirModule(const AST::ModulePtr kmod);
    void createKumirModuleImplementation(const AST::ModulePtr kmod);
    llvm::Module * getResult();
    llvm::Module * getStdLibModule();
    void createExternsTable(const llvm::Module * const source, const CString & prefix);
private:
    typedef llvm::IRBuilder<> Builder;
    void createStdLibModule();

    llvm::StructType * getScalarType();
    llvm::StructType * getArrayType();
    llvm::StructType * getStringRefType();

    void addGlobalVariable(llvm::IRBuilder<> & builder, const AST::VariablePtr kvar, bool constant);
    llvm::Function * addFunction(const AST::AlgorithmPtr kfunc, bool createBody);
    void addFunctionBody(const QList<AST::StatementPtr> & statements, const AST::AlgorithmPtr & alg);
    void createMainFunction(const AST::AlgorithmPtr & entryPoint);

    void addExternsToModule(llvm::Module * const target);
    void readStdLibFunctions();

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
    void createOutputValue(Builder & builder, const QString & name, llvm::Value * value, const AST::VariableBaseType type, const bool isArray);
    void createInputValue(Builder & builder, const QString & name, llvm::Value * value, const AST::VariableBaseType type, const bool isArray);
    static CString createAsciiName(const QString & unicodeName);

    llvm::AllocaInst * CreateAlloca(Builder & builder, llvm::Type * ty, const CString & name = CString());

    llvm::Module* currentModule_;
    AST::ModulePtr currentKModule_;
    llvm::Function* currentFunction_;
    llvm::BasicBlock * currentFunctionEntry_;
    AST::AlgorithmPtr currentAlgorithm_;
    llvm::LLVMContext* context_;    
    llvm::Module* stdlibModule_;
    QScopedPointer<llvm::MemoryBuffer> stdlibContents_;
    llvm::BasicBlock* currentBlock_;
    QStack<llvm::BasicBlock*> currentLoopEnd_;
    llvm::BasicBlock* currentFunctionExit_;
    uint32_t ifThenElseCounter_;
    uint32_t switchCaseCounter_;
    uint32_t loopCounter_;            

    llvm::Function* kumirInitStdLib_;
    llvm::Function* kumirSetMainArguments_;
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

    llvm::Function* kumirLoopForCheckCounter_;

    llvm::Function* kumirLoopTimesInitCounter_;
    llvm::Function* kumirLoopTimesCheckCounter_;

    llvm::Function* kumirLoopEndCounter_;

    llvm::Function* kumirScalarAsBool_;

    llvm::Function* kumirOutputStdoutII_;
    llvm::Function* kumirOutputStdoutSI_;
    llvm::Function* kumirOutputStdoutIS_;
    llvm::Function* kumirOutputStdoutSS_;
    llvm::Function* kumirOutputStdout_;

    llvm::Function* kumirPrintScalarVariable_;
    llvm::Function* kumirInputScalarVariable_;
    llvm::Function* kumirPrintArrayVariable_;
    llvm::Function* kumirInputArrayVariable_;

    llvm::Function* kumirOutputFileII_;
    llvm::Function* kumirOutputFileSI_;
    llvm::Function* kumirOutputFileIS_;
    llvm::Function* kumirOutputFileSS_;

    llvm::Function* kumirInputStdin_;
    llvm::Function* kumirInputFile_;
    llvm::Function* kumirGetScalarArgument_;
    llvm::Function* kumirGetArrayArgument_;

    llvm::Function* kumirAssert_;
    llvm::Function* kumirAbortOnError_;
    llvm::Function* kumirSetCurrentLineNumber_;
    llvm::Function* kumirCheckValueDefined_;
    llvm::Function* kumirHalt_;
    llvm::Function* kumirCheckCallStack_;
    llvm::Function* kumirPopCallStackCounter_;

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

    std::vector<llvm::Value*> tempValsToFree_;
    std::stack<size_t> tempValsToFreeStartPos_;
    bool addMainEntryPoint_;
    std::list<llvm::Function*> initFunctions_;

    Shared::GeneratorInterface::DebugLevel debugLevel_;

    QList<const llvm::Function*> externs_;


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
