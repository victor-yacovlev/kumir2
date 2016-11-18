#ifndef LLVM_TYPE_H
#define LLVM_TYPE_H


namespace llvm {
class Type;
class LLVMContext;
}

namespace LLVM {

class TypeRef {
    friend class ModuleRef;
public:

    llvm::Type * rawPtr() const;

    enum GenericType { TY_Void = 0, TY_Int = 1, TY_Real = 2, TY_Byte = 3, TY_CStr = 4 };

    static TypeRef getGeneric(GenericType t, llvm::LLVMContext* context);

    operator bool() const;
    bool operator ==(const TypeRef &other) const;

    TypeRef pointerTo() const;

private:
    llvm::Type * d = 0;
};

}

#endif
