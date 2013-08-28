#ifndef BROWSER_INTERFACE
#define BROWSER_INTERFACE

#include "browser_instanceinterface.h"

namespace Shared
{

class BrowserInterface
{
public:
    typedef QMap<QString, QObject*> ObjectsMap;
    virtual Browser::InstanceInterface * createBrowser(
            const QUrl & url = QUrl(),
            const ObjectsMap manageableObjects = ObjectsMap(),
            bool enableKeyboardNavigation = false) = 0;
};

}

Q_DECLARE_INTERFACE(Shared::BrowserInterface, "kumir2.browser")

#endif
