#include "dockwindowplace.h"
#include "dockwindowplace_impl.h"

namespace Widgets {

DockWindowPlace::DockWindowPlace(QWidget *parent,
                                 const QString settingsKey)
    : QTabWidget(parent)
    , pImpl_(new DockWindowPlaceImpl(this, settingsKey))
{
    setAutoFillBackground(true);
    setTabPosition(QTabWidget::South);
}

void DockWindowPlace::registerWindowHere(SecondaryWindow *window)
{
    pImpl_->registerWindowHere(window);
}

void DockWindowPlace::addPersistentWidget(QWidget *widget)
{
    pImpl_->addPersistentWidget(widget);
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
    pImpl_->updateSettings(settings);
}

void DockWindowPlace::saveState()
{
    pImpl_->saveState();
}

void DockWindowPlace::restoreState()
{
    pImpl_->restoreState();
}

} // namespace Widgets
