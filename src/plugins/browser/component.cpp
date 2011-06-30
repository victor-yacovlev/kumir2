#include "component.h"
#include "ui_component.h"
#include "webpage.h"

namespace Browser {

Component::Component() :
    QWidget(),
    ui(new Ui::Component)
{
    ui->setupUi(this);
    ui->webView->setPage(new WebPage);
    connect(ui->webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJavaScriptObjects()));
#ifdef QT_DEBUG
    ui->webView->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
#endif
    ui->webView->pageAction(QWebPage::Back)->setText(tr("Go back"));
    ui->webView->pageAction(QWebPage::Forward)->setText(tr("Go forward"));
    ui->webView->pageAction(QWebPage::Stop)->setText(tr("Stop"));
    ui->webView->pageAction(QWebPage::Reload)->setText(tr("Reload"));
    ui->webView->pageAction(QWebPage::Cut)->setText(tr("Cut"));
    ui->webView->pageAction(QWebPage::Copy)->setText(tr("Copy"));
    ui->webView->pageAction(QWebPage::Paste)->setText(tr("Paste"));
    
    ui->webView->pageAction(QWebPage::Cut)->setIcon(QIcon::fromTheme("edit-cut", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-cut.png")));
    ui->webView->pageAction(QWebPage::Copy)->setIcon(QIcon::fromTheme("edit-copy", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-copy.png")));
    ui->webView->pageAction(QWebPage::Paste)->setIcon(QIcon::fromTheme("edit-paste", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-paste.png")));

    connect(ui->webView, SIGNAL(titleChanged(QString)), this, SIGNAL(titleChanged(QString)));
    connect(ui->webView->page(), SIGNAL(loadStarted()), this, SLOT(handleLoadStarted()));
    connect(ui->webView->page(), SIGNAL(loadFinished(bool)), this, SLOT(handleLoadFinished()));

    a_goBack = new QAction(tr("Go back"), this);
    a_goBack->setIcon(ui->webView->pageAction(QWebPage::Back)->icon());
    connect(a_goBack, SIGNAL(triggered()), ui->webView->pageAction(QWebPage::Back), SLOT(trigger()));
    a_goBack->setEnabled(false);

    a_reloadStop = new QAction(this);
    connect(a_reloadStop, SIGNAL(triggered()), this, SLOT(handleReloadStop()));
    
    a_separator = new QAction(this);
    a_separator->setSeparator(true);
    
    menu_edit = new QMenu(tr("Edit"));
    
    menu_edit->addAction(a_goBack);
    menu_edit->addAction(ui->webView->pageAction(QWebPage::Forward));
    menu_edit->addAction(ui->webView->pageAction(QWebPage::Reload));
    menu_edit->addAction(ui->webView->pageAction(QWebPage::Stop));
    menu_edit->addSeparator();
    menu_edit->addAction(ui->webView->pageAction(QWebPage::Cut));
    menu_edit->addAction(ui->webView->pageAction(QWebPage::Copy));
    menu_edit->addAction(ui->webView->pageAction(QWebPage::Paste));
}

QList<QAction*> Component::toolbarActions()
{
    QList<QAction*> result;
    result << a_goBack;
    result << ui->webView->pageAction(QWebPage::Forward);
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
        ui->webView->page()->mainFrame()->addToJavaScriptWindowObject(name, obj);
    }
}

void Component::go(const QUrl &url)
{
    ui->webView->setUrl(url);
//    ui->webView->history()->clear();
    ui->webView->pageAction(QWebPage::Back)->setEnabled(false);
//    for (int i=0; i<ui->webView->history()->items().count(); i++) {
//        QWebHistoryItem item = ui->webView->history()->items()[i];
//        qDebug() << item.url();
//    }
}


void Component::showEvent(QShowEvent *e)
{
    if (ui->webView->page()->mainFrame()->metaData().contains("refresh", "onshow")) {
        ui->webView->page()->mainFrame()->evaluateJavaScript("updateContents()");
    }
    QWidget::showEvent(e);
}

void Component::handleLoadStarted()
{
    a_reloadStop->setText(tr("Stop"));
    a_reloadStop->setIcon(ui->webView->pageAction(QWebPage::Stop)->icon());
}

void Component::handleLoadFinished()
{
    int hc = ui->webView->history()->count();
//    qDebug() << hc;
    const QString title = ui->webView->title();
    emit titleChanged(title);
    a_goBack->setEnabled(hc>2);
    a_reloadStop->setText(tr("Reload"));
    a_reloadStop->setIcon(ui->webView->pageAction(QWebPage::Reload)->icon());
}

void Component::handleReloadStop()
{
    if (a_reloadStop->text()==tr("Reload")) {
        ui->webView->pageAction(QWebPage::Reload)->trigger();
    }
    else {
        ui->webView->pageAction(QWebPage::Stop)->trigger();
    }
}

Component::~Component()
{
    delete ui;
}

} // namespace Browser
