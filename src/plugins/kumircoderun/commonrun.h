#ifndef COMMONRUN_H
#define COMMONRUN_H

#include <QtCore>
#define DO_NOT_DECLARE_STATIC
#include "vm/vm_abstract_handlers.h"
#include "interfaces/actorinterface.h"

#ifndef _override
#if defined(_MSC_VER)
#   define _override
#else
#   define _override override
#endif
#endif

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
            VariableReferencesList alist, Kumir::String * error
            )  _override;
    ~ExternalModuleCallFunctor();
    void checkForActorConnected(const std::string & asciiModuleName);
    void terminate();

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
    String operator()(const Variable & variable, Kumir::String * error) _override;
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
                const String & typeName, Kumir::String * error
                ) _override;
};

}}

#endif // COMMONRUN_H
