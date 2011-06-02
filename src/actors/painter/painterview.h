#ifndef PAINTERVIEW_H
#define PAINTERVIEW_H

#include <QtGui>

namespace ActorPainter {

class PainterView : public QWidget
{
    Q_OBJECT
public:
    explicit PainterView(QWidget *parent = 0);
    void setCanvas(QImage * canvas, QMutex * locker);
    QImage * canvas();
    void setZoom(qreal v);
    inline qreal zoom() const { return r_zoom; }

signals:
    void cursorOver(int x, int y, const QColor &color);

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);


private:
    QImage * m_canvas;
    QMutex * m_locker;
    qreal r_zoom;
};

}

#endif // PAINTERVIEW_H
