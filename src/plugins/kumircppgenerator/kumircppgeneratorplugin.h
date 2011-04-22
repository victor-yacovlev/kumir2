#ifndef KUMIRCPPGENERATOR_H
#define KUMIRCPPGENERATOR_H

#include <extensionsystem/kplugin.h>

namespace KumirCppGenerator {

class KumirCppGeneratorPlugin
  : public ExtensionSystem::KPlugin
{
    Q_OBJECT

public:
    KumirCppGeneratorPlugin();
    ~KumirCppGeneratorPlugin();

    QString initialize(const QStringList &arguments);
    void start();
    void stop();

};

}

#endif // KUMIRCPPGENERATOR_H
