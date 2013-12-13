#include "externgenerator.h"

namespace LLVMCodeGenerator {

void ExternGenerator::reset(const llvm::Module * stdlibModule)
{
    nameTranslator_->reset();
    externFunctions_.clear();
    extraTypesUsed_.clear();

    kumirScalarType_ = 0;
    kumirArrayType_ = 0;
    cFilePtrType_ = 0;
    initConversionFunctions(stdlibModule);
}

ExternGenerator::ExternGenerator()
    : nameTranslator_(new NameTranslator)
{    
}

void ExternGenerator::initConversionFunctions(const llvm::Module *stdlibModule)
{
    kumirCreateInt_ = stdlibModule->getFunction("__kumir_create_int");
    Q_ASSERT(kumirCreateInt_);

    kumirCreateReal_ = stdlibModule->getFunction("__kumir_create_real");
    Q_ASSERT(kumirCreateReal_);

    kumirCreateBool_ = stdlibModule->getFunction("__kumir_create_bool");
    Q_ASSERT(kumirCreateBool_);

    kumirCreateCharFromChar_ = stdlibModule->getFunction("__kumir_create_char_from_char");
    Q_ASSERT(kumirCreateCharFromChar_);

    kumirCreateCharFromWChar_ = stdlibModule->getFunction("__kumir_create_char_from_wchar");
    Q_ASSERT(kumirCreateCharFromWChar_);

    kumirCreateStringFromCs_ = stdlibModule->getFunction("__kumir_create_string_from_cs");
    Q_ASSERT(kumirCreateStringFromCs_);

    kumirCreateStringFromWcs_ = stdlibModule->getFunction("__kumir_create_string_from_wcs");
    Q_ASSERT(kumirCreateStringFromWcs_);

    kumirCreateUndefinedScalar_ = stdlibModule->getFunction("__kumir_create_undefined_scalar");
    Q_ASSERT(kumirCreateUndefinedScalar_);

    kumirCreateUndefinedArray_ = stdlibModule->getFunction("__kumir_create_undefined_array");
    Q_ASSERT(kumirCreateUndefinedArray_);
}

void ExternGenerator::addFunction(
        const AST::ModulePtr &module,
        const AST::AlgorithmPtr &algorithm,
        llvm::Function *function)
{
    Q_ASSERT(module);
    Q_ASSERT(algorithm);
    Q_ASSERT(function);

    const QString qn = module->header.name.isEmpty()
            ? algorithm->header.name
            : module->header.name + "_" + algorithm->header.name;
    Q_ASSERT(qn.length());
    const CString cname = nameTranslator_->addGlobal(qn);
    Q_ASSERT(cname.length());

    FunctionSpec spec = { function,
                          algorithm->header.returnType,
                          algorithm->header.arguments,
                          createKumirSignature(module->header.name, algorithm)
                        };

    externFunctions_[cname] = spec;
}

void ExternGenerator::generateBody(
        llvm::LLVMContext &ctx,
        llvm::Module *module,
        bool useWChar)
{
    wcharTypeUsed_ = false;
    boolTypeUsed_ = false;
    int32TypeUsed_ = false;
    kumirScalarType_ = module->getTypeByName("struct.__kumir_scalar");
    kumirArrayType_ = module->getTypeByName("struct.__kumir_array");
    Q_ASSERT(kumirScalarType_);
    Q_ASSERT(kumirArrayType_);
    foreach (const CString & name, externFunctions_.keys()) {
        FunctionSpec & spec = externFunctions_[name];
        generateFunction(name, spec, ctx, module, useWChar);
    }
}

void ExternGenerator::generateFunction(
        const CString &name,
        FunctionSpec &spec,
        llvm::LLVMContext &ctx,
        llvm::Module *module,
        bool useWChar)
{    
    // === Create function header

    nameTranslator_->beginNamespace();

    llvm::Type * rtype = llvmType(ctx, spec.returnType, useWChar);
    Q_ASSERT(rtype);

    bool requiresStringAllocatorParameter =
            spec.returnType.kind == AST::TypeString;

    foreach (const AST::VariablePtr & karg, spec.arguments) {
        if (karg->accessType != AST::AccessArgumentIn &&
                karg->baseType.kind == AST::TypeString)
        {
            requiresStringAllocatorParameter = true;
            break;
        }
    }

    std::vector<llvm::Type*> argTypes(spec.arguments.size());
    std::vector<llvm::Argument*> arguments;
    for (size_t i=0; i<argTypes.size(); i++) {
        llvm::Type * atype = llvmType(ctx, spec.arguments[i]->baseType, useWChar);
        if (spec.arguments[i]->dimension > 0u
                || spec.arguments[i]->accessType != AST::AccessArgumentIn)
        {
            if ( spec.arguments[i]->dimension > 0u ||
                 (spec.arguments[i]->baseType.kind != AST::TypeString &&
                    spec.arguments[i]->baseType.kind != AST::TypeUser)
                 )
            {
                atype = atype->getPointerTo();
            }
        }
        Q_ASSERT(atype);
        argTypes[i] = atype;
    }

    if (requiresStringAllocatorParameter) {
        llvm::Type * voidPtrTy = llvm::Type::getVoidTy(ctx)->getPointerTo();
        Q_ASSERT(voidPtrTy);
        argTypes.push_back(voidPtrTy);
    }

    llvm::FunctionType * ft = llvm::FunctionType::get(rtype, argTypes, false);
    Q_ASSERT(ft);

    llvm::Function * func = llvm::Function::Create(
                ft, llvm::Function::ExternalLinkage, name, module
                );
    Q_ASSERT(func);

#if LLVM_VERSION_MINOR >= 3
    func->addFnAttr(llvm::Attribute::NoUnwind);
    func->addFnAttr(llvm::Attribute::UWTable);
#elif LLVM_VERSION_MINOR == 0
    func->addFnAttr(llvm::Attribute::NoUnwind);
    func->addFnAttr(llvm::Attribute::UWTable);
#else
    func->addFnAttr(llvm::Attributes::NoUnwind);
    func->addFnAttr(llvm::Attributes::UWTable);
#endif

    llvm::Function::ArgumentListType::iterator lIt = func->getArgumentList().begin();
    QList<AST::VariablePtr>::iterator kIt = spec.arguments.begin();
    for ( ; lIt != func->getArgumentList().end() && kIt != spec.arguments.end() ; ++lIt, ++kIt)
    {
        llvm::Argument & larg = *lIt;
        const AST::VariablePtr & karg = *kIt;
        const CString name = nameTranslator_->add(karg->name);
        larg.setName(name);
        spec.argnames.push_back(name);
        arguments.push_back(lIt);
    }

    if (requiresStringAllocatorParameter) {
        llvm::Argument & larg = func->getArgumentList().back();
        larg.setName(CString("__c_out_string_allocator__"));
        arguments.push_back(&larg);
    }

    Q_ASSERT(arguments.size() == argTypes.size());

    // === Create function body

    llvm::IRBuilder<> builder(ctx);

    // Allocate internal kumir arguments
    llvm::BasicBlock* functionEntry = llvm::BasicBlock::Create(ctx, "entry", func);
    builder.SetInsertPoint(functionEntry);

    std::vector<llvm::Value*> kumirArgs(spec.arguments.size());

    for (size_t i=0; i<spec.arguments.size(); i++) {
        llvm::Type * atype = spec.arguments[i]->dimension == 0u
                ? kumirScalarType_ : kumirArrayType_;
        Q_ASSERT(atype);
        llvm::Value * karg = builder.CreateAlloca(atype, 0, CString("__kumir_") + spec.argnames[i]);
        Q_ASSERT(karg);
        kumirArgs[i] = karg;
    }   

    // Copy values from C-arguments or make them unitialized
    for (size_t i=0; i<kumirArgs.size(); i++) {
        llvm::Value * karg = kumirArgs[i];
        llvm::Value * carg = arguments[i];
        const bool hasValue = spec.arguments[i]->accessType != AST::AccessArgumentOut;
        const bool isArray = spec.arguments[i]->dimension > 0u;
        if (hasValue && !isArray) {
            // If passed by pointer -- load its value
            if (spec.arguments[i]->accessType == AST::AccessArgumentInOut &&
                    spec.arguments[i]->baseType.kind != AST::TypeString &&
                    spec.arguments[i]->baseType.kind != AST::TypeUser
                    )
            {
                llvm::Value * deref = builder.CreateLoad(carg, carg->getName()+CString("_deref"));
                carg = deref;
            }

            // Create scalar value as of copy from C-value
            if (spec.arguments[i]->baseType.kind == AST::TypeBoolean) {
                builder.CreateCall2(kumirCreateBool_, karg, carg);
            }
            else if (spec.arguments[i]->baseType.kind == AST::TypeInteger) {
                builder.CreateCall2(kumirCreateInt_, karg, carg);
            }
            else if (spec.arguments[i]->baseType.kind == AST::TypeReal) {
                builder.CreateCall2(kumirCreateReal_, karg, carg);
            }
            else if (spec.arguments[i]->baseType.kind == AST::TypeCharect) {
                builder.CreateCall2(useWChar? kumirCreateCharFromWChar_
                                            : kumirCreateCharFromChar_
                                              , karg, carg);
            }
            else if (spec.arguments[i]->baseType.kind == AST::TypeString) {
                builder.CreateCall2(useWChar? kumirCreateStringFromWcs_
                                            : kumirCreateStringFromCs_
                                              , karg, carg);
            }
            else {
                qFatal("Not implemented");
            }
        }
        else if (hasValue && isArray) {
            // TODO implement me
        }
        else if (!hasValue && !isArray) {
            // Create undefined scalar
            builder.CreateCall(kumirCreateUndefinedScalar_, karg);
        }
        else if (!hasValue && isArray) {
            // Create undefined array
            builder.CreateCall(kumirCreateUndefinedArray_, karg);
        }
    }

    // Prepare return value
    llvm::Value * kretval = 0;
    if (spec.returnType.kind != AST::TypeNone) {
        kretval = builder.CreateAlloca(kumirScalarType_, 0, CString("__kumir_result__"));
        kumirArgs.insert(kumirArgs.begin(), 1u, kretval);
    }

    // Call kumir algorithm
    builder.CreateCall(spec.targetFunction, kumirArgs);

    // TODO copy back out-parameters

    // Create and return C-value
    llvm::Value * cretval = 0;
    if (spec.returnType.kind != AST::TypeNone) {
        cretval = builder.CreateAlloca(rtype, 0, CString("__c_result__"));

        builder.CreateRet(cretval);
    }
    else {
        builder.CreateRetVoid();
    }

    nameTranslator_->endNamespace();
}

QString ExternGenerator::kumirBaseTypeName(const AST::VariableBaseType t)
{
    switch (t) {
    case AST::TypeBoolean: return QString::fromUtf8("лог"); break;
    case AST::TypeCharect: return QString::fromUtf8("сим"); break;
    case AST::TypeInteger: return QString::fromUtf8("цел"); break;
    case AST::TypeReal: return QString::fromUtf8("вещ"); break;
    case AST::TypeString: return QString::fromUtf8("лит"); break;
    default: return QString();
    }
}

llvm::Type * ExternGenerator::llvmBaseType(llvm::LLVMContext &ctx, const AST::VariableBaseType t, bool useWChar)
{
    static const size_t wchar_size = sizeof(wchar_t) * 8u;
    switch (t) {
    case AST::TypeNone: return llvm::Type::getVoidTy(ctx); break;
    case AST::TypeBoolean:
        boolTypeUsed_ = true;
        return llvm::Type::getInt8Ty(ctx);
        break;
    case AST::TypeCharect:
        wcharTypeUsed_ = useWChar;
        return llvm::Type::getIntNTy(ctx, useWChar? wchar_size : 8u);
        break;
    case AST::TypeInteger:
        int32TypeUsed_ = true;
        return llvm::Type::getInt32Ty(ctx);
        break;
    case AST::TypeReal: return llvm::Type::getDoubleTy(ctx); break;
    case AST::TypeString:
        wcharTypeUsed_ = useWChar;
        return llvm::Type::getIntNPtrTy(ctx, useWChar? wchar_size : 8u);
        break;
    default: return 0;
    }
}

QString ExternGenerator::kumirTypeName(const AST::Type &t)
{
    if (t.kind == AST::TypeNone)
        return QString();
    else if (t.kind == AST::TypeUser)
        return t.name;
    else
        return kumirBaseTypeName(t.kind);
}

llvm::Type * ExternGenerator::llvmType(llvm::LLVMContext &ctx, const AST::Type &t, bool useWChar)
{
    if (t.kind == AST::TypeUser) {
        if (t.asciiName == "file") {
            // Special case of file handler.
            // Due to various FILE struct implementation,
            // use void* pointer internal representation
            return llvm::Type::getVoidTy(ctx)->getPointerTo();
        }
        else {
            // TODO implement me later
            return 0;
        }
    }
    else {
        return llvmBaseType(ctx, t.kind, useWChar);
    }
}

QString ExternGenerator::createKumirSignature(
        const QString &moduleName,
        const AST::AlgorithmPtr &algorithm)
{
    QString result = QString::fromUtf8("алг ");
    const QString qn = moduleName.isEmpty()
            ? algorithm->header.name
            : moduleName + "::" + algorithm->header.name;

    const QString rtype = kumirTypeName(algorithm->header.returnType);
    if (rtype.length())
        result += rtype + " ";

    result += qn;

    if (algorithm->header.arguments.size()) {
        result += "(";
        for (int i=0; i<algorithm->header.arguments.size(); i++) {
            if (i > 0) result += ", ";
            const AST::VariablePtr & arg = algorithm->header.arguments[i];
            if (arg->accessType == AST::AccessArgumentInOut)
                result += QString::fromUtf8("аргрез ");
            else if (arg->accessType == AST::AccessArgumentOut)
                result += QString::fromUtf8("рез ");
            else
                result += QString::fromUtf8("арг ");
            result += kumirTypeName(arg->baseType);
            if (arg->dimension) {
                result += QString::fromUtf8("таб");
            }
            result += " ";
            result += arg->name;
            for (uint8_t j=0; j<arg->dimension; j++) {
                result += "[][]";
            }
        }
        result += ")";
    }
    return result;
}



} // namespace LLVMCodeGenerator
