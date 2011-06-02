#include "painterruler.h"

namespace ActorPainter {

PainterRuler::PainterRuler(QWidget *parent)
    : QWidget(parent)
    , m_canvas(0)
    , m_scrollBar(0)
    , m_view(0)
    , i_highLight(-1)
    , r_zoom(1.0)
{
}

void PainterRuler::setScrollBar(QScrollBar *scrollBar)
{
    m_scrollBar = scrollBar;
    if (m_scrollBar->orientation()==Qt::Vertical) {
        setFixedWidth(24);
    }
    else {
        setFixedHeight(24);
    }
}

void PainterRuler::setZoom(qreal v)
{
    r_zoom = v;
    update();
}

void PainterRuler::setCanvas(QImage * canvas)
{
    m_canvas = canvas;
}

void PainterRuler::setView(QWidget *view)
{
    m_view = view;
}

void PainterRuler::highlightValue(int v)
{
    i_highLight = v;
    update();
}

void PainterRuler::paintEvent(QPaintEvent *event)
{
    if (m_canvas && m_scrollBar && m_view) {
        int offset = 18;
        if (m_scrollBar->orientation()==Qt::Horizontal) {
            if (m_scrollBar->isVisible()) {
                int val = m_scrollBar->value();
                offset -= val;
            }
            else {
                offset += m_view->x();
            }
        }
        if (m_scrollBar->orientation()==Qt::Vertical) {
            if (m_scrollBar->isVisible()) {
                int val = m_scrollBar->value();
                offset -= val;
            }
            else {
                offset += m_view->y();
            }
        }
        int size = m_scrollBar->orientation()==Qt::Vertical?
                   m_canvas->height() : m_canvas->width();
        paintRuler(offset, size);
        if (i_highLight>-1) {
            QPainter p(this);
            QColor c = palette().brush(QPalette::Highlight).color();
            p.setPen(QPen(c,3));
            if (m_scrollBar->orientation()==Qt::Horizontal) {
                p.drawLine(int(i_highLight*r_zoom)+offset, 0, int(i_highLight*r_zoom)+offset, 24);
            }
            else {
                p.drawLine(0, int(i_highLight*r_zoom)+offset, 24, int(i_highLight*r_zoom)+offset);
            }
        }
    }
    event->accept();
}

void PainterRuler::paintRuler(int offset, int size)
{
    QPainter p(this);
    p.setBrush(QColor("white"));
    p.setPen(QColor("black"));
    QFont f = p.font();
    f.setPixelSize(8);
    f.setBold(true);
    p.setFont(f);
    if (m_scrollBar->orientation()==Qt::Vertical) {
        p.drawRect(0, offset, 24, int(size*r_zoom));
    }
    else {
        p.drawRect(offset, 0, int(size*r_zoom), 24);
    }
    for (int v=offset; v<offset+int(size*r_zoom); v+=10) {
        if (m_scrollBar->orientation()==Qt::Vertical) {
            if ( (v-offset) % 100 == 0 ) {
                p.drawLine(0, v, 24, v);
                p.drawText(2, v+2, 22, 16, 0, QString::number(int((v-offset)/r_zoom)));
            }
            else {
                p.drawLine(12, v, 24, v);
            }
        }
        else {
            if ( (v-offset) % 100 == 0 ) {
                p.drawLine(v, 0, v, 24);
                p.drawText(v+2, 2, 22, 16, 0, QString::number(int((v-offset)/r_zoom)));
            }
            else {
                p.drawLine(v, 12, v, 24);
            }
        }
    }
}

}
