#include "generator.h"
#include "kumircodegeneratorplugin.h"
#include "bytecode/data.h"

using namespace KumirCodeGenerator;

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
    Bytecode::Data data;
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
