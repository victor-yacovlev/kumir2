#ifndef KUMIRCODERUN_RUN_H
#define KUMIRCODERUN_RUN_H

#include <QtCore>
#define DO_NOT_DECLARE_STATIC
#include "vm/vm.hpp"


namespace KumirCodeRun {

using Kumir::String;
using Kumir::real;
using VM::Variable;
using VM::AnyValue;

class Run
        : public QThread
        , public VM::AbstractInteractionHandler
{
    Q_OBJECT
public:
    enum RunMode { RM_StepOver, RM_ToEnd, RM_StepOut, RM_StepIn };
    explicit Run(QObject *parent);
    VM::KumirVM * vm;
    bool programLoaded;
    inline bool stopped() const { return b_stopping; }

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


    bool makeInput(
                std::deque<Variable> & /*references*/
                );
    bool makeInputArgument(Variable & /*reference*/);
    bool makeOutputArgument(const Variable & /*reference*/);
    bool inputScalarArgument(const QString & message, const QString & format, AnyValue & value);
    void handleInputArgumentRequest(int localId,
                                    const QString & varName,
                                    const QString & baseFormat,
                                    const QList<int> & bounds);
    void handleOutputArgumentRequest(const QVariant & value,
                                    const QString & varName,
                                    const QList<int> & bounds);
    bool makeOutput(
                const std::deque< std::pair<int,int> > & /*formats*/,
                const std::deque<Variable> & /*values*/
                );

    bool noticeOnLineNoChanged(
            int /*lineNo*/
            );

    bool noticeOnValueChange(int lineNo, const String & s);
    bool clearMargin(int from, int to);

    bool loadExternalModule(const String & moduleName,
                            const String & fileName,
                            /*out*/ std::list<String> & availableMethods);

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
    void finishInput(const QVariantList & data);
    void finishExternalFunctionCall(
        const QString & error,
        const QVariant & retval,
                                    const QVariantList & results);
    void handleExternalRequest(const QString & pluginName,
                               const QString & functionName,
                               const QVariantList & arguments,
                               const QList<quintptr> & references,
                               const QList<int> & indeces);
    void handleExternalFunctionCall(const QString & moduleName
                               , quint16 algId
                               , const QVariantList & arguments);
    bool evaluateExternalFunction(
            const String & /*moduleName*/, // IN
            uint16_t /*functionId*/, // IN
            const std::deque<Variable> & /*arguments*/, // IN
            std::deque<Variable> &,
            Variable & /*result*/,  // OUT
            String & /*moduleRuntimeError*/  // OUT
            );
    void handleExternalRequest(const QList<quintptr> & references);
    void handlePauseRequest();
    void prepareExternalCall();

signals:
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

    bool mustStop();

    RunMode e_runMode;

    bool b_stopping;
    QMutex * mutex_stopping;

    bool b_stepDone;
    QMutex * mutex_stepDone;

    bool b_algDone;
    QMutex * mutex_algDone;

    int i_originFunctionDeep;

    QMutex * mutex_interactDone;
    bool b_interactDone;

    QVariantList list_inputResult;
    QVariantList list_funcResults;
    QVariant v_funcResult;
    QString s_funcError;
    class Mutex * mutex_vm;

};

} // namespace KumirCodeRun

#endif // KUMIRCODERUN_RUN_H
