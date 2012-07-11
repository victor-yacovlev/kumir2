/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#ifndef ROBOTMODULE_H
#define ROBOTMODULE_H

#include <QtCore>
#include <QtGui>
#include "extensionsystem/kplugin.h"
#include "robotmodulebase.h"

namespace ActorRobot {

class RobotModule
	: public RobotModuleBase
{
	Q_OBJECT
public:
	// Constructor
	RobotModule(ExtensionSystem::KPlugin * parent);
public slots:
	// Reset actor state before program starts
	void reset();
	// Set animation flag
	void setAnimationEnabled(bool enabled);
	// Actor methods
	void runGoUp();
	void runGoDown();
	void runGoLeft();
	void runGoRight();
	void runDoPaint();
	bool runIsWallAtTop();
	bool runIsWallAtBottom();
	bool runIsWallAtLeft();
	bool runIsWallAtRight();

public:
	// GUI access methods
	QWidget* mainWidget() const;
	QWidget* pultWidget() const;

private:
    QWidget* m_mainWidget;
    QWidget* m_pultWidget;

}; // RobotModule
	
} // ActorRobot

#endif // ROBOTMODULE_H
