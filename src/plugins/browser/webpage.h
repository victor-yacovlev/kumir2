#ifndef BROWSER_WEBPAGE_H
#define BROWSER_WEBPAGE_H

#include <QtCore>
#if QT_VERSION >= 0x050000

#if QT_VERSION >= 0x050400
#include <QtWebEngineWidgets>
#else
#include <QtWebKitWidgets>
#endif
#else
#include <QtWebKit>
#endif

namespace Browser {

class WebPage :
        public
        #if QT_VERSION >= 0x050400
        QWebEnginePage
        #else
        QWebPage
        #endif
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
