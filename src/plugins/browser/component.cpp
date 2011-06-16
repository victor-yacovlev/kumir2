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
    ui->webView->pageAction(QWebPage::Back)->setText(tr("Go back"));
    ui->webView->pageAction(QWebPage::Forward)->setText(tr("Go forward"));
    ui->webView->pageAction(QWebPage::Stop)->setText(tr("Stop"));
    ui->webView->pageAction(QWebPage::Reload)->setText(tr("Reload"));
    ui->webView->pageAction(QWebPage::Cut)->setText(tr("Cut"));
    ui->webView->pageAction(QWebPage::Copy)->setText(tr("Copy"));
    ui->webView->pageAction(QWebPage::Paste)->setText(tr("Paste"));
    
    ui->webView->pageAction(QWebPage::Cut)->setIcon(QIcon::fromTheme("edit-cut"));
    ui->webView->pageAction(QWebPage::Copy)->setIcon(QIcon::fromTheme("edit-copy"));
    ui->webView->pageAction(QWebPage::Paste)->setIcon(QIcon::fromTheme("edit-paste"));
    
    a_separator = new QAction(this);
    a_separator->setSeparator(true);
    
    menu_edit = new QMenu(tr("Edit"));
    
    menu_edit->addAction(ui->webView->pageAction(QWebPage::Back));
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
    result << ui->webView->pageAction(QWebPage::Back);
    result << a_separator;
    result << ui->webView->pageAction(QWebPage::Copy);
    return result;
}

QList<QMenu*> Component::menuActions()
{
    QList<QMenu*> result;
    result << menu_edit;
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
