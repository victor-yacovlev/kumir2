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


#ifdef Q_OS_WIN32
void GroupBox::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    p.setPen(QPen(QColor("lightgray")));
    QRect lineRect(2, 2, width()-4, height()-4);
    p.drawRect(lineRect);
    p.setBrush(QColor("white"));
    p.setPen(QPen(QBrush(QColor("black")), 1.0));
    p.drawRect(6, 5, 8, 8);
    p.drawLine(6, 9, 13, 9);
    if (isCollapsed()) {
        p.drawLine(10, 5, 10, 13);
    }
    QFont f = font();
    f.setBold(true);
    p.setFont(f);
    int textHeight = QFontMetrics(f).height();
    p.drawText(24,textHeight,title());
    event->accept();
}
#else
void GroupBox::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QRect lineRect(2, 2, width()-4, height()-4);
    QStyleOptionFrame frameOpt;
    frameOpt.initFrom(this);
    frameOpt.rect = lineRect;
    p.save();
    style()->drawPrimitive(QStyle::PE_FrameGroupBox, &frameOpt, &p, this);
    p.restore();
    QStyleOption btnOpt;
    btnOpt.initFrom(this);
    btnOpt.state = QStyle::State_Children | (!isCollapsed()? QStyle::State_Open : QStyle::State_None);
    btnOpt.rect = QRect(4,4,16,16);
    p.save();
    style()->drawPrimitive(isCollapsed()
                           ? QStyle::PE_IndicatorArrowRight
                           : QStyle::PE_IndicatorArrowDown,
                           &btnOpt, &p, this);
    p.restore();
    QFont f = font();
    f.setBold(true);
    p.setFont(f);
    int textHeight = QFontMetrics(f).height();
    p.drawText(24,textHeight + 2,title());
    event->accept();
}
#endif

void GroupBox::setCollapsed(bool collapse)
{
    b_collapsed = collapse;
    foreach ( QWidget * widget, findChildren<QWidget*>() )
        if (widget->parent()==this)
            widget->setHidden(collapse);
    setFixedHeight(collapse? 24 : QWIDGETSIZE_MAX);
    update();
}

}
