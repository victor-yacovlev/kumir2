#ifndef KUMIRCOMPILERPLUGIN_H
#define KUMIRCOMPILERPLUGIN_H

#include <extensionsystem/kplugin.h>

namespace KumirAnalizer {
class Analizer;
}

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

};

}

#endif // KUMIRCOMPILERPLUGIN_H
