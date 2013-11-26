#ifndef PROCMANAGER_H
#define PROCMANAGER_H

#include <QtGlobal>
#ifdef Q_OS_WIN32
#include <Windows.h>
#else
#include <sys/types.h>
#endif

#include <QScopedPointer>

class ProcessManager
{
public:
    static ProcessManager & get();
    pid_t find(const QString & executable);

    class ImplInterface {
    public:
        virtual pid_t find(const QString &executable) = 0;
    };

private:
    explicit ProcessManager();
    QScopedPointer<ImplInterface> pImpl_;
};

#endif
