#ifndef BROWSER_INTERFACE
#define BROWSER_INTERFACE

#include "extensionsystem/visualcomponent.h"

namespace Shared
{

class BrowserInterface
{
public:
    virtual ExtensionSystem::VisualComponent * createBrowser(const QUrl & url, const QMap<QString, QObject*> manageableObjects) = 0;
signals:
    void openNewDocumentRequest(const QUrl &url);
};

}

Q_DECLARE_INTERFACE(Shared::BrowserInterface, "kumir2.browser")

#endif
