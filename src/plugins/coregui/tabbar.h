#ifndef COREGUI_TABBAR_H
#define COREGUI_TABBAR_H

#include <QtCore>
#include <QtGui>

namespace CoreGUI {

class TabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit TabBar(QWidget *parent = 0);
protected:
    void tabInserted(int index);
    void tabRemoved(int index);
signals:
private slots:
    void handleChanged(int index);
public slots:

};

} // namespace CoreGUI

#endif // COREGUI_TABBAR_H
