#ifndef WIDGETS_SECONDARYWINDOW_GENERIC_H
#define WIDGETS_SECONDARYWINDOW_GENERIC_H

#include "secondarywindow_interface.h"

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QIcon>

namespace Widgets {

class SecondaryWindowGenericImplementation
        : public QWidget
        , public SecondaryWindowImplementationInterface
{
    friend class SecondaryWindow;
    Q_OBJECT
public:    
    QWidget * toWidget();
    bool hasWidgetOwnership() const;
    virtual void getWidgetOwnership(QWidget * w);
    virtual QWidget* releaseWidgetOwnership();
    virtual void setPairedContainer(SecondaryWindowImplementationInterface *pairedContainer);

    virtual void setResizeble(bool v);
    virtual bool isResizable() const;

    virtual bool isDockable() const;

    virtual void setTitle(const QString &title);
    virtual QString title() const;

    virtual QSize minimumSizeHint() const;

signals:
    
public slots:
    virtual void deactivate();
    virtual void activate(const QPoint &ps, const QSize & sz);    
    void handleResizeRequest(const QSize & sz);

private /*event handlers*/:
    bool event(QEvent * evt);

protected /*methods*/:
    explicit SecondaryWindowGenericImplementation(QWidget * parent);
    virtual void setupWidgetsAppearance();
    virtual void setupWindow();
    virtual void paintWindowFrame();
    virtual void notifyOnDocked();

    enum HitTestResult {
        // Encoding: 0x{ROW}{COL} (from 1)
        Nothing = 0x00,
        Title = 0x11,
        LeftBorder = 0x21,
        RightBorder = 0x23,
        BottomBorder = 0x32,
        LeftCorner = 0x31,
        RightCorner = 0x33
    };

    virtual HitTestResult hitTest(const QPoint & mousePosition) const;
    void setProperCursor(HitTestResult hit);
    void processMoveWindow(const QPoint & mousePos);
    void processResizeWindow(const QPoint & mousePos);

protected slots:
    virtual void closeWindow();
    virtual void minimizeWindow();
    virtual void toggleStayOnTop(bool v);
    virtual void toggleDockWindow();

private /*methods*/:
    void createWidgets();
    void createLayout();

protected /*fields*/:
    QPushButton * btnClose_;
    QPushButton * btnMinimize_;
    QPushButton * btnDock_;
    QPushButton * btnStayOnTop_;
    QLabel * windowTitle_;
    QWidget * titleBox_;
    QWidget * centralWidget_;
    SecondaryWindowImplementationInterface * pairedContainer_;
    QIcon stayOnTopNotToggled_;
    QIcon stayOnTopToggled_;
    QPoint mousePressPoint_;
    HitTestResult mousePressHit_;

};

} // namespace Widgets

#endif // WIDGETS_SECONDARYWINDOW_GENERIC_H
