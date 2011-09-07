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

QPair<QString,QString> KumirCodeGeneratorPlugin::generateExecuable(
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

    out->open(QIODevice::WriteOnly);

    QDataStream ds(out);
    data.versionMaj = 2;
    data.versionMin = 0;
    data.versionRel = 0;
    ds << data;

    out->close();

    return QPair<QString,QString>("", MIME_BYTECODE_BINARY);
}

Q_EXPORT_PLUGIN(KumirCodeGeneratorPlugin)
