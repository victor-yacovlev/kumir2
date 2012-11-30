#include <sstream>
#include "stdlib/kumirstdlib.hpp"
#include "vm/variant.hpp"
#include "vm/vm_bytecode.hpp"
#include "generator.h"
#include "kumircodegeneratorplugin.h"

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
    , QByteArray & out, DebugLevel debugLevel)
{
    Data data;

    d->reset(tree, &data, debugLevel);
    for (int i=0; i<tree->modules.size(); i++) {
        d->addModule(tree->modules[i]);
    }
    d->generateConstantTable();
    d->generateExternTable();

    data.versionMaj = 2;
    data.versionMin = 0;
    data.versionRel = 0;

    std::list<char> buffer;
    Bytecode::bytecodeToDataStream(buffer, data);
    for (auto it=buffer.begin(); it!=buffer.end(); ++it) {
        out.push_back(*it);
    }

    return QPair<QString,QString>("", MIME_BYTECODE_BINARY);
}



Q_EXPORT_PLUGIN(KumirCodeGeneratorPlugin)
