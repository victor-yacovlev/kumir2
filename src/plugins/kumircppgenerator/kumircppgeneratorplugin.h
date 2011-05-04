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

public:
    KumirCppGeneratorPlugin();
    ~KumirCppGeneratorPlugin();

    QString initialize(const QStringList &arguments);
    void generateExecuable(const AST::Data *tree
                           , QIODevice *out
                           , Shared::GeneratorType &outType);
    void start();
    void stop();
private:
    struct KumirCppGeneratorPrivate * d;

};

}

#endif // KUMIRCPPGENERATOR_H
