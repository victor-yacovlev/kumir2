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
    QString key;
    foreach (const QString & arg, a->arguments()) {
        if (arg.startsWith("--key=")) {
            key = arg.mid(6);
        }
    }
    if (!key.isEmpty()) {
        StdLib::Connector::instance()->connectTo(key);
    }
    (*creator_funct)();
    StdLib::UserProgramThread * thread = new StdLib::UserProgramThread(a, main_funct);
    int result = a->exec();
    delete thread;
    delete a;
    return result;
}


extern void __create_window_for__(QWidget * widget, const QList<QAction*> & menus)
{
    StdLib::ActorWindow * w = new StdLib::ActorWindow(widget, menus);
    w->show();
}
