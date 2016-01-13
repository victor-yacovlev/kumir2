/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#include <QtCore>
#include <QtGui>
#include "complexnumbersmodule.h"
#include "stdlib/kumirstdlib.hpp"

namespace ActorComplexNumbers {

ComplexNumbersModule::ComplexNumbersModule(ExtensionSystem::KPlugin * parent)
    : ComplexNumbersModuleBase(parent)
{
    /* TODO 
    implement class Constructor
    */
}

void ComplexNumbersModule::reset()
{
    /* TODO
    This method is called when actor resets its state before program starts.
    */
}

void ComplexNumbersModule::setAnimationEnabled(bool enabled)
{
    /* TODO
    Set internal flag for animation
    */
}

QList<ExtensionSystem::CommandLineParameter> ComplexNumbersModule::acceptableCommandLineParameters()
{
    return QList<ExtensionSystem::CommandLineParameter>();
}

qreal ComplexNumbersModule::runRe(const Complex& x)
{
    return x.re;
}


qreal ComplexNumbersModule::runIm(const Complex& x)
{
    return x.im;
}


Complex ComplexNumbersModule::runOperatorPLUS(const Complex& x, const Complex& y)
{
    Complex result;
    result.re = x.re + y.re;
    result.im = x.im + y.im;
    return result;
}


Complex ComplexNumbersModule::runOperatorMINUS(const Complex& x, const Complex& y)
{
    Complex result;
    result.re = x.re - y.re;
    result.im = x.im - y.im;
    return result;
}


Complex ComplexNumbersModule::runOperatorASTERISK(const Complex& x, const Complex& y)
{
    Complex result;
    result.re = x.re*y.re - x.im*y.im;
    result.im = y.im*x.re + x.re*y.im;
    return result;
}


Complex ComplexNumbersModule::runOperatorSLASH(const Complex& x, const Complex& y)
{
    Complex result;
    result.im = result.re = 0.0;
    if (y.re==0 && y.im==0) {
        setError(QString::fromUtf8("Деление на комплексный нуль"));
        return result;
    }
    qreal factor = y.re*y.re + y.im*y.im;
    result.re = (x.re*y.re+x.im*y.im) / factor;
    result.im = (x.im*y.re-x.re*y.im) / factor;
    return result;
}


Complex ComplexNumbersModule::runOperatorASSIGN(const int x)
{
    Complex result;
    result.re = x;
    result.im = 0;
    return result;
}


Complex ComplexNumbersModule::runOperatorASSIGN(const qreal x)
{
    Complex result;
    result.re = x;
    result.im = 0;
    return result;
}


Complex ComplexNumbersModule::runOperatorINPUT(const QString& x, bool& ok)
{
    static const QRegExp rxPlusMinus("[+-]");
    int p = x.indexOf(rxPlusMinus);
    QString re = "0.0", im = "0.0";
    if (p==-1) {
        if (x.trimmed().endsWith(QChar('i'), Qt::CaseInsensitive))
            im = x.trimmed();
        else
            re = x.trimmed();
    }
    else {
        QString a = x.left(p).trimmed();
        QString b = x.mid(p+1).trimmed();
        if (a.endsWith(QChar('i'), Qt::CaseInsensitive)) {
            im = a;
            re = x[p]+b;
        }
        else {
            im = x[p]+b;
            re = a;
        }
    }
    bool ok1, ok2;
    Complex result;
    im.remove(QChar('i'), Qt::CaseInsensitive);
    im = im.trimmed();
    if (im.endsWith("*")) {
        im = im.left(im.length()-1);
        im = im.trimmed();
    }
    if (im.isEmpty()) {
        im = "1.0";
    }
    result.re = re.toDouble(&ok1);
    result.im = im.toDouble(&ok2);
    ok = ok1 && ok2;
    return result;
}


QString ComplexNumbersModule::runOperatorOUTPUT(const Complex& x)
{
    QString result;
    const QString re = QString::fromStdWString(
                Kumir::Converter::sprintfReal(x.re,'.',false,0,0,0)
                );
    const QString im = QString::fromStdWString(
                Kumir::Converter::sprintfReal(x.im,'.',false,0,0,0)
                );
    result = re;
    result += im.startsWith('-')? "" : "+";
    result += im + "i";
    return result;
}

    
} // $namespace
