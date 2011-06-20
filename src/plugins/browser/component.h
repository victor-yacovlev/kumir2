#ifndef BROWSER_COMPONENT_H
#define BROWSER_COMPONENT_H

#include <QtCore>
#include <QtGui>
#include <QtWebKit>

namespace Browser {

namespace Ui {
    class Component;
}

class Component
        : public QWidget
{
    Q_OBJECT

public:
    explicit Component();
    inline void setManageableObjects(const QMap<QString, QObject*> &os) {m_manageableObjects = os; }
    ~Component();
    QList<QAction*> toolbarActions();
    QList<QMenu*> menuActions();
public slots:
    void go(const QUrl & url);
protected:
    void showEvent(QShowEvent *e);
private:
    Ui::Component *ui;
    QMap<QString, QObject*> m_manageableObjects;
    QAction * a_separator;
    QAction * a_goBack;
    QAction * a_reloadStop;

    QMenu * menu_edit;
signals:
    void titleChanged(const QString & title);

private slots:
    void addJavaScriptObjects();
    void handleReloadStop();
    void handleLoadStarted();
    void handleLoadFinished();
};


} // namespace Browser
#endif // BROWSER_COMPONENT_H
