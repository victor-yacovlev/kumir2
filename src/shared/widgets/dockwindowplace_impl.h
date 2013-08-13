#ifndef WIDGETS_DOCKWINDOWPLACE_IMPL_H
#define WIDGETS_DOCKWINDOWPLACE_IMPL_H

#include "extensionsystem/settings.h"

#include <QList>
#include <QSize>


namespace Widgets {

class DockWindowPlaceImpl
{
public:
    explicit DockWindowPlaceImpl(class DockWindowPlace *parent,
                                 const QString & settingsKey);
    void addPersistentWidget(QWidget * widget);
    void registerWindowHere(class SecondaryWindow * window);
    QWidget * widget();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void dockWindow(class SecondaryWindow * window, bool forceVisible);
    void undockWindow(class SecondaryWindow * window);
    bool isWindowDocked(const class SecondaryWindow * window) const;
    void updateSettings(ExtensionSystem::SettingsPtr settings);
    void saveState();
    void restoreState();

private /*methods*/:
    void updateVisiblity();

private /*fields*/:
    class DockWindowPlace * pClass_;
    ExtensionSystem::SettingsPtr settings_;
    QString settingsKey_;
    QList<QWidget *> widgetsInDock_;
    QList<QWidget *> allWindows_;
    QSize preferredSize_;
};

} // namespace Widgets

#endif // WIDGETS_DOCKWINDOWPLACE_IMPL_H
