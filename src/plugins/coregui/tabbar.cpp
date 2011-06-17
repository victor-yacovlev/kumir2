#include "tabbar.h"

namespace CoreGUI {

TabBar::TabBar(QWidget *parent) :
    QTabBar(parent)
{
    setTabsClosable(true);
    static const char * css =
            "QTabBar::tab {"
            "  min-width: 18ex;"
            "  height: 28px;"
            "  text-align: right;"
            "}"
            "QTabBar::close-button {"
            "  image: url(:/coregui/close-tab.png);"
            "}"
            "QTabBar::close-button:hover {"
            "  image: url(:/coregui/close-tab-hovered.png);"
            "}"
            ;
    setStyleSheet(QString::fromAscii(css));
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(handleChanged(int)));
}


void TabBar::tabInserted(int index)
{
    QTabBar::tabInserted(index);
    handleChanged(index);
}

void TabBar::tabRemoved(int index)
{
    QTabBar::tabRemoved(index);
    handleChanged(index);
}

void TabBar::handleChanged(int index)
{
    Q_UNUSED(index);
    if (tabButton(0, QTabBar::RightSide)) {
        tabButton(0, QTabBar::RightSide)->resize(QSize(0,0));
        tabButton(0, QTabBar::RightSide)->setVisible(false);
    }
}

} // namespace CoreGUI
