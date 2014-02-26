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

#include "midebugger.h"
#include "midebuggerprivate.h"

namespace GdbMi {

DebuggerState MiDebugger::state() const
{
    return pImpl->state();
}

void MiDebugger::synchronizeBreakpoints()
{

}

void MiDebugger::gotoLocation(const Location &loc)
{

}

} // namespace GdbMI
