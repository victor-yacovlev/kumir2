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

    QPair<QString,QString> generateExecuable(
        const AST::Data * tree
        , QIODevice * out);

    inline void setVerbose(bool) {}
    inline void setTemporaryDir(const QString &, bool ) {}

protected:
    QString initialize(const QStringList &arguments);
    void start();
    void stop();
private:
    class Generator * d;



};

}

#endif // KUMIRCODEGENERATORPLUGIN_H
