#include "procmanager_winapi.h"

#include <Psapi.h>

#include <QString>
#include <QDebug>
#include <QFileInfo>

Pid ProcManagerWindows::find(const QString &executable)
{
    Pid result = 0;

    static DWORD processIds[2048];
    static const DWORD cb = sizeof(processIds);
    DWORD bytesReturned;

    const QString key = QFileInfo(executable).fileName();

    EnumProcesses(processIds, cb, &bytesReturned);

    const size_t numberOfProcesses = bytesReturned / sizeof(DWORD);

    for (size_t i=0u; i<numberOfProcesses; i++) {
        const DWORD pid = processIds[i];
        const HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                            FALSE, pid);
        if (hProcess) {
            wchar_t buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            const DWORD bufSize = GetModuleBaseNameW(hProcess, 0, buffer, 1024u);
            CloseHandle(hProcess);
            const QString procName = QString::fromWCharArray(buffer, bufSize);
            if (procName == key) {
                result = pid;
                break;
            }
        }
    }

    return result;
}
