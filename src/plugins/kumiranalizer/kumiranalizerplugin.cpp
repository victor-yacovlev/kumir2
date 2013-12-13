
#include "kumiranalizerplugin.h"
#include "analizer.h"
#include "errormessages/errormessages.h"
#include "lexer.h"

#include <QtCore>


using namespace KumirAnalizer;


KumirAnalizerPlugin::KumirAnalizerPlugin()
    : ExtensionSystem::KPlugin()
    , teacherMode_(false)
{
    analizers_ = QVector<Analizer*> (128, NULL);
}


KumirAnalizerPlugin::~KumirAnalizerPlugin()
{
}


QString KumirAnalizerPlugin::initialize(const QStringList &configurationArguments,
                                        const ExtensionSystem::CommandLine &runtimeArguments)
{
    QLocale::Language language = QLocale::Russian;

    Analizer::setModuleAlwaysAvailable(QString::fromUtf8("Стандартные функции"));

    Q_FOREACH (const QString &arg, configurationArguments) {
        if (arg.startsWith("language=")) {
            const QString lang = arg.mid(9);
            const QLocale loc(lang);
            if (loc.language()!=QLocale::C) {
                language = loc.language();
                break;
            }
        }
        else if (arg.startsWith("preload=")) {
            const QString modname = arg.mid(8);
            Analizer::setModuleAlwaysAvailable(modname);
        }
    }

    Analizer::setSourceLanguage(myResourcesDir(), language);

    Shared::ErrorMessages::loadMessages("KumirAnalizer");
    teacherMode_ = configurationArguments.contains("teacher", Qt::CaseInsensitive);
    return "";
}

void KumirAnalizerPlugin::start()
{

}

void KumirAnalizerPlugin::stop()
{

}

Shared::Analizer::InstanceInterface * KumirAnalizerPlugin::createInstance()
{
    Analizer * instance = new Analizer(this, teacherMode_);
    return instance;
}

Q_EXPORT_PLUGIN2(KumirAnalizerPlugin, KumirAnalizer::KumirAnalizerPlugin)
