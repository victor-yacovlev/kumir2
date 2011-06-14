#ifndef BROWSER_COMPONENT_H
#define BROWSER_COMPONENT_H

#include <QtCore>
#include <QtGui>
#include <QtWebKit>

#include "extensionsystem/visualcomponent.h"

namespace Browser {

namespace Ui {
    class Component;
}

class Component
        : public ExtensionSystem::VisualComponent
{
    Q_OBJECT

public:
    explicit Component();
    inline void setManageableObjects(const QMap<QString, QObject*> &os) {m_manageableObjects = os; }
    ~Component();
    QList<QAction*> toolbarActions();
    QList<ExtensionSystem::MenuActionsGroup> menuActions();
public slots:
    void go(const QUrl & url);
private:
    Ui::Component *ui;
    QMap<QString, QObject*> m_manageableObjects;

private slots:
    void addJavaScriptObjects();
};


} // namespace Browser
#endif // BROWSER_COMPONENT_H
