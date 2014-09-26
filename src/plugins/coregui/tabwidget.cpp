#include "tabwidget.h"
#include "tabbar.h"

namespace CoreGUI {

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    TabBar * tb = new TabBar(this);
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    setTabBar(tb);
    // qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
}

QSize TabWidget::minimumSizeHint() const
{
    QSize result(0, 0);
    if (currentWidget())
        result = currentWidget()->minimumSizeHint();
    if (tabBar()->isVisible())
        result += QSize(0, tabBar()->height());
    return result;
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
        painter.setPen(QPen(palette().brush(QPalette::Window).color()));
        painter.drawLine(0, 0, width(), 0);
    }
#endif
}


} // namespace CoreGUI
