#include "terminal_plane.h"
#include "terminal.h"
#include "terminal_onesession.h"

namespace Terminal {

static int sessionMargin = 4;

Plane::Plane(Terminal *parent)
    : QWidget(parent)
    , m_terminal(parent)
{

}

QPoint Plane::offset() const
{
    QPoint result(0,0);
    if (m_terminal->sb_horizontal->isEnabled()) {
        int valX = m_terminal->sb_horizontal->value();
        result.setX(-valX);
    }
    if (m_terminal->sb_vertical->isEnabled()) {
        int valY = m_terminal->sb_vertical->value();
        result.setY(-valY);
    }
    return result;
}


void Plane::updateScrollBars()
{
    QPoint prevOffset = offset();
    int w = 2 * sessionMargin;
    int h = 2 * sessionMargin;
    for (int i=0; i<m_terminal->l_sessions.size(); i++) {
        OneSession * s = m_terminal->l_sessions[i];
        QSize ss = s->visibleSize(width()-2*sessionMargin);
        w = qMax(w, sessionMargin*2 + ss.width());
        h += sessionMargin + ss.height();
    }

    QScrollBar * hb = m_terminal->sb_horizontal;
    QScrollBar * vb = m_terminal->sb_vertical;

    if (w<=width()) {
        hb->setEnabled(false);
    }
    else {
        hb->setEnabled(true);
        hb->setRange(0, w - width());
        hb->setSingleStep(width()/3);
        hb->setPageStep(width());
    }

    if (h<=height()) {
        vb->setEnabled(false);
    }
    else {
        QFontMetrics fm(font());
        int dH = fm.height();
        vb->setEnabled(true);
        vb->setRange(0, h - height());
        vb->setSingleStep(dH);
        vb->setPageStep(height());
    }

}

void Plane::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    updateScrollBars();
}

void Plane::wheelEvent(QWheelEvent *e)
{
    if (!m_terminal->sb_vertical->isEnabled() && e->orientation()==Qt::Vertical) {
        e->ignore();
        return;
    }
    if (!m_terminal->sb_horizontal->isEnabled() && e->orientation()==Qt::Horizontal) {
        e->ignore();
        return;
    }
    int degrees = e->delta() / 8;
    int steps = degrees / 15;
    QScrollBar * sb = e->orientation()==Qt::Vertical? m_terminal->sb_vertical : m_terminal->sb_horizontal;
    sb->setValue(sb->value()-steps*sb->singleStep()*3);
}

void Plane::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
//    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    p.setPen(QPen(hasFocus()? palette().brush(QPalette::Highlight) : palette().brush(QPalette::Window), 1));
    p.setBrush(QColor(100,100,100));
    p.drawRect(0,0,width(), height());

    QPoint off = offset();
    int y = sessionMargin;
    for (int i=0; i<m_terminal->l_sessions.size(); i++) {
        OneSession * session = m_terminal->l_sessions[i];
        const QSize sessionSize = session->visibleSize(width()-2*sessionMargin);
//        const QRect sessionRect = QRect(QPoint(sessionMargin, y), sessionSize).translated(off);
//        const QRect myRect(off, size());
//        if (myRect.intersects(sessionRect)) {
        {

            p.save();
            p.translate(sessionMargin+off.x(), y+off.y());
            session->draw(&p, width()-2*sessionMargin);
            p.restore();
        }
        y += sessionSize.height() + sessionMargin;
    }

    p.end();
    e->accept();
}

} // namespace Terminal
