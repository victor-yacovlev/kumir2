#include "tabwidget.h"
#include "tabbar.h"

namespace CoreGUI {

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    TabBar * tb = new TabBar(this);
    setTabBar(tb);
    customizeStyle();
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

void TabWidget::customizeStyle()
{
#ifdef Q_OS_MAC
    static const char * css =
            "QTabBar::close-button {"
            "  image: url(:/coregui/close-tab.png);"
            "}"
            "QTabBar::close-button:hover {"
            "  image: url(:/coregui/close-tab-hovered.png);"
            "}"
            ;
#else
    static const char * css =
            "QTabBar::tab {"
            "  min-width: 18ex;"
            "  height: 28px;"
            "  text-align: right;"
            "}"
            "QTabBar::tab:selected {"
            "}"
            "QTabBar::close-button {"
            "  image: url(:/coregui/close-tab.png);"
            "}"
            "QTabBar::close-button:hover {"
            "  image: url(:/coregui/close-tab-hovered.png);"
            "}"
            ;
#endif // MAC

    const QString preprocessedCss = QString::fromLatin1(css)
            .replace("$windowColor", palette().brush(QPalette::Window).color().name())
            .replace("$baseColor", palette().brush(QPalette::Base).color().name())
            .replace("$textColor", palette().brush(QPalette::Text).color().name())
            ;

    setStyleSheet(preprocessedCss);
}


} // namespace CoreGUI
