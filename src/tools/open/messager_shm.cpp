#include "messager_shm.h"

#include <Windows.h>
#include <Psapi.h>

#include <QDebug>
#include <QSharedMemory>

void MessagerShm::sendMessage(Pid pid, const QString &message)
{
    const HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                        FALSE, pid);
    if (hProcess) {
        wchar_t buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        DWORD sz = GetModuleBaseNameW(hProcess, 0, buffer, 1024u);
        CloseHandle(hProcess);
        QString procName = QString::fromWCharArray(buffer, sz);
        if (procName.length() > 0) {
            QSharedMemory shm(procName);
            if (shm.attach(QSharedMemory::ReadWrite)) {
                shm.lock();
                QByteArray utf8 = message.toUtf8();
                memset(shm.data(), 0, shm.size());
                memcpy(shm.data(), utf8.constData(), utf8.size());
                shm.unlock();
            }
        }
    }
}
