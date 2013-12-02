#include "llvmgenerator.h"
#include "nametranslator.h"
#include "kumtypes.h"

#include "dataformats/ast_variable.h"
#include "dataformats/ast_algorhitm.h"
#include "dataformats/ast_statement.h"
#include "dataformats/ast_expression.h"
#include "errormessages/errormessages.h"
#include "dataformats/lexem.h"

#include <llvm/Config/llvm-config.h>
#if LLVM_VERSION_MINOR >= 3
#include <llvm/IR/Type.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Attributes.h>
#elif LLVM_VERSION_MINOR == 0
#include <llvm/Support/TypeBuilder.h>
#include <llvm/GlobalVariable.h>
#include <llvm/Constants.h>
#include <llvm/ValueSymbolTable.h>
#include <llvm/Value.h>
#include <llvm/Attributes.h>
#else
#include <llvm/Type.h>
#include <llvm/TypeBuilder.h>
#include <llvm/GlobalVariable.h>
#include <llvm/Constants.h>
#include <llvm/ValueSymbolTable.h>
#include <llvm/Value.h>
#include <llvm/Attributes.h>
#endif

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/system_error.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Linker.h>

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
        qFatal("%s", message.toStdString().c_str());
    }

    QByteArray stdLibBytes = StdLibFile.readAll();
    StdLibFile.close();

    std::string strdata(stdLibBytes.constData(), stdLibBytes.size());
    stdlibContents_.reset(llvm::MemoryBuffer::getMemBufferCopy(strdata, "stdlib.bc"));

}


void LLVMGenerator::reset(bool addMainEntryPoint,
                          Shared::GeneratorInterface::DebugLevel debugLevel)
{
    externs_.clear();
    debugLevel_ = debugLevel;
    context_ = new llvm::LLVMContext();
    currentModule_ = new llvm::Module("", *context_);
    currentFunction_ = 0;
    addMainEntryPoint_ = addMainEntryPoint;
    nameTranslator_->reset();
    createStdLibModule();
    createExternsTable(stdlibModule_, CString("__kumir_"));
    externs_.clear();
    readStdLibFunctions();
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

llvm::Module* LLVMGenerator::getResult()
{
    return currentModule_;
}

llvm::Module* LLVMGenerator::getStdLibModule()
{
    return stdlibModule_;
}

void LLVMGenerator::addKumirModule(const AST::ModulePtr kmod)
{
    bool voidNameFunctionDeclared = false;
    for (int i=0; i<kmod->impl.algorhitms.size(); i++) {
        const AST::AlgorithmPtr func = kmod->impl.algorhitms.at(i);
        if (!func->header.broken &&
                !(voidNameFunctionDeclared && func->header.name.isEmpty())
                )
        {
            addFunction(func, false);
            if (func->header.name.isEmpty()) {
                voidNameFunctionDeclared = true;
            }
        }
    }

}

void LLVMGenerator::createKumirModuleImplementation(const AST::ModulePtr kmod)
{
    nameTranslator_->beginNamespace();
    currentKModule_ = kmod;
    const CString moduleName = CString(QString(
                kmod->header.name.isEmpty() ? kmod->header.sourceFileName
                                            : kmod->header.name
                ).toUtf8().constData());
    nameTranslator_->beginNamespace();

    llvm::Module * lmod = currentModule_;

    llvm::Function * initFunc = llvm::Function::Create(
                llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), false),
                llvm::GlobalValue::ExternalLinkage,
                CString("__init__" + moduleName),
                currentModule_
                );

    currentFunction_ = initFunc;
    initFunctions_.push_back(initFunc);
#if LLVM_VERSION_MINOR >= 3
    initFunc->addFnAttr(llvm::Attribute::NoUnwind);
    initFunc->addFnAttr(llvm::Attribute::UWTable);
#elif LLVM_VERSION_MINOR == 0
    initFunc->addFnAttr(llvm::Attribute::NoUnwind);
    initFunc->addFnAttr(llvm::Attribute::UWTable);
#else
    initFunc->addFnAttr(llvm::Attributes::NoUnwind);
    initFunc->addFnAttr(llvm::Attributes::UWTable);
#endif
    tempValsToFree_.clear();
    llvm::BasicBlock * initBlock = llvm::BasicBlock::Create(*context_, "", initFunc);
    currentBlock_ = initBlock;
    llvm::IRBuilder<> initBuilder(initBlock);

    for (int i=0; i<kmod->impl.globals.size(); i++) {
        const AST::VariablePtr glob = kmod->impl.globals.at(i);
        addGlobalVariable(initBuilder, glob, false);
    }

    addFunctionBody(kmod->impl.initializerBody, AST::AlgorithmPtr());
    initBuilder.SetInsertPoint(currentBlock_); // Might be changed
    initBuilder.CreateRetVoid();

    for (int i=0; i<kmod->impl.algorhitms.size(); i++) {
        const AST::AlgorithmPtr func = kmod->impl.algorhitms.at(i);
        if (!func->header.broken) {
            addFunction(func, true);
        }
    }

    if (addMainEntryPoint_) {
        if (kmod->header.type == AST::ModTypeUserMain ||
                kmod->header.type == AST::ModTypeTeacherMain)
        {
            createMainFunction(kmod->impl.algorhitms.isEmpty()
                               ? AST::AlgorithmPtr()
                               : kmod->impl.algorhitms.first());
        }
    }

    currentKModule_ = AST::ModulePtr();
    nameTranslator_->endNamespace();

}

void LLVMGenerator::addGlobalVariable(llvm::IRBuilder<> & builder, const AST::VariablePtr kvar, bool constant)
{
    Q_ASSERT(context_);
    Q_ASSERT(currentModule_);

    const QString qn = currentKModule_->header.name.isEmpty()
            ? kvar->name
            : currentKModule_->header.name + "__" + kvar->name;

    CString name = nameTranslator_->add(qn);
    llvm::Type * ty = kvar->dimension > 0u
            ? getArrayType() : getScalarType();

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
    currentFunction_ = lfn;
    nameTranslator_->beginNamespace();
    llvm::BasicBlock * functionBlock =
            llvm::BasicBlock::Create(*context_, "", lfn);
    llvm::IRBuilder<> builder(functionBlock);

    builder.CreateCall2(kumirSetMainArguments_,
                        &lfn->getArgumentList().front(),
                        &lfn->getArgumentList().back());

    builder.CreateCall(kumirInitStdLib_);

    typedef std::list<llvm::Function*>::const_iterator It;
    for (It it = initFunctions_.begin(); it != initFunctions_.end(); ++it) {
        llvm::Function * initFunc = *it;
        Q_ASSERT(initFunc);
        builder.CreateCall(initFunc);
    }

    if (entryPoint) {
        const QString entryPointQName = "__kumir_function_" + entryPoint->header.name;
        const CString entryPointName =
                CString(entryPointQName.toUtf8().constData());
        Q_ASSERT(entryPointName.length() > 0);

        llvm::Function * entryPointFunc = currentModule_->getFunction(entryPointName);
        Q_ASSERT(entryPointFunc);

        std::vector<llvm::Value*> args;

        llvm::Value * ret = 0;

        if (entryPoint->header.returnType != AST::TypeNone) {
            ret = builder.CreateAlloca(getScalarType());
            args.push_back(ret);
        }

        for (int i=0; i<entryPoint->header.arguments.size(); i++) {
            const AST::VariablePtr & arg = entryPoint->header.arguments.at(i);
            llvm::Value * larg = builder.CreateAlloca(
                        arg->dimension > 0u ? getArrayType() : getScalarType()
                        );
            const CString argName = nameTranslator_->add(arg->name);
            larg->setName(argName);
            args.push_back(larg);
            if (1u == arg->dimension) {
                llvm::Value * x_start = calculate(builder, arg->bounds[0].first);
                llvm::Value * x_end = calculate(builder, arg->bounds[0].second);
                builder.CreateCall3(kumirCreateArray1_, larg, x_start, x_end);
            }
            if (arg->accessType!=AST::AccessArgumentOut) {
                createInputValue(builder, arg->name, larg, arg->baseType.kind, arg->dimension > 0u);
            }
        }


        builder.CreateCall(entryPointFunc, args);

        if (ret) {
            createOutputValue(builder, "", ret, entryPoint->header.returnType.kind, false);
        }
    }
    builder.CreateRet(
                llvm::ConstantInt::getSigned(
                    llvm::Type::getInt32Ty(*context_),
                    0)
                );
    nameTranslator_->endNamespace();
}

void LLVMGenerator::createOutputValue(Builder &builder, const QString &name, llvm::Value *value, const AST::VariableBaseType type, const bool isArray)
{
    if (!isArray) {
        __kumir_scalar_type typee = __kumir_scalar_type(0x00);
        if (type == AST::TypeBoolean) {
            typee = __KUMIR_BOOL;
        }
        else if (type == AST::TypeInteger) {
            typee = __KUMIR_INT;
        }
        else if (type == AST::TypeReal) {
            typee = __KUMIR_REAL;
        }
        else if (type == AST::TypeCharect) {
            typee = __KUMIR_CHAR;
        }
        else if (type == AST::TypeString) {
            typee = __KUMIR_STRING;
        }
        else if (type == AST::TypeUser) {
            typee = __KUMIR_RECORD;
        }

        llvm::Value * lname = builder.CreateGlobalStringPtr(
                    std::string(name.toUtf8().constData())
                    );

        llvm::Value * ltype = llvm::ConstantInt::getSigned(
                    llvm::Type::getInt32Ty(ctx), typee
                    );

        builder.CreateCall3(kumirPrintScalarVariable_, lname, ltype, value);
    }
}

void LLVMGenerator::createInputValue(Builder &builder, const QString & name, llvm::Value *value, const AST::VariableBaseType type, const bool isArray)
{
    __kumir_scalar_type typee = __kumir_scalar_type(0x00);
    if (type == AST::TypeBoolean) {
        typee = __KUMIR_BOOL;
    }
    else if (type == AST::TypeInteger) {
        typee = __KUMIR_INT;
    }
    else if (type == AST::TypeReal) {
        typee = __KUMIR_REAL;
    }
    else if (type == AST::TypeCharect) {
        typee = __KUMIR_CHAR;
    }
    else if (type == AST::TypeString) {
        typee = __KUMIR_STRING;
    }
    else if (type == AST::TypeUser) {
        typee = __KUMIR_RECORD;
    }

    llvm::Value * lname = builder.CreateGlobalStringPtr(
                std::string(name.toUtf8().constData())
                );

    llvm::Value * ltype = llvm::ConstantInt::getSigned(
                llvm::Type::getInt32Ty(ctx), typee
                );

    if (!isArray) {
        builder.CreateCall3(kumirGetScalarArgument_, lname, ltype, value);
    }
    else {
        builder.CreateCall3(kumirGetArrayArgument_, lname, ltype, value);
    }
}

void LLVMGenerator::addFunction(const AST::AlgorithmPtr kfunc, bool createBody)
{
    Q_ASSERT(context_);
    Q_ASSERT(currentModule_);
    currentAlgorithm_ = kfunc;
    ifThenElseCounter_ = 0u;
    switchCaseCounter_ = 0u;
    loopCounter_ = 0u;
    currentLoopEnd_.clear();
    currentFunctionExit_ = nullptr;

    const QString actualName = "__kumir_function_" + kfunc->header.name;

    // Use UTF-8 names to prevent unambiguous linkage between files
    CString name = CString(actualName.toUtf8().data());


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
        llvm::Type * lvar = kvar->dimension > 0u
                ? getArrayType()->getPointerTo()
                : getScalarType()->getPointerTo();
        args[i+largsOffset] = lvar;
    }

    if (kfunc->header.returnType != AST::TypeNone) {
        llvm::Type * rtype = 0;
        rtype = getScalarType()->getPointerTo();
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
#if LLVM_VERSION_MINOR >= 3
            llvm::AttrBuilder abuilder;
            abuilder.addAttribute(llvm::Attribute::StructRet);
            abuilder.addAttribute(llvm::Attribute::NoAlias);
            firstArg.addAttr(llvm::AttributeSet::get(ctx, 0, abuilder));
            firstArg.addAttr(llvm::AttributeSet::get(ctx, 1, abuilder));
#elif LLVM_VERSION_MINOR == 0
            llvm::Attributes attrs = 0x00;
            attrs |= llvm::Attribute::StructRet;
            attrs |= llvm::Attribute::NoAlias;
            firstArg.addAttr(attrs);
#else
            std::vector<llvm::Attributes::AttrVal> attrs(2);
            attrs[0] = llvm::Attributes::StructRet;
            attrs[1] = llvm::Attributes::NoAlias;
            firstArg.addAttr(llvm::Attributes::get(*context_, attrs));
#endif
        }
#if LLVM_VERSION_MINOR >= 3
        lfn->addFnAttr(llvm::Attribute::NoUnwind);
        lfn->addFnAttr(llvm::Attribute::UWTable);
#elif LLVM_VERSION_MINOR == 0
        lfn->addFnAttr(llvm::Attribute::NoUnwind);
        lfn->addFnAttr(llvm::Attribute::UWTable);
#else
        lfn->addFnAttr(llvm::Attributes::NoUnwind);
        lfn->addFnAttr(llvm::Attributes::UWTable);
#endif
    }

    if (createBody) {
        tempValsToFree_.clear();
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
            llvm::Argument & larg = *lIt;
            larg.setName(lfn->getName());
            lIt ++;
            builder.CreateCall(kumirCreateUndefinedScalar_, &larg);
        }

        std::vector<llvm::Value*> argumentValues;

        for ( ; lIt != lfn->getArgumentList().end() && kIt != namedArgs.end() ; ++lIt, ++kIt)
        {
            llvm::Argument & larg = *lIt;
            const AST::VariablePtr & karg = *kIt;
            const QString kArgName = karg->name;
            const CString lArgName = nameTranslator_->add(kArgName);
            larg.setName(lArgName);
            argumentValues.push_back(&larg);
        }

        // Stack-allocate local variables
        for (int i=0; i<kfunc->impl.locals.size(); i++) {
            const AST::VariablePtr & kvar = kfunc->impl.locals[i];
            if (kvar->accessType == AST::AccessRegular && kvar->name != kfunc->header.name) {
                const QString qname = kvar->name;
                const CString cname = nameTranslator_->add(qname);
                Q_ASSERT(!cname.empty());
                llvm::Type * ty = kvar->dimension > 0u
                        ? getArrayType() : getScalarType();
                llvm::Value * arg = builder.CreateAlloca(ty, 0, cname);
                llvm::Function * initFunc = kvar->dimension == 0u
                        ? kumirCreateUndefinedScalar_ : kumirCreateUndefinedArray_;
                builder.CreateCall(initFunc, arg);
            }
        }

        // Initialize array arguments and wash-up out-parameters
        for (int i=0; i<kfunc->header.arguments.size(); i++) {
            const AST::VariablePtr & karg = kfunc->header.arguments[i];
            if (karg->dimension > 0u) {
                llvm::Value * lvar = argumentValues[i];
                llvm::Function * initFunc = 0;
                std::vector<llvm::Value*> initArgs;
                initArgs.push_back(lvar);
                for (size_t d=0u; d<karg->dimension; d++) {
                    const AST::Variable::Bound & bound = karg->bounds[d];
                    const AST::ExpressionPtr & left = bound.first;
                    const AST::ExpressionPtr & right = bound.second;
                    llvm::Value * lleft = calculate(builder, left);
                    llvm::Value * lright = calculate(builder, right);
                    initArgs.push_back(lleft);
                    initArgs.push_back(lright);
                }
                if (karg->accessType == AST::AccessArgumentIn) {
                    if      (1u == karg->dimension) initFunc = kumirCloneArray1_;
                    else if (2u == karg->dimension) initFunc = kumirCloneArray2_;
                    else if (3u == karg->dimension) initFunc = kumirCloneArray3_;
                }
                else {
                    if      (1u == karg->dimension) initFunc = kumirRefArray1_;
                    else if (2u == karg->dimension) initFunc = kumirRefArray2_;
                    else if (3u == karg->dimension) initFunc = kumirRefArray3_;
                }
                Q_ASSERT(initFunc);
                builder.CreateCall(initFunc, initArgs);
                if (karg->accessType == AST::AccessArgumentOut && karg->dimension > 0u) {
                    builder.CreateCall(kumirCleanUpArrayInShape_, lvar);
                }
            }
            else if (karg->accessType == AST::AccessArgumentOut) {
                llvm::Value * lvar = argumentValues[i];
                builder.CreateCall(kumirFreeScalar_, lvar);
            }
        }

        builder.SetInsertPoint(currentBlock_);
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
        // Free arrays, strings and record value scalars
        for (int i=0; i<kfunc->impl.locals.size(); i++) {
            const AST::VariablePtr & kvar = kfunc->impl.locals[i];
            if (kvar->accessType != AST::AccessArgumentInOut &&
                    kvar->accessType != AST::AccessArgumentOut &&
                    kvar->name != kfunc->header.name)
            {
                if (kvar->dimension > 0u)
                {
                    llvm::Value * lvar = findVariableAtCurrentContext(kvar);
                    Q_ASSERT(lvar);
                    builder.CreateCall(kumirFreeArray_, lvar);
                }
                else if (kvar->baseType.kind == AST::TypeString ||
                         kvar->baseType.kind == AST::TypeUser)
                {
                    if (kvar->accessType != AST::AccessArgumentIn) {
                        llvm::Value * lvar = findVariableAtCurrentContext(kvar);
                        Q_ASSERT(lvar);
                        builder.CreateCall(kumirFreeScalar_, lvar);
                    }
                }
            }

        }
        builder.CreateCall(kumirPopCallStackCounter_);
        builder.CreateRetVoid();
        nameTranslator_->endNamespace();
    }
}

void LLVMGenerator::addFunctionBody(const QList<AST::StatementPtr> & statements, const AST::AlgorithmPtr & alg)
{
    llvm::IRBuilder<> builder(currentBlock_);

    if (alg) {
        QString headerError =  "";
        QString beginError = "";
        int headerLine = -1;
        int beginLine = -1;

        if (alg->impl.headerLexems.size()>0) {
            for (int i=0; i<alg->impl.headerLexems.size();i++) {
                if (alg->impl.headerLexems[i]->error.size()>0) {
                    headerError = Shared::ErrorMessages::message("KumirAnalizer", QLocale::Russian, alg->impl.headerLexems[i]->error);
                    headerLine = alg->impl.headerLexems[i]->lineNo + 1;
                    break;
                }
            }
        }

        if (alg->impl.beginLexems.size()>0) {
            for (int i=0; i<alg->impl.beginLexems.size();i++) {
                if (alg->impl.beginLexems[i]->error.size()>0) {
                    beginError = Shared::ErrorMessages::message("KumirAnalizer", QLocale::Russian, alg->impl.beginLexems[i]->error);
                    beginLine = alg->impl.beginLexems[i]->lineNo + 1;
                    break;
                }
            }
        }

        QString err;
        int errLine = -1;
        if (headerError.length() > 0) {
            err = headerError;
            errLine = headerLine;
        }
        else {
            err = beginError;
            errLine = beginLine;
        }

        if (err.length() > 0) {
            if (debugLevel_ != Shared::GeneratorInterface::NoDebug) {
                builder.CreateCall(kumirSetCurrentLineNumber_,
                                   llvm::ConstantInt::getSigned(
                                       llvm::Type::getInt32Ty(ctx),
                                       errLine)
                                   );
            }
            const std::string error = std::string(err.toUtf8().data());
            llvm::Value * arg = builder.CreateGlobalStringPtr(error);
            Q_ASSERT(arg);
            Q_ASSERT(kumirAbortOnError_);
            builder.CreateCall(kumirAbortOnError_, arg);
            return;
        }
    }

    for (int i=0; i<statements.size(); i++) {
        llvm::BasicBlock * block = currentBlock_;
        Q_ASSERT(block);
        if (!block->empty() && block->back().isTerminator()) {
            break; // the remaining code is unreachable
        }
        builder.SetInsertPoint(block);
        const AST::StatementPtr & statement = statements.at(i);
        const AST::StatementType type = statement->type;
        if (statement->lexems.size() > 0) {
            const AST::LexemPtr frontLexem = statement->lexems.front();
            int lineNo = frontLexem->lineNo + 1;
            Q_ASSERT(kumirSetCurrentLineNumber_);
            if (debugLevel_ != Shared::GeneratorInterface::NoDebug) {
                builder.CreateCall(kumirSetCurrentLineNumber_,
                                   llvm::ConstantInt::getSigned(
                                       llvm::Type::getInt32Ty(ctx),
                                       lineNo)
                                   );
            }
        }
        if (!statement->error.isEmpty()) {
            if (!statement->skipErrorEvaluation) {
                createError(builder, statement);
            }
        }
        else {
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
            case AST::StError:
                if (!statement->skipErrorEvaluation) {
                    createError(builder, statement);
                }
                break;
            case AST::StHalt:
                createHalt(builder, statement, alg);
                break;
            case AST::StPause:
                break; // not used in console mode
            default:
                qFatal("Not implemented!");
                break;
            }
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
    llvm::Type * ty = var->dimension > 0u
            ? getArrayType() : getScalarType();
    CString name;
    llvm::Value * result = 0;
    if (overrideName.length() == 0) {
        result = findVariableAtCurrentContext(var);
        name = result->getName();
    }
    else {
        name = nameTranslator_->add(overrideName);
        Q_ASSERT(!name.empty());
        result = builder.CreateAlloca(ty, 0, name);
    }
    Q_ASSERT(result);
    llvm::Function * initFunc = 0;
    llvm::Function * fillFunc = 0;
    std::vector<llvm::Value*> initArgs;
    llvm::Value * fillValue = 0;
    initArgs.push_back(result);
    if (var->dimension == 0u) {
        if (var->initialValue.isValid()) {
            initFunc = kumirCreateDefinedScalar_;
            llvm::Value * cval = createConstant(builder, var->baseType, var->initialValue);
            Q_ASSERT(cval);
            initArgs.push_back(cval);
        }
    }
    else {
        llvm::Function * kumirInitArray = 0;
        if      (1u == var->dimension) kumirInitArray = kumirCreateArray1_;
        else if (2u == var->dimension) kumirInitArray = kumirCreateArray2_;
        else if (3u == var->dimension) kumirInitArray = kumirCreateArray3_;
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
    if (initFunc) {
        builder.CreateCall(initFunc, initArgs);
        if (fillFunc) {
            Q_ASSERT(fillValue);
            builder.CreateCall2(fillFunc, result, fillValue);
        }
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
        else if (lvalue->kind == AST::ExprArrayElement &&
                 lvalue->variable->dimension == lvalue->operands.size())
        {
            storeFunc = kumirAssignScalarToArrayElement_;
        }
        else if (lvalue->kind == AST::ExprArrayElement &&
                 lvalue->variable->dimension < lvalue->operands.size())
        {
            storeFunc = kumirAssignScalarToStringRef_;
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
        builder.CreateCall(kumirCheckValueDefined_, fileHandle);
        Q_ASSERT(fileHandle);
    }
    std::vector<llvm::Value *> values;
    std::vector<llvm::Value *> types;
    typedef struct {
        bool c;
        llvm::Value *s;
    } format_t ;

    std::vector<format_t> f1s;
    std::vector<format_t> f2s;

    for (int i=0; i<lexemsCount; i++) {
        int exprIndex = 3 * i;
        int format1Index = 3 * i + 1;
        int format2Index = 3 * i + 2;
        const AST::ExpressionPtr & expr = st->expressions.at(exprIndex);
        const AST::ExpressionPtr & format1 = st->expressions.at(format2Index);
        const AST::ExpressionPtr & format2 = st->expressions.at(format1Index);

        __kumir_scalar_type typee = __kumir_scalar_type(0x00);
        if (expr->baseType.kind == AST::TypeBoolean) {
            typee = __KUMIR_BOOL;
        }
        else if (expr->baseType.kind == AST::TypeInteger) {
            typee = __KUMIR_INT;
        }
        else if (expr->baseType.kind == AST::TypeReal) {
            typee = __KUMIR_REAL;
        }
        else if (expr->baseType.kind == AST::TypeCharect) {
            typee = __KUMIR_CHAR;
        }
        else if (expr->baseType.kind == AST::TypeString) {
            typee = __KUMIR_STRING;
        }
        else if (expr->baseType.kind == AST::TypeUser) {
            typee = __KUMIR_RECORD;
        }

        llvm::Value * ltype = llvm::ConstantInt::getSigned(
                    llvm::Type::getInt32Ty(ctx), typee
                    );

        types.push_back(ltype);

        llvm::Value * lexpr = calculate(builder, expr);
        builder.CreateCall(kumirCheckValueDefined_, lexpr);
        values.push_back(lexpr);

        llvm::Function * outFunc = 0;
        std::vector<llvm::Value*> args;
        format_t f1, f2;

        if (format1->kind == AST::ExprConst) {
            llvm::Value * format = llvm::ConstantInt::getSigned(
                        llvm::Type::getInt32Ty(*context_),
                        format1->constant.toInt()
                        );
            f1.s = format;
            f1.c = true;
        }
        else {
            llvm::Value * format = calculate(builder, format1);
            f1.s = format;
            f1.c = false;
        }

        if (format2->kind == AST::ExprConst) {
            llvm::Value * format = llvm::ConstantInt::getSigned(
                        llvm::Type::getInt32Ty(*context_),
                        format2->constant.toInt()
                        );
            f2.s = format;
            f2.c = true;
        }
        else {
            llvm::Value * format = calculate(builder, format2);
            f2.s = format;
            f2.c = false;
        }
        f1s.push_back(f1);
        f2s.push_back(f2);
    }

    Q_ASSERT(values.size() == types.size());
    Q_ASSERT(f1s.size() == f2s.size());
    Q_ASSERT(f1s.size() == values.size());

    for (size_t i=0u; i<values.size(); i++) {
        const format_t & f1 = f1s.at(i);
        const format_t & f2 = f2s.at(i);
        llvm::Function * outFunc = 0;
        std::vector<llvm::Value*> args;
        if (fileHandleProvided) {
            if      (f1.c && f2.c)      outFunc = kumirOutputFileII_;
            else if (f1.c && !f2.c)     outFunc = kumirOutputFileIS_;
            else if (!f1.c && f2.c)     outFunc = kumirOutputFileSI_;
            else if (!f1.c && !f2.c)    outFunc = kumirOutputFileSS_;
            args.push_back(fileHandle);
        }
        else {
            if      (f1.c && f2.c)      outFunc = kumirOutputStdoutII_;
            else if (f1.c && !f2.c)     outFunc = kumirOutputStdoutIS_;
            else if (!f1.c && f2.c)     outFunc = kumirOutputStdoutSI_;
            else if (!f1.c && !f2.c)    outFunc = kumirOutputStdoutSS_;
        }
        args.push_back(values[i]);
        args.push_back(types[i]);
        args.push_back(f1.s);
        args.push_back(f2.s);
        builder.CreateCall(outFunc, args);
    }

    createFreeTempScalars(builder);
}

void LLVMGenerator::createHalt(llvm::IRBuilder<> &builder, const AST::StatementPtr &, const AST::AlgorithmPtr &)
{
    builder.CreateCall(kumirHalt_);
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
        llvm::Type * const ptrTy = getScalarType()->getPointerTo();
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

    int errNo = -1;
    QString err;

    if (st->headerError.length() > 0) {
        errNo = st->headerErrorLine + 1;
        err = st->headerError;
    }
    else {
        foreach (const AST::LexemPtr lx, st->lexems) {
            if (lx->error.length() > 0) {
                err = lx->error;
                errNo = lx->lineNo + 1;
                break;
            }
        }
    }

    if (err.length() > 0) {
        if (debugLevel_ != Shared::GeneratorInterface::NoDebug) {
            builder.CreateCall(kumirSetCurrentLineNumber_,
                               llvm::ConstantInt::getSigned(
                                   llvm::Type::getInt32Ty(ctx),
                                   errNo
                                   )
                               );
        }
        builder.CreateCall(kumirAbortOnError_,
                           builder.CreateGlobalStringPtr(
                               std::string(
                                   Shared::ErrorMessages::message(
                                       "KumirAnalizer", QLocale::Russian,
                                       err
                                       )
                                   .toUtf8().constData()
                                   )
                               ));
        return;
    }

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

    currentLoopEnd_.push_back(loop_clean);
    createFreeTempScalars(builder);
    builder.CreateBr(loop_init);

    // --- initialization
    builder.SetInsertPoint(loop_init);
    if (loop.type == AST::LoopFor && loop.forVariable) {
        const AST::VariablePtr & kvar = loop.forVariable;
//        const CString name = nameTranslator_->find(kvar->name);
//        Q_ASSERT(!name.empty());
//        loop_variable = currentBlock_->getValueSymbolTable()->lookup(name);
//        if (!loop_variable) {
//            loop_variable = currentModule_->getGlobalVariable(name, true);
//        }
        loop_variable = findVariableAtCurrentContext(kvar);
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
            builder.CreateCall3(kumirLoopForFromToStepInitCounter_,
                                for_from, for_to, for_step);
        }
        else {
            Q_ASSERT(kumirLoopForFromToInitCounter_);
            builder.CreateCall2(kumirLoopForFromToInitCounter_, for_from, for_to);
        }
    }
    else if (loop.type == AST::LoopTimes) {
        times = calculate(builder, loop.timesValue);
        Q_ASSERT(times);
        Q_ASSERT(kumirLoopTimesInitCounter_);
        builder.CreateCall(kumirLoopTimesInitCounter_, times);
    }
    createFreeTempScalars(builder);
    builder.CreateBr(loop_begin);

    // --- loop pre-check
    builder.SetInsertPoint(loop_begin);
    llvm::Value * loop_pre_check = 0;
    if (loop.type == AST::LoopFor && loop_variable && for_from && for_to) {        
        loop_pre_check = builder.CreateCall (kumirLoopForCheckCounter_,
                                             loop_variable,
                                             basicName + CString("for_check")
                                             );
        Q_ASSERT(loop_pre_check);
    }
    else if (loop.type == AST::LoopTimes) {
        Q_ASSERT(kumirLoopTimesCheckCounter_);
        loop_pre_check = builder.CreateCall(kumirLoopTimesCheckCounter_,
                                            basicName + CString("times_check")
                                            );
    }
    else if (loop.type == AST::LoopWhile && loop.whileCondition) {
        Q_ASSERT(kumirScalarAsBool_);
        loop_pre_check = builder.CreateCall(kumirScalarAsBool_,
                                            calculate(builder, loop.whileCondition),
                                            basicName + CString("while_check")
                                            );
    }

    createFreeTempScalars(builder);
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
        createFreeTempScalars(builder);
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
        QString endErr;
        int endErrLine;
        foreach (const AST::LexemPtr lx, loop.endLexems) {
            if (lx->error.length() > 0) {
                endErr = lx->error;
                endErrLine = lx->lineNo + 1;
                break;
            }
        }
        if (endErr.length() > 0) {
            if (debugLevel_ != Shared::GeneratorInterface::NoDebug) {
                builder.CreateCall(kumirSetCurrentLineNumber_,
                                   llvm::ConstantInt::getSigned(
                                       llvm::Type::getInt32Ty(ctx),
                                       endErrLine
                                       )
                                   );
            }
            builder.CreateCall(kumirAbortOnError_,
                               builder.CreateGlobalStringPtr(
                                   std::string(
                                       Shared::ErrorMessages::message(
                                           "KumirAnalizer", QLocale::Russian,
                                           endErr
                                           )
                                       .toUtf8().constData()
                                       )
                                   ));
        }
        createFreeTempScalars(builder);
        builder.CreateBr(loop_begin);
    }

    // --- clean counters if need
    builder.SetInsertPoint(loop_clean);
    if (loop.type == AST::LoopFor || loop.type == AST::LoopTimes) {
        builder.CreateCall(kumirLoopEndCounter_);
    }
    createFreeTempScalars(builder);
    builder.CreateBr(loop_done);

    // --- loop done
    currentBlock_ = loop_done;
    builder.SetInsertPoint(loop_done);
    currentLoopEnd_.pop_back();
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
    createFreeTempScalars(builder);
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
        createFreeTempScalars(builder);
        builder.CreateBr(done);
    }

    // --- done
    currentBlock_ = done;
    builder.SetInsertPoint(done);
}

void LLVMGenerator::createSwitchCaseElse(llvm::IRBuilder<> &builder, const AST::StatementPtr &st, const AST::AlgorithmPtr &alg)
{
    if (st->headerError.size() > 0) {
        if (debugLevel_ != Shared::GeneratorInterface::NoDebug) {
            builder.CreateCall(kumirSetCurrentLineNumber_,
                               llvm::ConstantInt::getSigned(
                                   llvm::Type::getInt32Ty(ctx),
                                   st->headerErrorLine + 1
                                   ));
        }
        builder.CreateCall(kumirAbortOnError_,
                           builder.CreateGlobalStringPtr(
                               std::string(
                                   Shared::ErrorMessages::message(
                                       "KumirAnalizer",
                                       QLocale::Russian,
                                        st->headerError
                                       ).toUtf8().data()
                                   )
                               ));
    }

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
    createFreeTempScalars(builder);
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
            createFreeTempScalars(builder);
            builder.CreateCondBr(condBool, body, nextBlock);
        }
        else {
            createFreeTempScalars(builder);
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
            createFreeTempScalars(builder);
            builder.CreateBr(done);
        }
    }

    // --- done
    currentBlock_ = done;
    builder.SetInsertPoint(done);
}

void LLVMGenerator::createBreak(llvm::IRBuilder<> &builder, const AST::StatementPtr &st, const AST::AlgorithmPtr &alg)
{
    if (!currentLoopEnd_.isEmpty()) {
        createFreeTempScalars(builder);
        builder.CreateBr(currentLoopEnd_.back()); // break ends loop
    }
    else {
        createFreeTempScalars(builder);
        builder.CreateBr(currentFunctionExit_); // break ends function
    }
}

void LLVMGenerator::createError(llvm::IRBuilder<> &builder, const AST::StatementPtr &st)
{
    const QString qError = Shared::ErrorMessages::message("KumirAnalizer", QLocale::Russian, st->error);
    const std::string error = std::string(qError.toUtf8().data());
    llvm::Value * arg = builder.CreateGlobalStringPtr(error);
    Q_ASSERT(arg);
    Q_ASSERT(kumirAbortOnError_);
    builder.CreateCall(kumirAbortOnError_, arg);
}

void LLVMGenerator::createFreeTempScalars(llvm::IRBuilder<> &builder)
{
    size_t from = 0u;
    if (!tempValsToFreeStartPos_.empty()) {
        from = tempValsToFreeStartPos_.top();
    }
    for (size_t i=from; i<tempValsToFree_.size(); i++) {
        builder.CreateCall(kumirFreeScalar_, tempValsToFree_.at(i));
    }
    if (from < tempValsToFree_.size()) {
        tempValsToFree_.resize(from);
    }
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
        if (varName.empty() && currentKModule_->header.name.length() > 0) {
            varName = nameTranslator_->find(currentKModule_->header.name +
                                            "__" +
                                            kvar->name);
        }
        Q_ASSERT(varName.length() > 0);
        var = currentBlock_->getValueSymbolTable()->lookup(varName);
        if (!var) {
            var = currentModule_->getGlobalVariable(varName, true);
        }
        if (!var) {
//            currentFunction_->dump();
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
        if (ex->function->header.implType == AST::AlgorhitmCompiled &&
                Shared::GeneratorInterface::NoDebug != debugLevel_)
        {
            builder.CreateCall(kumirSetCurrentLineNumber_,
                               llvm::ConstantInt::getSigned(
                                   llvm::Type::getInt32Ty(ctx),
                                   ex->lexems.first()->lineNo + 1
                                   )
                               );
        }
    }
    else if (ex->kind == AST::ExprSubexpression) {
        result = createSubExpession(builder, ex);
    }
    if (!isLvalue && ex->kind != AST::ExprArrayElement) {
        bool notPlainScalar = ex->baseType.kind == AST::TypeString || ex->baseType.kind == AST::TypeUser;
        if (notPlainScalar && ex->dimension == 0u) {
            tempValsToFree_.push_back(result);
        }
    }
    return result;
}

llvm::Value * LLVMGenerator::createArrayElementGet(llvm::IRBuilder<> &builder, const AST::ExpressionPtr &ex, bool isLvalue)
{
//    CString varName = nameTranslator_->find(ex->variable->name);
//    Q_ASSERT(varName.length() > 0);
//    llvm::Value * var = currentBlock_->getValueSymbolTable()->lookup(varName);
//    if (!var) {
//        var = currentModule_->getGlobalVariable(varName, true);
//    }
    llvm::Value * var = findVariableAtCurrentContext(ex->variable);
    Q_ASSERT(var);
    std::vector<llvm::Value*> args;
    llvm::Value * elemPtr = builder.CreateAlloca(getScalarType()->getPointerTo());
    Q_ASSERT(elemPtr);
    if (ex->variable->dimension > 0u) {
        args.push_back(elemPtr);
        if (!isLvalue) {
            args.push_back(llvm::ConstantInt::getTrue(ctx));
        }
        else {
            args.push_back(llvm::ConstantInt::getFalse(ctx));
        }
        args.push_back(var);
        for (int i=0; i<ex->variable->dimension; i++) {
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
    }
    else {
        llvm::Value * variableArg = findVariableAtCurrentContext(ex->variable);
        Q_ASSERT(variableArg);
        builder.CreateStore(variableArg, elemPtr);
    }
    const size_t diff = static_cast<size_t>(ex->operands.size()) - ex->variable->dimension;
    if (diff == 0u) {
        // Array element access
        if (isLvalue) {
            return elemPtr;
        }
        else {
            llvm::Value * result = builder.CreateLoad(elemPtr);
            return result;
        }
    }
    else if (diff == 1u) {
        // String char element
        const AST::ExpressionPtr & indexOperand =
                ex->operands[ex->variable->dimension];
        llvm::Value * index = calculate(builder, indexOperand);
        Q_ASSERT(index);
        llvm::Value * result = 0;
        if (isLvalue) {
            llvm::StructType * struct_stringref =
                    stdlibModule_->getTypeByName("struct.__kumir_stringref");
            Q_ASSERT(struct_stringref);
            result = builder.CreateAlloca(struct_stringref);
            builder.CreateCall3(kumirGetStringElementRef_,
                                result, elemPtr, index);
        }
        else {
            llvm::StructType * struct_scalar =
                    stdlibModule_->getTypeByName("struct.__kumir_scalar");
            Q_ASSERT(struct_scalar);
            result = builder.CreateAlloca(struct_scalar);
            builder.CreateCall3(kumirGetStringElement_,
                                result, elemPtr, index);
        }
        Q_ASSERT(result);
        return result;
    }
    else if (diff == 2u) {
        // String slice
        const AST::ExpressionPtr & index1Operand =
                ex->operands[ex->variable->dimension];
        llvm::Value * index1 = calculate(builder, index1Operand);
        Q_ASSERT(index1);
        const AST::ExpressionPtr & index2Operand =
                ex->operands[ex->variable->dimension + 1];
        llvm::Value * index2 = calculate(builder, index2Operand);
        Q_ASSERT(index2);
        llvm::Value * result = 0;
        if (isLvalue) {
            llvm::StructType * struct_stringref =
                    stdlibModule_->getTypeByName("struct.__kumir_stringref");
            Q_ASSERT(struct_stringref);
            result = builder.CreateAlloca(struct_stringref);
            builder.CreateCall4(kumirGetStringSliceRef_,
                                result, elemPtr, index1, index2);
        }
        else {
            llvm::StructType * struct_scalar =
                    stdlibModule_->getTypeByName("struct.__kumir_scalar");
            Q_ASSERT(struct_scalar);
            result = builder.CreateAlloca(struct_scalar);
            builder.CreateCall4(kumirGetStringSlice_,
                                result, elemPtr, index1, index2);
        }
        Q_ASSERT(result);
        return result;
    }
    else {
        return 0;
    }
}

llvm::Value * LLVMGenerator::createStringSliceGet(llvm::IRBuilder<> &builder, const AST::ExpressionPtr &ex, bool isLvalue)
{
    return 0;
}

llvm::Value * LLVMGenerator::createConstant(llvm::IRBuilder<> & builder, const AST::Type kty, const QVariant &value)
{
    llvm::Value * result = 0;
    llvm::Type * ty = getScalarType();
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
    int32_t ival; double rval; bool bval; QByteArray sval; uint16_t cval;
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
    else if (AST::TypeCharect == bt) {
        cval = value.isValid() ? uint16_t(value.toChar().unicode()) : 0u;
        ptr = reinterpret_cast<char*>(&cval);
        sz = sizeof(uint16_t);
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
        builder.CreateCall(kumirCheckCallStack_);
        funcName = CString("__kumir_function_") +
                CString(alg->header.name.toUtf8().constData());
        func = currentModule_->getFunction(funcName);
        Q_ASSERT(func);
    }
    else if (alg->header.implType == AST::AlgorhitmExternal && (
            alg->header.external.moduleName.endsWith(".kum") ||
            alg->header.external.moduleName.endsWith(".kod") )
            )
    {
        funcName = CString(alg->header.name.toUtf8().constData());
        funcName = CString("__kumir_function_") +
                CString(alg->header.name.toUtf8().constData());
        func = currentModule_->getFunction(funcName);
        Q_ASSERT(func);
    }
    else if (alg->header.implType == AST::AlgorhitmExternal)
    {
        static const QStringList operators = QStringList()
                << "=" << "<>" << "<" << ">" << "+" << "-" << "*" << "/"
                << QString::fromUtf8("ввод") << QString::fromUtf8("вывод");
        static const QStringList operatorNames = QStringList()
                << "eq" << "neq" << "lt" << "gt" << "plus" << "minus" << "astarisk" << "slash"
                << QString::fromUtf8("input") << QString::fromUtf8("output");
        Q_ASSERT(operators.size() == operatorNames.size());
        std::vector<llvm::Type*> formalArgs(alg->header.arguments.size());
        funcName = alg->header.cHeader.toStdString();
        if (funcName.empty()) {
            funcName = "__kumir_stdlib_" ; // TODO implement non-std algorithms
            if (operators.contains(alg->header.name)) {
                int index = operators.indexOf(alg->header.name);
                const CString cname = operatorNames[index].toStdString();
                funcName = CString("__kumir_operator_") + cname;
            }
            else {
                funcName += NameTranslator::suggestName(alg->header.name).toStdString();
            }
        }
//        QStringList params;
//        for (int i=0; i<alg->header.arguments.size(); i++) {
//            const AST::VariablePtr & arg = alg->header.arguments[i];
//            QString param = arg->dimension == 0u? "__kumir_scalar" : "__kumir_array";
//            bool ref = false;
//            if (arg->accessType == AST::AccessArgumentInOut || arg->accessType == AST::AccessArgumentOut) {
//                param += "&";
//                ref = true;
//            }
//            params.push_back(param);
//            formalArgs[i] =
//                    arg->dimension > 0 ? getArrayType() : getScalarType();
//            if (ref) {
//                formalArgs[i] = formalArgs[i]->getPointerTo();
//            }
//        }
        llvm::Type * rtype = alg->header.returnType.kind == AST::TypeNone
                ? llvm::Type::getVoidTy(ctx) : getScalarType();
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
        llvm::Value * arg = 0;
        if (alg->header.arguments[i]->dimension == 0u) {
            // Scalar value
            const AST::ExpressionPtr & argExpr = arguments.at(i);
            arg = calculate(builder, argExpr);
            if (alg->header.arguments[i]->accessType == AST::AccessArgumentIn) {
                builder.CreateCall(kumirCheckValueDefined_, arg);
                // Copy variable to prevent it changing
                // if passed as another value out-parameter
                if (argExpr->kind == AST::ExprVariable) {
                    llvm::Value * argCopy = builder.CreateAlloca(
                                getScalarType(),
                                0,
                                arg->getName() + std::string("_copy")
                                );
                    builder.CreateCall2(kumirAssignScalarToScalar_,
                                        argCopy, arg);
                    arg = argCopy;
                }
            }            
        }
        else {
            // Array
            const AST::ExpressionPtr & argExpr = arguments.at(i);
            const AST::VariablePtr & varExpr = argExpr->variable;
            Q_ASSERT(varExpr);
            llvm::Value * var = findVariableAtCurrentContext(varExpr);
            Q_ASSERT(kumirLinkArray_);
            arg = builder.CreateAlloca(getArrayType());
            arg->setName(var->getName() + CString("_array_link"));
            builder.CreateCall2(kumirLinkArray_, arg, var);
        }
        Q_ASSERT(arg);
        args[loffset+i] = arg;
    }

    llvm::Value * result = 0;

    if (alg->header.returnType != AST::TypeNone) {
        llvm::Type * rtype = getScalarType();
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

    llvm::Value * result = builder.CreateAlloca(getScalarType());
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
    case AST::OpSubstract:
        opFunc = operands.size() == 2u
                ? kumirOpSub_
                : kumirOpNeg_;
        break;
    case AST::OpMultiply:       opFunc = kumirOpMul_;   break;
    case AST::OpDivision:       opFunc = kumirOpDiv_;   break;
    case AST::OpPower:          opFunc = kumirOpPow_;   break;
    case AST::OpNot:            opFunc = kumirOpNeg_;   break;
    case AST::OpAnd:            opFunc = 0;             break;
    case AST::OpOr:             opFunc = 0;             break;
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
    tempValsToFreeStartPos_.push(tempValsToFree_.size());
    Q_ASSERT(op == AST::OpAnd || op == AST::OpOr);
    llvm::Value * result = builder.CreateAlloca(getScalarType());
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
    createFreeTempScalars(builder);
    builder.CreateCondBr(leftBool, jumpOnTrue, jumpOnFalse);

    // --- check right operand
    builder.SetInsertPoint(checkNext);
    currentBlock_ = checkNext;

    llvm::Value * rightResult = calculate(builder, right);
    builder.CreateCall2(kumirMoveScalar_, result, rightResult);
    createFreeTempScalars(builder);
    builder.CreateBr(done);

    // --- done
    currentBlock_ = done;
    builder.SetInsertPoint(done);
    tempValsToFreeStartPos_.pop();
    return result;
}


void LLVMGenerator::createExternsTable(const llvm::Module * const source,
                                       const CString & prefix)
{
    typedef llvm::Module::const_iterator Fit;

    for (Fit it = source->begin(); it!=source->end(); ++it) {
        const llvm::Function & func = *it;
        if (func.hasExternalLinkage()) {
            const CString name = func.getName();
            if (0u == name.find(prefix)) {
                const CString realName = name.substr(prefix.length());
                if (realName.length() > 0 && '_' != realName.at(0)) {
                    externs_.push_back(&func);
                }
            }
        }
    }
    if (currentModule_) {
        addExternsToModule(currentModule_);
    }
}

void LLVMGenerator::addExternsToModule(llvm::Module * const target)
{
    for (int i=0; i<externs_.size(); i++) {
        const llvm::Function * const srcFunc = externs_[i];
        llvm::FunctionType * const srcFuncType = srcFunc->getFunctionType();
        const llvm::StringRef srcFuncName = srcFunc->getName();
        const llvm::GlobalValue::LinkageTypes srcLinkage = srcFunc->getLinkage();

        llvm::Function * targetFunc =
                llvm::Function::Create(srcFuncType, srcLinkage, srcFuncName, target);

    }
}

llvm::StructType * LLVMGenerator::getScalarType()
{
    llvm::StructType * result = currentModule_->getTypeByName("struct.__kumir_scalar");
    if (!result) {
        result = stdlibModule_->getTypeByName("struct.__kumir_scalar");
    }
    Q_ASSERT(result);
    return result;
}

llvm::StructType * LLVMGenerator::getArrayType()
{
    llvm::StructType * result = currentModule_->getTypeByName("struct.__kumir_array");
    if (!result) {
        result = stdlibModule_->getTypeByName("struct.__kumir_array");
    }
    Q_ASSERT(result);
    return result;
}

llvm::StructType * LLVMGenerator::getStringRefType()
{
    llvm::StructType * result = currentModule_->getTypeByName("struct.__kumir_stringref");
    if (!result) {
        result = stdlibModule_->getTypeByName("struct.__kumir_stringref");
    }
    Q_ASSERT(result);
    return result;
}

void LLVMGenerator::readStdLibFunctions()
{
    kumirInitStdLib_ = stdlibModule_->getFunction("__kumir_init_stdlib");
    Q_ASSERT(kumirInitStdLib_);

    kumirSetMainArguments_ = stdlibModule_->getFunction("__kumir_set_main_arguments");
    Q_ASSERT(kumirSetMainArguments_);

    kumirCreateUndefinedScalar_ = stdlibModule_->getFunction("__kumir_create_undefined_scalar");
    Q_ASSERT(kumirCreateUndefinedScalar_);

    kumirCreateUndefinedArray_ = stdlibModule_->getFunction("__kumir_create_undefined_array");
    Q_ASSERT(kumirCreateUndefinedArray_);

    kumirCreateDefinedScalar_ = stdlibModule_->getFunction("__kumir_create_defined_scalar");
    Q_ASSERT(kumirCreateDefinedScalar_);

    kumirCreateArray1_ = stdlibModule_->getFunction("__kumir_create_array_1");
    Q_ASSERT(kumirCreateArray1_);

    kumirRefArray1_ = stdlibModule_->getFunction("__kumir_create_array_ref_1");
    Q_ASSERT(kumirRefArray1_);

    kumirCloneArray1_ = stdlibModule_->getFunction("__kumir_create_array_copy_1");
    Q_ASSERT(kumirCloneArray1_);

    kumirCreateArray2_ = stdlibModule_->getFunction("__kumir_create_array_2");
    Q_ASSERT(kumirCreateArray2_);

    kumirRefArray2_ = stdlibModule_->getFunction("__kumir_create_array_ref_2");
    Q_ASSERT(kumirRefArray2_);

    kumirCloneArray2_ = stdlibModule_->getFunction("__kumir_create_array_copy_2");
    Q_ASSERT(kumirCloneArray2_);

    kumirCreateArray3_ = stdlibModule_->getFunction("__kumir_create_array_3");
    Q_ASSERT(kumirCreateArray3_);

    kumirRefArray3_ = stdlibModule_->getFunction("__kumir_create_array_ref_3");
    Q_ASSERT(kumirRefArray3_);

    kumirCloneArray3_ = stdlibModule_->getFunction("__kumir_create_array_copy_3");
    Q_ASSERT(kumirCloneArray3_);

    kumirLinkArray_ = stdlibModule_->getFunction("__kumir_link_array");
    Q_ASSERT(kumirLinkArray_);

    kumirFreeArray_ = stdlibModule_->getFunction("__kumir_free_array");
    Q_ASSERT(kumirFreeArray_);

    kumirCleanUpArrayInShape_ = stdlibModule_->getFunction("__kumir_cleanup_array_in_shape");
    Q_ASSERT(kumirCleanUpArrayInShape_);

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

    kumirAssignScalarToStringRef_ = stdlibModule_->getFunction("__kumir_modify_string");
    Q_ASSERT(kumirAssignScalarToStringRef_);

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

    kumirOutputStdout_ = stdlibModule_->getFunction("__kumir_output_stdout");
    Q_ASSERT(kumirOutputStdout_);

    kumirPrintScalarVariable_ = stdlibModule_->getFunction("__kumir_print_scalar_variable");
    kumirInputScalarVariable_ = stdlibModule_->getFunction("__kumir_input_scalar_variable");
    kumirPrintArrayVariable_ = stdlibModule_->getFunction("__kumir_print_array_variable");;
    kumirInputArrayVariable_ = stdlibModule_->getFunction("__kumir_input_array_variable");

    Q_ASSERT(kumirPrintScalarVariable_);
    Q_ASSERT(kumirInputScalarVariable_);
    Q_ASSERT(kumirPrintArrayVariable_);
    Q_ASSERT(kumirInputArrayVariable_);

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

    kumirGetScalarArgument_ = stdlibModule_->getFunction("__kumir_get_scalar_argument");
    Q_ASSERT(kumirGetScalarArgument_);

    kumirGetArrayArgument_ = stdlibModule_->getFunction("__kumir_get_array_argument");
    Q_ASSERT(kumirGetArrayArgument_);

    kumirAssert_ = stdlibModule_->getFunction("__kumir_assert");
    Q_ASSERT(kumirAssert_);

    kumirAbortOnError_ = stdlibModule_->getFunction("__kumir_abort_on_error");
    Q_ASSERT(kumirAbortOnError_);

    kumirSetCurrentLineNumber_ = stdlibModule_->getFunction("__kumir_set_current_line_number");
    Q_ASSERT(kumirSetCurrentLineNumber_);

    kumirCheckValueDefined_ = stdlibModule_->getFunction("__kumir_check_value_defined");
    Q_ASSERT(kumirCheckValueDefined_);

    kumirHalt_ = stdlibModule_->getFunction("__kumir_halt");
    Q_ASSERT(kumirHalt_);

    kumirCheckCallStack_ = stdlibModule_->getFunction("__kumir_check_call_stack");
    kumirPopCallStackCounter_ = stdlibModule_->getFunction("__kumir_pop_call_stack_counter");
    Q_ASSERT(kumirCheckCallStack_);
    Q_ASSERT(kumirPopCallStackCounter_);

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

    kumirGetArray1Element_ = stdlibModule_->getFunction("__kumir_get_array_1_element");
    Q_ASSERT(kumirGetArray1Element_);    

    kumirGetArray2Element_ = stdlibModule_->getFunction("__kumir_get_array_2_element");
    Q_ASSERT(kumirGetArray2Element_);

    kumirGetArray3Element_ = stdlibModule_->getFunction("__kumir_get_array_3_element");
    Q_ASSERT(kumirGetArray3Element_);

    kumirGetStringElement_ = stdlibModule_->getFunction("__kumir_get_string_element");
    Q_ASSERT(kumirGetStringElement_);

    kumirGetStringSlice_ = stdlibModule_->getFunction("__kumir_get_string_slice");
    Q_ASSERT(kumirGetStringSlice_);

    kumirGetStringElementRef_ = stdlibModule_->getFunction("__kumir_get_string_element_ref");
    Q_ASSERT(kumirGetStringElementRef_);

    kumirGetStringSliceRef_ = stdlibModule_->getFunction("__kumir_get_string_slice_ref");
    Q_ASSERT(kumirGetStringSliceRef_);

    kumirLoopForFromToInitCounter_ = stdlibModule_->getFunction("__kumir_loop_for_from_to_init_counter");
    kumirLoopForFromToStepInitCounter_ = stdlibModule_->getFunction("__kumir_loop_for_from_to_step_init_counter");
    kumirLoopForCheckCounter_ = stdlibModule_->getFunction("__kumir_loop_for_check_counter");
    kumirLoopTimesInitCounter_ = stdlibModule_->getFunction("__kumir_loop_times_init_counter");
    kumirLoopTimesCheckCounter_ = stdlibModule_->getFunction("__kumir_loop_times_check_counter");
    kumirLoopEndCounter_ = stdlibModule_->getFunction("__kumir_loop_end_counter");
    Q_ASSERT(kumirLoopForFromToInitCounter_);
    Q_ASSERT(kumirLoopForFromToStepInitCounter_);
    Q_ASSERT(kumirLoopForCheckCounter_);
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
