#include "llvm_type.h"

#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>

namespace LLVM {

llvm::Type * TypeRef::rawPtr() const
{
    return d;
}

TypeRef TypeRef::getGeneric(GenericType type, llvm::LLVMContext * ctx)
{
    llvm::Type * t = 0;
    switch (type) {
    case TY_Void:
        t = llvm::Type::getVoidTy(*ctx);
        break;
    case TY_Int:
        t = llvm::Type::getInt32Ty(*ctx);
        break;
    case TY_Real:
        t = llvm::Type::getDoubleTy(*ctx);
        break;
    case TY_Byte:
        t = llvm::Type::getInt8Ty(*ctx);
        break;
    case TY_CStr:
        t = llvm::Type::getInt8PtrTy(*ctx);
        break;
    default:
        t = 0;
        break;
    }
    TypeRef result;
    result.d = (t);
    return result;
}

LLVM::TypeRef::operator bool() const
{
    return bool(d);
}

bool TypeRef::operator ==(const TypeRef &other) const
{
    return rawPtr()==other.rawPtr();
}

TypeRef TypeRef::pointerTo() const
{
    llvm::Type * me = d;
    TypeRef result;
    result.d = (me->getPointerTo());
    return result;
}

}
