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
#define TEXT 5



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
		void setText(QString t){text=t;direction=TEXT;};
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



class  loggerButton : public QWidget
{
	Q_OBJECT
	public:
		/**
		 * Конструктор
		 * @param parent ссыка на объект-владелец
		 * 
		 */
		loggerButton ( QWidget* parent =0);
		/**
		 * Деструктор
		 */
		~loggerButton(){};
		void upArrowType(bool b){isUpArrow=b;};
	signals:
	void pressed();
protected:
 void paintEvent ( QPaintEvent * event );
 void mousePressEvent ( QMouseEvent * event );
 void mouseReleaseEvent ( QMouseEvent * event );
private:
int posX,posY;
bool isUpArrow;
QImage buttonImageUp,buttonImageDown;
bool downFlag;
QWidget* Parent;
QVector<QLine> upArrow,downArrow;
};


class logLine
{
public:
   logLine(QString KumCommand,
	   QString LogCommand,
	   QString React,QFrame* frame,QFrame* respFrame,uint pos)
	{
      kumCommand=KumCommand;
      logCommand=LogCommand;
      react=React;
      textLabel=new QLabel(frame);
      textLabel->setText(logCommand);
      textLabel->move(4,pos);
      textLabel->resize(120,20);
      textLabel->show();
            
      respLabel=new QLabel(respFrame);
      respLabel->setText(React);
      respLabel->move(4,pos);
      respLabel->resize(RESP_PANEL,20);
      respLabel->show();	
	};
   void moveUp()
	{
        textLabel->move(textLabel->x(),textLabel->y()-SCROLL_STEP);
	respLabel->move(respLabel->x(),respLabel->y()-SCROLL_STEP);
	};
   void moveDown()
	{
        textLabel->move(textLabel->x(),textLabel->y()+SCROLL_STEP);
        respLabel->move(respLabel->x(),respLabel->y()+SCROLL_STEP);
	};
   int pos()
	{
	return textLabel->y();
	};
   int removeLabels()
	{
	if(textLabel)delete textLabel;
	if(respLabel)delete respLabel;
    return 0;
	};
    QString KumCommand(){return kumCommand;};
private:
QString kumCommand;
QString logCommand;
QString react;
QLabel * textLabel;
QLabel * respLabel;
};
class pultLogger : public QWidget
{
	Q_OBJECT
	public:
		/**
		 * Конструктор
		 * @param parent ссыка на объект-владелец
		 * @param fl флаги окна
		 */
		pultLogger ( QWidget* parent = 0);
		/**
		 * Деструктор
		 */
		~pultLogger();
	void setSizes(uint w,uint h);
        void Move(uint x,uint y);

  


        void Show()
	{
        //mainFrame->show();
	//downButton->show();
	//upButton->show();
	//show();
	};
	void appendText(QString kumCommand,QString text,QString replay);
        QString log()
		{
		QString toret;
		for(int i=0;i<lines.count();i++)toret+=lines[i].KumCommand()+"\n";
		return toret;
		};
public slots:
void upBtnPressed();
void downBtnPressed();
void ClearLog();
void CopyLog();
private:
QFrame * mainFrame;
QFrame * dummyFrame;
QFrame * respFrame;
int W,H;
int pos;
//QLabel * testLabel;
//QFrame * mainFrame;
QList<logLine> lines;
int buttonSize;
loggerButton* downButton;
loggerButton* upButton;
};

class GrasshopperPult : public QWidget, public Ui::TurtlePult
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
