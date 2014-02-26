#ifndef DEBUGGERERROR_H
#define DEBUGGERERROR_H

namespace GdbMi {

enum DebuggerError {
    Err_NoError = 0,
    Err_CantStartGdbExecutable,
    Err_CantConnectToRemote
};

}

#endif // DEBUGGERERROR_H
