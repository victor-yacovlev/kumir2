#include "painterworker.h"
#include "painterwindow.h"
#include "painterview.h"
#include "paintertools.h"


namespace ActorPainter {

PainterWorker::PainterWorker(QSettings * settings, QObject * parent)
    : QObject(parent)
{
    originalCanvas = new QImage(QSize(640,480), QImage::Format_RGB32);
    originalCanvas->fill(QColor("white").rgb());
    canvas = new QImage(QSize(640,480), QImage::Format_RGB32);
    canvas->fill(QColor("white").rgb());
    m_mainWidget = new PainterWindow(settings);
    canvasLock = new QMutex;
    m_mainWidget->setCanvas(canvas, canvasLock);
    view = m_mainWidget->view();
    connect(m_mainWidget, SIGNAL(newImageRequest(int,int,QString,bool,QString)),
            this, SLOT(handleNewImageRequest(int,int,QString,bool,QString)));
    connect(m_mainWidget,SIGNAL(loadImageRequest(QString)),
            this, SLOT(handleLoadImageRequest(QString)));
    connect(m_mainWidget,SIGNAL(resetRequest()),
            this, SLOT(reset()));
    const QString resourcesRoot = QApplication::instance()->property("sharePath").toString();
    if (QFile::exists(resourcesRoot+"/actors/painter/default.png")) {
        QSettings * s = NULL;
        if (QFile::exists(resourcesRoot+"/actors/painter/templates.ini")) {
            s = new QSettings(resourcesRoot+"/actors/painter/templates.ini", QSettings::IniFormat);
            s->setIniCodec("UTF-8");
            templateName = s->value("Names/default.png", "[new page]").toString();
            s->deleteLater();

        }
        handleLoadImageRequest(resourcesRoot+"/actors/painter/default.png");
    }
    m_instance = this;
}

void PainterWorker::handleNewImageRequest(int w, int h, const QString &bg, bool useTemplate, const QString &fileName)
{
    const QString resourcesRoot = QApplication::instance()->property("sharePath").toString();
    if (useTemplate) {
        handleLoadImageRequest(resourcesRoot+"/actors/painter/"+fileName);
    }
    else {
        QColor c = parceColor(bg);
        QImage * oldCanvas = canvas;
        QImage * oldCanvas2 = originalCanvas;
        canvas = new QImage(w,h,QImage::Format_RGB32);
        canvas->fill(c.rgb());
        originalCanvas = new QImage(w,h,QImage::Format_RGB32);
        originalCanvas->fill(c.rgb());
        reset();
        m_mainWidget->setCanvas(canvas, canvasLock);
        delete oldCanvas;
        delete oldCanvas2;
    }
}

void PainterWorker::handleLoadImageRequest(const QString &fileName)
{
    QImage * oldCanvas = canvas;
    QImage * oldCanvas2 = originalCanvas;
    canvas = new QImage(fileName);
    originalCanvas = new QImage(fileName);
    if (m_mainWidget)
        m_mainWidget->setCanvas(canvas, canvasLock);
    delete oldCanvas;
    delete oldCanvas2;
}

QWidget * PainterWorker::mainWidget()
{
    return m_mainWidget;
}

void PainterWorker::drawEllipse(int x0, int y0, int x1, int y1)
{
    canvasLock->lock();
    QPainter p(canvas);
    p.setPen(pen);
    p.setBrush(brush);
    p.drawEllipse(x0, y0, x1-x0, y1-y0);
    canvasLock->unlock();
    if (view)
        view->update();
}

void PainterWorker::drawLine(int x0, int y0, int x1, int y1)
{
    canvasLock->lock();
    QPainter p(canvas);
    p.setPen(pen);
    p.setBrush(brush);
    p.drawLine(x0,y0,x1,y1);
    canvasLock->unlock();
    p.end();
    if (view)
        view->update();

}

void PainterWorker::drawPoint(int x, int y, const QColor &color)
{
    canvasLock->lock();
    QPainter p(canvas);
    p.setPen(QPen(color));
//    p.setBrush(brush);
    p.drawPoint(x,y);
    canvasLock->unlock();
    if (view)
        view->update();
}

void PainterWorker::drawPolygon(const QVector<QPoint> &points)
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

void PainterWorker::drawText(int x, int y, const QString &text)
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

void PainterWorker::fill(int x, int y)
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

void PainterWorker::setPen(int width, const QString &cs)
{
    QColor c = parceColor(cs.trimmed());
    if (cs.trimmed().isEmpty() || width<1) {
        pen = Qt::NoPen;
    }
    else {
        pen = QPen(c, width);
    }
}

void PainterWorker::setDensity(int dens)
{
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

void PainterWorker::setBrush(const QString &cs)
{
    QColor c = parceColor(cs);
    if (cs.trimmed().isEmpty())
        transparent = true;
    else
        transparent = false;
    if (!transparent && !c.isValid()) {
        lastError = tr("Invalid color");
    }
    brush.setColor(c);
    if (transparent)
        brush.setStyle(Qt::NoBrush);
    else
        brush.setStyle(style);
}

QVariantList PainterWorker::splitColor(const QString &model, const QString &cs)
{
    QVariantList result;
    QColor c = parceColor(cs);
    if (c.isValid()) {
        if (model=="rgb") {
            int R, G, B;
            R = c.red();
            G = c.green();
            B = c.blue();
            result << R << G << B;
        }
        else if (model=="cmyk") {
            int C, M, Y, K;
            C = c.cyan();
            M = c.magenta();
            Y = c.yellow();
            K = c.black();
            result << C << M << Y << K;
        }
        else if (model=="hsl") {
            int H, S, L;
            H = c.hue();
            S = c.saturation();
            L = c.lightness();
            result << H << S << L;
        }
        else if (model=="hsv") {
            int H, S, V;
            H = c.hue();
            S = c.saturation();
            V = c.value();
            result << H << S << V;
        }
    }
    else {
        lastError = tr("Invalid color");
    }
    return result;
}

void PainterWorker::unsetBrush()
{
    brush.setStyle(Qt::NoBrush);
    transparent = true;
}

void PainterWorker::pixel(int x, int y, const QString &cs)
{
    if (!cs.isEmpty()) {
        QColor c = parceColor(cs);
        if (c.isValid())
            drawPoint(x,y,c);
        else
            lastError = tr("Invalid color");
    }
}

void PainterWorker::moveTo(int x, int y)
{
    point.setX(x);
    point.setY(y);
}

void PainterWorker::lineTo(int x1, int y1)
{
    drawLine(point.x(), point.y(), x1, y1);
    point.setX(x1);
    point.setY(y1);
}

void PainterWorker::line(int x0, int y0, int x1, int y1)
{
    drawLine(x0, y0, x1, y1);
    point.setX(x1);
    point.setY(y1);
}

void PainterWorker::drawRect(int x0, int y0, int x1, int y1)
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

void PainterWorker::drawCircle(int xc, int yc, int r)
{
    int x0 = xc-r;
    int y0 = yc-r;
    int x1 = xc+r;
    int y1 = yc+r;
    drawEllipse(x0,y0,x1,y1);
}

void PainterWorker::drawPolygon(int N, const QList<QVariant> &xx, const QList<QVariant> &yy)
{
    QVector<QPoint> points(N);
    if (N!=xx.length() || N!=yy.length()) {
        lastError = tr("Invalid points number");
    }
    else {
        for (int i=0; i<N; i++) {
            points[i] = QPoint(xx[i].toInt(), yy[i].toInt());
        }
        drawPolygon(points);
    }
}

void PainterWorker::setFont(const QString &family, int size, bool bold, bool italic)
{
    if (!family.isEmpty())
        font.setFamily(family);
    if (size>0)
        font.setPixelSize(size);
    font.setBold(bold);
    font.setItalic(italic);
}

int PainterWorker::textWidth(const QString &text)
{
    const QFontMetrics fm(font);
    return fm.width(text);
}

int PainterWorker::pageWidth()
{
    return canvas->width();
}

int PainterWorker::pageHeight()
{
    return canvas->height();
}

void PainterWorker::newPage(int w, int h, const QString &bg)
{
    if (w<1 || h<1) {
        lastError = tr("Page size is too small");
    }
    else if (w>3000 || h>3000) {
        lastError = tr("Page size is too large (max. 3000 px)");
    }
    else {
        handleNewImageRequest(w, h, bg, false, "");
    }
}

void PainterWorker::loadPage(const QString &fileName)
{
    if (!QFile::exists(fileName)) {
        lastError = tr("File not found");
    }
    else {
        handleLoadImageRequest(fileName);
    }
}

void PainterWorker::savePage(const QString &fileName)
{
    QFile f(fileName);
    if (f.open(QIODevice::WriteOnly)) {
        canvas->save(&f,"PNG");
        f.close();
    }
    else {
        lastError = tr("Can't open file for writing");
    }
}

QString PainterWorker::pixelColor(int x, int y)
{
    if (x<0 || y<0) {
        lastError = QString::fromUtf8("Coordinate is less than 0");
        return "";
    }
    else if (x>=canvas->width() || y>=canvas->height()) {
        lastError = QString::fromUtf8("Coordinate is out of page");
        return "";
    }
    else {
        QColor c = QColor::fromRgb(canvas->pixel(x,y));
        return c.name();
    }
}

void PainterWorker::reset()
{
    QImage * del = canvas;
    lastError = "";

    point = QPoint(0,0);
    font = QFont();
    brush = QBrush();
    style = Qt::SolidPattern;
    transparent = false;
    pen = QPen();
    canvasLock->lock();
    canvas = new QImage(originalCanvas->copy());
    m_mainWidget->setCanvas(canvas, canvasLock);
    canvasLock->unlock();
    if (view)
        view->update();
    delete del;
}

PainterWorker* PainterWorker::m_instance = 0;
PainterWorker* PainterWorker::instance()
{
    if (!m_instance)
        m_instance = new PainterWorker(0, 0);
    return m_instance;
}

PainterWorker* PainterWorker::instance(QSettings * settings, QObject * parent)
{
    if (!m_instance)
        m_instance = new PainterWorker(settings, parent);
    else {
        m_instance->setParent(parent);
    }
    return m_instance;
}

} // namespace ActorPainter
