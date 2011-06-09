#ifndef GUIRUNNER_ACTORWINDOW_H
#define GUIRUNNER_ACTORWINDOW_H

#include <QtGui>

#include "extensionsystem/visualcomponent.h"

namespace GuiRunner {

class ActorWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ActorWindow(ExtensionSystem::VisualComponent * c);

signals:

public slots:

};

} // namespace GuiRunner

#endif // GUIRUNNER_ACTORWINDOW_H
