/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#include <QtCore>
#include <QtGui>
#include "robotmodule.h"

namespace ActorRobot {

RobotModule::RobotModule(ExtensionSystem::KPlugin * parent)
	: RobotModuleBase(parent)
{
	/* TODO 
	implement class Constructor
	*/
}

void RobotModule::reset()
{
	/* TODO
	This method is called when actor resets its state before program starts.
	*/
}

void RobotModule::setAnimationEnabled(bool enabled)
{
	/* TODO
	Set internal flag for animation
	*/
}


QWidget* RobotModule::mainWidget() const
{
	/* TODO
	This method should return a pointer to main widget.
	NOTE: This metod should NOT create main widget -- just return!
	*/
	return 0;
}

QWidget* RobotModule::pultWidget() const
{
	/* TODO
	This method should return a pointer to pult widget.
	NOTE: This metod should NOT create pult widget -- just return!
	*/
	return 0;
}

void RobotModule::runGoUp()
{
	/* TODO implement me */
    qDebug() << "Robot up";
    sleep(2);
	return;
}


void RobotModule::runGoDown()
{
	/* TODO implement me */
    qDebug() << "Robot down";
    sleep(2);
	return;
}


void RobotModule::runGoLeft()
{
	/* TODO implement me */
    qDebug() << "Robot left";
    sleep(2);
	return;
}


void RobotModule::runGoRight()
{
	/* TODO implement me */
    qDebug() << "Robot right";
    sleep(2);
	return;
}


void RobotModule::runDoPaint()
{
	/* TODO implement me */
	return;
}


bool RobotModule::runIsWallAtTop()
{
	/* TODO implement me */
	return false;
}


bool RobotModule::runIsWallAtBottom()
{
	/* TODO implement me */
	return false;
}


bool RobotModule::runIsWallAtLeft()
{
	/* TODO implement me */
	return false;
}


bool RobotModule::runIsWallAtRight()
{
	/* TODO implement me */
	return false;
}

	
} // $namespace
