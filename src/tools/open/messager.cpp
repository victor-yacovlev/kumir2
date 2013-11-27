#include "messager.h"

#ifndef Q_OS_WIN32
#include "messager_unix.h"
#else
#include "messager_shm.h"
#endif

Messager::Messager() {}

Messager & Messager::get()
{
    static Messager messager;
    if (!messager.pImpl_) {
#ifndef Q_OS_WIN32
    messager.pImpl_.reset(new MessagerUnix);
#else
    messager.pImpl_.reset(new MessagerShm);
#endif
    }
    return messager;
}

void Messager::sendMessage(Pid receiver, const QString &message)
{
    if (pImpl_) {
        pImpl_->sendMessage(receiver, message);
    }
}
