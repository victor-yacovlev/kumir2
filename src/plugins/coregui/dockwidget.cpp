#include "dockwidget.h"
#include "plugin.h"

namespace CoreGUI {

DockWidget::DockWidget(const QString & title, QWidget * parent)
    : QDockWidget(title, parent)
{
    m_location = Qt::NoDockWidgetArea;
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(handleDockLocationChanged(Qt::DockWidgetArea)));
}

void DockWidget::handleDockLocationChanged(Qt::DockWidgetArea area)
{
    m_location = area;
}

void DockWidget::saveState(QSettings *s) const
{
    const QString t = windowTitle();
    s->setValue(Plugin::DockVisibleKey+"/"+t, isVisible());
    s->setValue(Plugin::DockSideKey+"/"+t, quint32(dockLocation()));
    s->setValue(Plugin::DockFloatingKey+"/"+t, isFloating());
    QRect r(pos(), size());
    s->setValue(Plugin::DockGeometryKey+"/"+t, r);
}

void DockWidget::restoreState(QSettings *s, QMainWindow *w, bool terminalWindow)
{
    const QString t = windowTitle();

    const bool DefaultFloating = ! terminalWindow;
    const Qt::DockWidgetArea DefaultArea = terminalWindow? Qt::BottomDockWidgetArea : Qt::NoDockWidgetArea;
    const bool DefaultVisible = false;

    bool visible = s->value(Plugin::DockVisibleKey+"/"+t, DefaultVisible).toBool();
    bool floating = s->value(Plugin::DockFloatingKey+"/"+t, DefaultFloating).toBool();
    Qt::DockWidgetArea area = Qt::DockWidgetArea(s->value(Plugin::DockSideKey+"/"+t, quint32(DefaultArea)).toUInt());
    QRect r = s->value(Plugin::DockGeometryKey+"/"+t, QRect(0,0,0,0)).toRect();

    QSize sz = r.size();
    QPoint pnt = r.topLeft();

    setFloating(floating);
    if (floating) {
    }
    else {
        w->addDockWidget(area, this);
    }
    move(pnt);
    if (sz.width()>0 && sz.height()>0)
        resize(sz);
    setVisible(visible);

}

} // namespace CoreGUI
