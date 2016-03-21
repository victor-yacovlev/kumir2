#ifndef COREGUI_TABBAR_H
#define COREGUI_TABBAR_H

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

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
    void switchToTab();
public slots:
private:
    QVector<QIcon> v_activeIcons;
    QVector<QIcon> v_normalIcons;
    QVector<QAction*> v_actions;

};

} // namespace CoreGUI

#endif // COREGUI_TABBAR_H
