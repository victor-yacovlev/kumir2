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

class ExternalModuleResetFunctor
        : public VM::ExternalModuleResetFunctor
{
public:
    void operator()(const Kumir::String & moduleName)
        /* throws std::string, Kumir::String */ ;
};

class ExternalModuleCallFunctor:
        private QObject,
        public VM::ExternalModuleCallFunctor
{
    Q_OBJECT
public:
    explicit ExternalModuleCallFunctor(QObject * parent = 0);
    AnyValue operator()(
            const String & moduleName,
            const uint16_t algKey,
            VariableReferencesList alist
            )  /* throws std::string, Kumir::String */ ;
    ~ExternalModuleCallFunctor();

private slots:
    void handleActorSync();

private /*methods*/:
    void grabActor(ActorInterface * actor);
    void releaseActor(ActorInterface * actor);

private /*fields*/:
    bool finishedFlag_;
    QMutex * finishedMutex_;
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
                const String & moduleName,
                const String & typeName
                ) /*throws Kumir::String, std::string*/;
};

}}

#endif // COMMONRUN_H
