
#include "kumiranalizerplugin.h"
#include "analizer.h"
#include "errormessages/errormessages.h"
#include "lexer.h"

#include <QtCore>

#include "kumfilehandler.h"
#include "quickreferencewidget.h"

using namespace KumirAnalizer;

KumirAnalizerPlugin::KumirAnalizerPlugin()
    : ExtensionSystem::KPlugin()
    , teacherMode_(false)
    , kumFileHandler_(new KumFileHandler(this))
    , _quickReferenceWidget(NULL)
{
    analizers_ = QVector<Analizer*> (128, NULL);
}


Shared::Analizer::SourceFileInterface* KumirAnalizerPlugin::sourceFileHandler()
{
    return kumFileHandler_;
}


KumirAnalizerPlugin::~KumirAnalizerPlugin()
{
}

QWidget *KumirAnalizerPlugin::languageQuickReferenceWidget()
{
    bool hasGui = true;
#ifdef Q_WS_X11
    hasGui = getenv("DISPLAY")!=0;
#endif
    if (!_quickReferenceWidget && hasGui) {
        _quickReferenceWidget = new QuickReferenceWidget(this);
    }
    return _quickReferenceWidget;
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

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(KumirAnalizerPlugin, KumirAnalizer::KumirAnalizerPlugin)
#endif
