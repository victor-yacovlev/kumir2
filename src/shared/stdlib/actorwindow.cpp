#include "actorwindow.h"

namespace StdLib {

ActorWindow::ActorWindow(QWidget * widget, const QList<QMenu*> & menus) :
    QMainWindow(0)
{
    setWindowTitle(qApp->arguments()[0]);
    setCentralWidget(widget);
    for (int i=0; i<menus.size(); i++) {
        menuBar()->addMenu(menus[i]);
    }
}

} // namespace GuiRunner
