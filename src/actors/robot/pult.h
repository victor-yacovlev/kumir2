/****************************************************************************
**
** Copyright (C) 2004-2008 NIISI RAS. All rights reserved.
**
** This file is part of the KuMir.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef ROBOT_PULT_H
#define ROBOT_PULT_H

#include "ui_pult.h"

class pultLogger;
class linkLight;
class MainButton;

class RoboPult : public QWidget, public Ui::RoboPult
{
	Q_OBJECT
public:
	/**
		 * Конструктор
		 * @param parent ссыка на объект-владелец
		 * @param fl флаги окна
		 */
	RoboPult(QWidget* parent = 0, Qt::WindowFlags fl = 0);
	~RoboPult();

	bool Link() { return link; }
	pultLogger *Logger;
	pultLogger *pltLogger() { return Logger; }

public slots:
	void noLink();
	void LinkOK();

	void Up();
	void Down();
	void Left();
	void Right();

	void SwStena();
	void SwSvobodno();
	void TempS();
	void RadS();
	void switchButt();

	void CenterButton();
	void clearLog();

signals:
	void goUp();
	void robReset();
	void goDown();
	void goLeft();
	void goRight();

	void hasUpWall();
	void hasDownWall();
	void hasLeftWall();
	void hasRightWall();

	void noUpWall();
	void noDownWall();
	void noLeftWall();
	void noRightWall();

	void Color();

	void Clean();
	void Colored();

	void Rad();
	void Temp();
	void PultCmd(QString text);

private:
	bool link;
	void paintEvent(QPaintEvent *);
	linkLight  *greenLight;
	MainButton *buttUp, *buttDown, *buttLeft, *buttRight, *buttCenter;
	MainButton *askStena, *askFree, *buttRad, *buttTemp;
};
#endif
