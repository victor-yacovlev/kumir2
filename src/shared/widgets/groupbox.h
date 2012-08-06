#ifndef GROUPBOX_H
#define GROUPBOX_H
#include <QtCore>
#include <QtGui>

#ifdef WIDGETS_LIBRARY
#define WIDGETS_EXPORT Q_DECL_EXPORT
#else
#define WIDGETS_EXPORT Q_DECL_IMPORT
#endif

/*
 * Copyright notice.
 *
 * The solution found here:
 * http://lists.trolltech.com/qt-interest/2008-08/thread00130-0.html
 */

namespace Widgets {

class WIDGETS_EXPORT GroupBox
        : public QGroupBox
{
    Q_OBJECT
    Q_PROPERTY(bool collapsed READ isCollapsed WRITE setCollapsed)
public:
    GroupBox(QWidget * parent = 0);
    GroupBox(const QString & title, QWidget * parent = 0);

    inline bool isCollapsed() { return b_collapsed; }
    void setCollapsed(bool collapse);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
private:
    QPoint pnt_clickPos;
    bool b_collapsed;
};

}

#endif
