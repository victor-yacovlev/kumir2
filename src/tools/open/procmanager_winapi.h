#ifndef PROCMANAGER_WINAPI_H
#define PROCMANAGER_WINAPI_H

#include "procmanager.h"

class ProcManagerWindows
        : public ProcessManager::ImplInterface
{
public:
    Pid find(const QString &executable);
};

#endif
