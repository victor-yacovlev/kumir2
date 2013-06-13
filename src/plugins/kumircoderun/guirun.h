#ifndef GUIRUN_H
#define GUIRUN_H

#include <QtCore>
#define DO_NOT_DECLARE_STATIC
#include "vm/vm_abstract_handlers.h"
#include "interfaces/actorinterface.h"

namespace KumirCodeRun {

class Run;

namespace Gui {

using namespace Kumir;
using namespace VM;

class ExternalModuleLoadFunctor
        : public VM::ExternalModuleLoadFunctor
{
public:
    NamesList operator()(
                const String & moduleName,
                const std::string & canonicalModuleFileName)
                /* throws std::string, Kumir::String */ ;
};

class InputFunctor
        : private QObject
        , public VM::InputFunctor
{
    Q_OBJECT
public:
    InputFunctor();
    void setCustomTypeFromStringFunctor(VM::CustomTypeFromStringFunctor *f);
    void setRunnerInstance(class Run * runner);
    bool operator()(VariableReferencesList alist);
    ~InputFunctor();
signals:
    void requestInput(const QString & format);
private slots:
    void handleInputDone(const QVariantList & values);
private /*fields*/:
    bool finishedFlag_;
    QMutex * finishedMutex_;
    QVariantList inputValues_;
    VM::CustomTypeFromStringFunctor * converter_;
    Run * runner_;
};

class OutputFunctor
        : private QObject
        , public VM::OutputFunctor
{
    Q_OBJECT
public:
    OutputFunctor();
    void setCustomTypeToStringFunctor(VM::CustomTypeToStringFunctor *f);
    void setRunnerInstance(class Run * runner);
    void operator()(VariableReferencesList alist, FormatsList formats);
signals:
    void requestOutput(const QString & data);
private:
    VM::CustomTypeToStringFunctor * converter_;
};

class GetMainArgumentFunctor
        : private QObject
        , public VM::GetMainArgumentFunctor
{
    Q_OBJECT
public:
    GetMainArgumentFunctor();
    void operator()(Variable & reference);
    void setCustomTypeFromStringFunctor(VM::CustomTypeFromStringFunctor *f);
    void setRunnerInstance(class Run * runner);
    ~GetMainArgumentFunctor();
signals:
    void requestInput(const QString & format);
    void requestOutput(const QString & data);
private:
    bool inputScalarArgument(const QString & message,
                             const QString & format,
                             AnyValue & value);
private slots:
    void handleInputDone(const QVariantList & values);
private /*fields*/:
    bool finishedFlag_;
    QMutex * finishedMutex_;
    QVariantList inputValues_;
    VM::CustomTypeFromStringFunctor * converter_;
    Run * runner_;
};

class ReturnMainValueFunctor
        : private QObject
        , public VM::ReturnMainValueFunctor
{
    Q_OBJECT
public:
    ReturnMainValueFunctor();
    void setCustomTypeToStringFunctor(VM::CustomTypeToStringFunctor *f);
    void setRunnerInstance(class Run * runner);
    void operator()(const Variable & reference);
signals:
    void requestOutput(const QString & data);
private:
    VM::CustomTypeToStringFunctor * converter_;
};


class PauseFunctor
        : public QObject
        , public VM::PauseFunctor
{
    Q_OBJECT
public:
    PauseFunctor();
    void operator()();
signals:
    void requestPause();
};

}

}

#endif // GUIRUN_H
