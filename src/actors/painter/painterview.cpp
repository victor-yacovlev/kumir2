#include "painterview.h"
#include "paintertools.h"

namespace ActorPainter {

PainterView::PainterView(QWidget *parent)
    : QWidget(parent)
    , m_canvas(0)
    , m_locker(new QMutex)
    , r_zoom(1.0)
{
    setMouseTracking(true);
}

//void PainterView::setCanvas(QImage * canvas, QMutex *locker)
//{
//    m_canvas = canvas;
//    m_locker = locker;
//    if (m_canvas && m_locker) {
//        setFixedSize(int(canvas->size().width()*r_zoom)+36, int(canvas->size().height()*r_zoom)+36);
//        update();
//    }
//}

void PainterView::setCanvasSize(const QSize &size)
{
    QMutexLocker l(m_locker);
    if (m_canvas)
        delete m_canvas;
    m_canvas = new QImage(size, QImage::Format_ARGB32);
}

void PainterView::setCanvasData(QImage data)
{
    QMutexLocker l(m_locker);
    if (!m_canvas || m_canvas->width() != data.width() || m_canvas->height() != data.height()) {
        if (m_canvas) delete m_canvas;
        m_canvas = new QImage(data.size(), QImage::Format_ARGB32);
    }
    *m_canvas = data.copy(data.rect());
    update();
}

void PainterView::updateSizeFromCanvas()
{
    if (m_canvas && m_locker) {
        m_locker->lock();
        setFixedSize(int(m_canvas->size().width()*r_zoom)+36, int(m_canvas->size().height()*r_zoom)+36);
        update();
        m_locker->unlock();
    }
}

void PainterView::setZoom(qreal v)
{
    r_zoom = v;
    updateSizeFromCanvas();
}

void PainterView::paintEvent(QPaintEvent *event)
{
    m_locker->lock();
    if (m_canvas) {
        QPainter p(this);
        p.setBrush(QColor("black"));
        p.setPen(Qt::NoPen);
        p.drawRect(22,22,int(m_canvas->width()*r_zoom), int(m_canvas->height()*r_zoom));
        p.drawRect(17,17,int(m_canvas->width()*r_zoom)+2, int(m_canvas->height()*r_zoom)+2);
        int W = int(m_canvas->width()*r_zoom);
        int H = int(m_canvas->height()*r_zoom);
        p.drawImage(18,18,m_canvas->scaled(W,H));
    }
    m_locker->unlock();
    event->accept();
}

void PainterView::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_canvas) {
        event->ignore();
    }
    else {
        int x = int(event->pos().x()-18)/r_zoom;
        int y = int(event->pos().y()-18)/r_zoom;
        if (x<0 || y<0 || x>=m_canvas->width() || y>=m_canvas->height())
            emit cursorOver(-1, -1, QColor::Invalid);
        else {
            const QColor pixelColor = QColor::fromRgb(m_canvas->pixel(x,y));
            emit cursorOver(x,y,pixelColor);
        }
    }
    event->accept();
}

QImage * PainterView::canvas()
{
    return m_canvas;
}

}
