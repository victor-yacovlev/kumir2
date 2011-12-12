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
    cur_alg.id=0x0101;
    cur_alg.name = trUtf8("abs");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ arccos(вещ x)");
    cur_alg.cHeader = "double arccos__st_funct(double x)";
    cur_alg.pascalHeader = "function arccos(x: real): real";
    cur_alg.id=0x0102;
    cur_alg.name = trUtf8("arccos");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ arcctg(вещ x)");
    cur_alg.cHeader = "double arcctg__st_funct(double x)";
    cur_alg.pascalHeader = "function arcctg(x: real): real";
    cur_alg.id=0x0103;
    cur_alg.name = trUtf8("arcctg");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ arcsin(вещ x)");
    cur_alg.cHeader = "double arcsin__st_funct(double x)";
    cur_alg.pascalHeader = "function arcsin(x: real): real";
    cur_alg.id=0x0104;
    cur_alg.name = trUtf8("arcsin");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ arctg(вещ x)");
    cur_alg.cHeader = "double arctg__st_funct(double x)";
    cur_alg.pascalHeader = "function arctg(x: real): real";
    cur_alg.id=0x0105;
    cur_alg.name = trUtf8("arctg");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ cos(вещ x)");
    cur_alg.cHeader = "double cos__st_funct(double x)";
    cur_alg.pascalHeader = "function cos(x: real): real";
    cur_alg.name = trUtf8("cos");
    cur_alg.id=0x0106;
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ ctg(вещ x)");
    cur_alg.cHeader = "double ctg__st_funct(double x)";
    cur_alg.pascalHeader = "function ctg(x: real): real";
    cur_alg.name = trUtf8("ctg");
    cur_alg.id=0x0107;
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг delay(цел x)");
    cur_alg.cHeader = "void delay__st_funct(int x)";
    cur_alg.pascalHeader = "procedure delay(x: integer)";
    cur_alg.name = trUtf8("delay");
    cur_alg.id=0x0108;
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел div(цел x, цел y)");
    cur_alg.cHeader = "int div__st_funct(int x, int y)";
    cur_alg.pascalHeader = "function div(x, y: integer): integer";
    cur_alg.id=0x0209;
    cur_alg.name = trUtf8("div");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ exp(вещ x)");
    cur_alg.cHeader = "double exp__st_funct(double x)";
    cur_alg.pascalHeader = "function exp(x: real): real";
    cur_alg.name = trUtf8("exp");
    cur_alg.id=0x010a;
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел iabs(цел x)");
    cur_alg.cHeader = "int iabs__st_funct(int x)";
    cur_alg.pascalHeader = "function iabs(x: integer): integer";
    cur_alg.name = trUtf8("iabs");
    cur_alg.id=0x010b;
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел imax(цел x, цел y)");
    cur_alg.cHeader = "int imax__st_funct(int x, int y)";
    cur_alg.pascalHeader = "function imax(x, y: integer): integer";
    cur_alg.name = trUtf8("imax");
    cur_alg.id=0x020c;
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел imin(цел x, цел y)");
    cur_alg.cHeader = "int imin__st_funct(int x, int y)";
    cur_alg.pascalHeader = "function imin(x, y: integer): integer";
    cur_alg.id=0x020d;
    cur_alg.name = trUtf8("imin");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел int(вещ x)");
    cur_alg.cHeader = "int int__st_funct(double x)";
    cur_alg.pascalHeader = "function int(x: real): integer";
    cur_alg.name = trUtf8("int");
    cur_alg.id=0x010e;
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел irand(цел x, цел y)");
    cur_alg.cHeader = "int irand__st_funct(int x, int y)";
    cur_alg.pascalHeader = "function irand(x, y: integer): integer";
    cur_alg.name = trUtf8("irand");
    cur_alg.id=0x020f;
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел irnd(цел x)");
    cur_alg.cHeader = "int irnd__st_funct(int x)";
    cur_alg.pascalHeader = "function irnd(x: integer): integer";
    cur_alg.id=0x0110;
    cur_alg.name = trUtf8("irnd");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ lg(вещ x)");
    cur_alg.cHeader = "double lg__st_funct(double x)";
    cur_alg.pascalHeader = "function lg(x: real): real";
    cur_alg.name = trUtf8("lg");
    cur_alg.id=0x0111;
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ ln(вещ x)");
    cur_alg.cHeader = "double ln__st_funct(double x)";
    cur_alg.pascalHeader = "function ln(x: real): real";
    cur_alg.name = trUtf8("ln");
    cur_alg.id=0x0112;
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ max(вещ x, вещ y)");
    cur_alg.cHeader = "double max__st_funct(double x, double y)";
    cur_alg.pascalHeader = "function max(x,y: real): real";
    cur_alg.id=0x0213;
    cur_alg.name = trUtf8("max");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ min(вещ x, вещ y)");
    cur_alg.cHeader = "double min__st_funct(double x, double y)";
    cur_alg.pascalHeader = "function min(x,y: real): real";
    cur_alg.name = trUtf8("min");
    cur_alg.id=0x0214;
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел mod(цел x, цел y)");
    cur_alg.cHeader = "int mod__st_funct(int x, int y)";
    cur_alg.pascalHeader = "function mod(x,y: integer): integer";
    cur_alg.name = trUtf8("mod");
    cur_alg.id=0x0215;
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ rand(вещ x, вещ y)");
    cur_alg.cHeader = "double rand__st_funct(double x, double y)";
    cur_alg.pascalHeader = "function rand(x,y: real): real";
    cur_alg.id=0x0216;
    cur_alg.name = trUtf8("rand");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ rnd(вещ x)");
    cur_alg.cHeader = "double rnd__st_funct(double x)";
    cur_alg.pascalHeader = "function rnd(x: real): real";
    cur_alg.id=0x0117;
    cur_alg.name = trUtf8("rnd");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел sign(вещ x)");
    cur_alg.cHeader = "int sign__st_funct(double x)";
    cur_alg.pascalHeader = "function sign(x: real): integer";
    cur_alg.id=0x0118;
    cur_alg.name = trUtf8("sign");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ sin(вещ x)");
    cur_alg.cHeader = "double sin__st_funct(double x)";
    cur_alg.pascalHeader = "function sin(x: real): real";
    cur_alg.id=0x0119;
    cur_alg.name = trUtf8("sin");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ sqrt(вещ x)");
    cur_alg.cHeader = "double sqrt__st_funct(double x)";
    cur_alg.pascalHeader = "function sqrt(x: real): real";
    cur_alg.id=0x011a;
    cur_alg.name = trUtf8("sqrt");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ tg(вещ x)");
    cur_alg.cHeader = "double tg__st_funct(double x)";
    cur_alg.pascalHeader = "function tg(x: real): real";
    cur_alg.id=0x011b;
    cur_alg.name = trUtf8("tg");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ МАКСВЕЩ");
    cur_alg.cHeader = "double MAXREAL__st_funct()";
    cur_alg.pascalHeader = "function MAXREAL: real";
    cur_alg.id=0x001c;
    cur_alg.name = trUtf8("МАКСВЕЩ");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел МАКСЦЕЛ");
    cur_alg.cHeader = "int MAXINT__st_funct()";
    cur_alg.pascalHeader = "function MAXREAL: integer";
    cur_alg.id=0x001d;
    cur_alg.name = trUtf8("МАКСЦЕЛ");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг лит вещ_в_лит(вещ x)");
    cur_alg.cHeader = "wchar_t* real_to_string__st_funct(double x)";
    cur_alg.pascalHeader = "function real_to_string(double x): string";
    cur_alg.id=0x011e;
    cur_alg.name = trUtf8("вещ_в_лит");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел время");
    cur_alg.cHeader = "int time__st_funct()";
    cur_alg.pascalHeader = "function time: integer";
    cur_alg.name = trUtf8("время");
    cur_alg.id=0x001f;
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел длин(лит s)");
    cur_alg.cHeader = "int length__st_funct(wchar_t* s)";
    cur_alg.pascalHeader = "function length(s: string): integer";
    cur_alg.id=0x0120;
    cur_alg.name = trUtf8("длин");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел код(сим ch)");
    cur_alg.cHeader = "int code__st_funct(wchar_t ch)";
    cur_alg.pascalHeader = "function code(ch: charect): integer";
    cur_alg.id=0x0121;
    cur_alg.name = trUtf8("код");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг вещ лит_в_вещ(лит s, рез лог success)");
    cur_alg.cHeader = "double string_to_real__st_funct(wchar_t* s, unsigned char *success)";
    cur_alg.pascalHeader = "function string_to_real(s: string; var success: boolean): real";
    cur_alg.id=0x0122;
    cur_alg.name = trUtf8("лит_в_вещ");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел лит_в_цел(лит s, рез лог success)");
    cur_alg.cHeader = "int string_to_int__st_funct(wchar_t * s, unsigned char *success)";
    cur_alg.pascalHeader = "function string_to_int(s: string; var success: boolean): integer";
    cur_alg.id=0x0123;
    cur_alg.name = trUtf8("лит_в_цел");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг сим символ(цел n)");
    cur_alg.cHeader = "wchar_t symbol__st_funct(int n)";
    cur_alg.pascalHeader = "function symbol(n: integer): char";
    cur_alg.id=0x0124;
    cur_alg.name = trUtf8("символ");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг сим символ2(цел n)");
    cur_alg.cHeader = "wchar_t symbol2__st_funct(int n)";
    cur_alg.pascalHeader = "function symbol2(n: integer): char";
    cur_alg.id=0x0125;
    cur_alg.name = trUtf8("символ2");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг лит цел_в_лит(цел n)");
    cur_alg.cHeader = "wchar_t * int_to_string__st_funct(int n)";
    cur_alg.pascalHeader = "function int_to_string(n: integer): string";
    cur_alg.id=0x0126;
    cur_alg.name = trUtf8("цел_в_лит");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader=trUtf8("алг цел юникод(сим ch)");
    cur_alg.cHeader = "int unicode__st_funct(wchar_t ch)";
    cur_alg.pascalHeader = "function unicode(ch: char): integer";
    cur_alg.id=0x0127;
    cur_alg.name = trUtf8("юникод");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader = trUtf8("алг лог существует файл(лит имя файла");
    cur_alg.cHeader = "unsigned char is_file_exists__st_funct(wchar_t* file_name)";
    cur_alg.pascalHeader = "function is_file_exists(filename: string): boolean";
    cur_alg.id = 0x0128;
    cur_alg.name = trUtf8("существует файл");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader = trUtf8("алг цел удалить файл(лит имя файла)");
    cur_alg.cHeader = "int remove_file__st_funct(wchar_t * file_name)";
    cur_alg.pascalHeader = "function remove_file(filename: string): integer";
    cur_alg.id = 0x0129;
    cur_alg.name = trUtf8("удалить файл");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader = trUtf8("алг цел кодировка(лит имя кодировки)");
    cur_alg.cHeader = "void set_file_encoding__st_funct(wchar_t * encoding)";
    cur_alg.pascalHeader = "procedure set_file_encoding(s: string)";
    cur_alg.id = 0x012a;
    cur_alg.name = trUtf8("кодировка");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader = trUtf8("алг цел открыть на чтение(лит имя файла)");
    cur_alg.cHeader = "int open_file_r__st_funct(wchar_t * file_name)";
    cur_alg.pascalHeader = "function open_file_r(filename: string): integer";
    cur_alg.id = 0x012b;
    cur_alg.name = trUtf8("открыть на чтение");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader = trUtf8("алг цел открыть на запись(лит имя файла)");
    cur_alg.cHeader = "int open_file_w__st_funct(wchar_t * file_name)";
    cur_alg.pascalHeader = "function open_file_w(filename: string): integer";
    cur_alg.id = 0x012c;
    cur_alg.name = trUtf8("открыть на запись");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader = trUtf8("алг цел открыть на добавление(лит имя файла)");
    cur_alg.cHeader = "int open_file_a__st_funct(wchar_t * file_name)";
    cur_alg.pascalHeader = "function open_file_a(filename: string): integer";
    cur_alg.id = 0x012d;
    cur_alg.name = trUtf8("открыть на добавление");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader = trUtf8("алг закрыть(цел ключ)");
    cur_alg.cHeader = "void close_file_a__st_funct(int file_handle)";
    cur_alg.pascalHeader = "procedure close_file(file_handle: integer)";
    cur_alg.id = 0x012e;
    cur_alg.name = trUtf8("закрыть");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader = trUtf8("алг начать чтение(цел ключ)");
    cur_alg.cHeader = "void reset_file_a__st_funct(int file_handle)";
    cur_alg.pascalHeader = "procedure reset_file(file_handle: integer)";
    cur_alg.id = 0x012f;
    cur_alg.name = trUtf8("начать чтение");
    tmpL.append(cur_alg);

    cur_alg.kumirHeader = trUtf8("алг лог конец файла(цел ключ)");
    cur_alg.cHeader = "unsigned char is_file_at_end__st_funct(int file_handle)";
    cur_alg.pascalHeader = "function is_file_at_end(file_handle: integer)";
    cur_alg.id = 0x0130;
    cur_alg.name = trUtf8("конец файла");
    tmpL.append(cur_alg);

    return tmpL;
}

EvaluationStatus St_functPlugin::evaluate(quint32 id)
{
    optResults.clear();
    switch(id) {
    case 0x001c:
        return MaxReal();
    case 0x001d:
        return MaxInteger();
    case 0x001f:
        return Time();
    default:
        qFatal("Unknown method id: %x", id);
        return ES_Error;
    }
}

EvaluationStatus St_functPlugin::evaluate(quint32 id, const QVariant &x)
{
    optResults.clear();
    switch(id) {
    case 0x0101:
        return Abs(x);
    case 0x0102:
        return ArcCos(x);
    case 0x0103:
        return ArcCtg(x);
    case 0x0104:
        return ArcSin(x);
    case 0x0105:
        return ArcTg(x);
    case 0x0106:
        return Cos(x);
    case 0x0107:
        return Ctg(x);
    case 0x0108:
        return Delay(x);
    case 0x010a:
        return Exp(x);
    case 0x010b:
        return Iabs(x);
    case 0x010e:
        return Int(x);
    case 0x0110:
        return Irnd(x);
    case 0x0111:
        return Lg(x);
    case 0x0112:
        return Ln(x);
    case 0x0117:
        return Rnd(x);
    case 0x0118:
        return Sign(x);
    case 0x0119:
        return Sin(x);
    case 0x011a:
        return Sqrt(x);
    case 0x011b:
        return Tg(x);
    case 0x011e:
        return StrOfReal(x);
    case 0x0120:
        return Length(x);
    case 0x0121:
        return Code(x);
    case 0x0122:
        return RealOfString(x);
    case 0x0123:
        return IntegerOfString(x);
    case 0x0124:
        return Symbol(x);
    case 0x0125:
        return UniSymbol(x);
    case 0x0126:
        return StrOfInteger(x);
    case 0x0127:
        return Unicode(x);
    case 0x0128:
        return FileExists(x);
    case 0x0129:
        return FileUnlink(x);
    case 0x012a:
        return FileSetEncoding(x);
    case 0x012b:
        return FileOpenForRead(x);
    case 0x012c:
        return FileOpenForWrite(x);
    case 0x012d:
        return FileOpenForAppend(x);
    case 0x012e:
        return FileClose(x);
    case 0x012f:
        return FileReset(x);
    case 0x0130:
        return FileAtEnd(x);
    default:
        qFatal("Unknown method id: %x", id);
        return ES_Error;
    }
}

EvaluationStatus St_functPlugin::evaluate(quint32 id, const QVariant & x, const QVariant & y)
{
    optResults.clear();
    switch(id) {
    case 0x0209:
        return Div(x,y);
    case 0x020c:
        return Imax(x,y);
    case 0x020d:
        return Imin(x,y);
    case 0x020f:
        return Irand(x,y);
    case 0x0213:
        return Max(x,y);
    case 0x0214:
        return Min(x,y);
    case 0x0215:
        return Mod(x,y);
    case 0x0216:
        return Rand(x,y);
    default:
        qFatal("Unknown method id: %x", id);
        return ES_Error;
    }
}


QStringList St_functPlugin::actorLibraries() const
{
    return QStringList() << "KumirStdLib";
}

QStringList St_functPlugin::usedQtLibraries() const
{
    return QStringList() << "QtCore" << "QtGui";
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
//    return QVariant();
    return Res;
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
    QList<QVariant> opts = optResults;
    return opts;
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


EvaluationStatus St_functPlugin::Abs(const QVariant & x){
    Res = QVariant(abs__st_funct(x.toDouble()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::ArcCos(const QVariant & x){
    Res = QVariant(arccos__st_funct(x.toDouble()));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::ArcSin(const QVariant & x){
    Res = QVariant(arcsin__st_funct(x.toDouble()));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::ArcCtg(const QVariant & x){
    Res = QVariant(arcctg__st_funct(x.toDouble()));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::ArcTg(const QVariant & x){
    Res = QVariant(arctg__st_funct(x.toDouble()));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Cos(const QVariant & x){
    Res = QVariant(cos__st_funct(x.toDouble()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Ctg(const QVariant & x){
    Res = QVariant(ctg__st_funct(x.toDouble()));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Delay(const QVariant & x){
    return ES_NoResult;
    // TODO implement me
}

EvaluationStatus St_functPlugin::Div(const QVariant & x, const QVariant & y){
    Res = QVariant(div__st_funct(x.toInt(), y.toInt()));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Exp(const QVariant & x){
    Res = QVariant(exp__st_funct(x.toDouble()));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Iabs(const QVariant & x){
    Res = QVariant(iabs__st_funct(x.toInt()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Imax(const QVariant & x, const QVariant & y){
    Res = QVariant(imax__st_funct(x.toInt(), y.toInt()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Imin(const QVariant & x, const QVariant & y){
    Res = QVariant(imin__st_funct(x.toInt(), y.toInt()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Int(const QVariant & x){
    Res = QVariant(int__st_funct(x.toDouble()));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Irand(const QVariant & x, const QVariant & y){
    Res = QVariant(irand__st_funct(x.toInt(), y.toInt()));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Irnd(const QVariant & x){
    Res = QVariant(irnd__st_funct(x.toInt()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Lg(const QVariant & x){
    Res = QVariant(lg__st_funct(x.toDouble()));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Ln(const QVariant & x){
    Res = QVariant(ln__st_funct(x.toDouble()));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Max(const QVariant & x, const QVariant & y){
    Res = QVariant(max__st_funct(x.toDouble(), y.toDouble()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Min(const QVariant & x, const QVariant & y){
    Res = QVariant(min__st_funct(x.toDouble(), y.toDouble()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Mod(const QVariant & x, const QVariant & y){
    Res = QVariant(mod__st_funct(x.toInt(), y.toInt()));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Rand(const QVariant & x, const QVariant & y){
    Res = QVariant(rand__st_funct(x.toDouble(), y.toDouble()));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Rnd(const QVariant & x){
    Res = QVariant(rnd__st_funct(x.toDouble()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Sign(const QVariant & x){
    Res = QVariant(sign__st_funct(x.toDouble()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Sin(const QVariant & x){
    Res = QVariant(sin__st_funct(x.toDouble()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Sqrt(const QVariant & x){
    Res = QVariant(sqrt__st_funct(x.toDouble()));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Tg(const QVariant & x){
    Res = QVariant(tg__st_funct(x.toDouble()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::MaxReal(){
    Res = QVariant(MAXREAL__st_funct());
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::MaxInteger(){
    Res = QVariant(MAXINT__st_funct());
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::StrOfReal(const QVariant & x){
    wchar_t * buffer = real_to_string__st_funct(x.toDouble());
    Res = QVariant(QString::fromWCharArray(buffer));
    free(buffer);
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::StrOfInteger(const QVariant & x){
    wchar_t * buffer = int_to_string__st_funct(x.toDouble());
    Res = QVariant(QString::fromWCharArray(buffer));
    free(buffer);
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Time(){
    Res = QVariant(time__st_funct());
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Length(const QVariant & x){
    Res = QVariant(x.toString().length());
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Code(const QVariant & x){
    wchar_t buffer[1];
    x.toString().toWCharArray(buffer);
    Res = QVariant(code__st_funct(buffer[0]));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::RealOfString(const QVariant & x){
    wchar_t * buffer = (wchar_t*)calloc(x.toString().size()+1, sizeof(wchar_t));
    x.toString().toWCharArray(buffer);
    buffer[x.toString().length()] = L'\0';
    unsigned char success;
    Res = QVariant(string_to_real__st_funct(buffer, &success));
    optResults << QVariant(success==1? true : false);
    free(buffer);
    return ES_StackRezResult;
}

EvaluationStatus St_functPlugin::IntegerOfString(const QVariant & x){
    wchar_t * buffer = (wchar_t*)calloc(x.toString().size()+1, sizeof(wchar_t));
    x.toString().toWCharArray(buffer);
    buffer[x.toString().length()] = L'\0';
    unsigned char success;
    Res = QVariant(string_to_int__st_funct(buffer, &success));
    optResults << QVariant(success==1? true : false);
    free(buffer);
    return ES_StackRezResult;
}

EvaluationStatus St_functPlugin::Symbol(const QVariant & x){
    wchar_t buffer[1];
    buffer[0] = symbol__st_funct(x.toInt());
    Res = QVariant(QString::fromWCharArray(buffer, 1).at(0));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::UniSymbol(const QVariant & x){
    wchar_t buffer[1];
    buffer[0] = symbol2__st_funct(x.toInt());
    Res = QVariant(QString::fromWCharArray(buffer, 1).at(0));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Unicode(const QVariant & x){
    wchar_t buffer[1];
    x.toString().toWCharArray(buffer);
    Res = QVariant(unicode__st_funct(buffer[0]));
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::FileExists(const QVariant &x) {
    wchar_t * buffer = (wchar_t*)calloc(x.toString().size()+1, sizeof(wchar_t));
    x.toString().toWCharArray(buffer);
    buffer[x.toString().length()] = L'\0';
    unsigned char success;
    success = is_file_exists__st_funct(buffer);
    Res = QVariant(success);
    errText = __get_error__st_funct();
    free(buffer);
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::FileUnlink(const QVariant &x) {
    wchar_t * buffer = (wchar_t*)calloc(x.toString().size()+1, sizeof(wchar_t));
    x.toString().toWCharArray(buffer);
    buffer[x.toString().length()] = L'\0';
    int errcode = remove_file__st_funct(buffer);
    Res = QVariant(errcode);
    free(buffer);
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::FileSetEncoding(const QVariant &x) {
    wchar_t * buffer = (wchar_t*)calloc(x.toString().size()+1, sizeof(wchar_t));
    x.toString().toWCharArray(buffer);
    buffer[x.toString().length()] = L'\0';
    set_file_encoding__st_funct(buffer);
    errText = __get_error__st_funct();
    free(buffer);
    return errText.isEmpty()? ES_NoResult : ES_Error;
}

EvaluationStatus St_functPlugin::FileOpenForRead(const QVariant &x) {
    wchar_t * buffer = (wchar_t*)calloc(x.toString().size()+1, sizeof(wchar_t));
    x.toString().toWCharArray(buffer);
    buffer[x.toString().length()] = L'\0';
    int handle = open_file_r__st_funct(buffer);
    Res = QVariant(handle);
    free(buffer);
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::FileOpenForWrite(const QVariant &x) {
    wchar_t * buffer = (wchar_t*)calloc(x.toString().size()+1, sizeof(wchar_t));
    x.toString().toWCharArray(buffer);
    buffer[x.toString().length()] = L'\0';
    int handle = open_file_w__st_funct(buffer);
    Res = QVariant(handle);
    free(buffer);
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::FileOpenForAppend(const QVariant &x) {
    wchar_t * buffer = (wchar_t*)calloc(x.toString().size()+1, sizeof(wchar_t));
    x.toString().toWCharArray(buffer);
    buffer[x.toString().length()] = L'\0';
    int handle = open_file_a__st_funct(buffer);
    Res = QVariant(handle);
    free(buffer);
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::FileClose(const QVariant &x) {
    int handle = x.toInt();
    close_file__st_funct(handle);
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_NoResult : ES_Error;
}

EvaluationStatus St_functPlugin::FileReset(const QVariant &x) {
    int handle = x.toInt();
    reset_file__st_funct(handle);
    errText = __get_error__st_funct();
    return errText.isEmpty()? ES_NoResult : ES_Error;
}

EvaluationStatus St_functPlugin::FileAtEnd(const QVariant &x) {
    int handle = x.toInt();
    unsigned char atend = is_file_at_end__st_funct(handle);
    errText = __get_error__st_funct();
    Res = QVariant(atend);
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

} // end namespace st_funct


Q_EXPORT_PLUGIN(st_funct::St_functPlugin)
