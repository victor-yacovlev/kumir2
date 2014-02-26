#ifndef STOPREASON_H
#define STOPREASON_H

#include <QtGlobal>
#include <QString>
#include <QVariant>

namespace GdbMi {

struct StopEvent {
    enum StopReason {
        Unknown = 0,
        BreakpointHit,
        WatchpointTrigger,
        FunctionFinished,
        EndSteppingRange,
        Exited,
        ExitedNormally,
        SignalReceived
    }
    reason;
    quint16 threadId;
    quint16 breakpointNo;
    struct Frame * frame;
    QString signalName;
    QVariant watchpointOldValue;
    QVariant watchpointNewValue;
    int exitCode;
};

}

#endif // STOPREASON_H
