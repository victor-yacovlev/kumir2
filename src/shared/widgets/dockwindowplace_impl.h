#ifndef WIDGETS_DOCKWINDOWPLACE_IMPL_H
#define WIDGETS_DOCKWINDOWPLACE_IMPL_H

#include "secondarywindow_interface.h"
#include "extensionsystem/settings.h"
#include "dockwindowplace.h"
#include <QObject>

namespace Widgets {

class DockWindowPlaceImpl : public QObject
{
    Q_OBJECT
public:
    explicit DockWindowPlaceImpl(DockWindowPlace * parent,
                                 const QString & settingsKey);

    void registerWindowHere(class SecondaryWindow * window);
    void addPersistentWidget(QWidget * widget, const QString & title);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

signals:
    
public slots:

public /*fields*/:
    DockWindowPlace * pClass_;
    QString settingsKey_;
    ExtensionSystem::SettingsPtr settings_;
    QList<SecondaryWindowImplementationInterface*> dockWidgets_;
    QList<QWidget*> allWidgets_;
    QSize preferredSize_;
};

} // namespace Widgets

#endif // WIDGETS_DOCKWINDOWPLACE_IMPL_H
