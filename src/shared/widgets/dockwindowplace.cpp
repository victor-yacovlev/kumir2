#include "dockwindowplace.h"
#include "dockwindowplace_impl.h"

#include "extensionsystem/settings.h"

#include <QTabBar>
#include <QResizeEvent>
#include <QDebug>

namespace Widgets {

DockWindowPlace::DockWindowPlace(QWidget *parent,
                                 const QString settingsKey)
    : QTabWidget(parent)
    , pImpl_(new DockWindowPlaceImpl(this, settingsKey))
{
    setAutoFillBackground(true);
    setTabPosition(QTabWidget::South);
    setVisible(false);
}

void DockWindowPlace::tabInserted(int)
{
    updateGeometry();
    setVisible(true);
    tabBar()->setVisible(count() > 1);
    emit visiblityRequest(true, sizeHint());
}

void DockWindowPlace::tabRemoved(int)
{
    updateGeometry();
    setVisible(count() > 0);
    tabBar()->setVisible(count() > 1);
    emit visiblityRequest(isVisible(), QSize());
}

void DockWindowPlace::resizeEvent(QResizeEvent *e)
{
    const QSize minSize = minimumSizeHint();
    if (e->size().width() < minSize.width() || e->size().height() < minSize.height()) {
    }
    else {
        QWidget * curW = currentWidget();
        if (curW) {
            pImpl_->preferredSize_ = curW->size();
        }
    }
    QTabWidget::resizeEvent(e);
}

void DockWindowPlace::setPreferredItemSize(const QSize &size)
{
    if (count() == 0) {
        pImpl_->preferredSize_ = size;
    }
    else {
        pImpl_->preferredSize_.rwidth() = qMax(
                    pImpl_->preferredSize_.width(), size.width()
                    );
        pImpl_->preferredSize_.rheight() = qMax(
                    pImpl_->preferredSize_.height(), size.height()
                    );
    }
}

void DockWindowPlace::registerWindowHere(SecondaryWindow *window)
{
    pImpl_->registerWindowHere(window);
}

void DockWindowPlace::addPersistentWidget(QWidget *widget,
                                          const QString & title)
{
    pImpl_->addPersistentWidget(widget, title);
}

QSize DockWindowPlace::minimumSizeHint() const
{
    return pImpl_->minimumSizeHint();
}

QSize DockWindowPlace::sizeHint() const
{
    return pImpl_->sizeHint();
}

void DockWindowPlace::updateSettings(ExtensionSystem::SettingsPtr settings)
{
    saveState();
    pImpl_->settings_ = settings;
    restoreState();
}

void DockWindowPlace::saveState()
{
    if (!pImpl_->settings_) return;
    const QString key1 = pImpl_->settingsKey_ + "/DockPlaceSize";
    const QString key2 = pImpl_->settingsKey_ + "/NewWidgetSize";
    pImpl_->settings_->setValue(key1, size());
    pImpl_->settings_->setValue(key2, pImpl_->preferredSize_);

}

void DockWindowPlace::restoreState()
{
    if (!pImpl_->settings_) return;
    const QString key1 = pImpl_->settingsKey_ + "/DockPlaceSize";
    const QString key2 = pImpl_->settingsKey_ + "/NewWidgetSize";
    const QSize sz = pImpl_->settings_->value(key1, QSize()).toSize();
    resize(sz);
    pImpl_->preferredSize_ = pImpl_->settings_->value(key2, QSize()).toSize();
}

} // namespace Widgets
