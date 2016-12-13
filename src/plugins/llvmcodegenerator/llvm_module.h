#ifndef LLVM_MODULE_H
#define LLVM_MODULE_H

#include "llvm_function.h"
#include "llvm_type.h"

#include <memory>

#include <QByteArray>
#include <QString>

namespace llvm {
class Module;
class LLVMContext;
}

namespace LLVM {

class FunctionRef;
struct ModuleRefPrivate;

class ModuleRef {
public:

    llvm::Module * rawPtr() const;

    static ModuleRef create(llvm::LLVMContext * context);
    static ModuleRef fromTextRepresentation(const QByteArray &text, llvm::LLVMContext * context);
    static ModuleRef fromBinaryRepresentation(const QByteArray &data,
                                              const QString &fileName,
                                              llvm::LLVMContext * context
                                              );


    LLVM::FunctionRef getFunction(const QByteArray & name) const;
    LLVM::TypeRef getType(const QByteArray &name) const;
    QList<LLVM::FunctionRef> functions() const;

    // Waring!!!: this DESTROYS source module, so it must be recreated
    // after calling this method (thanks to f**king LLVM API)
    void linkInto(ModuleRef & dest);
    QByteArray textRepresentation() const;
    QByteArray binaryRepresentation() const;

    operator bool() const;
    bool operator == (const ModuleRef other) const;

    ModuleRef();
    ModuleRef(ModuleRef && src);
    ModuleRef& operator=(ModuleRef && src);
    ~ModuleRef();


private:
    std::unique_ptr<llvm::Module> d;
};


}

#endif
