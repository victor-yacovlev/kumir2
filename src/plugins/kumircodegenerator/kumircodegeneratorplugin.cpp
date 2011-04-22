#include "kumircodegeneratorplugin.h"

using namespace KumirCodeGenerator;

KumirCodeGeneratorPlugin::KumirCodeGeneratorPlugin()
{
}

KumirCodeGeneratorPlugin::~KumirCodeGeneratorPlugin()
{
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


Q_EXPORT_PLUGIN(KumirCodeGeneratorPlugin)
