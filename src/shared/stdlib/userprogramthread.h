#ifndef GUIRUNNER_USERPROGRAMTHREAD_H
#define GUIRUNNER_USERPROGRAMTHREAD_H

#include <QtCore>

namespace StdLib {

class UserProgramThread
        : public QThread
{
public:
    explicit UserProgramThread( QObject *parent, void(*func)(), bool connectMode );
    ~UserProgramThread();

protected:
    void timerEvent(QTimerEvent *e);
    void run();

private:
    void (*m_func)();
    int i_timerId;
    bool b_started;
    bool b_connectMode;
};

}

#endif
