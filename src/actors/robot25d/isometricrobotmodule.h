/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#ifndef ISOMETRICROBOTMODULE_H
#define ISOMETRICROBOTMODULE_H

#include <QtCore>
#include <QtGui>
#include "extensionsystem/kplugin.h"
#include "isometricrobotmodulebase.h"
#include "robot25dwindow.h"

namespace ActorIsometricRobot {

class IsometricRobotModule
    : public IsometricRobotModuleBase
{
    Q_OBJECT
public:
    // Constructor

    IsometricRobotModule(ExtensionSystem::KPlugin * parent);

public slots:
    // Reset actor state before program starts
    void reset();
    // Set animation flag
    void setAnimationEnabled(bool enabled);
    // Actor methods
    void runGoForward();
    void runTurnRight();
    void runTurnLeft();
    void runDoPaint();
    bool runIsCellPainted();
    bool runIsCellClean();
    bool runIsWallAhead();
    bool runIsFreeAhead();

    inline void reloadSettings(ExtensionSystem::SettingsPtr ) { }
    inline void changeGlobalState(ExtensionSystem::GlobalState, ExtensionSystem::GlobalState) {}

public:
    // GUI access methods
    QWidget* mainWidget() const;
    inline QWidget* pultWidget() const { return nullptr; }

private:
    Robot25DWindow * window_;
    Robot25D::RobotView * robotView_;


}; // IsometricRobotModule
    
} // ActorIsometricRobot

#endif // ISOMETRICROBOTMODULE_H
