#include <sstream>
#include <cstdlib>
#include "stdlib/kumirstdlib.hpp"
#include "vm/variant.hpp"
#include "vm/vm_bytecode.hpp"
#include "generator.h"
#include "kumircodegeneratorplugin.h"
#include "extensionsystem/pluginmanager.h"

using namespace KumirCodeGenerator;
using namespace Bytecode;

static const QString MIME_BYTECODE_BINARY   = QString::fromAscii("application/vnd.niisi.kumir2.bytecode.binary");
static const QString MIME_BYTECODE_TEXT     = QString::fromAscii("text/plain");

KumirCodeGeneratorPlugin::KumirCodeGeneratorPlugin()
    : KPlugin()
    , d(new Generator(this))
    , textMode_(false)
{
}

QList<ExtensionSystem::CommandLineParameter>
KumirCodeGeneratorPlugin::acceptableCommandLineParameters() const
{
    using ExtensionSystem::CommandLineParameter;
    QList<CommandLineParameter> result;
    result << CommandLineParameter(
                  false,
                  's', "assembly",
                  tr("Generate bytecode assemby text instead of executable code")
                  );
    result << CommandLineParameter(
                  false,
                  'g', "debuglevel",
                  tr("Generate code with debug level from 0 (nothing) to 2 (maximum debug information)"),
                  QVariant::Int, false
                  );
    return result;
}

QString KumirCodeGeneratorPlugin::initialize(const QStringList &/*configurationArguments*/,
                                             const ExtensionSystem::CommandLine &runtimeArguments)
{    
    textMode_ = runtimeArguments.hasFlag('s');
    DebugLevel debugLevel = DebugLevel::LinesOnly;
    if (runtimeArguments.value('g').isValid()) {
        int level = runtimeArguments.value('g').toInt();
        level = qMax(0, level);
        level = qMin(2, level);
        debugLevel = DebugLevel(level);
    }
    setDebugLevel(debugLevel);
    return QString();
}

void KumirCodeGeneratorPlugin::setOutputToText(bool flag)
{
    textMode_ = false;
}

void KumirCodeGeneratorPlugin::setDebugLevel(DebugLevel debugLevel)
{
    d->setDebugLevel(debugLevel);
}

void KumirCodeGeneratorPlugin::start()
{

}

void KumirCodeGeneratorPlugin::stop()
{

}

void KumirCodeGeneratorPlugin::generateExecuable(
        const AST::DataPtr tree,
        QByteArray & out,
        QString & mimeType,
        QString & fileSuffix
        )
{
    Data data;

    QList<AST::ModulePtr> & modules = tree->modules;

    d->reset(tree, &data);
    AST::ModulePtr userModule, teacherModule;
    AST::ModulePtr linkedModule = AST::ModulePtr(new AST::Module);
    for (int i=0; i<modules.size(); i++) {
        AST::ModulePtr mod = modules[i];
        if (mod->header.type == AST::ModTypeUserMain) {
            userModule = mod;
        }
        else if (mod->header.type == AST::ModTypeTeacherMain) {
            teacherModule = mod;
        }
        else {
            d->addModule(tree->modules[i]);
        }
    }    
    linkedModule->impl.globals = userModule->impl.globals;
    linkedModule->impl.initializerBody = userModule->impl.initializerBody;
    linkedModule->impl.algorhitms = userModule->impl.algorhitms;
    linkedModule->header.algorhitms = userModule->header.algorhitms;
    modules.removeAll(userModule);
    if (teacherModule) {
        linkedModule->impl.globals += teacherModule->impl.globals;
        linkedModule->impl.initializerBody += teacherModule->impl.initializerBody;
        linkedModule->impl.algorhitms += teacherModule->impl.algorhitms;
        linkedModule->header.algorhitms += teacherModule->header.algorhitms;
        modules.removeAll(teacherModule);
    }
    modules.push_back(linkedModule);
    d->addModule(linkedModule);
    d->generateConstantTable();
    d->generateExternTable();
    modules.pop_back();
    modules.push_back(userModule);
    if (teacherModule) {
        modules.push_back(teacherModule);
    }

    data.versionMaj = 2;
    data.versionMin = 0;
    data.versionRel = 90;
    std::list<char> buffer;
    if (textMode_) {
        std::ostringstream stream;
        Bytecode::bytecodeToTextStream(stream, data);
        const std::string text = stream.str();
        out = QByteArray::fromRawData(text.c_str(), text.size());
        mimeType = MIME_BYTECODE_TEXT;
        fileSuffix = ".kod.txt";
    }
    else {
        out.clear();
        Bytecode::bytecodeToDataStream(buffer, data);
        for (std::list<char>::const_iterator it=buffer.begin(); it!=buffer.end(); ++it) {
            out.push_back(*it);
        }
        mimeType = MIME_BYTECODE_BINARY;
        fileSuffix = ".kod";
    }
}



Q_EXPORT_PLUGIN(KumirCodeGeneratorPlugin)
