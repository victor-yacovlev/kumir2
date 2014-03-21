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
#include <QtSvg>
 #include <QGraphicsSvgItem>

//class QAction;
//class QMenu;
//class QTextEdit;
//#define FIELD_SX 500
//#define FIELD_SY 00
#define BORDER_SZ 20

class pultLogger;
class TurtlePult;

class turtle:public QMainWindow
{
      Q_OBJECT

public:
      turtle(QDir mresd);
      ~turtle();
	QGraphicsScene *scene;
      QGraphicsView *view;
 double   step;
 qreal grad;	
 pultLogger* logger;
 TurtlePult* Tpult;
 QDir myresdir;
 void AutoClose(){autoClose=true;close();};
 void showTurtle(){setVisible(true);};
 void hideTurtle(){setVisible(false);};

protected:
 void mousePressEvent(QMouseEvent *event);
 void timerEvent(QTimerEvent *event);
void closeEvent ( QCloseEvent * event );
public slots:
 void rotate();
 bool moveT();
 void TailUp();
 void TailDown();
 void reset();
 void DoRotate(double deg);
 void DoMove(int range);
 void hideTurtle(QPoint pos);
 void Repaint(){
     view->repaint();
     scene->update();};
 signals:
 void Otkaz(QString msg);
 void Ok();

private:
 void loadIniFile();
 void drawTail();
 void showCurTurtle();
 void rotateImages();
 void CreateBorders();
 bool checkPos()
	{
	qreal x,y;
	return checkPos(&x,&y);
	};
  bool checkPos(qreal * x,qreal *y);

QPoint Position;
 QGraphicsSvgItem * t1;
 QGraphicsSvgItem * t2;
 QGraphicsSvgItem * t3;
 QList<QGraphicsLineItem*> lines;
QGraphicsSvgItem * curTurtle;
  void CreateTurtle(void);      
 qreal ang;
 QList<QLineF> desertBorders; 
 bool tail;
 qreal curX,curY;
 qreal zoom,delta;
 qreal AncX,AncY;

 QGraphicsPathItem * Tail;
 QPointF tailPoint;
 int curTurtleId;
 bool autoClose,obod;
 int FIELD_SX;
 int FIELD_SY;
 QBasicTimer timer;
// WHeader* turtleHeader;
};

#endif
