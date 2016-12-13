#ifndef LLVMGENERATOR_H
#define LLVMGENERATOR_H

#include "llvm_module.h"
#include "llvm_function.h"

#include "nametranslator.h"

#include "dataformats/ast.h"
#include "dataformats/ast_expression.h"
#include "dataformats/ast_type.h"

#include "interfaces/generatorinterface.h"
#include <llvm/Config/llvm-config.h>

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/MemoryBuffer.h>

#include <cstdint>
#include <stack>

// For QMap key usage
inline bool operator<(const AST::ExpressionWPtr & a, const AST::ExpressionWPtr & b)
{
    return a.data() < b.data();
}

namespace LLVMCodeGenerator {

class NameTranslator;
class LLVMCodeGeneratorPlugin;

class LLVMGenerator
{
public:
    explicit LLVMGenerator();
    void initialize(const QDir & resourcesRoot);
    void reset(bool addMainEntryPoint, Shared::GeneratorInterface::DebugLevel debugLevel);
    void addKumirModule(const AST::ModulePtr kmod);
    void createKumirModuleImplementation(const AST::ModulePtr kmod);
    LLVM::ModuleRef & getResult();
    LLVM::ModuleRef & getStdLibModule();

    void createExternsTable(const LLVM::ModuleRef & source, const QByteArray & prefix);

/* Notice:
 * The fields below are private, but f**king LLVM API changes each minor release,
 * so it is convient to keep them public for #ifdef'ed access
 */
public:
    enum AllocaPlace {
        AsIs,
        BeforeTerminator,
        FunctionBegin
    };

    typedef llvm::IRBuilder<> Builder;
    void createStdLibModule();

    LLVM::TypeRef getScalarType();
    LLVM::TypeRef getArrayType();
    LLVM::TypeRef getStringRefType();

    void addGlobalVariable(llvm::IRBuilder<> & builder, const AST::VariablePtr kvar, bool constant);
    void addFunction(const AST::AlgorithmPtr kfunc, bool createBody);
    void addFunctionBody(const QList<AST::StatementPtr> & statements, const AST::AlgorithmPtr & alg);
    void createMainFunction(const AST::AlgorithmPtr & entryPoint);

    void addExternsToModule(LLVM::ModuleRef & target);
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
    llvm::Value* createConstant(llvm::IRBuilder<> & builder, const AST::Type kty, const QVariant & value, AllocaPlace allocaPlace);
    QByteArray createArray_0_ConstantData(const AST::VariableBaseType bt, const QVariant & value, bool addDefFlag);
    QByteArray createArray_1_ConstantData(const AST::VariableBaseType bt, const QVariantList & list);
    QByteArray createArray_2_ConstantData(const AST::VariableBaseType bt, const QVariantList & list);
    QByteArray createArray_3_ConstantData(const AST::VariableBaseType bt, const QVariantList & list);
    llvm::Value* createArrayConstant(llvm::IRBuilder<> & builder, const AST::VariableBaseType bt, const uint8_t dim, const QVariant & value);
    llvm::Value* createFunctionCall(llvm::IRBuilder<> & builder, const AST::AlgorithmPtr & alg, const QList<AST::ExpressionPtr> & arguments, AllocaPlace allocaPlace);
    llvm::Value* createSubExpession(llvm::IRBuilder<> & builder, const AST::ExpressionPtr & ex);
    llvm::Value* createShortCircuitOperation(llvm::IRBuilder<> & builder, const AST::ExpressionPtr & left, const AST::ExpressionPtr & right, const AST::ExpressionOperator op, AllocaPlace allocaPlace);
    llvm::Value* createArrayElementGet(llvm::IRBuilder<> & builder, const AST::ExpressionPtr & ex, bool isLvalue, AllocaPlace allocaPlace);
    llvm::Value* createStringSliceGet(llvm::IRBuilder<> & builder, const AST::ExpressionPtr & ex, bool isLvalue, AllocaPlace allocaPlace);
    llvm::Value* findVariableAtCurrentContext(const AST::VariablePtr & var);
    void createFreeTempScalars(llvm::IRBuilder<> & builder);
    void createOutputValue(Builder & builder, const QString & name, llvm::Value * value, const AST::VariableBaseType type, const bool isArray);
    void createInputValue(Builder & builder, const QString & name, llvm::Value * value, const AST::VariableBaseType type, const bool isArray);



    llvm::AllocaInst * CreateAlloca(Builder & builder, LLVM::TypeRef ty, const QByteArray & name, AllocaPlace allocaPlace);

    LLVM::ModuleRef currentModule_;

    AST::ModulePtr currentKModule_;
    LLVM::FunctionRef currentFunction_;
    llvm::BasicBlock * currentFunctionEntry_;    
    llvm::BasicBlock * functionEntryPoint_;
    AST::AlgorithmPtr currentAlgorithm_;
    llvm::LLVMContext* context_;
    QScopedPointer<NameTranslator> nameTranslator_;
    LLVM::ModuleRef stdlibModule_;
    QMap<AST::AlgorithmPtr, LLVM::FunctionRef> functMap_;
    QByteArray stdlibContents_;
    llvm::BasicBlock* currentBlock_;
    QStack<llvm::BasicBlock*> currentLoopEnd_;
    llvm::BasicBlock* currentFunctionExit_;
    uint32_t ifThenElseCounter_;
    uint32_t switchCaseCounter_;
    uint32_t loopCounter_;            

    LLVM::FunctionRef kumirInitStdLib_;
    LLVM::FunctionRef kumirSetMainArguments_;
    LLVM::FunctionRef kumirCreateUndefinedScalar_;
    LLVM::FunctionRef kumirCreateUndefinedArray_;
    LLVM::FunctionRef kumirCreateDefinedScalar_;
    LLVM::FunctionRef kumirLinkArray_;
    LLVM::FunctionRef kumirCreateArray1_;
    LLVM::FunctionRef kumirRefArray1_;
    LLVM::FunctionRef kumirCloneArray1_;
    LLVM::FunctionRef kumirCreateArray2_;
    LLVM::FunctionRef kumirRefArray2_;
    LLVM::FunctionRef kumirCloneArray2_;
    LLVM::FunctionRef kumirCreateArray3_;
    LLVM::FunctionRef kumirRefArray3_;
    LLVM::FunctionRef kumirCloneArray3_;
    LLVM::FunctionRef kumirFreeArray_;
    LLVM::FunctionRef kumirCleanUpArrayInShape_;

    LLVM::FunctionRef kumirFillArrayI_;
    LLVM::FunctionRef kumirFillArrayR_;
    LLVM::FunctionRef kumirFillArrayB_;
    LLVM::FunctionRef kumirFillArrayC_;
    LLVM::FunctionRef kumirFillArrayS_;
    LLVM::FunctionRef kumirCreateInt_;
    LLVM::FunctionRef kumirCreateReal_;
    LLVM::FunctionRef kumirCreateBool_;
    LLVM::FunctionRef kumirCreateChar_;
    LLVM::FunctionRef kumirCreateString_;
    LLVM::FunctionRef kumirAssignScalarToScalar_;
    LLVM::FunctionRef kumirAssignScalarToStringRef_;
    LLVM::FunctionRef kumirAssignScalarToArrayElement_;
    LLVM::FunctionRef kumirMoveScalar_;
    LLVM::FunctionRef kumirFreeScalar_;
    LLVM::FunctionRef kumirGetArray1Element_;
    LLVM::FunctionRef kumirGetArray2Element_;
    LLVM::FunctionRef kumirGetArray3Element_;
    LLVM::FunctionRef kumirGetStringElement_;
    LLVM::FunctionRef kumirGetStringSlice_;
    LLVM::FunctionRef kumirGetStringElementRef_;
    LLVM::FunctionRef kumirGetStringSliceRef_;

    LLVM::FunctionRef kumirConvertCharToString_;
    LLVM::FunctionRef kumirConvertIntToReal_;

    LLVM::FunctionRef kumirLoopForFromToInitCounter_;
    LLVM::FunctionRef kumirLoopForFromToStepInitCounter_;

    LLVM::FunctionRef kumirLoopForCheckCounter_;

    LLVM::FunctionRef kumirLoopTimesInitCounter_;
    LLVM::FunctionRef kumirLoopTimesCheckCounter_;

    LLVM::FunctionRef kumirLoopEndCounter_;

    LLVM::FunctionRef kumirScalarAsBool_;

    LLVM::FunctionRef kumirOutputStdoutII_;
    LLVM::FunctionRef kumirOutputStdoutSI_;
    LLVM::FunctionRef kumirOutputStdoutIS_;
    LLVM::FunctionRef kumirOutputStdoutSS_;
    LLVM::FunctionRef kumirOutputStdout_;

    LLVM::FunctionRef kumirPrintScalarVariable_;
    LLVM::FunctionRef kumirInputScalarVariable_;
    LLVM::FunctionRef kumirPrintArrayVariable_;
    LLVM::FunctionRef kumirInputArrayVariable_;

    LLVM::FunctionRef kumirOutputFileII_;
    LLVM::FunctionRef kumirOutputFileSI_;
    LLVM::FunctionRef kumirOutputFileIS_;
    LLVM::FunctionRef kumirOutputFileSS_;

    LLVM::FunctionRef kumirInputStdin_;
    LLVM::FunctionRef kumirInputFile_;
    LLVM::FunctionRef kumirGetScalarArgument_;
    LLVM::FunctionRef kumirGetArrayArgument_;

    LLVM::FunctionRef kumirAssert_;
    LLVM::FunctionRef kumirAbortOnError_;
    LLVM::FunctionRef kumirSetCurrentLineNumber_;
    LLVM::FunctionRef kumirCheckValueDefined_;
    LLVM::FunctionRef kumirHalt_;
    LLVM::FunctionRef kumirCheckCallStack_;
    LLVM::FunctionRef kumirPopCallStackCounter_;

    LLVM::FunctionRef kumirOpEq_;
    LLVM::FunctionRef kumirOpNeq_;
    LLVM::FunctionRef kumirOpLs_;
    LLVM::FunctionRef kumirOpGt_;
    LLVM::FunctionRef kumirOpLq_;
    LLVM::FunctionRef kumirOpGq_;

    LLVM::FunctionRef kumirOpAdd_;
    LLVM::FunctionRef kumirOpSub_;
    LLVM::FunctionRef kumirOpMul_;
    LLVM::FunctionRef kumirOpDiv_;
    LLVM::FunctionRef kumirOpPow_;

    LLVM::FunctionRef kumirOpNeg_;

    std::vector<llvm::Value*> tempValsToFree_;
    std::stack<size_t> tempValsToFreeStartPos_;
    bool addMainEntryPoint_;
    QList<LLVM::FunctionRef> initFunctions_;

    Shared::GeneratorInterface::DebugLevel debugLevel_;

    QList<LLVM::FunctionRef> externs_;
    QMap<AST::ExpressionWPtr, llvm::Value*> calculateCache_;
    int lastLineNumber_;


    /**
     * @brief buildCXXName Converts function C++ name to C/LLVM name
     * @param ns C++ namespace
     * @param name C++ name
     * @param params list of parameter type names
     * @return extern "C" name
     */
    static QByteArray buildCXXName(const QString & ns,
                                const QString & clazz,
                                const QString & name,
                                const QStringList & params
                                );
};

}

#endif // LLVMGENERATOR_H
