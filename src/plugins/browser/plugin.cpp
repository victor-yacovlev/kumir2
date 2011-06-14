#include "plugin.h"
#include "component.h"

namespace Browser {

Plugin::Plugin()
{
}

ExtensionSystem::VisualComponent * Plugin::createBrowser(const QUrl &url, const QMap<QString, QObject *> manageableObjects)
{
    Component * c = new Component();
    c->setManageableObjects(manageableObjects);
    c->go(url);
    return c;
}

} // namespace Browser

Q_EXPORT_PLUGIN2(Browser, Browser::Plugin)
