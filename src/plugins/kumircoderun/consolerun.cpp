#include "consolerun.h"
#include "extensionsystem/pluginmanager.h"
#include "interfaces/actorinterface.h"
#include "util.h"

#include <QWidget>
#include <QMap>

namespace KumirCodeRun {
namespace Console {

static QMap<Shared::ActorInterface*, QWidget*> ACTOR_WINDOWS;

VM::ExternalModuleLoadFunctor::NamesList
ExternalModuleLoadFunctor::operator() (
        const std::string & moduleAsciiName,
        const Kumir::String & moduleLocalizedName
        , Kumir::String * error
    )
{
    using namespace ExtensionSystem;
    using namespace Shared;

    std::string moduleCanonicalFileName;
    bool forceUpper = true;
    for (int i=0; i<moduleAsciiName.size(); i++) {
        char ch = moduleAsciiName[i];
        QChar qch(ch);
        if (forceUpper) {
            forceUpper = false;
            qch = qch.toUpper();
            ch = qch.toLatin1();
        }
        if (ch != ' ') {
            moduleCanonicalFileName.push_back(ch);
        }
        else {
            forceUpper = true;
        }
    }

    ActorInterface * actor = Util::findActor(moduleAsciiName);

    if (! actor) {
        const QString localError =
                PluginManager::instance()->loadExtraModule(moduleCanonicalFileName);
        if (localError.length() > 0) {
            const QString qModuleName = QString::fromStdWString(moduleLocalizedName);
            const Kumir::String errorMessage =
                    QString::fromUtf8(
                        "Ошибка загрузки исполнителя %1: %2"
                        ).arg(qModuleName).arg(localError).toStdWString();
            if (error) {
                error->assign(errorMessage);
            }
            return NamesList();
        }
        actor = Util::findActor(moduleAsciiName);
    }

    if (! actor) {
        const QString qModuleName = QString::fromStdWString(moduleLocalizedName);
        QString qFileName = QString::fromStdString(moduleCanonicalFileName);
#if defined(Q_OS_WIN32)
        qFileName += ".dll";
#elif defined(Q_OS_MACX)
        qFileName = "lib"+qFileName+".dylib";
#else
        qFileName = "lib"+qFileName+".so";
#endif
        const Kumir::String errorMessage =
                QString::fromUtf8(
                    "Ошибка загрузки исполнителя %1: "
                    "модуль %2 не содежит данного исполнителя"
                    ).arg(qModuleName).arg(qFileName).toStdWString();
        if (error) {
            error->assign(errorMessage);
            return NamesList();
        }
    }

    NamesList namesList;
    if (actor) /* must check in case of exceptions disabled */ {
        foreach ( const ActorInterface::Function & function, actor->functionList() ) {
            namesList.push_back(std::string(function.asciiName.constData()));
        }
    }

    bool gui = true;
#ifdef Q_OS_LINUX
    gui = gui && getenv("DISPLAY")!=0;
#endif

    if (actor && gui && actor->mainWidget()) {
        if (!ACTOR_WINDOWS.contains(actor)) {
            ACTOR_WINDOWS[actor] = actor->mainWidget();
        }
    }

    return namesList;
}


void ExternalModuleResetFunctor::operator ()(const std::string & moduleAsciiName, const Kumir::String & moduleLocalizedName, Kumir::String * error)
{
    using namespace Shared;
    using namespace ExtensionSystem;

    ActorInterface * actor = Util::findActor(moduleAsciiName);

    if (actor) {
        actor->reset();
        if (callFunctor_) {
            callFunctor_->checkForActorConnected(moduleAsciiName);
        }
        if (ACTOR_WINDOWS.contains(actor)) {
            QWidget * actorWindow = ACTOR_WINDOWS[actor];
            actorWindow->setVisible(true);
        }
    }
    else {
        const QString qModuleName = QString::fromStdWString(moduleLocalizedName);
        const Kumir::String errorMessage =
                QString::fromUtf8(
                    "Ошибка инициализации исполнителя: нет исполнителя "
                    "с именем %1"
                    ).arg(qModuleName).toStdWString();
        if (error) {
            error->assign(errorMessage);
        }
        return;
    }
}

} // namespace Console
}
