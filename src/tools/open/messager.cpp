#include "messager.h"

#ifndef Q_OS_WIN32
#include "messager_unix.h"
#else
#endif

Messager::Messager() {}

Messager & Messager::get()
{
    static Messager messager;
    if (!messager.pImpl_) {
#ifndef Q_OS_WIN32
    messager.pImpl_.reset(new MessagerUnix);
#else
#endif
    }
    return messager;
}

void Messager::sendMessage(pid_t receiver, const QString &message)
{
    if (pImpl_) {
        pImpl_->sendMessage(receiver, message);
    }
}
