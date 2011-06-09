#include "tabwidget.h"

namespace CoreGUI {

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent)
{
}

void TabWidget::tabInserted(int)
{
    setTabsClosable(count()>1);
}

void TabWidget::tabRemoved(int)
{
    setTabsClosable(count()>1);
}

} // namespace CoreGUI
