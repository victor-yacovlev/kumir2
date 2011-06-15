#include "component.h"
#include "ui_component.h"

namespace Browser {

Component::Component() :
    QWidget(),
    ui(new Ui::Component)
{
    ui->setupUi(this);
    connect(ui->webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJavaScriptObjects()));
#ifdef QT_DEBUG
    ui->webView->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
#endif
}

QList<QAction*> Component::toolbarActions()
{
    QList<QAction*> result;
    result << ui->webView->pageAction(QWebPage::Back);
    QAction * separator = new QAction(this);
    separator->setSeparator(true);
    result << separator;
    result << ui->webView->pageAction(QWebPage::Copy);
    result.last()->setIcon(QIcon::fromTheme("edit-copy"));
    return result;
}

QList<QMenu*> Component::menuActions()
{
    QList<QMenu*> result;
    QMenu * edit = new QMenu(tr("Edit"));
    edit->addAction(ui->webView->pageAction(QWebPage::Back));
    edit->addAction(ui->webView->pageAction(QWebPage::Forward));
    edit->addAction(ui->webView->pageAction(QWebPage::Reload));
    edit->addAction(ui->webView->pageAction(QWebPage::Stop));
    edit->addSeparator();
    QAction * cut = ui->webView->pageAction(QWebPage::Cut);
    cut->setIcon(QIcon::fromTheme("edit-cut"));
    edit->addAction(cut);
    QAction * copy = ui->webView->pageAction(QWebPage::Copy);
    copy->setIcon(QIcon::fromTheme("edit-copy"));
    edit->addAction(copy);
    QAction * paste = ui->webView->pageAction(QWebPage::Paste);
    paste->setIcon(QIcon::fromTheme("edit-paste"));
    edit->addAction(paste);
    result << edit;
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
}

Component::~Component()
{
    delete ui;
}

} // namespace Browser
