#ifndef BROWSER_COMPONENT_H
#define BROWSER_COMPONENT_H

#include <QtCore>
#include <QtGui>
#include <QtWebKit>

#include "interfaces/browserinterface.h"

namespace Browser {

namespace Ui {
    class Component;
}

class Component
        : public QWebView
{
    Q_OBJECT

public:
    explicit Component(class Plugin * plugin);
    inline void setManageableObjects(const QMap<QString, QObject*> &os) {m_manageableObjects = os; }
    QWebPage * createChildPage();
    ~Component();
    QList<QAction*> toolbarActions();
    QList<QMenu*> menuActions();

public slots:
    void go(const QUrl & url);
    void evaluateCommand(const QString & method, const QVariantList & arguments);
protected:
    void showEvent(QShowEvent *e);
private:

    QMap<QString, QObject*> m_manageableObjects;
    QAction * a_separator;
    QAction * a_goBack;
    QAction * a_reloadStop;

    QMenu * menu_edit;
    class Plugin * m_plugin;
    QWebFrame * frame_toPrint;

signals:
    void newWindowCreated(const Shared::BrowserComponent &);

private slots:
    void addJavaScriptObjects();
    void handleReloadStop();
    void handleLoadStarted();
    void handleLoadFinished();
    void handlePrintRequest(QWebFrame * frame);
    void handlePaintPrinterFrame(QPrinter * printer);
};


} // namespace Browser
#endif // BROWSER_COMPONENT_H
