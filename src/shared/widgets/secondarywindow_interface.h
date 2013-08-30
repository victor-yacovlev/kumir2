#ifndef SECONDARYWINDOW_INTERFACE_H
#define SECONDARYWINDOW_INTERFACE_H

#include <QWidget>

namespace Widgets {

class SecondaryWindowImplementationInterface
{
public:

    virtual QWidget * toWidget() = 0;

    virtual bool hasWidgetOwnership() const = 0;
    virtual void getWidgetOwnership(QWidget * w) = 0;
    virtual QWidget * releaseWidgetOwnership() = 0;
    virtual void setPairedContainer(SecondaryWindowImplementationInterface *pairedContainer) = 0;

    virtual void activate(const QPoint & ps, const QSize & sz) = 0;
    virtual void deactivate() = 0;

    virtual void setResizeble(bool v) = 0;
    virtual bool isResizable() const = 0;

    virtual bool isDockable() const = 0;

    virtual void setTitle(const QString & title) = 0;
    virtual QString title() const = 0;

};

}

Q_DECLARE_INTERFACE(Widgets::SecondaryWindowImplementationInterface,
                    "kumir2.widgets.secondarywindow.implementation")

#endif // SECONDARYWINDOW_INTERFACE_H
