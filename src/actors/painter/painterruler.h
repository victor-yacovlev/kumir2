#ifndef PAINTERRULER_H
#define PAINTERRULER_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

namespace ActorPainter {

class PainterRuler : public QWidget
{
    Q_OBJECT
public:
    explicit PainterRuler(QWidget *parent = 0);
    void setCanvasSize(int pixelSize);
    void setScrollBar(QScrollBar * scrollBar);
    void setView(QWidget * view);
    void setZoom(qreal v);
public slots:
    void highlightValue(int v);
private:
    void paintEvent(QPaintEvent *event);
    void paintRuler(int offset, int size);

    int canvasPixelSize_;
    QScrollBar * m_scrollBar;
    QWidget * m_view;
    int i_highLight;
    qreal r_zoom;
};

}

#endif // PAINTERRULER_H
