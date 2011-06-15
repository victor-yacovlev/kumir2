#ifndef BROWSER_INTERFACE
#define BROWSER_INTERFACE

#include <QtCore>
#include <QtGui>

namespace Shared
{

struct BrowserComponent {
    QWidget * widget;
    QList<QAction*> toolbarActions;
    QList<QMenu*> menus;
};

class BrowserInterface
{
public:
    virtual BrowserComponent createBrowser(const QUrl & url, const QMap<QString, QObject*> manageableObjects) = 0;
signals:
    void openNewDocumentRequest(const QUrl &url);
};

}

Q_DECLARE_INTERFACE(Shared::BrowserInterface, "kumir2.browser")

#endif
