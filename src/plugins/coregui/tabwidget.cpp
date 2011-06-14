#include "tabwidget.h"

namespace CoreGUI {

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    a_closeCurrentTab = new QAction(this);
    a_closeCurrentTab->setIcon(QIcon::fromTheme("tab-close"));
    a_closeCurrentTab->setToolTip(tr("Close current tab"));
    a_closeCurrentTab->setText("X");
    connect(a_closeCurrentTab, SIGNAL(triggered()), this, SLOT(handleCloseButtonPressed()));
    QToolButton * btn = new QToolButton(this);
    btn->setAutoRaise(true);
    btn->setDefaultAction(a_closeCurrentTab);
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(handleCurrentChanged(int)));
    setCornerWidget(btn);
}

void TabWidget::handleCurrentChanged(int index)
{
    Q_UNUSED(index);
    QWidget * w = currentWidget();
    bool uncloseable = w->property("uncloseable").toBool();
    a_closeCurrentTab->setEnabled(!uncloseable);

}

void TabWidget::handleCloseButtonPressed()
{
    emit tabCloseRequested(currentIndex());
}

void TabWidget::tabInserted(int)
{
//    setTabsClosable(count()>1);
    handleCurrentChanged(currentIndex());
}

void TabWidget::tabRemoved(int)
{
//    setTabsClosable(count()>1);
    handleCurrentChanged(currentIndex());
}

} // namespace CoreGUI
