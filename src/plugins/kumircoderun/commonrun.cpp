#include "commonrun.h"
#include "util.h"
#include "extensionsystem/pluginmanager.h"

#include <QWidget>

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
    const std::string & asciiModuleName,
    const String & moduleName,
    const uint16_t algKey,
    VariableReferencesList alist, Kumir::String * error
)
{
    // Clear state
    finishedFlag_ = false;

    // Convert STL+Kumir into Qt value types
    const QString qModuleName = QString::fromStdWString(moduleName);
    const quint16 qAlgKey = quint16(algKey);
    QVariantList arguments;
    for (std::deque<Variable>::const_iterator it=alist.begin(); it!=alist.end(); ++it) {
        const QVariant qVal = Util::VariableToQVariant(*it);
        arguments.push_back(qVal);
    }

    // Find an actor (or throw)
    Shared::ActorInterface * actor = Util::findActor(asciiModuleName);

    if (! actor) {
        const String errorMessage = QString::fromUtf8(
                    "Нельзя вызвать алгоритм из %1: исполнитель не загружен"
                    ).arg(qModuleName).toStdWString();
        if (error) {
            error->assign(errorMessage);
        }
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
        if (error) {
            error->assign(message);
        }
        return AnyValue();
    }

    // Get result
    AnyValue result = Util::QVariantToValue(returnValue, 0);

    // Check for out and in/out arguments and store them
    for (size_t i=0; i<qMin((size_t)argumentReturnValues.size(), alist.size()); i++) {
        Variable var = alist.at(i);
        const QVariant & qval = argumentReturnValues.at(i);
        if (var.isReference() && qval.isValid()) {
            quint8 dim = quint8(var.dimension());
            const AnyValue kval = Util::QVariantToValue(qval, dim);
            var.setValue(kval);
        }
    }

    // Return a result
    return result;
}

void ExternalModuleCallFunctor::checkForActorConnected(const std::string &asciiModuleName)
{
    using namespace Shared;
    using namespace ExtensionSystem;

    ActorInterface * actor = Util::findActor(asciiModuleName);

    if (actor) {
        if (connectedActors_.count(actor)==0) {
            actor->connectSync(this, SLOT(handleActorSync()));
            connectedActors_.push_back(actor);
        }
    }
}

void ExternalModuleCallFunctor::terminate()
{
    finishedMutex_->lock();
    finishedFlag_ = true;
    finishedMutex_->unlock();
}

void ExternalModuleCallFunctor::handleActorSync()
{
    finishedMutex_->lock();
    finishedFlag_ = true;
    finishedMutex_->unlock();
}


String CustomTypeToStringFunctor::operator ()(const Variable & variable, Kumir::String * error)
{
    const QByteArray modAsciiName =
            QByteArray(variable.recordModuleAsciiName().c_str());
    const QString modLocalizedName =
            QString::fromStdWString(variable.recordModuleLocalizedName());
    const QByteArray classAsciiName =
            QByteArray(variable.recordClassAsciiName().c_str());

    ActorInterface * actor = Util::findActor(modAsciiName);

    String result;
    if (actor) {
        QVariant value = Util::VariableToQVariant(variable);
        const QString qString = actor->customValueToString(classAsciiName, value);
        result = qString.toStdWString();
    }
    else {
        if (error) {
            error->assign(
                QString::fromUtf8("Нет такого исполнителя: %1").arg(modLocalizedName).toStdWString()
                        );
        }
    }
    return result;
}

AnyValue CustomTypeFromStringFunctor::operator ()(
        const String & source,
        const std::string & moduleAsciiName,
        const String & moduleName,
        const std::string & typeAsciiName,
        const String & typeName, Kumir::String * error
        )
{
    AnyValue result;
    QString errorMessage;
    const QString modName =
            QString::fromStdWString(moduleName);
    const QString className =
            QString::fromStdWString(typeName);
    const QString qString = QString::fromStdWString(source);

    ActorInterface * actor = Util::findActor(moduleAsciiName);
    if (actor) {
        const QVariant value = actor->customValueFromString(QByteArray(typeAsciiName.c_str()), qString);
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
    if (errorMessage.length()>0 && error)
        error->assign(errorMessage.toStdWString());
    return result;
}


}}
