#ifndef RUN_INTERFACE
#define RUN_INTERFACE

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

namespace Shared {

class RunInterface {
public:
    struct RunnableProgram {
        QString sourceFileName;
        QString executableFileName;
        QByteArray executableData;
        QString sourceData;
    };

    enum StopReason { SR_Done, SR_UserInteraction, SR_InputRequest, SR_Error, SR_UserTerminated };
    virtual bool loadProgram(const RunnableProgram &sourceInfo) = 0;
    virtual QDateTime loadedProgramVersion() const = 0;

    virtual bool canStepOut() const = 0;
    virtual void runBlind() = 0;
    virtual void runContinuous() = 0;
    virtual void runStepOver() = 0;
    virtual void runStepInto() = 0;
    virtual void runToEnd() = 0;
    virtual void runTesting() = 0;
    virtual bool isTestingRun() const = 0;
    virtual void terminate() = 0;
    virtual bool hasMoreInstructions() const = 0;
    virtual bool hasTestingEntryPoint() const = 0;
    virtual void terminateAndWaitForStopped() = 0;
    virtual int currentLineNo() const = 0;
    virtual QPair<quint32,quint32> currentColumn() const = 0;
    virtual QString error() const = 0;
    virtual QVariant valueStackTopItem() const = 0;
    virtual unsigned long int stepsCounted() const = 0;
    virtual QAbstractItemModel * debuggerVariablesViewModel() const = 0;
};

}
Q_DECLARE_METATYPE(Shared::RunInterface::StopReason)
Q_DECLARE_INTERFACE(Shared::RunInterface, "kumir2.run")

#endif
