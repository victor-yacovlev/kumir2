#ifndef COREGUI_DOCKWIDGET_H
#define COREGUI_DOCKWIDGET_H

#include <QtCore>
#include <QtGui>

namespace CoreGUI {

class DockWidget
        : public QDockWidget
{
    Q_OBJECT
public:
    explicit DockWidget(const QString & title, QWidget * parent = 0);
    inline Qt::DockWidgetArea dockLocation() const { return m_location; }
    void saveState(QSettings * s) const;
    void restoreState(QSettings * s, QMainWindow * w, bool terminalWindow);
private slots:
    void handleDockLocationChanged(Qt::DockWidgetArea area);
private:
    Qt::DockWidgetArea m_location;
};

} // namespace CoreGUI

#endif // COREGUI_DOCKWIDGET_H
