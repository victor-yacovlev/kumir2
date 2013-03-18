/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#include <QtCore>
#include <QtGui>
#include "isometricrobotmodule.h"

namespace ActorIsometricRobot {


IsometricRobotModule::IsometricRobotModule(ExtensionSystem::KPlugin * parent)
    : IsometricRobotModuleBase(parent)
    , window_(new Robot25DWindow(0))
    , robotView_(window_->robotView())
{
}

void IsometricRobotModule::reset()
{
    bool wasAnimated = robotView_->isAnimated();
    robotView_->reset();
    robotView_->setAnimated(wasAnimated);
}

void IsometricRobotModule::setAnimationEnabled(bool enabled)
{
    robotView_->setAnimated(enabled);
}


QWidget* IsometricRobotModule::mainWidget() const
{
    return window_;
}

void IsometricRobotModule::runGoForward()
{
    if (!robotView_->goForward()) {
        setError(robotView_->lastError(QLocale::Russian));
    }
    robotView_->waitForAnimated();
}


void IsometricRobotModule::runTurnRight()
{
    robotView_->turnRight();
    robotView_->waitForAnimated();
}


void IsometricRobotModule::runTurnLeft()
{
    robotView_->turnLeft();
    robotView_->waitForAnimated();
}


void IsometricRobotModule::runDoPaint()
{
    robotView_->doPaint();
    robotView_->waitForAnimated();
}


bool IsometricRobotModule::runIsCellPainted()
{
    return robotView_->isPainted();
}


bool IsometricRobotModule::runIsCellClean()
{
    return ! robotView_->isPainted();
}


bool IsometricRobotModule::runIsWallAhead()
{
    return robotView_->isWall();
}


bool IsometricRobotModule::runIsFreeAhead()
{
    return ! robotView_->isWall();
}

    
} // $namespace
