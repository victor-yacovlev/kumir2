/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#include <QtCore>
#include <QtGui>
#include "complexnumbersmodule.h"

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


qreal ComplexNumbersModule::runRe(const Complex& x)
{
    /* TODO implement me */
    return 0.0;
}


qreal ComplexNumbersModule::runIm(const Complex& x)
{
    /* TODO implement me */
    return 0.0;
}


Complex ComplexNumbersModule::runOperatorPLUS(const Complex& x, const Complex& y)
{
    /* TODO implement me */
    return Complex();
}


Complex ComplexNumbersModule::runOperatorMINUS(const Complex& x, const Complex& y)
{
    /* TODO implement me */
    return Complex();
}


Complex ComplexNumbersModule::runOperatorASTERISK(const Complex& x, const Complex& y)
{
    /* TODO implement me */
    return Complex();
}


Complex ComplexNumbersModule::runOperatorSLASH(const Complex& x, const Complex& y)
{
    /* TODO implement me */
    return Complex();
}


Complex ComplexNumbersModule::runOperatorASSIGN(const int x)
{
    /* TODO implement me */
    return Complex();
}


Complex ComplexNumbersModule::runOperatorASSIGN(const qreal x)
{
    /* TODO implement me */
    return Complex();
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
    result.re = re.toDouble(&ok1);
    result.im = im.toDouble(&ok2);
    ok = ok1 && ok2;
    return result;
}


QString ComplexNumbersModule::runOperatorOUTPUT(const Complex& x)
{
    /* TODO implement me */
    return "";
}

    
} // $namespace
