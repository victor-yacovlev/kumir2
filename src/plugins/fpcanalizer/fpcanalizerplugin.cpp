#include "fpcanalizerplugin.h"
#include "fpcanalizer.h"

namespace FpcAnalizer {

QString FpcAnalizerPlugin::initialize(const QStringList &, const CommandLine &)
{
    lastInstanceIndex_ = 0;
    return "";
}

void FpcAnalizerPlugin::updateSettings(const QStringList &)
{

}

Analizer::InstanceInterface* FpcAnalizerPlugin::createInstance()
{
    lastInstanceIndex_ ++;
    return new FpcAnalizer(this, lastInstanceIndex_);
}

}

Q_EXPORT_PLUGIN2(FPCAnalizerPlugin, FpcAnalizer::FpcAnalizerPlugin)
