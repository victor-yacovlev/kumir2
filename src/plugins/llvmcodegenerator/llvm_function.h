#ifndef LLVM_FUNCTION_H
#define LLVM_FUNCTION_H

#include "llvm_module.h"
#include "llvm_type.h"

#include <memory>
#include <QVector>

namespace llvm {
class LLVMContext;
class Function;
class Value;
class CallInst;
}

namespace LLVM {

struct FunctionRefPrivate;

class FunctionRef {
    friend class ModuleRef;
public:
    enum Visibility { VS_Extern, VS_Static };

    llvm::Function* rawPtr() const;
    QByteArray name() const;
    bool hasExternalLinkage() const;

    static FunctionRef create(
            TypeRef returnType,
            Visibility visibility,
            const QByteArray &name,
            const QVector<TypeRef> &argTypes,
            const QVector<QByteArray> &argNames,
            bool firstArgIsReturnValuePointer,
            class ModuleRef & parent,
            llvm::LLVMContext* context
            );

    void copyToModule(ModuleRef & target);


    operator bool() const;
    bool operator == (const FunctionRef & other) const;

    llvm::CallInst* createCallInstruction(void *builder, const QVector<llvm::Value*> &values);

    inline llvm::CallInst* createCallInstruction(void *builder) {
        return createCallInstruction(builder, QVector<llvm::Value*>());
    }

    inline llvm::CallInst* createCallInstruction(void *builder, llvm::Value* arg1) {
        QVector<llvm::Value*> args(1);
        args[0] = arg1;
        return createCallInstruction(builder, args);
    }
    inline llvm::CallInst* createCallInstruction(void *builder, llvm::Value* arg1, llvm::Value* arg2) {
        QVector<llvm::Value*> args(2);
        args[0] = arg1;
        args[1] = arg2;
        return createCallInstruction(builder, args);
    }
    inline llvm::CallInst* createCallInstruction(void *builder, llvm::Value* arg1, llvm::Value* arg2, llvm::Value* arg3) {
        QVector<llvm::Value*> args(3);
        args[0] = arg1;
        args[1] = arg2;
        args[2] = arg3;
        return createCallInstruction(builder, args);
    }
    inline llvm::CallInst* createCallInstruction(void *builder, llvm::Value* arg1, llvm::Value* arg2, llvm::Value* arg3, llvm::Value* arg4) {
        QVector<llvm::Value*> args(4);
        args[0] = arg1;
        args[1] = arg2;
        args[2] = arg3;
        args[3] = arg4;
        return createCallInstruction(builder, args);
    }

private:
    llvm::Function * d = 0;
};

}

#endif
