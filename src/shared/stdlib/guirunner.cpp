#include <QtCore>
#include <QtGui>
#include <iostream>
#include "userprogramthread.h"
#include "actorwindow.h"
#include "connector.h"
#include "guirunner.h"


extern "C" int __main_gui__( int argc, char *argv[],
                              void(*creator_funct)(),
                              void(*main_funct)() )
{
    QApplication * a = new QApplication(argc, argv);
    __try_connect_to_kumir__(argc, argv);
    (*creator_funct)();
    StdLib::UserProgramThread * thread =
            new StdLib::UserProgramThread(
                a,
                main_funct,
                __connected_to_kumir__());
    int result = 0;
    if (__connected_to_kumir__()) {
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
