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
	inline void setDirection(int d){direction=d;}
	inline void setText(const QString &t){text=t;direction=TEXT;}
	inline bool isChecked() const {return checked;}
	inline void setCheckable(bool flag){Q_UNUSED(flag);checkable=true;}
	inline void setChecked(bool flag)
	{
		checked=flag;
		downFlag=flag;
		repaint();
	}
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
	loggerButton (QDir resource_dir=QDir(), QWidget* parent =0);
	/**
		 * Деструктор
		 */
	~loggerButton(){};
	void upArrowType(bool b){isUpArrow=b;};
    void loadButtons(QDir dir);
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
	inline logLine(QString KumCommand,
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
	}
	inline void moveUp()
	{
        textLabel->move(textLabel->x(),textLabel->y()-SCROLL_STEP);
        respLabel->move(respLabel->x(),respLabel->y()-SCROLL_STEP);
	}
	inline void moveDown()
	{
        textLabel->move(textLabel->x(),textLabel->y()+SCROLL_STEP);
        respLabel->move(respLabel->x(),respLabel->y()+SCROLL_STEP);
	}
	inline int pos()
	{
		return textLabel->y();
	}
	inline void removeLabels()
	{
		if(textLabel)delete textLabel;
		if(respLabel)delete respLabel;
	}
    inline QString KumCommand() const {return kumCommand;}
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
	pultLogger (QDir resDir=QDir(), QWidget* parent = 0);
	/**
		 * Деструктор
		 */
	~pultLogger();
	void setSizes(uint w,uint h);
	void Move(uint x,uint y);




	inline        void Show()
	{
        //mainFrame->show();
        //downButton->show();
        //upButton->show();
        //show();
	}
	inline void appendText(QString kumCommand,QString text,QString replay);
	QString log()
	{
		QString toret;
		for(int i=0;i<lines.count();i++)toret+=lines[i].KumCommand();
		return toret;
	}
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
