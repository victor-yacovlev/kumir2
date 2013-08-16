#ifndef WIDGETS_SECONDARYWINDOW_IMPL_H
#define WIDGETS_SECONDARYWINDOW_IMPL_H

#include "extensionsystem/settings.h"

#include <QObject>
#include <QWidget>
#include <QAbstractButton>
#include <QStyle>
#include <QMainWindow>


#ifndef OS_SPECIFIC_DEFINES
#define OS_SPECIFIC_DEFINES

#if defined(Q_OS_MAC)
#define STAY_ON_TOP_FLAGS Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Window
#define REGULAR_FLAGS Qt::FramelessWindowHint|Qt::Window
#define PARENT mainWindow
#define D_PARENT mainWindow_
#elif defined(Q_OS_WIN32)
#define STAY_ON_TOP_FLAGS Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Window
#define REGULAR_FLAGS Qt::FramelessWindowHint|Qt::Window
#define PARENT 0
#define D_PARENT mainWindow_
#else
#define STAY_ON_TOP_FLAGS Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Window
#define REGULAR_FLAGS Qt::FramelessWindowHint|Qt::Window
#define PARENT mainWindow
#define D_PARENT mainWindow_
#endif
#endif


namespace Widgets {



class BorderWidget :
        public QWidget
{
public:
    explicit BorderWidget(
            QWidget *parent
            , QWidget *window
            , class SecondaryWindowImpl *q
            , char border
            , const QList<QPixmap*> &pixmaps
            , bool resizable
            );
    void switchPixmaps(const QList<QPixmap*> & pixmaps);
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

    bool mouseDragAction(const QPoint &offset);

    char border_;
    QList<QPixmap*> pixmaps_;
    bool resizableFlag_;
    bool movingFlag_;
    QPoint p_start;
    class SecondaryWindowImpl *pWindowImpl_;
    QWidget *window_;    
};

class SecondaryWindowButton :
        public QAbstractButton
{
public:
    explicit SecondaryWindowButton(QWidget *parent, bool checkable, const QString &name, QStyle::StandardPixmap role,
                          const QList<QPixmap*> &pixmaps, const QString &toolTip);
    inline bool isHovered() const { return hoveredFlag_; }
    inline bool isPressed() const { return pressedFlag_; }
    inline void forceUnhighlight() { pressedFlag_ = hoveredFlag_ = false; update(); }
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent (QPaintEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    bool hoveredFlag_;
    bool pressedFlag_;
    QList<QPixmap*> pixmaps_;
    QStyle::StandardPixmap role_;

};

class SecondaryWindowTitle :
        public QWidget
{
public:
    explicit SecondaryWindowTitle(QWidget *parent, const QString &style);
    void setStyle(const QString & style);
    void setText(const QString &v);
    inline QString text() const { return titleText_; }
protected:
    void paintEvent(QPaintEvent *e);
    QFont font_;
    QColor foregroundColor_;
    Qt::AlignmentFlag textAlignment_;
    int padding_;
    QString titleText_;

};



class SecondaryWindowImpl :
        public QObject
{
    Q_OBJECT
public:
    void init(QWidget *centralWidget,
              QSharedPointer<class DockWindowPlaceImpl> dockPlace,
              QMainWindow * mainWindow,
              ExtensionSystem::SettingsPtr settings,
              const QString &settingsKey,
              bool resizableX, bool resizableY);

    void setFloating(bool v);    

    QWidget* centralWidget_;
    QSharedPointer<class DockWindowPlaceImpl> dockPlace_;
    QMainWindow * mainWindow_;

    class BorderWidget* topBorder_;
    class BorderWidget* bottomBorder_;
    class BorderWidget* leftBorder_;
    class BorderWidget* rightBorder_;

    QAbstractButton* buttonStayOnTop_;
    QAbstractButton* buttonMinimize_;
    QAbstractButton* buttonToggleDocked_;
    QAbstractButton* buttonClose_;

    QAction* toggleVisibleAction_;

    QList<QPixmap*> topBorderPixmaps_;
    QList<QPixmap*> topDockedBorderPixmaps_;
    QList<QPixmap*> bottomBorderPixmaps_;
    QList<QPixmap*> leftBorderPixmaps_;
    QList<QPixmap*> rightBorderPixmaps_;

    QList<QPixmap*> stayOnTopButtonPixmaps_;
    QList<QPixmap*> minimizeButtonPixmaps_;
    QList<QPixmap*> toggleDockedButtonPixmaps_;
    QList<QPixmap*> closeButtonPixmaps_;

    QString windowTitleStylesheet_;
    QString dockedWindowTitleStylesheet_;

    class SecondaryWindow *pClass_;
    bool visibleFlag_;
    QString settingsKey_;
    ExtensionSystem::SettingsPtr settings_;

    void timerEvent(QTimerEvent *);
    bool floating_;
    QRect floatingRect_;

public slots:
    void handleResizeRequest(const QSize & size);
};


} // namespace Widgets

#endif // WIDGETS_SECONDARYWINDOW_IMPL_H
