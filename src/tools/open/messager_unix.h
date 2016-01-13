#ifndef MESSAGER_UNIX_H
#define MESSAGER_UNIX_H

#include "messager.h"

class MessagerUnix
        : public Messager::ImplInterface
{
public:
    void sendMessage(pid_t receiver, const QString &message);
};


#endif
