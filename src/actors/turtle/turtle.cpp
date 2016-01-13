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


#include <QtGui>
#include "turtle.h"

#include "pult.h"
#include <QTextEdit>
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>
static const double Pi = 3.14159265358979323846264338327950288419717;
turtle::turtle(QDir mresd)
{
    myresdir=mresd;
    FIELD_SX=500;
    FIELD_SY=500;
   loadIniFile();


	autoClose=false;
//        turtleHeader=new WHeader();
        scene = new QGraphicsScene(this);
        view=new QGraphicsView(this);
        view->setScene(scene);
        setContentsMargins(0,0,0,0);
        view->resize(FIELD_SX+2*BORDER_SZ+5,FIELD_SY+2*BORDER_SZ+5);
        view->setSceneRect(-(FIELD_SX/2+BORDER_SZ),-(FIELD_SY/2+BORDER_SZ),FIELD_SX+2*BORDER_SZ,FIELD_SY+2*BORDER_SZ);

//        view->move(1,HEADER_SIZE);
        view->setFixedSize(FIELD_SX+2*BORDER_SZ+5,FIELD_SY+2*BORDER_SZ+5);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setFixedSize(FIELD_SX+2*BORDER_SZ+5,FIELD_SY+2*BORDER_SZ+5);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        view->setViewportUpdateMode (QGraphicsView::NoViewportUpdate);

//        turtleHeader->setChildWidget(this);
        setCentralWidget(view);
//        turtleHeader->setWMTitle(QString::fromUtf8("Черепаха"));
        setWindowTitle(QString::fromUtf8("Черепаха"));
//        connect(turtleHeader,SIGNAL(mousePress(QPoint)),this,SLOT(hideTurtle(QPoint)));
//scene->addLine(0,0,1000,300);
//QPalette PallGrey;
//PallGrey.setBrush(QPalette::Window,QBrush(QColor(140,140,100)));
//mw->setTitle(QString::fromUtf8("Черепаха"));
scene->setBackgroundBrush(QBrush(QColor(180,180,10)));
//scene->setPalette (PallGrey);

   //desertBorders.append(QLineF(260-delta,260-delta,260-delta,-220+delta));//Правый край
  // desertBorders.append(QLineF(-270,280-delta,270,280-delta));//нижний край 
  // desertBorders.append(QLineF(-225+delta,260-delta,-225+delta,-225+delta));//левый край  
  // desertBorders.append(QLineF(-250,-220+delta,260,-220+delta));//верхний край 

ang=0;	
step=30;
tail=true;
curX=0;
curY=0;
curTurtleId=1;
zoom=0.2;
setWindowTitle(QString::fromUtf8("Черепаха"));
delta=100*zoom;
 desertBorders.append(QLineF(-FIELD_SX/2,FIELD_SY/2,FIELD_SX/2,FIELD_SY/2));//Низ
 desertBorders.append(QLineF(-FIELD_SX/2-10,-FIELD_SY/2,FIELD_SX/2,-FIELD_SY/2));//вверх
 desertBorders.append(QLineF(FIELD_SX/2,-FIELD_SY/2,FIELD_SX/2,FIELD_SY/2));
 desertBorders.append(QLineF(-FIELD_SX/2,-FIELD_SY/2,-FIELD_SX/2,FIELD_SY/2));//лево
 
 CreateBorders();
// 	for(int i=0;i<desertBorders.count();i++)
// 	   {
// 		QGraphicsLineItem* bortLine=new QGraphicsLineItem(desertBorders[i]);
// 		
// 		bortLine->setPen(redPen);
// 		scene->addItem(bortLine);
// 	   };
	CreateTurtle();
	drawTail();
    timer.start(50,this);
}
void turtle::loadIniFile()
{
     FIELD_SX=500;
     FIELD_SY=500;
    return;
 

};
void turtle::CreateBorders()
{

//cross1->show();
//cross2->show();
     QBrush blueBr(QColor(30,30,250));
 blueBr.setStyle(Qt::Dense3Pattern);
 QGraphicsRectItem* leftBorder=new QGraphicsRectItem (-(FIELD_SX/2+BORDER_SZ),-(FIELD_SY/2+BORDER_SZ),BORDER_SZ,FIELD_SY+BORDER_SZ);
  leftBorder->setBrush(blueBr);
   leftBorder->setPen(Qt::NoPen);
   leftBorder->setZValue(1);
  scene->addItem(leftBorder);

 QGraphicsRectItem* downBorder=new QGraphicsRectItem (-(FIELD_SX/2+BORDER_SZ),FIELD_SY/2,FIELD_SX+2*BORDER_SZ,BORDER_SZ);
downBorder->setBrush(blueBr);
  downBorder->setPen(Qt::NoPen);
  downBorder->setZValue(1);
  scene->addItem(downBorder);
 QGraphicsRectItem* rightBorder=new QGraphicsRectItem (FIELD_SX/2,-(FIELD_SY/2+BORDER_SZ),BORDER_SZ,FIELD_SY+BORDER_SZ);
 rightBorder->setBrush(blueBr);
 rightBorder->setZValue(1);
  rightBorder->setPen(Qt::NoPen);
  scene->addItem(rightBorder);
 QGraphicsRectItem* upBorder=new QGraphicsRectItem (-(FIELD_SX/2+BORDER_SZ),-(FIELD_SY/2+BORDER_SZ),FIELD_SX+BORDER_SZ,BORDER_SZ);
 upBorder->setBrush(blueBr);
  upBorder->setZValue(1);
 upBorder->setPen(Qt::NoPen);
  scene->addItem(upBorder);
// desertBorders.append(QLineF(-225-delta,260-delta,-225+delta,-225+delta));

}
//-----------------------------------
void turtle::CreateTurtle(void)
{
    obod=false;
ang=0;	
step=30;
tail=true;
curX=0;
curY=0;
t1= new QGraphicsSvgItem(myresdir.absoluteFilePath("Trtl1.svg"));
scene->addItem(t1);

t2= new QGraphicsSvgItem(":/trtl_obod.svg");
//scene->addItem(t2); 
t3= new QGraphicsSvgItem(":/img/Trtl1.svg");
//scene->addItem(t3);

//t1->moveBy(100,100);

QPen redPen(QColor(250,10,10));



		//blackPen.setWidth(3);


//curTurtle->moveBy(-curTurtle->sceneBoundingRect().height () /2,
   //               -curTurtle->sceneBoundingRect().width ()/2);
showCurTurtle();
//qDebug()<<"moveBy"<<-curTurtle->sceneBoundingRect().width ()/2;
AncX=(curTurtle->sceneBoundingRect().width()/2)-7;
AncY=(curTurtle->sceneBoundingRect().height()/2)+140;
//qDebug()<<"AncX"<<AncX<<" AncY"<<AncY;

curX=AncX*zoom;curY=AncY*zoom;
t1->moveBy(-curX,-curY);
curX=0;curY=0;
t1->setTransform(QTransform().translate(AncX*zoom, AncY*zoom).rotate(360).translate(-AncX*zoom, -AncY*zoom));
/* t1->scale(zoom ,zoom); */ t1->setScale(zoom);

}
//-----------------------------------
void turtle::rotate(void)
{
   
ang+=grad;
//qDebug()<<"ang"<<ang;
if(ang<0)ang=360+ang;
if(ang>360)ang=ang-360;


rotateImages();
/* Tail->rotate(grad); */ Tail->setRotation(grad);
showCurTurtle();
}
void turtle::rotateImages()
{
    //qDebug()<<"Ang"<<ang;
    if ( ang!=60 && ang!=120 && ang!=240 && ang!=320 ) {
        t1->setTransform(QTransform().translate(AncX*zoom, AncY*zoom).rotate(ang).translate(-AncX*zoom, -AncY*zoom));
        //    t1->setRotation(ang);
    }else
    {
        //qDebug()<<"Ang bugfix"<<ang;
        t1->setTransform(QTransform().translate(AncX*zoom, AncY*zoom).rotate(ang-1).translate(-AncX*zoom, -AncY*zoom));
        //     t1->setRotation(ang);
    };
    /* t1->scale(zoom,zoom); */ t1->setScale(zoom);

//view->update();

};

bool turtle::moveT(void)
{
   
bool toret=true;
qreal oldX=curX;
qreal oldY=curY;
qreal moveX=step*sin(ang*(Pi/180));
qreal moveY=-step*cos(ang*(Pi/180));
if(!checkPos())
   {
   	checkPos(&curX,&curY);
	moveX=curX-oldX;
	moveY=curY-oldY;
	qDebug()<<"CurX"<<curX<<" CurY"<<curY;

	
	toret=false;
   }else
   { 
   curX=curX+step*sin(ang*(Pi/180));
   curY=curY-step*cos(ang*(Pi/180));
   };
t1->moveBy(moveX,moveY);


Tail->moveBy(moveX,moveY);
if(tail){
lines.append(new QGraphicsLineItem(oldX,oldY,curX,curY));//Add line to lines list 
scene->addItem(lines.last());};
showCurTurtle();
//view->update();

return toret;
}

void turtle::TailUp()
  {
  tail=false;
  Tail->hide();
  };
void turtle:: TailDown()
  {
 tail=true;
  Tail->show();

  };

void turtle::drawTail()
 {
 QPainterPath myPath;
 myPath.cubicTo(QPointF(-10*2,13*2),QPointF(19,17),QPointF(0,-1) );
 
 Tail = new QGraphicsPathItem();
 Tail->setPath(myPath);
 Tail->setBrush(QBrush(Qt::SolidPattern));
// Tail->scale(0.5,0.5);
// Tail->rotate(180);
 Tail->setScale(0.5);
 Tail->setRotation(180);
 Tail->setPos(curX,curY);
 scene->addItem(Tail);
 };


void turtle::showCurTurtle()
 {
  curTurtle=t1;
  return;
  if(curTurtleId==1)
            {
	  t1->show();
	  t2->hide();
	  t3->hide();
	  curTurtle=t1;
	  qDebug()<<"t1";
            };

  if(curTurtleId==2)
            {
	  t1->show();
	  t2->hide();
	  t3->hide();
          curTurtle=t2;
	  qDebug()<<"t2";

            };
  if(curTurtleId==3)
            {
	  t1->show();
	  t2->hide();
	  t3->hide();
          curTurtle=t3;
	  qDebug()<<"t3";

            };
 curTurtleId++;
 if(curTurtleId>3)curTurtleId=1;

 };



bool turtle::checkPos(qreal* x,qreal *y)
 {

 
 qreal futurePosX=curX+step*sin(ang*(Pi/180));
 qreal futurePosY=curY-step*cos(ang*(Pi/180));
 QLineF turtleStepLine(curX,curY,futurePosX,futurePosY);
 QPointF endPos;
  for(int i=0;i<desertBorders.count();i++)//Проверяем выход за границу поля
	{
	if(turtleStepLine.intersect (desertBorders[i], &endPos)==QLineF::BoundedIntersection)
		{
		*x=endPos.x();
		*y=endPos.y();
		if(*x>0)*x=*x-0.00001;
		if(*x<0)*x=*x+0.00001;
		if(*y>0)*y=*y-0.00001;
		if(*y<0)*y=*y+0.00001;

		qDebug()<<"Turtle out of field! Pos:"<<endPos;
		return false;
		};
	};
    

 return true;
 };
void turtle::reset()
{
QList<QGraphicsItem *> items=scene->items();
for(int i=0;i<items.count();i++)scene->removeItem(items[i]);
  CreateBorders();
lines.clear();
	CreateTurtle();

	drawTail();
	showCurTurtle();
};
turtle::~turtle()
{

}
 void turtle:: DoRotate(double deg)
	{
        grad=deg;
        rotate();
	};
 void turtle:: DoMove(int range)
        {
   
	step=range;
	moveT();
	};

 void turtle::mousePressEvent(QMouseEvent *event)
 {
	QPointF point = event->pos() - rect().center();
//	qDebug()<<"TailPoint:"<<point;
//	tailPoint=point;
//	showCurTurtle();
    hideTurtle(event->pos());


 };
void turtle::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timer.timerId()) {
        Repaint();
    }
};
void turtle:: closeEvent ( QCloseEvent * event )
{
qDebug()<<"libM"<<Tpult->libMode<<" autoClose"<<autoClose;
if ((Tpult->libMode)||(autoClose))
	{
	close();
	event->accept();
	return;
	};
int ret = QMessageBox::warning(this, QString::fromUtf8("Черепаха"),
                        QString::fromUtf8("Закрыть исполнитель черепаха?"),
                        QMessageBox::Yes | QMessageBox::Default,
                        QMessageBox::No,
                        QMessageBox::Cancel | QMessageBox::Escape);
     if (ret == QMessageBox::Yes) {
	Tpult->AutoClose();
	 Tpult->close();
         event->accept();
     } else {
         event->ignore();
     }
};
void turtle::hideTurtle(QPoint pos)
{
    qDebug()<<"hideTurtle"<<pos;
    if(pos.x()>25 && pos.y()>25) {
        if(!obod) {
            t1->hide();
            t2->show();
            obod=true;
            qDebug()<<"t2";
        }
        else {
            t1->show();
            obod=false;
        }
    }

};
