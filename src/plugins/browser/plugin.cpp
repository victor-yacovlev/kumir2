#include "plugin.h"
#include "component.h"
#include "dir.h"
#include "localhostserver.h"
#include "networkaccessmanager.h"

namespace Browser {

Plugin::Plugin()
    : KPlugin()
    , localDirectoryContents_(new Dir(this))
    , networkAccessManager_(nullptr)
{
}

void Plugin::changeCurrentDirectory(const QString &path)
{
    localDirectoryContents_->m_dir = QDir(path);
}

QString Plugin::initialize(const QStringList &, const ExtensionSystem::CommandLine &)
{
    LocalhostServer * localhost = new LocalhostServer(QDir(qApp->property("sharePath").toString()+"/webapps/"), this);
    NetworkAccessManager * networkAccessManager = new NetworkAccessManager(this);
    networkAccessManager->setLocalhostServer(localhost);
    networkAccessManager_ = networkAccessManager;
    return "";
}

Shared::Browser::InstanceInterface * Plugin::createBrowser(const QUrl &url, const QMap<QString, QObject *> manageableObjects, bool enableKeyboardNavigation)
{
    Component * c = new Component(this);
    c->setMinimumSize(400, 200);
    c->setAcceptDrops(false);
    c->page()->settings()->setAttribute(QWebSettings::SpatialNavigationEnabled, enableKeyboardNavigation);
    QMap<QString,QObject*> objs = manageableObjects;
    objs["directory"] = localDirectoryContents_;
    objs["application"] = qApp;
    c->manageableObjects() = objs;

    if (!url.isEmpty())
        c->go(url);

    return c;
}

} // namespace Browser

Q_EXPORT_PLUGIN2(Browser, Browser::Plugin)
