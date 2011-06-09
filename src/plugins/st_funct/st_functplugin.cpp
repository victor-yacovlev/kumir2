#include <QtCore>
#include <QtGui>

#include "st_functplugin.h"
#include "stdlib/kumstdlib.h"
#include "stdlib/genericinputoutput.h"

using namespace Shared;

namespace st_funct {


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
    cur_alg.cHeader = "double abs__st_funct(double x)";
    cur_alg.pascalHeader = "function abs(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ arccos(вещ x)");
    cur_alg.cHeader = "double arccos__st_funct(double x)";
    cur_alg.pascalHeader = "function arccos(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ arcctg(вещ x)");
    cur_alg.cHeader = "double arcctg__st_funct(double x)";
    cur_alg.pascalHeader = "function arcctg(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ arcsin(вещ x)");
    cur_alg.cHeader = "double arcsin__st_funct(double x)";
    cur_alg.pascalHeader = "function arcsin(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ arctg(вещ x)");
    cur_alg.cHeader = "double arctg__st_funct(double x)";
    cur_alg.pascalHeader = "function arctg(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ cos(вещ x)");
    cur_alg.cHeader = "double cos__st_funct(double x)";
    cur_alg.pascalHeader = "function cos(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ ctg(вещ x)");
    cur_alg.cHeader = "double ctg__st_funct(double x)";
    cur_alg.pascalHeader = "function ctg(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг delay(цел x)");
    cur_alg.cHeader = "void delay__st_funct(int x)";
    cur_alg.pascalHeader = "procedure delay(x: integer)";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел div(цел x, цел y)");
    cur_alg.cHeader = "int div__st_funct(int x, int y)";
    cur_alg.pascalHeader = "function div(x, y: integer): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ exp(вещ x)");
    cur_alg.cHeader = "double exp__st_funct(double x)";
    cur_alg.pascalHeader = "function exp(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел iabs(цел x)");
    cur_alg.cHeader = "int iabs__st_funct(int x)";
    cur_alg.pascalHeader = "function iabs(x: integer): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел imax(цел x, цел y)");
    cur_alg.cHeader = "int imax__st_funct(int x, int y)";
    cur_alg.pascalHeader = "function imax(x, y: integer): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел imin(цел x, цел y)");
    cur_alg.cHeader = "int imin__st_funct(int x, int y)";
    cur_alg.pascalHeader = "function imin(x, y: integer): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел int(вещ x)");
    cur_alg.cHeader = "int int__st_funct(double x)";
    cur_alg.pascalHeader = "function int(x: real): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел irand(цел x, цел y)");
    cur_alg.cHeader = "int irand__st_funct(int x, int y)";
    cur_alg.pascalHeader = "function irand(x, y: integer): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел irnd(цел x, цел y)");
    cur_alg.cHeader = "int irnd__st_funct(int x, int y)";
    cur_alg.pascalHeader = "function irnd(x, y: integer): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ lg(вещ x)");
    cur_alg.cHeader = "double lg__st_funct(double x)";
    cur_alg.pascalHeader = "function lg(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ ln(вещ x)");
    cur_alg.cHeader = "double ln__st_funct(double x)";
    cur_alg.pascalHeader = "function ln(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ max(вещ x, вещ y)");
    cur_alg.cHeader = "double max__st_funct(double x, double y)";
    cur_alg.pascalHeader = "function max(x,y: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ min(вещ x, вещ y)");
    cur_alg.cHeader = "double min__st_funct(double x, double y)";
    cur_alg.pascalHeader = "function min(x,y: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел mod(цел x, цел y)");
    cur_alg.cHeader = "int mod__st_funct(int x, int y)";
    cur_alg.pascalHeader = "function mod(x,y: integer): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ rand(вещ x, вещ y)");
    cur_alg.cHeader = "double rand__st_funct(double x, double y)";
    cur_alg.pascalHeader = "function rand(x,y: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ rnd(вещ x)");
    cur_alg.cHeader = "double rnd__st_funct(double x)";
    cur_alg.pascalHeader = "function rnd(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел sign(вещ x)");
    cur_alg.cHeader = "int sign__st_funct(double x)";
    cur_alg.pascalHeader = "function sign(x: real): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ sin(вещ x)");
    cur_alg.cHeader = "double sin__st_funct(double x)";
    cur_alg.pascalHeader = "function sin(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ sqrt(вещ x)");
    cur_alg.cHeader = "double sqrt__st_funct(double x)";
    cur_alg.pascalHeader = "function sqrt(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ tg(вещ x)");
    cur_alg.cHeader = "double tg__st_funct(double x)";
    cur_alg.pascalHeader = "function tg(x: real): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ МАКСВЕЩ");
    cur_alg.cHeader = "double MAXREAL__st_funct()";
    cur_alg.pascalHeader = "function MAXREAL: real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел МАКСЦЕЛ");
    cur_alg.cHeader = "int MAXINT__st_funct()";
    cur_alg.pascalHeader = "function MAXREAL: integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг лит вещ_в_лит(вещ x)");
    cur_alg.cHeader = "wchar_t* real_to_string__st_funct(double x)";
    cur_alg.pascalHeader = "function real_to_string(double x): string";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел время");
    cur_alg.cHeader = "int time__st_funct()";
    cur_alg.pascalHeader = "function time: integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел длин(лит s)");
    cur_alg.cHeader = "int length__st_funct(wchar_t* s)";
    cur_alg.pascalHeader = "function length(s: string): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел код(сим ch)");
    cur_alg.cHeader = "int code__st_funct(wchar_t ch)";
    cur_alg.pascalHeader = "function code(ch: charect): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ лит_в_вещ(лит s, рез лог success)");
    cur_alg.cHeader = "double string_to_real__st_funct(wchar_t* s, unsigned char *success)";
    cur_alg.pascalHeader = "function string_to_real(s: string; var success: boolean): real";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел лит_в_цел(лит s, рез лог success)");
    cur_alg.cHeader = "int string_to_int__st_funct(wchar_t * s, unsigned char *success)";
    cur_alg.pascalHeader = "function string_to_int(s: string; var success: boolean): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг сим символ(цел n)");
    cur_alg.cHeader = "wchar_t symbol__st_funct(int n)";
    cur_alg.pascalHeader = "function symbol(n: integer): char";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг сим символ2(цел n)");
    cur_alg.cHeader = "wchar_t symbol2__st_funct(int n)";
    cur_alg.pascalHeader = "function symbol2(n: integer): char";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг лит цел_в_лит(цел n)");
    cur_alg.cHeader = "wchar_t * int_to_string__st_funct(int n)";
    cur_alg.pascalHeader = "function int_to_string(n: integer): string";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел юникод(сим ch)");
    cur_alg.cHeader = "int unicode__st_funct(wchar_t ch)";
    cur_alg.pascalHeader = "function unicode(ch: char): integer";
    cur_alg.id=tmpL.size();
    tmpL.append(cur_alg);

    return tmpL;
}

QStringList St_functPlugin::actorLibraries() const
{
    return QStringList() << "KumirStdLib";
}

QStringList St_functPlugin::usedQtLibraries() const
{
    return QStringList() << "QtCore";
}

QString St_functPlugin::name(ProgrammingLanguage pl, QLocale::Language nl) const
{
    if (pl==PL_Kumir) {
        if (nl==QLocale::Russian) {
            return QString::fromUtf8("Стандартные функции");
        }
        else {
            return "Standart functions";
        }
    }
    else if (pl==PL_C) {
        return "st_funct";
    }
    else if (pl==PL_Pascal) {
        return "KumirStandartFunctions";
    }
    else {
        return "";
    }
}

QVariant St_functPlugin::result() const
{
    return QVariant();
    //return Res;
}

void  St_functPlugin::runFunct(const QString & name,const QList<QVariant> &args)
{    
    optResults.clear();

    if (name==tr("abs")) {
        Res = QVariant(abs__st_funct(args[0].toDouble()));
    }
    else if (name==tr("arccos")) {
        Res = QVariant(arccos__st_funct(args[0].toDouble()));
    }
    else if (name==tr("arcctg")) {
        Res = QVariant(arcctg__st_funct(args[0].toDouble()));
    }
    else if (name==tr("arcsin")) {
        Res = QVariant(arcsin__st_funct(args[0].toDouble()));
    }
    else if (name==tr("arctg")) {
        Res = QVariant(arctg__st_funct(args[0].toDouble()));
    }
    else if (name==tr("cos")) {
        Res = QVariant(cos__st_funct(args[0].toDouble()));
    }
    else if (name==tr("ctg")) {
        Res = QVariant(ctg__st_funct(args[0].toDouble()));
    }
    else if (name==tr("delay")) {
        Res = QVariant::Invalid;
        delay__st_funct(args[0].toInt());
    }
    else if (name==tr("div")) {
        Res = QVariant(div__st_funct(args[0].toInt(), args[1].toInt()));
    }
    else if (name==tr("exp")) {
        Res = QVariant(exp__st_funct(args[0].toDouble()));
    }
    else if (name==tr("iabs")) {
        Res = QVariant(iabs__st_funct(args[0].toInt()));
    }
    else if (name==tr("imin")) {
        Res = QVariant(imin__st_funct(args[0].toInt(), args[1].toInt()));
    }
    else if (name==tr("imax")) {
        Res = QVariant(imax__st_funct(args[0].toInt(), args[1].toInt()));
    }
    else if (name==tr("int")) {
        Res = QVariant(int__st_funct(args[0].toDouble()));
    }
    else if (name==tr("irand")) {
        Res = QVariant(irand__st_funct(args[0].toInt(), args[1].toInt()));
    }
    else if (name==tr("irnd")) {
        Res = QVariant(irnd__st_funct(args[0].toInt()));
    }
    else if (name==tr("lg")) {
        Res = QVariant(lg__st_funct(args[0].toDouble()));
    }
    else if (name==tr("ln")) {
        Res = QVariant(ln__st_funct(args[0].toDouble()));
    }
    else if (name==tr("max")) {
        Res = QVariant(max__st_funct(args[0].toDouble(), args[1].toDouble()));
    }
    else if (name==tr("min")) {
        Res = QVariant(min__st_funct(args[0].toDouble(), args[1].toDouble()));
    }
    else if (name==tr("mod")) {
        Res = QVariant(mod__st_funct(args[0].toInt(), args[1].toInt()));
    }
    else if (name==tr("rand")) {
        Res = QVariant(rand__st_funct(args[0].toDouble(), args[1].toDouble()));
    }
    else if (name==tr("rnd")) {
        Res = QVariant(rnd__st_funct(args[0].toDouble()));
    }
    else if (name==tr("sign")) {
        Res = QVariant(sign__st_funct(args[0].toDouble()));
    }
    else if (name==tr("sin")) {
        Res = QVariant(sin__st_funct(args[0].toDouble()));
    }
    else if (name==tr("sqrt")) {
        Res = QVariant(sqrt__st_funct(args[0].toDouble()));
    }
    else if (name==tr("tg")) {
        Res = QVariant(tg__st_funct(args[0].toDouble()));
    }
    else if (name==trUtf8("МАКСВЕЩ")) {
        Res = QVariant(MAXREAL__st_funct());
    }
    else if (name==trUtf8("МАКСЦЕЛ")) {
        Res = QVariant(MAXINT__st_funct());
    }
    else if (name==trUtf8("вещ_в_лит")) {
        wchar_t * buffer = real_to_string__st_funct(args[0].toDouble());
        Res = QVariant(QString::fromWCharArray(buffer));
        free(buffer);
    }
    else if (name==trUtf8("время")) {
        Res = QVariant(time__st_funct());
    }
    else if (name==trUtf8("длин")) {
        wchar_t * buffer = (wchar_t*)calloc(args[0].toString().size()+1, sizeof(wchar_t));
        args[0].toString().toWCharArray(buffer);
        buffer[args[0].toString().length()] = L'\0';
        Res = QVariant(length__st_funct(buffer));
        free(buffer);
    }
    else if (name==trUtf8("код")) {
        wchar_t buffer[1];
        args[0].toString().toWCharArray(buffer);
        Res = QVariant(code__st_funct(buffer[0]));
    }
    else if (name==trUtf8("лит_в_вещ")) {
        wchar_t * buffer = (wchar_t*)calloc(args[0].toString().size()+1, sizeof(wchar_t));
        args[0].toString().toWCharArray(buffer);
        buffer[args[0].toString().length()] = L'\0';
        unsigned char success;
        Res = QVariant(string_to_real__st_funct(buffer, &success));
        optResults << QVariant(success==1? true : false);
        free(buffer);
    }
    else if (name==trUtf8("лит_в_цел")) {
        wchar_t * buffer = (wchar_t*)calloc(args[0].toString().size()+1, sizeof(wchar_t));
        args[0].toString().toWCharArray(buffer);
        buffer[args[0].toString().length()] = L'\0';
        unsigned char success;
        Res = QVariant(string_to_int__st_funct(buffer, &success));
        optResults << QVariant(success==1? true : false);
        free(buffer);
    }
    else if (name==trUtf8("символ")) {
        wchar_t buffer[1];
        buffer[0] = symbol__st_funct(args[0].toInt());
        Res = QVariant(QString::fromWCharArray(buffer, 1).at(0));
    }
    else if (name==trUtf8("символ2")) {
        wchar_t buffer[1];
        buffer[0] = symbol2__st_funct(args[0].toInt());
        Res = QVariant(QString::fromWCharArray(buffer, 1).at(0));
    }
    else if (name==trUtf8("юникод")) {
        wchar_t buffer[1];
        args[0].toString().toWCharArray(buffer);
        Res = QVariant(unicode__st_funct(buffer[0]));
    }

    errText = __get_error__st_funct();
    emit sync();
}

QList<QVariant> St_functPlugin::algOptResults() const
{
    return QList<QVariant>();
}

ExtensionSystem::VisualComponent* St_functPlugin::mainWidget()
{
    return 0;
}

void St_functPlugin::reset()
{
    errText="";
    __init__st_funct();
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

} // end namespace st_funct


Q_EXPORT_PLUGIN(st_funct::St_functPlugin)
