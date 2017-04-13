#ifndef CONSOLERUN_H
#define CONSOLERUN_H

#include <QtCore>
#define DO_NOT_DECLARE_STATIC
#include <kumir2-libs/vm/vm_abstract_handlers.h>

#ifndef _override
#if defined(_MSC_VER)
#   define _override
#else
#   define _override override
#endif
#endif

namespace KumirCodeRun {
namespace Console {

class ExternalModuleLoadFunctor
        : public VM::ExternalModuleLoadFunctor
{
public:
    NamesList operator()(
                const std::string & moduleAsciiName,
                const Kumir::String & moduleName, Kumir::String * error) _override;
};

class ExternalModuleResetFunctor
        : public VM::ExternalModuleResetFunctor
{
public:
    void operator()(const std::string & moduleName, const Kumir::String & localizedName, Kumir::String * error) _override;
};

}} // namespace Console


#endif // CONSOLERUN_H
