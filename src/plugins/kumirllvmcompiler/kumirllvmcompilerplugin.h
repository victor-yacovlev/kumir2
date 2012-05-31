#ifndef KUMIRCOMPILERPLUGIN_H
#define KUMIRCOMPILERPLUGIN_H

#include <extensionsystem/kplugin.h>
#include "interfaces/analizerinterface.h"


namespace KumirLLVMCompiler {

class KumirLLVMCompilerPlugin
  : public ExtensionSystem::KPlugin
{
    Q_OBJECT

public:
    KumirLLVMCompilerPlugin();
    ~KumirLLVMCompilerPlugin();

    QString initialize(const QStringList &arguments);
    void start();
    void stop();
private:
    Shared::AnalizerInterface * m_analizer;

};

}

#endif
