#ifndef VM_ABSTRACT_HANDLERS_H
#define VM_ABSTRACT_HANDLERS_H

/* This file contains interface declarations for VM handlers.
 *
 * By default, all handlers are trivial and do nothing.
 *
 * Debugger handler is implemented as a Java-style handler with a set
 * of virtual methods.
 *
 * Mutex interface is just a abstract class with Lock and Unlock virtual
 *
 * More critical handlers are implemented as distinct functors.
 *
 */

#ifndef NO_UNICODE
#include <wchar.h>
#endif

#if defined(WIN32) || defined(_WIN32)
#   include <windows.h>
#else
#   include <unistd.h>
#endif

#include <deque>
#include <string>
#include <list>
#include <vector>

#include "variant.hpp"


namespace VM {


/** A generic "Mutex" interface to be implemented using
 *  platform-specific Mutex implementation */

class CriticalSectionLocker {
public:
    virtual void lock() {}
    virtual void unlock() {}
    virtual void reset() {}
    // destructor MUST me virtual even not need
    inline virtual ~CriticalSectionLocker() {}
};

/* ======================================================================= */
/* =================== Begin functors declarations ======================= */

class Functor {
public:
    enum Type {
        Invalid = 0,
        ExternalModuleReset,
        ExternalModuleLoad,
        ExternalModuleCall,
        ConvertToString,
        ConvertFromString,
        Input,
        Output,
        GetMainArgument,
        ReturnMainValue,
        Pause,
        Delay
    };
    virtual Type type() const = 0;
protected:
    // destructor MUST me virtual even not need
    inline virtual ~Functor() {}
};

/* ====== Functors for external modules initialization ====== */

/** A functor to reset external module before execution by a given name
 */
class ExternalModuleResetFunctor: public Functor {
public:
    inline ExternalModuleResetFunctor(): callFunctor_(0) {}
    inline Type type() const override { return ExternalModuleReset; }
    inline virtual void operator()(const std::string & /*moduleName*/, const Kumir::String & localizedName, Kumir::String * error)
    {
        const Kumir::String errorMessage =
                Kumir::Core::fromUtf8("Невозможно использовать \"")+
                localizedName+
                Kumir::Core::fromUtf8("\": исполнители не поддерживаются");
        if (error) {
            error->assign(errorMessage);
        }
    }    
    inline void setCallFunctor(class ExternalModuleCallFunctor * callFunctor)
    {
        callFunctor_ = callFunctor;
    }

protected:
    class ExternalModuleCallFunctor * callFunctor_;
};


/** A functor to load external module plugin by a given name and
 *  canonical module file name, e.g. a file name without prefix 'lib' and
 *  any dot-after suffix in name.
 *
 *  Example:
 *    ExternalModuleLoadFunctor("A great module", "greatModule", &errorMessage)
 *
 *    --- loads 'A great module' from file 'libGreatModule.so' (on Linux), or
 *        from file 'GreatModule.dll' (on Windows).
 *    --- returns a list of module provided names
 */
class ExternalModuleLoadFunctor: public Functor {
public:
    inline Type type() const override { return ExternalModuleLoad; }
    typedef std::deque<std::string> NamesList;
    inline virtual NamesList operator()(
            const std::string & /*moduleAsciiName*/,
            const Kumir::String & moduleName,
            Kumir::String * error
            )
    {
        const Kumir::String errorMessage =
                Kumir::Core::fromUtf8("Невозможно использовать \"")+
                moduleName+
                Kumir::Core::fromUtf8("\": исполнители не поддерживаются");
        if (error) {
            error->assign(errorMessage);
        }
        return NamesList();
    }
};


/** A functor to call external module module by a given name and ID.
 *
 *  An arguments list is passed to functor; return value is a
 *  calling function return (is any), or a dummy any value (if void).
 *
 */
class ExternalModuleCallFunctor: public Functor {
public:
    inline Type type() const override { return ExternalModuleCall; }
    typedef const std::deque<Variable> & VariableReferencesList;
    inline virtual AnyValue operator()(
            const std::string & /*asciiModuleName*/,
            const Kumir::String & localizedModuleName,
            const uint16_t /*alogrithmId*/,
            VariableReferencesList /*arguments*/,
            Kumir::String * error
            )
    {
        const Kumir::String errorMessage =
                Kumir::Core::fromUtf8("Невозможно вызвать алгоритм исполнителя \"")+
                localizedModuleName+
                Kumir::Core::fromUtf8("\": исполнители не поддерживаются");
        if (error) {
            error->assign(errorMessage);
        }
        return AnyValue();
    }
    inline virtual void checkForActorConnected(const std::string & /*asciiModuleName*/) {}
};

class CustomTypeToStringFunctor: public Functor {
public:
    inline Type type() const override { return ConvertToString; }
    inline virtual Kumir::String operator()(
            const Variable & variable,
            Kumir::String * error
            )
    {
        const Kumir::String errorMessage =
            Kumir::Core::fromUtf8("Не могу вывести значение типа \"") +
                variable.recordClassLocalizedName()+Kumir::Core::fromAscii("\"");
        if (error) {
            error->assign(errorMessage);
        }
        return Kumir::String();
    }
};

class CustomTypeFromStringFunctor: public Functor {
public:
    inline Type type() const override { return ConvertFromString; }
    inline virtual VM::AnyValue operator()(
            const Kumir::String & /*source*/,
            const std::string & /*moduleAsciiName*/,
            const Kumir::String & /*moduleName*/,
            const std::string & /* typeAsciiName */,
            const Kumir::String & typeLocalizedName,
            Kumir::String * error
            )
    {
        const Kumir::String errorMessage =
            Kumir::Core::fromUtf8("Не могу разобрать значение типа \"") +
                typeLocalizedName+Kumir::Core::fromAscii("\"");
        if (error) {
            error->assign(errorMessage);
        }
        return VM::AnyValue();
    }
};

/* ====== Functors for execution control function run ======= */

/** A functor to make a pause while debugging
 */
class PauseFunctor : public Functor {
public:
    inline Type type() const override { return Pause; }
    inline virtual void operator()() {}
};


/** A functor to sleep execution.
 *  Default implementation is not multi-thread
 *
 *  Argument is a sleep time in milliseconds
 */
class DelayFunctor : public Functor {
public:
    inline Type type() const override { return Delay; }
    inline virtual void operator()(uint32_t msec) {
#if defined(WIN32) || defined(_WIN32)
        Sleep(msec);
#else
        uint32_t sec = msec / 1000;
        uint32_t usec = (msec - sec * 1000) * 1000;
        // usleep works in range [0, 1000000), so
        // call sleep(sec) first for long periods
        sleep(sec);
        usleep(usec);
#endif
    }
};

/* ====== Functors for user input and output================= */

/** A functor to input variables values within user interface
 *
 *  Argument is a list of Reference-variables to be set.
 *
 */
class InputFunctor: public Functor {
public:
    inline Type type() const override { return Input; }
    typedef std::deque<Variable> & VariableReferencesList;
    inline virtual bool operator()(VariableReferencesList /*alist*/, Kumir::String * error)
    {
        static const Kumir::String errorMessage =
            Kumir::Core::fromUtf8("Операция ввода не поддерживается");
        if (error) {
            error->assign(errorMessage);
        }
        return false;
    }
};

/** A functor to output variables values within user interface
 *
 *  Argument is a list of Reference-variables to be shown and
 *  a list of the same size of int-pairs (format parameters).
 *
 */
class OutputFunctor: public Functor {
public:
    inline Type type() const override { return Output; }
    typedef const std::deque<Variable> & VariableReferencesList;
    typedef const std::deque< std::pair<int,int> > & FormatsList;
    inline virtual void operator()(
            VariableReferencesList /*vars*/,
            FormatsList /*formats*/,
            Kumir::String * error
            )
    {
        static const Kumir::String errorMessage =
            Kumir::Core::fromUtf8("Операция вывода не поддерживается");
        if (error) {
            error->assign(errorMessage);
        }
    }
};

class GetMainArgumentFunctor: public Functor {
public:
    inline Type type() const override { return GetMainArgument; }
    inline virtual void operator()(Variable & /*reference*/, Kumir::String * error)
    {
        static const Kumir::String errorMessage =
            Kumir::Core::fromUtf8("Запуск первого алгоритма с аргументами не поддерживается");
        if (error) {
            error->assign(errorMessage);
        }
    }
};

class ReturnMainValueFunctor: public Functor {
public:
    inline Type type() const override { return ReturnMainValue; }
    inline virtual void operator()(const Variable & /*reference*/, Kumir::String * error)
    {
        static const Kumir::String errorMessage =
            Kumir::Core::fromUtf8("Возвращение значений первого алгоритма не поддерживается");
        if (error) {
            error->assign(errorMessage);
        }
    }
};

/* --------------------------------------------------- */
/* ====== End all functors declarations ============== */

/** An interface to communicating GUI debugger */

class DebuggingInteractionHandler {
public:
    inline virtual bool appendTextToMargin(
            int /*lineNo*/,
            const Kumir::String & /*noticeText*/
            ) { return false; }

    inline virtual bool setTextToMargin(
            int /*lineNo*/,
            const Kumir::String & /*text*/,
            bool /*redColorForeground*/
            ) { return false; }

    inline virtual bool clearMargin(
            int /*fromLine*/, int /*toLine*/
            ) { return false; }

    inline virtual bool noticeOnFunctionReturn()
    { return false; }

    inline virtual bool noticeOnLineChanged(
            int /*lineNo*/,
            uint32_t /*columnStartNo*/,
            uint32_t /*columnEndNo*/
            ) { return false; }

    inline virtual bool noticeOnStepsChanged(
            uint64_t /*stepsDone*/
            ) { return false; }

    inline virtual void debuggerReset() {}
    inline virtual void debuggerNoticeBeforePopContext() {}
    inline virtual void debuggerNoticeBeforePushContext() {}
    inline virtual void debuggerNoticeAfterPopContext() {}
    inline virtual void debuggerNoticeAfterPushContext() {}
    inline virtual void debuggerNoticeBeforeArrayInitialize(const VM::Variable & /*variable*/,
                                                            const int /*bounds*/[7]) {}
    inline virtual void debuggerNoticeAfterArrayInitialize(const VM::Variable & /*variable*/) {}
    inline virtual void debuggerNoticeOnValueChanged(const VM::Variable & /*variable*/,
                                                     const int[4] /*indeces*/) {}

    inline virtual void debuggerNoticeOnBreakpointHit(const String & /*filename*/, const uint32_t /*lineNo*/) {}
};

}

#endif // VM_ABSTRACT_HANDLERS_H
