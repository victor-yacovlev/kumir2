#ifndef KUMIRCPPGENERATOR_H
#define KUMIRCPPGENERATOR_H

#include "extensionsystem/kplugin.h"
#include "interfaces/generatorinterface.h"
#include "abstractsyntaxtree/ast.h"

namespace KumirCppGenerator {

class KumirCppGeneratorPlugin
        : public ExtensionSystem::KPlugin
        , public Shared::GeneratorInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::GeneratorInterface)
public:
    KumirCppGeneratorPlugin();
    ~KumirCppGeneratorPlugin();

    QString initialize(const QStringList &arguments);
    Shared::GeneratorType generateExecuable(
        const AST::Data *tree
        , QIODevice *out);
private:
    void start();
    void stop();
    struct KumirCppGeneratorPrivate * d;

};

}

#endif // KUMIRCPPGENERATOR_H
