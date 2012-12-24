/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#ifndef COMPLEXNUMBERSMODULE_H
#define COMPLEXNUMBERSMODULE_H

#include <QtCore>
#include <QtGui>
#include "extensionsystem/kplugin.h"
#include "complexnumbersmodulebase.h"

namespace ActorComplexNumbers {

class ComplexNumbersModule
    : public ComplexNumbersModuleBase
{
    Q_OBJECT
public:
    // Constructor

    ComplexNumbersModule(ExtensionSystem::KPlugin * parent);

public slots:
    // Reset actor state before program starts
    void reset();
    // Set animation flag
    void setAnimationEnabled(bool enabled);
    // Actor methods
    qreal runRe(const Complex& x);
    qreal runIm(const Complex& x);
    Complex runOperatorPLUS(const Complex& x, const Complex& y);
    Complex runOperatorMINUS(const Complex& x, const Complex& y);
    Complex runOperatorASTERISK(const Complex& x, const Complex& y);
    Complex runOperatorSLASH(const Complex& x, const Complex& y);
    Complex runOperatorASSIGN(const int x);
    Complex runOperatorASSIGN(const qreal x);
    Complex runOperatorINPUT(const QString& x, bool& ok);
    QString runOperatorOUTPUT(const Complex& x);



}; // ComplexNumbersModule
    
} // ActorComplexNumbers

#endif // COMPLEXNUMBERSMODULE_H
