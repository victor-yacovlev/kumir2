#ifndef VISUAL_COMPONENT
#define VISUAL_COMPONENT

#include <QtGui>

#ifdef EXTENSIONSYSTEM_LIBRARY
#define EXTENSIONSYSTEM_EXPORT Q_DECL_EXPORT
#else
#define EXTENSIONSYSTEM_EXPORT Q_DECL_IMPORT
#endif


namespace ExtensionSystem {

struct MenuActionsGroup {
    QString menuText;
    QList<QAction*> actions;
};

class EXTENSIONSYSTEM_EXPORT VisualComponent:
        public QWidget
{
    Q_OBJECT
public:
    virtual QList<QAction*> toolbarActions() = 0;
    virtual QList<MenuActionsGroup> menuActions() = 0;
signals:
    void urlsDragAndDropped(const QList<QUrl> &);
};

}

#endif
