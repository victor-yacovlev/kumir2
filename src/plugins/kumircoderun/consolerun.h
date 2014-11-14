#ifndef CONSOLERUN_H
#define CONSOLERUN_H

#include <QtCore>
#define DO_NOT_DECLARE_STATIC
#include "vm/vm_abstract_handlers.h"


namespace KumirCodeRun {
namespace Console {

class ExternalModuleLoadFunctor
        : public VM::ExternalModuleLoadFunctor
{
public:
    NamesList operator()(
                const std::string & moduleAsciiName,
                const Kumir::String & moduleName, Kumir::String * error) override;
};

class ExternalModuleResetFunctor
        : public VM::ExternalModuleResetFunctor
{
public:
    void operator()(const std::string & moduleName, const Kumir::String & localizedName, Kumir::String * error) override;
};

}} // namespace Console


#endif // CONSOLERUN_H
