#ifndef COMMONRUN_H
#define COMMONRUN_H

#include <QtCore>
#define DO_NOT_DECLARE_STATIC
#include "vm/vm_abstract_handlers.h"
#include "interfaces/actorinterface.h"

namespace KumirCodeRun {
namespace Common {

using namespace Kumir;
using namespace VM;
using namespace Shared;

class CriticalSectionLocker
        : public VM::CriticalSectionLocker
{
public:
    explicit CriticalSectionLocker();
    void lock();
    void unlock();
    ~CriticalSectionLocker();
private:
    QMutex * mutex_;
};


class ExternalModuleCallFunctor:
        private QObject,
        public VM::ExternalModuleCallFunctor
{
    Q_OBJECT
public:
    explicit ExternalModuleCallFunctor(QObject * parent = 0);
    AnyValue operator()(
            const std::string & asciiModuleName,
            const String & moduleName,
            const uint16_t algKey,
            VariableReferencesList alist
            )  /* throws std::string, Kumir::String */ ;
    ~ExternalModuleCallFunctor();
    void checkForActorConnected(const std::string & asciiModuleName);

private slots:
    void handleActorSync();

private /*fields*/:
    bool finishedFlag_;
    QMutex * finishedMutex_;
    QList<Shared::ActorInterface*> connectedActors_;
};


class CustomTypeToStringFunctor
        : public VM::CustomTypeToStringFunctor
{
public:
    String operator()(const Variable & variable)
        /*throws Kumir::String, std::string*/;
};

class CustomTypeFromStringFunctor
        : public VM::CustomTypeFromStringFunctor
{
public:
    AnyValue operator()(
                const String & source,
                const std::string & moduleAsciiName,
                const String & moduleName,
                const std::string & typeAsciiName,
                const String & typeName
                ) /*throws Kumir::String, std::string*/;
};

}}

#endif // COMMONRUN_H
