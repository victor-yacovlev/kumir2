#ifndef SECONDARY_WINDOW_H
#define SECONDARY_WINDOW_H

#include "extensionsystem/settings.h"

#include <QtCore>
#include <QtGui>

#ifdef WIDGETS_LIBRARY
#define WIDGETS_EXPORT Q_DECL_EXPORT
#else
#define WIDGETS_EXPORT Q_DECL_IMPORT
#endif

namespace Widgets {


class WIDGETS_EXPORT SecondaryWindow :
        public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString windowTitle READ windowTitle WRITE setWindowTitle)
    Q_PROPERTY(bool stayOnTop READ isStayOnTop WRITE setStayOnTop)
public:
    explicit SecondaryWindow(QWidget *centralComponent,
                          QWidget * dockPlace,
                             QMainWindow * mainWindow,
                             ExtensionSystem::SettingsPtr settings,
                          const QString &settingsKey,
                          bool resizableX = true, bool resizableY = true);
public slots:
    void setWindowTitle(const QString &);
    void close();
    void setVisible(bool visible);
    void showMinimized();
    bool isFloating() const;
    void toggleDocked();
    QString windowTitle();
    void setStayOnTop(bool v);
    bool isStayOnTop();
    QAction * toggleViewAction() const;
    void setSettingsObject(ExtensionSystem::SettingsPtr settings);
    void restoreState();
    void saveState();
    void paintEvent(QPaintEvent *e);
signals:
    void docked(QWidget * place, const QString & title);
    void undocked(QWidget * place);
public:
    QSize minimumSizeHint() const;
protected:
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *obj, QEvent *evt);
protected slots:
    void checkForPlaceVisible();
    void activate();
private:
    class SecondaryWindowPrivate *pImpl_;
};


class SecondaryWindowPrivate :
        public QObject
{
    Q_OBJECT
public:
    void init(QWidget *centralWidget,
              QWidget *dockPlace,
              QMainWindow * mainWindow,
              ExtensionSystem::SettingsPtr settings,
              const QString &settingsKey,
              bool resizableX, bool resizableY);

    QWidget* centralWidget_;
    QWidget* dockPlace_;
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

    SecondaryWindow *pClass_;
    bool visibleFlag_;
    QString settingsKey_;
    ExtensionSystem::SettingsPtr settings_;

    void timerEvent(QTimerEvent *);
};

}

#endif // CUSTOMWINDOW_H
