/****************************************************************************
**
** Some parts of code taken from QtCreator 2.8.1 by Digia Plc
**
** Licensed under the terms of GNU Lesser GPL Licensee 2.1
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
****************************************************************************/

#ifndef GDBMI_MIDEBUGGER_H
#define GDBMI_MIDEBUGGER_H

#include <QObject>
#include "stopevent.h"
#include "runevent.h"
#include "location.h"

namespace GdbMi {

enum DebuggerState
{
    DebuggerNotReady,          // Debugger not started

    EngineSetupRequested,      // Engine starts
    EngineSetupFailed,
    EngineSetupOk,

    InferiorSetupRequested,
    InferiorSetupFailed,
    InferiorSetupOk,

    EngineRunRequested,
    EngineRunFailed,

    InferiorUnrunnable,        // Used in the core dump adapter

    InferiorRunRequested,      // Debuggee requested to run
    InferiorRunOk,             // Debuggee running
    InferiorRunFailed,         // Debuggee running

    InferiorStopRequested,     // Debuggee running, stop requested
    InferiorStopOk,            // Debuggee stopped
    InferiorStopFailed,        // Debuggee not stopped, will kill debugger

    InferiorExitOk,

    InferiorShutdownRequested,
    InferiorShutdownFailed,
    InferiorShutdownOk,

    EngineShutdownRequested,
    EngineShutdownFailed,
    EngineShutdownOk,

    DebuggerFinished
};

class MiDebugger
        : public QObject
{
    friend class MiDebuggerPrivate;
    Q_OBJECT
public:
    bool selectRemoteTarget(const QString & hostName, quint16 portNumber);
    DebuggerState state() const;
    void synchronizeBreakpoints();
    void gotoLocation(const Location &loc);

Q_SIGNALS:
    void stopped(const StopEvent & stopEvent);
    void running(const RunEvent & runEvent);

protected:
    MiDebugger();
private:
    class MiDebuggerPrivate* pImpl;
};


} // namespace GdbMI

#endif // GDBMI_MIDEBUGGER_H
