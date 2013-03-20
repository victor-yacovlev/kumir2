#include "commonrun.h"
#include "util.h"
#include "extensionsystem/pluginmanager.h"

namespace KumirCodeRun {

namespace Common {

CriticalSectionLocker::CriticalSectionLocker()
    : mutex_(new QMutex)
{
}

void CriticalSectionLocker::lock()
{
    mutex_->lock();
}

void CriticalSectionLocker::unlock()
{
    mutex_->unlock();
}

CriticalSectionLocker::~CriticalSectionLocker()
{
    delete mutex_;
}


void ExternalModuleResetFunctor::operator ()(const Kumir::String & moduleName)
{
    using namespace ExtensionSystem;

    ActorInterface * actor = Util::findActor(moduleName);

    if (actor) {
        actor->reset();
    }
    else {
        const QString qModuleName = QString::fromStdWString(moduleName);
        const Kumir::String errorMessage =
                QString::fromUtf8(
                    "Ошибка инициализации исполнителя: нет исполнителя "
                    "с именем %1"
                    ).arg(qModuleName).toStdWString();
        throw errorMessage;
    }
}

ExternalModuleCallFunctor::ExternalModuleCallFunctor(QObject *parent)
    : QObject(parent)
    , finishedFlag_(false)
    , finishedMutex_(new QMutex)
{
}

ExternalModuleCallFunctor::~ExternalModuleCallFunctor()
{
    delete finishedMutex_;
}

AnyValue ExternalModuleCallFunctor::operator ()
(
    const String & moduleName,
    const uint16_t algKey,
    VariableReferencesList alist
)
{
    // Clear state
    finishedFlag_ = false;

    // Convert STL+Kumir into Qt value types
    const QString qModuleName = QString::fromStdWString(moduleName);
    const quint16 qAlgKey = quint16(algKey);
    QVariantList arguments;
    for (auto it=alist.begin(); it!=alist.end(); ++it) {
        const QVariant qVal = Util::VariableToQVariant(*it);
        arguments.push_back(qVal);
    }

    // Find an actor (or throw)
    Shared::ActorInterface * actor = Util::findActor(moduleName);

    if (! actor) {
        const String errorMessage = QString::fromUtf8(
                    "Нельзя вызвать алгоритм из %1: исполнитель не загружен"
                    ).arg(qModuleName).toStdWString();
        throw errorMessage;
    }

    if (! actor) {
        // Prevent further execution if no exceptions support
        return AnyValue();
    }

    if (actor->evaluate(qAlgKey, arguments)==Shared::ES_Async) {
        // Wait for actor thread to finish
        forever {
            bool done = false;
            finishedMutex_->lock();
            done = finishedFlag_;
            finishedMutex_->unlock();
            if (!done) {
                Util::SleepFunctions::msleep(1);
            }
            else {
                break;
            }
        }
    }

    // Collect actor result
    const QString errorMessage = actor->errorText();
    const QVariant returnValue = actor->result();
    const QVariantList argumentReturnValues = actor->algOptResults();

    // Check for runtime error
    if (errorMessage.length()>0) {
        const String message = errorMessage.toStdWString();
        throw message;
    }

    // Get result
    AnyValue result = Util::QVariantToValue(returnValue, 0);

    // Check for out and in/out arguments and store them
    for (size_t i=0; i<alist.size(); i++) {
        Variable var = alist.at(i);
        const QVariant & qval = argumentReturnValues.at(i);
        if (var.isReference() && qval.isValid()) {
            quint8 dim = quint8(var.dimension());
            const AnyValue kval = Util::QVariantToValue(qval, dim);
            var.setValue(kval);
        }
    }

    // Disconnect an actor
    releaseActor(actor);

    // Return a result
    return result;
}

void ExternalModuleCallFunctor::releaseActor(ActorInterface *actor)
{
    QObject * actorObject = dynamic_cast<QObject*>(actor);
    disconnect(actorObject, SIGNAL(sync()), this, SLOT(handleActorSync()));
}




void ExternalModuleCallFunctor::handleActorSync()
{
    finishedMutex_->lock();
    finishedFlag_ = true;
    finishedMutex_->unlock();
}


String CustomTypeToStringFunctor::operator ()(const Variable & variable)
        /*throws Kumir::String, std::string*/
{
    const QString modName =
            QString::fromStdWString(variable.recordModuleName());
    const QString className =
            QString::fromStdWString(variable.recordClassName());

    ActorInterface * actor = Util::findActor(variable.recordModuleName());

    String result;
    if (actor) {
        Shared::ActorInterface::CustomType ct;
        ct.first = className;
        QVariant value = Util::VariableToQVariant(variable);
        const QString qString = actor->customValueToString(ct, value);
        result = qString.toStdWString();
    }
    else {
        throw QString::fromUtf8("Нет такого исполнителя: %1").arg(modName)
                .toStdWString();
    }
    return result;
}

AnyValue CustomTypeFromStringFunctor::operator ()(
        const String & source,
        const String & moduleName,
        const String & typeName
        ) /*throws Kumir::String, std::string*/
{
    AnyValue result;
    QString errorMessage;
    const QString modName =
            QString::fromStdWString(moduleName);
    const QString className =
            QString::fromStdWString(typeName);
    const QString qString = QString::fromStdWString(source);

    ActorInterface * actor = Util::findActor(moduleName);
    if (actor) {
        ActorInterface::CustomType ct;
        ct.first = className;
        const QVariant value = actor->customValueFromString(ct, qString);
        if (value.isValid()) {
            result = Util::QVariantToValue(value, 0);
        }
        else {
            errorMessage = QString::fromUtf8("Ошибка ввода значения типа \"%1\"")
                    .arg(className);
        }
    }
    else {
        errorMessage = QString::fromUtf8("Исполнитель не доступен: %1")
                .arg(modName);
    }
    if (errorMessage.length()>0)
        throw errorMessage.toStdWString();
    return result;
}


}}
