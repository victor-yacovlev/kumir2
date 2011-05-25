#include <QtCore>
#include <QtGui>

#include "st_functplugin.h"

using namespace Shared;

namespace st_funct {

static wchar_t __error__[256];

St_functPlugin::St_functPlugin()
{

}

St_functPlugin::~St_functPlugin()
{

}

QList<Alg>  St_functPlugin::funcList() const
{
    QList<Alg> tmpL;

    Alg cur_alg;

    cur_alg.kumirHeader=trUtf8("алг вещ abs(вещ x)");
    cur_alg.cHeader = "double abs(double x)";
    cur_alg.pascalHeader = "function abs(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ arccos(вещ x)");
    cur_alg.cHeader = "double arccos(double x)";
    cur_alg.pascalHeader = "function arccos(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ arcctg(вещ x)");
    cur_alg.cHeader = "double arcctg(double x)";
    cur_alg.pascalHeader = "function arcctg(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ arcsin(вещ x)");
    cur_alg.cHeader = "double arcsin(double x)";
    cur_alg.pascalHeader = "function arcsin(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ arctg(вещ x)");
    cur_alg.cHeader = "double arctg(double x)";
    cur_alg.pascalHeader = "function arctg(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ cos(вещ x)");
    cur_alg.cHeader = "double cos(double x)";
    cur_alg.pascalHeader = "function cos(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ ctg(вещ x)");
    cur_alg.cHeader = "double ctg(double x)";
    cur_alg.pascalHeader = "function ctg(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг delay(цел x)");
    cur_alg.cHeader = "void delay(int x)";
    cur_alg.pascalHeader = "procedure delay(x: integer)";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел div(цел x, цел y)");
    cur_alg.cHeader = "int div(int x, int y)";
    cur_alg.pascalHeader = "function div(x, y: integer): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ exp(вещ x)");
    cur_alg.cHeader = "double exp(double x)";
    cur_alg.pascalHeader = "function exp(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел iabs(цел x)");
    cur_alg.cHeader = "int iabs(int x)";
    cur_alg.pascalHeader = "function iabs(x: integer): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел imax(цел x, цел y)");
    cur_alg.cHeader = "int imax(int x, int y)";
    cur_alg.pascalHeader = "function imax(x, y: integer): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел imin(цел x, цел y)");
    cur_alg.cHeader = "int imin(int x, int y)";
    cur_alg.pascalHeader = "function imin(x, y: integer): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел int(вещ x)");
    cur_alg.cHeader = "int int_(double x)";
    cur_alg.pascalHeader = "function int(x: real): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел irand(цел x, цел y)");
    cur_alg.cHeader = "int irand(int x, int y)";
    cur_alg.pascalHeader = "function irand(x, y: integer): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел irnd(цел x, цел y)");
    cur_alg.cHeader = "int irnd(int x, int y)";
    cur_alg.pascalHeader = "function irnd(x, y: integer): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ lg(вещ x)");
    cur_alg.cHeader = "double lg(double x)";
    cur_alg.pascalHeader = "function lg(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ ln(вещ x)");
    cur_alg.cHeader = "double ln(double x)";
    cur_alg.pascalHeader = "function ln(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ max(вещ x, вещ y)");
    cur_alg.cHeader = "double max(double x, double y)";
    cur_alg.pascalHeader = "function max(x,y: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ min(вещ x, вещ y)");
    cur_alg.cHeader = "double min(double x, double y)";
    cur_alg.pascalHeader = "function min(x,y: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел mod(цел x, цел y)");
    cur_alg.cHeader = "int mod(int x, int y)";
    cur_alg.pascalHeader = "function mod(x,y: integer): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ rand(вещ x, вещ y)");
    cur_alg.cHeader = "double rand(double x, double y)";
    cur_alg.pascalHeader = "function rand(x,y: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ rnd(вещ x)");
    cur_alg.cHeader = "double rnd(double x)";
    cur_alg.pascalHeader = "function rnd(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел sign(вещ x)");
    cur_alg.cHeader = "int sign(double x)";
    cur_alg.pascalHeader = "function sign(x: real): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ sin(вещ x)");
    cur_alg.cHeader = "double sin(double x)";
    cur_alg.pascalHeader = "function sin(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ sqrt(вещ x)");
    cur_alg.cHeader = "double sqrt(double x)";
    cur_alg.pascalHeader = "function sqrt(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ tg(вещ x)");
    cur_alg.cHeader = "double tg(double x)";
    cur_alg.pascalHeader = "function tg(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ МАКСВЕЩ");
    cur_alg.cHeader = "double MAXREAL()";
    cur_alg.pascalHeader = "function MAXREAL: real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел МАКСЦЕЛ");
    cur_alg.cHeader = "int MAXINT()";
    cur_alg.pascalHeader = "function MAXREAL: integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг лит вещ_в_лит(вещ x)");
    cur_alg.cHeader = "STRING real_to_string(double x)";
    cur_alg.pascalHeader = "function real_to_string(double x): string";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел время");
    cur_alg.cHeader = "int time()";
    cur_alg.pascalHeader = "function time: integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел длин(лит s)");
    cur_alg.cHeader = "int length(STRING s)";
    cur_alg.pascalHeader = "function length(s: string): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел код(сим ch)");
    cur_alg.cHeader = "int code(CHAR ch)";
    cur_alg.pascalHeader = "function code(ch: charect): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ лит_в_вещ(лит s, рез лог success)");
    cur_alg.cHeader = "double string_to_real(STRING s, BOOL *success)";
    cur_alg.pascalHeader = "function string_to_real(s: string; var success: boolean): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел лит_в_цел(лит s, рез лог success)");
    cur_alg.cHeader = "int string_to_int(STRING s, BOOL *success)";
    cur_alg.pascalHeader = "function string_to_int(s: string; var success: boolean): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг сим символ(цел n)");
    cur_alg.cHeader = "CHAR symbol(int n)";
    cur_alg.pascalHeader = "function symbol(n: integer): char";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг сим символ2(цел n)");
    cur_alg.cHeader = "CHAR symbol2(int n)";
    cur_alg.pascalHeader = "function symbol2(n: integer): char";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг лит цел_в_лит(цел n)");
    cur_alg.cHeader = "STRING int_to_string(int n)";
    cur_alg.pascalHeader = "function int_to_string(n: integer): string";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел юникод(сим ch)");
    cur_alg.cHeader = "int unicode(CHAR ch)";
    cur_alg.pascalHeader = "function unicode(ch: char): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    return tmpL;
}

QString St_functPlugin::name() const
{
    return "Standart functions";
}

QVariant St_functPlugin::result() const
{
    return QVariant();
    //return Res;
}

void  St_functPlugin::runFunct(const QString & name,const QList<QVariant> &args)
{
    __error__[0] = L'\0';
    optResults.clear();

    if (name==tr("abs")) {
        Res = QVariant(st_funct::abs(args[0].toDouble()));
    }
    else if (name==tr("arccos")) {
        Res = QVariant(st_funct::arccos(args[0].toDouble()));
    }
    else if (name==tr("arcctg")) {
        Res = QVariant(st_funct::arcctg(args[0].toDouble()));
    }
    else if (name==tr("arcsin")) {
        Res = QVariant(st_funct::arcsin(args[0].toDouble()));
    }
    else if (name==tr("arctg")) {
        Res = QVariant(st_funct::argtg(args[0].toDouble()));
    }
    else if (name==tr("cos")) {
        Res = QVariant(st_funct::cos(args[0].toDouble()));
    }
    else if (name==tr("ctg")) {
        Res = QVariant(st_funct::ctg(args[0].toDouble()));
    }
    else if (name==tr("delay")) {
        Res = QVariant::Invalid;
        st_funct::delay(args[0].toInt());
    }
    else if (name==tr("div")) {
        Res = QVariant(st_funct::div(args[0].toInt(), args[1].toInt()));
    }
    else if (name==tr("exp")) {
        Res = QVariant(st_funct::exp(args[0].toDouble()));
    }
    else if (name==tr("iabs")) {
        Res = QVariant(st_funct::iabs(args[0].toInt()));
    }
    else if (name==tr("imin")) {
        Res = QVariant(st_funct::imin(args[0].toInt(), args[1].toInt()));
    }
    else if (name==tr("imax")) {
        Res = QVariant(st_funct::imax(args[0].toInt(), args[1].toInt()));
    }
    else if (name==tr("int")) {
        Res = QVariant(st_funct::int_(args[0].toDouble()));
    }
    else if (name==tr("irand")) {
        Res = QVariant(st_funct::irand(args[0].toInt(), args[1].toInt()));
    }
    else if (name==tr("irnd")) {
        Res = QVariant(st_funct::irnd(args[0].toInt()));
    }
    else if (name==tr("lg")) {
        Res = QVariant(st_funct::lg(args[0].toDouble()));
    }
    else if (name==tr("ln")) {
        Res = QVariant(st_funct::ln(args[0].toDouble()));
    }
    else if (name==tr("max")) {
        Res = QVariant(st_funct::max(args[0].toDouble(), args[1].toDouble()));
    }
    else if (name==tr("min")) {
        Res = QVariant(st_funct::min(args[0].toDouble(), args[1].toDouble()));
    }
    else if (name==tr("mod")) {
        Res = QVariant(st_funct::mod(args[0].toInt(), args[1].toInt()));
    }
    else if (name==tr("rand")) {
        Res = QVariant(st_funct::rand(args[0].toDouble(), args[1].toDouble()));
    }
    else if (name==tr("rnd")) {
        Res = QVariant(st_funct::rnd(args[0].toDouble()));
    }
    else if (name==tr("sign")) {
        Res = QVariant(st_funct::sign(args[0].toDouble()));
    }
    else if (name==tr("sin")) {
        Res = QVariant(st_funct::sin(args[0].toDouble()));
    }
    else if (name==tr("sqrt")) {
        Res = QVariant(st_funct::sqrt(args[0].toDouble()));
    }
    else if (name==tr("tg")) {
        Res = QVariant(st_funct::tg(args[0].toDouble()));
    }
    else if (name==trUtf8("МАКСВЕЩ")) {
        Res = QVariant(st_funct::MAXREAL());
    }
    else if (name==trUtf8("МАКСЦЕЛ")) {
        Res = QVariant(st_funct::MAXINT());
    }
    else if (name==trUtf8("вещ_в_лит")) {
        wchar_t * buffer = st_funct::real_to_string(args[0].toDouble());
        Res = QVariant(QString::fromWCharArray(buffer));
        free(buffer);
    }
    else if (name==trUtf8("время")) {
        Res = QVariant(st_funct::time());
    }
    else if (name==trUtf8("длин")) {
        wchar_t * buffer = (wchar_t*)calloc(args[0].toString().size()+1, sizeof(wchar_t));
        args[0].toString().toWCharArray(buffer);
        buffer[args[0].toString().length()] = L'\0';
        Res = QVariant(st_funct::length(buffer));
        free(buffer);
    }
    else if (name==trUtf8("код")) {
        wchar_t buffer[1];
        args[0].toString().toWCharArray(buffer);
        Res = QVariant(st_funct::code(buffer[0]));
    }
    else if (name==trUtf8("лит_в_вещ")) {
        wchar_t * buffer = (wchar_t*)calloc(args[0].toString().size()+1, sizeof(wchar_t));
        args[0].toString().toWCharArray(buffer);
        buffer[args[0].toString().length()] = L'\0';
        unsigned char success;
        Res = QVariant(st_funct::string_to_real(buffer, &success));
        optResults << QVariant(success==1? true : false);
        free(buffer);
    }
    else if (name==trUtf8("лит_в_цел")) {
        wchar_t * buffer = (wchar_t*)calloc(args[0].toString().size()+1, sizeof(wchar_t));
        args[0].toString().toWCharArray(buffer);
        buffer[args[0].toString().length()] = L'\0';
        unsigned char success;
        Res = QVariant(st_funct::string_to_int(buffer, &success));
        optResults << QVariant(success==1? true : false);
        free(buffer);
    }
    else if (name==trUtf8("символ")) {
        wchar_t buffer[1];
        buffer[0] = st_funct::symbol(args[0].toInt());
        Res = QVariant(QString::fromWCharArray(buffer, 1).at(0));
    }
    else if (name==trUtf8("символ2")) {
        wchar_t buffer[1];
        buffer[0] = st_funct::symbol2(args[0].toInt());
        Res = QVariant(QString::fromWCharArray(buffer, 1).at(0));
    }
    else if (name==trUtf8("юникод")) {
        wchar_t buffer[1];
        args[0].toString().toWCharArray(buffer);
        Res = QVariant(st_funct::unicode(buffer[0]));
    }



    errText = QString::fromWCharArray(__error__);
}

QList<QVariant> St_functPlugin::algOptResults() const
{
    return QList<QVariant>();
}

QWidget* St_functPlugin::mainWidget()
{
    return 0;
}

void St_functPlugin::reset()
{
    errText="";
    __init__();
}

QString St_functPlugin::errorText() const
{
    return errText;
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

}

Q_EXPORT_PLUGIN(st_funct::St_functPlugin);
