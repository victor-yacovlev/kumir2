#include "procmanager.h"

#ifdef Q_OS_LINUX
#include "procmanager_linux.h"
#endif

ProcessManager::ProcessManager() {}

ProcessManager & ProcessManager::get()
{
    static ProcessManager manager;
    if (manager.pImpl_ == 0) {
#ifdef Q_OS_LINUX
        manager.pImpl_.reset(new ProcManagerLinux);
#endif
    }
    return manager;
}

pid_t ProcessManager::find(const QString &executable)
{
    if (pImpl_) {
        return pImpl_->find(executable);
    }
    else {
        return 0;
    }
}
