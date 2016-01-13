/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#ifndef COMPLEXNUMBERSMODULE_H
#define COMPLEXNUMBERSMODULE_H

#include <QtCore>
#if QT_VERSION >= 0x050000
#   include <QtWidgets>
#else
#   include <QtGui>
#endif
#include "extensionsystem/kplugin.h"
#include "complexnumbersmodulebase.h"

namespace ActorComplexNumbers {

class ComplexNumbersModule
    : public ComplexNumbersModuleBase
{
    Q_OBJECT
public:
    // Constructor
    static QList<ExtensionSystem::CommandLineParameter> acceptableCommandLineParameters();
    ComplexNumbersModule(ExtensionSystem::KPlugin * parent);
    inline void reloadSettings(ExtensionSystem::SettingsPtr, const QStringList & ) {}
    inline void changeGlobalState(ExtensionSystem::GlobalState, ExtensionSystem::GlobalState) {}

public slots:
    // Reset actor state before program starts
    void reset();
    inline void terminateEvaluation() {}
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
