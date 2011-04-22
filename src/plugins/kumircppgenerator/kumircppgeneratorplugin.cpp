#include "kumircppgeneratorplugin.h"

#include <QtCore>


using namespace KumirCppGenerator;

KumirCppGeneratorPlugin::KumirCppGeneratorPlugin()
{
}

KumirCppGeneratorPlugin::~KumirCppGeneratorPlugin()
{
}

QString KumirCppGeneratorPlugin::initialize(const QStringList &arguments)
{
    Q_UNUSED(arguments)
    return "";
}

void KumirCppGeneratorPlugin::start()
{

}

void KumirCppGeneratorPlugin::stop()
{

}

Q_EXPORT_PLUGIN(KumirCppGeneratorPlugin)
