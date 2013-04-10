#ifndef SECONDARY_WINDOW_H
#define SECONDARY_WINDOW_H


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
                             QSettings * settings,
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
    void setSettingsObject(QSettings * settings);
    void restoreState();
    void saveState();
    void paintEvent(QPaintEvent *e);
public:
    QSize minimumSizeHint() const;
protected:
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *obj, QEvent *evt);
protected slots:
    void checkForPlaceVisible(bool show);
private:
    class SecondaryWindowPrivate *pImpl_;
};

}

#endif // CUSTOMWINDOW_H
