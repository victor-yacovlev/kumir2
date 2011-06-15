#ifndef ACTORPAINTER_PAINTERWORKER_H
#define ACTORPAINTER_PAINTERWORKER_H

#include <QtCore>
#include <QtGui>

namespace ActorPainter {

class PainterWorker
        : public QObject
{
    Q_OBJECT
public:
    PainterWorker(QSettings * settings, QObject * parent = 0);
    static PainterWorker* instance();
    static PainterWorker* instance(QSettings * settings, QObject * parent);
    inline QString error() const { return lastError; }
    QWidget * mainWidget();
    QList<QAction*> toolbarActions();
    QList<QMenu*> menus();
public slots:
    void reset();
    void setPen(int width, const QString &cs);
    void setDensity(int dens);
    void setBrush(const QString & cs);
    QVariantList splitColor(const QString & model, const QString & cs);
    void unsetBrush();
    void pixel(int x, int y, const QString &cs);
    void moveTo(int x, int y);
    void lineTo(int x1, int y1);
    void line(int x0, int y0, int x1, int y1);
    void drawRect(int x0, int y0, int x1, int y1);
    void drawCircle(int xc, int yx, int r);
    void drawEllipse(int x0, int y0, int x1, int y1);
    void drawPolygon(int N, const QList<QVariant> &xx, const QList<QVariant> &yy);
    void setFont(const QString &family, int size, bool bold, bool italic);
    int textWidth(const QString &text);
    void drawText(int x, int y, const QString &text);
    void fill(int x, int y);
    static inline QString RGB(int r, int g, int b) { return QString("rbg(%1,%2,%3)").arg(r).arg(g).arg(b); }
    static inline QString RGBA(int r, int g, int b, int a) { return QString("rbga(%1,%2,%3,%4)").arg(r).arg(g).arg(b).arg(a); }
    static inline QString CMYK(int c, int m, int y, int k) { return QString("cmyk(%1,%2,%3,%4)").arg(c).arg(m).arg(y).arg(k); }
    static inline QString CMYKA(int c, int m, int y, int k, int a) { return QString("cmyka(%1,%2,%3,%4,%5)").arg(c).arg(m).arg(y).arg(k).arg(a); }
    static inline QString HSL(int h, int s, int l) { return QString("hsl(%1,%2,%3)").arg(h).arg(s).arg(l); }
    static inline QString HSLA(int h, int s, int l, int a) { return QString("hsla(%1,%2,%3,%4)").arg(h).arg(s).arg(l).arg(a); }
    static inline QString HSV(int h, int s, int v) { return QString("hsv(%1,%2,%3)").arg(h).arg(s).arg(v); }
    static inline QString HSVA(int h, int s, int v, int a) { return QString("hsva(%1,%2,%3,%4)").arg(h).arg(s).arg(v).arg(a); }
    int pageWidth();
    int pageHeight();
    inline int pageCenterX() { return pageWidth()/2; }
    inline int pageCenterY() { return pageHeight()/2; }
    void newPage(int w, int h, const QString &bg);
    void loadPage(const QString &fileName);
    void savePage(const QString &fileName);
    QString pixelColor(int x, int y);

private slots:
    void drawPoint(int x, int y, const QColor &color);
    void drawLine(int x0, int y0, int x1, int y1);
    void drawPolygon(const QVector<QPoint> &points);


    void handleNewImageRequest(int w, int h, const QString &bg, bool useTemplate, const QString &fileName);
    void handleLoadImageRequest(const QString &fileName);
private:
    class PainterWindow * m_mainWidget;
    QWidget * view;
    QString templateName;
    QString lastError;
    QPen pen;
    QBrush brush;
    Qt::BrushStyle style;
    bool transparent;
    QPoint point;
    QFont font;
    QMutex * canvasLock;
    QImage * canvas;
    QImage * originalCanvas;
    static PainterWorker* m_instance;
};

} // namespace ActorPainter

#endif // ACTORPAINTER_PAINTERWORKER_H
