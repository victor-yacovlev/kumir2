#ifndef MENUBAR_H
#define MENUBAR_H

#include <QtCore>
#include <QtGui>

class MenuBar
        : public QMenuBar
{
public:
    inline MenuBar(QWidget * parent): QMenuBar(parent) {}
protected:
    inline bool eventFilter(QObject *object, QEvent *event) {
        bool catched = false;

        if (event->type()==QEvent::KeyRelease) {
            QKeyEvent *kev = static_cast<QKeyEvent*>(event);
            if (kev->key() == Qt::Key_Alt || kev->key() == Qt::Key_Meta) {
                catched = true;
            }
        }

        if (catched)
            return false;
        else
            return QMenuBar::eventFilter(object, event);
    }
};

#endif // MENUBAR_H
