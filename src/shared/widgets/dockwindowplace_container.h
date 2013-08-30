#ifndef WIDGETS_DOCKWINDOWPLACE_CONTAINER_H
#define WIDGETS_DOCKWINDOWPLACE_CONTAINER_H

#include "secondarywindow_interface.h"
#include "secondarywindow_generic.h"

#include <QWidget>
#include <QPushButton>
#include <QLabel>

namespace Widgets {

class DockWindowPlaceContainer
        : public SecondaryWindowGenericImplementation
{
    friend class SecondaryWindow;
    Q_OBJECT
public:        

signals:
    
public slots:
    virtual void activate(const QPoint &ps, const QSize &sz);
    virtual void deactivate();

private /*methods*/:
    explicit DockWindowPlaceContainer(class DockWindowPlace *parent);

    virtual void paintWindowFrame();
    virtual void setupWindow();
    virtual void setupWidgetsAppearance();
    virtual HitTestResult hitTest(const QPoint & mousePosition) const;
    virtual void notifyOnDocked();

private /*fields*/:
    class DockWindowPlace * place_;
};

} // namespace Widgets

#endif // WIDGETS_DOCKWINDOWPLACE_CONTAINER_H
