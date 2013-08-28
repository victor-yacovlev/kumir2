#ifndef KUMIRCOMPILERPLUGIN_H
#define KUMIRCOMPILERPLUGIN_H

#include <extensionsystem/kplugin.h>
#include "interfaces/analizerinterface.h"
#include "interfaces/generatorinterface.h"


namespace KumirCompilerTool {

class KumirCompilerToolPlugin
  : public ExtensionSystem::KPlugin
{
    Q_OBJECT

public:
    KumirCompilerToolPlugin();

    QString initialize(
            const QStringList & configurationArguments,
            const ExtensionSystem::CommandLine & runtimeArguments
            );
    QList<ExtensionSystem::CommandLineParameter> acceptableCommandLineParameters() const;
    void start();
    void stop();
    inline void updateSettings(const QStringList &) {}
private:
    Shared::AnalizerInterface * analizer_;
    Shared::GeneratorInterface * generator_;
    QString sourceFileName_;
    QString sourceFileEncoding_;
    bool useAnsiWindowsOutput_;
    QString outFileName_;


};

}

#endif // KUMIRCOMPILERPLUGIN_H
