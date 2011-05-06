#ifndef VISUAL_COMPONENT
#define VISUAL_COMPONENT

#include <QtGui>

namespace Shared {

struct MenuActionsGroup {
    QString menuText;
    QList<QAction*> actions;
};

class VisualComponent:
        public QWidget
{
public:
    virtual QList<QAction*> toolbarActions() = 0;
    virtual QList<MenuActionsGroup> menuActions() = 0;
};

}

#endif
