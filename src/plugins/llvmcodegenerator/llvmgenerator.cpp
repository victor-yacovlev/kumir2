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

#include <llvm/IR/Type.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Attributes.h>


#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Linker/Linker.h>

#include <string>
#include <iostream>
#include <assert.h>
#include <algorithm>


namespace LLVMCodeGenerator {


LLVMGenerator::LLVMGenerator()
    : currentFunctionEntry_(0)
    , functionEntryPoint_(0)
    , context_(0)
    , nameTranslator_(new NameTranslator)
{

}

void LLVMGenerator::initialize(const QDir &resourcesRoot)
{
    const QString StdLibFileName = resourcesRoot.absoluteFilePath("stdlib.bc");

    QFile StdLibFile(StdLibFileName);
    if (!StdLibFile.open(QIODevice::ReadOnly)) {
        const QString message = QString::fromLatin1("Can't open %1")
                .arg(StdLibFileName);
        qFatal("%s", message.toStdString().c_str());
    }

    stdlibContents_ = StdLibFile.readAll();
    StdLibFile.close();
}


void LLVMGenerator::reset(bool addMainEntryPoint,
                          Shared::GeneratorInterface::DebugLevel debugLevel)
{
    functMap_.clear();
    externs_.clear();
    debugLevel_ = debugLevel;
    context_ = new llvm::LLVMContext();
    currentModule_ = std::move(LLVM::ModuleRef::create(context_));
    currentFunction_ = LLVM::FunctionRef();
    addMainEntryPoint_ = addMainEntryPoint;
    nameTranslator_->reset();
    createStdLibModule();
    createExternsTable(stdlibModule_, "__kumir_");
    externs_.clear();
    readStdLibFunctions();
    calculateCache_.clear();
    lastLineNumber_ = 0;
}

void LLVMGenerator::createStdLibModule()
{
    stdlibModule_ = LLVM::ModuleRef::fromBinaryRepresentation(
                stdlibContents_,
                "",
                context_
                );
}

LLVM::ModuleRef& LLVMGenerator::getResult()
{
    return currentModule_;
}

LLVM::ModuleRef& LLVMGenerator::getStdLibModule()
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
    const QByteArray moduleName = QString(
                kmod->header.name.isEmpty() ? kmod->header.sourceFileName
                                            : kmod->header.name
                ).toUtf8();
    nameTranslator_->beginNamespace();

    LLVM::ModuleRef & lmod = currentModule_;

    LLVM::FunctionRef initFunc = LLVM::FunctionRef::create(
                LLVM::TypeRef::getGeneric(LLVM::TypeRef::TY_Void, context_),
                LLVM::FunctionRef::VS_Extern,
                "__init__" + moduleName,
                QVector<LLVM::TypeRef>(),
                QVector<QByteArray>(),
                false,
                currentModule_,
                context_
                );

    currentFunction_ = initFunc;
    initFunctions_.push_back(initFunc);

    tempValsToFree_.clear();
    llvm::BasicBlock * initEntry = llvm::BasicBlock::Create(*context_, "__init__entry", (llvm::Function*) initFunc.rawPtr());
    llvm::BasicBlock * initBody = llvm::BasicBlock::Create(*context_, "__init__body", (llvm::Function*) initFunc.rawPtr());
    currentBlock_ = initEntry;
    currentFunctionEntry_ = initEntry;
    llvm::IRBuilder<> initBuilder(initEntry);
    initBuilder.CreateBr(initBody);
    currentBlock_ = initBody;
    initBuilder.SetInsertPoint(initBody);

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

    QByteArray name = nameTranslator_->add(qn);
    LLVM::TypeRef ty = kvar->dimension > 0u
            ? getArrayType() : getScalarType();

    llvm::GlobalVariable * lvar = new llvm::GlobalVariable(
                *((llvm::Module*)currentModule_.rawPtr()),
                (llvm::Type*) ty.rawPtr(),
                constant,
                llvm::GlobalValue::PrivateLinkage,
                llvm::ConstantStruct::getNullValue((llvm::Type*) ty.rawPtr()),
                std::string(name.constData())
                );

    Q_ASSERT(lvar);

}



void LLVMGenerator::createMainFunction(const AST::AlgorithmPtr &entryPoint)
{   
    QVector<LLVM::TypeRef> mainArgs(2);
    mainArgs[0] = LLVM::TypeRef::getGeneric(LLVM::TypeRef::TY_Int, context_);
    mainArgs[1] = LLVM::TypeRef::getGeneric(LLVM::TypeRef::TY_CStr, context_).pointerTo();

    QVector<QByteArray> mainArgNames(2);
    mainArgNames[0] = "argc";
    mainArgNames[1] = "argv";

    LLVM::FunctionRef lfn =
            LLVM::FunctionRef::create(
                LLVM::TypeRef::getGeneric(LLVM::TypeRef::TY_Int, context_),
                LLVM::FunctionRef::VS_Extern,
                "main",
                mainArgs,
                mainArgNames,
                false,
                currentModule_,
                context_
                );
    currentFunction_ = lfn;
    nameTranslator_->beginNamespace();
    llvm::BasicBlock * functionBlock =
            llvm::BasicBlock::Create(*context_, "__main__body:", (llvm::Function*)lfn.rawPtr());
    llvm::IRBuilder<> builder(functionBlock);

    QVector<llvm::Value*> mainArgValues;
    mainArgValues.reserve(2);
    llvm::Function::ArgumentListType & argList = lfn.rawPtr()->getArgumentList();
    typedef llvm::Function::ArgumentListType::iterator AIt;
    for (AIt it=argList.begin(); it!=argList.end(); ++it) {
        llvm::Argument & arg = *it;
        llvm::Value* argVal = &arg;
        mainArgValues.append(argVal);
    }

    kumirSetMainArguments_.createCallInstruction(&builder, mainArgValues);
    kumirInitStdLib_.createCallInstruction(&builder);

    Q_FOREACH(LLVM::FunctionRef initFunc, initFunctions_) {
        initFunc.createCallInstruction(&builder);
    }


    if (entryPoint) {
        AST::LexemPtr firstAlgLexem = entryPoint->impl.headerLexems.first();
        int headerLineNo = firstAlgLexem->lineNo + 1;
        const QString entryPointQName = "__kumir_function_" + entryPoint->header.name;
        const QByteArray entryPointName = NameTranslator::screenUtf8Name(entryPointQName);

        Q_ASSERT(entryPointName.length() > 0);

        LLVM::FunctionRef entryPointFunc = functMap_.value(entryPoint);
        Q_ASSERT(entryPointFunc);

        QVector<llvm::Value*> args;

        llvm::Value * ret = 0;

        if (entryPoint->header.returnType != AST::TypeNone) {
            ret = builder.CreateAlloca((llvm::Type*)getScalarType().rawPtr(), 0, "__main__return_value__");
            args.push_back(ret);
        }

//        llvm::raw_os_ostream debug(std::cerr);
//        currentModule_->print(debug, 0);

        for (int i=0; i<entryPoint->header.arguments.size(); i++) {
            const AST::VariablePtr & arg = entryPoint->header.arguments.at(i);
            const QByteArray argName = nameTranslator_->add(arg->name);
            llvm::Value * larg = builder.CreateAlloca( (llvm::Type*)
                        (arg->dimension > 0u ? getArrayType().rawPtr() : getScalarType().rawPtr()), 0,
                                              std::string(argName.constData())
                        );
            args.push_back(larg);
            if (1u == arg->dimension) {
                llvm::Value * x_start = calculate(builder, arg->bounds[0].first);
                llvm::Value * x_end = calculate(builder, arg->bounds[0].second);
                QVector<llvm::Value*> arrArgs(3);
                arrArgs[0] = larg;
                arrArgs[1] = x_start;
                arrArgs[2] = x_end;
                kumirCreateArray1_.createCallInstruction(&builder, arrArgs);
            }
            if (arg->accessType!=AST::AccessArgumentOut) {
                lastLineNumber_ = headerLineNo;
                QVector<llvm::Value*> slnArgs(1);
                slnArgs[0] = llvm::ConstantInt::getSigned(
                            llvm::Type::getInt32Ty(*context_),
                            headerLineNo);
                kumirSetCurrentLineNumber_.createCallInstruction(&builder, slnArgs);
                createInputValue(builder, arg->name, larg, arg->baseType.kind, arg->dimension > 0u);
            }
        }        

        entryPointFunc.createCallInstruction(&builder, args);

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
                    llvm::Type::getInt32Ty(*context_), typee
                    );

        QVector<llvm::Value*> printArgs(3);
        printArgs[0] = lname;
        printArgs[1] = ltype;
        printArgs[2] = value;
        kumirPrintScalarVariable_.createCallInstruction(&builder, printArgs);
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
                llvm::Type::getInt32Ty(*context_), typee
                );

    QVector<llvm::Value*> getArgs(3);
    getArgs[0] = lname;
    getArgs[1] = ltype;
    getArgs[2] = value;

    if (!isArray) {
        kumirGetScalarArgument_.createCallInstruction(&builder, getArgs);
    }
    else {
        kumirGetArrayArgument_.createCallInstruction(&builder, getArgs);
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
    QByteArray name = NameTranslator::screenUtf8Name(actualName);


    size_t largsCount = 0u;
    size_t largsOffset = 0u;

    if (kfunc->header.returnType == AST::TypeNone) {
        largsCount = kfunc->header.arguments.size();
    }
    else {
        largsCount = kfunc->header.arguments.size() + 1;
        largsOffset = 1u;
    }

    QVector<LLVM::TypeRef> argTypes(largsCount);
    QVector<QByteArray> argNames(largsCount);

    nameTranslator_->beginNamespace();
    for (size_t i=0; i<kfunc->header.arguments.size(); i++) {
        const AST::VariablePtr kvar = kfunc->header.arguments.at(i);
        LLVM::TypeRef lvar = kvar->dimension > 0u
                ? getArrayType().pointerTo()
                : getScalarType().pointerTo();
        argTypes[i+largsOffset] = lvar;
        argNames[i+largsOffset] = NameTranslator::screenUtf8Name(kvar->name);
    }

    if (kfunc->header.returnType != AST::TypeNone) {
        argTypes[0] = getScalarType().pointerTo();
        argNames[0] = "__retval__"+name;
    }
    nameTranslator_->endNamespace();

    LLVM::FunctionRef fn;

    if (createBody) {
        if (!functMap_.contains(kfunc)) {
            return; // Algorithm is broken
        }
        fn = functMap_.value(kfunc);
    }
    else {
        LLVM::FunctionRef::Visibility link =
                kfunc->header.name.isEmpty() || kfunc->header.name.startsWith("_")
                ? LLVM::FunctionRef::VS_Static
                : LLVM::FunctionRef::VS_Extern
                  ;
        fn = LLVM::FunctionRef::create(
                        LLVM::TypeRef::getGeneric(LLVM::TypeRef::TY_Void, context_),
                        link,
                        name,
                        argTypes,
                        argNames,
                        kfunc->header.returnType != AST::TypeNone,
                        currentModule_,
                        context_
                        );

        functMap_.insert(kfunc, fn);
    }

    if (createBody) {
        tempValsToFree_.clear();
        nameTranslator_->beginNamespace();
        currentFunction_ = fn;

        llvm::BasicBlock * functionEntry =
                llvm::BasicBlock::Create(*context_, "function_entry", (llvm::Function*) fn.rawPtr());
        llvm::BasicBlock * functionPre =
                llvm::BasicBlock::Create(*context_, "function_pre", (llvm::Function*) fn.rawPtr());
        llvm::BasicBlock * functionBody =
                llvm::BasicBlock::Create(*context_, "function_body", (llvm::Function*) fn.rawPtr());
        llvm::BasicBlock * functionPost =
                llvm::BasicBlock::Create(*context_, "function_post", (llvm::Function*) fn.rawPtr());
        llvm::BasicBlock * functionExit =
                llvm::BasicBlock::Create(*context_, "function_exit", (llvm::Function*) fn.rawPtr());

        currentFunctionEntry_ = functionEntry;
        functionEntryPoint_ = functionEntry;

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

        llvm::Function* lfn = (llvm::Function*) fn.rawPtr();
        llvm::Function::ArgumentListType::iterator lIt = lfn->getArgumentList().begin();
        std::list<AST::VariablePtr>::iterator kIt = namedArgs.begin();

        llvm::Argument * returnValue = 0;

        if (kfunc->header.returnType != AST::TypeNone) {
            llvm::Argument & larg = *lIt;
            larg.setName("__retval__" + lfn->getName());
            lIt ++;
            returnValue = &larg;

            QVector<llvm::Value*> cusArgs(1);
            cusArgs[0] = returnValue;
            kumirCreateUndefinedScalar_.createCallInstruction(&builder, cusArgs);
        }

        std::vector<llvm::Value*> argumentValues;

        for ( ; lIt != lfn->getArgumentList().end() && kIt != namedArgs.end() ; ++lIt, ++kIt)
        {
            llvm::Argument & larg = *lIt;
            const AST::VariablePtr & karg = *kIt;
            const QString kArgName = karg->name;
            const QByteArray lArgName = nameTranslator_->add(kArgName);
            larg.setName(std::string(lArgName.constData()));
            argumentValues.push_back(&larg);
        }

        // Stack-allocate local variables
        for (int i=0; i<kfunc->impl.locals.size(); i++) {
            const AST::VariablePtr & kvar = kfunc->impl.locals[i];
            if (kvar->accessType == AST::AccessRegular && kvar->name != kfunc->header.name) {
                const QString qname = kvar->name;
                const QByteArray cname = nameTranslator_->add(qname);
                Q_ASSERT(!cname.isEmpty());
                LLVM::TypeRef ty = kvar->dimension > 0u
                        ? getArrayType() : getScalarType();
                llvm::Value * arg = CreateAlloca(builder, ty, cname, BeforeTerminator);
                LLVM::FunctionRef initFunc = kvar->dimension == 0u
                        ? kumirCreateUndefinedScalar_ : kumirCreateUndefinedArray_;
                initFunc.createCallInstruction(&builder, arg);
            }
        }

        // Initialize array arguments and wash-up out-parameters
        for (int i=0; i<kfunc->header.arguments.size(); i++) {
            const AST::VariablePtr & karg = kfunc->header.arguments[i];
            if (karg->dimension > 0u) {
                llvm::Value * lvar = argumentValues[i];
                LLVM::FunctionRef initFunc;
                QVector<llvm::Value*> initArgs;
                initArgs.reserve(20);
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
                initFunc.createCallInstruction(&builder, initArgs);
                if (karg->accessType == AST::AccessArgumentOut && karg->dimension > 0u) {
                    kumirCleanUpArrayInShape_.createCallInstruction(&builder, lvar);
                }
            }
            else if (karg->accessType == AST::AccessArgumentOut) {
                llvm::Value * lvar = argumentValues[i];
                kumirFreeScalar_.createCallInstruction(&builder, lvar);
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
                    kumirFreeArray_.createCallInstruction(&builder, lvar);
                }
                else if (kvar->baseType.kind == AST::TypeString ||
                         kvar->baseType.kind == AST::TypeUser)
                {
                    if (kvar->accessType != AST::AccessArgumentIn) {
                        llvm::Value * lvar = findVariableAtCurrentContext(kvar);
                        Q_ASSERT(lvar);
                        kumirFreeScalar_.createCallInstruction(&builder, lvar);
                    }
                }
            }

        }
        QString endError;
        int endErrorLine = -1;
        for (int i=0; i<kfunc->impl.endLexems.size(); i++) {
            AST::LexemPtr lx = kfunc->impl.endLexems.at(i);
            if (lx->error.length() > 0) {
                endErrorLine = lx->lineNo + 1;
                endError = Shared::ErrorMessages::message("KumirAnalizer", QLocale::Russian, lx->error);
                break;
            }
        }
        if (-1 != endErrorLine) {
            llvm::Value* lval = llvm::ConstantInt::getSigned(
                        llvm::Type::getInt32Ty(*context_),
                        endErrorLine);
            kumirSetCurrentLineNumber_.createCallInstruction(&builder, lval);
        }
        if (endError.length() > 0) {
            llvm::Value* lval = builder.CreateGlobalStringPtr(
                        std::string(
                            Shared::ErrorMessages::message(
                                "KumirAnalizer",
                                QLocale::Russian,
                                 endError
                                ).toUtf8().data()
                            )
                        );
            kumirAbortOnError_.createCallInstruction(&builder, lval);
        }
        kumirPopCallStackCounter_.createCallInstruction(&builder);
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

        if (headerError.isEmpty() && alg->impl.headerRuntimeError.length() > 0) {
            headerError = Shared::ErrorMessages::message("KumirAnalizer", QLocale::Russian, alg->impl.headerRuntimeError);
            headerLine = alg->impl.headerRuntimeErrorLine + 1;
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
                lastLineNumber_ = errLine;
                llvm::Value* lval = llvm::ConstantInt::getSigned(
                            llvm::Type::getInt32Ty(*context_),
                            errLine);
                kumirSetCurrentLineNumber_.createCallInstruction(&builder, lval);
            }
            const std::string error = std::string(err.toUtf8().data());
            llvm::Value * arg = builder.CreateGlobalStringPtr(error);
            Q_ASSERT(arg);
            Q_ASSERT(kumirAbortOnError_);
            kumirAbortOnError_.createCallInstruction(&builder, arg);
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
            lastLineNumber_ = lineNo;
            Q_ASSERT(kumirSetCurrentLineNumber_);
            if (debugLevel_ != Shared::GeneratorInterface::NoDebug) {
                llvm::Value* lval = llvm::ConstantInt::getSigned(
                            llvm::Type::getInt32Ty(*context_),
                            lineNo);
                kumirSetCurrentLineNumber_.createCallInstruction(&builder, lval);
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
    LLVM::TypeRef ty = var->dimension > 0u
            ? getArrayType() : getScalarType();
    QByteArray name;
    llvm::Value * result = 0;
    if (overrideName.length() == 0) {
        result = findVariableAtCurrentContext(var);
        name = QByteArray(result->getName().begin(), result->getName().size());
    }
    else {
        name = nameTranslator_->add(overrideName);
        Q_ASSERT(!name.isEmpty());
        result = CreateAlloca(builder, ty, name, BeforeTerminator);
    }
    Q_ASSERT(result);
    LLVM::FunctionRef initFunc;
    LLVM::FunctionRef fillFunc;
    QVector<llvm::Value*> initArgs;
    llvm::Value * fillValue = 0;
    initArgs.push_back(result);
    if (var->dimension == 0u) {
        if (var->initialValue.isValid()) {
            initFunc = kumirCreateDefinedScalar_;
            llvm::Value * cval = createConstant(builder, var->baseType, var->initialValue, AsIs);
            Q_ASSERT(cval);
            initArgs.push_back(cval);
        }
    }
    else {
        LLVM::FunctionRef kumirInitArray;
        if      (1u == var->dimension) kumirInitArray = kumirCreateArray1_;
        else if (2u == var->dimension) kumirInitArray = kumirCreateArray2_;
        else if (3u == var->dimension) kumirInitArray = kumirCreateArray3_;
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
            fillValue = createArrayConstant(builder, var->baseType.kind, var->dimension, var->initialValue);
            Q_ASSERT(fillValue);
        }
    }
    if (initFunc) {
        initFunc.createCallInstruction(&builder, initArgs);
        if (fillFunc) {
            Q_ASSERT(fillValue);
            fillFunc.createCallInstruction(&builder, result, fillValue);
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
        LLVM::FunctionRef storeFunc;
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
        storeFunc.createCallInstruction(&builder, llvm_lvalue, llvm_rvalue);
    }
    createFreeTempScalars(builder);
}

void LLVMGenerator::createAssert(llvm::IRBuilder<> &builder, const AST::StatementPtr &st, const AST::AlgorithmPtr &alg)
{
    for (int i=0; i<st->expressions.size(); i++) {
        const AST::ExpressionPtr & expr = st->expressions[i];
        llvm::Value * assertionValue = calculate(builder, expr);
        Q_ASSERT(assertionValue);
        kumirAssert_.createCallInstruction(&builder, assertionValue);
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
        kumirCheckValueDefined_.createCallInstruction(&builder, fileHandle);
        Q_ASSERT(fileHandle);
    }
    QVector<llvm::Value *> values;
    QVector<llvm::Value *> types;
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
                    llvm::Type::getInt32Ty(*context_), typee
                    );

        types.push_back(ltype);

        llvm::Value * lexpr = calculate(builder, expr);
        kumirCheckValueDefined_.createCallInstruction(&builder, lexpr);
        values.push_back(lexpr);

        LLVM::FunctionRef outFunc;
        QVector<llvm::Value*> args;
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
        LLVM::FunctionRef outFunc;
        QVector<llvm::Value*> args;
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
        outFunc.createCallInstruction(&builder, args);
    }

    createFreeTempScalars(builder);
}

void LLVMGenerator::createHalt(llvm::IRBuilder<> &builder, const AST::StatementPtr &, const AST::AlgorithmPtr &)
{
    kumirHalt_.createCallInstruction(&builder);
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
        QVector<llvm::Value*> args;
        if (fileHandle) {
            args.push_back(fileHandle);
        }
        uint32_t fmt = 0x00;
        if      (AST::TypeInteger   == varExpr->variable->baseType.kind)   fmt = 0x01;
        else if (AST::TypeReal      == varExpr->variable->baseType.kind)   fmt = 0x02;
        else if (AST::TypeBoolean   == varExpr->variable->baseType.kind)   fmt = 0x03;
        else if (AST::TypeCharect   == varExpr->variable->baseType.kind)   fmt = 0x04;
        else if (AST::TypeString    == varExpr->variable->baseType.kind)   fmt = 0x05;
        args.push_back(llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*context_), fmt));
        llvm::Value * ptr = 0;
        LLVM::TypeRef ptrTy = getScalarType().pointerTo();
        if (varExpr->kind == AST::ExprVariable) {
            llvm::Value * var = findVariableAtCurrentContext(varExpr->variable);
            Q_ASSERT(var);
            ptr = CreateAlloca(builder, ptrTy, "", BeforeTerminator);
            builder.CreateStore(var, ptr);
        }
        else if (varExpr->kind == AST::ExprArrayElement) {
            ptr = calculate(builder, varExpr, true);
        }
        args.push_back(ptr);
        if (fileHandle) {
            kumirInputFile_.createCallInstruction(&builder, args);                    }
        else {
            kumirInputStdin_.createCallInstruction(&builder, args);
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
            lastLineNumber_ = errNo;
            llvm::Value* lval = llvm::ConstantInt::getSigned(
                        llvm::Type::getInt32Ty(*context_),
                        errNo
                        );
            kumirSetCurrentLineNumber_.createCallInstruction(&builder, lval);
        }
        llvm::Value* lerrval = builder.CreateGlobalStringPtr(
                    std::string(
                        Shared::ErrorMessages::message(
                            "KumirAnalizer", QLocale::Russian,
                            err
                            )
                        .toUtf8().constData()
                        )
                    );
        kumirAbortOnError_.createCallInstruction(&builder, lerrval);
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
    const QByteArray basicName = QString("loop_%1_").arg(loopCounter_).toLatin1();

    llvm::BasicBlock * loop_init =
            llvm::BasicBlock::Create(*context_,
                                     std::string(basicName + "init"),
                                     currentFunction_.rawPtr());

    llvm::BasicBlock * loop_begin =
            llvm::BasicBlock::Create(*context_,
                                     std::string(basicName + "begin"),
                                     currentFunction_.rawPtr());
    llvm::BasicBlock * loop_body =
            llvm::BasicBlock::Create(*context_,
                                     std::string(basicName + "body"),
                                     currentFunction_.rawPtr());
    llvm::BasicBlock * loop_end =
            llvm::BasicBlock::Create(*context_,
                                     std::string(basicName + "end"),
                                     currentFunction_.rawPtr());
    llvm::BasicBlock * loop_clean =
            llvm::BasicBlock::Create(*context_,
                                     std::string(basicName + "clean"),
                                     currentFunction_.rawPtr());
    llvm::BasicBlock * loop_done =
            llvm::BasicBlock::Create(*context_,
                                     std::string(basicName + "done"),
                                     currentFunction_.rawPtr());

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
            for_from->setName(std::string(basicName + "from"));
        for_to = calculate(builder, loop.toValue);
        Q_ASSERT(for_to);
        if (loop.toValue->kind != AST::ExprVariable)
            for_to->setName(std::string(basicName + "to"));
        if (loop.stepValue) {
            for_step = calculate(builder, loop.stepValue);
            Q_ASSERT(for_step);
            if (loop.stepValue->kind != AST::ExprVariable)
                for_step->setName(std::string(basicName + "step"));
            Q_ASSERT(kumirLoopForFromToStepInitCounter_);
            kumirLoopForFromToStepInitCounter_.createCallInstruction(&builder, for_from, for_to, for_step);
        }
        else {
            Q_ASSERT(kumirLoopForFromToInitCounter_);
            kumirLoopForFromToInitCounter_.createCallInstruction(&builder, for_from, for_to);
        }
    }
    else if (loop.type == AST::LoopTimes) {
        times = calculate(builder, loop.timesValue);
        Q_ASSERT(times);
        Q_ASSERT(kumirLoopTimesInitCounter_);
        kumirLoopTimesInitCounter_.createCallInstruction(&builder, times);
    }
    createFreeTempScalars(builder);
    builder.CreateBr(loop_begin);

    // --- loop pre-check
    builder.SetInsertPoint(loop_begin);
    llvm::Value * loop_pre_check = 0;
    if (loop.type == AST::LoopFor && loop_variable && for_from && for_to) {
        loop_pre_check = kumirLoopForCheckCounter_.createCallInstruction(&builder, loop_variable);
        loop_pre_check->setName(std::string(basicName + "for_check"));
    }
    else if (loop.type == AST::LoopTimes) {
        Q_ASSERT(kumirLoopTimesCheckCounter_);
        loop_pre_check = kumirLoopTimesCheckCounter_.createCallInstruction(&builder);
        loop_pre_check->setName(std::string(basicName + "times_check"));
    }
    else if (loop.type == AST::LoopWhile && loop.whileCondition) {
        Q_ASSERT(kumirScalarAsBool_);
        llvm::Value* lcond = calculate(builder, loop.whileCondition);
        loop_pre_check = kumirScalarAsBool_.createCallInstruction(&builder, lcond);
        loop_pre_check->setName(std::string(basicName + "while_check"));
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
        if (loop.endCondition->kind != AST::ExprVariable) {
            endCond->setName(std::string(basicName + "end_condition"));
        }
        Q_ASSERT(endCond);
        Q_ASSERT(kumirScalarAsBool_);
        llvm::CallInst * endBoolCondInst = kumirScalarAsBool_.createCallInstruction(&builder, endCond);
        endBoolCondInst->setName(std::string(basicName+"end_condition_boolean"));
        llvm::Value * endBoolCond = endBoolCondInst;
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
                lastLineNumber_ = endErrLine;
                llvm::Value* lval = llvm::ConstantInt::getSigned(
                            llvm::Type::getInt32Ty(*context_),
                            endErrLine
                            );
                kumirSetCurrentLineNumber_.createCallInstruction(&builder, lval);
            }
            llvm::Value* lerr = builder.CreateGlobalStringPtr(
                        std::string(
                            Shared::ErrorMessages::message(
                                "KumirAnalizer", QLocale::Russian,
                                endErr
                                )
                            .toUtf8().constData()
                            )
                        );
            kumirAbortOnError_.createCallInstruction(&builder, lerr);
        }
        createFreeTempScalars(builder);
        builder.CreateBr(loop_begin);
    }

    // --- clean counters if need
    builder.SetInsertPoint(loop_clean);
    if (loop.type == AST::LoopFor || loop.type == AST::LoopTimes) {
        kumirLoopEndCounter_.createCallInstruction(&builder);
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
    QByteArray basicName = QString("if_%1_").arg(ifThenElseCounter_).toLatin1();

    llvm::BasicBlock * then = llvm::BasicBlock::Create(
                *context_,
                std::string(basicName + "then"),
                currentFunction_.rawPtr()
                );
    llvm::BasicBlock * elze = llvm::BasicBlock::Create(
                *context_,
                std::string(basicName + "else"),
                currentFunction_.rawPtr()
                );
    llvm::BasicBlock * done = llvm::BasicBlock::Create(
                *context_,
                std::string(basicName + "done"),
                currentFunction_.rawPtr()
                );

    // --- calculate condition
    Q_ASSERT(thenSpec.condition);
    llvm::Value * cond = calculate(builder, thenSpec.condition);
    Q_ASSERT(cond);
    if (thenSpec.condition->kind != AST::ExprVariable)
        cond->setName(std::string(basicName + "condition"));
    Q_ASSERT(kumirScalarAsBool_);
    llvm::CallInst * condBoolCall = kumirScalarAsBool_.createCallInstruction(&builder, cond);
    condBoolCall->setName(std::string(basicName + "condition_boolean"));
    llvm::Value * condBool = condBoolCall;
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
    if (st->endBlockError.length() > 0) {
        if (st->endBlockLexems.size() > 0) {
            AST::LexemPtr lx = st->endBlockLexems.first();
            int lineNo = lx->lineNo + 1;
            llvm::Value* lval = llvm::ConstantInt::getSigned(
                        llvm::Type::getInt32Ty(*context_),
                        lineNo);
            kumirSetCurrentLineNumber_.createCallInstruction(&builder, lval);
        }
        llvm::Value* lerr = builder.CreateGlobalStringPtr(
                    std::string(
                        Shared::ErrorMessages::message(
                            "KumirAnalizer",
                            QLocale::Russian,
                             st->endBlockError
                            ).toUtf8().data()
                        )
                    );
        kumirAbortOnError_.createCallInstruction(&builder, lerr);
    }
}

void LLVMGenerator::createSwitchCaseElse(llvm::IRBuilder<> &builder, const AST::StatementPtr &st, const AST::AlgorithmPtr &alg)
{
    if (st->headerError.size() > 0) {
        if (debugLevel_ != Shared::GeneratorInterface::NoDebug) {
            lastLineNumber_ = st->headerErrorLine + 1;
            llvm::Value* lval = llvm::ConstantInt::getSigned(
                        llvm::Type::getInt32Ty(*context_),
                        st->headerErrorLine + 1
                        );
            kumirSetCurrentLineNumber_.createCallInstruction(&builder, lval);
        }
        llvm::Value* lerr = builder.CreateGlobalStringPtr(
                    std::string(
                        Shared::ErrorMessages::message(
                            "KumirAnalizer",
                            QLocale::Russian,
                             st->headerError
                            ).toUtf8().data()
                        )
                    );
        kumirAbortOnError_.createCallInstruction(&builder, lerr);
    }

    Q_ASSERT(st->conditionals.size() > 0);

    // --- create program blocks
    switchCaseCounter_ ++;
    QByteArray basicName = QString("switch_%1_").arg(switchCaseCounter_).toLatin1();

    QList<llvm::BasicBlock*> cases;
    QList<llvm::BasicBlock*> caseBodies;
    for (int i=0; i<st->conditionals.size(); i++) {
        const AST::ConditionSpec spec = st->conditionals[i];
        QByteArray caseName = basicName;
        caseName += spec.condition
                ? QString("case_%1").arg(i+1).toLatin1() : QByteArray("else");
        llvm::BasicBlock * caze = llvm::BasicBlock::Create(
                    *context_, std::string(caseName), currentFunction_.rawPtr());
        cases.push_back(caze);
        llvm::BasicBlock * body = llvm::BasicBlock::Create(
                    *context_, std::string(caseName + "_body"), currentFunction_.rawPtr()
                    );
        caseBodies.push_back(body);
    }

    llvm::BasicBlock * done = llvm::BasicBlock::Create(
                *context_,
                std::string(basicName + "done"),
                currentFunction_.rawPtr()
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
        if (spec.conditionError.length() > 0) {
            if (spec.lexems.size() > 0) {
                AST::LexemPtr lx = spec.lexems.first();
                int lineNo = lx->lineNo + 1;
                llvm::Value* lval = llvm::ConstantInt::getSigned(
                            llvm::Type::getInt32Ty(*context_),
                            lineNo);
                kumirSetCurrentLineNumber_.createCallInstruction(&builder, lval);
            }
            llvm::Value* lerr = builder.CreateGlobalStringPtr(
                        std::string(
                            Shared::ErrorMessages::message(
                                "KumirAnalizer",
                                QLocale::Russian,
                                 spec.conditionError
                                ).toUtf8().data()
                            )
                        );
            kumirAbortOnError_.createCallInstruction(&builder, lerr);
        }
        if (spec.condition) {
            llvm::Value * cond = calculate(builder, spec.condition);
            Q_ASSERT(cond);
            if (spec.condition->kind != AST::ExprVariable)
                cond->setName(caze->getName() + "_condition");
            Q_ASSERT(kumirScalarAsBool_);
            llvm::CallInst* condBoolInst =
                    kumirScalarAsBool_.createCallInstruction(&builder, cond);
            condBoolInst->setName(cond->getName() + std::string("_boolean"));
            llvm::Value * condBool = condBoolInst;
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
    kumirAbortOnError_.createCallInstruction(&builder, arg);
}

void LLVMGenerator::createFreeTempScalars(llvm::IRBuilder<> &builder)
{
    size_t from = 0u;
    if (!tempValsToFreeStartPos_.empty()) {
        from = tempValsToFreeStartPos_.top();
    }
    for (size_t i=from; i<tempValsToFree_.size(); i++) {
        kumirFreeScalar_.createCallInstruction(&builder, tempValsToFree_.at(i));
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
        llvm::Argument &firstArg = currentFunction_.rawPtr()->getArgumentList().front();
        var = &firstArg;
    }
    else {
        QByteArray varName = nameTranslator_->find(kvar->name);
        if (varName.isEmpty() && currentKModule_->header.name.length() > 0) {
            varName = nameTranslator_->find(currentKModule_->header.name +
                                            "__" +
                                            kvar->name);
        }
        Q_ASSERT(varName.length() > 0);
        var = currentBlock_->getValueSymbolTable()->lookup(std::string(varName));
        if (!var) {
            var = currentModule_.rawPtr()->getGlobalVariable(std::string(varName), true);
        }
    }
    Q_ASSERT(var);
    return var;
}

llvm::Value * LLVMGenerator::calculate(llvm::IRBuilder<> &builder, const AST::ExpressionPtr &ex, bool isLvalue)
{
    llvm::Value * result = 0;
    if (ex->useFromCache) {
        Q_ASSERT(calculateCache_.contains(ex->cacheReference));
        return calculateCache_.value(ex->cacheReference);
    }
    if (ex->kind == AST::ExprConst && ex->dimension==0u && ex->baseType.kind != AST::TypeUser) {
        result = createConstant(builder, ex->baseType, ex->constant, ex->keepInCache ? FunctionBegin : AsIs);
    }
    else if (ex->kind == AST::ExprVariable) {
        result = findVariableAtCurrentContext(ex->variable);
        if (ex->keepInCache) {
            calculateCache_.insert(ex.toWeakRef(), result);
        }
        return result;
    }
    else if (ex->kind == AST::ExprArrayElement) {
        result = createArrayElementGet(builder, ex, isLvalue, ex->keepInCache ? FunctionBegin : BeforeTerminator);
    }
    else if (ex->kind == AST::ExprFunctionCall) {
        int beforeCallLineNumber = lastLineNumber_;
        result = createFunctionCall(builder, ex->function, ex->operands, ex->keepInCache ? FunctionBegin : BeforeTerminator);
        lastLineNumber_ = beforeCallLineNumber;
        if (ex->function->header.implType == AST::AlgorhitmCompiled &&
                Shared::GeneratorInterface::NoDebug != debugLevel_)
        {
            llvm::Value* llineno = llvm::ConstantInt::getSigned(
                        llvm::Type::getInt32Ty(*context_),
                        lastLineNumber_
                        );
            kumirSetCurrentLineNumber_.createCallInstruction(&builder, llineno);
        }
    }
    else if (ex->kind == AST::ExprSubexpression) {
        result = createSubExpession(builder, ex);
    }
    if (!isLvalue && ex->kind != AST::ExprArrayElement) {
        bool notPlainScalar = ex->baseType.kind == AST::TypeString || ex->baseType.kind == AST::TypeUser;
        if (notPlainScalar && ex->dimension == 0u) {
            if (!ex->keepInCache) {
                tempValsToFree_.push_back(result);
            }
        }
    }
    if (ex->keepInCache) {
        calculateCache_.insert(ex.toWeakRef(), result);
    }
    return result;
}

llvm::Value * LLVMGenerator::createArrayElementGet(llvm::IRBuilder<> &builder, const AST::ExpressionPtr &ex, bool isLvalue, AllocaPlace allocaPlace)
{
    allocaPlace = BeforeTerminator;
    llvm::Value * var = findVariableAtCurrentContext(ex->variable);
    Q_ASSERT(var);
    QVector<llvm::Value*> args;
    args.reserve(20);
    llvm::Value * elemPtr = CreateAlloca(builder, getScalarType().pointerTo(), "", allocaPlace);
    Q_ASSERT(elemPtr);
    if (ex->variable->dimension > 0u) {
        args.push_back(elemPtr);
        if (!isLvalue) {
            args.push_back(llvm::ConstantInt::getTrue(*context_));
        }
        else {
            args.push_back(llvm::ConstantInt::getFalse(*context_));
        }
        args.push_back(var);
        for (int i=0; i<ex->variable->dimension; i++) {
            const AST::ExpressionPtr& indexOperand = ex->operands[i];
            llvm::Value * index = calculate(builder, indexOperand);
            Q_ASSERT(index);
            args.push_back(index);
        }
        LLVM::FunctionRef getPtrFunc;
        if      (1u == ex->variable->dimension) getPtrFunc = kumirGetArray1Element_;
        else if (2u == ex->variable->dimension) getPtrFunc = kumirGetArray2Element_;
        else if (3u == ex->variable->dimension) getPtrFunc = kumirGetArray3Element_;
        Q_ASSERT(getPtrFunc);
        getPtrFunc.createCallInstruction(&builder, args);
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
            LLVM::TypeRef struct_stringref = stdlibModule_.getType("struct.__kumir_stringref");
            Q_ASSERT(struct_stringref);
            result = CreateAlloca(builder, struct_stringref, "", allocaPlace);
            kumirGetStringElementRef_.createCallInstruction(&builder, result, elemPtr, index);
        }
        else {
            LLVM::TypeRef struct_scalar = stdlibModule_.getType("struct.__kumir_scalar");
            Q_ASSERT(struct_scalar);
            result = CreateAlloca(builder, struct_scalar, "", allocaPlace);
            kumirGetStringElement_.createCallInstruction(&builder, result, elemPtr, index);
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
            LLVM::TypeRef struct_stringref = stdlibModule_.getType("struct.__kumir_stringref");
            Q_ASSERT(struct_stringref);
            result = CreateAlloca(builder, struct_stringref, "", allocaPlace);
            kumirGetStringSliceRef_.createCallInstruction(&builder, result, elemPtr, index1, index2);
        }
        else {
            LLVM::TypeRef struct_scalar = stdlibModule_.getType("struct.__kumir_scalar");
            Q_ASSERT(struct_scalar);
            result = CreateAlloca(builder, struct_scalar, "", allocaPlace);
            kumirGetStringSlice_.createCallInstruction(&builder, result, elemPtr, index1, index2);
        }
        Q_ASSERT(result);
        return result;
    }
    else {
        return 0;
    }
}

llvm::Value * LLVMGenerator::createStringSliceGet(llvm::IRBuilder<> &builder, const AST::ExpressionPtr &ex, bool isLvalue, AllocaPlace allocaPlace)
{
    return 0;
}

llvm::Value * LLVMGenerator::createConstant(llvm::IRBuilder<> & builder, const AST::Type kty, const QVariant &value, AllocaPlace allocaPlace)
{
    llvm::Value * result = 0;
    LLVM::TypeRef ty = getScalarType();
    llvm::Value * arg = 0;
    LLVM::FunctionRef func;
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
        const std::string strval = std::string(value.toString().toUtf8().data());
        arg = builder.CreateGlobalStringPtr(strval);
        func = kumirCreateString_;
    }
    llvm::Value * tmp = CreateAlloca(builder, ty, "", FunctionBegin);
    result = func.createCallInstruction(&builder, tmp, arg);
    Q_ASSERT(result);
    return tmp;
}

static std::string convertByteArrayToHexCoding(const QByteArray & data)
{
    std::string result;
    for (int i=0; i<data.size(); i++) {
        if (i>0) result.push_back(' ');
        char b = data[i];
        QByteArray n = QByteArray::number((unsigned int)(b), 16);
        if (n.size() < 2)
            n.prepend('0');
        result += std::string(n.data());
    }
    return result;
}

llvm::Value * LLVMGenerator::createArrayConstant(llvm::IRBuilder<> &builder, const AST::VariableBaseType bt, const uint8_t dim, const QVariant &value)
{
    QByteArray data;
    if      (1u == dim) data = createArray_1_ConstantData(bt, value.toList());
    else if (2u == dim) data = createArray_2_ConstantData(bt, value.toList());
    else if (3u == dim) data = createArray_3_ConstantData(bt, value.toList());

    llvm::Value * ldata = builder.CreateGlobalStringPtr(convertByteArrayToHexCoding(data));
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

static QByteArray createNormalizedAsciiName(const QByteArray & source)
{
    static const QList<QByteArray> operators = QList<QByteArray>()
            << "=" << "<>" << "<" << ">" << "+" << "-" << "*" << "/";
    static const QList<QByteArray> operatorNames = QList<QByteArray>()
            << "eq" << "neq" << "lt" << "gt" << "plus" << "minus" << "astarisk" << "slash";

    Q_ASSERT(operators.size() == operatorNames.size());

    QByteArray result;
    for (int i=0; i<source.length(); i++) {
        const char ch = source[i];
        const QChar qch(ch);
        if (qch.isLetterOrNumber() || ch == '_') {
            result.push_back(ch);
        }
        else if (ch == ' ') {
            result.push_back('_');
        }
    }

    if (result.isEmpty() && operators.contains(source)) {
        int index = operators.indexOf(source);
        result = QByteArray("operator_") + operatorNames[index];
    }

    return result;
}

llvm::Value * LLVMGenerator::createFunctionCall(llvm::IRBuilder<> &builder, const AST::AlgorithmPtr &alg, const QList<AST::ExpressionPtr> & arguments, AllocaPlace allocaPlace)
{
    LLVM::FunctionRef func;
    QByteArray funcName;

    if (alg->header.implType == AST::AlgorhitmCompiled) {
        kumirCheckCallStack_.createCallInstruction(&builder);
        funcName = NameTranslator::screenUtf8Name(
                    "__kumir_function_" + alg->header.name
                    );
        func = currentModule_.getFunction(funcName);
        Q_ASSERT(func);
    }
    else if (alg->header.implType == AST::AlgorhitmExternal && (
            alg->header.external.moduleName.endsWith(".kum") ||
            alg->header.external.moduleName.endsWith(".kod") )
            )
    {
        funcName = NameTranslator::screenUtf8Name(
                    "__kumir_function_" + alg->header.name
                    );
        func = currentModule_.getFunction(funcName);
        Q_ASSERT(func);
    }
    else if (alg->header.implType == AST::AlgorhitmExternal)
    {
        funcName =
                createNormalizedAsciiName(alg->header.external.moduleAsciiName) +
                "_" +
                createNormalizedAsciiName(alg->header.external.algorithmAsciiName);        

        size_t argsSize = alg->header.arguments.size();
        size_t argsOffset = 0u;

        if (alg->header.returnType.kind != AST::TypeNone) {
            argsSize += 1u;
            argsOffset = 1u;
        }

        QVector<LLVM::TypeRef> formalArgs(argsSize);

        if (alg->header.returnType.kind != AST::TypeNone) {
            LLVM::TypeRef rtype = getScalarType().pointerTo();
            Q_ASSERT(rtype);
            formalArgs[0] = rtype;

        }

        for (int i=0; i<alg->header.arguments.size(); i++) {
            const AST::VariablePtr karg = alg->header.arguments[i];
            LLVM::TypeRef largType = karg->dimension > 0u
                    ? getArrayType().pointerTo()
                    : getScalarType().pointerTo();
            Q_ASSERT(largType);
            formalArgs[i+argsOffset] = largType;
        }

        func = currentModule_.getFunction(funcName); // maybe already used
        if (!func) {
            func = LLVM::FunctionRef::create(
                        LLVM::TypeRef::getGeneric(LLVM::TypeRef::TY_Void, context_),
                        LLVM::FunctionRef::VS_Extern,
                        funcName,
                        formalArgs,
                        QVector<QByteArray>(formalArgs.size()),
                        false,
                        currentModule_,
                        context_
                        );
        }
        Q_ASSERT(func);
    }

    size_t largsCount = alg->header.arguments.size();
    size_t loffset = 0u;

    if (alg->header.returnType != AST::TypeNone) {
        largsCount += 1u;
        loffset = 1u;
    }

    QVector<llvm::Value*> args(largsCount);

    for (size_t i=0; i<alg->header.arguments.size(); i++) {
        llvm::Value * arg = 0;
        if (alg->header.arguments[i]->dimension == 0u) {
            // Scalar value
            const AST::ExpressionPtr & argExpr = arguments.at(i);
            arg = calculate(builder, argExpr);
            if (alg->header.arguments[i]->accessType == AST::AccessArgumentIn) {
                kumirCheckValueDefined_.createCallInstruction(&builder, arg);
                // Copy variable to prevent it changing
                // if passed as another value out-parameter
                if (argExpr->kind == AST::ExprVariable) {
                    llvm::Value * argCopy = CreateAlloca(builder,
                                                         getScalarType(),
                                                         "",
                                                         BeforeTerminator);
                    kumirAssignScalarToScalar_.createCallInstruction(&builder, argCopy, arg);
                    arg = argCopy;
                }

                // Make implicit type cast if need
                if (AST::TypeCharect == argExpr->baseType.kind && AST::TypeString==alg->header.arguments[i]->baseType.kind) {
                    // Char -> String
                    llvm::Value * argCast = CreateAlloca(builder,
                                                         getScalarType(),
                                                         "",
                                                         BeforeTerminator);
                    tempValsToFree_.push_back(argCast);
                    kumirConvertCharToString_.createCallInstruction(&builder, argCast, arg);
                    arg = argCast;
                }
                else if (AST::TypeInteger == argExpr->baseType.kind && AST::TypeReal==alg->header.arguments[i]->baseType.kind) {
                    // Int -> Real
                    llvm::Value * argCast = CreateAlloca(builder,
                                                         getScalarType(),
                                                         "",
                                                         BeforeTerminator);
                    kumirConvertIntToReal_.createCallInstruction(&builder, argCast, arg);
                    arg = argCast;
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
            arg = CreateAlloca(builder, getArrayType(), "", BeforeTerminator);
            kumirLinkArray_.createCallInstruction(&builder, arg, var);
        }
        Q_ASSERT(arg);
        args[loffset+i] = arg;
    }

    llvm::Value * result = 0;

    if (alg->header.returnType != AST::TypeNone) {
        LLVM::TypeRef rtype = getScalarType();
        llvm::Value * rval = CreateAlloca(builder, rtype, "", allocaPlace);
        args[0] = rval;
        result = rval;
    }
    func.createCallInstruction(&builder, args);
    if (alg->header.returnType != AST::TypeNone && alg->header.implType == AST::AlgorhitmCompiled) {
        int endLineNo = alg->impl.endLexems.first()->lineNo + 1;
        lastLineNumber_ = endLineNo;
        llvm::Value* llineno = llvm::ConstantInt::getSigned(
                    llvm::Type::getInt32Ty(*context_),
                    endLineNo);
        kumirSetCurrentLineNumber_.createCallInstruction(&builder, llineno);
        kumirCheckValueDefined_.createCallInstruction(&builder, result);
    }
    return result;
}

llvm::Value * LLVMGenerator::createSubExpession(llvm::IRBuilder<> &builder, const AST::ExpressionPtr &ex)
{
    if (ex->operatorr == AST::OpAnd || ex->operatorr == AST::OpOr) {
        return createShortCircuitOperation(builder,
                                           ex->operands.first(),
                                           ex->operands.last(),
                                           ex->operatorr,
                                           ex->keepInCache? FunctionBegin : BeforeTerminator
                                           );
    }
    QVector<llvm::Value*> operands(ex->operands.size());

    for (int i=0; i<ex->operands.size(); i++) {
        llvm::Value * operand = calculate(builder, ex->operands[i]);
        operands[i] = operand;
    }

    llvm::Value * result = CreateAlloca(builder, getScalarType(), "", ex->keepInCache? FunctionBegin : BeforeTerminator);
    LLVM::FunctionRef opFunc;
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
    case AST::OpAnd:                                    break;
    case AST::OpOr:                                     break;
    case AST::OpNone:                                   break;
    }
    if (1u == operandsCount) {
        opFunc.createCallInstruction(&builder, result, operands[0]);
    }
    else if (2u == operandsCount) {
        opFunc.createCallInstruction(&builder, result, operands[0], operands[1]);
    }
    return result;
}

llvm::Value * LLVMGenerator::createShortCircuitOperation(llvm::IRBuilder<> &builder, const AST::ExpressionPtr &left, const AST::ExpressionPtr &right, const AST::ExpressionOperator op, AllocaPlace allocaPlace)
{
    tempValsToFreeStartPos_.push(tempValsToFree_.size());
    Q_ASSERT(op == AST::OpAnd || op == AST::OpOr);
    llvm::Value * result = CreateAlloca(builder, getScalarType(), "", allocaPlace);
    llvm::Value * leftResult = calculate(builder, left);
    Q_ASSERT(kumirScalarAsBool_);
    Q_ASSERT(kumirMoveScalar_);
    kumirMoveScalar_.createCallInstruction(&builder, result, leftResult);
    llvm::Value * leftBool =
            kumirScalarAsBool_.createCallInstruction(&builder, leftResult);
    const std::string scName = op == AST::OpAnd ? std::string("sc_and") : std::string("sc_or");
    llvm::BasicBlock * jumpOnTrue = 0;
    llvm::BasicBlock * jumpOnFalse = 0;
    llvm::BasicBlock * checkNext = 0;
    llvm::BasicBlock * done = 0;

    if (op == AST::OpAnd) {
        checkNext = jumpOnTrue = llvm::BasicBlock::Create(*context_,
                                              scName + "_ok_then_check_next",
                                              currentFunction_.rawPtr());
        done = jumpOnFalse = llvm::BasicBlock::Create(*context_,
                                               scName + "_first_is_false",
                                               currentFunction_.rawPtr());
    }
    else {
        done = jumpOnTrue = llvm::BasicBlock::Create(*context_,
                                              scName + "_first_is_true",
                                              currentFunction_.rawPtr());
        checkNext = jumpOnFalse = llvm::BasicBlock::Create(*context_,
                                               scName + "_false_but_check_next",
                                               currentFunction_.rawPtr());
    }
    createFreeTempScalars(builder);
    builder.CreateCondBr(leftBool, jumpOnTrue, jumpOnFalse);

    // --- check right operand
    builder.SetInsertPoint(checkNext);
    currentBlock_ = checkNext;

    llvm::Value * rightResult = calculate(builder, right);
    kumirMoveScalar_.createCallInstruction(&builder, result, rightResult);
    createFreeTempScalars(builder);
    builder.CreateBr(done);

    // --- done
    currentBlock_ = done;
    builder.SetInsertPoint(done);
    tempValsToFreeStartPos_.pop();
    return result;
}


void LLVMGenerator::createExternsTable(const LLVM::ModuleRef & source,
                                       const QByteArray & prefix)
{

    const QList<LLVM::FunctionRef> functions = source.functions();

    Q_FOREACH(LLVM::FunctionRef func, functions) {
        if (func.hasExternalLinkage()) {
            const QByteArray funcName = func.name();
            if (funcName.startsWith(prefix)) {
                const QByteArray realName = funcName.mid(prefix.length());
                if (realName.length() > 0 && '_' != realName.at(0)) {
                    externs_.push_back(func);
                }
            }
        }
    }

    if (currentModule_) {
        addExternsToModule(currentModule_);
    }
}

void LLVMGenerator::addExternsToModule(LLVM::ModuleRef & target)
{
    Q_FOREACH(LLVM::FunctionRef srcFunc, externs_) {
        srcFunc.copyToModule(target);
    }
}

LLVM::TypeRef LLVMGenerator::getScalarType()
{
    LLVM::TypeRef result = currentModule_.getType("struct.__kumir_scalar");
    if (!result) {
        result = stdlibModule_.getType("struct.__kumir_scalar");
    }
    Q_ASSERT(result);
    return result;
}

LLVM::TypeRef LLVMGenerator::getArrayType()
{
    LLVM::TypeRef result = currentModule_.getType("struct.__kumir_array");
    if (!result) {
        result = stdlibModule_.getType("struct.__kumir_array");
    }
    Q_ASSERT(result);
    return result;
}

LLVM::TypeRef LLVMGenerator::getStringRefType()
{
    LLVM::TypeRef result = currentModule_.getType("struct.__kumir_stringref");
    if (!result) {
        result = stdlibModule_.getType("struct.__kumir_stringref");
    }
    Q_ASSERT(result);
    return result;
}

/** Create alloca instruction at function begin to avoid stack grow in loops */
llvm::AllocaInst * LLVMGenerator::CreateAlloca(Builder &builder,
                                               LLVM::TypeRef ty,
                                               const QByteArray &name,
                                               AllocaPlace allocaPlace)
{
    Q_ASSERT(currentFunctionEntry_);
    Q_ASSERT(functionEntryPoint_ || FunctionBegin != allocaPlace);

    llvm::BasicBlock * currentBlock = builder.GetInsertBlock();
    llvm::BasicBlock * allocaBlock = FunctionBegin==allocaPlace
            ? functionEntryPoint_ : currentFunctionEntry_;

    if (AsIs != allocaPlace) {
        if (allocaBlock->size() > 0 &&
                allocaBlock->back().isTerminator())
        {
            // insert before "br" terminator
            builder.SetInsertPoint(&allocaBlock->back());
        }
        else {
            // append to end of function entry block
            builder.SetInsertPoint(allocaBlock);
        }
    }

    llvm::AllocaInst * result = 0;
    result = builder.CreateAlloca(ty.rawPtr(), 0, std::string(name));

    // Restore insert point
    builder.SetInsertPoint(currentBlock);

    return result;
}

void LLVMGenerator::readStdLibFunctions()
{
    kumirInitStdLib_ = stdlibModule_.getFunction("__kumir_init_stdlib");
    Q_ASSERT(kumirInitStdLib_);

    kumirSetMainArguments_ = stdlibModule_.getFunction("__kumir_set_main_arguments");
    Q_ASSERT(kumirSetMainArguments_);

    kumirCreateUndefinedScalar_ = stdlibModule_.getFunction("__kumir_create_undefined_scalar");
    Q_ASSERT(kumirCreateUndefinedScalar_);

    kumirCreateUndefinedArray_ = stdlibModule_.getFunction("__kumir_create_undefined_array");
    Q_ASSERT(kumirCreateUndefinedArray_);

    kumirCreateDefinedScalar_ = stdlibModule_.getFunction("__kumir_create_defined_scalar");
    Q_ASSERT(kumirCreateDefinedScalar_);

    kumirCreateArray1_ = stdlibModule_.getFunction("__kumir_create_array_1");
    Q_ASSERT(kumirCreateArray1_);

    kumirRefArray1_ = stdlibModule_.getFunction("__kumir_create_array_ref_1");
    Q_ASSERT(kumirRefArray1_);

    kumirCloneArray1_ = stdlibModule_.getFunction("__kumir_create_array_copy_1");
    Q_ASSERT(kumirCloneArray1_);

    kumirCreateArray2_ = stdlibModule_.getFunction("__kumir_create_array_2");
    Q_ASSERT(kumirCreateArray2_);

    kumirRefArray2_ = stdlibModule_.getFunction("__kumir_create_array_ref_2");
    Q_ASSERT(kumirRefArray2_);

    kumirCloneArray2_ = stdlibModule_.getFunction("__kumir_create_array_copy_2");
    Q_ASSERT(kumirCloneArray2_);

    kumirCreateArray3_ = stdlibModule_.getFunction("__kumir_create_array_3");
    Q_ASSERT(kumirCreateArray3_);

    kumirRefArray3_ = stdlibModule_.getFunction("__kumir_create_array_ref_3");
    Q_ASSERT(kumirRefArray3_);

    kumirCloneArray3_ = stdlibModule_.getFunction("__kumir_create_array_copy_3");
    Q_ASSERT(kumirCloneArray3_);

    kumirLinkArray_ = stdlibModule_.getFunction("__kumir_link_array");
    Q_ASSERT(kumirLinkArray_);

    kumirFreeArray_ = stdlibModule_.getFunction("__kumir_free_array");
    Q_ASSERT(kumirFreeArray_);

    kumirCleanUpArrayInShape_ = stdlibModule_.getFunction("__kumir_cleanup_array_in_shape");
    Q_ASSERT(kumirCleanUpArrayInShape_);

    kumirFillArrayI_ = stdlibModule_.getFunction("__kumir_fill_array_i");
    Q_ASSERT(kumirFillArrayI_);

    kumirFillArrayR_ = stdlibModule_.getFunction("__kumir_fill_array_r");
    Q_ASSERT(kumirFillArrayR_);

    kumirFillArrayB_ = stdlibModule_.getFunction("__kumir_fill_array_b");
    Q_ASSERT(kumirFillArrayB_);

    kumirFillArrayC_ = stdlibModule_.getFunction("__kumir_fill_array_c");
    Q_ASSERT(kumirFillArrayC_);

    kumirFillArrayS_ = stdlibModule_.getFunction("__kumir_fill_array_s");
    Q_ASSERT(kumirFillArrayS_);

    kumirCreateInt_ = stdlibModule_.getFunction("__kumir_create_int");
    Q_ASSERT(kumirCreateInt_);

    kumirCreateReal_ = stdlibModule_.getFunction("__kumir_create_real");
    Q_ASSERT(kumirCreateReal_);

    kumirCreateBool_ = stdlibModule_.getFunction("__kumir_create_bool");
    Q_ASSERT(kumirCreateBool_);

    kumirCreateChar_ = stdlibModule_.getFunction("__kumir_create_char");
    Q_ASSERT(kumirCreateChar_);

    kumirCreateString_ = stdlibModule_.getFunction("__kumir_create_string");
    Q_ASSERT(kumirCreateString_);

    kumirAssignScalarToScalar_ = stdlibModule_.getFunction("__kumir_copy_scalar");
    Q_ASSERT(kumirAssignScalarToScalar_);

    kumirAssignScalarToStringRef_ = stdlibModule_.getFunction("__kumir_modify_string");
    Q_ASSERT(kumirAssignScalarToStringRef_);

    kumirAssignScalarToArrayElement_ = stdlibModule_.getFunction("__kumir_store_scalar");
    Q_ASSERT(kumirAssignScalarToArrayElement_);

    kumirMoveScalar_ = stdlibModule_.getFunction("__kumir_move_scalar");
    Q_ASSERT(kumirMoveScalar_);

    kumirFreeScalar_ = stdlibModule_.getFunction("__kumir_free_scalar");
    Q_ASSERT(kumirFreeScalar_);

    kumirOutputStdoutII_ = stdlibModule_.getFunction("__kumir_output_stdout_ii");
    Q_ASSERT(kumirOutputStdoutII_);

    kumirOutputStdoutIS_ = stdlibModule_.getFunction("__kumir_output_stdout_is");
    Q_ASSERT(kumirOutputStdoutIS_);

    kumirOutputStdoutSI_ = stdlibModule_.getFunction("__kumir_output_stdout_si");
    Q_ASSERT(kumirOutputStdoutSI_);

    kumirOutputStdoutSS_ = stdlibModule_.getFunction("__kumir_output_stdout_ss");
    Q_ASSERT(kumirOutputStdoutSS_);

    kumirOutputStdout_ = stdlibModule_.getFunction("__kumir_output_stdout");
    Q_ASSERT(kumirOutputStdout_);

    kumirPrintScalarVariable_ = stdlibModule_.getFunction("__kumir_print_scalar_variable");
    kumirInputScalarVariable_ = stdlibModule_.getFunction("__kumir_input_scalar_variable");
    kumirPrintArrayVariable_ = stdlibModule_.getFunction("__kumir_print_array_variable");;
    kumirInputArrayVariable_ = stdlibModule_.getFunction("__kumir_input_array_variable");

    Q_ASSERT(kumirPrintScalarVariable_);
    Q_ASSERT(kumirInputScalarVariable_);
    Q_ASSERT(kumirPrintArrayVariable_);
    Q_ASSERT(kumirInputArrayVariable_);

    kumirOutputFileII_ = stdlibModule_.getFunction("__kumir_output_file_ii");
    Q_ASSERT(kumirOutputFileII_);

    kumirOutputFileIS_ = stdlibModule_.getFunction("__kumir_output_file_is");
    Q_ASSERT(kumirOutputFileIS_);

    kumirOutputFileSI_ = stdlibModule_.getFunction("__kumir_output_file_si");
    Q_ASSERT(kumirOutputFileSI_);

    kumirOutputFileSS_ = stdlibModule_.getFunction("__kumir_output_file_ss");
    Q_ASSERT(kumirOutputFileSS_);

    kumirInputFile_ = stdlibModule_.getFunction("__kumir_input_file");
    Q_ASSERT(kumirInputFile_);

    kumirInputStdin_ = stdlibModule_.getFunction("__kumir_input_stdin");
    Q_ASSERT(kumirInputStdin_);

    kumirGetScalarArgument_ = stdlibModule_.getFunction("__kumir_get_scalar_argument");
    Q_ASSERT(kumirGetScalarArgument_);

    kumirGetArrayArgument_ = stdlibModule_.getFunction("__kumir_get_array_argument");
    Q_ASSERT(kumirGetArrayArgument_);

    kumirAssert_ = stdlibModule_.getFunction("__kumir_assert");
    Q_ASSERT(kumirAssert_);

    kumirAbortOnError_ = stdlibModule_.getFunction("__kumir_abort_on_error");
    Q_ASSERT(kumirAbortOnError_);

    kumirSetCurrentLineNumber_ = stdlibModule_.getFunction("__kumir_set_current_line_number");
    Q_ASSERT(kumirSetCurrentLineNumber_);

    kumirCheckValueDefined_ = stdlibModule_.getFunction("__kumir_check_value_defined");
    Q_ASSERT(kumirCheckValueDefined_);

    kumirHalt_ = stdlibModule_.getFunction("__kumir_halt");
    Q_ASSERT(kumirHalt_);

    kumirCheckCallStack_ = stdlibModule_.getFunction("__kumir_check_call_stack");
    kumirPopCallStackCounter_ = stdlibModule_.getFunction("__kumir_pop_call_stack_counter");
    Q_ASSERT(kumirCheckCallStack_);
    Q_ASSERT(kumirPopCallStackCounter_);

    kumirOpEq_ = stdlibModule_.getFunction("__kumir_operator_eq");
    Q_ASSERT(kumirOpEq_);

    kumirOpNeq_ = stdlibModule_.getFunction("__kumir_operator_neq");
    Q_ASSERT(kumirOpNeq_);

    kumirOpLs_ = stdlibModule_.getFunction("__kumir_operator_ls");
    Q_ASSERT(kumirOpLs_);

    kumirOpGt_ = stdlibModule_.getFunction("__kumir_operator_gt");
    Q_ASSERT(kumirOpGt_);

    kumirOpLq_ = stdlibModule_.getFunction("__kumir_operator_lq");
    Q_ASSERT(kumirOpLq_);

    kumirOpGq_ = stdlibModule_.getFunction("__kumir_operator_gq");
    Q_ASSERT(kumirOpGq_);

    kumirOpAdd_ = stdlibModule_.getFunction("__kumir_operator_sum");
    Q_ASSERT(kumirOpAdd_);

    kumirOpSub_ = stdlibModule_.getFunction("__kumir_operator_sub");
    Q_ASSERT(kumirOpSub_);

    kumirOpMul_ = stdlibModule_.getFunction("__kumir_operator_mul");
    Q_ASSERT(kumirOpMul_);

    kumirOpDiv_ = stdlibModule_.getFunction("__kumir_operator_div");
    Q_ASSERT(kumirOpDiv_);

    kumirOpPow_ = stdlibModule_.getFunction("__kumir_operator_pow");
    Q_ASSERT(kumirOpPow_);

    kumirOpNeg_ = stdlibModule_.getFunction("__kumir_operator_neg");
    Q_ASSERT(kumirOpNeg_);

    kumirGetArray1Element_ = stdlibModule_.getFunction("__kumir_get_array_1_element");
    Q_ASSERT(kumirGetArray1Element_);    

    kumirGetArray2Element_ = stdlibModule_.getFunction("__kumir_get_array_2_element");
    Q_ASSERT(kumirGetArray2Element_);

    kumirGetArray3Element_ = stdlibModule_.getFunction("__kumir_get_array_3_element");
    Q_ASSERT(kumirGetArray3Element_);

    kumirGetStringElement_ = stdlibModule_.getFunction("__kumir_get_string_element");
    Q_ASSERT(kumirGetStringElement_);

    kumirGetStringSlice_ = stdlibModule_.getFunction("__kumir_get_string_slice");
    Q_ASSERT(kumirGetStringSlice_);

    kumirGetStringElementRef_ = stdlibModule_.getFunction("__kumir_get_string_element_ref");
    Q_ASSERT(kumirGetStringElementRef_);

    kumirGetStringSliceRef_ = stdlibModule_.getFunction("__kumir_get_string_slice_ref");
    Q_ASSERT(kumirGetStringSliceRef_);

    kumirConvertCharToString_ = stdlibModule_.getFunction("__kumir_convert_char_to_string");
    Q_ASSERT(kumirConvertCharToString_);

    kumirConvertIntToReal_ = stdlibModule_.getFunction("__kumir_convert_int_to_real");
    Q_ASSERT(kumirConvertIntToReal_);

    kumirLoopForFromToInitCounter_ = stdlibModule_.getFunction("__kumir_loop_for_from_to_init_counter");
    kumirLoopForFromToStepInitCounter_ = stdlibModule_.getFunction("__kumir_loop_for_from_to_step_init_counter");
    kumirLoopForCheckCounter_ = stdlibModule_.getFunction("__kumir_loop_for_check_counter");
    kumirLoopTimesInitCounter_ = stdlibModule_.getFunction("__kumir_loop_times_init_counter");
    kumirLoopTimesCheckCounter_ = stdlibModule_.getFunction("__kumir_loop_times_check_counter");
    kumirLoopEndCounter_ = stdlibModule_.getFunction("__kumir_loop_end_counter");
    Q_ASSERT(kumirLoopForFromToInitCounter_);
    Q_ASSERT(kumirLoopForFromToStepInitCounter_);
    Q_ASSERT(kumirLoopForCheckCounter_);
    Q_ASSERT(kumirLoopTimesInitCounter_);
    Q_ASSERT(kumirLoopTimesCheckCounter_);
    Q_ASSERT(kumirLoopEndCounter_);

    kumirScalarAsBool_ = stdlibModule_.getFunction("__kumir_scalar_as_bool");
    Q_ASSERT(kumirScalarAsBool_);


}

QByteArray LLVMGenerator::buildCXXName(const QString &ns,
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
    return result.toLatin1();
}

}
