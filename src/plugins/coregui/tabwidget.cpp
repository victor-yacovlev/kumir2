#include "tabwidget.h"
#include "tabbar.h"

namespace CoreGUI {

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    TabBar * tb = new TabBar(this);
    setTabBar(tb);
//    setStyleSheet("background-color: white;");
}

void TabWidget::disableTabs()
{
    tabBar()->setEnabled(false);
    tabBar()->setVisible(false);
}

} // namespace CoreGUI
