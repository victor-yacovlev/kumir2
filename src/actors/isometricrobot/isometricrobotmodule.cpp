/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#include <QtCore>
#include <QtGui>
#include <iostream>
#include "isometricrobotmodule.h"
#include "sch_environment.h"

namespace ActorIsometricRobot {


IsometricRobotModule::IsometricRobotModule(ExtensionSystem::KPlugin * parent)
    : IsometricRobotModuleBase(parent)
    , window_(new Robot25DWindow(parent->myResourcesDir(), 0))
    , robotView_(window_->robotView())
{
    connect(m_actionRobot25DLoadEnvironment, SIGNAL(triggered()),
            window_, SLOT(handleLoadAction()));
    connect(m_actionRobot25DResetEnvironment, SIGNAL(triggered()),
            this, SLOT(reset()));
}

QList<ExtensionSystem::CommandLineParameter> IsometricRobotModule::acceptableCommandLineParameters()
{
    return QList<ExtensionSystem::CommandLineParameter>();
}

void IsometricRobotModule::reset()
{
    bool wasAnimated = robotView_->isAnimated();
    robotView_->reset();
    robotView_->setAnimated(wasAnimated);
}

void IsometricRobotModule::loadActorData(QIODevice *source)
{
    QTextStream ts(source);
    ts.setCodec("UTF-8");
    ts.setAutoDetectUnicode(true);
    const QString data = ts.readAll();
    Schema::Environment env;
    Schema::parceJSON(data, env);
    robotView_->loadEnvironment(env);
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
//    window_->statusMessage(__FUNCTION__);
    if (!robotView_->goForward()) {
        setError(robotView_->lastError(QLocale::Russian));
    }
    robotView_->waitForAnimated();
}


void IsometricRobotModule::runTurnRight()
{
//    window_->statusMessage(__FUNCTION__);
    robotView_->turnRight();
    robotView_->waitForAnimated();
}


void IsometricRobotModule::runTurnLeft()
{
//    window_->statusMessage(__FUNCTION__);
    robotView_->turnLeft();
    robotView_->waitForAnimated();
}


void IsometricRobotModule::runDoPaint()
{
//    window_->statusMessage(__FUNCTION__);
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

bool IsometricRobotModule::runIsPainted(const int x, const int y)
{
    return robotView_->isPainted(x - 1, y - 1);
}

bool IsometricRobotModule::runIsMarked(const int x, const int y)
{
    return robotView_->isPointed(x - 1, y - 1);
}

int IsometricRobotModule::runPositionX()
{
    return robotView_->positionX() + 1;
}

int IsometricRobotModule::runPositionY()
{
    return robotView_->positionY() + 1;
}

int IsometricRobotModule::runSizeX()
{
    return robotView_->sizeX();
}

int IsometricRobotModule::runSizeY()
{
    return robotView_->sizeY();
}

    
} // $namespace
