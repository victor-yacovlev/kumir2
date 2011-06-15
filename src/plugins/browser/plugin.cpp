#include "plugin.h"
#include "component.h"

namespace Browser {

Plugin::Plugin()
{
}

Shared::BrowserComponent Plugin::createBrowser(const QUrl &url, const QMap<QString, QObject *> manageableObjects)
{
    Component * c = new Component();
    c->setManageableObjects(manageableObjects);
    c->go(url);
    Shared::BrowserComponent result;
    result.widget = c;
    result.toolbarActions = c->toolbarActions();
    result.menus = c->menuActions();
    return result;
}

} // namespace Browser

Q_EXPORT_PLUGIN2(Browser, Browser::Plugin)
