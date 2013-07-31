/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#include <QtCore>
#include <QtGui>
#include "paintermodule.h"
#include "painterwindow.h"
#include "paintertools.h"

namespace ActorPainter {

PainterModule::PainterModule(ExtensionSystem::KPlugin * parent)
    : PainterModuleBase(parent)
{
    m_window = new PainterWindow(this, 0);
    view = m_window->view();
    originalCanvas = new QImage(QSize(640,480), QImage::Format_RGB32);
    originalCanvas->fill(QColor("white").rgb());
    canvas = new QImage(QSize(640,480), QImage::Format_RGB32);
    canvas->fill(QColor("white").rgb());
    canvasLock = new QMutex;
    m_window->setCanvas(canvas, canvasLock);
    reset();
}

QList<ExtensionSystem::CommandLineParameter> PainterModule::acceptableCommandLineParameters()
{
    return QList<ExtensionSystem::CommandLineParameter>();
}

void PainterModule::reset()
{
    QImage * del = canvas;

    point = QPoint(0,0);
    font = QFont();
    brush = QBrush();
    style = Qt::SolidPattern;
    transparent = false;
    pen = QPen();
    canvasLock->lock();
    canvas = new QImage(originalCanvas->copy());
    m_window->setCanvas(canvas, canvasLock);
    canvasLock->unlock();
    if (view)
        view->update();
    delete del;
}

void PainterModule::setAnimationEnabled(bool )
{

}


QWidget* PainterModule::mainWidget() const
{
    return m_window;
}

Color PainterModule::runCMYK(const int c, const int m, const int y, const int k)
{
    Color cc;
    cc.cssValue = QString::fromAscii("cmyk(%1,%2,%3,%4)").arg(c).arg(m).arg(y).arg(k);
    return cc;
}


Color PainterModule::runCMYKA(const int c, const int m, const int y, const int k, const int a)
{
    Color cc;
    cc.cssValue = QString::fromAscii("cmyka(%1,%2,%3,%4,%5)").arg(c).arg(m).arg(y).arg(k).arg(a);
    return cc;
}


Color PainterModule::runHSL(const int h, const int s, const int l)
{
    Color c;
    c.cssValue = QString::fromAscii("hsl(%1,%2,%3)").arg(h).arg(s).arg(l);
    return c;
}


Color PainterModule::runHSLA(const int h, const int s, const int l, const int a)
{
    Color c;
    c.cssValue = QString::fromAscii("hsla(%1,%2,%3,%4)").arg(h).arg(s).arg(l).arg(a);
    return c;
}


Color PainterModule::runHSV(const int h, const int s, const int v)
{
    Color c;
    c.cssValue = QString::fromAscii("hsv(%1,%2,%3)").arg(h).arg(s).arg(v);
    return c;
}


Color PainterModule::runHSVA(const int h, const int s, const int v, const int a)
{
    Color c;
    c.cssValue = QString::fromAscii("hsva(%1,%2,%3,%4)").arg(h).arg(s).arg(v).arg(a);
    return c;
}


Color PainterModule::runRGB(const int r, const int g, const int b)
{
    Color c;
    c.cssValue = QString::fromAscii("rgb(%1,%2,%3)").arg(r).arg(g).arg(b);
    return c;
}


Color PainterModule::runRGBA(const int r, const int g, const int b, const int a)
{
    Color c;
    c.cssValue = QString::fromAscii("rgba(%1,%2,%3,%4)").arg(r).arg(g).arg(b).arg(a);
    return c;
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
    QImage * oldCanvas = canvas;
    canvas = new QImage(fileName);
    if (m_window)
        m_window->setCanvas(canvas, canvasLock);
    delete oldCanvas;
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
            view->update();
            stack.push(QPoint(pnt.x()-1, pnt.y()));
            stack.push(QPoint(pnt.x()+1, pnt.y()));
            stack.push(QPoint(pnt.x(), pnt.y()-1));
            stack.push(QPoint(pnt.x(), pnt.y()+1));
        }
    }
    if (view)
        view->update();
}


void PainterModule::runSetBrush(const Color& cs)
{
    QColor c = parseColor(cs.cssValue);
    if (cs.cssValue.trimmed().isEmpty())
        transparent = true;
    else
        transparent = false;
    if (!transparent && !c.isValid()) {
        setError(tr("Invalid color"));
    }
    brush.setColor(c);
    if (transparent)
        brush.setStyle(Qt::NoBrush);
    else
        brush.setStyle(style);
}


void PainterModule::runLine(const int x0, const int y0, const int x, const int y)
{
    canvasLock->lock();
    QPainter p(canvas);
    p.setPen(pen);
    p.setBrush(brush);
    p.drawLine(x0,y0,x,y);
    canvasLock->unlock();
    p.end();
    if (view)
        view->update();
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
    QPainter p(canvas);
    p.setPen(pen);
    p.setBrush(brush);
    p.drawPolygon(QPolygon(points));
    canvasLock->unlock();
    if (view)
        view->update();
}


void PainterModule::runWrite(const int x, const int y, const QString& text)
{
    canvasLock->lock();
    QPainter p(canvas);
    p.setPen(pen);
    p.setBrush(brush);
    p.setFont(font);
    p.drawText(x,y,text);
    canvasLock->unlock();
    if (view)
        view->update();
}


void PainterModule::runNewPage(const int width, const int height, const Color& backgroundColor)
{
    QColor clr = parseColor(backgroundColor.cssValue);
    QImage * oldCanvas = canvas;
    canvas = new QImage(width,height,QImage::Format_RGB32);
    canvas->fill(clr.rgb());
    m_window->setCanvas(canvas, canvasLock);
    delete oldCanvas;
}


void PainterModule::runCircle(const int xc, const int yc, const int r)
{
    int x0 = xc-r;
    int y0 = yc-r;
//    int x1 = xc+r;
//    int y1 = yc+r;
    runEllipse(x0,y0,2 * r,2 * r);
}


void PainterModule::runSetPen(const int width, const Color& cs)
{
    QColor c = parseColor(cs.cssValue.trimmed());
    if (cs.cssValue.trimmed().isEmpty() || width<1) {
        pen = Qt::NoPen;
    }
    else {
        pen = QPen(c, width);
    }
}


void PainterModule::runPoint(const int x, const int y, const Color& color)
{
    canvasLock->lock();
    QPainter p(canvas);
    p.setPen(QPen(parseColor(color.cssValue)));
    p.drawPoint(x,y);
    canvasLock->unlock();
    if (view)
        view->update();
}


void PainterModule::runDensity(const int value)
{
    int dens = value;
    dens = qMax(8, dens);
    dens = qMin(0, dens);
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
    const QColor clr(color.cssValue);
    c = clr.cyan();
    m = clr.magenta();
    y = clr.yellow();
    k = clr.black();
}


void PainterModule::runSplitToHSL(const Color& color, int& h, int& s, int& l)
{
    const QColor clr(color.cssValue);
    h = clr.hue();
    s = clr.saturation();
    l = clr.lightness();
}


void PainterModule::runSplitToHSV(const Color& color, int& h, int& s, int& v)
{
    const QColor clr(color.cssValue);
    h = clr.hue();
    s = clr.saturation();
    v = clr.value();
}


void PainterModule::runSplitToRGB(const Color& color, int& r, int& g, int& b)
{
    const QColor clr(color.cssValue);
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
        cc.cssValue = c.name();
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


void PainterModule::runEllipse(const int x, const int y, const int r1, const int r2)
{
    canvasLock->lock();
    QPainter p(canvas);
    p.setPen(pen);
    p.setBrush(brush);
    p.drawEllipse(x, y, r1, r2);
    canvasLock->unlock();
    if (view)
        view->update();
}

Color PainterModule::runOperatorINPUT(const QString& x, bool& ok)
{
    QStringList validValues;
    validValues << QString::fromUtf8("прозрачный");
    validValues << QString::fromUtf8("белый");
    validValues << QString::fromUtf8("черный");
    validValues << QString::fromUtf8("чёрный");
    validValues << QString::fromUtf8("серый");
    validValues << QString::fromUtf8("фиолетовый");
    validValues << QString::fromUtf8("синий");
    validValues << QString::fromUtf8("голубой");
    validValues << QString::fromUtf8("зелёный");
    validValues << QString::fromUtf8("зеленый");
    validValues << QString::fromUtf8("жёлтый");
    validValues << QString::fromUtf8("желтый");
    validValues << QString::fromUtf8("оранжевый");
    validValues << QString::fromUtf8("красный");
    ok = validValues.contains(x.trimmed());
    Color result;
    if (ok) {
        QColor c = parseColor(x.trimmed());
        result.cssValue = c.name();
    }
    return result;
}

QString PainterModule::runOperatorOUTPUT(const Color& x)
{
    QString result;
    QMap<QString,QString> standardColorNames;
    standardColorNames[parseColor(QString::fromUtf8("прозрачный")).name()] = (QString::fromUtf8("прозрачный"));
    standardColorNames[parseColor(QString::fromUtf8("белый")).name()] = (QString::fromUtf8("белый"));
    standardColorNames[parseColor(QString::fromUtf8("чёрный")).name()] = (QString::fromUtf8("чёрный"));
    standardColorNames[parseColor(QString::fromUtf8("серый")).name()] = (QString::fromUtf8("серый"));
    standardColorNames[parseColor(QString::fromUtf8("фиолетовый")).name()] = (QString::fromUtf8("фиолетовый"));
    standardColorNames[parseColor(QString::fromUtf8("синий")).name()] = (QString::fromUtf8("синий"));
    standardColorNames[parseColor(QString::fromUtf8("голубой")).name()] = (QString::fromUtf8("голубой"));
    standardColorNames[parseColor(QString::fromUtf8("зелёный")).name()] = (QString::fromUtf8("зелёный"));
    standardColorNames[parseColor(QString::fromUtf8("жёлтый")).name()] = (QString::fromUtf8("жёлтый"));
    standardColorNames[parseColor(QString::fromUtf8("оранжевый")).name()] = (QString::fromUtf8("оранжевый"));
    standardColorNames[parseColor(QString::fromUtf8("красный")).name()] = (QString::fromUtf8("красный"));
    if (standardColorNames.contains(x.cssValue)) {
        result = standardColorNames[x.cssValue];
    }
    else {
        result = x.cssValue;
    }
    return result;
}

    
} // $namespace
