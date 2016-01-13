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

#include "ui_pult.h"
#include "turtle.h"
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
#define TEXTT 5



class OvenTimer : public QWidget
{
    Q_OBJECT

public:
    OvenTimer(QWidget *parent = 0);

    void setDuration(int secs);
    int duration() const;
    void draw(QPainter *painter);

signals:
    void angChange(int value);

public slots:
    void setValue(int value);
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent ( QMouseEvent * event ); 
    void mouseReleaseEvent ( QMouseEvent * event );
private:
    //  QDateTime finishTime;
    //  QTimer *updateTimer;
    //  QTimer *finishTimer;
    bool mouseFlag;
    QPointF old_mouse_pos;
    int gradValue; 
    int oldValue;
};




class TurtlePult : public QWidget, public Ui::TurtlePult 
{
	Q_OBJECT
public:
	/**
		 * Конструктор
		 * @param parent ссыка на объект-владелец
		 * @param fl флаги окна
		 */
    TurtlePult (QDir resDir=QDir(), QWidget* parent = 0, Qt::WindowFlags fl = 0 );
	/**
		 * Деструктор
		 */
	inline ~TurtlePult(){};
    
    bool Link(){return link;};
    pultLogger * Logger;
	pultLogger * pltLogger(){return Logger;};
	turtle* turtleObj; 
	bool libMode;
	//KNPServer* form_kumir;

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

	void SwStena();
	void SwSvobodno();
	void TempS();
	void RadS();

	void resetTurtle();
	void showMessage(QString message){label->setText(message);};
	void logToKumir();
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
	void sendText(QString);
protected:
	virtual void closeEvent ( QCloseEvent * event );
    void paintEvent(QPaintEvent *event);
private:
    bool link;
    bool autoClose;
    linkLight * greenLight;
    MainButton* buttFwd,*buttBack,*turnLeft,*turnRight;
    MainButton* askStena,*askFree,*buttRad,*buttTemp;
    OvenTimer * GradInput;
   
};
