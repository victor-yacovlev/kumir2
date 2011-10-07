#include "painterplugin.h"
#include "painter_c.h"
#include "stdlib/kumstdlib.h"



namespace ActorPainter {

PainterPlugin::PainterPlugin() :
    KPlugin()
{
    m_mainWidget = 0;
    __create2__ActorPainter(mySettings(), this);
}

QList<Alg> PainterPlugin::funcList() const
{
    QList<Alg> result;
    Alg a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг перо(цел толщина, лит цвет)");
    a.cHeader = "void setPen__ActorPainter(int width, const wchar_t * ss)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг кисть(лит цвет)");
    a.cHeader = "void setBrush__ActorPainter(const wchar_t * ss)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг плотность(цел пп)");
    a.cHeader = "void setDensity__ActorPainter(int dens)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг убрать кисть");
    a.cHeader = "void unsetBrush__ActorPainter()";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг пиксель(цел х, у, лит цвет)");
    a.cHeader = "void pixel__ActorPainter(int x, int y, const wchar_t * ss)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг в точку(цел х, у)");
    a.cHeader = "void setPen__ActorPainter(int width, const wchar_t * ss)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг линия в точку(цел х, у)");
    a.cHeader = "void lineTo__ActorPainter(int x, int y)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг линия(цел х0, у0, х1, у1)");
    a.cHeader = "void line__ActorPainter(int x0, int y0, int x1, int y1)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг прямоугольник(цел х0, у0, х1, у1)");
    a.cHeader = "void drawRect__ActorPainter(int x0, int y0, int x1, int y1)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг эллипс(цел х0, у0, х1, у1)");
    a.cHeader = "void drawEllipse__ActorPainter(int x0, int y0, int x1, int y1)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг окружность(цел х, у, р)");
    a.cHeader = "void drawCircle__ActorPainter(int xc, int yc, int r)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг многоугольник(цел чт, целтаб хх[1:чт], yy[1:чт])");
    a.cHeader = "void drawPolygon__ActorPainter(int N, struct __array__ xx, struct __array__ yy)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг шрифт(лит название, цел размер, лог жирный, лог курсив)");
    a.cHeader = "void setFont__ActorPainter(const wchar_t * ss, int size, unsigned char bold, unsigned char italic)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг цел ширина текста(лит текст)");
    a.cHeader = "int textWidth__ActorPainter(const wchar_t * ss)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг надпись(цел х, у, лит текст)");
    a.cHeader = "void drawText__ActorPainter(int x, int y, const wchar_t * ss)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг залить(цел х, у)");
    a.cHeader = "void fill__ActorPainter(int x, int y)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг лит RGB(цел r, g, b)");
    a.cHeader = "wchar_t* RGB__ActorPainter(int r, int g, int b)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг лит RGBA(цел r, g, b, a)");
    a.cHeader = "wchar_t* RGBA__ActorPainter(int r, int g, int b, int a)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг лит CMYK(цел c, m, y, k)");
    a.cHeader = "wchar_t* CMYK__ActorPainter(int c, int m, int y, int k)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг лит CMYKA(цел c, m, y, k, a)");
    a.cHeader = "wchar_t* CMYKA__ActorPainter(int c, int m, int y, int k, int a)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг лит HSL(цел h, s, l)");
    a.cHeader = "wchar_t* HSL__ActorPainter(int h, int s, int l)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг лит HSLA(цел h, s, l, a)");
    a.cHeader = "wchar_t* HSLA__ActorPainter(int h, int s, int l, int a)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг лит HSV(цел h, s, v)");
    a.cHeader = "wchar_t* HSV__ActorPainter(int h, int s, int v)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг лит HSVA(цел h, s, v, a)");
    a.cHeader = "wchar_t* HSVA__ActorPainter(int h, int s, int v, int a)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг цел ширина листа");
    a.cHeader = "int pageWidth__ActorPainter()";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг цел высота листа");
    a.cHeader = "int pageHeight__ActorPainter()";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг цел центр x");
    a.cHeader = "int pageCenterX__ActorPainter()";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг цел центр y");
    a.cHeader = "int pageCenterY__ActorPainter()";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг лит цвет в точке(цел х, у)");
    a.cHeader = "wchar_t* pixelColor__ActorPainter(int x, int y)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг новый лист(цел ширина, высота, лит цвет фона)");
    a.cHeader = "void newPage__ActorPainter(int w, int h, const wchar_t * bg)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг загрузить лист(лит имя файла)");
    a.cHeader = "void loadPage__ActorPainter(const wchar_t * ss)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг сохранить лист(лит имя файла)");
    a.cHeader = "void savePage__ActorPainter(const wchar_t * ss)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг разложить цвет в RGB(лит цвет, рез цел r, g, b)");
    a.cHeader = "void splitColor_rgb__ActorPainter(const wchar_t *ss, int *r, int *g, int *b)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг разложить цвет в CMYK(лит цвет, рез цел c, m, y, k)");
    a.cHeader = "void splitColor_cmyk__ActorPainter(const wchar_t *ss, int *c, int *m, int *y, int *k)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг разложить цвет в HSL(лит цвет, рез цел h, s, l)");
    a.cHeader = "void splitColor_hsl__ActorPainter(const wchar_t *ss, int *h, int *s, int *l)";
    result << a;

    a.id = result.size();
    a.kumirHeader = QString::fromUtf8("алг разложить цвет в HSV(лит цвет, рез цел h, s, v)");
    a.cHeader = "void splitColor_hsv__ActorPainter(const wchar_t *ss, int *h, int *s, int *v)";
    result << a;

    return result;
}

QString PainterPlugin::name(ProgrammingLanguage pl, QLocale::Language nl) const
{
    if (pl==PL_Kumir) {
        return nl==QLocale::Russian? QString::fromUtf8("Рисователь") : "Painter";
    }
    else {
        return "ActorPainter";
    }
}

QString PainterPlugin::errorText() const
{
    return __get_error__ActorPainter();
}

void PainterPlugin::runFunct(const QString &alg, const QList<QVariant> &params)
{
    l_optResults.clear();
    if (alg==QString::fromUtf8("перо")) {
        wchar_t * buffer = (wchar_t*)calloc(params[1].toString().length()+1, sizeof(wchar_t));
        params[1].toString().toWCharArray(buffer);
        buffer[params[1].toString().length()] = L'\0';
        setPen__ActorPainter(params[0].toInt(), buffer);
        free(buffer);
    }
    else if (alg==QString::fromUtf8("плотность")) {
        setDensity__ActorPainter(params[0].toInt());
    }
    else if (alg==QString::fromUtf8("кисть")) {
        wchar_t * buffer = (wchar_t*)calloc(params[0].toString().length()+1, sizeof(wchar_t));
        params[0].toString().toWCharArray(buffer);
        buffer[params[0].toString().length()] = L'\0';
        setBrush__ActorPainter(buffer);
        free(buffer);
    }
    else if (alg.startsWith(QString::fromUtf8("разложить цвет в "))) {
        const QString model = alg.mid(17).trimmed().toLower();
        const QString cs = params[0].toString().trimmed();
        wchar_t * buffer = (wchar_t*)calloc(params[0].toString().length()+1, sizeof(wchar_t));
        params[0].toString().toWCharArray(buffer);
        buffer[params[0].toString().length()] = L'\0';
        if (model=="rgb") {
            int r, g, b;
            splitColor_rgb__ActorPainter(buffer, &r, &g, &b);
            l_optResults << r << g << b;
        }
        else if (model=="cmyk") {
            int c, m, y, k;
            splitColor_cmyk__ActorPainter(buffer, &c, &m, &y, &k);
            l_optResults << c << m << y << k;
        }
        else if (model=="hsl") {
            int h, s, l;
            splitColor_hsl__ActorPainter(buffer, &h, &s, &l);
            l_optResults << h << s << l;
        }
        else if (model=="hsv") {
            int h, s, v;
            splitColor_hsv__ActorPainter(buffer, &h, &s, &v);
            l_optResults << h << s << v;
        }
        free(buffer);
    }
    else if (alg==QString::fromUtf8("убрать кисть")) {
        unsetBrush__ActorPainter();
    }
    else if (alg==QString::fromUtf8("пиксель")) {
        wchar_t * buffer = (wchar_t*)calloc(params[2].toString().length()+1, sizeof(wchar_t));
        params[2].toString().toWCharArray(buffer);
        buffer[params[2].toString().length()] = L'\0';
        pixel__ActorPainter(params[0].toInt(), params[1].toInt(), buffer);
        free(buffer);
    }
    else if (alg==QString::fromUtf8("в точку")) {
        moveTo__ActorPainter(params[0].toInt(), params[1].toInt());
    }
    else if (alg==QString::fromUtf8("линия в точку")) {
        lineTo__ActorPainter(params[0].toInt(), params[1].toInt());
    }
    else if (alg==QString::fromUtf8("линия")) {
        line__ActorPainter(params[0].toInt(), params[1].toInt(), params[2].toInt(), params[3].toInt());
    }
    else if (alg==QString::fromUtf8("прямоугольник")) {
        drawRect__ActorPainter(params[0].toInt(), params[1].toInt(), params[2].toInt(), params[3].toInt());
    }
    else if (alg==QString::fromUtf8("эллипс")) {
        drawEllipse__ActorPainter(params[0].toInt(), params[1].toInt(), params[2].toInt(), params[3].toInt());
    }
    else if (alg==QString::fromUtf8("окружность")) {
        drawCircle__ActorPainter(params[0].toInt(), params[1].toInt(), params[2].toInt());
    }
    else if (alg==QString::fromUtf8("многоугольник")) {
        QVariantList xx = params[1].toList();
        QVariantList yy = params[2].toList();
        struct __array__ X;
        struct __array__ Y;
        X.dim = Y.dim = 1;
        X.type = Y.type = 'i';
        X.lefts[0] = Y.lefts[0] = 0;
        X.sizes[0] = xx.size();
        Y.sizes[0] = yy.size();
        X.data = calloc(X.sizes[0], sizeof(int));
        Y.data = calloc(Y.sizes[0], sizeof(int));
        int * Xdata = (int*)(X.data);
        int * Ydata = (int*)(Y.data);
        for (int i=0; i<xx.size(); i++) {
            Xdata[i] = xx.at(i).toInt();
        }
        for (int i=0; i<yy.size(); i++) {
            Ydata[i] = yy.at(i).toInt();
        }
        drawPolygon__ActorPainter(params[0].toInt(), X, Y);
        free(Xdata);
        free(Ydata);
    }
    else if (alg==QString::fromUtf8("шрифт")) {
        wchar_t * buffer = (wchar_t*)calloc(params[0].toString().length()+1, sizeof(wchar_t));
        params[0].toString().toWCharArray(buffer);
        buffer[params[0].toString().length()] = L'\0';
        setFont__ActorPainter(buffer,
                              params[1].toInt(),
                              params[2].toBool()? 1 : 0,
                              params[3].toBool()? 1 : 0);
        free(buffer);
    }
    else if (alg==QString::fromUtf8("ширина текста")) {
        wchar_t * buffer = (wchar_t*)calloc(params[0].toString().length()+1, sizeof(wchar_t));
        params[0].toString().toWCharArray(buffer);
        buffer[params[0].toString().length()] = L'\0';
        v_result = textWidth__ActorPainter(buffer);
        free(buffer);
    }
    else if (alg==QString::fromUtf8("надпись")) {
        wchar_t * buffer = (wchar_t*)calloc(params[2].toString().length()+1, sizeof(wchar_t));
        params[2].toString().toWCharArray(buffer);
        buffer[params[2].toString().length()] = L'\0';
        drawText__ActorPainter(params[0].toInt(), params[1].toInt(), buffer);
        free(buffer);
    }
    else if (alg==QString::fromUtf8("залить")) {
        fill__ActorPainter(params[0].toInt(), params[1].toInt());
    }
    else if (alg=="RGB") {
        wchar_t * buffer = RGB__ActorPainter(params[0].toInt(), params[1].toInt(), params[2].toInt());
        v_result = QString::fromWCharArray(buffer);
        free(buffer);
    }
    else if (alg=="RGBA") {
        wchar_t * buffer = RGBA__ActorPainter(params[0].toInt(), params[1].toInt(), params[2].toInt(), params[3].toInt());
        v_result = QString::fromWCharArray(buffer);
        free(buffer);
    }
    else if (alg=="CMYK") {
        wchar_t * buffer = CMYK__ActorPainter(params[0].toInt(), params[1].toInt(), params[2].toInt(), params[3].toInt());
        v_result = QString::fromWCharArray(buffer);
        free(buffer);
    }
    else if (alg=="CMYKA") {
        wchar_t * buffer = CMYKA__ActorPainter(params[0].toInt(), params[1].toInt(), params[2].toInt(), params[3].toInt(), params[4].toInt());
        v_result = QString::fromWCharArray(buffer);
        free(buffer);
    }
    else if (alg=="HSL") {
        wchar_t * buffer = HSL__ActorPainter(params[0].toInt(), params[1].toInt(), params[2].toInt());
        v_result = QString::fromWCharArray(buffer);
        free(buffer);
    }
    else if (alg=="HSLA") {
        wchar_t * buffer = HSLA__ActorPainter(params[0].toInt(), params[1].toInt(), params[2].toInt(), params[3].toInt());
        v_result = QString::fromWCharArray(buffer);
        free(buffer);
    }
    else if (alg=="HSV") {
        wchar_t * buffer = HSV__ActorPainter(params[0].toInt(), params[1].toInt(), params[2].toInt());
        v_result = QString::fromWCharArray(buffer);
        free(buffer);
    }
    else if (alg=="HSVA") {
        wchar_t * buffer = HSVA__ActorPainter(params[0].toInt(), params[1].toInt(), params[2].toInt(), params[3].toInt());
        v_result = QString::fromWCharArray(buffer);
        free(buffer);
    }
    else if (alg==QString::fromUtf8("ширина листа")) {
        v_result = pageWidth__ActorPainter();
    }
    else if (alg==QString::fromUtf8("высота листа")) {
        v_result = pageHeight__ActorPainter();
    }
    else if (alg==QString::fromUtf8("центр x")) {
        v_result = pageCenterX__ActorPainter();
    }
    else if (alg==QString::fromUtf8("центр y")) {
        v_result = pageCenterY__ActorPainter();
    }
    else if (alg==QString::fromUtf8("новый лист")) {
        wchar_t * buffer = (wchar_t*)calloc(params[2].toString().length()+1, sizeof(wchar_t));
        params[2].toString().toWCharArray(buffer);
        buffer[params[2].toString().length()] = L'\0';
        newPage__ActorPainter(params[0].toInt(), params[1].toInt(), buffer);
        free(buffer);
    }
    else if (alg==QString::fromUtf8("загрузить лист")) {
        wchar_t * buffer = (wchar_t*)calloc(params[0].toString().length()+1, sizeof(wchar_t));
        params[0].toString().toWCharArray(buffer);
        buffer[params[0].toString().length()] = L'\0';
        loadPage__ActorPainter(buffer);
        free(buffer);
    }
    else if (alg==QString::fromUtf8("сохранить лист")) {
        wchar_t * buffer = (wchar_t*)calloc(params[0].toString().length()+1, sizeof(wchar_t));
        params[0].toString().toWCharArray(buffer);
        buffer[params[0].toString().length()] = L'\0';
        savePage__ActorPainter(buffer);
        free(buffer);
    }
    else if (alg=="цвет в точке") {
        wchar_t * buffer = pixelColor__ActorPainter(params[0].toInt(), params[1].toInt());
        v_result = QString::fromWCharArray(buffer);
        free(buffer);
    }
    else {
        qDebug() << "Unknown method: %1" << alg;
    }
    emit sync();
}

void PainterPlugin::reset()
{
    __init__ActorPainter();
}

QString PainterPlugin::initialize(const QStringList &)
{
    return "";
}

void PainterPlugin::start()
{
    // TODO implement standalone actor implementation
}

void PainterPlugin::stop()
{

}

QWidget * PainterPlugin::mainWidget()
{
    if (!m_mainWidget) {
        __create2__ActorPainter(mySettings(), this);
        m_mainWidget = __mainWidget__ActorPainter();
        l_menus = __menuActions__ActorPainter();
    }
    return m_mainWidget;
}

QList<QAction*> PainterPlugin::menuActions()
{
    if (!m_mainWidget) {
        __create2__ActorPainter(mySettings(), this);
        m_mainWidget = __mainWidget__ActorPainter();
        l_menus = __menuActions__ActorPainter();
    }
    return l_menus;
}

} // namespace ActorPainter

Q_EXPORT_PLUGIN(ActorPainter::PainterPlugin)
