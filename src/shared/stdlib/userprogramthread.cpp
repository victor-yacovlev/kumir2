#include "userprogramthread.h"
#include <iostream>

namespace StdLib {


UserProgramThread::UserProgramThread( QObject *parent, void(*func)() )
    : QThread(parent)
{
    m_func = func;
    i_timerId = startTimer(500);
    b_started = false;
}
UserProgramThread::~UserProgramThread() {
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
    std::cerr << tr("\nEvaluation finished. Close all windows to exit program.\n").toLocal8Bit().data();
}


} // namespace GuiRunner
