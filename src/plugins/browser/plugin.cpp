#include "plugin.h"
#include "component.h"
#include "dir.h"
#include "localhostserver.h"
#include "networkaccessmanager.h"

namespace Browser {

Plugin::Plugin()
{
    m_directory = new Dir(this);
    m_networkAccessManager = 0;
}

Plugin::~Plugin()
{
    m_directory->deleteLater();
}

void Plugin::changeCurrentDirectory(const QString &path)
{
    m_directory->m_dir = QDir(path);
}

QString Plugin::initialize(const QStringList &)
{
    qRegisterMetaType<Shared::BrowserComponent>("BrowserComponent");
    LocalhostServer * localhost = new LocalhostServer(QDir(qApp->property("sharePath").toString()+"/webapps/"), this);
    NetworkAccessManager * networkAccessManager = new NetworkAccessManager(this);
    networkAccessManager->setLocalhostServer(localhost);
    m_networkAccessManager = networkAccessManager;
    return "";
}

Shared::BrowserComponent Plugin::createBrowser(const QUrl &url, const QMap<QString, QObject *> manageableObjects)
{
    qDebug() << "Creating browser for " << url.toString();
    Component * c = new Component(this);
    qDebug() << "Component created";
    QMap<QString,QObject*> objs = manageableObjects;
    objs["directory"] = m_directory;
    objs["application"] = qApp;
    c->setManageableObjects(objs);
    qDebug() << "Manageable objects are set";
    if (!url.isEmpty())
        c->go(url);
    qDebug() << "URL set";
    Shared::BrowserComponent result;
    result.widget = c;
    result.toolbarActions = c->toolbarActions();
    result.menus = c->menuActions();
    qDebug() << "Creating browser done.";
    return result;
}

} // namespace Browser

Q_EXPORT_PLUGIN2(Browser, Browser::Plugin)
