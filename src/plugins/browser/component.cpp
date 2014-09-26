#include "component.h"
#include "webpage.h"
#include "plugin.h"

namespace Browser {

static QString screenString(QString s)
{
    s.replace("\\", "\\\\");
    s.replace("\"", "\\\"");
    s.replace("\n", "\\n");
    return s;
}

void Component::evaluateCommand(const QString &method, const QVariantList &arguments)
{
    QString js = method + "(";
    for (int i=0; i<arguments.size(); i++) {
        QVariant arg = arguments[i];
        if (arg.type()==QVariant::Char || arg.type()==QVariant::String) {
            js += "\""+screenString(arg.toString())+"\"";
        }
        else if (arg.type()==QVariant::Bool) {
            js += arg.toBool() ? "true" : "false";
        }
        else {
            js += arg.toString();
        }
    }
    js += ")";
    page()->currentFrame()->evaluateJavaScript(js);
}

Component::Component(class Plugin * plugin) :
    QWebView()
{
    WebPage * pg = new WebPage();
    pg->setComponent(this);
    setPage(pg);
    page()->setNetworkAccessManager(plugin->networkAccessManager());
    connect(page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJavaScriptObjects()));
#ifdef QT_DEBUG
    page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
#endif
    if (qApp->applicationVersion().contains("/")) {  // Not tagged version
        page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    }
    connect(page()->mainFrame(), SIGNAL(titleChanged(QString)),
            this, SLOT(handleWebPageTitleChanged(QString)), Qt::DirectConnection);
}

void Component::handleWebPageTitleChanged(const QString &title)
{
    emit titleChangeRequest(title, this);
}

void Component::addJavaScriptObjects()
{
    foreach (const QString & name, manageableObjects_.keys()) {
        QObject * obj = manageableObjects_[name];
        page()->mainFrame()->addToJavaScriptWindowObject(name, obj);
    }
}

void Component::setContent(const QString &data)
{
    setHtml(data);
    pageAction(QWebPage::Back)->setEnabled(false);
}

void Component::go(const QUrl &url)
{
    setUrl(url);
    pageAction(QWebPage::Back)->setEnabled(false);
}


void Component::showEvent(QShowEvent *e)
{
    if (page()->mainFrame()->metaData().contains("refresh", "onshow")) {
        page()->mainFrame()->evaluateJavaScript("updateContents()");
    }
    QWidget::showEvent(e);
}

QString Component::title() const
{
    return page()->mainFrame()->title();
}

QUrl Component::currentLocation() const
{
    return url();
}

void Component::setTitleChangeHandler(const QObject *receiver, const char *method)
{
    connect(this, SIGNAL(titleChangeRequest(QString,const Shared::Browser::InstanceInterface*)),
            receiver, method, Qt::DirectConnection);
}

Component::~Component()
{

}


} // namespace Browser
