#include "consolerun.h"
#include "extensionsystem/pluginmanager.h"
#include "interfaces/actorinterface.h"
#include "util.h"

#include <QWidget>

namespace KumirCodeRun {
namespace Console {



VM::ExternalModuleLoadFunctor::NamesList
ExternalModuleLoadFunctor::operator() (
    const Kumir::String & moduleLocalizedName,
    const std::string & moduleAsciiName)
    /* throws std::string, Kumir::String */
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
            throw errorMessage;
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
        throw errorMessage;
    }

    NamesList namesList;
    if (actor) /* must check in case of exceptions disabled */ {
        foreach ( const ActorInterface::Function & function, actor->functionList() ) {
            namesList.push_back(std::string(function.asciiName.constData()));
        }
    }

    return namesList;
}


} // namespace Console
}
