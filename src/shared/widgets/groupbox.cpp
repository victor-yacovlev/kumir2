#include "groupbox.h"

namespace Widgets {

GroupBox::GroupBox(QWidget *parent): QGroupBox(parent), b_collapsed(false) {}
GroupBox::GroupBox(const QString &title, QWidget *parent): QGroupBox(title, parent), b_collapsed(false) {}

void GroupBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button()==Qt::LeftButton)
    {
        pnt_clickPos = event->pos();
    }
    QGroupBox::mousePressEvent(event);
}

void GroupBox::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button()==Qt::LeftButton && event->pos().y()<24) {
        int dx = abs(event->pos().x()-pnt_clickPos.x());
        int dy = abs(event->pos().y()-pnt_clickPos.y());
        int delta = dx>dy? dx : dy;
        if (delta<=QApplication::startDragDistance()) {
            setCollapsed(!isCollapsed());
            event->accept();
            return;
        }
    }
    QGroupBox::mouseReleaseEvent(event);
}

void GroupBox::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    QRect lineRect(2, 2, width()-4, height()-4);
    p.drawRoundedRect(lineRect, 6, 6);
    p.setBrush(palette().brush(QPalette::Base));
    p.drawEllipse(4,4,16,16);
    p.setPen(QPen(Qt::black));
    p.drawLine(6,12,18,12);
    if (isCollapsed())
        p.drawLine(12, 6, 12, 18);
    QFont f = font();
    f.setBold(true);
    p.setFont(f);
    int textHeight = QFontMetrics(f).height();
    p.drawText(24,textHeight,title());
    event->accept();
}

void GroupBox::setCollapsed(bool collapse)
{
    b_collapsed = collapse;
    foreach ( QWidget * widget, findChildren<QWidget*>() )
        widget->setHidden(collapse);
    setFixedHeight(collapse? 24 : QWIDGETSIZE_MAX);
    update();
}

}
