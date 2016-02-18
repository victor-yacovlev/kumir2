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
#if QT_VERSION >= 0x050400
    page()->runJavaScript(js);
#else
    page()->currentFrame()->evaluateJavaScript(js);
#endif
}

Component::Component(class Plugin * plugin) :
    #if QT_VERSION >= 0x050400
    QWebEngineView()
    #else
    QWebView()
    #endif
{
    WebPage * pg = new WebPage();
    pg->setComponent(this);
    setPage(pg);
#if QT_VERSION < 0x050400
    page()->setNetworkAccessManager(plugin->networkAccessManager());
#else
    // TODO implement me!
#endif

#if QT_VERSION >= 0x050400
    connect(page(), SIGNAL(loadStarted()), this, SLOT(addJavaScriptObjects()));
#else
    connect(page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJavaScriptObjects()));
#endif

#if defined(QT_DEBUG) && (QT_VERSION < 0x050400)
    page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    if (qApp->applicationVersion().contains("/")) {  // Not tagged version
        page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    }
#endif

#if QT_VERSION >= 0x050400
    connect(page(), SIGNAL(titleChanged(QString)),
            this, SLOT(handleWebPageTitleChanged(QString)), Qt::DirectConnection);
#else
    connect(page()->mainFrame(), SIGNAL(titleChanged(QString)),
            this, SLOT(handleWebPageTitleChanged(QString)), Qt::DirectConnection);
#endif
}

void Component::handleWebPageTitleChanged(const QString &title)
{
    emit titleChangeRequest(title, this);
}

void Component::addJavaScriptObjects()
{
    foreach (const QString & name, manageableObjects_.keys()) {
        QObject * obj = manageableObjects_[name];
#if QT_VERSION >= 0x050400
        // TODO implement me!
#else
        page()->mainFrame()->addToJavaScriptWindowObject(name, obj);
#endif
    }
}

void Component::setContent(const QString &data)
{
    setHtml(data);
#if QT_VERSION >= 0x050400
        // TODO implement me!
#else
    pageAction(QWebPage::Back)->setEnabled(false);
#endif
}

void Component::go(const QUrl &url)
{
    setUrl(url);
#if QT_VERSION >= 0x050400
        // TODO implement me!
#else
    pageAction(QWebPage::Back)->setEnabled(false);
#endif
}


void Component::showEvent(QShowEvent *e)
{
#if QT_VERSION >= 0x050400
        // TODO implement me!
#else
    if (page()->mainFrame()->metaData().contains("refresh", "onshow")) {
        page()->mainFrame()->evaluateJavaScript("updateContents()");
    }
#endif
    QWidget::showEvent(e);
}

QString Component::title() const
{
#if QT_VERSION >= 0x050400
    return page()->title();
#else
    return page()->mainFrame()->title();
#endif
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
