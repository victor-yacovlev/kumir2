#ifndef KUMIRCPPGENERATOR_H
#define KUMIRCPPGENERATOR_H

#include "extensionsystem/kplugin.h"
#include "interfaces/generatorinterface.h"
#include "dataformats/ast.h"
#include "compilerbackend.h"

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
    QPair<QString,QString> generateExecuable(
        const AST::Data *tree
        , QIODevice *out);
    void setVerbose(bool v);
    void setTemporaryDir(const QString & path, bool autoclean);

private:
    void start();
    void stop();
    struct KumirNativeGeneratorPrivate * d;


};

}

#endif // KUMIRCPPGENERATOR_H
