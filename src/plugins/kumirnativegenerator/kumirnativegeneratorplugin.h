#ifndef KUMIRCPPGENERATOR_H
#define KUMIRCPPGENERATOR_H

#include "extensionsystem/kplugin.h"
#include "interfaces/generatorinterface.h"
#include "dataformats/ast.h"

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
#ifdef USE_CLANG
    inline Shared::GeneratorResult generateExecuable(
        const AST::Data *tree
        , QIODevice *out) { return generateExecuableUsingCLang(tree, out); }
#else
    inline Shared::GeneratorResult generateExecuable(
        const AST::Data *tree
        , QIODevice *out) { return generateExecuableUsingGCC(tree, out); }
#endif
private:
#ifdef USE_CLANG
    Shared::GeneratorResult generateExecuableUsingCLang(
        const AST::Data * tree,
        QIODevice * out);
#endif
    Shared::GeneratorResult generateExecuableUsingGCC(
        const AST::Data *tree,
        QIODevice *out);
    void start();
    void stop();
    struct KumirNativeGeneratorPrivate * d;


};

}

#endif // KUMIRCPPGENERATOR_H
