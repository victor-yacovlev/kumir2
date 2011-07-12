#include <QtCore>
#include <QtGui>
#include <iostream>
#include "userprogramthread.h"
#include "actorwindow.h"
#include "stdlib/connector.h"
#include "guirunner.h"


extern "C" int __main_gui__( int argc, char *argv[],
                              void(*creator_funct)(),
                              void(*main_funct)() )
{
    QApplication * a = new QApplication(argc, argv);
    int pid = 0;
    foreach (const QString & arg, a->arguments()) {
        if (arg.startsWith("--key=")) {
            pid = arg.mid(6).toInt();
        }
    }
    if (pid) {
        StdLib::Connector::instance()->connectTo(pid);
    }
    (*creator_funct)();
    StdLib::UserProgramThread * thread = new StdLib::UserProgramThread(a, main_funct, pid);
    int result = 0;
    if (pid) {
        thread->start();
        thread->wait();
        result = 0;
    }
    else {
        result = a->exec();
    }
    delete thread;
    delete a;
    return result;
}


extern void __create_window_for__(QWidget * widget, const QList<QAction*> & menus)
{
    StdLib::ActorWindow * w = new StdLib::ActorWindow(widget, menus);
    w->show();
}
