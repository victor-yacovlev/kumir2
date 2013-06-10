#include <sstream>
#include <cstdlib>
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
    const AST::DataPtr tree
    , QByteArray & out, DebugLevel debugLevel)
{
    Data data;

    d->reset(tree, &data, debugLevel);
    AST::ModulePtr userModule, teacherModule;
    AST::ModulePtr linkedModule = AST::ModulePtr(new AST::Module);
    for (int i=0; i<tree->modules.size(); i++) {
        AST::ModulePtr mod = tree->modules[i];
        if (mod->header.type == AST::ModTypeUser && mod->header.name.isEmpty()) {
            userModule = mod;
        }
        else if (mod->header.type == AST::ModTypeTeacher && mod->header.name.isEmpty()) {
            teacherModule = mod;
        }
        else {
            d->addModule(tree->modules[i]);
        }
    }
    if (teacherModule) {
        linkedModule->impl.globals = teacherModule->impl.globals;
        linkedModule->impl.initializerBody = teacherModule->impl.initializerBody;
        linkedModule->impl.algorhitms = teacherModule->impl.algorhitms;
        linkedModule->header.algorhitms = teacherModule->header.algorhitms;
    }
    linkedModule->impl.globals += userModule->impl.globals;
    linkedModule->impl.initializerBody += userModule->impl.initializerBody;
    linkedModule->impl.algorhitms += userModule->impl.algorhitms;
    linkedModule->header.algorhitms += userModule->header.algorhitms;
    tree->modules.push_back(linkedModule);
    d->addModule(linkedModule);
    d->generateConstantTable();
    d->generateExternTable();
    tree->modules.pop_back();

    data.versionMaj = 2;
    data.versionMin = 0;
    data.versionRel = 0;
    std::list<char> buffer;
    Bytecode::bytecodeToDataStream(buffer, data);
    for (std::list<char>::const_iterator it=buffer.begin(); it!=buffer.end(); ++it) {
        out.push_back(*it);
    }
    return QPair<QString,QString>("", MIME_BYTECODE_BINARY);
}



Q_EXPORT_PLUGIN(KumirCodeGeneratorPlugin)
