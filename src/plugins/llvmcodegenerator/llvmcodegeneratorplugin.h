#ifndef LLVMCODEGENERATORPLUGIN_H
#define LLVMCODEGENERATORPLUGIN_H

#include "extensionsystem/kplugin.h"
#include "interfaces/generatorinterface.h"

#include <QObject>
#include <llvm/Module.h>
#include <llvm/ADT/Triple.h>

namespace LLVMCodeGenerator {

class LLVMCodeGeneratorPlugin
        : public ExtensionSystem::KPlugin
        , public Shared::GeneratorInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::GeneratorInterface)

public:
    LLVMCodeGeneratorPlugin();
    ~LLVMCodeGeneratorPlugin();

    QList<ExtensionSystem::CommandLineParameter> acceptableCommandLineParameters() const;

    void setDebugLevel(DebugLevel debugLevel);
    void generateExecuable(
                const AST::DataPtr tree,
                QByteArray & out,
                QString & mimeType,
                QString & fileSuffix
                );

    void setOutputToText(bool flag);
    inline void setVerbose(bool) {}
    inline void setTemporaryDir(const QString &, bool ) {}
    inline void updateSettings(const QStringList &) {}

protected:
    QString initialize(const QStringList &configurationArguments,
                       const ExtensionSystem::CommandLine &runtimeArguments);
    void start();
    void stop();

//    static std::string generateNativeExecutable(llvm::Module * module);
    static QByteArray runExternalToolsToGenerateExecutable(const QByteArray & bitcode);
//    static llvm::Triple getNativeTriple();
    static QString findUtil(const QString & name);

private:
    class LLVMGenerator * d;
    bool compileOnly_;
    bool textForm_;
    bool linkStdLib_;

};

} // end namespace

#endif // LLVMCODEGENERATORPLUGIN_H
