#ifndef BROWSER_WEBPAGE_H
#define BROWSER_WEBPAGE_H

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWebKitWidgets>
#else
#include <QtWebKit>
#endif

namespace Browser {

class WebPage : public QWebPage
{
    Q_OBJECT
public:
    WebPage();

    void setComponent(class Component * v);

protected:
    QObject * createPlugin(const QString &classid, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues);

private:
    class Component * component_;


};

} // namespace Browser

#endif // BROWSER_WEBPAGE_H
