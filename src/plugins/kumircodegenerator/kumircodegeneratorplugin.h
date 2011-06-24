#ifndef KUMIRCODEGENERATORPLUGIN_H
#define KUMIRCODEGENERATORPLUGIN_H

#include "extensionsystem/kplugin.h"
#include "interfaces/generatorinterface.h"


using namespace Shared;

namespace KumirCodeGenerator {

class KumirCodeGeneratorPlugin
        : public ExtensionSystem::KPlugin
        , public GeneratorInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::GeneratorInterface)

public:
    KumirCodeGeneratorPlugin();
    ~KumirCodeGeneratorPlugin();

    GeneratorResult generateExecuable(
        const AST::Data * tree
        , QIODevice * out);

protected:
    QString initialize(const QStringList &arguments);
    void start();
    void stop();
private:
    class Generator * d;



};

}

#endif // KUMIRCODEGENERATORPLUGIN_H
