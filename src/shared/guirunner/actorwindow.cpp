#include "actorwindow.h"

namespace GuiRunner {

ActorWindow::ActorWindow(ExtensionSystem::VisualComponent * c) :
    QMainWindow(0)
{
    setWindowTitle(qApp->arguments()[0]);
    setCentralWidget(c);
    for (int i=0; i<c->menuActions().size(); i++) {
        ExtensionSystem::MenuActionsGroup group = c->menuActions()[i];
        QMenu * menu = menuBar()->addMenu(group.menuText);
        menu->setTearOffEnabled(true);
        menu->setWindowTitle(group.menuText);
        menu->addActions(group.actions);
    }
}

} // namespace GuiRunner
