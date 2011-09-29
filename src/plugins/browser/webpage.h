#ifndef BROWSER_WEBPAGE_H
#define BROWSER_WEBPAGE_H

#include <QtWebKit/QWebView>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebSettings>

namespace Browser {

class WebPage : public QWebPage
{
    Q_OBJECT
public:
    WebPage();

    void setComponent(class Component * v);

protected:
    bool event(QEvent *e);
    QObject * createPlugin(const QString &classid, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues);
    QWebPage * createWindow(WebWindowType type);

private:
    class Component * m_component;


};

} // namespace Browser

#endif // BROWSER_WEBPAGE_H
