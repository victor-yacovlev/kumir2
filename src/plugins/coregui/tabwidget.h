#ifndef COREGUI_TABWIDGET_H
#define COREGUI_TABWIDGET_H

#include <QTabWidget>

namespace CoreGUI {

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = 0);

signals:

public slots:
private:
    void tabInserted(int index);
    void tabRemoved(int index);

};

} // namespace CoreGUI

#endif // COREGUI_TABWIDGET_H
