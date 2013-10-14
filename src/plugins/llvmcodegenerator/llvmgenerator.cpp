#include "llvmgenerator.h"
#include "nametranslator.h"

#include "dataformats/ast_variable.h"
#include "dataformats/ast_algorhitm.h"
#include "dataformats/ast_statement.h"
#include "dataformats/ast_expression.h"

#include <llvm/Type.h>
#include <llvm/TypeBuilder.h>
#include <llvm/GlobalVariable.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/system_error.h>
#include <llvm/Constants.h>
#include <llvm/ValueSymbolTable.h>
#include <llvm/Value.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Linker.h>
#include <llvm/Attributes.h>

#include <string>
#include <iostream>
#include <assert.h>
#include <algorithm>

namespace LLVMCodeGenerator {

#define ctx *context_

LLVMGenerator::LLVMGenerator()
    : currentModule_(0)
    , currentFunction_(0)
    , context_(0)
    , nameTranslator_(new NameTranslator)
    , stdlibModule_(0)
{
    static const QString StdLibFileName =
            qApp->property("sharePath").toString() + "/llvmcodegenerator/stdlib.bc";

    QFile StdLibFile(StdLibFileName);
    if (!StdLibFile.open(QIODevice::ReadOnly)) {
        const QString message = QString::fromAscii("Can't open %1")
                .arg(StdLibFileName);
        qFatal(message.toStdString().c_str());
    }

    QByteArray stdLibBytes = StdLibFile.readAll();
    StdLibFile.close();

    std::string strdata(stdLibBytes.constData(), stdLibBytes.size());
    stdlibContents_.reset(llvm::MemoryBuffer::getMemBufferCopy(strdata, "stdlib.bc"));

}


void LLVMGenerator::reset(bool linkStdLibModule)
{
    linkStdLibModule_ = linkStdLibModule;
    context_ = new llvm::LLVMContext();
    currentModule_ = 0;
    currentFunction_ = 0;    
    nameTranslator_->reset();
    createStdLibModule();
}

void LLVMGenerator::createStdLibModule()
{
    std::string error;
    stdlibModule_ = llvm::ParseBitcodeFile(stdlibContents_.data(),
                                               *context_,
                                               &error);
    if (!error.empty()) {
        qDebug() << QString::fromStdString(error);
    }

}

llvm::Module* LLVMGenerator::createModule(const AST::ModulePtr kmod)
{

    const CString moduleName = nameTranslator_->add(
                !kmod->header.name.isEmpty()
                ? QString("__self__")
                : kmod->header.name
                  );

    nameTranslator_->beginNamespace();

    llvm::Module * lmod = new llvm::Module(moduleName, *context_);
    currentModule_ = lmod;
    createExternsTable();
    std::cerr << lmod->getModuleIdentifier() << std::endl;

    llvm::Function * initFunc = llvm::Function::Create(
                llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), false),
                llvm::GlobalValue::LinkerPrivateLinkage,
                CString("__init__" + moduleName),
                currentModule_
                );

    currentFunction_ = initFunc;
    initFunctions_.push_back(initFunc);
    initFunc->addFnAttr(llvm::Attributes::NoUnwind);
    initFunc->addFnAttr(llvm::Attributes::UWTable);
    llvm::BasicBlock * initBlock = llvm::BasicBlock::Create(*context_, "", initFunc);
    currentBlock_ = initBlock;
    llvm::IRBuilder<> initBuilder(initBlock);

    for (int i=0; i<kmod->impl.globals.size(); i++) {
        const AST::VariablePtr glob = kmod->impl.globals.at(i);
        addGlobalVariable(initBuilder, glob, false);
    }

    addFunctionBody(kmod->impl.initializerBody, AST::AlgorithmPtr());
    initBuilder.CreateRetVoid();

    for (int i=0; i<kmod->impl.algorhitms.size(); i++) {
        const AST::AlgorithmPtr func = kmod->impl.algorhitms.at(i);
        addFunction(func, false);
    }

    for (int i=0; i<kmod->impl.algorhitms.size(); i++) {
        const AST::AlgorithmPtr func = kmod->impl.algorhitms.at(i);
        addFunction(func, true);
    }

    if (kmod->header.type == AST::ModTypeUser ||
            kmod->header.type == AST::ModTypeTeacher)
    {
        createMainFunction(kmod->impl.algorhitms.first());
    }

    nameTranslator_->endNamespace();

    return currentModule_;
}

void LLVMGenerator::addGlobalVariable(llvm::IRBuilder<> & builder, const AST::VariablePtr kvar, bool constant)
{
    Q_ASSERT(context_);
    Q_ASSERT(currentModule_);

    CString name = nameTranslator_->add(kvar->name);
    llvm::Type * ty = findOrRegisterType(kvar->baseType, false, kvar->dimension);

    llvm::GlobalVariable * lvar = new llvm::GlobalVariable(
                *currentModule_,
                ty,
                constant,
                llvm::GlobalValue::LinkerPrivateLinkage,
                llvm::ConstantStruct::getNullValue(ty),
                name
                );

    Q_ASSERT(lvar);

}

void LLVMGenerator::createMainFunction(const AST::AlgorithmPtr &entryPoint)
{
    std::vector<llvm::Type*> mainArgs(2);
    mainArgs[0] = llvm::Type::getInt32Ty(*context_);
    mainArgs[1] = llvm::Type::getInt8PtrTy(*context_)->getPointerTo();
    llvm::FunctionType * lft = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*context_), mainArgs, false);
    llvm::Function * lfn =
            llvm::Function::Create(lft,
                                   llvm::GlobalValue::ExternalLinkage,
                                   "main",
                                   currentModule_);
    lfn->getArgumentList().front().setName("argc");
    lfn->getArgumentList().back().setName("argv");
    llvm::BasicBlock * functionBlock =
            llvm::BasicBlock::Create(*context_, "", lfn);
    llvm::IRBuilder<> builder(functionBlock);

    builder.CreateCall(kumirInitStdLib_);

    const QString entryPointQName =
            entryPoint->header.name.isEmpty()
            ? QString("__main__") : entryPoint->header.name;

    const CString entryPointName = nameTranslator_->find(entryPointQName);
    Q_ASSERT(entryPointName.length() > 0);

    llvm::Function * entryPointFunc = currentModule_->getFunction(entryPointName);
    Q_ASSERT(entryPointFunc);

    typedef std::list<llvm::Function*>::const_iterator It;
    for (It it = initFunctions_.begin(); it != initFunctions_.end(); ++it) {
        llvm::Function * initFunc = *it;
        Q_ASSERT(initFunc);
        builder.CreateCall(initFunc);
    }

    builder.CreateCall(entryPointFunc);
    builder.CreateRet(
                llvm::ConstantInt::getSigned(
                    llvm::Type::getInt32Ty(*context_),
                    0)
                );
}

void LLVMGenerator::addFunction(const AST::AlgorithmPtr kfunc, bool createBody)
{
    Q_ASSERT(context_);
    Q_ASSERT(currentModule_);
    currentAlgorithm_ = kfunc;
    ifThenElseCounter_ = 0u;
    switchCaseCounter_ = 0u;
    loopCounter_ = 0u;
    currentLoopEnd_ = nullptr;
    currentFunctionExit_ = nullptr;

    const QString actualName = kfunc->header.name.isEmpty()
            ? QString("__main__") : kfunc->header.name;

    CString name = createBody
            ? nameTranslator_->find(actualName)
                // already added at previous stage
            : nameTranslator_->add(actualName)
                // create new name
              ;

    size_t largsCount = 0u;
    size_t largsOffset = 0u;

    if (kfunc->header.returnType == AST::TypeNone) {
        largsCount = kfunc->header.arguments.size();
    }
    else {
        largsCount = kfunc->header.arguments.size() + 1;
        largsOffset = 1u;
    }

    std::vector<llvm::Type*> args(largsCount);
    for (size_t i=0; i<kfunc->header.arguments.size(); i++) {
        const AST::VariablePtr kvar = kfunc->header.arguments.at(i);
        llvm::Type * lvar = findOrRegisterType(kvar->baseType,
                                         true,
                                         kvar->dimension);
        args[i+largsOffset] = lvar;
    }

    if (kfunc->header.returnType != AST::TypeNone) {
        llvm::Type * rtype = 0;
        rtype = findOrRegisterType(kfunc->header.returnType,
                                                false,
                                                0u);
        rtype = rtype->getPointerTo();
        args[0] = rtype;
    }

    llvm::FunctionType * lft = llvm::FunctionType::get(
                llvm::Type::getVoidTy(*context_),
                args,
                false
                );

    llvm::Function * lfn = 0;
    if (createBody) {
        lfn = currentModule_->getFunction(name);
    }
    else {
        lfn = llvm::Function::Create(
                lft,
                kfunc->header.name.isEmpty() || kfunc->header.name.startsWith("_")
                ? llvm::GlobalValue::LinkerPrivateLinkage
                : llvm::GlobalValue::ExternalLinkage,
                name,
                currentModule_
                );
        if (kfunc->header.returnType != AST::TypeNone) {
            llvm::Argument & firstArg = lfn->getArgumentList().front();
            std::vector<llvm::Attributes::AttrVal> attrs(2);
            attrs[0] = llvm::Attributes::StructRet;
            attrs[1] = llvm::Attributes::NoAlias;
            firstArg.addAttr(llvm::Attributes::get(*context_, attrs));
        }
        lfn->addFnAttr(llvm::Attributes::NoUnwind);
        lfn->addFnAttr(llvm::Attributes::UWTable);
    }

    if (createBody) {
        nameTranslator_->beginNamespace();
        currentFunction_ = lfn;

        llvm::BasicBlock * functionEntry =
                llvm::BasicBlock::Create(ctx, "function_entry", lfn);
        llvm::BasicBlock * functionPre =
                llvm::BasicBlock::Create(ctx, "function_pre", lfn);
        llvm::BasicBlock * functionBody =
                llvm::BasicBlock::Create(ctx, "function_body", lfn);
        llvm::BasicBlock * functionPost =
                llvm::BasicBlock::Create(ctx, "function_post", lfn);
        llvm::BasicBlock * functionExit =
                llvm::BasicBlock::Create(ctx, "function_exit", lfn);

        currentFunctionExit_ = functionExit;
        llvm::IRBuilder<> builder(currentBlock_);
        currentBlock_ = functionEntry;
        builder.SetInsertPoint(currentBlock_);

        AST::VariablePtr retval;
        std::list<AST::VariablePtr> namedArgs;

        for (int i=0; i<kfunc->impl.locals.size(); i++) {
            const AST::VariablePtr & loc = kfunc->impl.locals[i];
            if (loc->name == kfunc->header.name) {
                retval = loc;
            }
            else if (loc->accessType != AST::AccessRegular){
                namedArgs.push_back(loc);
            }
        }

        llvm::Function::ArgumentListType::iterator lIt = lfn->getArgumentList().begin();
        std::list<AST::VariablePtr>::iterator kIt = namedArgs.begin();

        if (kfunc->header.returnType != AST::TypeNone) {
            lIt ++; // skip first argument used by retval
        }

        for ( ; lIt != lfn->getArgumentList().end() && kIt != namedArgs.end() ; ++lIt, ++kIt)
        {
            llvm::Argument & larg = *lIt;
            const AST::VariablePtr & karg = *kIt;
            const QString kArgName = karg->name;
            const CString lArgName = nameTranslator_->add(kArgName);
            larg.setName(lArgName);
        }

        builder.CreateBr(functionPre);

        currentBlock_ = functionPre;
        addFunctionBody(kfunc->impl.pre, kfunc);
        builder.SetInsertPoint(functionPre);
        builder.CreateBr(functionBody);

        currentBlock_ = functionBody;
        addFunctionBody(kfunc->impl.body, kfunc);
        builder.SetInsertPoint(currentBlock_); // Might be changed in body
        builder.CreateBr(functionPost);

        currentBlock_ = functionPost;
        addFunctionBody(kfunc->impl.post, kfunc);
        builder.SetInsertPoint(functionPost);
        builder.CreateBr(functionExit);

        currentBlock_ = functionExit;
        builder.SetInsertPoint(functionExit);
        builder.CreateRetVoid();
        nameTranslator_->endNamespace();
    }
}

void LLVMGenerator::addFunctionBody(const QList<AST::StatementPtr> & statements, const AST::AlgorithmPtr & alg)
{
    for (int i=0; i<statements.size(); i++) {
        llvm::BasicBlock * block = currentBlock_;
        Q_ASSERT(block);
        llvm::IRBuilder<> builder(block);
        builder.SetInsertPoint(block);
        const AST::StatementPtr & statement = statements.at(i);
        const AST::StatementType type = statement->type;
        switch (type) {
        case AST::StVarInitialize:
            createVarInitialize(builder, statement, alg.isNull());
            break;
        case AST::StAssign:
            createAssign(builder, statement, alg);
            break;
        case AST::StAssert:
            createAssert(builder, statement, alg);
            break;
        case AST::StOutput:
            createOutput(builder, statement, alg);
            break;
        case AST::StInput:
            createInput(builder, statement, alg);
            break;
        case AST::StLoop:
            createLoop(builder, statement, alg);
            break;
        case AST::StIfThenElse:
            createIfThenElse(builder, statement, alg);
            break;
        case AST::StSwitchCaseElse:
            createSwitchCaseElse(builder, statement, alg);
            break;
        case AST::StBreak:
            createBreak(builder, statement, alg);
            break;
        default:
            qFatal("Not implemented!");
            break;
        }
    }
}

void LLVMGenerator::createVarInitialize(llvm::IRBuilder<> &builder, const AST::StatementPtr &st, bool global)
{
    for (int i=0; i<st->variables.size(); i++) {
        const AST::VariablePtr var = st->variables.at(i);
        if (var->accessType == AST::AccessRegular) {
            createVarInitialize(builder, var, "", global);
        }
    }
}

llvm::Value* LLVMGenerator::createVarInitialize(llvm::IRBuilder<> &builder, const AST::VariablePtr & var, const QString & overrideName, bool global)
{
    llvm::Type * ty = findOrRegisterType(var->baseType, false, var->dimension);
    CString name;
    llvm::Value * result = 0;
    if (!global) {
        name = nameTranslator_->add(overrideName.isEmpty() ? var->name : overrideName);
        Q_ASSERT(!name.empty());
        result = builder.CreateAlloca(ty, 0, name);
        Q_ASSERT(result);
    }
    else {
        name = nameTranslator_->find(var->name);
        Q_ASSERT(!name.empty());
        result = currentModule_->getGlobalVariable(name, true);
        Q_ASSERT(result);
    }
    llvm::Function * initFunc = 0;
    llvm::Function * fillFunc = 0;
    std::vector<llvm::Value*> initArgs;
    llvm::Value * fillValue = 0;
    initArgs.push_back(result);
    if (var->dimension == 0u) {
        if (!var->initialValue.isValid()) {
            initFunc = kumirCreateUndefinedScalar_;
        }
        else {
            initFunc = kumirCreateDefinedScalar_;
            llvm::Value * cval = createConstant(builder, var->baseType, var->initialValue);
            Q_ASSERT(cval);
            initArgs.push_back(cval);
        }
    }
    else {
        llvm::Function * kumirInitArray = 0;
        if          (1u == var->dimension) kumirInitArray = kumirCreateArray1_;
        else if     (2u == var->dimension) kumirInitArray = kumirCreateArray2_;
        else if     (3u == var->dimension) kumirInitArray = kumirCreateArray2_;
        Q_ASSERT(kumirInitArray);
        initFunc = kumirInitArray;
        for (quint8 boundIndex=0; boundIndex<var->dimension; boundIndex++) {
            const AST::Variable::Bound & bound = var->bounds[boundIndex];
            const AST::ExpressionPtr & left = bound.first;
            const AST::ExpressionPtr & right = bound.second;
            llvm::Value * lleft = calculate(builder, left);
            llvm::Value * lright = calculate(builder, right);
            initArgs.push_back(lleft);
            initArgs.push_back(lright);
        }
        if (var->initialValue.isValid()) {
            if      (AST::TypeInteger   == var->baseType.kind) fillFunc = kumirFillArrayI_;
            else if (AST::TypeReal      == var->baseType.kind) fillFunc = kumirFillArrayR_;
            else if (AST::TypeBoolean   == var->baseType.kind) fillFunc = kumirFillArrayB_;
            else if (AST::TypeCharect   == var->baseType.kind) fillFunc = kumirFillArrayC_;
            else if (AST::TypeString    == var->baseType.kind) fillFunc = kumirFillArrayS_;
            Q_ASSERT(fillFunc);
            fillValue = createArrayConstant(builder, var->baseType.kind, var->dimension, var->initialValue);
            Q_ASSERT(fillValue);
        }
    }
    Q_ASSERT(initFunc);
    llvm::CallInst * initCall = builder.CreateCall(initFunc, initArgs);
    Q_ASSERT(initCall);
    if (fillFunc) {
        Q_ASSERT(fillValue);
        llvm::CallInst * fillCall = builder.CreateCall2(fillFunc, result, fillValue);
        Q_ASSERT(fillCall);
    }
    return result;
}

void LLVMGenerator::createAssign(llvm::IRBuilder<> &builder, const AST::StatementPtr &st, const AST::AlgorithmPtr & alg)
{
    const AST::ExpressionPtr rvalue = st->expressions.at(0);
    llvm::Value * llvm_rvalue = calculate(builder, rvalue);
    if (st->expressions.size() > 1) {
        const AST::ExpressionPtr lvalue = st->expressions.at(1);        
        llvm::Value * llvm_lvalue = calculate(builder, lvalue, true);
        Q_ASSERT(llvm_lvalue);
        llvm::Function * storeFunc = 0;
        if (lvalue->kind == AST::ExprVariable) {
            storeFunc = kumirAssignScalarToScalar_;
        }
        else if (lvalue->kind == AST::ExprArrayElement) {
            storeFunc = kumirAssignScalarToArrayElement_;
        }
        else {
            qFatal("Not implemented yet");
        }
        Q_ASSERT(storeFunc);
        builder.CreateCall2(storeFunc, llvm_lvalue, llvm_rvalue);
    }
    createFreeTempScalars(builder);
}

void LLVMGenerator::createAssert(llvm::IRBuilder<> &builder, const AST::StatementPtr &st, const AST::AlgorithmPtr &alg)
{
    for (int i=0; i<st->expressions.size(); i++) {
        const AST::ExpressionPtr & expr = st->expressions[i];
        llvm::Value * assertionValue = calculate(builder, expr);
        Q_ASSERT(assertionValue);
        builder.CreateCall(kumirAssert_, assertionValue);
        createFreeTempScalars(builder);
    }
}

void LLVMGenerator::createOutput(llvm::IRBuilder<> &builder, const AST::StatementPtr &st, const AST::AlgorithmPtr &alg)
{
    const int lexemsCount = st->expressions.size() / 3;
    const bool fileHandleProvided = st->expressions.size() % 3 > 0;
    llvm::Value * fileHandle = 0;
    if (fileHandleProvided) {
        fileHandle = calculate(builder, st->expressions.last());
        Q_ASSERT(fileHandle);
    }
    for (int i=0; i<lexemsCount; i++) {
        int exprIndex = 3 * i;
        int format1Index = 3 * i + 1;
        int format2Index = 3 * i + 2;
        const AST::ExpressionPtr & expr = st->expressions.at(exprIndex);
        const AST::ExpressionPtr & format1 = st->expressions.at(format2Index);
        const AST::ExpressionPtr & format2 = st->expressions.at(format1Index);

        llvm::Value * lexpr = calculate(builder, expr);      

        llvm::Function * outFunc = 0;
        std::vector<llvm::Value*> args;

        if (fileHandleProvided) {
            args.push_back(fileHandle);
            args.push_back(lexpr);
            if (format1->kind == AST::ExprConst && format2->kind == AST::ExprConst) {
                llvm::Value * lformat1 = llvm::ConstantInt::getSigned(
                            llvm::Type::getInt32Ty(*context_),
                            format1->constant.toInt()
                            );
                llvm::Value * lformat2 = llvm::ConstantInt::getSigned(
                            llvm::Type::getInt32Ty(*context_),
                            format2->constant.toInt()
                            );
                outFunc = kumirOutputFileII_;
                args.push_back(lformat1);
                args.push_back(lformat2);
            }
        }
        else {
            args.push_back(lexpr);
            if (format1->kind == AST::ExprConst && format2->kind == AST::ExprConst) {
                llvm::Value * lformat1 = llvm::ConstantInt::getSigned(
                            llvm::Type::getInt32Ty(*context_),
                            format1->constant.toInt()
                            );
                llvm::Value * lformat2 = llvm::ConstantInt::getSigned(
                            llvm::Type::getInt32Ty(*context_),
                            format2->constant.toInt()
                            );
                outFunc = kumirOutputStdoutII_;
                args.push_back(lformat1);
                args.push_back(lformat2);
            }
        }
        Q_ASSERT(outFunc);
        builder.CreateCall(outFunc, args);
        createFreeTempScalars(builder);
    }
}

void LLVMGenerator::createInput(llvm::IRBuilder<> &builder, const AST::StatementPtr &st, const AST::AlgorithmPtr &alg)
{
    Q_ASSERT(st->expressions.size() > 0);
    int endIndex = st->expressions.size();
    llvm::Value * fileHandle = 0;
    const AST::ExpressionPtr & lastExpr = st->expressions.last();
    if (lastExpr->baseType.kind == AST::TypeUser &&
            lastExpr->baseType.name == QString::fromUtf8("файл"))
    {
        fileHandle = calculate(builder, lastExpr);
        Q_ASSERT(fileHandle);
        endIndex -= 1;
    }

    for (int i=0; i<endIndex; i++) {
        const AST::ExpressionPtr & varExpr = st->expressions.at(i);
        std::vector<llvm::Value*> args;
        if (fileHandle) {
            args.push_back(fileHandle);
        }
        uint32_t fmt = 0x00;
        if      (AST::TypeInteger   == varExpr->variable->baseType.kind)   fmt = 0x01;
        else if (AST::TypeReal      == varExpr->variable->baseType.kind)   fmt = 0x02;
        else if (AST::TypeBoolean   == varExpr->variable->baseType.kind)   fmt = 0x03;
        else if (AST::TypeCharect   == varExpr->variable->baseType.kind)   fmt = 0x04;
        else if (AST::TypeString    == varExpr->variable->baseType.kind)   fmt = 0x05;
        args.push_back(llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(ctx), fmt));
        llvm::Value * ptr = 0;
        llvm::Type * const ptrTy = findOrRegisterType(varExpr->variable->baseType, false ,0u)->getPointerTo();
        if (varExpr->kind == AST::ExprVariable) {
            llvm::Value * var = findVariableAtCurrentContext(varExpr->variable);
            Q_ASSERT(var);
            ptr = builder.CreateAlloca(ptrTy);
            ptr->setName(var->getName() + CString("_ptr"));
            builder.CreateStore(var, ptr);
        }
        else if (varExpr->kind == AST::ExprArrayElement) {
            ptr = calculate(builder, varExpr, true);
        }
        args.push_back(ptr);
        if (fileHandle) {
            builder.CreateCall(kumirInputFile_, args);
        }
        else {
            builder.CreateCall(kumirInputStdin_, args);
        }
    }
}

void LLVMGenerator::createLoop(llvm::IRBuilder<> &builder, const AST::StatementPtr &st, const AST::AlgorithmPtr &alg)
{
    const AST::LoopSpec & loop = st->loop;


    llvm::Value * for_from = 0;
    llvm::Value * for_to = 0;
    llvm::Value * for_step = 0;
    llvm::Value * while_cond = 0;
    llvm::Value * times = 0;
    llvm::Value * end_cond = 0;
    llvm::Value * loop_variable = 0;

    // --- loop structure
    loopCounter_ ++;
    CString basicName = QString("loop_%1_")
            .arg(loopCounter_).toStdString();

    llvm::BasicBlock * loop_init =
            llvm::BasicBlock::Create(*context_,
                                     basicName + CString("init"),
                                     currentFunction_);

    llvm::BasicBlock * loop_begin =
            llvm::BasicBlock::Create(*context_,
                                     basicName + CString("begin"),
                                     currentFunction_);
    llvm::BasicBlock * loop_body =
            llvm::BasicBlock::Create(*context_,
                                     basicName + CString("body"),
                                     currentFunction_);
    llvm::BasicBlock * loop_end =
            llvm::BasicBlock::Create(*context_,
                                     basicName + CString("end"),
                                     currentFunction_);
    llvm::BasicBlock * loop_clean =
            llvm::BasicBlock::Create(*context_,
                                     basicName + CString("clean"),
                                     currentFunction_);
    llvm::BasicBlock * loop_done =
            llvm::BasicBlock::Create(*context_,
                                     basicName + CString("done"),
                                     currentFunction_);

    currentLoopEnd_ = loop_clean;

    builder.CreateBr(loop_init);

    // --- initialization
    builder.SetInsertPoint(loop_init);
    if (loop.type == AST::LoopFor) {
        const AST::VariablePtr & kvar = loop.forVariable;
        const CString name = nameTranslator_->find(kvar->name);
        Q_ASSERT(!name.empty());
        loop_variable = currentBlock_->getValueSymbolTable()->lookup(name);
        if (!loop_variable) {
            loop_variable = currentModule_->getGlobalVariable(name, true);
        }
        Q_ASSERT(loop_variable);
        for_from = calculate(builder, loop.fromValue);
        Q_ASSERT(for_from);
        if (loop.fromValue->kind != AST::ExprVariable)
            for_from->setName(basicName + CString("from"));
        for_to = calculate(builder, loop.toValue);
        Q_ASSERT(for_to);
        if (loop.toValue->kind != AST::ExprVariable)
            for_to->setName(basicName + CString("to"));
        if (loop.stepValue) {
            for_step = calculate(builder, loop.stepValue);
            Q_ASSERT(for_step);
            if (loop.stepValue->kind != AST::ExprVariable)
                for_step->setName(basicName + CString("step"));
            Q_ASSERT(kumirLoopForFromToStepInitCounter_);
            builder.CreateCall2(kumirLoopForFromToStepInitCounter_,
                                for_from, for_step);
        }
        else {
            Q_ASSERT(kumirLoopForFromToInitCounter_);
            builder.CreateCall(kumirLoopForFromToInitCounter_, for_from);
        }
    }
    else if (loop.type == AST::LoopTimes) {
        times = calculate(builder, loop.timesValue);
        Q_ASSERT(times);
        Q_ASSERT(kumirLoopTimesInitCounter_);
        builder.CreateCall(kumirLoopTimesInitCounter_, times);
    }

    builder.CreateBr(loop_begin);

    // --- loop pre-check
    builder.SetInsertPoint(loop_begin);
    llvm::Value * loop_pre_check = 0;
    if (loop.type == AST::LoopFor) {
        if (loop.stepValue) {
            loop_pre_check = builder.CreateCall4(kumirLoopForFromToStepCheckCounter_,
                                                 loop_variable,
                                                 for_from,
                                                 for_to,
                                                 for_step,
                                                 basicName + CString("for_check")
                                                 );
        }
        else {
            loop_pre_check = builder.CreateCall3(kumirLoopForFromToCheckCounter_,
                                                 loop_variable,
                                                 for_from,
                                                 for_to,
                                                 basicName + CString("for_check")
                                                 );
            Q_ASSERT(loop_pre_check);
        }
    }
    else if (loop.type == AST::LoopTimes) {
        Q_ASSERT(kumirLoopTimesCheckCounter_);
        loop_pre_check = builder.CreateCall(kumirLoopTimesCheckCounter_,
                                            basicName + CString("times_check")
                                            );
    }

    if (loop_pre_check) {
        builder.CreateCondBr(loop_pre_check, loop_body, loop_clean);
    }
    else {
        builder.CreateBr(loop_body);
    }

    // --- loop body
    currentBlock_ = loop_body;
    builder.SetInsertPoint(loop_body);
    addFunctionBody(loop.body, alg);
    builder.SetInsertPoint(currentBlock_); // might be changed via inner block
    if (currentBlock_->size() && currentBlock_->back().isTerminator()) {
        // do not add 'done' break
    }
    else {
        builder.CreateBr(loop_end);
    }

    // --- check for end condition
    builder.SetInsertPoint(loop_end);
    if (loop.endCondition) {
        llvm::Value * endCond = calculate(builder, loop.endCondition);
        if (loop.endCondition->kind != AST::ExprVariable)
            endCond->setName(basicName + CString("end_condition"));
        Q_ASSERT(endCond);
        Q_ASSERT(kumirScalarAsBool_);
        llvm::Value * endBoolCond = builder.CreateCall(kumirScalarAsBool_,
                                                       endCond,
                                                       basicName + CString("end_condition_boolean"));
        Q_ASSERT(endBoolCond);
        builder.CreateCondBr(endBoolCond,
                             loop_clean, // True
                             loop_begin // False
                             );
    }
    else {
        builder.CreateBr(loop_begin);
    }

    // --- clean counters if need
    builder.SetInsertPoint(loop_clean);
    if (loop.type == AST::LoopFor || loop.type == AST::LoopTimes) {
        builder.CreateCall(kumirLoopEndCounter_);
    }
    builder.CreateBr(loop_done);

    // --- loop done
    currentBlock_ = loop_done;
    builder.SetInsertPoint(loop_done);
    currentLoopEnd_ = nullptr;
}

void LLVMGenerator::createIfThenElse(llvm::IRBuilder<> &builder, const AST::StatementPtr &st, const AST::AlgorithmPtr &alg)
{
    Q_ASSERT(st->conditionals.size() > 0);
    const AST::ConditionSpec thenSpec = st->conditionals.first();
    // --- create program blocks
    ifThenElseCounter_ ++;
    CString basicName = QString("if_%1_")
            .arg(ifThenElseCounter_).toStdString();

    llvm::BasicBlock * then = llvm::BasicBlock::Create(
                ctx,
                basicName + CString("then"),
                currentFunction_
                );
    llvm::BasicBlock * elze = llvm::BasicBlock::Create(
                ctx,
                basicName + CString("else"),
                currentFunction_
                );
    llvm::BasicBlock * done = llvm::BasicBlock::Create(
                ctx,
                basicName + CString("done"),
                currentFunction_
                );

    // --- calculate condition
    Q_ASSERT(thenSpec.condition);
    llvm::Value * cond = calculate(builder, thenSpec.condition);
    Q_ASSERT(cond);
    if (thenSpec.condition->kind != AST::ExprVariable)
        cond->setName(basicName + CString("condition"));
    Q_ASSERT(kumirScalarAsBool_);
    llvm::Value * condBool = builder.CreateCall(kumirScalarAsBool_, cond,
                                                basicName + CString("condition_boolean"));
    Q_ASSERT(condBool);
    builder.CreateCondBr(condBool, then, elze);

    // --- create then body
    currentBlock_ = then;
    builder.SetInsertPoint(then);
    addFunctionBody(thenSpec.body, alg);
    builder.SetInsertPoint(currentBlock_); // might be changed via inner block
    if (currentBlock_->size() && currentBlock_->back().isTerminator()) {
        // do not add 'done' break
    }
    else {
        builder.CreateBr(done);
    }

    // --- create else body
    currentBlock_ = elze;
    builder.SetInsertPoint(elze);
    if (st->conditionals.size() > 1) {
        const AST::ConditionSpec elseSpec = st->conditionals.last();
        addFunctionBody(elseSpec.body, alg);
        builder.SetInsertPoint(currentBlock_); // might be changed via inner block
    }
    if (currentBlock_->size() && currentBlock_->back().isTerminator()) {
        // do not add 'done' break
    }
    else {
        builder.CreateBr(done);
    }

    // --- done
    currentBlock_ = done;
    builder.SetInsertPoint(done);
}

void LLVMGenerator::createSwitchCaseElse(llvm::IRBuilder<> &builder, const AST::StatementPtr &st, const AST::AlgorithmPtr &alg)
{
    Q_ASSERT(st->conditionals.size() > 0);

    // --- create program blocks
    switchCaseCounter_ ++;
    CString basicName = QString("switch_%1_")
            .arg(switchCaseCounter_).toStdString();

    QList<llvm::BasicBlock*> cases;
    QList<llvm::BasicBlock*> caseBodies;
    for (int i=0; i<st->conditionals.size(); i++) {
        const AST::ConditionSpec spec = st->conditionals[i];
        CString caseName = basicName;
        caseName += spec.condition
                ? QString("case_%1").arg(i+1).toStdString() : CString("else");
        llvm::BasicBlock * caze = llvm::BasicBlock::Create(
                    ctx, caseName, currentFunction_);
        cases.push_back(caze);
        llvm::BasicBlock * body = llvm::BasicBlock::Create(
                    ctx, caseName + CString("_body"), currentFunction_
                    );
        caseBodies.push_back(body);
    }

    llvm::BasicBlock * done = llvm::BasicBlock::Create(
                ctx,
                basicName + CString("done"),
                currentFunction_
                );

    builder.CreateBr(cases.first()); // Jump to first case

    for (int i=0; i<st->conditionals.size(); i++) {
        const AST::ConditionSpec & spec = st->conditionals[i];
        llvm::BasicBlock * caze = cases[i];
        llvm::BasicBlock * body = caseBodies[i];
        llvm::BasicBlock * nextBlock = i == st->conditionals.size() - 1
                ? done : cases[i+1];

        // --- calculate condition
        builder.SetInsertPoint(caze);
        if (spec.condition) {
            llvm::Value * cond = calculate(builder, spec.condition);
            Q_ASSERT(cond);
            if (spec.condition->kind != AST::ExprVariable)
                cond->setName(caze->getName() + "_condition");
            Q_ASSERT(kumirScalarAsBool_);
            llvm::Value * condBool = builder.CreateCall(kumirScalarAsBool_, cond,
                                                        cond->getName() + CString("_boolean"));
            Q_ASSERT(condBool);
            builder.CreateCondBr(condBool, body, nextBlock);
        }
        else {
            builder.CreateBr(body);
        }

        // --- create body
        currentBlock_ = body;
        builder.SetInsertPoint(body);
        addFunctionBody(spec.body, alg);
        builder.SetInsertPoint(currentBlock_); // might be changed via inner block
        if (currentBlock_->size() && currentBlock_->back().isTerminator()) {
            // do not add 'done' break
        }
        else {
            builder.CreateBr(done);
        }
    }

    // --- done
    currentBlock_ = done;
    builder.SetInsertPoint(done);
}

void LLVMGenerator::createBreak(llvm::IRBuilder<> &builder, const AST::StatementPtr &st, const AST::AlgorithmPtr &alg)
{
    if (currentLoopEnd_) {
        builder.CreateBr(currentLoopEnd_); // break ends loop
    }
    else {
        builder.CreateBr(currentFunctionExit_); // break ends function
    }
}

void LLVMGenerator::createFreeTempScalars(llvm::IRBuilder<> &builder)
{
    typedef std::list<llvm::Value*>::iterator It;
    for (It it = tempValsToFree_.begin(); it != tempValsToFree_.end(); ++it) {
        builder.CreateCall(kumirFreeScalar_, *it);
    }
    tempValsToFree_.clear();
}

llvm::Value * LLVMGenerator::findVariableAtCurrentContext(const AST::VariablePtr &kvar)
{
    llvm::Value * var = 0;
    bool isRetval = false;
    if (currentAlgorithm_ && currentAlgorithm_->header.returnType != AST::TypeNone) {
        const QString & currentAlgName = currentAlgorithm_->header.name;
        isRetval = kvar->name == currentAlgName;
    }
    if (isRetval) {
        llvm::Argument &firstArg = currentFunction_->getArgumentList().front();
        var = &firstArg;
    }
    else {
        CString varName = nameTranslator_->find(kvar->name);
        Q_ASSERT(varName.length() > 0);
        var = currentBlock_->getValueSymbolTable()->lookup(varName);
        if (!var) {
            var = currentModule_->getGlobalVariable(varName, true);
        }
        if (!var) {
            currentFunction_->dump();
        }
    }
    Q_ASSERT(var);
    return var;
}

llvm::Value * LLVMGenerator::calculate(llvm::IRBuilder<> &builder, const AST::ExpressionPtr &ex, bool isLvalue)
{
    llvm::Value * result = 0;
    if (ex->kind == AST::ExprConst && ex->dimension==0u && ex->baseType.kind != AST::TypeUser) {
        result = createConstant(builder, ex->baseType, ex->constant);
    }
    else if (ex->kind == AST::ExprVariable) {
        return findVariableAtCurrentContext(ex->variable);
    }
    else if (ex->kind == AST::ExprArrayElement) {
        result = createArrayElementGet(builder, ex, isLvalue);
    }
    else if (ex->kind == AST::ExprFunctionCall) {
        result = createFunctionCall(builder, ex->function, ex->operands);
    }
    else if (ex->kind == AST::ExprSubexpression) {
        result = createSubExpession(builder, ex);
    }
//    Q_ASSERT(result);
    if (ex->baseType == AST::TypeString && ex->dimension == 0u) {
        tempValsToFree_.push_back(result);
    }
    return result;
}

llvm::Value * LLVMGenerator::createArrayElementGet(llvm::IRBuilder<> &builder, const AST::ExpressionPtr &ex, bool isLvalue)
{
    CString varName = nameTranslator_->find(ex->variable->name);
    Q_ASSERT(varName.length() > 0);
    llvm::Value * var = currentBlock_->getValueSymbolTable()->lookup(varName);
    if (!var) {
        var = currentModule_->getGlobalVariable(varName, true);
    }
    Q_ASSERT(var);
    std::vector<llvm::Value*> args;
    llvm::Value * elemPtr = builder.CreateAlloca(
                findOrRegisterType(ex->variable->baseType, false, 0u)->getPointerTo()
                );
    Q_ASSERT(elemPtr);
    args.push_back(elemPtr);
    if (!isLvalue) {
        args.push_back(llvm::ConstantInt::getTrue(ctx));
    }
    else {
        args.push_back(llvm::ConstantInt::getFalse(ctx));
    }
    args.push_back(var);
    for (int i=0; i<ex->operands.size(); i++) {
        const AST::ExpressionPtr& indexOperand = ex->operands[i];
        llvm::Value * index = calculate(builder, indexOperand);
        Q_ASSERT(index);
        args.push_back(index);
    }
    llvm::Function * getPtrFunc = 0;
    if      (1u == ex->variable->dimension) getPtrFunc = kumirGetArray1Element_;
    else if (2u == ex->variable->dimension) getPtrFunc = kumirGetArray2Element_;
    else if (3u == ex->variable->dimension) getPtrFunc = kumirGetArray3Element_;
    Q_ASSERT(getPtrFunc);
    builder.CreateCall(getPtrFunc, args);
    if (isLvalue) {
        return elemPtr;
    }
    else {
        llvm::Value * result = builder.CreateLoad(elemPtr);
        return result;
    }
}

llvm::Value * LLVMGenerator::createConstant(llvm::IRBuilder<> & builder, const AST::Type kty, const QVariant &value)
{
    llvm::Value * result = 0;
    llvm::Type * ty = findOrRegisterType(kty, false, 0);
    llvm::Value * arg = 0;
    llvm::Function * func = 0;
    Q_ASSERT(ty);
    if (kty.kind == AST::TypeInteger) {
        int64_t val = value.toInt();
        arg = llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*context_), val);
        func = kumirCreateInt_;
    }
    else if (kty.kind == AST::TypeBoolean) {
        bool val = value.toBool();
        arg = val ? llvm::ConstantInt::getTrue(*context_)
                  : llvm::ConstantInt::getFalse(*context_);
        func = kumirCreateBool_;
    }
    else if (kty.kind == AST::TypeCharect) {
        const std::string strval = std::string(value.toString().toUtf8().data());
        arg = builder.CreateGlobalStringPtr(strval);
        func = kumirCreateChar_;
    }
    else if (kty.kind == AST::TypeReal) {
        double val = value.toDouble();
        arg = llvm::ConstantFP::get(*context_, llvm::APFloat(val));
        func = kumirCreateReal_;
    }
    else if (kty.kind == AST::TypeString) {
        const CString strval = std::string(value.toString().toUtf8().data());
        arg = builder.CreateGlobalStringPtr(strval);
        func = kumirCreateString_;
    }
    llvm::Value * tmp = builder.CreateAlloca(ty);
    result = builder.CreateCall2(func, tmp, arg);
    Q_ASSERT(result);
    return tmp;
}

llvm::Value * LLVMGenerator::createArrayConstant(llvm::IRBuilder<> &builder, const AST::VariableBaseType bt, const uint8_t dim, const QVariant &value)
{
    QByteArray data;
    if      (1u == dim) data = createArray_1_ConstantData(bt, value.toList());
    else if (2u == dim) data = createArray_2_ConstantData(bt, value.toList());
    else if (3u == dim) data = createArray_3_ConstantData(bt, value.toList());

    std::string cdata(data.constData(), data.size());
    llvm::Value * ldata = builder.CreateGlobalStringPtr(cdata);
    return ldata;
}

QByteArray LLVMGenerator::createArray_0_ConstantData(const AST::VariableBaseType bt, const QVariant &value, bool addDefFlag)
{
    QByteArray result;
    if (addDefFlag) {
        bool def = value.isValid();
        char * cdef = reinterpret_cast<char*>(&def);
        result.push_back(cdef[0]);
    }
    char * ptr = 0;
    size_t sz = 0;
    int32_t ival; double rval; bool bval; QByteArray sval;
    if (AST::TypeInteger == bt) {
        ival = value.isValid() ? value.toInt() : 0;
        ptr = reinterpret_cast<char*>(&ival);
        sz = sizeof(int32_t);
    }
    else if (AST::TypeReal == bt) {
        rval = value.isValid() ? value.toDouble() : 0.0;
        ptr = reinterpret_cast<char*>(&rval);
        sz = sizeof(double);
    }
    else if (AST::TypeBoolean == bt) {
        bval = value.isValid() ? value.toBool() : false;
        ptr = reinterpret_cast<char*>(&bval);
        sz = sizeof(bool);
    }
    else {
        const QString s = value.isValid() ? value.toString() : QString();
        sval = s.toUtf8();
    }
    QByteArray vbytes;
    if (sz) {
        vbytes = QByteArray(ptr, sz);
    }
    else {
        vbytes = sval;
    }
    result += vbytes;
    return result;
}

QByteArray LLVMGenerator::createArray_1_ConstantData(const AST::VariableBaseType bt, const QVariantList &list)
{
    QByteArray result;
    result += createArray_0_ConstantData(AST::TypeInteger, list.size(), false);
    for (int i=0; i<list.size(); i++) {
        result += createArray_0_ConstantData(bt, list[i], true);
    }
    return result;
}

QByteArray LLVMGenerator::createArray_2_ConstantData(const AST::VariableBaseType bt, const QVariantList &list)
{
    QByteArray result;
    result += createArray_0_ConstantData(AST::TypeInteger, list.size(), false);
    for (int i=0; i<list.size(); i++) {
        result += createArray_1_ConstantData(bt, list[i].toList());
    }
    return result;
}

QByteArray LLVMGenerator::createArray_3_ConstantData(const AST::VariableBaseType bt, const QVariantList &list)
{
    QByteArray result;
    result += createArray_0_ConstantData(AST::TypeInteger, list.size(), false);
    for (int i=0; i<list.size(); i++) {
        result += createArray_2_ConstantData(bt, list[i].toList());
    }
    return result;
}

llvm::Value * LLVMGenerator::createFunctionCall(llvm::IRBuilder<> &builder, const AST::AlgorithmPtr &alg, const QList<AST::ExpressionPtr> & arguments)
{
    llvm::Function * func = 0;
    CString funcName;

    if (alg->header.implType == AST::AlgorhitmCompiled) {
        funcName = nameTranslator_->find(alg->header.name);
        Q_ASSERT(funcName.length() > 0);
        func = currentModule_->getFunction(funcName);
        Q_ASSERT(func);
    }
    else {
        std::vector<llvm::Type*> formalArgs(alg->header.arguments.size());
        funcName = alg->header.cHeader.toStdString();
        if (funcName.empty()) {
            funcName = "__kumir__stdlib__" ; // TODO implement non-std algorithms
            funcName += NameTranslator::suggestName(alg->header.name).toStdString();
        }
        QStringList params;
        for (int i=0; i<alg->header.arguments.size(); i++) {
            const AST::VariablePtr & arg = alg->header.arguments[i];
            QString param = arg->dimension == 0u? "__kumir_scalar" : "__kumir_array";
            bool ref = false;
            if (arg->accessType == AST::AccessArgumentInOut || arg->accessType == AST::AccessArgumentOut) {
                param += "&";
                ref = true;
            }
            params.push_back(param);
            formalArgs[i] = findOrRegisterType(arg->baseType, ref, arg->dimension);
        }
        llvm::Type * rtype = findOrRegisterType(alg->header.returnType, false, 0u);
        llvm::FunctionType * ftype = llvm::FunctionType::get(rtype, formalArgs, false);
        func = currentModule_->getFunction(funcName); // maybe already used
        if (!func) {
            func = llvm::Function::Create(ftype,
                                          llvm::Function::ExternalLinkage,
                                          funcName,
                                          currentModule_);
        }
    }

    size_t largsCount = alg->header.arguments.size();
    size_t loffset = 0u;

    if (alg->header.returnType != AST::TypeNone) {
        largsCount += 1u;
        loffset = 1u;
    }

    std::vector<llvm::Value*> args(largsCount);

    for (size_t i=0; i<alg->header.arguments.size(); i++) {
        const AST::ExpressionPtr & argExpr = arguments.at(i);
        llvm::Value * arg = calculate(builder, argExpr);
        args[loffset+i] = arg;
    }

    llvm::Value * result = 0;

    if (alg->header.returnType != AST::TypeNone) {
        llvm::Type * rtype = findOrRegisterType(alg->header.returnType, false, 0u);
        llvm::Value * rval = builder.CreateAlloca(rtype);
        args[0] = rval;
        result = rval;
    }
    builder.CreateCall(func, args);
    return result;
}

llvm::Value * LLVMGenerator::createSubExpession(llvm::IRBuilder<> &builder, const AST::ExpressionPtr &ex)
{
    if (ex->operatorr == AST::OpAnd || ex->operatorr == AST::OpOr) {
        return createShortCircuitOperation(builder, ex->operands.first(), ex->operands.last(), ex->operatorr);
    }
    std::vector<llvm::Value*> operands(ex->operands.size());

    for (int i=0; i<ex->operands.size(); i++) {
        llvm::Value * operand = calculate(builder, ex->operands[i]);
        operands[i] = operand;
    }

    llvm::Value * result = builder.CreateAlloca(findOrRegisterType(ex->baseType, false, 0u));
    llvm::Function * opFunc = 0;
    size_t operandsCount = operands.size();
    switch (ex->operatorr) {
    case AST::OpEqual:          opFunc = kumirOpEq_;    break;
    case AST::OpNotEqual:       opFunc = kumirOpNeq_;   break;
    case AST::OpLess:           opFunc = kumirOpLs_;    break;
    case AST::OpGreater:        opFunc = kumirOpGt_;    break;
    case AST::OpLessOrEqual:    opFunc = kumirOpLq_;    break;
    case AST::OpGreaterOrEqual: opFunc = kumirOpGq_;    break;
    case AST::OpSumm:           opFunc = kumirOpAdd_;   break;
    case AST::OpSubstract:      opFunc = kumirOpSub_;   break;
    case AST::OpMultiply:       opFunc = kumirOpMul_;   break;
    case AST::OpDivision:       opFunc = kumirOpDiv_;   break;
    case AST::OpPower:          opFunc = kumirOpPow_;   break;
    case AST::OpNot:            opFunc = kumirOpNeg_;   break;
    case AST::OpAnd:            opFunc = kumirOpAnd_;   break;
    case AST::OpOr:             opFunc = kumirOpOr_;    break;
    case AST::OpNone:                                   break;
    }
    if (1u == operandsCount) {
        builder.CreateCall2(opFunc, result, operands[0]);
    }
    else if (2u == operandsCount) {
        builder.CreateCall3(opFunc, result, operands[0], operands[1]);
    }
    return result;
}

llvm::Value * LLVMGenerator::createShortCircuitOperation(llvm::IRBuilder<> &builder, const AST::ExpressionPtr &left, const AST::ExpressionPtr &right, const AST::ExpressionOperator op)
{
    Q_ASSERT(op == AST::OpAnd || op == AST::OpOr);
    llvm::Value * result = builder.CreateAlloca(findOrRegisterType(left->baseType, false, 0u));
    llvm::Value * leftResult = calculate(builder, left);
    Q_ASSERT(kumirScalarAsBool_);
    Q_ASSERT(kumirMoveScalar_);
    builder.CreateCall2(kumirMoveScalar_, result, leftResult);
    llvm::Value * leftBool = builder.CreateCall(kumirScalarAsBool_,
                                                leftResult);
    const CString scName = op == AST::OpAnd ? CString("sc_and") : CString("sc_or");
    llvm::BasicBlock * jumpOnTrue = 0;
    llvm::BasicBlock * jumpOnFalse = 0;
    llvm::BasicBlock * checkNext = 0;
    llvm::BasicBlock * done = 0;

    if (op == AST::OpAnd) {
        checkNext = jumpOnTrue = llvm::BasicBlock::Create(ctx,
                                              scName + "_ok_then_check_next",
                                              currentFunction_);
        done = jumpOnFalse = llvm::BasicBlock::Create(ctx,
                                               scName + "_first_is_false",
                                               currentFunction_);
    }
    else {
        done = jumpOnTrue = llvm::BasicBlock::Create(ctx,
                                              scName + "_first_is_true",
                                              currentFunction_);
        checkNext = jumpOnFalse = llvm::BasicBlock::Create(ctx,
                                               scName + "_false_but_check_next",
                                               currentFunction_);
    }
    builder.CreateCondBr(leftBool, jumpOnTrue, jumpOnFalse);

    // --- check right operand
    builder.SetInsertPoint(checkNext);
    currentBlock_ = checkNext;

    llvm::Value * rightResult = calculate(builder, right);
    builder.CreateCall2(kumirMoveScalar_, result, rightResult);
    builder.CreateBr(done);

    // --- done
    currentBlock_ = done;
    builder.SetInsertPoint(done);
    return result;
}

llvm::Type * LLVMGenerator::findOrRegisterType(const AST::Type &bt, const bool reference, const uint8_t dim)
{
    if (bt.kind == AST::TypeNone) {
        return llvm::Type::getVoidTy(*context_);
    }
    else {
        return findStandardType(bt.kind, reference, dim);
    }
    return 0;
}

llvm::Type * LLVMGenerator::findStandardType(const AST::VariableBaseType bt, const bool reference, const uint8_t dim)
{
    llvm::Type * result = 0;
    const CString cTypeName = dim == 0
            ? std::string("struct.__kumir_scalar") : std::string("struct.__kumir_array");

    result = stdlibModule_->getTypeByName(llvm::StringRef(cTypeName));
    if (reference)
        result = result->getPointerTo();

    return result;
}

void LLVMGenerator::createExternsTable()
{
    // Type handling functions
    createInternalExternsTable();
}

void LLVMGenerator::createInternalExternsTable()
{
    if (linkStdLibModule_) {
        llvm::Linker::LinkModules(currentModule_, stdlibModule_, 0, 0);
    }

    kumirInitStdLib_ = stdlibModule_->getFunction("__kumir_init_stdlib");
    Q_ASSERT(kumirInitStdLib_);

    kumirCreateUndefinedScalar_ = stdlibModule_->getFunction("__kumir_create_undefined_scalar");
    Q_ASSERT(kumirCreateUndefinedScalar_);

    kumirCreateDefinedScalar_ = stdlibModule_->getFunction("__kumir_create_defined_scalar");
    Q_ASSERT(kumirCreateDefinedScalar_);

    kumirCreateArray1_ = stdlibModule_->getFunction("__kumir_create_array_1");
    Q_ASSERT(kumirCreateArray1_);

    kumirCreateArray2_ = stdlibModule_->getFunction("__kumir_create_array_2");
    Q_ASSERT(kumirCreateArray2_);

    kumirCreateArray3_ = stdlibModule_->getFunction("__kumir_create_array_3");
    Q_ASSERT(kumirCreateArray3_);

    kumirFillArrayI_ = stdlibModule_->getFunction("__kumir_fill_array_i");
    Q_ASSERT(kumirFillArrayI_);

    kumirFillArrayR_ = stdlibModule_->getFunction("__kumir_fill_array_r");
    Q_ASSERT(kumirFillArrayR_);

    kumirFillArrayB_ = stdlibModule_->getFunction("__kumir_fill_array_b");
    Q_ASSERT(kumirFillArrayB_);

    kumirFillArrayC_ = stdlibModule_->getFunction("__kumir_fill_array_c");
    Q_ASSERT(kumirFillArrayC_);

    kumirFillArrayS_ = stdlibModule_->getFunction("__kumir_fill_array_s");
    Q_ASSERT(kumirFillArrayS_);

    kumirCreateInt_ = stdlibModule_->getFunction("__kumir_create_int");
    Q_ASSERT(kumirCreateInt_);

    kumirCreateReal_ = stdlibModule_->getFunction("__kumir_create_real");
    Q_ASSERT(kumirCreateReal_);

    kumirCreateBool_ = stdlibModule_->getFunction("__kumir_create_bool");
    Q_ASSERT(kumirCreateBool_);

    kumirCreateChar_ = stdlibModule_->getFunction("__kumir_create_char");
    Q_ASSERT(kumirCreateChar_);

    kumirCreateString_ = stdlibModule_->getFunction("__kumir_create_string");
    Q_ASSERT(kumirCreateString_);

    kumirAssignScalarToScalar_ = stdlibModule_->getFunction("__kumir_copy_scalar");
    Q_ASSERT(kumirAssignScalarToScalar_);

    kumirAssignScalarToArrayElement_ = stdlibModule_->getFunction("__kumir_store_scalar");
    Q_ASSERT(kumirAssignScalarToArrayElement_);

    kumirMoveScalar_ = stdlibModule_->getFunction("__kumir_move_scalar");
    Q_ASSERT(kumirMoveScalar_);

    kumirFreeScalar_ = stdlibModule_->getFunction("__kumir_free_scalar");
    Q_ASSERT(kumirFreeScalar_);

    kumirOutputStdoutII_ = stdlibModule_->getFunction("__kumir_output_stdout_ii");
    Q_ASSERT(kumirOutputStdoutII_);

    kumirOutputStdoutIS_ = stdlibModule_->getFunction("__kumir_output_stdout_is");
    Q_ASSERT(kumirOutputStdoutIS_);

    kumirOutputStdoutSI_ = stdlibModule_->getFunction("__kumir_output_stdout_si");
    Q_ASSERT(kumirOutputStdoutSI_);

    kumirOutputStdoutSS_ = stdlibModule_->getFunction("__kumir_output_stdout_ss");
    Q_ASSERT(kumirOutputStdoutSS_);

    kumirOutputFileII_ = stdlibModule_->getFunction("__kumir_output_file_ii");
    Q_ASSERT(kumirOutputFileII_);

    kumirOutputFileIS_ = stdlibModule_->getFunction("__kumir_output_file_is");
    Q_ASSERT(kumirOutputFileIS_);

    kumirOutputFileSI_ = stdlibModule_->getFunction("__kumir_output_file_si");
    Q_ASSERT(kumirOutputFileSI_);

    kumirOutputFileSS_ = stdlibModule_->getFunction("__kumir_output_file_ss");
    Q_ASSERT(kumirOutputFileSS_);

    kumirInputFile_ = stdlibModule_->getFunction("__kumir_input_file");
    Q_ASSERT(kumirInputFile_);

    kumirInputStdin_ = stdlibModule_->getFunction("__kumir_input_stdin");
    Q_ASSERT(kumirInputStdin_);

    kumirAssert_ = stdlibModule_->getFunction("__kumir_assert");
    Q_ASSERT(kumirAssert_);

    kumirOpEq_ = stdlibModule_->getFunction("__kumir_operator_eq");
    Q_ASSERT(kumirOpEq_);

    kumirOpNeq_ = stdlibModule_->getFunction("__kumir_operator_neq");
    Q_ASSERT(kumirOpNeq_);

    kumirOpLs_ = stdlibModule_->getFunction("__kumir_operator_ls");
    Q_ASSERT(kumirOpLs_);

    kumirOpGt_ = stdlibModule_->getFunction("__kumir_operator_gt");
    Q_ASSERT(kumirOpGt_);

    kumirOpLq_ = stdlibModule_->getFunction("__kumir_operator_lq");
    Q_ASSERT(kumirOpLq_);

    kumirOpGq_ = stdlibModule_->getFunction("__kumir_operator_gq");
    Q_ASSERT(kumirOpGq_);

    kumirOpAdd_ = stdlibModule_->getFunction("__kumir_operator_sum");
    Q_ASSERT(kumirOpAdd_);

    kumirOpSub_ = stdlibModule_->getFunction("__kumir_operator_sub");
    Q_ASSERT(kumirOpSub_);

    kumirOpMul_ = stdlibModule_->getFunction("__kumir_operator_mul");
    Q_ASSERT(kumirOpMul_);

    kumirOpDiv_ = stdlibModule_->getFunction("__kumir_operator_div");
    Q_ASSERT(kumirOpDiv_);

    kumirOpPow_ = stdlibModule_->getFunction("__kumir_operator_pow");
    Q_ASSERT(kumirOpPow_);

    kumirOpNeg_ = stdlibModule_->getFunction("__kumir_operator_neg");
    Q_ASSERT(kumirOpNeg_);

    kumirOpAnd_ = stdlibModule_->getFunction("__kumir_operator_and");
    Q_ASSERT(kumirOpAnd_);

    kumirOpOr_ = stdlibModule_->getFunction("__kumir_operator_or");
    Q_ASSERT(kumirOpOr_);

    kumirGetArray1Element_ = stdlibModule_->getFunction("__kumir_get_array_1_element");
    kumirGetArray2Element_ = stdlibModule_->getFunction("__kumir_get_array_2_element");
    kumirGetArray3Element_ = stdlibModule_->getFunction("__kumir_get_array_3_element");
    Q_ASSERT(kumirGetArray1Element_);
    Q_ASSERT(kumirGetArray2Element_);
    Q_ASSERT(kumirGetArray3Element_);

    kumirLoopForFromToInitCounter_ = stdlibModule_->getFunction("__kumir_loop_for_from_to_init_counter");
    kumirLoopForFromToStepInitCounter_ = stdlibModule_->getFunction("__kumir_loop_for_from_to_step_init_counter");
    kumirLoopForFromToCheckCounter_ = stdlibModule_->getFunction("__kumir_loop_for_from_to_check_counter");
    kumirLoopForFromToStepCheckCounter_ = stdlibModule_->getFunction("__kumir_loop_for_from_to_step_check_counter");
    kumirLoopTimesInitCounter_ = stdlibModule_->getFunction("__kumir_loop_times_init_counter");
    kumirLoopTimesCheckCounter_ = stdlibModule_->getFunction("__kumir_loop_times_check_counter");
    kumirLoopEndCounter_ = stdlibModule_->getFunction("__kumir_loop_end_counter");
    Q_ASSERT(kumirLoopForFromToInitCounter_);
    Q_ASSERT(kumirLoopForFromToStepInitCounter_);
    Q_ASSERT(kumirLoopForFromToCheckCounter_);
    Q_ASSERT(kumirLoopForFromToStepCheckCounter_);
    Q_ASSERT(kumirLoopTimesInitCounter_);
    Q_ASSERT(kumirLoopTimesCheckCounter_);
    Q_ASSERT(kumirLoopEndCounter_);

    kumirScalarAsBool_ = stdlibModule_->getFunction("__kumir_scalar_as_bool");
    Q_ASSERT(kumirScalarAsBool_);

}

CString LLVMGenerator::buildCXXName(const QString &ns,
                                    const QString &clazz,
                                    const QString &name,
                                    const QStringList &params
                                    )
{
    QString result = "_Z";
    if (ns=="std" && clazz=="wstirng") {
        // special case
        result += "NSbIwSt11char_traitsIwESaIwEE";
    }
    else {
        if (ns.length() > 0)  {
            result += QString("N%1%2").arg(ns.length()).arg(ns);
            if (clazz.length() > 0) {
                result += QString("%1%2").arg(clazz.length()).arg(clazz);
            }
        }
        else if (clazz.length() > 0) {
            result += QString("N%1%2").arg(clazz.length()).arg(clazz);
        }
    }
    if (name.length() > 0) {
        result += QString("%1%2").arg(name.length()).arg(name);
    }
    else if (name.length() == 0) {
        // constructor
        result += "C1";
    }
    else if (name == "~") {
        // destructor
        result += "D1";
    }
    if (clazz.length() > 0) {
        result += "E";
    }
    if (params.size() == 0 || (params.size()==1 && clazz.length()>0)) {
        result += "v";
    }
    else {
        foreach (QString param, params) {
            param = param.trimmed();
            bool isConst = param.startsWith("const");
            if (isConst) {
                param = param.mid(5).trimmed();
            }
            bool isPointer = param.endsWith("*");
            bool isReference = param.endsWith("&");
            if (isPointer || isReference) {
                param = param.left(param.length()-1).trimmed();
            }
            if (isReference) {
                result += "R";
            }
            if (isPointer) {
                result += "P";
            }
            if (isConst) {
                result += "K";
            }
            if ("int" == param) {
                result += "i";
            }
            else if ("double" == param) {
                result += "d";
            }
            else if ("bool" == param) {
                result += "b";
            }
            else if ("wchar_t" == param) {
                result += "w";
            }
            else if ("char" == param) {
                result += "c";
            }
            else if ("wstring" == param || "std::wstring" == param) {
                result += "NSbIwSt11char_traitsIwESaIwEE";
            }
            else {
                // class variable
                if (param.contains(':')) {
                    QStringList pair = param.split("::");
                    result += QString("EN%1%2%3%4E")
                            .arg(pair[0].length())
                            .arg(pair[0])
                            .arg(pair[1].length())
                            .arg(pair[1]);
                }
                else {
                    result += QString("E%1%2").arg(param.length()).arg(param);
                }
            }
        }
    }
    return result.toStdString();
}

}
