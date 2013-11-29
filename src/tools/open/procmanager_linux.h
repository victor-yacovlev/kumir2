#ifndef PROCMANAGER_LINUX_H
#define PROCMANAGER_LINUX_H

#include "procmanager.h"

class ProcManagerLinux
        : public ProcessManager::ImplInterface
{
public:
    pid_t find(const QString &executable);
};

#endif
