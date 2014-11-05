#ifndef STARTPAGE_WIDGET_INTERFACE_H
#define STARTPAGE_WIDGET_INTERFACE_H

#include <QtGlobal>
#include <QWidget>
#include <QList>
#include <QAction>
#include <QMenu>

namespace Shared {

class StartpageWidgetInterface
{
public:
    virtual QWidget* startPageWidget() = 0;
    inline virtual QMenu * editMenuForStartPage() { return nullptr; }
    inline virtual QList<QAction*> startPageActions() { return QList<QAction*>(); }
    virtual QString startPageTitle() const = 0;
    virtual void setStartPageTitleChangeHandler(const QObject * receiver,
                                       const char * method /*(QString title, const QObject * sender)*/
                                       ) = 0;
};

}

Q_DECLARE_INTERFACE(Shared::StartpageWidgetInterface, "kumir2.startpage")

#endif // STARTPAGE_WIDGET_INTERFACE_H
