#ifndef COREGUI_TABWIDGET_H
#define COREGUI_TABWIDGET_H

#include <QtCore>
#include <QtGui>

namespace CoreGUI {

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = 0);
    void disableTabs();
protected:
    void paintEvent(QPaintEvent *);
};

} // namespace CoreGUI

#endif // COREGUI_TABWIDGET_H
