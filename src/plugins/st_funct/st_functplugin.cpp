#include <QtCore>
#include <QtGui>

#include "st_functplugin.h"
// #include "stdlib/kumstdlib.h"
#include "stdlib/genericinputoutput.h"
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

EvaluationStatus St_functPlugin::evaluate(quint32 id, const QVariantList & args)
{
    optResults.clear();
    return ES_Error;

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
    Kumir::initStandardLibrary();
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
    Res = QVariant(Kumir::Math::abs(x.toDouble()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::ArcCos(const QVariant & x){
    Res = QVariant(Kumir::Math::arccos(x.toDouble()));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::ArcSin(const QVariant & x){
    Res = QVariant(Kumir::Math::arcsin(x.toDouble()));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::ArcCtg(const QVariant & x){
    Res = QVariant(Kumir::Math::arcctg(x.toDouble()));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::ArcTg(const QVariant & x){
    Res = QVariant(Kumir::Math::arctg(x.toDouble()));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Cos(const QVariant & x){
    Res = QVariant(Kumir::Math::cos(x.toDouble()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Ctg(const QVariant & x){
    Res = QVariant(Kumir::Math::ctg(x.toDouble()));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Delay(const QVariant & x){
    return ES_NoResult;
    // TODO implement me
}

EvaluationStatus St_functPlugin::Div(const QVariant & x, const QVariant & y){
    Res = QVariant(Kumir::Math::div(x.toInt(), y.toInt()));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Exp(const QVariant & x){
    Res = QVariant(Kumir::Math::exp(x.toDouble()));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Iabs(const QVariant & x){
    Res = QVariant(Kumir::Math::iabs(x.toInt()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Imax(const QVariant & x, const QVariant & y){
    Res = QVariant(Kumir::Math::imax(x.toInt(), y.toInt()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Imin(const QVariant & x, const QVariant & y){
    Res = QVariant(Kumir::Math::imin(x.toInt(), y.toInt()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Int(const QVariant & x){
    Res = QVariant(Kumir::Math::intt(x.toDouble()));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Irand(const QVariant & x, const QVariant & y){
    Res = QVariant(Kumir::Random::irand(x.toInt(), y.toInt()));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Irnd(const QVariant & x){
    Res = QVariant(Kumir::Random::irnd(x.toInt()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Lg(const QVariant & x){
    Res = QVariant(Kumir::Math::lg(x.toDouble()));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Ln(const QVariant & x){
    Res = QVariant(Kumir::Math::ln(x.toDouble()));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Max(const QVariant & x, const QVariant & y){
    Res = QVariant(Kumir::Math::rmax(x.toDouble(), y.toDouble()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Min(const QVariant & x, const QVariant & y){
    Res = QVariant(Kumir::Math::rmin(x.toDouble(), y.toDouble()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Mod(const QVariant & x, const QVariant & y){
    Res = QVariant(Kumir::Math::mod(x.toInt(), y.toInt()));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Rand(const QVariant & x, const QVariant & y){
    Res = QVariant(Kumir::Random::rrand(x.toDouble(), y.toDouble()));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Rnd(const QVariant & x){
    Res = QVariant(Kumir::Random::rrnd(x.toDouble()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Sign(const QVariant & x){
    Res = QVariant(Kumir::Math::sign(x.toDouble()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Sin(const QVariant & x){
    Res = QVariant(Kumir::Math::sin(x.toDouble()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Sqrt(const QVariant & x){
    Res = QVariant(Kumir::Math::sqrt(x.toDouble()));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Tg(const QVariant & x){
    Res = QVariant(Kumir::Math::tg(x.toDouble()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::MaxReal(){
    Res = QVariant(Kumir::Math::maxreal());
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::MaxInteger(){
    Res = QVariant(Kumir::Math::maxint());
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::StrOfReal(const QVariant & x){
    Res = QVariant(
                QString::fromStdWString(
                Kumir::Converter::realToString(x.toDouble())
                    )
                );
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::StrOfInteger(const QVariant & x){
    Res = QVariant(
                QString::fromStdWString(
                Kumir::Converter::intToString(x.toInt())
                    )
                );
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Time(){
    Res = QVariant(Kumir::System::time());
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Length(const QVariant & x){
    Res = QVariant(Kumir::StringUtils::length(x.toString().toStdWString()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::Code(const QVariant & x){
    Res = QVariant(Kumir::StringUtils::code(x.toString().toStdWString().at(0)));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::RealOfString(const QVariant & x){
    bool ok;
    double value = Kumir::Converter::stringToReal(x.toString().toStdWString(), ok);
    Res = QVariant(value);
    optResults << QVariant(ok);
    return ES_StackRezResult;
}

EvaluationStatus St_functPlugin::IntegerOfString(const QVariant & x){
    bool ok;
    int value = Kumir::Converter::stringToInt(x.toString().toStdWString(), ok);
    Res = QVariant(value);
    optResults << QVariant(ok);
    return ES_StackRezResult;
}

EvaluationStatus St_functPlugin::Symbol(const QVariant & x){
    wchar_t buffer[1];
    buffer[0] = Kumir::StringUtils::symbol(x.toInt());
    Res = QVariant(QString::fromWCharArray(buffer, 1).at(0));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::UniSymbol(const QVariant & x){
    wchar_t buffer[1];
    buffer[0] = Kumir::StringUtils::unisymbol(x.toInt());
    Res = QVariant(QString::fromWCharArray(buffer, 1).at(0));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::Unicode(const QVariant & x){
    Res = QVariant(Kumir::StringUtils::code(x.toString().toStdWString().at(0)));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::FileExists(const QVariant &x) {
    Res = QVariant(Kumir::Files::exist(x.toString().toStdWString()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::FileUnlink(const QVariant &x) {
    Res = QVariant(Kumir::Files::unlinkFile(x.toString().toStdWString()));
    return ES_StackResult;
}

EvaluationStatus St_functPlugin::FileSetEncoding(const QVariant &x) {
    Kumir::Files::setFileEncoding(x.toString().toStdWString());
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_NoResult : ES_Error;
}

EvaluationStatus St_functPlugin::FileOpenForRead(const QVariant &x) {
    Res = QVariant(Kumir::Files::open(x.toString().toStdWString(), Kumir::Files::FM_Read));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::FileOpenForWrite(const QVariant &x) {
    Res = QVariant(Kumir::Files::open(x.toString().toStdWString(), Kumir::Files::FM_Write));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::FileOpenForAppend(const QVariant &x) {
    Res = QVariant(Kumir::Files::open(x.toString().toStdWString(), Kumir::Files::FM_Append));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

EvaluationStatus St_functPlugin::FileClose(const QVariant &x) {
    Kumir::Files::close(x.toInt());
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_NoResult : ES_Error;
}

EvaluationStatus St_functPlugin::FileReset(const QVariant &x) {
    Kumir::Files::reset(x.toInt());
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_NoResult : ES_Error;
}

EvaluationStatus St_functPlugin::FileAtEnd(const QVariant &x) {
    Res = QVariant(Kumir::Files::eof(x.toInt()));
    errText = QString::fromWCharArray(Kumir::Core::getError().c_str());
    return errText.isEmpty()? ES_StackResult : ES_Error;
}

} // end namespace st_funct


Q_EXPORT_PLUGIN(st_funct::St_functPlugin)
