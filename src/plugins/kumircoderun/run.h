#ifndef KUMIRCODERUN_RUN_H
#define KUMIRCODERUN_RUN_H

#include <QtCore>
#define DO_NOT_DECLARE_STATIC
#include "vm/vm.hpp"
#include "interfaces/actorinterface.h"

namespace KumirCodeRun {

using Kumir::String;
using Kumir::real;
using VM::Variable;
using VM::AnyValue;

class Run
        : public QThread
        , public VM::DebuggingInteractionHandler
{
    Q_OBJECT
public:
    enum RunMode { RM_StepOver, RM_ToEnd, RM_StepOut, RM_StepIn };
    explicit Run(QObject *parent);
    VM::KumirVM * vm;
    bool programLoaded;
    inline bool stopped() const { return stoppingFlag_; }
    bool mustStop() const;

    // VM Access methods
    int effectiveLineNo() const;
    void loadProgramFromBinaryBuffer(std::list<char> & stream, const String & filename);
    void loadProgramFromTextBuffer(const std::string & stream, const String & filename);
    inline void setProgramDirectory(const QString & dirName) { vm->setProgramDirectory(dirName.toStdWString()); }
    QString error() const;

    void setEntryPointToMain();
    void setEntryPointToTest();
    bool hasMoreInstructions() const;
    void reset();
    void evaluateNextInstruction();
    bool canStepOut() const;

public slots:
    void lockVMMutex();
    void unlockVMMutex();
    void stop();
    void runStepOver();
    void runStepIn();
    void runStepOut();
    void runBlind();
    void runContinuous();

    bool noticeOnLineNoChanged(
            int /*lineNo*/
            );

    bool noticeOnValueChange(int lineNo, const String & s);
    bool clearMargin(int from, int to);

    bool debuggerReset();
    bool debuggerPopContext();
    bool debuggerPushContext(const Kumir::String &,
                             const std::deque<Kumir::String> &,
                             const std::deque<Kumir::String> &,
                             const std::deque<uint8_t> &);
    bool debuggerSetGlobals(const Kumir::String &,
                            const std::deque<Kumir::String> &,
                            const std::deque<Kumir::String> &,
                            const std::deque<uint8_t> &);
    bool debuggerUpdateLocalVariable(const Kumir::String &,
                                const Kumir::String &);
    bool debuggerUpdateGlobalVariable(const Kumir::String &,
                                      const Kumir::String &,
                                const Kumir::String &);
    bool debuggerUpdateLocalTableBounds(const Kumir::String &, const int[7]);
    bool debuggerUpdateGlobalTableBounds(const Kumir::String &,
                                        const Kumir::String &, const int[7]);
    bool debuggerSetLocalReference(
            const Kumir::String &,
            const Kumir::String &,
            const int[4],
            const int,
            const Kumir::String &
            );
    bool debuggerForceUpdateValues();
    bool debuggerUpdateLocalTableValue(const Kumir::String & name,
                                       const int indeces[4]);
    bool debuggerUpdateGlobalTableValue(const Kumir::String &,
                                       const Kumir::String & name,
                                       const int indeces[4]);

    void handleAlgorhitmDone(int lineNo);


signals:
    void finishInput(const QVariantList &data);
    void lineChanged(int lineNo);
    void output(const QString &value);
    void error(const QString & message);
    void input(const QString & format);
    void marginText(int lineNo, const QString & text);
    void externalFunctionCall(const QString & pluginName,
                              quint16 algId,
                              const QVariantList & arguments);
    void resetModule(const QString & pluginName);
    void aboutToStop();
    void clearMarginRequest(int,int);


    // Signals for debugger window
    void signal_debuggerReset();
    void signal_debuggerSetGlobals(
            /** module name */           const QString & moduleName,
            /** variable names */         const QStringList & names,
            /** variable base types */const QStringList & baseTypes,
            /** variable dimensions */ const QList<int> & dimensions
            );
    void signal_debuggerPushContext(
            /** context header */       const QString & contextName,
            /** variable names */         const QStringList & names,
            /** variable base types */const QStringList & baseTypes,
            /** variable dimensions */ const QList<int> & dimensions
            );
    void signal_debuggerPopContext();
    void signal_debuggerUpdateLocalVariable(
            /** variable name */       const QString & name,
            /** value */              const QString & value
            );
    void signal_debuggerUpdateGlobalVariable(
            /** module name */   const QString & moduleName,
            /** variable name */       const QString & name,
            /** value */              const QString & value
            );
    void signal_debuggerUpdateLocalTableBounds(
            /** variable name */       const QString & name,
            /** bounds */          const QList<int> & bounds
            );
    void signal_debuggerUpdateGlobalTableBounds(
            /** module name */   const QString & moduleName,
            /** variable name */       const QString & name,
            /** bounds */          const QList<int> & bounds
            );
    void signal_debuggerSetLocalReference(
            /** variable name */             const QString & name,
            /** target name */         const QString & targetName,
            /** target array indeces */const QList<int> & indeces,
            /** stack frames back */                     int back,
            /** module name for a global variable */const QString & moduleName
            );
    void signal_debuggerForceUpdateValues();
    void signal_debuggerUpdateLocalTableValue(
            /** variable name */            const QString & name,
            /** indeces */             const QList<int> & indeces
            );
    void signal_debuggerUpdateGlobalTableValue(
            /** module name */        const QString & moduleName,
            /** variable name */            const QString & name,
            /** indeces */             const QList<int> & indeces
            );

protected :
    void run();

    RunMode runMode_;

    bool stoppingFlag_;
    QMutex * stoppingMutex_;

    bool stepDoneFlag_;
    QMutex * stepDoneMutex_;

    bool algDoneFlag_;
    QMutex * algDoneMutex_;

    int originFunctionDeep_;

    QMutex * interactDoneMutex_;
    bool interactDoneFlag_;

    QVariantList inputResult_;
    QVariantList funcOptResults_;
    QVariant funcResult_;
    QString funcError_;
    class Mutex * VMMutex_;

    class ExternalModuleResetFunctor * externalModuleResetFunctor_;
    class ExternalModuleLoadFunctor  * externalModuleLoadFunctor_ ;
    class ExternalModuleCallFunctor  * externalModuleCallFunctor_ ;
    class CustomTypeToStringFunctor  * customTypeToStringFunctor_;
    class CustomTypeFromStringFunctor* customTypeFromStringFunctor_;
    class InputFunctor               * inputFunctor_;
    class OutputFunctor              * outputFunctor_;
    class GetMainArgumentFunctor     * getMainArgumentFunctor_;
    class ReturnMainValueFunctor     * returnMainValueFunctor_;

};

class SleepFunctions: private QThread
{
public:
    inline static void msleep(quint32 msec) {
        QThread::msleep(msec);
    }
    inline static void usleep(quint32 usec) {
        QThread::usleep(usec);
    }
private:
    inline void run() {}
};

class ExternalModuleLoadFunctor: public VM::ExternalModuleLoadFunctor
{
};

class ExternalModuleResetFunctor: public VM::ExternalModuleResetFunctor
{
public:
    void operator()(const String & moduleName);
};


class ExternalModuleCallFunctor:
        private QObject,
        public VM::ExternalModuleCallFunctor
{
    Q_OBJECT
public:
    explicit ExternalModuleCallFunctor(QObject * parent);
    AnyValue operator()(
            const String & moduleName,
            const uint16_t algKey,
            VariableReferencesList alist
            );
    ~ExternalModuleCallFunctor();

private slots:
    void handleActorSync();

private /*methods*/:
    Shared::ActorInterface * findActor(const QString & moduleName)
        /* throws Kumir::String */;
    void releaseActor(Shared::ActorInterface * actor);

private /*fields*/:
    bool finishedFlag_;
    QMutex * finishedMutex_;
};

class InputFunctor
        : private QObject
        , public VM::InputFunctor
{
    Q_OBJECT
public:
    explicit InputFunctor(class Run * parent,
                          VM::CustomTypeFromStringFunctor * converter);
    void operator()(VariableReferencesList alist);
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
    class Run * runner_;
};

class OutputFunctor
        : private QObject
        , public VM::OutputFunctor
{
    Q_OBJECT
public:
    explicit OutputFunctor(QObject * parent,
                           VM::CustomTypeToStringFunctor * converter);
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
    explicit GetMainArgumentFunctor(class Run * parent,
                          VM::CustomTypeFromStringFunctor * converter);
    void operator()(Variable & reference);
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
    class Run * runner_;
};

class ReturnMainValueFunctor
        : private QObject
        , public VM::ReturnMainValueFunctor
{
    Q_OBJECT
public:
    explicit ReturnMainValueFunctor(QObject * parent,
                           VM::CustomTypeToStringFunctor * converter);
    void operator()(const Variable & reference);
signals:
    void requestOutput(const QString & data);
private:
    VM::CustomTypeToStringFunctor * converter_;
};

class CustomTypeToStringFunctor
        : public VM::CustomTypeToStringFunctor
{
public:
    String operator()(const Variable & variable);
};

class CustomTypeFromStringFunctor
        : public VM::CustomTypeFromStringFunctor
{
public:
    AnyValue operator()(
                const String & source,
                const String & moduleName,
                const String & typeName
                );
};

} // namespace KumirCodeRun

#endif // KUMIRCODERUN_RUN_H
