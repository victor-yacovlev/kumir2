#ifndef PAINTERRULER_H
#define PAINTERRULER_H

#include <QtGui>

namespace ActorPainter {

class PainterRuler : public QWidget
{
    Q_OBJECT
public:
    explicit PainterRuler(QWidget *parent = 0);
    void setCanvas(QImage * canvas);
    void setScrollBar(QScrollBar * scrollBar);
    void setView(QWidget * view);
    void setZoom(qreal v);
public slots:
    void highlightValue(int v);
private:
    void paintEvent(QPaintEvent *event);
    void paintRuler(int offset, int size);

    QImage * m_canvas;
    QScrollBar * m_scrollBar;
    QWidget * m_view;
    int i_highLight;
    qreal r_zoom;
};

}

#endif // PAINTERRULER_H
