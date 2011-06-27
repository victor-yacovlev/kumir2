#include "plugin.h"
#include "variant.h"

namespace KumirCodeRun {

Plugin::Plugin() :
    ExtensionSystem::KPlugin()
{
}

QString Plugin::initialize(const QStringList &)
{
    qRegisterMetaType<Variant>("KumirCodeRun::Variant");
    qRegisterMetaType<VariantList>("KumirCodeRun::VariantList");
    return "";
}

} // namespace KumirCodeRun
