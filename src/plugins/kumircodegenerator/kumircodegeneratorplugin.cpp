#include "generator.h"
#include "kumircodegeneratorplugin.h"
#include "dataformats/bytecode.h"

using namespace KumirCodeGenerator;
using namespace Bytecode;

KumirCodeGeneratorPlugin::KumirCodeGeneratorPlugin()
{
    d = new Generator(this);
}

KumirCodeGeneratorPlugin::~KumirCodeGeneratorPlugin()
{
    d->deleteLater();
}


QString KumirCodeGeneratorPlugin::initialize(const QStringList &arguments)
{
    Q_UNUSED(arguments)
    return "";
}

void KumirCodeGeneratorPlugin::start()
{

}

void KumirCodeGeneratorPlugin::stop()
{

}

GeneratorResult KumirCodeGeneratorPlugin::generateExecuable(
    const AST::Data * tree
    , QIODevice * out)
{
    Data data;

    d->reset(tree, &data);
    for (int i=0; i<tree->modules.size(); i++) {
        d->addModule(tree->modules[i]);
    }
    d->generateConstantTable();
    d->generateExternTable();

    QDataStream ds(out);
    data.versionMaj = 2;
    data.versionMin = 0;
    data.versionRel = 0;
    ds << data;
    GeneratorResult result;
    result.type = GenNotNativeExecuable;
    return result;
}

Q_EXPORT_PLUGIN(KumirCodeGeneratorPlugin)
