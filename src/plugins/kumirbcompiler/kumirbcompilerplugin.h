#ifndef KUMIRCOMPILERPLUGIN_H
#define KUMIRCOMPILERPLUGIN_H

#include <extensionsystem/kplugin.h>
#include "interfaces/analizerinterface.h"


namespace KumirBytecodeCompiler {

class KumirBytecodeCompilerPlugin
  : public ExtensionSystem::KPlugin
{
    Q_OBJECT

public:
    KumirBytecodeCompilerPlugin();
    ~KumirBytecodeCompilerPlugin();

    QString initialize(const QStringList &arguments);
    void start();
    void stop();
private:
    Shared::AnalizerInterface * m_analizer;

};

}

#endif // KUMIRCOMPILERPLUGIN_H
