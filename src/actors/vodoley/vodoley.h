//****************************************************************************
//**
//** Copyright (C) 2004-2008 IMPB RAS. All rights reserved.
//**
//** This file is part of the KuMir.
//**
//** This file may be used under the terms of the GNU General Public
//** License version 2.0 as published by the Free Software Foundation
//** and appearing in the file LICENSE.GPL included in the packaging of
//** this file.
//**
//** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
//** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//**
//****************************************************************************/
#ifndef TURTLE_H
#define TURTLE_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QGraphicsScene>
//#include <QtSvg>
//#include <QGraphicsSvgItem>
#include "dialog.h"

//#include "kumfiledialog.h"
//#include "../../isp_window_header.h"
#define MAX_SIZE 160.0
#define X_SIZE 30
        //class QAction;
        //class QMenu;
        //class QTextEdit;
        class QAction;
class pultLogger;
class VodoleyPult;
class Menzurka:public QGraphicsItem
{

public:
    Menzurka(int x,int y,uint size,float lsize,QMutex* mutex);
    ~Menzurka(){};

    QRectF boundingRect() const
    {
        qreal penWidth = 1;
        return QRectF(-10 - penWidth / 2, -10 - penWidth / 2,
                      20 + penWidth / 2, 20 + penWidth / 2);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    void setGp(int gp)
	{
		qDebug()<<"Gp"<<gp;
		Gp=gp;
		update();
	};
    void setGpY(int gpy)
	{
		qDebug()<<"GpY"<<gpy;
		GpY=gpy;
		update();
	};
	void setCurFill(uint value)
	{
		curFil=value;
	};
	void setNeedFill(uint value)
	{
		needFill=value;
	};
	void setSize(uint value)
	{
		SizeInLiters=value;
	};
	void setLiterSize(float value)
	{
		literSize=value;
	};
	void setYoffset(float value)
	{
		offsetY=value;
	};	

private:

    uint SizeInLiters;
    float literSize;
    uint curFil;
    int  needFill;
    int offsetX;
    int offsetY;
    int Gp,GpY;
    QMutex* M;

};
class NewDialog:public QDialog
{
	Q_OBJECT

public:
	NewDialog();
	~NewDialog(){};	
public slots:

private:
	QFrame * sizeFrame;
	QFrame * fillFrame;
	QFrame * valueFrame;
	QFrame * buttonFrame;
	QToolButton *btnOK;
	QToolButton *btnCancel;
};

class Vodoley:public QMainWindow
{
    Q_OBJECT

public:
    Vodoley();
    ~Vodoley();
	QGraphicsScene *scene;
	QGraphicsView *view;
	int   step;
	qreal grad;
	pultLogger* logger;
	VodoleyPult* pult;
    bool isReady()
    {
     if((CurA()==AfillR)||(CurB()==AfillR)||(CurC()==AfillR))return true;
        else return false;
    };

	void showVodoley()
	{
                show();
		setVisible(true);
	}
	uint CurA() const
	{
		return Curfill[0];
	};
	uint CurB() const
	{
		return Curfill[1];
	};
	uint CurC() const
	{
		return Curfill[2];
	};
	void setBaseFill(int A,int B,int C)
	{
		Curfill[0]=A;
		Curfill[1]=B;
		Curfill[2]=C;

	};
	uint Asize() const {return Maxfill[0];};
	uint Bsize() const {return Maxfill[1];};
	uint Csize() const {return Maxfill[2];};

	uint maxSize()
	{
		uint max=0;
		if (Asize()>Bsize())
			max=Asize();
		else max=Bsize();
		if (max<Csize())
			max=Csize();
		return max;
	};
    bool loadIoDevice(QIODevice * source);
	bool loadFile(QString fileName);
	void AutoClose() {
		autoClose=true;
//		vodHeader->close();
		setVisible(false);
	}
	//Actions

	QAction * actNew;
	QAction * actLoad;
	QAction * actSave;
    void createActions(QList<QAction*> actions);
    bool ready()
    {return ((Curfill[0]==AfillR)||(Curfill[1]==AfillR)||(Curfill[2]==AfillR));};
 
protected:
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent ( QResizeEvent * event );
//    void closeEvent ( QCloseEvent * event );
public slots:
    void reset();

    void newZ();
    void loadZ();
    void saveZ();

    void FillA();
    void FillB();
    void FillC();
    
    void MoveFromTo(uint from,uint to);
    void redraw();

signals:
    void Otkaz(QString msg);
    void Ok();
    void CNull();
    void CNotNull();
    void FileLoaded(QString fname);
private:
    void updateMenzur();
    void updateNeedBirka();

    
   // QMenu * menu;
    //QMenuBar * menuBar;
    void drawTail();
    void showCurTurtle();
    void rotateImages();
    QGraphicsSimpleTextItem* Atext;
    QGraphicsSimpleTextItem* Btext;
    QGraphicsSimpleTextItem* Ctext;

    QLabel* needLabel;
    QFrame*  needFrame;


    QList<QGraphicsLineItem*> lines;

    void CreateVodoley(void);
    void CreateDummyTask();
    qreal ang;
    QList<QLineF> desertBorders;
    bool tail;
    qreal curX,curY;
    qreal zoom,delta;
    qreal AncX,AncY;

    QPointF tailPoint;
    int curTurtleId;

    //VODOLEY
    // uint Asize,Bsize,Csize; //Размер емкости
    uint Afill,Bfill,Cfill;//Сколько  налито изначально
    QList<uint> Curfill;//Сколько сейчас налито
    QList<uint> Maxfill;//Размер емкости
    uint AfillR,BfillR,CfillR;//Сколько должно быть

    Menzurka* Amen;
    Menzurka* Bmen;
    Menzurka* Cmen;
    QMutex mutex;
    QString curDir;
    bool autoClose;

};

#endif
