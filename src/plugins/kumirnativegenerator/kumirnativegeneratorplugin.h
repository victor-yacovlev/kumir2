#ifndef KUMIRCPPGENERATOR_H
#define KUMIRCPPGENERATOR_H

#include "extensionsystem/kplugin.h"
#include "interfaces/generatorinterface.h"
#include "abstractsyntaxtree/ast.h"

namespace KumirNativeGenerator {

class KumirNativeGeneratorPlugin
        : public ExtensionSystem::KPlugin
        , public Shared::GeneratorInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::GeneratorInterface)
public:
    KumirNativeGeneratorPlugin();
    ~KumirNativeGeneratorPlugin();

    QString initialize(const QStringList &arguments);
    Shared::GeneratorResult generateExecuable(
        const AST::Data *tree
        , QIODevice *out);
private:
    void start();
    void stop();
    struct KumirNativeGeneratorPrivate * d;

};

}

#endif // KUMIRCPPGENERATOR_H
