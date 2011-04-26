#ifndef KUMIRCOMPILERPLUGIN_H
#define KUMIRCOMPILERPLUGIN_H

#include <extensionsystem/kplugin.h>
#include "interfaces/analizerinterface.h"


namespace KumirCompiler {

class KumirCompilerPlugin
  : public ExtensionSystem::KPlugin
{
    Q_OBJECT

public:
    KumirCompilerPlugin();
    ~KumirCompilerPlugin();

    QString initialize(const QStringList &arguments);
    void start();
    void stop();
private:
    Shared::AnalizerInterface * m_analizer;

};

}

#endif // KUMIRCOMPILERPLUGIN_H
