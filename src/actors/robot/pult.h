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
#include <QWidget>
#ifndef ROBOT_PULT_H
#define ROBOT_PULT_H
#include "ui_pult.h"
#include "widgets/pultlogger.h"
#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#define SCROLL_STEP 10
#define RESP_PANEL 50
#define LOGGER_BUTTONS 140
#define TEXT_STEP 14

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define TEXT 5




class RoboPult : public QWidget, public Ui::RoboPult
{
	Q_OBJECT
public:
	/**
		 * Конструктор
		 * @param parent ссыка на объект-владелец
		 * @param fl флаги окна
		 */
	RoboPult ( QWidget* parent = 0, Qt::WFlags fl = 0 );
	/**
		 * Деструктор
		 */
	//~RoboPult(){};
    
    bool Link(){return link;};
    pultLogger * Logger;
	pultLogger * pltLogger(){return Logger;};
	public
			slots:
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


	void CenterButton();

signals:
	void goUp();
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
    linkLight * greenLight;
	MainButton* buttUp,*buttDown,*buttLeft,*buttRight;
	MainButton* askStena,*askFree,*buttRad,*buttTemp;
};
#endif