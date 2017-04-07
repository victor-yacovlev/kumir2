/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "paintermodule.h"
#include "painterwindow.h"
#include "painterview.h"
#include "paintertools.h"

namespace ActorPainter {

PainterModule::PainterModule(ExtensionSystem::KPlugin * parent)
    : PainterModuleBase(parent)
{
    m_window = 0;
    dirty_ = true;
}

void PainterModule::createGui()
{
    m_window = new PainterWindow(this, 0);
    view = m_window->view();
    originalCanvas.reset(new QImage(QSize(640,480), QImage::Format_RGB32));
    originalCanvas->fill(QColor("white").rgb());
    canvas.reset(new QImage(QSize(640,480), QImage::Format_RGB32));
    canvas->fill(QColor("white").rgb());
    canvasLock = new QMutex;
    m_window->setCanvasSize(canvas->size());
    dirtyLock_ = new QMutex;
    startTimer(50);
    reset();
}

QString PainterModule::initialize(const QStringList &configurationParameters, const ExtensionSystem::CommandLine &)
{
    if (!configurationParameters.contains("tablesOnly")) {
        createGui();
    }
    return "";
}

void PainterModule::timerEvent(QTimerEvent *event)
{
    dirtyLock_->lock();
    if (dirty_) {
        canvasLock->lock();
        if (view) {
            view->setCanvasData(*canvas);
        }
        canvasLock->unlock();
        dirty_ = false;
    }
    dirtyLock_->unlock();
    event->accept();
}

void PainterModule::markViewDirty()
{
    QMutexLocker l(dirtyLock_);
    dirty_ = true;
}

QList<ExtensionSystem::CommandLineParameter> PainterModule::acceptableCommandLineParameters()
{
    return QList<ExtensionSystem::CommandLineParameter>();
}

void PainterModule::reset()
{
    point = QPoint(0,0);
    font = QFont();
    brush = QBrush();
    style = Qt::SolidPattern;
    transparent = false;
    pen = QPen();
    canvasLock->lock();
    canvas.reset(new QImage(originalCanvas->copy()));
    m_window->setCanvasSize(originalCanvas->size());
    canvasLock->unlock();
    markViewDirty();
}

void PainterModule::setAnimationEnabled(bool )
{

}


QWidget* PainterModule::mainWidget() const
{
    return m_window;
}

static Color colorFromCss(const QString & css)
{
    QColor qColor = parseColor(css);
    QColor qRgbColor = qColor.toRgb();
    Color result;
    result.r = qRgbColor.red();
    result.g = qRgbColor.green();
    result.b = qRgbColor.blue();
    result.a = qRgbColor.alpha();
    return result;
}

Color PainterModule::runCMYK(const int c, const int m, const int y, const int k)
{
    return colorFromCss(QString::fromLatin1("cmyk(%1,%2,%3,%4)").arg(c).arg(m).arg(y).arg(k));
}


Color PainterModule::runCMYKA(const int c, const int m, const int y, const int k, const int a)
{
    return colorFromCss(QString::fromLatin1("cmyka(%1,%2,%3,%4,%5)").arg(c).arg(m).arg(y).arg(k).arg(a));
}


Color PainterModule::runHSL(const int h, const int s, const int l)
{
    return colorFromCss(QString::fromLatin1("hsl(%1,%2,%3)").arg(h).arg(s).arg(l));
}


Color PainterModule::runHSLA(const int h, const int s, const int l, const int a)
{
    return colorFromCss(QString::fromLatin1("hsla(%1,%2,%3,%4)").arg(h).arg(s).arg(l).arg(a));
}


Color PainterModule::runHSV(const int h, const int s, const int v)
{
    return colorFromCss(QString::fromLatin1("hsv(%1,%2,%3)").arg(h).arg(s).arg(v));
}


Color PainterModule::runHSVA(const int h, const int s, const int v, const int a)
{
    return colorFromCss(QString::fromLatin1("hsva(%1,%2,%3,%4)").arg(h).arg(s).arg(v).arg(a));
}


Color PainterModule::runRGB(const int r, const int g, const int b)
{
    return colorFromCss(QString::fromLatin1("rgb(%1,%2,%3)").arg(r).arg(g).arg(b));
}


Color PainterModule::runRGBA(const int r, const int g, const int b, const int a)
{
    return colorFromCss(QString::fromLatin1("rgba(%1,%2,%3,%4)").arg(r).arg(g).arg(b).arg(a));
}


void PainterModule::runMoveTo(const int x, const int y)
{
    point.setX(x);
    point.setY(y);
}


int PainterModule::runPageHeight()
{
    return canvas->height();
}


void PainterModule::runLoadPage(const QString& fileName)
{
    if (!QFile::exists(fileName)) {
        setError(tr("File not exists: %s").arg(fileName));
    }
    canvas.reset(new QImage(fileName));
    if (m_window) {
        m_window->setCanvasSize(canvas->size());
        m_window->setCanvasName(QFile(fileName).fileName());
    }
    markViewDirty();
}


void PainterModule::runFill(const int x, const int y)
{
    if (transparent)
        return;
    QStack<QPoint> stack;
    QRgb replaceColor = canvas->pixel(x,y);
    if (replaceColor==brush.color().rgb())
        return;
    stack.push(QPoint(x,y));
    while (!stack.isEmpty()) {
        QPoint pnt = stack.pop();
        if (pnt.x()<0 || pnt.y()<0 || pnt.x()>=canvas->width() || pnt.y()>=canvas->height())
            continue;
        QRgb value = canvas->pixel(pnt);
        if (value==replaceColor) {
            canvasLock->lock();
            canvas->setPixel(pnt, brush.color().rgb());
            canvasLock->unlock();
            stack.push(QPoint(pnt.x()-1, pnt.y()));
            stack.push(QPoint(pnt.x()+1, pnt.y()));
            stack.push(QPoint(pnt.x(), pnt.y()-1));
            stack.push(QPoint(pnt.x(), pnt.y()+1));
        }
    }
    markViewDirty();
}


void PainterModule::runSetBrush(const Color& cs)
{
    QColor c(cs.r, cs.g, cs.b, cs.a);
    brush.setColor(c);
    brush.setStyle(style);
    transparent = false;
}


void PainterModule::runLine(const int x0, const int y0, const int x, const int y)
{
    canvasLock->lock();
    QPainter p(canvas.data());
    p.setPen(pen);
    p.setBrush(brush);
    p.drawLine(x0,y0,x,y);
    canvasLock->unlock();
    p.end();
    markViewDirty();
}


void PainterModule::runLineTo(const int x1, const int y1)
{
    runLine(point.x(), point.y(), x1, y1);
    point.setX(x1);
    point.setY(y1);
}

void PainterModule::runPolygon(const int N, const QVector<int> &xx, const QVector<int> &yy)
{
    QVector<QPoint> points(N);
    if (N!=xx.size() || N!=yy.size()) {
        setError(tr("Invalid points number"));
    }
    else {
        for (int i=0; i<N; i++) {
            points[i] = QPoint(xx[i], yy[i]);
        }
        drawPolygon(points);
    }
}

void PainterModule::drawPolygon(const QVector<QPoint> &points)
{
    canvasLock->lock();
    QPainter p(canvas.data());
    p.setPen(pen);
    p.setBrush(brush);
    p.drawPolygon(QPolygon(points));
    canvasLock->unlock();
    markViewDirty();
}


void PainterModule::runWrite(const int x, const int y, const QString& text)
{
    canvasLock->lock();
    QPainter p(canvas.data());
    p.setPen(pen);
    p.setBrush(brush);
    p.setFont(font);
    p.drawText(x,y,text);
    canvasLock->unlock();
    markViewDirty();
}


void PainterModule::runNewPage(const int width, const int height, const Color& backgroundColor)
{
    QColor clr(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    canvas.reset(new QImage(width,height,QImage::Format_RGB32));
    canvas->fill(clr.rgb());
    m_window->setCanvasSize(canvas->size());
    QString colorName = findColorName(backgroundColor);
    if (colorName.length() > 0 && ! colorName.contains("(") ) {
        colorName[0] = colorName[0].toUpper();
    }
    m_window->setCanvasName(QString("%1 %2x%3").arg(colorName).arg(width).arg(height));
    markViewDirty();
}


void PainterModule::runCircle(const int xc, const int yc, const int r)
{
    canvasLock->lock();
    QPainter p(canvas.data());
    p.setPen(pen);
    p.setBrush(brush);
    p.drawEllipse(QPoint(xc, yc), r, r);
    canvasLock->unlock();
    markViewDirty();
}


void PainterModule::runSetPen(const int width, const Color& cs)
{
    QColor c(cs.r, cs.g, cs.b, cs.a);
    if (width<1) {
        pen = Qt::NoPen;
    }
    else {
        pen = QPen(c, width);
    }
}


void PainterModule::runPoint(const int x, const int y, const Color& color)
{
    canvasLock->lock();
    QPainter p(canvas.data());
    const QColor qColor(color.r, color.g, color.b, color.a);
    p.setPen(QPen(qColor));
    p.drawPoint(x,y);
    canvasLock->unlock();
    markViewDirty();
}


void PainterModule::runDensity(const int value)
{
    int dens = value;
    dens = qMin(8, dens);
    dens = qMax(0, dens);
    if (dens==0) {
        brush.setStyle(Qt::NoBrush);
    }
    else if (dens==1) {
        brush.setStyle(Qt::Dense7Pattern);
    }
    else if (dens==2) {
        brush.setStyle(Qt::Dense6Pattern);
    }
    else if (dens==3) {
        brush.setStyle(Qt::Dense5Pattern);
    }
    else if (dens==4) {
        brush.setStyle(Qt::Dense4Pattern);
    }
    else if (dens==5) {
        brush.setStyle(Qt::Dense3Pattern);
    }
    else if (dens==6) {
        brush.setStyle(Qt::Dense2Pattern);
    }
    else if (dens==7) {
        brush.setStyle(Qt::Dense1Pattern);
    }
    else if (dens==8) {
        brush.setStyle(Qt::SolidPattern);
    }
    style = brush.style();
    if (transparent)
        brush.setStyle(Qt::NoBrush);
    else
        brush.setStyle(style);
}


void PainterModule::runRectangle(const int x0, const int y0, const int x1, const int y1)
{
    QVector<QPoint> points(4);
    points[0] = QPoint(x0,y0);
    points[1] = QPoint(x1,y0);
    points[2] = QPoint(x1,y1);
    points[3] = QPoint(x0,y1);
    drawPolygon(points);
    point.setX(x1);
    point.setY(y1);
}


void PainterModule::runSplitToCMYK(const Color& color, int& c, int& m, int& y, int& k)
{
    const QColor clr = QColor(color.r, color.g, color.b, color.a).toCmyk();
    c = clr.cyan();
    m = clr.magenta();
    y = clr.yellow();
    k = clr.black();
}


void PainterModule::runSplitToHSL(const Color& color, int& h, int& s, int& l)
{
    const QColor clr = QColor(color.r, color.g, color.b, color.a).toHsl();
    h = clr.hue();
    s = clr.saturation();
    l = clr.lightness();
}


void PainterModule::runSplitToHSV(const Color& color, int& h, int& s, int& v)
{
    const QColor clr = QColor(color.r, color.g, color.b, color.a).toHsv();
    h = clr.hue();
    s = clr.saturation();
    v = clr.value();
}


void PainterModule::runSplitToRGB(const Color& color, int& r, int& g, int& b)
{
    const QColor clr = QColor(color.r, color.g, color.b, color.a).toRgb();
    r = clr.red();
    g = clr.green();
    b = clr.blue();
}


void PainterModule::runSavePage(const QString& fileName)
{
    QFile f(fileName);
    if (f.open(QIODevice::WriteOnly)) {
        canvas->save(&f,"PNG");
        f.close();
    }
    else {
        setError(tr("Can't open file for writing: %s").arg(fileName));
    }
}


void PainterModule::runUnsetBrush()
{
    brush.setStyle(Qt::NoBrush);
    transparent = true;
}


Color PainterModule::runPointSample(const int x, const int y)
{
    if (x<0 || y<0) {
        setError(tr("Coordinate is less than 0"));
        return Color();
    }
    else if (x>=canvas->width() || y>=canvas->height()) {
        setError(tr("Coordinate is out of page"));
        return Color();
    }
    else {
        QColor c = QColor::fromRgb(canvas->pixel(x,y));
        Color cc;
        cc.r = c.red();
        cc.g = c.green();
        cc.b = c.blue();
        cc.a = c.alpha();
        return cc;
    }
}


int PainterModule::runCenterX()
{
    return canvas->width()/2;
}


int PainterModule::runCenterY()
{
    return canvas->height()/2;
}


int PainterModule::runPageWidth()
{
    return canvas->width();
}


int PainterModule::runTextWidth(const QString& text)
{
    const QFontMetrics fm(font);
    return fm.width(text);
}


void PainterModule::runSetFont(const QString& family, const int size, const bool bold, const bool italic)
{
    if (!family.isEmpty())
        font.setFamily(family);
    if (size>0)
        font.setPixelSize(size);
    font.setBold(bold);
    font.setItalic(italic);
}


void PainterModule::runEllipse(const int x_left, const int y_top, const int x_right, const int y_bottom)
{
    canvasLock->lock();
    QPainter p(canvas.data());
    p.setPen(pen);
    p.setBrush(brush);
    const QRect ellipseRect(QPoint(x_left, y_top), QPoint(x_right, y_bottom));
    p.drawEllipse(ellipseRect.center(), ellipseRect.width()/2, ellipseRect.height()/2);
    canvasLock->unlock();
    markViewDirty();
}


    
} // $namespace
