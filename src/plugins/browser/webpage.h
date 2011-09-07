#ifndef BROWSER_WEBPAGE_H
#define BROWSER_WEBPAGE_H

#include <QWebPage>

namespace Browser {

class WebPage : public QWebPage
{
    Q_OBJECT
public:
    WebPage();

    void setComponent(class Component * v);

protected:
    QObject * createPlugin(const QString &classid, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues);
    QWebPage * createWindow(WebWindowType type);

private:
    class Component * m_component;


};

} // namespace Browser

#endif // BROWSER_WEBPAGE_H
