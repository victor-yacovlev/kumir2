#ifndef COREGUI_TABWIDGET_H
#define COREGUI_TABWIDGET_H

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

namespace CoreGUI {

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = 0);
    void disableTabs();
    QSize minimumSizeHint() const /*override*/;
protected:
    void paintEvent(QPaintEvent *);
};

} // namespace CoreGUI

#endif // COREGUI_TABWIDGET_H
