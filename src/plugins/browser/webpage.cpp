#include "webpage.h"

namespace Browser {

WebPage::WebPage(QWidget *parent) :
    QWebPage(parent)
{

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
