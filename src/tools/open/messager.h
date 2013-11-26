#ifndef MESSAGER_H
#define MESSAGER_H

#include <QtGlobal>
#ifdef Q_OS_WIN32
#include <Windows.h>
#else
#include <sys/types.h>
#endif

#include <QString>
#include <QScopedPointer>

class Messager
{
public:

    static Messager & get();
    void sendMessage(pid_t receiver, const QString & message);

    class ImplInterface {
    public:
        virtual void sendMessage(pid_t receiver, const QString & message) = 0;
    };

private:
    explicit Messager();
    QScopedPointer<ImplInterface> pImpl_;
};

#endif
