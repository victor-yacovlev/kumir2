#ifndef WIDGETS_ACTIONPROXY_H
#define WIDGETS_ACTIONPROXY_H

#include <QAction>

#ifdef WIDGETS_LIBRARY
#define WIDGETS_EXPORT Q_DECL_EXPORT
#else
#define WIDGETS_EXPORT Q_DECL_IMPORT
#endif

namespace Widgets {

class WIDGETS_EXPORT ActionProxy : public QAction
{
    Q_OBJECT
public:
    explicit ActionProxy(QAction * target, QObject *parent = 0);

protected Q_SLOTS:
    void handleTargetChanged();

private:
    QAction * target_;

};

} // namespace Widgets

#endif // WIDGETS_ACTIONPROXY_H
