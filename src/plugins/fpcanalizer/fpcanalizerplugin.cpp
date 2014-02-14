#include "fpcanalizerplugin.h"
#include "fpcanalizer.h"

namespace FpcAnalizer {

QString FpcAnalizerPlugin::initialize(const QStringList &, const CommandLine &)
{
    return "";
}

void FpcAnalizerPlugin::updateSettings(const QStringList &)
{

}

Analizer::InstanceInterface* FpcAnalizerPlugin::createInstance()
{
    return new FpcAnalizer(this);
}

}

Q_EXPORT_PLUGIN2(FPCAnalizerPlugin, FpcAnalizer::FpcAnalizerPlugin)
