#include "dockwindowplace_impl.h"
#include "dockwindowplace.h"
#include "secondarywindow.h"

namespace Widgets {

DockWindowPlaceImpl::DockWindowPlaceImpl(DockWindowPlace *parent,
                                         const QString & settingsKey)
    : pClass_(parent)
    , settingsKey_(settingsKey)
{
    pClass_ = qobject_cast<DockWindowPlace*>(parent);
    pClass_->setVisible(false);
}

void DockWindowPlaceImpl::registerWindowHere(SecondaryWindow *window)
{
    allWindows_.push_back(window);
}

QWidget * DockWindowPlaceImpl::widget()
{
    return pClass_;
}

void DockWindowPlaceImpl::addPersistentWidget(QWidget *widget)
{
    allWindows_.push_front(widget);
    widgetsInDock_.push_front(widget);
    pClass_->addTab(widget, widget->windowTitle());
    preferredSize_ = widget->sizeHint();
    pClass_->tabBar()->setVisible(widgetsInDock_.size() > 1);
    pClass_->updateGeometry();
    pClass_->setVisible(true);
}

QSize DockWindowPlaceImpl::minimumSizeHint() const
{
    int hh = pClass_->tabBar()->isVisible()
            ? pClass_->tabBar()->minimumSizeHint().height() : 0;
    int h = 0;
    int w = pClass_->tabBar()->minimumSizeHint().width();
    foreach (const QWidget * window , widgetsInDock_) {
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

void DockWindowPlaceImpl::dockWindow(SecondaryWindow *window, bool forceVisible)
{
    Q_ASSERT(allWindows_.contains(window));
    if (!widgetsInDock_.contains(window)) {
        widgetsInDock_.push_back(window);
        preferredSize_ = window->size();
        pClass_->addTab(window, window->windowTitle());
        pClass_->setCurrentIndex(pClass_->count()-1);
        pClass_->tabBar()->setVisible(widgetsInDock_.size() > 1);
        pClass_->updateGeometry();
        emit pClass_->visiblityRequest(true, sizeHint());
    }
    else {
        emit pClass_->visiblityRequest(true, QSize());
    }
    if (!forceVisible)
        updateVisiblity();
    else {
        pClass_->setVisible(true);
        window->setVisible(true);
    }
}

void DockWindowPlaceImpl::undockWindow(SecondaryWindow *window)
{
    Q_ASSERT(allWindows_.contains(window));
    if (!widgetsInDock_.contains(window))
        return;
    widgetsInDock_.removeAll(window);
    pClass_->removeTab(pClass_->count()-1);
    pClass_->tabBar()->setVisible(widgetsInDock_.size() > 1);
    updateVisiblity();
    if (pClass_->count() > 0)
        pClass_->setCurrentIndex(pClass_->count()-1);
    emit pClass_->visiblityRequest(widgetsInDock_.size() > 0, QSize());
}

void DockWindowPlaceImpl::updateVisiblity()
{
    bool visible = false;
    foreach (QWidget * window , widgetsInDock_)
        visible = visible || window->isVisible();
    pClass_->setVisible(visible);
}

bool DockWindowPlaceImpl::isWindowDocked(const SecondaryWindow *window) const
{
    SecondaryWindow * w = const_cast<SecondaryWindow*>(window);
    return widgetsInDock_.contains(w);
}

void DockWindowPlaceImpl::updateSettings(ExtensionSystem::SettingsPtr settings)
{
    if (settings_) saveState();
    settings_ = settings;
    foreach (QWidget * window , allWindows_) {
        SecondaryWindow * w = qobject_cast<SecondaryWindow*>(window);
        if (w)
            w->setSettingsObject(settings);
    }
    restoreState();
}

void DockWindowPlaceImpl::saveState()
{
    foreach (QWidget * window , allWindows_) {
        SecondaryWindow * w = qobject_cast<SecondaryWindow*>(window);
        if (w)
            w->saveState();
    }
}

void DockWindowPlaceImpl::restoreState()
{
    foreach (QWidget * window , allWindows_) {
        SecondaryWindow * w = qobject_cast<SecondaryWindow*>(window);
        if (w)
            w->restoreState();
    }
}

} // namespace Widgets
