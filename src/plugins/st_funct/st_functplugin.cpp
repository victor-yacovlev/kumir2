#include <QtCore>
#include <QtGui>

#include "st_functplugin.h"
#include "stdlib/kumirstdlib.hpp"

using namespace Shared;

namespace st_funct {


St_functPlugin::St_functPlugin()
{

}

St_functPlugin::~St_functPlugin()
{

}

QStringList St_functPlugin::funcList() const
{
    QStringList result;
    // TODO implement non-russian headers
    // (check system locale)

    /* 0x0000 */ result << trUtf8("алг вещ abs(вещ x)");
    /* 0x0001 */ result << trUtf8("алг вещ arccos(вещ x)");
    /* 0x0002 */ result << trUtf8("алг вещ arcctg(вещ x)");
    /* 0x0003 */ result << trUtf8("алг вещ arcsin(вещ x)");
    /* 0x0004 */ result << trUtf8("алг вещ arctg(вещ x)");
    /* 0x0005 */ result << trUtf8("алг вещ cos(вещ x)");
    /* 0x0006 */ result << trUtf8("алг вещ ctg(вещ x)");
    /* 0x0007 */ result << trUtf8("алг delay(цел x)");
    /* 0x0008 */ result << trUtf8("алг цел div(цел x, цел y)");
    /* 0x0009 */ result << trUtf8("алг вещ exp(вещ x)");
    /* 0x000a */ result << trUtf8("алг цел iabs(цел x)");
    /* 0x000b */ result << trUtf8("алг цел imax(цел x, цел y)");
    /* 0x000c */ result << trUtf8("алг цел imin(цел x, цел y)");
    /* 0x000d */ result << trUtf8("алг цел int(вещ x)");
    /* 0x000e */ result << trUtf8("алг цел irand(цел x, цел y)");
    /* 0x000f */ result << trUtf8("алг цел irnd(цел x)");
    /* 0x0010 */ result << trUtf8("алг вещ lg(вещ x)");
    /* 0x0011 */ result << trUtf8("алг вещ ln(вещ x)");
    /* 0x0012 */ result << trUtf8("алг вещ max(вещ x, вещ y)");
    /* 0x0013 */ result << trUtf8("алг вещ min(вещ x, вещ y)");
    /* 0x0014 */ result << trUtf8("алг цел mod(цел x, цел y)");
    /* 0x0015 */ result << trUtf8("алг вещ rand(вещ x, вещ y)");
    /* 0x0016 */ result << trUtf8("алг вещ rnd(вещ x)");
    /* 0x0017 */ result << trUtf8("алг цел sign(вещ x)");
    /* 0x0018 */ result << trUtf8("алг вещ sin(вещ x)");
    /* 0x0019 */ result << trUtf8("алг вещ sqrt(вещ x)");
    /* 0x001a */ result << trUtf8("алг вещ tg(вещ x)");
    /* 0x001b */ result << trUtf8("алг вещ МАКСВЕЩ");
    /* 0x001c */ result << trUtf8("алг цел МАКСЦЕЛ");
    /* 0x001d */ result << trUtf8("алг лит вещ_в_лит(вещ x)");
    /* 0x001e */ result << trUtf8("алг цел время");
    /* 0x001f */ result << trUtf8("алг цел длин(лит s)");
    /* 0x0020 */ result << trUtf8("алг цел код(сим ch)");
    /* 0x0021 */ result << trUtf8("алг вещ лит_в_вещ(лит s, рез лог success)");
    /* 0x0022 */ result << trUtf8("алг цел лит_в_цел(лит s, рез лог success)");
    /* 0x0023 */ result << trUtf8("алг сим символ(цел n)");
    /* 0x0024 */ result << trUtf8("алг сим символ2(цел n)");
    /* 0x0025 */ result << trUtf8("алг лит цел_в_лит(цел n)");
    /* 0x0026 */ result << trUtf8("алг цел юникод(сим ch)");

    /* 0x0027 */ result << trUtf8("алг цел открыть на чтение(лит имя файла)");
    /* 0x0028 */ result << trUtf8("алг цел открыть на запись(лит имя файла)");
    /* 0x0029 */ result << trUtf8("алг цел открыть на добавление(лит имя файла)");
    /* 0x002a */ result << trUtf8("алг закрыть(цел ключ)");
    /* 0x002b */ result << trUtf8("алг начать чтение(цел ключ)");
    /* 0x002c */ result << trUtf8("алг лог конец файла(цел ключ)");

    /* 0x002d */ result << trUtf8("алг установить кодировку(лит кодировка)");

    return result;
}


QString St_functPlugin::name() const
{
    // TODO implement non-russian headers
    // (check system locale)
    return QString::fromUtf8("Стандартные функции");
}


QString St_functPlugin::initialize(const QStringList &)
{
    return "";
}

void St_functPlugin::start()
{
}

void St_functPlugin::stop()
{
}



} // end namespace st_funct


Q_EXPORT_PLUGIN(st_funct::St_functPlugin)
