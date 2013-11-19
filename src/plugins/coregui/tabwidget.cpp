#include "tabwidget.h"
#include "tabbar.h"

namespace CoreGUI {

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    TabBar * tb = new TabBar(this);
    setTabBar(tb);
}

void TabWidget::disableTabs()
{
    tabBar()->setEnabled(false);
    tabBar()->setVisible(false);
}

void TabWidget::paintEvent(QPaintEvent * e)
{
    QTabWidget::paintEvent(e);
#ifdef Q_WS_X11
    if (!tabBar()->isVisible()) {
        QPainter painter(this);
        painter.setPen(QPen(palette().brush(QPalette::WindowText).color()));
        painter.drawLine(0, 0, width(), 0);
    }
#endif
}


} // namespace CoreGUI
