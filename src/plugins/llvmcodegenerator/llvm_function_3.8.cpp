#include "llvm_function.h"
#include "llvm_type.h"
#include "llvm_module.h"

#include <llvm/IR/IRBuilder.h>

namespace LLVM {

llvm::Function* FunctionRef::rawPtr() const
{
    return d;
}

QByteArray FunctionRef::name() const
{
    llvm::StringRef ls = rawPtr()->getName();
    return QByteArray(ls.begin(), ls.size());
}

bool FunctionRef::hasExternalLinkage() const
{
    return rawPtr()->hasExternalLinkage();
}

FunctionRef FunctionRef::create(
        TypeRef returnType,
        Visibility visibility,
        const QByteArray &name,
        const QVector<TypeRef> &argTypes,
        const QVector<QByteArray> &argNames,
        bool firstArgIsReturnValuePointer,
        ModuleRef & parent,
        llvm::LLVMContext* ctx
        )
{
    llvm::Type *lt = returnType.rawPtr();
    std::vector<llvm::Type*> largTypes(argTypes.size());
    for (int i=0; i<argTypes.size(); ++i) {
        llvm::Type *at = (llvm::Type*) argTypes[i].rawPtr();
        largTypes[i] = at;
    }

    llvm::FunctionType * lft = largTypes.empty()
            ? llvm::FunctionType::get(lt, false)
            : llvm::FunctionType::get(lt, largTypes, false)
              ;

    llvm::GlobalValue::LinkageTypes link =
            VS_Extern==visibility
            ? llvm::GlobalValue::ExternalLinkage
            : llvm::GlobalValue::PrivateLinkage
              ;

    std::string lname(name.constData(), name.size());
    llvm::Module *lm = (llvm::Module*) parent.rawPtr();
    llvm::Function *lf = llvm::Function::Create(
                lft, link, lname, lm
                );

    lf->addFnAttr(llvm::Attribute::NoUnwind);
    lf->addFnAttr(llvm::Attribute::UWTable);

    typedef llvm::Function::ArgumentListType::iterator AIt;
    int argNumber = 0;
    for (AIt it=lf->getArgumentList().begin();
         argNumber<argNames.size() ; ++argNumber, ++it)
    {
        const std::string argName(argNames[argNumber].constData());
        it->setName(argName);
    }

    if (firstArgIsReturnValuePointer) {
        llvm::Argument & firstArg = lf->getArgumentList().front();
        llvm::AttrBuilder abuilder;
        abuilder.addAttribute(llvm::Attribute::StructRet);
        abuilder.addAttribute(llvm::Attribute::NoAlias);
        firstArg.addAttr(llvm::AttributeSet::get(*ctx, 0, abuilder));
        firstArg.addAttr(llvm::AttributeSet::get(*ctx, 1, abuilder));
    }

    FunctionRef result;
    result.d = (lf);
    return result;
}

void FunctionRef::copyToModule(ModuleRef & target)
{
    llvm::FunctionType * ft = d->getFunctionType();
    llvm::StringRef fn = d->getName();
    llvm::GlobalValue::LinkageTypes fl = d->getLinkage();

    llvm::Module* tm = target.rawPtr();
    llvm::Function::Create(ft,fl,fn,tm);
}

LLVM::FunctionRef::operator bool() const
{
    return 0 != rawPtr();
}

bool FunctionRef::operator ==(const FunctionRef &other) const
{
    return rawPtr() == other.rawPtr();
}

llvm::CallInst *FunctionRef::createCallInstruction(void *b, const QVector<llvm::Value *> &values)
{
    llvm::IRBuilder<> * builder =
            reinterpret_cast<llvm::IRBuilder<>*>(b);
    std::vector<llvm::Value*> callArgs(values.size());
    std::copy(values.begin(), values.end(), callArgs.begin());
    return builder->CreateCall(d, callArgs);
}


}
