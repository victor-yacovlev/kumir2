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
    for (int i=0; i<kmod->impl.globals.size(); i++) {
        const AST::VariablePtr glob = kmod->impl.globals.at(i);
        addGlobalVariable(glob, false);
    }

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

void LLVMGenerator::addGlobalVariable(const AST::VariablePtr kvar, bool constant)
{
    Q_ASSERT(context_);
    Q_ASSERT(currentModule_);

    CString name = nameTranslator_->add(kvar->name);
    llvm::Type * ty = findOrRegisterType(kvar->baseType, false, kvar->dimension);
    llvm::Constant * initialValue = nullptr;

    llvm::GlobalVariable * lvar = new llvm::GlobalVariable(
                *currentModule_,
                ty,
                constant,
                llvm::GlobalValue::ExternalLinkage,
                initialValue,
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

        llvm::BasicBlock * functionBlock =
                llvm::BasicBlock::Create(*context_, "", lfn);

        currentBlock_.push_back(functionBlock);

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

        llvm::IRBuilder<> builder(functionBlock);

        addFunctionBody(kfunc->impl.pre, kfunc);
        addFunctionBody(kfunc->impl.body, kfunc);
        addFunctionBody(kfunc->impl.post, kfunc);


        builder.CreateRetVoid();

        currentBlock_.pop_back();
        nameTranslator_->endNamespace();
    }
}

void LLVMGenerator::addFunctionBody(const QList<AST::StatementPtr> & statements, const AST::AlgorithmPtr & alg)
{
    Q_ASSERT(currentBlock_.size() == 1);
    llvm::BasicBlock * block = currentBlock_.back();
    Q_ASSERT(block);
    llvm::IRBuilder<> builder(block);

    for (int i=0; i<statements.size(); i++) {
        const AST::StatementPtr & statement = statements.at(i);
        const AST::StatementType type = statement->type;
        switch (type) {
        case AST::StVarInitialize:
            createVarInitialize(builder, statement);
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
        default:
            break;
        }
    }
}

void LLVMGenerator::createVarInitialize(llvm::IRBuilder<> &builder, const AST::StatementPtr &st)
{
    for (int i=0; i<st->variables.size(); i++) {
        const AST::VariablePtr var = st->variables.at(i);
        if (var->accessType == AST::AccessRegular) {
            createVarInitialize(builder, var, "");
        }
    }
}

llvm::Value* LLVMGenerator::createVarInitialize(llvm::IRBuilder<> &builder, const AST::VariablePtr & var, const QString & overrideName)
{
    llvm::Type * ty = findOrRegisterType(var->baseType, false, var->dimension);
    const CString name = nameTranslator_->add(
                overrideName.isEmpty() ? var->name : overrideName
                                         );
    llvm::AllocaInst * alloca = builder.CreateAlloca(ty, 0, name);
    Q_ASSERT(alloca);
    llvm::Function * initFunc = 0;
    std::vector<llvm::Value*> initArgs(1u + 2u*var->dimension);
    initArgs[0] = alloca;
    if (var->dimension == 0u) {
        llvm::Function * kumirInitScalar = stdlibModule_->getFunction("__kumir_init_scalar");
        Q_ASSERT(kumirInitScalar);
        initFunc = kumirInitScalar;
    }
    llvm::CallInst * initCall = builder.CreateCall(initFunc, initArgs);
    Q_ASSERT(initCall);
    return alloca;
}

void LLVMGenerator::createAssign(llvm::IRBuilder<> &builder, const AST::StatementPtr &st, const AST::AlgorithmPtr & alg)
{
    const AST::ExpressionPtr rvalue = st->expressions.at(0);
    llvm::Value * llvm_rvalue = calculate(builder, rvalue, false);
    if (st->expressions.size() > 1) {
        const AST::ExpressionPtr lvalue = st->expressions.at(1);
        if (lvalue->kind == AST::ExprVariable) {
            llvm::Value * var = 0;
            if (lvalue->variable->name == alg->header.name) {
                // return value
                llvm::Argument& firstArg = currentFunction_->getArgumentList().front();
                // function retval pointer is passed via 1-st argument
                var = & firstArg;
            }
            else {
                // local or global variable
                const CString varName = nameTranslator_->find(lvalue->variable->name);
                Q_ASSERT(varName.length() > 0);
                var = currentBlock_.last()->getValueSymbolTable()->lookup(varName);
            }
            Q_ASSERT(var);
            builder.CreateCall2(kumirAssignScalarToScalar_, var, llvm_rvalue);
        }
    }
    createFreeTempScalars(builder);
}

void LLVMGenerator::createAssert(llvm::IRBuilder<> &builder, const AST::StatementPtr &st, const AST::AlgorithmPtr &alg)
{
    for (int i=0; i<st->expressions.size(); i++) {
        const AST::ExpressionPtr & expr = st->expressions[i];
        llvm::Value * assertionValue = calculate(builder, expr, false);
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
        fileHandle = calculate(builder, st->expressions.last(), false);
        Q_ASSERT(fileHandle);
    }
    for (int i=0; i<lexemsCount; i++) {
        int exprIndex = 3 * i;
        int format1Index = 3 * i + 1;
        int format2Index = 3 * i + 2;
        const AST::ExpressionPtr & expr = st->expressions.at(exprIndex);
        const AST::ExpressionPtr & format1 = st->expressions.at(format1Index);
        const AST::ExpressionPtr & format2 = st->expressions.at(format2Index);

        llvm::Value * lexpr = calculate(builder, expr, false);
        createFreeTempScalars(builder);

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

void LLVMGenerator::createFreeTempScalars(llvm::IRBuilder<> &builder)
{
    typedef std::list<llvm::Value*>::iterator It;
    for (It it = tempValsToFree_.begin(); it != tempValsToFree_.end(); ++it) {
        builder.CreateCall(kumirFreeScalar_, *it);
    }
    tempValsToFree_.clear();
}

llvm::Value * LLVMGenerator::calculate(llvm::IRBuilder<> &builder, const AST::ExpressionPtr &ex, bool lvalue)
{
    llvm::Value * result = 0;
    if (ex->kind == AST::ExprConst && ex->dimension==0u && ex->baseType.kind != AST::TypeUser) {
        result = createConstant(builder, ex->baseType, ex->constant);
    }
    else if (ex->kind == AST::ExprVariable) {
        CString varName = nameTranslator_->find(ex->variable->name);
        Q_ASSERT(varName.length() > 0);
        llvm::Value * var = currentBlock_.last()->getValueSymbolTable()->lookup(varName);
        Q_ASSERT(var);
        return var;
    }
    else if (ex->kind == AST::ExprArrayElement) {

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
        llvm::Value * arg = calculate(builder, argExpr, false);
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
    std::vector<llvm::Value*> operands(ex->operands.size());

    for (int i=0; i<ex->operands.size(); i++) {
        llvm::Value * operand = calculate(builder, ex->operands[i], false);
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
    default:
        break;
    }
    if (1u == operandsCount) {
        builder.CreateCall2(opFunc, result, operands[0]);
    }
    else if (2u == operandsCount) {
        builder.CreateCall3(opFunc, result, operands[0], operands[1]);
    }
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

    qFatal("Not implemented"); // TODO implement non-scalar types
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

    kumirInitScalar_ = stdlibModule_->getFunction("__kumir_init_scalar");
    Q_ASSERT(kumirInitScalar_);

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
