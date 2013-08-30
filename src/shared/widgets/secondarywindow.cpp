#include "secondarywindow.h"
#include "secondarywindow_interface.h"
#include "dockwindowplace.h"
#include "secondarywindow_generic.h"
#include "dockwindowplace_container.h"
#include <QApplication>
#include <QDesktopWidget>

namespace Widgets {

SecondaryWindow::SecondaryWindow(QWidget *topLevelParent,
                                 class SecondaryWindowImplementationInterface * windowContainer,
                                 class SecondaryWindowImplementationInterface * dockContainer,
                                 const QString & settingsKey)
    : QObject(topLevelParent)
    , dockContainer_(dockContainer)
    , windowContainer_(windowContainer)
    , settingsKey_(settingsKey)
    , topLevelParent_(topLevelParent)
{

}

SecondaryWindow * SecondaryWindow::createSecondaryWindow(
        QWidget *centralWidget,
        const QString &title,
        QWidget *topLevelParent,
        DockWindowPlace *dockPlace,
        const QString &settingsKey,
        bool resizable)
{
    SecondaryWindowImplementationInterface * window =
            createWindowContainer(
                                  title,
                                  topLevelParent,
                                  resizable
                                  );

    SecondaryWindowImplementationInterface * dock =
            createDockContainer(
                                title,
                                dockPlace
                                );

    SecondaryWindow * result = new SecondaryWindow(topLevelParent,
                                                   window, dock, settingsKey);

    if (dock) {
        dock->setPairedContainer(window);
        dockPlace->registerWindowHere(result);
        window->setPairedContainer(dock);
    }

    window->getWidgetOwnership(centralWidget);

    const QMetaObject * meta = centralWidget->metaObject();
    if (meta->indexOfSignal("resizeRequest(QSize)") != -1) {
        connect(centralWidget, SIGNAL(resizeRequest(QSize)),
                window->toWidget(), SLOT(handleResizeRequest(QSize)));
    }

    return result;
}

SecondaryWindowImplementationInterface *
SecondaryWindow::createWindowContainer(const QString &title,
                                       QWidget *topLevelParent,
                                       bool resizable)
{
    SecondaryWindowImplementationInterface * result = nullptr;

    result = new SecondaryWindowGenericImplementation(topLevelParent);
    //          TODO create platform-specific implementations

    result->setResizeble(resizable);
    result->setTitle(title);

    return result;
}

SecondaryWindowImplementationInterface *
SecondaryWindow::createDockContainer(const QString &title,
                                     DockWindowPlace *dockPlace)
{
    if (dockPlace) {
        SecondaryWindowImplementationInterface * result =
                new DockWindowPlaceContainer(dockPlace);

        result->setTitle(title);
        return result;
    }
    else {
        return nullptr;
    }
}

SecondaryWindowImplementationInterface *
SecondaryWindow::dockContainer()
{
    return dockContainer_;
}

SecondaryWindowImplementationInterface *
SecondaryWindow::windowContainer()
{
    return windowContainer_;
}

const QString & SecondaryWindow::settingsKey() const
{
    return settingsKey_;
}

void SecondaryWindow::activate()
{
    SecondaryWindowImplementationInterface * whom = currentContainer();
    if (whom) {
        whom->activate(QPoint(), QSize());
    }
}

SecondaryWindowImplementationInterface * SecondaryWindow::currentContainer()
{
    SecondaryWindowImplementationInterface * result = nullptr;
    if (windowContainer_ && windowContainer_->hasWidgetOwnership()) {
        result = windowContainer_;
    }
    else if (dockContainer_ && dockContainer_->hasWidgetOwnership()) {
        result = dockContainer_;
    }
    return result;
}

void SecondaryWindow::updateSettings(ExtensionSystem::SettingsPtr settings,
                                     const QStringList &)
{
    saveState();
    settings_ = settings;
    restoreState();
}

static const char * WindowSize = "/WindowSize";
static const char * WindowPos = "/WindowPos";
static const char * DockSize = "/DockSize";
static const char * IsDocked = "/IsDocked";
static const char * IsDockVisible = "/IsDockVisible";

void SecondaryWindow::saveState()
{
    if (!settings_) return;
    if (windowContainer_) {
        settings_->setValue(settingsKey_ + WindowSize,
                            windowContainer_->toWidget()->size());
        settings_->setValue(settingsKey_ + WindowPos,
                            windowContainer_->toWidget()->pos());

    }
    if (dockContainer_) {
        settings_->setValue(settingsKey_ + DockSize,
                            dockContainer_->toWidget()->size());
        settings_->setValue(settingsKey_ + IsDockVisible,
                            dockContainer_->toWidget()->isVisible());
        settings_->setValue(settingsKey_ + IsDocked,
                            dockContainer_ == currentContainer());
    }
}

void SecondaryWindow::restoreState()
{
    if (!settings_) return;
    if (windowContainer_) {
        QWidget * window = windowContainer_->toWidget();
        QSize sz = settings_->value(settingsKey_ + WindowSize).toSize();
        QPoint ps = settings_->value(settingsKey_ + WindowPos, QPoint(-1, -1)).toPoint();
        if (ps == QPoint(-1, -1)) {
            ps = QApplication::desktop()->availableGeometry(topLevelParent_).center();
            ps.rx() -= sz.width() / 2;
            ps.ry() -= sz.height() / 2;
        }
        window->resize(sz);
        window->move(ps);
    }    
    bool docked = settings_->value(settingsKey_ + IsDocked, false).toBool();
    if (dockContainer_ && docked) {
        QWidget * w = currentContainer()->releaseWidgetOwnership();
        dockContainer_->getWidgetOwnership(w);
        bool dockVisible = settings_->value(settingsKey_ + IsDockVisible, false).toBool();
        if (dockVisible) {
            dockContainer_->activate(QPoint(), QSize());
        }
        QWidget * dock = dockContainer_->toWidget();
        QSize sz = settings_->value(settingsKey_ + DockSize).toSize();
        dock->resize(sz);
    }
    else {
        QWidget * w = currentContainer()->releaseWidgetOwnership();
        windowContainer_->getWidgetOwnership(w);
    }
}

} // namespace Widgets
