//
//  pultlogger.h
//  Kumir2
//
//  Created by Denis Khachko on 22.05.14.
//  Copyright (c) 2014 NIISI. All rights reserved.
//

#ifndef Kumir2_pultlogger_h
#define Kumir2_pultlogger_h
#include <QWidget>


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
#ifdef WIDGETS_LIBRARY
#define WIDGETS_EXPORT Q_DECL_EXPORT
#else
#define WIDGETS_EXPORT Q_DECL_IMPORT
#endif

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
class WIDGETS_EXPORT pultLogger : public QWidget
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
	inline void appendText(QString kumCommand,QString text,QString replay)
    {
        while(pos>H-68)downBtnPressed();
        lines.append(logLine(kumCommand,text,replay,dummyFrame,respFrame,pos));
        pos=pos+TEXT_STEP;
    }
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




#endif
