#include "webpage.h"
#include "component.h"

namespace Browser {

WebPage::WebPage()
    : QWebPage()
{
    component_ = 0;
}

void WebPage::setComponent(Component *v)
{
    component_ = v;
}


QObject * WebPage::createPlugin(const QString &classid, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues)
{
    Q_UNUSED(classid);
    Q_UNUSED(url);
    QWidget * result = 0;
    if (result) {
        for (int i=0; i<paramNames.size(); i++) {
            if (paramNames[i]=="stylesheet") {
                result->setStyleSheet(paramValues[i]);
            }
        }
    }
    return result;
}


} // namespace Browser
