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
#include "remotecontrol.h"

namespace ActorIsometricRobot {


IsometricRobotModule::IsometricRobotModule(ExtensionSystem::KPlugin * parent)
    : IsometricRobotModuleBase(parent)
    , window_(0)
    , robotView_(0)
    , parentObject_(parent)
    , remoteControl_(0)
    , remoteControlWidget_(0)
{   
}

void IsometricRobotModule::createGui()
{
    window_ = new Robot25DWindow(parentObject_->myResourcesDir(), 0);
    remoteControlWidget_ = new QWidget(0);
    QVBoxLayout*  rcLayout = new QVBoxLayout;
    remoteControlWidget_->setLayout(rcLayout);
    const QString rcFileName = parentObject_->myResourcesDir().absoluteFilePath("robot25d-rc.svg");
    remoteControl_ = new SvgRemoteControl(parentObject_, this, rcFileName, remoteControlWidget_);
    rcLayout->addWidget(remoteControl_);

    robotView_ = window_->robotView();
    connect(m_actionRobot25DLoadEnvironment, SIGNAL(triggered()),
            window_, SLOT(handleLoadAction()));
    connect(m_actionRobot25DResetEnvironment, SIGNAL(triggered()),
            this, SLOT(reset()));

}

QString IsometricRobotModule::initialize(const QStringList &configurationParameters, const ExtensionSystem::CommandLine &)
{
    if (!configurationParameters.contains("tablesOnly")) {
        createGui();
    }
    return "";
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
    setError("");
//    window_->statusMessage(__FUNCTION__);
    if (!robotView_->goForward()) {
        setError(robotView_->lastError(QLocale::Russian));
    }
    robotView_->waitForAnimated();
}


void IsometricRobotModule::runTurnRight()
{
    setError("");
//    window_->statusMessage(__FUNCTION__);
    robotView_->turnRight();
    robotView_->waitForAnimated();
}


void IsometricRobotModule::runTurnLeft()
{
    setError("");
//    window_->statusMessage(__FUNCTION__);
    robotView_->turnLeft();
    robotView_->waitForAnimated();
}


void IsometricRobotModule::runDoPaint()
{
    setError("");
//    window_->statusMessage(__FUNCTION__);
    robotView_->doPaint();
    robotView_->waitForAnimated();
}


bool IsometricRobotModule::runIsCellPainted()
{
    setError("");
    return robotView_->isPainted();
}

bool IsometricRobotModule::runIsFlagged(int x, int y)
{
    setError("");
    return robotView_->isFlagged(x, y);
}


bool IsometricRobotModule::runIsCellClean()
{
    setError("");
    return ! robotView_->isPainted();
}


bool IsometricRobotModule::runIsWallAhead()
{
    setError("");
    return robotView_->isWall();
}


bool IsometricRobotModule::runIsFreeAhead()
{
    setError("");
    return ! robotView_->isWall();
}

bool IsometricRobotModule::runIsPainted(const int x, const int y)
{
    setError("");
    return robotView_->isPainted(x - 1, y - 1);
}

bool IsometricRobotModule::runIsMarked(const int x, const int y)
{
    setError("");
    return robotView_->isPointed(x - 1, y - 1);
}

int IsometricRobotModule::runPositionX()
{
    setError("");
    return robotView_->positionX() + 1;
}

int IsometricRobotModule::runPositionY()
{
    setError("");
    return robotView_->positionY() + 1;
}

int IsometricRobotModule::runSizeX()
{
    setError("");
    return robotView_->sizeX();
}

int IsometricRobotModule::runSizeY()
{
    setError("");
    return robotView_->sizeY();
}

    
} // $namespace
