#include "terminal_plane.h"
#include "terminal.h"
#include "terminal_onesession.h"

namespace Terminal {

static int sessionMargin = 4;

Plane::Plane(Terminal *parent)
    : QWidget(parent)
    , m_terminal(parent)
    , pnt_offset(0,0)
{

}

void Plane::paintEvent(QPaintEvent *e)
{
    QPainter p(this);


    QPoint offset = pnt_offset;
    int y = sessionMargin;
    for (int i=0; i<m_terminal->l_sessions.size(); i++) {
        OneSession * session = m_terminal->l_sessions[i];
        const QSize sessionSize = session->visibleSize();
        const QRect sessionRect(QPoint(sessionMargin, y), sessionSize);
        const QRect myRect(offset, size());
        if (myRect.intersects(sessionRect)) {
            p.save();
            p.translate(sessionMargin+pnt_offset.x(), y+pnt_offset.y());
            session->draw(&p, width()-2*sessionMargin);
            p.restore();
        }
    }
    p.setPen(QPen(hasFocus()? palette().brush(QPalette::Highlight) : palette().brush(QPalette::Window), 1));
    p.setBrush(QColor(240,240,240));
    p.drawRect(0,0,width(), height());
    p.end();
    e->accept();
}

} // namespace Terminal
