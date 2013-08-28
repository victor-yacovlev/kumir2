#ifndef BROWSER_INTERFACE
#define BROWSER_INTERFACE

#include "browser_instanceinterface.h"

namespace Shared
{

class BrowserInterface
{
public:
    virtual Browser::InstanceInterface * createBrowser(
            const QUrl & url = QUrl(),
            const QMap<QString, QObject*> manageableObjects = QMap<QString,QObject*>(),
            bool enableKeyboardNavigation = false) = 0;
};

}

Q_DECLARE_INTERFACE(Shared::BrowserInterface, "kumir2.browser")

#endif
