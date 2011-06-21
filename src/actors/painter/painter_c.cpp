#include <QtCore>
#include "painter_c.h"
#include "painterworker.h"
#include "stdlib/connector.h"
#include "stdlib/guirunner.h"
#include "stdlib/kumstdlib.h"

static wchar_t __error__ActorPainter[256];

using StdLib::Connector;

extern QString __get_error__ActorPainter()
{
    return QString::fromWCharArray(__error__ActorPainter);
}

static ActorPainter::PainterWorker * p;

extern "C" void __init__ActorPainter()
{
    __error__ActorPainter[0] = L'\0';
    if (__connected_to_kumir__()) {
        __reset_actor__("ActorPainter");
    }
    p->reset();
}

extern "C" void __create__ActorPainter()
{
    p = ActorPainter::PainterWorker::instance();
    bool connectToKumir = false;
    QString shmKey;
    foreach (const QString & arg, qApp->arguments()) {
        if (arg.startsWith("--attach=")) {
            shmKey = arg.mid(9);
            connectToKumir = true;
            break;
        }
    }
    if (connectToKumir) {
        __connect_to_kumir__(shmKey);
    }
    else {
        __create_window_for__(p->mainWidget(), p->menuActions());
    }
}


extern void __create2__ActorPainter(class QSettings *s, class QObject * parent)
{
    p = ActorPainter::PainterWorker::instance(s, parent);
}

extern class QWidget * __mainWidget__ActorPainter()
{
    return p->mainWidget();
}

extern QList<class QAction*> __menuActions__ActorPainter()
{
    return p->menuActions();
}

extern "C" void setPen__ActorPainter(int width, const wchar_t * ss)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("перо"),
                    QVariantList() << QString::fromWCharArray(ss)
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
    p->setPen(width, QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
}
}

extern "C" void setDensity__ActorPainter(int dens)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("плотность"),
                    QVariantList() << dens
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
    p->setDensity(dens);
    p->error().toWCharArray(__error__ActorPainter);
}
}

extern "C" void setBrush__ActorPainter(const wchar_t * ss)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("кисть"),
                    QVariantList() << QString::fromWCharArray(ss)
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
    p->setBrush(QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
}
}

extern "C" void splitColor_rgb__ActorPainter(const wchar_t *ss, int *r, int *g, int *b)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("разложить цвет в RGB"),
                    QVariantList() << QString::fromWCharArray(ss)
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        else {
            Q_ASSERT(resp.res.size()==3);
            *r = resp.res.at(0).toInt();
            *g = resp.res.at(1).toInt();
            *b = resp.res.at(2).toInt();
        }
    }
    else {
    QVariantList rr = p->splitColor("rgb", QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
    if (p->error().isEmpty()) {
        Q_ASSERT(rr.size()==3);
        *r = rr.at(0).toInt();
        *g = rr.at(1).toInt();
        *b = rr.at(2).toInt();
    }
}
}

extern "C" void splitColor_cmyk__ActorPainter(const wchar_t *ss, int *c, int *m, int *y, int *k)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("разложить цвет в CMYK"),
                    QVariantList() << QString::fromWCharArray(ss)
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        else {
            Q_ASSERT(resp.res.size()==4);
            *c = resp.res.at(0).toInt();
            *m = resp.res.at(1).toInt();
            *y = resp.res.at(2).toInt();
            *k = resp.res.at(3).toInt();
        }
    }
    else {
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
}
extern "C" void splitColor_hsl__ActorPainter(const wchar_t *ss, int *h, int *s, int *l)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("разложить цвет в HSL"),
                    QVariantList() << QString::fromWCharArray(ss)
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        else {
            Q_ASSERT(resp.res.size()==3);
            *h = resp.res.at(0).toInt();
            *s = resp.res.at(1).toInt();
            *l = resp.res.at(2).toInt();
        }
    }
    else {
    QVariantList rr = p->splitColor("hsl", QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
    if (p->error().isEmpty()) {
        Q_ASSERT(rr.size()==3);
        *h = rr.at(0).toInt();
        *s = rr.at(1).toInt();
        *l = rr.at(2).toInt();
    }
}
}
extern "C" void splitColor_hsv__ActorPainter(const wchar_t *ss, int *h, int *s, int *v)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("разложить цвет в HSV"),
                    QVariantList() << QString::fromWCharArray(ss)
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        else {
            Q_ASSERT(resp.res.size()==3);
            *h = resp.res.at(0).toInt();
            *s = resp.res.at(1).toInt();
            *v = resp.res.at(2).toInt();
        }
    }
    else {
    QVariantList rr = p->splitColor("hsv", QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
    if (p->error().isEmpty()) {
        Q_ASSERT(rr.size()==3);
        *h = rr.at(0).toInt();
        *s = rr.at(1).toInt();
        *v = rr.at(2).toInt();
    }
}
}
extern "C" void unsetBrush__ActorPainter()
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("убрать кисть"),
                    QVariantList()
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
    p->unsetBrush();
    p->error().toWCharArray(__error__ActorPainter);
}
}

extern "C" void pixel__ActorPainter(int x, int y, const wchar_t * ss)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("пиксель"),
                    QVariantList() << x << y << QString::fromWCharArray(ss)
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
    p->pixel(x,y,QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
}
}

extern "C" void moveTo__ActorPainter(int x, int y)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("в точку"),
                    QVariantList() << x << y
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
    p->moveTo(x, y);
    p->error().toWCharArray(__error__ActorPainter);
}
}

extern "C" void lineTo__ActorPainter(int x, int y)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("линия в точку"),
                    QVariantList() << x << y
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
    p->lineTo(x, y);
    p->error().toWCharArray(__error__ActorPainter);
}
}

extern "C" void line__ActorPainter(int x0, int y0, int x1, int y1)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("линия"),
                    QVariantList() << x0 << y0 << x1 << y1
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
    p->line(x0, y0, x1, y1);
    p->error().toWCharArray(__error__ActorPainter);
}
}

extern "C" void drawRect__ActorPainter(int x0, int y0, int x1, int y1)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("прямоугольник"),
                    QVariantList() << x0 << y0 << x1 << y1
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
    p->drawRect(x0, y0, x1, y1);
    p->error().toWCharArray(__error__ActorPainter);
}
}

extern "C" void drawEllipse__ActorPainter(int x0, int y0, int x1, int y1)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("эллипс"),
                    QVariantList() << x0 << y0 << x1 << y1
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
    p->drawEllipse(x0, y0, x1, y1);
    p->error().toWCharArray(__error__ActorPainter);
}
}

extern "C" void drawCircle__ActorPainter(int xc, int yc, int r)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("окружность"),
                    QVariantList() << xc << yc << r
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
    p->drawCircle(xc, yc, r);
    p->error().toWCharArray(__error__ActorPainter);
}
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
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("многоугольник"),
                    QVariantList() << N << QVariant(X) << QVariant(Y)
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
    p->drawPolygon(N, X, Y);
    p->error().toWCharArray(__error__ActorPainter);
}
}

extern "C" void setFont__ActorPainter(const wchar_t * ss, int size, unsigned char bold, unsigned char italic)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("шрифт"),
                    QVariantList() << QString::fromWCharArray(ss) << size << (bold>0? true : false) << (italic>0? true : false)
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
    p->setFont(QString::fromWCharArray(ss), size, bold>0? true : false, italic>0? true : false);
    p->error().toWCharArray(__error__ActorPainter);
}
}

extern "C" int textWidth__ActorPainter(const wchar_t * ss)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("ширина текста"),
                    QVariantList() << QString::fromWCharArray(ss)
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        return resp.result.toInt();
    }
    else {
    int result = p->textWidth(QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
    return result;
}
}

extern "C" void drawText__ActorPainter(int x, int y, const wchar_t * ss)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("надпись"),
                    QVariantList() << x << y << QString::fromWCharArray(ss)
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
    p->drawText(x,y,QString::fromWCharArray(ss));
    p->error().toWCharArray(__error__ActorPainter);
}
}

extern "C" void fill__ActorPainter(int x, int y)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("залить"),
                    QVariantList() << x << y
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
    p->fill(x, y);
    p->error().toWCharArray(__error__ActorPainter);
}
}

extern "C" wchar_t* RGB__ActorPainter(int r, int g, int b)
{
    QString result;
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("RGB"),
                    QVariantList() << r << g << b
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        result = resp.result.toString();
    }
    else {
    result = p->RGB(r,g,b);
    p->error().toWCharArray(__error__ActorPainter);
}
wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
result.toWCharArray(buffer);
buffer[result.size()] = L'\0';
return buffer;
}

extern "C" wchar_t* RGBA__ActorPainter(int r, int g, int b, int a)
{
    QString result;
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("RGBA"),
                    QVariantList() << r << g << b << a
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        result = resp.result.toString();
    }
    else {
        result = p->RGBA(r,g,b,a);
        p->error().toWCharArray(__error__ActorPainter);
    }

    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
    result.toWCharArray(buffer);
    buffer[result.size()] = L'\0';
    return buffer;
}

extern "C" wchar_t* CMYK__ActorPainter(int c, int m, int y, int k)
{
    QString result;
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("CMYK"),
                    QVariantList() << c << m << y << k
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        result = resp.result.toString();
    }
    else {
        result = p->CMYK(c,m,y,k);
        p->error().toWCharArray(__error__ActorPainter);
    }

    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
    result.toWCharArray(buffer);
    buffer[result.size()] = L'\0';
    return buffer;
}

extern "C" wchar_t* CMYKA__ActorPainter(int c, int m, int y, int k, int a)
{
    QString result;
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("CMYKA"),
                    QVariantList() << c << m << y << k << a
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        result = resp.result.toString();
    }
    else {
        result = p->CMYKA(c,m,y,k,a);
        p->error().toWCharArray(__error__ActorPainter);
    }

    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
    result.toWCharArray(buffer);
    buffer[result.size()] = L'\0';
    return buffer;
}

extern "C" wchar_t* HSL__ActorPainter(int h, int s, int l)
{
    QString result;
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("HSL"),
                    QVariantList() << h << s << l
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        result = resp.result.toString();
    }
    else {
        result = p->HSL(h,s,l);
        p->error().toWCharArray(__error__ActorPainter);
    }

    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
    result.toWCharArray(buffer);
    buffer[result.size()] = L'\0';
    return buffer;
}

extern "C" wchar_t* HSLA__ActorPainter(int h, int s, int l, int a)
{
    QString result;
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("HSLA"),
                    QVariantList() << h << s << l << a
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        result = resp.result.toString();
    }
    else {
        result = p->HSLA(h,s,l,a);
        p->error().toWCharArray(__error__ActorPainter);
    }

    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
    result.toWCharArray(buffer);
    buffer[result.size()] = L'\0';
    return buffer;
}

extern "C" wchar_t* HSV__ActorPainter(int h, int s, int v)
{
    QString result;
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("HSV"),
                    QVariantList() << h << s << v
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        result = resp.result.toString();
    }
    else {
        result = p->HSV(h,s,v);
        p->error().toWCharArray(__error__ActorPainter);
    }

    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));

    result.toWCharArray(buffer);
    buffer[result.size()] = L'\0';
    return buffer;
}

extern "C" wchar_t* HSVA__ActorPainter(int h, int s, int v, int a)
{
    QString result;
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("HSVA"),
                    QVariantList() << h << s << v << a
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        result = resp.result.toString();
    }
    else {
        result = p->HSVA(h,s,v,a);
        p->error().toWCharArray(__error__ActorPainter);
    }

    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
    result.toWCharArray(buffer);
    buffer[result.size()] = L'\0';
    return buffer;
}

extern "C" int pageWidth__ActorPainter()
{
    int result = 0;
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("ширина листа"),
                    QVariantList()
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        result = resp.result.toInt();
    }
    else {
        result = p->pageWidth();
        p->error().toWCharArray(__error__ActorPainter);
    }
    return result;
}

extern "C" int pageHeight__ActorPainter()
{
    int result = 0;
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("высота листа"),
                    QVariantList()
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        result = resp.result.toInt();
    }
    else {
        result = p->pageHeight();
        p->error().toWCharArray(__error__ActorPainter);
    }
    return result;
}

extern "C" int pageCenterX__ActorPainter()
{
    int result = 0;
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("центр x"),
                    QVariantList()
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        result = resp.result.toInt();
    }
    else {
        result = p->pageCenterX();
        p->error().toWCharArray(__error__ActorPainter);
    }
    return result;
}

extern "C" int pageCenterY__ActorPainter()
{
    int result = 0;
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("центр y"),
                    QVariantList()
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        result = resp.result.toInt();
    }
    else {
        result = p->pageCenterY();
        p->error().toWCharArray(__error__ActorPainter);
    }
    return result;
}

extern "C" void newPage__ActorPainter(int w, int h, const wchar_t * bg)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("новый лист"),
                    QVariantList() << w << h << QString::fromWCharArray(bg)
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
        p->newPage(w,h,QString::fromWCharArray(bg));
        p->error().toWCharArray(__error__ActorPainter);
    }
}

extern "C" void loadPage__ActorPainter(const wchar_t * ss)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("загрузить лист"),
                    QVariantList() << QString::fromWCharArray(ss)
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
        p->loadPage(QString::fromWCharArray(ss));
        p->error().toWCharArray(__error__ActorPainter);
    }
}

extern "C" void savePage__ActorPainter(const wchar_t * ss)
{
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("сохранить лист"),
                    QVariantList() << QString::fromWCharArray(ss)
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
    }
    else {
        p->savePage(QString::fromWCharArray(ss));
        p->error().toWCharArray(__error__ActorPainter);
    }
}


extern "C" wchar_t* pixelColor__ActorPainter(int x, int y)
{
    QString result;
    if (__connected_to_kumir__()) {
        ActorResponse resp = __run_actor_command__(
                    "ActorPainter",
                    QString::fromUtf8("цвет в точке"),
                    QVariantList() << x << y
                    );
        resp.error.toWCharArray(__error__ActorPainter);
        if (!resp.error.isEmpty()) {
            __abort__(__error__ActorPainter, -1);
        }
        result = resp.result.toString();
    }
    else {
        result = p->pixelColor(x, y);
        p->error().toWCharArray(__error__ActorPainter);
    }
    wchar_t * buffer = (wchar_t*)calloc(result.size()+1, sizeof(wchar_t));
    buffer[result.size()] = L'\0';
    return buffer;
}
