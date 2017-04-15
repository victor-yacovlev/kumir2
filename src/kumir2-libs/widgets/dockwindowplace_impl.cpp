#include "secondarywindow.h"
#include "dockwindowplace_impl.h"

#include <QTabBar>

namespace Widgets {

DockWindowPlaceImpl::DockWindowPlaceImpl(DockWindowPlace *parent,
                                         const QString & settingsKey)
    : QObject(parent)
    , pClass_(parent)
    , settingsKey_(settingsKey)
{
}

void DockWindowPlaceImpl::registerWindowHere(SecondaryWindow *window)
{
    if (window->dockContainer()) {
        dockWidgets_.push_back(window->dockContainer());
    }
}

void DockWindowPlaceImpl::addPersistentWidget(QWidget *widget,
                                              const QString &title)
{
    pClass_->addTab(widget, title);
}

QSize DockWindowPlaceImpl::minimumSizeHint() const
{
    int hh = pClass_->tabBar()->isVisible()
            ? pClass_->tabBar()->minimumSizeHint().height() : 0;
    int h = 0;
    int w = pClass_->tabBar()->minimumSizeHint().width();
    for (int i=0; i<pClass_->count(); i++) {
        const QWidget * window = pClass_->widget(i);
        const QSize windowMinimumSize = window->minimumSizeHint();
        w = qMax(w, windowMinimumSize.width());
        h = qMax(h, windowMinimumSize.height());
    }
    return QSize(w, hh + h);
}

QSize DockWindowPlaceImpl::sizeHint() const
{
    if (!preferredSize_.isValid())
        return minimumSizeHint();
    QSize size = preferredSize_;
    if (pClass_->tabBar()->isVisible()) {
        size.rheight() += pClass_->tabBar()->height();
        size.rwidth() = qMax(size.rwidth(), pClass_->tabBar()->width());
    }
    return size;
}


} // namespace Widgets
