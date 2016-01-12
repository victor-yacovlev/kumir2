#ifndef GUIRUN_H
#define GUIRUN_H

#include <QtCore>
#define DO_NOT_DECLARE_STATIC
#include "vm/vm_abstract_handlers.h"
#include "interfaces/actorinterface.h"
#include "commonrun.h"

#ifndef _override
#if defined(_MSC_VER)
#   define _override
#else
#   define _override override
#endif
#endif

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
            const std::string & moduleAsciiName,
            const Kumir::String & moduleName, Kumir::String * error)
                _override;
};

class InputFunctor
        : private QObject
        , public VM::InputFunctor
        , public Kumir::AbstractInputBuffer
{
    Q_OBJECT
public:
    InputFunctor();
    void setCustomTypeFromStringFunctor(VM::CustomTypeFromStringFunctor *f);
    void setRunnerInstance(class Run * runner);
    bool operator()(VariableReferencesList alist, Kumir::String * error) _override;
    ~InputFunctor();

    // Raw data handling methods
    void clear() _override;
    bool readRawChar(Kumir::Char &ch) _override;
    void pushLastCharBack() _override;

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

    QString rawBuffer_;
    QChar rawBufferLastReadChar_;
};

class SimulatedInputBuffer
        : public Kumir::AbstractInputBuffer
{
public:
    inline explicit SimulatedInputBuffer(QTextStream * stream): io_(stream), prevChar_(QChar::Null), lastChar_(QChar::Null) {}

    void clear() _override;
    bool readRawChar(Kumir::Char &ch) _override;
    void pushLastCharBack() _override;

private:
    QTextStream *io_;
    QChar prevChar_;
    QChar lastChar_;

};

class OutputFunctor
        : private QObject
        , public VM::OutputFunctor
        , public Kumir::AbstractOutputBuffer
{
    Q_OBJECT
public:
    OutputFunctor();
    void setCustomTypeToStringFunctor(VM::CustomTypeToStringFunctor *f);
    void setRunnerInstance(class Run * runner);
    void operator()(VariableReferencesList alist, FormatsList formats, Kumir::String * error) _override;
    void writeRawString(const String & s) _override;
signals:
    void requestOutput(const QString & data);
private:
    VM::CustomTypeToStringFunctor * converter_;
};

class SimulatedOutputBuffer
        : public Kumir::AbstractOutputBuffer
{
public:
    inline explicit SimulatedOutputBuffer(QTextStream * stream): io_(stream) {}
    void writeRawString(const Kumir::String &) _override;
private:
    QTextStream *io_;
};

class GetMainArgumentFunctor
        : private QObject
        , public VM::GetMainArgumentFunctor
{
    Q_OBJECT
public:
    GetMainArgumentFunctor();
    void operator()(Variable & reference, Kumir::String * error) _override;
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
    void operator()(const Variable & reference, Kumir::String * error) _override;
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
    void operator()() _override;
signals:
    void requestPause();
};

class DelayFunctor
        : public QThread
        , public VM::DelayFunctor
{
    Q_OBJECT
public:
    DelayFunctor();
    void operator()(quint32 msec) _override;
    void stop();
private:
    bool stopFlag_;
    QScopedPointer<QMutex> stopMutex_;
};

class ExternalModuleResetFunctor
        : public QObject
        , public VM::ExternalModuleResetFunctor
{
    Q_OBJECT
public:
    ExternalModuleResetFunctor();
    virtual void operator()(const std::string & moduleName, const String & localizedName, Kumir::String * error) _override;
signals:
    void showActorWindow(const QByteArray & actorAsciiName);
};

}

}

#endif // GUIRUN_H
