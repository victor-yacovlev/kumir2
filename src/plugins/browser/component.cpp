#include "component.h"
#include "webpage.h"
#include "plugin.h"
#include "../../VERSION.h"

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
    m_plugin = plugin;
    WebPage * pg = new WebPage();
    pg->setComponent(this);
    setPage(pg);
    page()->setNetworkAccessManager(plugin->networkAccessManager());
    connect(page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJavaScriptObjects()));
#ifdef QT_DEBUG
    page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
#endif
#if VERSION_ALPHA > 0
    page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
#endif
    pageAction(QWebPage::Back)->setText(tr("Go back"));
    pageAction(QWebPage::Forward)->setText(tr("Go forward"));
    pageAction(QWebPage::Stop)->setText(tr("Stop"));
    pageAction(QWebPage::Reload)->setText(tr("Reload"));
    pageAction(QWebPage::Cut)->setText(tr("Cut"));
    pageAction(QWebPage::Copy)->setText(tr("Copy"));
    pageAction(QWebPage::Paste)->setText(tr("Paste"));

    pageAction(QWebPage::Cut)->setIcon(QIcon::fromTheme("edit-cut", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-cut.png")));
    pageAction(QWebPage::Copy)->setIcon(QIcon::fromTheme("edit-copy", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-copy.png")));
    pageAction(QWebPage::Paste)->setIcon(QIcon::fromTheme("edit-paste", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-paste.png")));

    connect(page(), SIGNAL(loadStarted()), this, SLOT(handleLoadStarted()));
    connect(page(), SIGNAL(loadFinished(bool)), this, SLOT(handleLoadFinished()));
    connect(page(), SIGNAL(printRequested(QWebFrame*)), this, SLOT(handlePrintRequest(QWebFrame*)));

    a_goBack = new QAction(tr("Go back"), this);
    a_goBack->setIcon(pageAction(QWebPage::Back)->icon());
    connect(a_goBack, SIGNAL(triggered()), pageAction(QWebPage::Back), SLOT(trigger()));
    a_goBack->setEnabled(false);

    a_reloadStop = new QAction(this);
    connect(a_reloadStop, SIGNAL(triggered()), this, SLOT(handleReloadStop()));
    
    a_separator = new QAction(this);
    a_separator->setSeparator(true);
    
    menu_edit = new QMenu(tr("Edit"));
    
    menu_edit->addAction(a_goBack);
    menu_edit->addAction(pageAction(QWebPage::Forward));
    menu_edit->addAction(pageAction(QWebPage::Reload));
    menu_edit->addAction(pageAction(QWebPage::Stop));
    menu_edit->addSeparator();
    menu_edit->addAction(pageAction(QWebPage::Cut));
    menu_edit->addAction(pageAction(QWebPage::Copy));
    menu_edit->addAction(pageAction(QWebPage::Paste));

    frame_toPrint = 0;

}

QList<QAction*> Component::toolbarActions()
{
    QList<QAction*> result;
    result << a_goBack;
    result << pageAction(QWebPage::Forward);
    result << a_reloadStop;
    return result;
}

QList<QMenu*> Component::menuActions()
{
    QList<QMenu*> result;
//    result << menu_edit;
    return result;
}

void Component::addJavaScriptObjects()
{
    foreach (const QString & name, m_manageableObjects.keys()) {
        QObject * obj = m_manageableObjects[name];
        page()->mainFrame()->addToJavaScriptWindowObject(name, obj);
    }
}

void Component::go(const QUrl &url)
{
    setUrl(url);
//    ui->webView->history()->clear();
    pageAction(QWebPage::Back)->setEnabled(false);
//    for (int i=0; i<ui->webView->history()->items().count(); i++) {
//        QWebHistoryItem item = ui->webView->history()->items()[i];
//        qDebug() << item.url();
//    }
}


void Component::showEvent(QShowEvent *e)
{
    if (page()->mainFrame()->metaData().contains("refresh", "onshow")) {
        page()->mainFrame()->evaluateJavaScript("updateContents()");
    }
    QWidget::showEvent(e);
}

void Component::handleLoadStarted()
{
    a_reloadStop->setText(tr("Stop"));
    a_reloadStop->setIcon(pageAction(QWebPage::Stop)->icon());
}

void Component::handleLoadFinished()
{
    int hc = history()->count();
    a_goBack->setEnabled(hc>2);
    a_reloadStop->setText(tr("Reload"));
    a_reloadStop->setIcon(pageAction(QWebPage::Reload)->icon());
}

void Component::handleReloadStop()
{
    if (a_reloadStop->text()==tr("Reload")) {
        pageAction(QWebPage::Reload)->trigger();
    }
    else {
        pageAction(QWebPage::Stop)->trigger();
    }
}

void Component::handlePrintRequest(QWebFrame *frame)
{
#if QT_VERSION >= 0x040800
    frame_toPrint = frame;
    QPrintPreviewDialog * dialog = new QPrintPreviewDialog(this);
    connect(dialog, SIGNAL(paintRequested(QPrinter*)),
            this, SLOT(handlePaintPrinterFrame(QPrinter*)));
    dialog->exec();
    dialog->deleteLater();
    frame_toPrint = 0;
#else
    QPrintDialog * dialog = new QPrintDialog(this);
    if (dialog->exec()==QDialog::Accepted) {
        QPrinter * printer = dialog->printer();
        if (printer) {
            frame->print(printer);
        }
    }
    dialog->deleteLater();
#endif
}

void Component::handlePaintPrinterFrame(QPrinter *printer)
{
    if (printer && frame_toPrint) {
        frame_toPrint->print(printer);
    }
}

Component::~Component()
{

}

QWebPage * Component::createChildPage()
{
    Shared::BrowserComponent bc = m_plugin->createBrowser(QUrl(), m_manageableObjects);
    emit newWindowCreated(bc);
    Component * cw = qobject_cast<Component*>(bc.widget);
    return cw->page();
}

} // namespace Browser
