#include <QtCore>
#include "painter_c.h"
#include "painterworker.h"

static wchar_t __error__ActorPainter[256];

extern QString __get_error__ActorPainter()
{
    return QString::fromWCharArray(__error__ActorPainter);
}

static ActorPainter::PainterWorker * p;

extern "C" void __init__ActorPainter()
{
    __error__ActorPainter[0] = L'\0';
    p = ActorPainter::PainterWorker::instance();
    p->reset();
}

extern void __init__ActorPainter2(class QSettings *s, class QObject * parent)
{
    __error__ActorPainter[0] = L'\0';
    p = ActorPainter::PainterWorker::instance(s, parent);
    p->reset();
}

extern class Shared::VisualComponent * __mainWidget__ActorPainter()
{
    return p->mainWidget();
}

extern "C" void setPen__ActorPainter(int width, const wchar_t * ss)
{
    p->setPen(width, QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" void setDensity__ActorPainter(int dens)
{
    p->setDensity(dens);
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" void setBrush__ActorPainter(const wchar_t * ss)
{
    p->setBrush(QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" void splitColor_rgb__ActorPainter(const wchar_t *ss, int *r, int *g, int *b)
{
    QVariantList rr = p->splitColor("rgb", QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
    if (p->error().isEmpty()) {
        Q_ASSERT(rr.size()==3);
        *r = rr.at(0).toInt();
        *g = rr.at(1).toInt();
        *b = rr.at(2).toInt();
    }
}

extern "C" void splitColor_cmyk__ActorPainter(const wchar_t *ss, int *c, int *m, int *y, int *k)
{
    QVariantList rr = p->splitColor("cmyk", QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
    if (p->error().isEmpty()) {
        Q_ASSERT(rr.size()==4);
        *c = rr.at(0).toInt();
        *m = rr.at(1).toInt();
        *y = rr.at(2).toInt();
        *k = rr.at(3).toInt();
    }
}
extern "C" void splitColor_hsl__ActorPainter(const wchar_t *ss, int *h, int *s, int *l)
{
    QVariantList rr = p->splitColor("hsl", QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
    if (p->error().isEmpty()) {
        Q_ASSERT(rr.size()==3);
        *h = rr.at(0).toInt();
        *s = rr.at(1).toInt();
        *l = rr.at(2).toInt();
    }
}
extern "C" void splitColor_hsv__ActorPainter(const wchar_t *ss, int *h, int *s, int *v)
{
    QVariantList rr = p->splitColor("hsv", QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
    if (p->error().isEmpty()) {
        Q_ASSERT(rr.size()==3);
        *h = rr.at(0).toInt();
        *s = rr.at(1).toInt();
        *v = rr.at(2).toInt();
    }
}
extern "C" void unsetBrush__ActorPainter()
{
    p->unsetBrush();
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" void pixel__ActorPainter(int x, int y, const wchar_t * ss)
{
    p->pixel(x,y,QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" void moveTo__ActorPainter(int x, int y)
{
    p->moveTo(x, y);
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" void lineTo__ActorPainter(int x, int y)
{
    p->lineTo(x, y);
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" void line__ActorPainter(int x0, int y0, int x1, int y1)
{
    p->line(x0, y0, x1, y1);
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" void drawRect__ActorPainter(int x0, int y0, int x1, int y1)
{
    p->drawRect(x0, y0, x1, y1);
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" void drawEllipse__ActorPainter(int x0, int y0, int x1, int y1)
{
    p->drawEllipse(x0, y0, x1, y1);
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" void drawCircle__ActorPainter(int xc, int yc, int r)
{
    p->drawCircle(xc, yc, r);
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" void drawPolygon__ActorPainter(int N, struct __array__ xx, struct __array__ yy)
{
    QVariantList X;
    QVariantList Y;
    Q_ASSERT(xx.dim==1);
    Q_ASSERT(yy.dim==1);
    Q_ASSERT(xx.type=='i');
    Q_ASSERT(yy.type=='i');
    int * x = static_cast<int*>(xx.data);
    int * y = static_cast<int*>(yy.data);
    for (int i=0; i<xx.sizes[0]; i++) {
        X << x[i];
    }
    for (int i=0; i<yy.sizes[0]; i++) {
        Y << y[i];
    }
    p->drawPolygon(N, X, Y);
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" void setFont__ActorPainter(const wchar_t * ss, int size, unsigned char bold, unsigned char italic)
{
    p->setFont(QString::fromWCharArray(ss), size, bold>0? true : false, italic>0? true : false);
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" int textWidth__ActorPainter(const wchar_t * ss)
{
    int result = p->textWidth(QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
    return result;
}

extern "C" void drawText__ActorPainter(int x, int y, const wchar_t * ss)
{
    p->drawText(x,y,QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" void fill__ActorPainter(int x, int y)
{
    p->fill(x, y);
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" wchar_t* RGB__ActorPainter(int r, int g, int b)
{
    QString result = p->RGB(r,g,b);
    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
    result.toWCharArray(buffer);
    buffer[result.size()] = L'\0';
    p->error().toWCharArray(__error__ActorPainter);
    return buffer;
}

extern "C" wchar_t* RGBA__ActorPainter(int r, int g, int b, int a)
{
    QString result = p->RGBA(r,g,b,a);
    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
    result.toWCharArray(buffer);
    buffer[result.size()] = L'\0';
    p->error().toWCharArray(__error__ActorPainter);
    return buffer;
}

extern "C" wchar_t* CMYK__ActorPainter(int c, int m, int y, int k)
{
    QString result = p->CMYK(c,m,y,k);
    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
    result.toWCharArray(buffer);
    buffer[result.size()] = L'\0';
    p->error().toWCharArray(__error__ActorPainter);
    return buffer;
}

extern "C" wchar_t* CMYKA__ActorPainter(int c, int m, int y, int k, int a)
{
    QString result = p->CMYKA(c,m,y,k,a);
    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
    result.toWCharArray(buffer);
    buffer[result.size()] = L'\0';
    p->error().toWCharArray(__error__ActorPainter);
    return buffer;
}

extern "C" wchar_t* HSL__ActorPainter(int h, int s, int l)
{
    QString result = p->HSL(h,s,l);
    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
    result.toWCharArray(buffer);
    buffer[result.size()] = L'\0';
    p->error().toWCharArray(__error__ActorPainter);
    return buffer;
}

extern "C" wchar_t* HSLA__ActorPainter(int h, int s, int l, int a)
{
    QString result = p->HSLA(h,s,l,a);
    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
    result.toWCharArray(buffer);
    buffer[result.size()] = L'\0';
    p->error().toWCharArray(__error__ActorPainter);
    return buffer;
}

extern "C" wchar_t* HSV__ActorPainter(int h, int s, int v)
{
    QString result = p->HSV(h,s,v);
    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
    result.toWCharArray(buffer);
    buffer[result.size()] = L'\0';
    p->error().toWCharArray(__error__ActorPainter);
    return buffer;
}

extern "C" wchar_t* HSVA__ActorPainter(int h, int s, int v, int a)
{
    QString result = p->HSVA(h,s,v,a);
    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
    result.toWCharArray(buffer);
    buffer[result.size()] = L'\0';
    p->error().toWCharArray(__error__ActorPainter);
    return buffer;
}

extern "C" int pageWidth__ActorPainter()
{
    int result = p->pageWidth();
    p->error().toWCharArray(__error__ActorPainter);
    return result;
}

extern "C" int pageHeight__ActorPainter()
{
    int result = p->pageHeight();
    p->error().toWCharArray(__error__ActorPainter);
    return result;
}

extern "C" int pageCenterX__ActorPainter()
{
    int result = p->pageCenterX();
    p->error().toWCharArray(__error__ActorPainter);
    return result;
}

extern "C" int pageCenterY__ActorPainter()
{
    int result = p->pageCenterY();
    p->error().toWCharArray(__error__ActorPainter);
    return result;
}

extern "C" void newPage__ActorPainter(int w, int h, const wchar_t * bg)
{
    p->newPage(w,h,QString::fromWCharArray(bg));
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" void loadPage__ActorPainter(const wchar_t * ss)
{
    p->loadPage(QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
}

extern "C" void savePage__ActorPainter(const wchar_t * ss)
{
    p->savePage(QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
}


extern "C" wchar_t* pixelColor__ActorPainter(int x, int y)
{
    QString result = p->pixelColor(x, y);
    p->error().toWCharArray(__error__ActorPainter);
    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
    buffer[result.size()] = L'\0';
    return buffer;
}
