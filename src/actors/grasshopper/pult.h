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
#ifndef PULT_H
#define PULT_H
#include <QWidget>

#include "ui_pult.h"
#include "kuznec.h"
#include "widgets/pultlogger.h"
//#include "network.h"
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
#define TEXTT 5



class  linkLight : public QWidget
{
	Q_OBJECT
	public:
		/**
		 * Конструктор
		 * @param parent ссыка на объект-владелец
		 * 
		 */
		linkLight ( QWidget* parent =0);
		/**
		 * Деструктор
		 */
		~linkLight(){};
		void setLink(bool b){onLine=b;};
		bool link(){return onLine;};
        QString text;
	signals:
	//void pressed();
protected:
 void paintEvent ( QPaintEvent * event );
// void mousePressEvent ( QMouseEvent * event );
 //void mouseReleaseEvent ( QMouseEvent * event );
private:
int posX,posY;
bool onLine;
};


class  MainButton : public QWidget
{
	Q_OBJECT
	public:
		/**
		 * Конструктор
		 * @param parent ссыка на объект-владелец
		 * 
		 */
		MainButton ( QWidget* parent =0);
		/**
		 * Деструктор
		 */
		~MainButton(){};
		void setDirection(int d){direction=d;};
        void setText(QString t){text=t;direction=TEXTT;};
                bool isChecked(){return checked;};
		void setCheckable(bool flag){checkable=true;};
		void setChecked(bool flag)
			{
			checked=flag;
			downFlag=flag;
			repaint();
			};
		bool loadIcon(QString icon);
		
	signals:
	void pressed();
	void clicked();
protected:
 void paintEvent ( QPaintEvent * event );
 void mousePressEvent ( QMouseEvent * event );
 void mouseReleaseEvent ( QMouseEvent * event );
 void enterEvent ( QEvent * event );
 void leaveEvent ( QEvent * event );
  
private:
void drawAddons(QPainter* painter);
int posX,posY;
uint direction;
QImage buttonImageUp,buttonImageDown,buttonIcon;
bool downFlag,checked,checkable,mouseOver,icon;
QWidget* Parent;
QVector<QLine> upArrow,downArrow,leftArrow,rightArrow;
QString text;
};




class GrasshopperPult : public QWidget, public Ui::GrasshopperPult
{
	Q_OBJECT
	public:
		/**
		 * Конструктор
		 * @param parent ссыка на объект-владелец
		 * @param fl флаги окна
		 */
        GrasshopperPult (QDir resDir=QDir(), QWidget* parent = 0, Qt::WindowFlags fl = 0 );
		/**
		 * Деструктор
		 */
        ~GrasshopperPult(){};
    
   bool Link(){return link;};
        pultLogger * Logger;
	pultLogger * pltLogger(){return Logger;};
	KumKuznec* kuznecObj; 
	
//       KNPServer* Server;
 //      void Connect(KNPServer* server);
       void SetAloneMode(){libMode=false;toKumir->setEnabled(false);};
       bool libMode;	
	void AutoClose(){autoClose=true;};
	
public 
	
 slots:
	void noLink();
        void LinkOK();
 
        void newClient(QString);
        void clientDisconnect();

	void Up();
	void Down();
	void Left();
	void Right();
	void ColorUnColor();
	void SwStena();
	void SwSvobodno();
	void TempS();
	void RadS();

	void resetKuznec();
        void showMessage(QString message){label->setText(message);};
        void logToKumir();
	void setStepSizes(int Fwd,int Back)
	{
	
	//qDebug()<<"LCD NUMBER"<<lcdNumber;
	FwdNumber->display(Fwd);
	BackNumber->display(Back);

	};
 signals:
	void goUp();
	void goDown();
	void goLeft();
	void goRight();


	void hasLeftWall();
	void hasRightWall();

	void noUpWall();
	void noDownWall();
	void noLeftWall();
	void noRightWall();

	void Color();

	void Clean();


	void Rad();
	void Temp();
	void logToK();
	void PultCmd(QString text);
	void sendText(QString text);
 protected: 
 virtual void closeEvent ( QCloseEvent * event );
 virtual void paintEvent (QPaintEvent * event);
private:
 bool link;

 linkLight * greenLight;
 MainButton* buttFwd,*buttBack,*turnLeft,*turnRight,*recolorB;
 MainButton* askStena,*askFree,*buttRad,*buttTemp;
 bool autoClose;
 
// KNPServer* Server;
};
#endif
