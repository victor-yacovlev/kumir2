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


qreal ComplexNumbersModule::runRe(const Complex & x)
{
    /* TODO implement me */
    return 0.0;
}


qreal ComplexNumbersModule::runIm(const Complex & x)
{
    /* TODO implement me */
    return 0.0;
}


Complex  ComplexNumbersModule::runOperatorPLUS(const Complex & x, const Complex & y)
{
    /* TODO implement me */
    return Complex ();
}


Complex  ComplexNumbersModule::runOperatorMINUS(const Complex & x, const Complex & y)
{
    /* TODO implement me */
    return Complex ();
}


Complex  ComplexNumbersModule::runOperatorASTERISK(const Complex & x, const Complex & y)
{
    /* TODO implement me */
    return Complex ();
}


Complex  ComplexNumbersModule::runOperatorSLASH(const Complex & x, const Complex & y)
{
    /* TODO implement me */
    return Complex ();
}

    
} // $namespace
