#ifndef LLVMCODEGENERATORPLUGIN_H
#define LLVMCODEGENERATORPLUGIN_H

#include "extensionsystem/kplugin.h"
#include "interfaces/generatorinterface.h"

#include <QObject>
#include <llvm/Config/llvm-config.h>
#if LLVM_VERSION_MINOR >= 3
#include <llvm/IR/Module.h>
#else
#include <llvm/Module.h>
#endif
#include <llvm/ADT/Triple.h>

namespace LLVMCodeGenerator {

class LLVMCodeGeneratorPlugin
        : public ExtensionSystem::KPlugin
        , public Shared::GeneratorInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "kumir2.LLVMCodeGenerator" FILE "")
#endif
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
    static void fixMultipleTypeDeclarations(QByteArray & data);
    void start();
    void stop();

    static QByteArray runExternalToolsToGenerateExecutable(const QByteArray & bitcode);
    static bool compileExternalUnit(const QString & fileName);
    static QString findUtil(const QString & name);

private:
    class LLVMGenerator * d;
    bool createMain_;
    bool linkStdLib_;
    bool linkAllUnits_;
    bool textForm_;
    bool runToolChain_;
    DebugLevel debugLevel_;

};

} // end namespace

#endif // LLVMCODEGENERATORPLUGIN_H
