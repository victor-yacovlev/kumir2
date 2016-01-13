#include "procmanager.h"

#ifdef Q_OS_LINUX
#include "procmanager_linux.h"
#endif
#ifdef Q_OS_WIN32
#include "procmanager_winapi.h"
#endif

ProcessManager::ProcessManager() {}

ProcessManager & ProcessManager::get()
{
    static ProcessManager manager;
    if (manager.pImpl_ == 0) {
#ifdef Q_OS_LINUX
        manager.pImpl_.reset(new ProcManagerLinux);
#endif
#ifdef Q_OS_WIN32
        manager.pImpl_.reset(new ProcManagerWindows);
#endif
    }
    return manager;
}

Pid ProcessManager::find(const QString &executable)
{
    if (pImpl_) {
        return pImpl_->find(executable);
    }
    else {
        return 0;
    }
}
