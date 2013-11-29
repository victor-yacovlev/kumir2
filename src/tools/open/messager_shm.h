#ifndef MESSAGER_WINAPI_H
#define MESSAGER_WINAPI_H

#include "messager.h"

class MessagerShm
        : public Messager::ImplInterface
{
public:
    void sendMessage(Pid pid, const QString & message);
};

#endif
