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

#ifndef GDBMI_MIDEBUGGERPRIVATE_H
#define GDBMI_MIDEBUGGERPRIVATE_H

#include <QObject>
#include <QProcess>
#include <QTextCodec>

#include "debuggererror.h"
#include "gdbmivalue.h"
#include "breakhandler.h"
#include "breakpoint.h"
#include "midebugger.h"

namespace GdbMi {


class MiDebuggerPrivate
        : public QObject
{
    Q_OBJECT
public /*methods*/:
    void initialize();
    DebuggerError start();
    void handleResponse(const QByteArray & buff);
    void handleRunningResponse(const GdbMiValue &data);
    void handleStopResponse(const GdbMiValue & data);
    DebuggerState state() const { return m_state; }
    inline void notifyInferiorPid(qint64 pid) { m_inferiorPid = pid; }
    inline BreakHandler* breakHandler() const { return breakHandler_; }
    void updateResponse(BreakpointResponse &response, const GdbMiValue &bkpt);
    QString cleanupFullName(const QString &fileName);


public Q_SLOTS:
    void handleStdOutReady();

public /*fields*/:
    MiDebugger* pClass_;
    QTextCodec* codec_;
    QProcess* process_;
    BreakHandler* breakHandler_;

    QByteArray m_pendingConsoleStreamOutput;
    QByteArray m_pendingLogStreamOutput;
    DebuggerState m_state;
    qint64 m_inferiorPid;
    bool m_isMacGdb;
    bool m_hasBreakpointNotifications;
};

} // namespace GdbMi

#endif // GDBMI_MIDEBUGGERPRIVATE_H
