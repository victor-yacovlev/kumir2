#ifndef BROWSER_COMPONENT_H
#define BROWSER_COMPONENT_H

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#include <QtWebKitWidgets>
#else
#include <QtGui>
#include <QtWebKit>
#endif

#include "interfaces/browserinterface.h"
#include "interfaces/browser_instanceinterface.h"

namespace Browser {

namespace Ui {
    class Component;
}

class Component
        : public QWebView
        , public Shared::Browser::InstanceInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::Browser::InstanceInterface)
public:
    explicit Component(class Plugin * plugin);
    ~Component();
    inline QWidget * widget() { return this; }
    QString title() const;
    QUrl currentLocation() const;
    void setTitleChangeHandler(const QObject *receiver, const char *method);
    inline QMap<QString,QObject*> & manageableObjects() {
        return manageableObjects_;
    }

public slots:
    void go(const QUrl & url);
    void setContent(const QString & data);
    void evaluateCommand(const QString & method, const QVariantList & arguments);
protected:
    void showEvent(QShowEvent *e);
private:
    QMap<QString, QObject*> manageableObjects_;

private slots:
    void addJavaScriptObjects();
    void handleWebPageTitleChanged(const QString & title);

/*private*/ signals:
    void titleChangeRequest(const QString & title, const Shared::Browser::InstanceInterface * instance);
};


} // namespace Browser
#endif // BROWSER_COMPONENT_H
