#ifndef RUN_INTERFACE
#define RUN_INTERFACE

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

namespace Shared {

namespace Analizer {
class HelperInterface;
}

class RunInterface {
public:
    struct RunnableProgram {
        QString sourceFileName;
        QString executableFileName;
        QByteArray executableData;
        QString sourceData;
    };

    enum StopReason { SR_Done, SR_UserInteraction, SR_InputRequest, SR_Error, SR_UserTerminated };
    enum RunMode { RM_StepOver, RM_ToEnd, RM_StepOut, RM_StepIn };

    inline virtual bool hasBreakpointsSupport() const { return false; }
    inline virtual void setBreakpoint(
            const QString & /*sourceFileName*/,
            const quint16 /*lineNumber*/,
            const QString & /*condition*/,
            const quint32 /*ignoreCount*/) {}

    virtual bool loadProgram(const RunnableProgram &sourceInfo) = 0;
    virtual QDateTime loadedProgramVersion() const = 0;

    virtual RunMode currentRunMode() const = 0;
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

    virtual void setStdInTextStream(QTextStream *) = 0;
    virtual void setStdOutTextStream(QTextStream *) = 0;

    inline virtual void setSourceHelper(Analizer::HelperInterface *) {}

    inline virtual void removeAllBreakpoints() {}

    inline virtual void insertOrChangeBreakpoint(bool /*enabled*/, const QString &/*fileName*/,
                                                 quint32 /*lineNo*/, quint32 /*ignoreCount*/,
                                                 const QString & /*condition*/) {}

    inline virtual void insertSingleHitBreakpoint(const QString &/*fileName*/, quint32 /*lineNo*/) {}

    inline virtual void removeBreakpoint(const QString & /*fileName*/, quint32 /*lineNo*/) {}

};

}
Q_DECLARE_METATYPE(Shared::RunInterface::StopReason)
Q_DECLARE_INTERFACE(Shared::RunInterface, "kumir2.run")

#endif
