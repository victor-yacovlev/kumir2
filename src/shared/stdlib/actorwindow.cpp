#include "actorwindow.h"

namespace StdLib {

ActorWindow::ActorWindow(QWidget * widget, const QList<QAction*> & actions) :
    QMainWindow(0)
{
    setWindowTitle(qApp->arguments()[0]);
    setCentralWidget(widget);
    QMenu * menu = menuBar()->addMenu(tr("Actions"));
    menu->addActions(actions);
}

} // namespace GuiRunner
