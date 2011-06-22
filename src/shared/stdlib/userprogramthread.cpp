#include "userprogramthread.h"
#include <iostream>

namespace StdLib {


UserProgramThread::UserProgramThread( QObject *parent, void(*func)(), bool connectMode )
    : QThread(parent)
{
    m_func = func;
    b_connectMode = connectMode;
    i_timerId = connectMode? -1 : startTimer(500);
    b_started = false;
}
UserProgramThread::~UserProgramThread() {
    if (i_timerId!=-1)
        killTimer(i_timerId);
}


void UserProgramThread::timerEvent(QTimerEvent *e) {
    e->accept();
    if (!b_started) {
        b_started = true;
        start();
    }
}

void UserProgramThread::run() {
    (*m_func)();
    if (!b_connectMode)
        std::cerr << tr("\nEvaluation finished. Close all windows to exit program.\n").toLocal8Bit().data();
}


} // namespace GuiRunner
