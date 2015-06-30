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
}

void ActorsHandler::resetActors()
{
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
        addActor(actor);
    }
}

void ActorsHandler::addActor(ActorInterface *actor)
{
    if (actors_.contains(actor))
        return;
    const QList<ActorInterface*> uses = actor->usesList();

    Q_FOREACH(ActorInterface * use, uses)
        addActor(use);

    QString canonicalName = actorCanonicalName<QByteArray>(actor->asciiModuleName());
    const QString & camelCaseName = toCamelCase(canonicalName);
    const QString & pythonicName = toPythonicName(canonicalName);
    names_.append(pythonicName);
    actors_.append(actor);
    wrappers_.append(createActorWrapper(wrappers_.size(), actor, camelCaseName));

    const ActorInterface::TypeList typeList = actor->typeList();
    Q_FOREACH(const ActorInterface::RecordSpecification & type, typeList) {
        TypeSpec typeSpec(actor, type);
        QString typeName = toCamelCase(type.asciiName);
        actorCustomTypes_[typeName] = typeSpec;
    }

    actor->connectSync(this, SLOT(handleActorSync()));
}

QString ActorsHandler::toCamelCase(const QString &s)
{
    QString result;
    bool nextIsUpper = true;
    for (int i=0; i<s.length(); i++) {
        if (s[i]=='@' || s[i]=='_') {
            nextIsUpper = true;
            result += "_";
        }
        else if (s[i].isSpace()) {
            nextIsUpper = true;
        }
        else {
            result += nextIsUpper
                    ? s[i].toUpper()
                    : s[i].toLower();
            nextIsUpper = false;
        }
    }
    return result;
}

QString ActorsHandler::toPythonicName(const QString &s)
{
    QString result = s.toLower();
    result.replace(" ", "_");
    result.replace("@", "_");
    result.replace("%1", "");
    result.replace("%2", "");
    result.replace("%3", "");
    result.replace("%4", "");
    result.replace("%5", "");
    while (result.endsWith("_"))
        result = result.left(result.length()-1);
    return result;
}

QString ActorsHandler::createActorWrapper(const int id, const ActorInterface *actor, const QString & camelCaseName)
{
    QString r = "import _kumir\n";

    const QList<ActorInterface*> usesList = actor->usesList();
    Q_FOREACH(ActorInterface* use, usesList) {
        QString canonicalImportName = actorCanonicalName<QByteArray>(use->asciiModuleName());
        QString pythonicImportName = toPythonicName(canonicalImportName);
        r += "import " + pythonicImportName + "\n";
    }
    r += "\n\n";

    r += "class " + camelCaseName + "Error(Exception):\n";
    r += "    def __init__(self, message):\n";
    r += "        self.message = message\n";
    r += "    def __str__(self):\n";
    r += "        return self.message\n";
    r += "    def __repr__(self):\n";
    r += "        return self.message\n";
    r += "\n\n";

    const ActorInterface::FunctionList functionList = actor->functionList();
    const ActorInterface::TypeList typeList = actor->typeList();

    QString moduleMethods;
    QMap<QByteArray,QString> moduleClasses;

    Q_FOREACH(const ActorInterface::RecordSpecification &type, typeList) {
        const QString className = toCamelCase(type.asciiName);
        QString rr = "class " + className + ":\n";

        // Default constructor
        rr += "    def __init__(self):\n";
        Q_FOREACH(const ActorInterface::Field &field, type.record) {
            const QString fieldName = toPythonicName(field.first);
            rr += "        self." + fieldName + " = ";
            if (ActorInterface::Int == field.second)
                rr += "0";
            else if (ActorInterface::Real == field.second)
                rr += "0.0";
            else if (ActorInterface::String == field.second)
                rr += "\"\"";
            else if (ActorInterface::Char == field.second)
                rr += "' '";
            else if (ActorInterface::Bool == field.second)
                rr += "False";
            else
                rr += "None";
            rr += "\n";
        }
        rr += "\n";

        // Field-passed constructor
        rr += "    def __init__(self";
        Q_FOREACH(const ActorInterface::Field &field, type.record) {
            const QString fieldName = toPythonicName(field.first);
            rr += ", " + fieldName;
        }
        rr += "):\n";
        Q_FOREACH(const ActorInterface::Field &field, type.record) {
            const QString fieldName = toPythonicName(field.first);
            rr += "        self." + fieldName + " = " + fieldName + "\n";
        }
        rr += "\n";
        moduleClasses[type.asciiName] = rr;
    }

    Q_FOREACH(const ActorInterface::Function & func, functionList) {
        if (isOperator(func.asciiName) && func.arguments.size() > 0) {
            const ActorInterface::Argument & firstArgument = func.arguments.at(0);
            if (
                    (func.asciiName!=":=") &&
                    (firstArgument.type == ActorInterface::RecordType) &&
                    (moduleClasses.contains(firstArgument.typeSpecification.asciiName))
               )
            {
                QString classDef = moduleClasses[firstArgument.typeSpecification.asciiName];
                classDef += createClassMethod(id, func, camelCaseName);
                moduleClasses[firstArgument.typeSpecification.asciiName] = classDef;
            }
        }
        else if (!isOperator(func.asciiName)) {
            moduleMethods += createActorMethod(id, func, camelCaseName);
        }
    }

    Q_FOREACH(const QByteArray & key, moduleClasses.keys()) {
        const QString & classDef = moduleClasses[key];
        r += classDef + "\n\n";
    }

    r += moduleMethods + "\n\n";

    // Class instantiate helpers
    Q_FOREACH(const ActorInterface::RecordSpecification &type, typeList) {
        const QString className = toCamelCase(type.asciiName);
        r += "def __init__" + className + "(";
        Q_FOREACH(const ActorInterface::Field &field, type.record) {
            const QString fieldName = toPythonicName(field.first);
            if (!r.endsWith('('))
                r += ", ";
            r += fieldName;
        }
        r += "):\n";
        r += "    return " + className + "(";
        Q_FOREACH(const ActorInterface::Field &field, type.record) {
            const QString fieldName = toPythonicName(field.first);
            if (!r.endsWith('('))
                r += ", ";
            r += fieldName;
        }
        r += ")\n\n";
    }

    return r;
}

QString ActorsHandler::createActorMethod(
        const int moduleId,
        const ActorInterface::Function &func,
        const QString &actorCamelCaseName
        )
{
    QString r;
    QString funcName = toPythonicName(func.asciiName);

    const ActorInterface::ArgumentList argList = func.arguments;
    QStringList formalArgs;
    QStringList passedArgs;

    for (int i=0; i<argList.size(); i++) {
        const ActorInterface::Argument & arg = argList.at(i);
        if (ActorInterface::OutArgument!=arg.accessType) {
            QString argName = toPythonicName(arg.asciiName);
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
            .arg(moduleId).arg(func.id).arg(passedArgs.join(", "));
    r += "    if __error__:\n";
    r += "        raise " + actorCamelCaseName + "Error(__error__)\n";
    r += "    else:\n";
    r += "        return __result__\n";
    r += "\n";

    return r;
}

QString ActorsHandler::createClassMethod(
        const int moduleId,
        const ActorInterface::Function &func,
        const QString &actorCamelCaseName
        )
{
    QString r;
    QString funcName = isOperator(func.asciiName)
            ? toPythonOperator(func.asciiName)
            : toPythonicName(func.asciiName)
              ;

    const ActorInterface::ArgumentList argList = func.arguments;
    QStringList formalArgs;
    QStringList passedArgs;

    for (int i=0; i<argList.size(); i++) {
        const ActorInterface::Argument & arg = argList.at(i);
        if (ActorInterface::OutArgument!=arg.accessType) {
            QString argName = toPythonicName(arg.asciiName);
            if (argName.isEmpty()) {
                argName = QString::fromLatin1("arg_%1").arg(i);
            }
            if (i > 0) {
                formalArgs.push_back(argName);
                passedArgs.push_back(argName);
            }
            else {
                formalArgs.push_back("self");
                passedArgs.push_back("self");
            }
        }
        else {
            passedArgs.push_back("0");
        }
    }

    r += "    def " + funcName + "(" + formalArgs.join(", ") + "):\n";
    r += QString::fromLatin1("        __error__, __result__ = _kumir.actor_call(%1, %2, [%3])\n")
            .arg(moduleId).arg(func.id).arg(passedArgs.join(", "));
    r += "        if __error__:\n";
    r += "            raise " + actorCamelCaseName + "Error(__error__)\n";
    r += "        else:\n";
    r += "            return __result__\n";
    r += "    \n";

    return r;
}

bool ActorsHandler::isOperator(const QByteArray &asciiName)
{
    static const QList<QByteArray> Ops = QList<QByteArray>()
            << "+" << "-" << "*" << "/" << "**"
            << ":=" << "<>" << "=" << "<" << ">"
            << "input" << "output";
    return Ops.contains(asciiName);
}

QString ActorsHandler::toPythonOperator(const QByteArray &asciiName)
{
    if (asciiName=="+")
        return "__add__";
    else if (asciiName=="-")
        return "__sub__";
    else if (asciiName=="*")
        return "__mul__";
    else if (asciiName=="/")
        return "__div__";
    else if (asciiName=="**")
        return "__pow__";
    else if (asciiName=="input")
        return "__init__";
    else if (asciiName=="output")
        return "__str__";
    else if (asciiName=="=")
        return "__eq__";
    else if (asciiName=="<>")
        return "__neq__";
    else
        return "__unknown__";
}

QVariant ActorsHandler::encodeActorCustomTypeArgument(const QVariant &from) const
{
    Q_ASSERT(QVariant::Map == from.type());
    const QMap<QString,QVariant> propertyMap = from.toMap();
    Q_ASSERT(propertyMap.contains("__type__.__name__"));
    const QString typeName = propertyMap["__type__.__name__"].toString();
    Q_ASSERT(actorCustomTypes_.contains(typeName));
    const TypeSpec typeSpec = actorCustomTypes_[typeName];
    const ActorInterface::RecordSpecification type = typeSpec.second;
    QVariantList result;
    Q_FOREACH(const ActorInterface::Field & field, type.record) {
        const QString fieldName = toPythonicName(field.first);
        QVariant fieldValue = QVariant::Invalid;
        if (propertyMap.contains(fieldName)) {
            // Avoid customized or deleted fields
            fieldValue = propertyMap[fieldName];
        }
        result.append(fieldValue);
    }
    return result;
}

QVariant ActorsHandler::decodeActorCustomTypeValue(
        const QString &typeName,
        const QVariant &from
        ) const
{
    if (!actorCustomTypes_.contains(typeName))
        return from;

    const TypeSpec & typeSpec = actorCustomTypes_[typeName];
    const ActorInterface::RecordSpecification & spec = typeSpec.second;
    ActorInterface * actor = typeSpec.first;
    const QString actorName = actorCanonicalName<QString>(actor->asciiModuleName());
    const QString moduleName = toPythonicName(actorName);

    QMap<QString,QVariant> propertyMap;
    const QVariantList fromList = from.toList();
    Q_ASSERT(spec.record.size() == fromList.size());
    propertyMap["__type__.__module__.__name__"] = moduleName;
    propertyMap["__type__.__name__"] = toCamelCase(spec.asciiName);
    for (int i=0; i<spec.record.size(); i++) {
        const QString fieldName = toPythonicName(spec.record[i].first);
        const QVariant fieldValue = fromList.at(i);
        propertyMap[fieldName] = fieldValue;
    }
    return propertyMap;
}

QVariant ActorsHandler::call(int moduleId, int functionId, const QVariantList &arguments)
{
    ActorInterface * actor = actors_[moduleId];
    QVariantList args = arguments;
    const ActorInterface::Function func = actor->functionList()[functionId];
    const ActorInterface::ArgumentList arglist = func.arguments;
    Q_ASSERT(arglist.size()==args.size());
    for (int i=0; i<args.size(); i++) {
        QVariant & argument = args[i];
        const ActorInterface::Argument & argSpec = arglist[i];
        if (ActorInterface::RecordType==argSpec.type) {
            if (QVariant::Map == argument.type() &&
                    argument.toMap().contains("__type__.__name__") &&
                    actorCustomTypes_.contains(argument.toMap()["__type__.__name__"].toString())
                    )
            {
                argument = encodeActorCustomTypeArgument(argument);
            }
            else {
                argument = actor->customValueFromString(argSpec.typeSpecification.asciiName,
                                                   argument.toString());
                if (!argument.isValid()) {
                    QVariantList errorResult;
                    errorResult.append(tr("Can't convert string argument to '%1'").
                                       arg(
                                           QString::fromLatin1(
                                               argSpec.typeSpecification.asciiName
                                               )
                                           )
                                       );
                    errorResult.append(QVariant());
                    return errorResult;
                }
            }
        }
    }
    EvaluationStatus status = actor->evaluate(functionId, args);
    QVariantList result;
    result.append(QString()); // first item is an error text
    result.append(QVariant()); // second item is a function result
    switch (status) {
    case ES_Error:
        result[0] = actor->errorText();
        break;
    case ES_StackResult:
        result[1] = decodeActorCustomTypeValue(
                    toCamelCase(func.returnTypeSpecification.asciiName),
                    actor->result()
                    );
        break;
    case ES_RezResult:
        result[1] = actor->algOptResults();
        break;
    case ES_StackRezResult: {
        QVariantList tuple = actor->algOptResults();
        tuple.prepend(decodeActorCustomTypeValue(
                          toCamelCase(func.returnTypeSpecification.asciiName),
                          actor->result()
                          ));
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
