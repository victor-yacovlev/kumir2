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

    return result;
}

EvaluationStatus St_functPlugin::evaluate(quint32 id, const QVariantList & args)
{
    optResults.clear();
    switch(id) {
    /* алг вещ abs(вещ x) */
    case 0x0000: {
        return Abs(args[0]);
    }
    /* алг вещ arccos(вещ x) */
    case 0x0001: {
        return ArcCos(args[0]);
    }
    /* алг вещ arcctg(вещ x) */
    case 0x0002: {
        return ArcCtg(args[0]);
    }
    /* алг вещ arcsin(вещ x) */
    case 0x0003: {
        return ArcSin(args[0]);
    }
    /* алг вещ arctg(вещ x) */
    case 0x0004: {
        return ArcTg(args[0]);
    }
    /* алг вещ cos(вещ x) */
    case 0x0005: {
        return Cos(args[0]);
    }
    /* алг вещ ctg(вещ x) */
    case 0x0006: {
        return Ctg(args[0]);
    }
    /* алг delay(цел x) */
    case 0x0007: {
        return Delay(args[0]);
    }
    /* алг цел div(цел x, цел y) */
    case 0x0008: {
        return Div(args[0], args[1]);
    }
    /* алг вещ exp(вещ x) */
    case 0x0009: {
        return Exp(args[0]);
    }
    /* алг цел iabs(цел x) */
    case 0x000a: {
        return Iabs(args[0]);
    }
    /* алг цел imax(цел x, цел y) */
    case 0x000b: {
        return Imax(args[0], args[1]);
    }
    /* алг цел imin(цел x, цел y) */
    case 0x000c: {
        return Imin(args[0], args[1]);
    }
    /* алг цел int(вещ x) */
    case 0x000d: {
        return Int(args[0]);
    }
    /* алг цел irand(цел x, цел y) */
    case 0x000e: {
        return Irand(args[0], args[1]);
    }
    /* алг цел irnd(цел x) */
    case 0x000f: {
        return Irnd(args[0]);
    }
    /* алг вещ lg(вещ x) */
    case 0x0010: {
        return Lg(args[0]);
    }
    /* алг вещ ln(вещ x) */
    case 0x0011: {
        return Ln(args[0]);
    }
    /* алг вещ max(вещ x, вещ y) */
    case 0x0012: {
        return Max(args[0], args[1]);
    }
    /* алг вещ min(вещ x, вещ y) */
    case 0x0013: {
        return Min(args[0], args[1]);
    }
    /* алг цел mod(цел x, цел y) */
    case 0x0014: {
        return Mod(args[0], args[1]);
    }
    /* алг вещ rand(вещ x, вещ y) */
    case 0x0015: {
        return Rand(args[0], args[1]);
    }
    /* алг вещ rnd(вещ x) */
    case 0x0016: {
        return Rnd(args[0]);
    }
    /* алг цел sign(вещ x) */
    case 0x0017: {
        return Sign(args[0]);
    }
    /* алг вещ sin(вещ x) */
    case 0x0018: {
        return Sin(args[0]);
    }
    /* алг вещ sqrt(вещ x) */
    case 0x0019: {
        return Sqrt(args[0]);
    }
    /* алг вещ tg(вещ x) */
    case 0x001a: {
        return Tg(args[0]);
    }
    /* алг вещ МАКСВЕЩ */
    case 0x001b: {
        return MaxReal();
    }
    /* алг цел МАКСЦЕЛ */
    case 0x001c: {
        return MaxInteger();
    }
    /* алг лит вещ_в_лит(вещ x) */
    case 0x001d: {
        return StrOfReal(args[0]);
    }
    /* алг цел время */
    case 0x001e: {
        return Time();
    }
    /* алг цел длин(лит s) */
    case 0x001f: {
        return Length(args[0]);
    }
    /* алг цел код(сим ch) */
    case 0x0020: {
        return Code(args[0]);
    }
    /* алг вещ лит_в_вещ(лит s, рез лог success) */
    case 0x0021: {
        return RealOfString(args[0]);
    }
    /* алг цел лит_в_цел(лит s, рез лог success) */
    case 0x0022: {
        return IntegerOfString(args[0]);
    }
    /* алг сим символ(цел n) */
    case 0x0023: {
        return Symbol(args[0]);
    }
    /* алг сим символ2(цел n) */
    case 0x0024: {
        return UniSymbol(args[0]);
    }
    /* алг лит цел_в_лит(цел n) */
    case 0x0025: {
        return StrOfInteger(args[0]);
    }
    /* алг цел юникод(сим ch) */
    case 0x0026: {
        return Unicode(args[0]);
    }
    default: {
        qFatal("Unknown method id: %x", id);
        return ES_Error;
    }
    }
}


QString St_functPlugin::name() const
{
    // TODO implement non-russian headers
    // (check system locale)
    return QString::fromUtf8("Стандартные функции");
}

QVariant St_functPlugin::result() const
{
    return Res;
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
