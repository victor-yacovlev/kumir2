#ifndef GUIRUNNER_ACTORWINDOW_H
#define GUIRUNNER_ACTORWINDOW_H

#include <QtGui>

namespace StdLib {

class ActorWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ActorWindow(QWidget * c, const QList<QAction*> & menus);

signals:

public slots:

};

}

#endif
