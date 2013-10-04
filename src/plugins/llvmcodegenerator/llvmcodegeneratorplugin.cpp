#include "llvmcodegeneratorplugin.h"
#include "llvmgenerator.h"

namespace LLVMCodeGenerator {

LLVMCodeGeneratorPlugin::LLVMCodeGeneratorPlugin()
    : ExtensionSystem::KPlugin()
    , d(new LLVMGenerator)
{
}

LLVMCodeGeneratorPlugin::~LLVMCodeGeneratorPlugin()
{
    delete d;
}

QList<ExtensionSystem::CommandLineParameter>
LLVMCodeGeneratorPlugin::acceptableCommandLineParameters() const
{
    return QList<ExtensionSystem::CommandLineParameter>();
}

void LLVMCodeGeneratorPlugin::setDebugLevel(DebugLevel debugLevel)
{

}

void LLVMCodeGeneratorPlugin::generateExecuable(
            const AST::DataPtr tree,
            QByteArray & out,
            QString & mimeType,
            QString & fileSuffix
            )
{
    d->reset();
}

void LLVMCodeGeneratorPlugin::setOutputToText(bool flag)
{

}

QString LLVMCodeGeneratorPlugin::initialize(
        const QStringList &configurationArguments,
        const ExtensionSystem::CommandLine &runtimeArguments)
{
    return "";
}

void LLVMCodeGeneratorPlugin::start()
{

}

void LLVMCodeGeneratorPlugin::stop()
{

}

}
