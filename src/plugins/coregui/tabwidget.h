#ifndef COREGUI_TABWIDGET_H
#define COREGUI_TABWIDGET_H

#include <QtCore>
#include <QtGui>

namespace CoreGUI {

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = 0);

signals:

public slots:
private slots:
    void handleCloseButtonPressed();
    void handleCurrentChanged(int);
private:

    void tabInserted(int index);
    void tabRemoved(int index);
    QAction *a_closeCurrentTab;

};

} // namespace CoreGUI

#endif // COREGUI_TABWIDGET_H
