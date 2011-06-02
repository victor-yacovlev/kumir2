#ifndef PAINTER_C_H
#define PAINTER_C_H

#include <wchar.h>

struct __array__ {
    int dim;
    int lefts[3];
    int rights[3];
    int sizes[3];
    void * data;
    char type;
};

namespace Shared { class VisualComponent; }

extern QString __get_error__ActorPainter();

extern "C" void __init__ActorPainter();
extern void __init2__ActorPainter(class QSettings *s, class QObject * parent);
extern class Shared::VisualComponent * __mainWidget__ActorPainter();

extern "C" void setPen__ActorPainter(int width, const wchar_t * ss);
extern "C" void setDensity__ActorPainter(int dens);
extern "C" void setBrush__ActorPainter(const wchar_t * ss);
extern "C" void splitColor_rgb__ActorPainter(const wchar_t *ss, int *r, int *g, int *b);
extern "C" void splitColor_cmyk__ActorPainter(const wchar_t *ss, int *c, int *m, int *y, int *k);
extern "C" void splitColor_hsl__ActorPainter(const wchar_t *ss, int *h, int *s, int *l);
extern "C" void splitColor_hsv__ActorPainter(const wchar_t *ss, int *h, int *s, int *v);
extern "C" void unsetBrush__ActorPainter();
extern "C" void pixel__ActorPainter(int x, int y, const wchar_t * ss);
extern "C" void moveTo__ActorPainter(int x, int y);
extern "C" void lineTo__ActorPainter(int x, int y);
extern "C" void line__ActorPainter(int x0, int y0, int x1, int y1);
extern "C" void drawRect__ActorPainter(int x0, int y0, int x1, int y1);
extern "C" void drawEllipse__ActorPainter(int x0, int y0, int x1, int y1);
extern "C" void drawCircle__ActorPainter(int xc, int yc, int r);
extern "C" void drawPolygon__ActorPainter(int N, struct __array__ xx, struct __array__ yy);
extern "C" void setFont__ActorPainter(const wchar_t * ss, int size, unsigned char bold, unsigned char italic);
extern "C" int textWidth__ActorPainter(const wchar_t * ss);
extern "C" void drawText__ActorPainter(int x, int y, const wchar_t * ss);
extern "C" void fill__ActorPainter(int x, int y);
extern "C" wchar_t* RGB__ActorPainter(int r, int g, int b);
extern "C" wchar_t* RGBA__ActorPainter(int r, int g, int b, int a);
extern "C" wchar_t* CMYK__ActorPainter(int c, int m, int y, int k);
extern "C" wchar_t* CMYKA__ActorPainter(int c, int m, int y, int k, int a);
extern "C" wchar_t* HSL__ActorPainter(int h, int s, int l);
extern "C" wchar_t* HSLA__ActorPainter(int h, int s, int l, int a);
extern "C" wchar_t* HSV__ActorPainter(int h, int s, int v);
extern "C" wchar_t* HSVA__ActorPainter(int h, int s, int v, int a);
extern "C" int pageWidth__ActorPainter();
extern "C" int pageHeight__ActorPainter();
extern "C" int pageCenterX__ActorPainter();
extern "C" int pageCenterY__ActorPainter();
extern "C" void newPage__ActorPainter(int w, int h, const wchar_t * bg);
extern "C" void loadPage__ActorPainter(const wchar_t * ss);
extern "C" void savePage__ActorPainter(const wchar_t * ss);
extern "C" wchar_t* pixelColor__ActorPainter(int x, int y);

#endif // PAINTER_C_H
