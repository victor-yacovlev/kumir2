#include "kumircompilerplugin.h"

#include <QtCore>
#include "kumiranalizer/analizer.h"
#include "kumiranalizer/kumiranalizerplugin.h"

using namespace KumirCompiler;
using namespace KumirAnalizer;


KumirCompilerPlugin::KumirCompilerPlugin()
{


}


KumirCompilerPlugin::~KumirCompilerPlugin()
{

}


QString KumirCompilerPlugin::initialize(const QStringList &arguments)
{
    Q_UNUSED(arguments)
    return "";
}

void KumirCompilerPlugin::start()
{

}

void KumirCompilerPlugin::stop()
{

}


Q_EXPORT_PLUGIN(KumirCompilerPlugin)
