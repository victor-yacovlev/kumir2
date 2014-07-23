#ifndef BROWSER_PLUGIN_H
#define BROWSER_PLUGIN_H

#include "extensionsystem/kplugin.h"
#include "interfaces/browserinterface.h"

#include <QtNetwork>

namespace Browser {

class Plugin
        : public ExtensionSystem::KPlugin
        , public Shared::BrowserInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "kumir2.Browser" FILE "")
#endif
    Q_INTERFACES(Shared::BrowserInterface)
public:
    Plugin();
    Shared::Browser::InstanceInterface * createBrowser(const QUrl & url = QUrl(),
					   const ObjectsMap manageableObjects = ObjectsMap(),
                                           bool enableKeyboardNavigation = false);
    inline QNetworkAccessManager * networkAccessManager() { return networkAccessManager_; }
    inline void updateSettings(const QStringList &) {}    

protected:
    QString initialize(const QStringList &, const ExtensionSystem::CommandLine &);
    void changeCurrentDirectory(const QString &path);
private:
    class Dir * localDirectoryContents_;
    QNetworkAccessManager * networkAccessManager_;
};

} // namespace Browser

#endif // BROWSER_PLUGIN_H
