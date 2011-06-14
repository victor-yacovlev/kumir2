#include "component.h"
#include "ui_component.h"

namespace Browser {

Component::Component() :
    ExtensionSystem::VisualComponent(),
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

QList<ExtensionSystem::MenuActionsGroup> Component::menuActions()
{
    QAction * separator = new QAction(this);
    separator->setSeparator(true);
    QList<ExtensionSystem::MenuActionsGroup> result;
    ExtensionSystem::MenuActionsGroup group;
    group.menuText = tr("Edit");
    group.actions << ui->webView->pageAction(QWebPage::Back);
    group.actions << ui->webView->pageAction(QWebPage::Forward);
    group.actions << ui->webView->pageAction(QWebPage::Reload);
    group.actions << ui->webView->pageAction(QWebPage::Stop);
    group.actions << separator;
    group.actions << ui->webView->pageAction(QWebPage::Cut);
    group.actions.last()->setIcon(QIcon::fromTheme("edit-cut"));
    group.actions << ui->webView->pageAction(QWebPage::Copy);
    group.actions << ui->webView->pageAction(QWebPage::Paste);
    group.actions.last()->setIcon(QIcon::fromTheme("edit-paste"));
    result << group;
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
