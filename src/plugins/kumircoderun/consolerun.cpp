#include "consolerun.h"
#include "extensionsystem/pluginmanager.h"
#include "interfaces/actorinterface.h"
#include "util.h"

namespace KumirCodeRun {
namespace Console {



VM::ExternalModuleLoadFunctor::NamesList
ExternalModuleLoadFunctor::operator() (
    const Kumir::String & moduleName,
    const std::string & canonicalModuleFileName)
    /* throws std::string, Kumir::String */
{
    using namespace ExtensionSystem;
    using namespace Shared;

    ActorInterface * actor = Util::findActor(moduleName);

    if (! actor) {
        const QString localError =
                PluginManager::instance()->loadExtraModule(canonicalModuleFileName);
        if (localError.length() > 0) {
            const QString qModuleName = QString::fromStdWString(moduleName);
            const Kumir::String errorMessage =
                    QString::fromUtf8(
                        "Ошибка загрузки исполнителя %1: %2"
                        ).arg(qModuleName).arg(localError).toStdWString();
            throw errorMessage;
        }
        actor = Util::findActor(moduleName);
    }

    if (! actor) {
        const QString qModuleName = QString::fromStdWString(moduleName);
        QString qFileName = QString::fromStdString(canonicalModuleFileName);
        if (qFileName.length()>0)
            qFileName[0] = qFileName[0].toUpper();
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
        foreach ( const QString & functionName, actor->funcList() ) {
            namesList.push_back(functionName.toStdWString());
        }
    }

    return namesList;
}


} // namespace Console
}
