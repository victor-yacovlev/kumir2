#ifndef PROCMANAGER_H
#define PROCMANAGER_H

#include <QtGlobal>
#ifdef Q_OS_WIN32
#include <Windows.h>
typedef DWORD Pid;
#else
#include <sys/types.h>
typedef pid_t Pid;
#endif

#include <QScopedPointer>

class ProcessManager
{
public:
    static ProcessManager & get();
    Pid find(const QString & executable);

    class ImplInterface {
    public:
        virtual Pid find(const QString &executable) = 0;
        virtual ~ImplInterface() {}
    };

private:
    explicit ProcessManager();
    QScopedPointer<ImplInterface> pImpl_;
};

#endif
