#include <QtCore>
#include <QtGui>
#include <iostream>
#include "userprogramthread.h"
#include "actorwindow.h"
#include "stdlib/connector.h"
#include "guirunner.h"


extern "C" void __main_gui__( int argc, char *argv[],
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
    GuiRunner::UserProgramThread * thread = new GuiRunner::UserProgramThread(a, main_funct);
    a->exec();
    delete thread;
    delete a;
}


extern void __create_window_for__(ExtensionSystem::VisualComponent * component)
{
    GuiRunner::ActorWindow * w = new GuiRunner::ActorWindow(component);
    w->show();
}
