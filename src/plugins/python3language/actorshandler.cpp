#include "actorshandler.h"

#include "extensionsystem/pluginmanager.h"

namespace Python3Language {

ActorsHandler::ActorsHandler(QObject *parent)
    : QObject(parent)
    , syncSemaphore_(new QSemaphore(0))
{
    initializeActors();
}

ActorsHandler* ActorsHandler::self = 0;

ActorsHandler* ActorsHandler::instance(QObject *parent)
{
    if (!self) {
        self = new ActorsHandler(parent);
    }
    return self;
}

void ActorsHandler::reset()
{
    // Ensure semaphore value == 0
    int sem = syncSemaphore_->available();
    while (!sem) {
        syncSemaphore_->release();
        sem = syncSemaphore_->available();
    }
    while (sem) {
        syncSemaphore_->acquire();
        sem = syncSemaphore_->available();
    }

    // Reset all registered actors
    Q_FOREACH(ActorInterface * actor, actors_) {
        actor->reset();
    }
}

void ActorsHandler::initializeActors()
{
    using namespace ExtensionSystem;
    QList<ActorInterface*> actors =
            PluginManager::instance()->findPlugins<ActorInterface>();
    names_.reserve(actors.size());
    wrappers_.reserve(actors.size());
    actors_.reserve(actors.size());

    Q_FOREACH(ActorInterface * actor, actors) {
        const QPair<QString,QString> names = generateActorNames(actor);
        const QString & camelCaseName = names.first;
        const QString & pythonicName = names.second;
        names_.append(pythonicName);
        actors_.append(actor);
        wrappers_.append(createActorWrapper(wrappers_.size(), actor, camelCaseName));
        actor->connectSync(this, SLOT(handleActorSync()));
    }
}

QPair<QString,QString> ActorsHandler::generateActorNames(const ActorInterface *actor)
{
    QString canonicalName = actorCanonicalName<QByteArray>(actor->asciiModuleName());
    QString camelCaseName;
    QString pythonicName;
    bool nextIsUpper = true;
    for (int i=0; i<canonicalName.length(); i++) {
        const QChar source = canonicalName[i];
        if (source.isSpace() || '@' == source) {
            nextIsUpper = true;
            pythonicName += "_";
        }
        else {
            if (nextIsUpper)
                camelCaseName += source.toUpper();
            else
                camelCaseName += source.toLower();
            pythonicName += source.toLower();
            nextIsUpper = false;
        }
    }
    return QPair<QString,QString>(camelCaseName, pythonicName);
}

QString ActorsHandler::createActorWrapper(const int id, const ActorInterface *actor, const QString & camelCaseName)
{
    QString r = "import _kumir\n\n\n";
    r += "class " + camelCaseName + "Error(Exception):\n";
    r += "    def __init__(self, message):\n";
    r += "        self.message = message\n";
    r += "    def __str__(self):\n";
    r += "        return self.message\n";
    r += "\n\n";

    const ActorInterface::FunctionList functionList = actor->functionList();

    Q_FOREACH(const ActorInterface::Function & func, functionList) {

        QString funcName = func.asciiName.toLower();
        funcName = funcName.replace(' ', '_');
        funcName = funcName.replace('@', '_');

        const ActorInterface::ArgumentList argList = func.arguments;
        QStringList formalArgs;
        QStringList passedArgs;

        for (int i=0; i<argList.size(); i++) {
            const ActorInterface::Argument & arg = argList.at(i);
            if (ActorInterface::OutArgument!=arg.accessType) {
                QString argName = arg.asciiName.toLower();
                argName = argName.replace(' ', '_');
                argName = argName.replace('@', '_');
                if (argName.isEmpty()) {
                    argName = QString::fromLatin1("arg_%1").arg(i);
                }
                formalArgs.push_back(argName);
                passedArgs.push_back(argName);
            }
            else {
                passedArgs.push_back("0");
            }
        }

        r += "def " + funcName + "(" + formalArgs.join(", ") + "):\n";
        r += QString::fromLatin1("    __error__, __result__ = _kumir.actor_call(%1, %2, [%3])\n")
                .arg(id).arg(func.id).arg(passedArgs.join(", "));
        r += "    if __error__:\n";
        r += "        raise " + camelCaseName + "Error(__error__)\n";
        r += "    else:\n";
        r += "        return __result__\n";
        r += "\n";
    }
    return r;
}

QVariant ActorsHandler::call(int moduleId, int functionId, const QVariantList &arguments)
{
    ActorInterface * actor = actors_[moduleId];
    EvaluationStatus status = actor->evaluate(functionId, arguments);
    QVariantList result;
    result.append(QString()); // first item is an error text
    result.append(QVariant()); // second item is a function result
    switch (status) {
    case ES_Error:
        result[0] = actor->errorText();
        break;
    case ES_StackResult:
        result[1] = actor->result();
        break;
    case ES_RezResult:
        result[1] = actor->algOptResults();
        break;
    case ES_StackRezResult: {
        QVariantList tuple = actor->algOptResults();
        tuple.prepend(actor->result());
        result[1] = tuple;
    }
        break;
    case ES_Async:
        syncSemaphore_->acquire();
        result[0] = actor->errorText();
        result[1] = actorCallResult_;
        break;
    default:
        break;
    }
    return result;
}

void ActorsHandler::handleActorSync()
{
    actorCallResult_.clear();
    ActorInterface * actor = qobject_cast<ActorInterface*>(sender());
    if (actor->algOptResults().size() > 0 && QVariant::Invalid != actor->result()) {
        QVariantList tuple = actor->algOptResults();
        tuple.prepend(actor->result());
        actorCallResult_ = tuple;
    }
    else if (actor->algOptResults().size() > 0) {
        actorCallResult_ = actor->algOptResults();
    }
    else {
        actorCallResult_ = actor->result();
    }
    syncSemaphore_->release();
}

} // namespace Python3Language
