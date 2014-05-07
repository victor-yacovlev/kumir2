//****************************************************************************
//**
//** Copyright (C) 2004-2014 IMPB RAS. All rights reserved.
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

//begin 14-02-2008

#include "kuznec.h"
#include <math.h>
#include "dialog.h"
#include "pult.h"
//#include "kumfiledialog.h"
 #include <QGraphicsSimpleTextItem>
class DrawTopFrame
	: public QFrame
{
	protected:
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
                //void mouseMoveEvent(QMouseEvent *event);
	private:
		bool moving;
};

void DrawView:: scrollContentsBy ( int dx, int dy )
{
//qDebug() << QString("SCROLL EVENT ") << dx << " " << dy;
    QGraphicsView::scrollContentsBy ( dx,  dy );
}


DrawView::DrawView(QGraphicsScene *scene, QWidget *window)
	: QGraphicsView(scene, window)
{
}


KumScene::KumScene(QWidget *parent)
	: QGraphicsScene(parent)
{
};


void KumScene::mousePressEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
bool LeftButtonFlag;
//if (mouseEvent->type() == QEvent::NonClientAreaMouseButtonDblClick)

//qWarning("doubleclick %i",mouseEvent->type());
//if (mouseEvent->type() == QEvent::GraphicsSceneMouseDoubleClick)
//{
//qWarning("doubleclick");
//}
if (mouseEvent->button() == 0x00000001)
LeftButtonFlag = true;
else
LeftButtonFlag = false;

QPointF posScene=mouseEvent->scenePos();
QPointF pos=mouseEvent->screenPos();
qreal x=pos.x();
qreal y=pos.y();
qreal xScene=posScene.x();
qreal yScene=posScene.y();


//qWarning("RobotPressEvent");
emit MousePress(x,y,LeftButtonFlag,xScene,yScene);
}; 

void KumScene::mouseReleaseEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
bool LeftButtonFlag;
//qWarning("robot release");
if (mouseEvent->button() == 0x00000001)
LeftButtonFlag = true;
else
LeftButtonFlag = false;

//QPointF pos=mouseEvent->scenePos();
QPointF pos=mouseEvent->screenPos();
qreal x=pos.x();
qreal y=pos.y();
emit MouseRelease(x,y,LeftButtonFlag);

}

void KumScene::mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
bool LeftButtonFlag;
//qWarning("robot move");
if (mouseEvent->button() == 0x00000001)
LeftButtonFlag = true;
else
LeftButtonFlag = false;

//QPointF pos=mouseEvent->scenePos();
QPoint pos=mouseEvent->screenPos();
int x=pos.x();
int y=pos.y();
//QPointF posScene=mouseEvent->scenePos();
emit MouseMove(x,y,LeftButtonFlag);
QGraphicsScene::mouseMoveEvent(mouseEvent);
}
//------------------------
void KumScene::wheelEvent( QGraphicsSceneWheelEvent * wheelEvent )
{

int Delta = wheelEvent->delta();
//qWarning("wheelEvent %i",Delta);
emit MouseWheel(Delta);

}

KuznSled::KuznSled(int start,int fin)
{
St=start;
Fn=fin;
Intens=0; 
};
void KuznSled::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget)
     {
	QPen black(QColor(Intens,Intens,Intens));
	int H=32;
	if(abs(Fn-St)>128)H=64;
	qDebug()<<"ABS"<<abs(Fn-St);
	 painter->setPen(black);
         painter->drawArc(St,0,Fn,32,0,360*8);
		//qDebug()<<"SLED PAINT";
     };


//-------------------------------------------------------------------------------

void KumKuznec::showHideWindow(bool show)//Show - true
{
MV->showNormal();


};

void KumKuznec::Close()
{
    qDebug()<<"KUZNEC CLOSE Close";
if((Kpult->libMode)||(autoClose))
        {
         close();

         return;
        };
 Kpult->close();

}
void KumKuznec::resizeEvent ( QResizeEvent * event )
 {
 qDebug()<<"RESIZE"<<event->size(); 
 qDebug()<<"RESIZE old:"<<event->oldSize(); 
   if(event->oldSize()==QSize(-1,-1))return;
 int Delta=SizeX;
 SizeX=event->size().width();
 //SizeY=event->size().height ()-MenuHigth;

 Delta-=SizeX;
 qDebug()<<"Delta"<<Delta;
 SetWindowSize(10,10,SizeX,SizeY);
 resize(event->size());
 qDebug()<<"SizeX"<<SizeX;
 
 }; 
//-----------------------------------------------------

KumKuznec::KumKuznec()
{

autoClose=false;
#if QT_VERSION >= 0x050000
curDir = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
#else
curDir = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#endif
if (!QDir(curDir).exists()) {
    QDir::root().mkpath(curDir);
}
curMode=4;
moving=false;

MV=new QMainWindow();
MV->setAttribute(Qt::WA_KeyboardFocusChange,false);
MV->setGeometry(10,10,500,500);
QIcon icon(":/grasshopper.png");
Q_ASSERT(!icon.isNull());
MV->setWindowIcon(icon);



MV->setWindowTitle ( QString::fromUtf8("Кузнечик - нет файла"));
//kuznHeader->setWMTitle( QString::fromUtf8("Кузнечик - нет файла"));
QPalette PallGrey;
//PallGrey.setBrush(QPalette::Window,QBrush(QColor(140,140,100)));
MV->setPalette (PallGrey);
CreateMenu(); // debug09
//CreateNetWindow();
//CreateInfoWindow();

 scene =new KumScene(MV);
 viewFrame=new QFrame(MV);
 viewFrame->setGeometry(0,MenuHigth+1,DEFAULT_SIZEX,DEFAULT_SIZEY-MenuHigth);
view=new DrawView(scene,viewFrame);

view->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
view->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );

connect (scene, SIGNAL(MousePress(qreal,qreal,bool,qreal,qreal)), this, SLOT(MousePress(qreal,qreal,bool,qreal,qreal)));
connect (scene, SIGNAL(MouseRelease(qreal,qreal,bool)), this, SLOT(MouseRelease(qreal,qreal,bool)));
connect (scene, SIGNAL(MouseMove(int,int,bool)), this, SLOT(MouseMove(int,int,bool)));
connect (scene, SIGNAL(MouseWheel(int)), this, SLOT(MouseWheel(int )));
this->setGeometry(MV->geometry());


//MV->setWindowFlags( Qt::FramelessWindowHint);
scene->setBackgroundBrush (QBrush(QColor("lightgreen")));
//scene->setBackgroundBrush (QBrush(QColor("blue")));
StepX = 1.;
StepY = 1.;
SizeX = DEFAULT_SIZEX;
SizeY = DEFAULT_SIZEY;
//view->setGeometry( 4, MenuHigth+5, 480, SizeY + MenuHigth-5 );
this->setFixedHeight(SizeY + MenuHigth+2);
//setFixedSize(SizeX+2,SizeY + MenuHigth+2);
//SizeY = 290;
ButtonSdwig = 100;
SqwareNetFlag = true;
NetShowFlag = true;
TmpNetShowFlag = true;
LeftMousePressFlag=false;
//CurColorName = QString::fromUtf8("черный");
lines.clear();

//Createbtn11();

Createbtn12();
Createbtn13();
CreateXYLabel();

	//connect(btn11,SIGNAL(clicked()),this,SLOT(ZoomUp()));

	//connect(btn13,SIGNAL(clicked()),this,SLOT(ZoomDown()));

	//connect(btn21,SIGNAL(clicked()),this,SLOT(ShowNetWindow()));
	//connect(btn22,SIGNAL(clicked()),this,SLOT(ToCenter()));
	//connect(btn23,SIGNAL(clicked()),this,SLOT(ClearPicture()));
	//connect(btn31,SIGNAL(clicked()),this,SLOT(ToWindow()));
	//connect(btn33,SIGNAL(clicked()),this,SLOT(PrintToPDF()));

	WindowZoom = 1;
	WindowX0 = -SizeX/2;
	WindowY0 = -SizeY/2;
	
	NLastVec = 0;
	CurColor.Red = 0;
	CurColor.Green = 0;
	CurColor.Blue = 0;
	CurY = 0;
	CurX = 0;
	CurZ = 1;
	leftBorder=0;
	rightBorder=0;
	borderEnable=false;

	startPos=0;
	PenPosition = true;
	mPen = NULL;
SdwigNetX = 0.;
SdwigNetY = 0.;	

	CreatePen();
SetColorString(QString::fromUtf8("красный"));	
MinZoom = 0.5;
MaxZoom = 2;

//MV->setMinimumSize(SizeX+2-100,SizeY  + MenuHigth+2);
//MV->setMaximumSize(SizeX+2+300,SizeY + MenuHigth+2);
SetWindowSize(10,10,SizeX,SizeY);
WindowZoom=BASEZOOM;
WindowX0 =  - SizeX/(WindowZoom);
WindowY0 =  - SizeY/(WindowZoom*2)-1;

connect(scene,SIGNAL(sceneRectChanged (QRectF)),view,SLOT(updateSceneRect(QRectF)));
QRectF curentRect;
curentRect.setRect(-3.5,-3,7,5);
//scene->setSceneRect(-2048,-1800,4096,3600);


view->scale(BASEZOOM,BASEZOOM);

view->centerOn(0,YCENTER);
qDebug()<<"Center: "<<view->mapToScene(view->viewport()->rect().center())<<" YCENTER="<<YCENTER;

WindowX0 =visibleRect().x();
WindowY0 =visibleRect().y();



//qDebug()<<"DrawPos"<<app()->settings->value("Draw/Xpos").toInt();
//ToDubl();
RightMousePressFlag=false;
//GoVector(10,0);
Fstep=3;
Bstep=2;
Finish=1;

      //Risuem flag:
	static const int points[] = { 0,2, 2,2,1,1,2,0,0,0,0,4};
        
         QPolygon polygon;
         polygon.setPoints(6, points);
	QPolygonF polygonf = QPolygonF(polygon);

	mFlag = new QGraphicsPolygonItem ( );
	mFlag->setPolygon(polygonf);
	mFlag->setZValue(100);
    mFlag->setBrush(QBrush(QColor(150,100,100)));
    mFlag->setRotation(180);
    mFlag->setScale(0.5);
//	mFlag->rotate(180);
//	mFlag->scale(0.5,0.5);
	mFlag->moveBy(0,0);
	//scene->addItem(mFlag); UBRAN FLAG

	mFlagLine=new QGraphicsLineItem(0,0,0,-1.5);
        mFlagLine->setPen(QPen(QColor(0,0,0)));
	mFlagLine->setZValue(100);
	//scene->addItem(mFlagLine); UBRAN FLAG
        redrawFlags();
	//Reset();
	mPen->setPos(0,-0.25);
        mPen->setBrush(QBrush(QColor("blue")));
scene->addItem(new QGraphicsLineItem(0,-ZOOMMULTIP*2,0,-ZOOMMULTIP*2+0.0005));
//scene->addItem(new QGraphicsLineItem(-500,-7,500,-7));
//scene->addItem(new QGraphicsLineItem(-2,-10,2,-10));
//scene->addItem(new QGraphicsLineItem(-1,10,1,10));
//scene->addItem(new QGraphicsLineItem(-0.5,20,0.5,20));
//scene->addItem(new QGraphicsLineItem(-0.25,40,0.25,40));

fileName="";
advancedTask=false;
    MV->hide();
} 


//----------------------------------------------------
//сдвиг на вектор
int KumKuznec::GoVector(double DeltaXX,double DeltaYY) 
{

	

	qreal x1 = CurX;
	qreal y1 = -CurY;

	qreal x2 = CurX + DeltaXX;
	qreal y2 = -(CurY + DeltaYY);
	PenPosition=false;
	if (PenPosition)
	{
	mPen->setBrush(QBrush(QColor(CurColor.Red,CurColor.Green,CurColor.Blue)));
	
	lines.append(scene->addLine(x1, y1 , x2, y2 ));
	lines.last()->setZValue(CurZ);
	lines.last()->setPen(QPen(QColor(CurColor.Red,CurColor.Green,CurColor.Blue)));
	CurZ += 0.01;	
	CurX += DeltaXX;
	CurY += DeltaYY;	
	}
	else
	{
	//QGraphicsLineItem *tmp = new QGraphicsLineItem(x1, y1 , x2, y2 );
	//lines.append(tmp);
	//mPen->setBrush(QBrush(QColor("white")));
		CurX += DeltaXX;
		CurY += DeltaYY;
	}
	mPen->setPos(CurX,-CurY);
	redrawFlags();
	//qDebug()<<CurX;	
return 0;
	
}

//опустить перо
//------------------------------------
void KumKuznec::PenDown()
{
	PenPosition = true;
}
//------------------------------------------------------
//поднять перо
void KumKuznec::PenUp()
{
	PenPosition = false;
}
//------------------------------------------
//переход в точку
int KumKuznec::GoPoint(double xx,double yy) 
{
	double DeltaX = xx - CurX;
	double DeltaY = yy - CurY;
	
	GoVector(DeltaX,DeltaY);
	
	return 0;
}

//----------------------------------------------------------------------------
//--------------------------------------------------------------
// сдвиг окна в мировых координатах
//void KumKuznec::SetWindowPosition(double x0,double y0, double zoom) 
//{
//}
//-----------------------------------------------------------------------

//-------------------------------------------------------------------
//сдвиг окна вправо
void KumKuznec::MoveRight(void)
{

	WindowX0 = WindowX0 - SizeX/WindowZoom/4;
	
	WindowRedraw();

}
//------------------------------------------------------
//сдвиг окна влево
void KumKuznec::MoveLeft(void)
{

	WindowX0 = WindowX0 + SizeX/WindowZoom/4;
	
	WindowRedraw();

}
//-----------------------------------------------------
//сдвиг окна вверх
void KumKuznec::MoveUp(void)
{
	WindowY0 = WindowY0 - SizeY/WindowZoom/4;
	
	WindowRedraw();
}
//-----------------------------------------------------
//сдвиг окна вниз
void KumKuznec::MoveDown(void)
{
	WindowY0 = WindowY0 + SizeY/WindowZoom/4;
	
	WindowRedraw();
}
//-----------------------------------------------------
void KumKuznec::ZoomUp(void)
{
if (WindowZoom > MaxZoom)return;



	qreal CenterX=center().x();
	qreal CenterY=center().y();

	WindowZoom = WindowZoom*2;

	
	//(WindowZoom>64)WindowY0 -= 1;
	view->scale(2.,2.);
	//view->centerOn(CenterX,0);
	WindowX0=visibleRect().x();
	WindowY0=visibleRect().y();
	qDebug()<<"Zoom"<<WindowZoom<<" viewRECT"<<view->sceneRect()<<"sceneRect"<<scene->sceneRect ();
	//WindowRedraw();
	
	//if(WindowZoom>64){MoveDown();MoveDown();qDebug()<<"MOVE DOWN";};
	

}
//--------------------------------------------------
void KumKuznec::ZoomDown(void)
{
if (WindowZoom < MinZoom)return;


	qreal CenterX=center().x();
	qreal CenterY=center().y();
	//if(WindowZoom>64)WindowY0 += 1;
	WindowZoom = WindowZoom/2;

	
	QPointF center_p = view->mapToScene(view->viewport()->rect().center());
	view->scale(0.5,0.5);
	//view->centerOn(CenterX,0);
	
	WindowX0=visibleRect().x();
	WindowY0=visibleRect().y();
	//view->centerOn(CenterX,0);
	qDebug()<<"Zoom OUT"<<WindowZoom<<"SCENE RECT"<<view->sceneRect()<<"ViewRect"<<visibleRect()<<"Center"<<center_p;
}
//-------------------------------------------------------------------
void KumKuznec::lockControls()
{
/*
	if  ( btnBox->isEnabled() )
		btnBox->setEnabled(false);

*/
}
//----------------------------------------
void KumKuznec::unlockControls()
{
/*
	if ( !btnBox->isEnabled() )
		btnBox->setEnabled(true);
*/
}
//----------------------------------------------------------
//сохранение в ps-файл
int KumKuznec::SaveToFile(QString p_FileName)
{
QFile l_File(p_FileName);
QChar Bukva;
	char ctmp[200];	
	if  (!l_File.open(QIODevice::WriteOnly))
	{
	return 1;
	}
	
	//QString ttt = QString::fromUtf8("Чертежник - Начало");
	
//l_File.write( ttt.toUtf8());			
	l_File.write( "%!PS-Adobe-1.0 EPSF-1.0\n");
	QString l_String;
	l_File.write( "%%Creator: Cherteznik\n");
	l_File.write("%%Pages: 1\n");
	l_File.write("%%Orientation: Portrait\n");

	
	// maximum, minimum

	qreal MinX,MaxX,MinY,MaxY,VecX1,VecX2,VecY1,VecY2;
	
QLineF TmpLine;
	MinX = 1000000;
	MinY = 1000000;

	MaxX = -1000000;
	MaxY = -1000000;


	for (int i = 0; i <lines.count(); i++)
	{
	
		TmpLine = lines[i]->line();
		VecX1 = TmpLine.x1();
		VecY1 = -TmpLine.y1();
		VecX2 = TmpLine.x2();
		VecY2 = -TmpLine.y2();
		if (VecX1 < MinX)MinX = VecX1;
		if (VecY1 < MinY)MinY = VecY1;
		if (VecX1 > MaxX)MaxX = VecX1;
		if (VecY1 > MaxY)MaxY = VecY1;
	
		if (VecX2 < MinX)MinX = VecX2;
		if (VecY2 < MinY)MinY = VecY2;
		if (VecX2 > MaxX)MaxX = VecX2;
		if (VecY2 > MaxY)MaxY = VecY2;

	}
	double Scale;

	if (MaxX - MinX > MaxY - MinY)
	{
		Scale = (596-10)/(MaxX-MinX);
	}
	else
	{
		Scale = (842-10)/(MaxY-MinY);
	}
	Scale = Scale*0.9;
	
//	QString tmp1 = QString(ctmp)+" scale\n";
	
	
	
	l_File.write("%%BoundingBox: 0 0 596 842\n");
	l_File.write("%%HiResBoundingBox: 0 0 596 842\n");
	l_File.write( "%%EndComments\n");
	l_File.write( "%%Page: 1 1\n");
	sprintf(ctmp,"%f %f translate\n",-MinX,-MinY);
	l_File.write(ctmp);
	sprintf(ctmp,"%f %f scale\n",Scale,Scale);	
	l_File.write(ctmp);
	l_File.write("gsave [1 0 0 1 0 0] concat\n");
	l_File.write("0 0 0 setrgbcolor\n");
	l_File.write("[] 0 setdash\n");
	l_File.write("1 setlinewidth\n");
	l_File.write("0 setlinejoin\n");
	l_File.write("0 setlinecap\n");
	l_File.write("newpath\n");
//QColor TmpColor;	
QPen TmpPen;
QColor TmpColor;
	for (int i = 0; i <lines.count(); i++)
	{
	
		TmpLine = lines[i]->line();
		TmpPen = lines[i]->pen();
		TmpColor = TmpPen.color();
		sprintf(ctmp,"%i %i %i setrgbcolor\n", TmpColor.red(),  TmpColor.green(), TmpColor.blue());
		l_File.write(ctmp);
		
		VecX1 = TmpLine.x1();
		VecY1 = -TmpLine.y1();
		VecX2 = TmpLine.x2();
		VecY2 = -TmpLine.y2();
          
		sprintf(ctmp,"%f %f moveto\n", VecX1, VecY1);
		l_File.write(ctmp);
		
		sprintf(ctmp,"%f %f lineto\n", VecX2, VecY2);
		
		l_File.write(ctmp);


	}

//77777777777777777777777777777777777
QString TmpText;
QByteArray ccc;
qreal tmpX,tmpY,FontSize;
	for (int i = 0; i<kumtexts.count(); i++)
	{
	FontSize = kumtexts[i]->Size;
	sprintf(ctmp,"/Curier findfont %f scalefont setfont\n",FontSize);
	l_File.write(ctmp);

	tmpX = kumtexts[i]->x;
	tmpY = kumtexts[i]->y;
	sprintf(ctmp,"%f %f moveto\n", tmpX, tmpY);
	l_File.write(ctmp);
		

	//TmpPen = texts[i]->pen();
	//TColor = TmpPen.color();
	sprintf(ctmp,"%i %i %i setrgbcolor\n", kumtexts[i]->color.Red,  kumtexts[i]->color.Green, kumtexts[i]->color.Blue);
	l_File.write(ctmp);

	TmpText = "("+texts[i]->text()+") show\n";
	ccc = TmpText.toUtf8();
	l_File.write(ccc);
	}

//777777777777777777777777777

	l_File.write("stroke\n");
	l_File.write("grestore\n");
	l_File.write( "showpage\n");
	l_File.close();
	return 0;


	l_File.close();
	return 0;

}
//--------------------------------------------
//запись строки в файл
//void KumKuznec::WriteLine(QFile *p_File, QString p_String)
//{
/*
	p_File->writeBlock(p_String, p_String.length());
*/
//}
//---------------------------------------------------------------------------
//Загрузка из файла
int KumKuznec::LoadFromFile(QString p_FileName)
{
if((fileName=="")&&(advancedTask))
	{
		if(QMessageBox::question( this, "", QString::fromUtf8("Сохранить изменения?"), QMessageBox::Yes,QMessageBox::No,0) == QMessageBox::Yes)
		{
               SaveToFileActivated();
		};
	};
QFileInfo fi(p_FileName);
 QString name = fi.fileName();                // name = "archive.tar.gz"

QString Title = QString::fromUtf8("Кузнечик - ") + name;

MV->setWindowTitle ( Title);

qreal CurrentScale; 

	QString tmp = "";
	char ctmp[200];
	QString l_String;
	QFile l_File(p_FileName);
	
   
    
	int NStrok; 
	NStrok = 0;

//	long l_Err;
	//int CurX,CurY;
//	int SizeX, SizeY;	
qreal x1,y1,x2,y2;
	if  (!l_File.open(QIODevice::ReadOnly))	
	{
	QMessageBox::information( MV, "", QString::fromUtf8("Ошибка открытия файла"), 0,0,0);
	return 1;
	}
	
	//l_String = l_File.readLine();
//QMessageBox::information( 0, "", tmp, 0,0,0);
QByteArray ttt;
	ttt = l_File.readLine();
	
	//Читаем размеры прыжков
        while(ttt[0]==';')ttt = l_File.readLine();
	qDebug()<<ttt;
	l_String = QString::fromUtf8(ttt);
    l_String = l_String.simplified();
	QStringList zadanie=l_String.split(" ");
	if(zadanie.count()<2) return 1;
        Fstep=zadanie[0].toInt();
        Bstep=zadanie[1].toInt();
        Kpult->setStepSizes(Fstep,Bstep);
       
        ttt = l_File.readLine();
	   //Читаем стартовую позицию
        while(ttt[0]==';')ttt = l_File.readLine();
	qDebug()<<ttt;
	l_String = QString::fromUtf8(ttt);
	bool ok;
	startPos=l_String.toInt(&ok);
	if(!ok)return 2;
	
        ttt = l_File.readLine();
	//Читаем границы
 	while(ttt[0]==';')ttt = l_File.readLine();
	qDebug()<<ttt;
	l_String = QString::fromUtf8(ttt);
        l_String = l_String.simplified();
	zadanie=l_String.split(" ");
	if(zadanie.count()<2) return 3;
        leftBorder=zadanie[0].toInt(&ok);
	if(!ok)return 4;
        rightBorder=zadanie[1].toInt(&ok);
	if(!ok)return 4;
	borderEnable=true;
	if(leftBorder<-1000)borderEnable=false;
	if(rightBorder>1000)borderEnable=false;
	ttt = l_File.readLine();
	//Читаем флаги
	flags_pos.clear();
	while(ttt[0]==';')ttt = l_File.readLine();
	
	qDebug()<<ttt;
	l_String = QString::fromUtf8(ttt);
    l_String = l_String.simplified();
	zadanie=l_String.split(" ",QString::SkipEmptyParts);
	for(int i=0;i<zadanie.count()-1;i++)
		{
		qDebug()<<"flag:"<<zadanie[i];
		flags_pos.append(zadanie[i].toInt(&ok));
		if(!ok)return 5;
		};


	Reset();
fileName=QFileInfo(l_File).fileName ();			
MV->setWindowTitle ( QString::fromUtf8("Кузнечик - ")+QFileInfo(l_File).fileName ());
//kuznHeader->setWMTitle( QString::fromUtf8("Кузнечик - ")+QFileInfo(l_File).fileName ());
createFlags();
	return 0;

}
//------------------------------------------------

float KumKuznec::GetMinX(void)
{

qreal MinX = 1000000;
qreal VecX1,VecX2;
QLineF TmpLine;


	for (int i = 0; i <lines.count(); i++)
	{
	
		TmpLine = lines[i]->line();
		VecX1 = TmpLine.x1();
		VecX2 = TmpLine.x2();
		
	if (VecX1 < MinX)MinX = VecX1;
	if (VecX2 < MinX)MinX = VecX2;
	
        }
return MinX;	

}
//--------------------------------------
float KumKuznec::GetMinY(void)
{

qreal MinY = 1000000;
qreal VecY1,VecY2;
QLineF TmpLine;


	for (int i = 0; i <lines.count(); i++)
	{
	
		TmpLine = lines[i]->line();
		VecY1 = -TmpLine.y1();
		VecY2 = -TmpLine.y2();
		
	
	if (VecY1 < MinY)MinY = VecY1;
	if (VecY2 < MinY)MinY = VecY2;
	
        }

return MinY;	

}
//------------------------------------------
float KumKuznec::GetMaxX(void)
{

qreal MaxX = -1000000;
qreal VecX1,VecX2;
QLineF TmpLine;


	for (int i = 0; i <lines.count(); i++)
	{
	
		TmpLine = lines[i]->line();
		VecX1 = TmpLine.x1();
		VecX2 = TmpLine.x2();
		
	
	if (VecX1 > MaxX)MaxX = VecX1;
	if (VecX2 > MaxX)MaxX = VecX2;
	
        }
return MaxX;	


}
//---------------------------------------
float KumKuznec::GetMaxY(void)
{

qreal MaxY = -1000000;
qreal VecY1,VecY2;
QLineF TmpLine;


	for (int i = 0; i <lines.count(); i++)
	{
	
		TmpLine = lines[i]->line();
		VecY1 = -TmpLine.y1();
		VecY2 = -TmpLine.y2();
	
	if (VecY1 > MaxY)MaxY = VecY1;
	if (VecY2 > MaxY)MaxY = VecY2;
	
        }

return MaxY;	




}
//-------------------------------------------
//загоняет картинку в окно
void KumKuznec::ToWindow(void)
{

qreal MinX = GetMinX();
qreal MaxX = GetMaxX();
qreal MinY = GetMinY();
qreal MaxY = GetMaxY();
qreal TextX,TextY;
for (int i = 0;i<kumtexts.count();i++)
{
TextX = kumtexts[i]->x;
TextY = kumtexts[i]->y;
int len = kumtexts[i]->text.length();
qreal TextScale = kumtexts[i]->Size;
  if(TextX < MinX)MinX = TextX;
  if(TextY < MinY)MinY = TextY;
  if (TextX+len*TextScale > MaxX)  MaxX = TextX+len*TextScale;
  if (TextY+TextScale > MaxY)  MaxY = TextY+TextScale;
}

MinX = MinX - (MaxX-MinX)*0.1;
MaxX = MaxX + (MaxX-MinX)*0.1;

MinY = MinY - (MaxY-MinY)*0.1;
MaxY = MaxY + (MaxY-MinY)*0.1;

qreal tmp;
qreal tmpZoom;
qreal Delta ;
qreal XSdwig = 0;
qreal YSdwig = 0;

if ((MaxX-MinX < 0.00001) && (MaxY - MinY < 0.00001))return;

//if (MaxX - MinX > MaxY - MinY)
if ((MaxX - MinX)/SizeX > (MaxY - MinY)/SizeY)
{
tmp = MaxX - MinX;
tmpZoom = (SizeX)/tmp; 
//YSdwig = (MaxX-MinX)/2 - (MaxY-MinY)/2;
YSdwig = SizeY/2/tmpZoom -(MaxY-MinY)/2;
//Delta = (MaxX-MinX)/50;
Delta = 0;
}
else
{
tmp = MaxY - MinY;
tmpZoom = (SizeY)/tmp; 
//XSdwig = (MaxY-MinY)/2 - (MaxX-MinX)/2;
XSdwig = SizeX/2/tmpZoom -(MaxX-MinX)/2;
//Delta = (MaxY-MinY)/50;
Delta = 0;
}

qreal Scale = tmpZoom/WindowZoom;

WindowZoom = tmpZoom;
WindowX0 = MinX  - XSdwig;

//WindowY0 = MinY - YSdwig;
WindowY0 = -MaxY - YSdwig;
//qWarning(",WindowY0 %f %f",WindowX0,WindowY0);
//qWarning("WindowX0,WindowY0 %f %f",WindowX0,WindowY0);
//mPen->scale(1/Scale,1/Scale);
view->scale(Scale,Scale);


WindowRedraw(); 

}
//------------------------------------------------
void KumKuznec::ClearPicture(void)
{
HideCoord();

Dialog* newZdialog=new Dialog(MV);

 newZdialog->setSizes(Fstep,Bstep);
 
 newZdialog->setFlags(flags_pos);
 newZdialog->setStart(startPos);
 newZdialog->setLeftBorder(leftBorder);
 newZdialog->setRightBorder(rightBorder);
 newZdialog->setBordersChB(borderEnable);
qDebug()<<"Task:"<<advancedTask;
 newZdialog->setTaskNeeded(advancedTask);
 	if(!newZdialog->exec())return;
qDebug()<<"Asize"<<newZdialog->ASize();
Kpult->setStepSizes(newZdialog->ASize(),newZdialog->BSize());
Fstep=newZdialog->ASize();
Bstep=newZdialog->BSize();
startPos=newZdialog->getStart();
flags_pos=newZdialog->getFlags();
//Finish=newZdialog->ANeed();

MV->setWindowTitle ( QString::fromUtf8("Кузнечик - без имени"));
//kuznHeader->setWMTitle( QString::fromUtf8("Кузнечик - без имени"));
for (int i = 0; i <lines.count(); i++)delete lines[i];
lines.clear();

for (int i = 0; i <texts.count(); i++)delete texts[i];
texts.clear();

for (int i = 0; i <kumtexts.count(); i++)delete kumtexts[i];
kumtexts.clear();
borderEnable=newZdialog->borders();
if(borderEnable)
	{
	leftBorder=newZdialog->getLeftBorder();
	rightBorder=newZdialog->getRightBorder();
	};
WindowRedraw();
advancedTask=newZdialog->isTaskNeeded();
CurX = 0;
CurY = 0;
mPen->setPos(startPos*ZOOMMULTIP,0);
CurX=ZOOMMULTIP*startPos;
mPen->setBrush(QBrush(QColor("blue")));
createFlags();
ClearSledi();
ClearZakr();
ToDubl();
//MV->setWindowTitle ( QString::fromUtf8("Кузнечик - нет файла"));
//kuznHeader->setWMTitle( QString::fromUtf8("Кузнечик - нет файла"));
fileName="";
}


 
//---------------------------------------------------
void KumKuznec::CreatePen(void)
{


 	//static const int points[] = {  0,0, 6,-3, 18,-15, 15,-18, 3,-6 };

	static const int points[] = { 0,0, -3,-6, 3,-6, 0,0 };
        
         QPolygon polygon;
         polygon.setPoints(4, points);
	QPolygonF polygonf = QPolygonF(polygon);

	mPen = new QGraphicsPolygonItem ( );
	mPen->setPolygon(polygonf);
	mPen->setZValue(100);
	//mPen->setBrush(QBrush(QColor("ligthgray")));
//mPen->setColor(QColor("ligthgray"));

//mPen->setBrush(QBrush(QColor(CurColor.Red,CurColor.Green,CurColor.Blue)));
mPen->setBrush(QBrush(QColor(200,200,200)));
		
	
	scene->addItem(mPen);
	//mPen->scale(0.5,0.5);
	//mPen->scale(0.5,0.5);
		

//debug1	
mPen->moveBy(0,-0.25);

}

void KumKuznec::SetLineColor(int LRed, int LGreen, int LBlue)
{

CurColor.Red = LRed;
CurColor.Green = LGreen;
CurColor.Blue = LBlue;
mPen->setBrush(QBrush(QColor(CurColor.Red,CurColor.Green,CurColor.Blue)));
}

//--------------------------------------
bool KumKuznec::SetColorString(QString Color)
{
CurColorName = Color;
if (Color == QString::fromUtf8("черный")){SetLineColor(1,1,1);return true;}
if (Color == QString::fromUtf8("белый")){SetLineColor(254,254,254);return true;}
if (Color == QString::fromUtf8("красный")){SetLineColor(254,0,0);return true;}
if (Color == QString::fromUtf8("желтый")){SetLineColor(254,254,0);return true;}
if (Color == QString::fromUtf8("оранжевый")){SetLineColor(254,128,0);return true;}
if (Color == QString::fromUtf8("зеленый")){SetLineColor(0,254,0);return true;}
if (Color == QString::fromUtf8("голубой")){SetLineColor(137,175,245);return true;}
if (Color == QString::fromUtf8("синий")){SetLineColor(0,0,254);return true;}
if (Color == QString::fromUtf8("фиолетовый")){SetLineColor(128,0,255);return true;}
return false;
}

//------------------------------------------------------------
void KumKuznec::Createbtn11()
{
	btn11 = new QToolButton(MV);
	btn11->setIcon(QIcon(QString::fromUtf8(":/icons/media-skip-forward.png")));
	btn11->setIconSize(QSize(20, 20));
	btn11->setToolTip(trUtf8("Крупнее"));
	//btn11->move(SizeX/2 - BUTTON_SIZE/2 -BUTTON_SIZE+3, SizeY+20-BUTTON_SIZE+2+ MenuHigth);
//btn11->move(SizeX/2 - BUTTON_SIZE/2 -BUTTON_SIZE+3, SizeY+20+2+ MenuHigth);
	//btn11->move(SizeX/2 - BUTTON_SIZE/2 -BUTTON_SIZE+3, SizeY+15+ MenuHigth);
	btn11->move(ButtonSdwig, 5);
	btn11->resize(BUTTON_SIZE,BUTTON_SIZE);
	btn11->show();
}

//------------------------------------------
void KumKuznec::Createbtn12()
{

	btn12 = new QToolButton(MV);
	//btn12->setIcon(QIcon(QString::fromUtf8(":/icons/tab-new.png")));
	btn12->setIcon(QIcon(QString::fromUtf8(":/icons/document-open.png")));
	btn12->setIconSize(QSize(20, 20));
	btn12->setToolTip(trUtf8("Загрузить"));
	//btn12->move(SizeX/2 - BUTTON_SIZE/2+3, SizeY+15+ MenuHigth);
	btn12->move(100+BUTTON_SIZE, 5);
	btn12->resize(BUTTON_SIZE,BUTTON_SIZE);
//	btn12->show();
btn12->hide();
}
//--------------------------------------------------------
void KumKuznec::Createbtn13()
{
	btn13 = new QToolButton(MV);
	btn13->setIcon(QIcon(QString::fromUtf8(":/icons/media-skip-backward.png")));
	//btn22->setIcon(QIcon(QString::fromUtf8("1uparrow.png")));
	btn13->setIconSize(QSize(20, 20));
	btn13->setToolTip(trUtf8("Мельче"));
	//btn13->move(SizeX/2 - BUTTON_SIZE/2+BUTTON_SIZE+3, SizeY+15+ MenuHigth);
	btn13->move(ButtonSdwig+BUTTON_SIZE, 5);
	btn13->resize(BUTTON_SIZE,BUTTON_SIZE);
       btn13->hide();
}


void KumKuznec::ToCenter(void)
{


qDebug()<<"WZ"<<WindowZoom<<" BZ"<<BASEZOOM;
view->scale(BASEZOOM/WindowZoom,BASEZOOM/WindowZoom);
view->centerOn(0,0);
WindowZoom=BASEZOOM;
//mPen->scale(1/Scale,1/Scale);
//view->scale(64,Scale);

}
//-----------------------------------------------------------------------

// пересветка окна
void KumKuznec::WindowRedraw(void)
{


QRectF curentRect;
//=view->sceneRect();

curentRect.setRect(WindowX0,WindowY0,SizeX/WindowZoom,SizeY/WindowZoom);
qDebug()<<"WindowY0"<<WindowY0;
DrawNet();
//view->show();

//scene->setSceneRect(curentRect);
	
}
//--------------------------------------------------
void KumKuznec::DrawNet()
{
//� исуем сетку и оси

	int i;
	for ( i = 0; i < Netlines.count(); i++)delete Netlines[i];
	for ( i = 0; i < NetText.count(); i++)delete NetText[i];
	Netlines.clear();
	NetText.clear();
	
// вертикальная сетка

	double BegX=-1024*ZOOMMULTIP;
	double EndX=1024*ZOOMMULTIP;
	int Zapas = 100;
	double fx1,fy1,fx2,fy2;
//,DeltaXX,DeltaYY;

	
	//BegX = WindowX0 -  SdvigX - SizeX/WindowZoom;
	//EndX = WindowX0 + SizeX/WindowZoom +SizeX/WindowZoom;
	


	

	//float BegY = WindowY0 -SdvigY - SizeY/WindowZoom;
	//float EndY = WindowY0 + SizeY/WindowZoom;
       float Step = ZOOMMULTIP;

			

	fx1=BegX;
	
	while (fx1 < EndX)
	{
	fx1 = fx1 + Step;
	fx2=fx1+ZOOMMULTIP;
	
//	fy1 = WindowY0 - 100/WindowZoom-100*Step;
//	fy2 = WindowY0 + SizeY/WindowZoom +100/WindowZoom+100*Step;



	Netlines.append(scene->addLine((fx1-0.2*ZOOMMULTIP), 0.2*ZOOMMULTIP , fx1-0.2*ZOOMMULTIP, -0.2*ZOOMMULTIP ));
	//Netlines.last()->scale(ZOOMMULTIP,ZOOMMULTIP);
	Netlines.last()->setZValue(0.5);
	Netlines.last()->setPen(QPen(QColor("blue"))); // Левая вертикаль

	Netlines.append(scene->addLine((fx1+0.2*ZOOMMULTIP), 0.2*ZOOMMULTIP , fx1+0.2*ZOOMMULTIP, -0.2*ZOOMMULTIP ));
	//Netlines.last()->scale(ZOOMMULTIP,ZOOMMULTIP);
	Netlines.last()->setZValue(0.5);
	Netlines.last()->setPen(QPen(QColor("blue")));//Правая вертикаль

	Netlines.append(scene->addLine((fx1-0.2*ZOOMMULTIP), -0.2*ZOOMMULTIP , fx1+0.2*ZOOMMULTIP, -0.2*ZOOMMULTIP ));
	//Netlines.last()->scale(ZOOMMULTIP,ZOOMMULTIP);
	Netlines.last()->setZValue(0.5);
	Netlines.last()->setPen(QPen(QColor("blue")));//верх


	Netlines.append(scene->addLine((fx1-0.2*ZOOMMULTIP), 0.2*ZOOMMULTIP , fx1+0.2*ZOOMMULTIP, 0.2*ZOOMMULTIP ));
	//Netlines.last()->scale(ZOOMMULTIP,ZOOMMULTIP);
	Netlines.last()->setZValue(0.5);
	Netlines.last()->setPen(QPen(QColor("blue")));//Низ

	Netlines.append(scene->addLine((fx1+0.2*ZOOMMULTIP), 0, fx2-0.2*ZOOMMULTIP, 0 ));
	//Netlines.last()->scale(ZOOMMULTIP,ZOOMMULTIP);
	Netlines.last()->setZValue(0.5);
	Netlines.last()->setPen(QPen(QColor("blue")));//соединялка

//         Netlines.append(scene->addLine(fx1-0.2, 0.2 , fx2-0.2, -0.2 ));
// 	Netlines.last()->scale(ZOOMMULTIP,ZOOMMULTIP);
// 	Netlines.last()->setZValue(0.5);
// 	Netlines.last()->setPen(QPen(QColor("blue")));
// 
// 	Netlines.append(scene->addLine(fx1-0.2, 0.2 , fx2+0.2, 0.2 ));
// 	Netlines.last()->scale(ZOOMMULTIP,ZOOMMULTIP);
// 	Netlines.last()->setZValue(0.5);
// 	Netlines.last()->setPen(QPen(QColor("blue")));
//         Netlines.append(scene->addLine(fx1-0.2, -0.2 , fx2+0.2, -0.2 ));
// 	Netlines.last()->scale(ZOOMMULTIP,ZOOMMULTIP);
// 	Netlines.last()->setZValue(0.5);
// 	Netlines.last()->setPen(QPen(QColor("blue")));
//        
//         Netlines.append(scene->addLine(fx1+0.2, 0.0 , fx1+Step-0.2, 0.0 ));
// 	Netlines.last()->scale(ZOOMMULTIP,ZOOMMULTIP);
// 	Netlines.last()->setZValue(0.5);
// 	Netlines.last()->setPen(QPen(QColor("blue")));
	if((borderEnable)&&((fx1/ZOOMMULTIP==rightBorder)||(fx1/ZOOMMULTIP==leftBorder)))
		{
		qreal zdvig=Step/2;
		if(fx1/ZOOMMULTIP==leftBorder)zdvig=-zdvig;
		Netlines.append(scene->addLine(fx1+zdvig, -16 , fx1+zdvig, 16 ));
		Netlines.last()->setZValue(0.5);
		Netlines.last()->setPen(QPen(QColor("red")));
		
		}

	}

//Оси координат

	
        qreal textX,textY;
	textX=BegX;
	while(textX<EndX)
	{
	NetText.append(new QGraphicsSimpleTextItem(QString::number(textX/ZOOMMULTIP)));
	
	qreal zdvig=-0.2*ZOOMMULTIP;
	 if((textX/ZOOMMULTIP>0.5)||(textX/ZOOMMULTIP<9.5))zdvig+=0.1*ZOOMMULTIP;
	//if(textX<-9.5)zdvig+=0.1;
	NetText.last()->setPos(textX+zdvig,0.2*ZOOMMULTIP);
	int Zoom=Step;
	
      //qDebug()<<((int)textX)%100;
	if((((int)textX/ZOOMMULTIP)%100) == 0)
		{
		//qDebug()<<"Text ZoomUP";
		Zoom=Zoom*1.5;
		};
//	NetText.last()->scale(0.03*Zoom,0.03*Zoom);
    NetText.last()->setScale(0.03 * Zoom);
        NetText.last()->setZValue(1000);
	
	scene->addItem(NetText.last());


	textX=textX+Step;	
	//Netlines.append(scene->addLine(textX, -0.25 , textX, 0.25 ));
	//Netlines.last()->setZValue(1000);
	//Netlines.last()->setPen(QPen(QColor("black")));
	};
	
	//Netlines.append(scene->addLine(fx1, fy1 , fx2, fy2 ));
	//Netlines.last()->setZValue(0.6);
	//Netlines.last()->setPen(QPen(QColor("blue")));
	









}	


void KumKuznec::ToDubl()
{
QLineF TmpLine;
QPen TmpPen;
QGraphicsLineItem *TmpItem;
for (int i = 0; i <linesDubl.count(); i++)delete linesDubl[i];

linesDubl.clear();


	for (int i = 0; i <lines.count(); i++)
	{
	
	TmpLine = lines[i]->line();
	TmpPen = lines[i]->pen();
	//TmpItem = lines[i];
	TmpItem = new QGraphicsLineItem(TmpLine);
	TmpItem->setPen(TmpPen);
	linesDubl.append(TmpItem);
	}

PenXDubl = CurX;
PenYDubl = CurY;
PenPositionDubl = PenPosition;
}
//--------------------------------------------
void KumKuznec::FromDubl()
{
if (linesDubl.count() == 0){
 			//ClearPicture();
			return;
			};

QLineF TmpLine;

for (int i = 0; i <lines.count(); i++)delete lines[i];

lines.clear();

QGraphicsLineItem *TmpItem;
QPen TmpPen;
QColor TmpColor;

	for (int i = 0; i <linesDubl.count(); i++)
	{
	
	TmpItem = linesDubl[i];
	TmpLine = TmpItem->line();
	TmpPen = TmpItem->pen();
	TmpColor = TmpPen.color();
	lines.append(scene->addLine(TmpLine ));
	lines.last()->setPen(QPen(TmpColor));
	}

CurX = PenXDubl;
CurY = PenYDubl;
PenPosition = PenPositionDubl;
mPen->setPos(CurX,-CurY);	
}
//-------------------------------------------------------------
void KumKuznec::Info()
{
char ctmp[200];
QString tmp;

sprintf(ctmp,"%7.2f",WindowX0);
tmp = QString(ctmp);
lX0->setText(tmp);

sprintf(ctmp,"%7.2f",-WindowY0-SizeY/WindowZoom);
tmp = QString(ctmp);
lY0->setText(tmp);

sprintf(ctmp,"%7.2f",WindowX0+SizeX/WindowZoom);
tmp = QString(ctmp);
lX1->setText(tmp);

sprintf(ctmp,"%7.2f",-WindowY0);
tmp = QString(ctmp);
lY1->setText(tmp);


sprintf(ctmp,"%7.2f",WindowX0+SizeX/WindowZoom/2);
tmp = QString(ctmp);
lXCen->setText(tmp);

sprintf(ctmp,"%7.2f",-WindowY0 -SizeY/WindowZoom/2);
tmp = QString(ctmp);
lYCen->setText(tmp);

sprintf(ctmp,"%7.2f",SdwigNetX);
tmp = QString(ctmp);
lXNetX0->setText(tmp);


sprintf(ctmp,"%7.2f",SdwigNetY);
tmp = QString(ctmp);
lYNetY0->setText(tmp);

sprintf(ctmp,"%7.2f",StepX);
tmp = QString(ctmp);
lXNetDX->setText("("+tmp);

sprintf(ctmp,"%7.2f",StepY);
tmp = QString(ctmp);
lYNetDY->setText(tmp+")");

QPalette Pall;
Pall.setColor(QPalette::BrightText,QColor(CurColor.Red,CurColor.Green,CurColor.Blue));
Pall.setBrush(QPalette::WindowText,QBrush(QColor(CurColor.Red,CurColor.Green,CurColor.Blue)));
lColor->setPalette(Pall);

tmp = QString::fromUtf8("Цвет ")+CurColorName;
lColor->setText(tmp);

if (PenPosition)
tmp = QString::fromUtf8("Перо опущено");
else
tmp = QString::fromUtf8("Перо поднято");

lPen->setText(tmp);

//**InfoWindow->show();
}
//-------------------------------------------------------
bool KumKuznec::PrintToPDF(void)
{
// KumFileDialog dialog(MV,QString::fromUtf8 ("Печатать в файл"),curDir, "(*.pdf)",false); 
// dialog.setAcceptMode(QFileDialog::AcceptSave);
// if(!dialog.exec())return false;
// 
// QString	PrintFileName = dialog.selectedFiles().first();
// QDir dir=dialog.directory();
// curDir=dir.path();
// 
// if (PrintFileName.contains("*") || PrintFileName.contains("?"))
// {
// QMessageBox::information( 0, "", QString::fromUtf8("Недопустимый символ в имени файла!"), 0,0,0);
// return false;
// }
// 
// if(PrintFileName.right(4) != ".pdf")PrintFileName += ".pdf";
// 
// 
// QPrinter *RPrinter = new QPrinter(QPrinter::PrinterResolution);
// RPrinter->setOutputFileName(PrintFileName);
// RPrinter->setFullPage(false);
// RPrinter->setPageSize(QPrinter::A4);
// 
// 
// QPainter *RPainter = new QPainter();
// RPainter->begin(RPrinter);	
// RPainter->setBrush(Qt::SolidPattern);
// QColor RLineColor = QColor(1,1,1);
// 
// 
// //RPainter->setViewTransformEnabled(true);
// 
// QLineF TmpLine;
// 	
// qreal MinX,MaxX,MinY,MaxY,VecX1,VecX2,VecY1,VecY2;
// MinX = WindowX0;
// MinY = -WindowY0- SizeY/WindowZoom;
// MaxX = WindowX0 + SizeX/WindowZoom;
// MaxY = -WindowY0 ;
// 
// qreal Scale;
// int PageWidth = 540;
// 
// 	if (MaxX - MinX > MaxY - MinY)
// 	{
// 	Scale = MaxX-MinX;
// 	}
// 	else
// 	{
// 	Scale = MaxY-MinY;
// 	}
// 
// if (Scale < 0.000001)return false;
// 
// int PenWidth = 1;
// QPen Pen;
// 
// 
// //	 int Stepen = log10(Scale);
// //	 Step = 0.1*pow(10,Stepen);
// 
// Scale = PageWidth/Scale;
// PenWidth = 2;
// Pen = QPen(RLineColor,PenWidth);
// RPainter->setPen(Pen);
// RPainter->setClipRect(0,0,(MaxX-MinX)*Scale,(MaxY-MinY)*Scale);
// //RPainter->setWindow ( 0, -5, 10,10 );
// //RPainter->setViewport ( MinX, MinY, Scale, Scale*1.41 );
// //RPainter->setWindow ( MinX, MinY, Scale+1, (Scale+1)*1.41 );
// qreal x1,y1,x2,y2;
// 
// 
// x1 = (MinX - MinX)*Scale+1;
// y1 = (MinY - MinY)*Scale+1;
// x2 = (MaxX - MinX)*Scale-2;
// y2 = (MaxY - MinY)*Scale-1;
// //рисуем границы
// 
// RPainter->drawLine(x1, y1, x1, y2 );
// RPainter->drawLine(x1, y2, x2, y2 );
// RPainter->drawLine(x2, y2, x2, y1 );
// RPainter->drawLine(x2, y1, x1, y1 );
// 
// 
// 
// //qreal Stepen = log(Scale)/2;
// //qreal Step = 10./pow(10,Stepen);
// //qWarning ("Stepen Step %f %f",Stepen,Step);
// 
// //� исуем сетку
// Pen = QPen(QColor("gray"),1);
// RPainter->setPen(Pen);
// Pen.setWidthF(0.5);
// qreal XCur = 0;
// 
// if (NetShowFlag && TmpNetShowFlag)
// {
// 	while (XCur < MaxX - StepX)
// 	{
// 	XCur += StepX;
// 	if (XCur < MinX)continue;
// 
// 	x1 = (XCur - MinX)*Scale+1;
// 	y1 = (MinY - MinY)*Scale+1;
// 	x2 = (XCur - MinX)*Scale-1;
// 	y2 = (MaxY - MinY)*Scale-1;
// 
// 	RPainter->drawLine(x1 , y1, x2, y2);
// 	}
// 
// XCur = 0;
// 	while (XCur > MinX +StepX)
// 	{
// 	XCur -= StepX;
// 	if (XCur > MaxX)continue;
// 
// 	x1 = (XCur - MinX)*Scale+1;
// 	y1 = (MinY - MinY)*Scale+1;
// 	x2 = (XCur - MinX)*Scale-1;
// 	y2 = (MaxY - MinY)*Scale-1;
// 	RPainter->drawLine(x1 , y1, x2, y2);
// 	}
// 
// 
// qreal YCur = 0;
// 	while (YCur < MaxY - StepY)
// 	{
// 	YCur += StepY;
// 	if (YCur < MinY)continue;
// 	x1 = (MinX - MinX)*Scale+1;
// 	y1 = (MaxY-YCur )*Scale+1;
// 	x2 = (MaxX - MinX)*Scale-1;
// 	y2 = (MaxY-YCur )*Scale-1;
// 
// 	RPainter->drawLine(x1 , y1, x2, y2);
// 	}
// 
// YCur = 0;
// 	while (YCur > MinY + StepY)
// 	{
// 	YCur -= StepY;
// 	if (YCur > MaxY)continue;
// 	x1 = (MinX - MinX)*Scale+1;
// 	y1 = (MaxY-YCur )*Scale;
// 	x2 = (MaxX - MinX)*Scale-1;
// 	y2 = (MaxY-YCur )*Scale;
// 	RPainter->drawLine(x1 , y1, x2, y2);
// 	}
// }
// 
// //рисуем оси
// Pen.setWidthF(2);
// RPainter->setPen(Pen);
// //вертикальная ось
// if (MinX < 0 && MaxX > 0)
// {
// x1 = (0 - MinX)*Scale;
// y1 = (MaxY-MinY )*Scale;
// x2 = (0 - MinX)*Scale;
// y2 = (MaxY-MaxY )*Scale;
// RPainter->drawLine(x1 , y1, x2, y2 );
// }
// //горизонтальная ось
// if (MinY < 0 && MaxY > 0)
// {
// x1 = (MinX - MinX)*Scale;
// y1 = MaxY*Scale;
// x2 = (MaxX - MinX)*Scale;
// y2 = MaxY*Scale;
// RPainter->drawLine(x1 , y1, x2, y2 );
// }
// 
// 
// 
// //Pen.setStyle(Qt::DashDotDotLine);
// Pen.setColor("black");
// Pen.setWidthF(1);
// RPainter->setPen(Pen);
// //рисуем картинку
// 	for (int i = 0; i <lines.count(); i++)
// 	{
// 	TmpLine = lines[i]->line();
// 	VecX1 = TmpLine.x1();
// 	VecY1 = -TmpLine.y1();
// 	VecX2 = TmpLine.x2();
// 	VecY2 = -TmpLine.y2();
// 
// 	x1 = (VecX1 - MinX)*Scale;
// 	y1 = (MaxY-VecY1 )*Scale;
// 	x2 = (VecX2 - MVMinX)*Scale;
// 	y2 = (MaxY-VecY2 )*Scale;
// 
// 	RPainter->drawLine(x1 , y1, x2, y2 );
// 	}
// //рисуем текст
// qreal FontSize,tmpX,tmpY;
// QString TmpText;
// //QFont font ( "Courier", -1, -1,  false );
// QFont font ( "Courier", 40 );
// RPainter->setFont(font);
// 	for (int i = 0; i<kumtexts.count(); i++)
// 	{
// 	FontSize = kumtexts[i]->Size*Scale/24; //Подобрано экспериментально
// 	tmpX = kumtexts[i]->x;
// 	tmpY = kumtexts[i]->y;
// 	x1 = (tmpX - MinX)*Scale/FontSize;
// 	y1 = (MaxY-tmpY )*Scale/FontSize;
// 
// 	TmpText = texts[i]->text();
// 	RPainter->scale(FontSize,FontSize);
// 	RPainter->drawText(x1,y1,TmpText);
// 	RPainter->scale(1/FontSize,1/FontSize);
// 	}
// 
// RPainter->end();
    return false;
}


//-----------------------------------------------------------------
//-----------------------------------------------------------------
void KumKuznec::MouseWheel(int Delta)
{
qWarning("wheel %i",Delta);
if (Delta > 0)
ZoomUp();
else
ZoomDown();
}
//-------------------------------------------------------
void KumKuznec::SetWindowSize(int x1, int y1, int w, int h)
{

SizeX = w;
SizeY = h;
MV->setGeometry(x1,y1,w,h);
MV->setFixedSize(w,h);
MV->resize(SizeX, SizeY+MenuHigth);
MV->setFixedSize(SizeX,SizeY+MenuHigth);
MV->setFixedHeight ( SizeY + MenuHigth+1);
viewFrame->setGeometry(0,MenuHigth+1,SizeX+1,SizeY);	

view->setGeometry(0,0, SizeX, SizeY);

 		 	qDebug()<<"Wx0"<<WindowX0;	 		

view->show();

WindowRedraw();
qDebug()<<"Redraw";
this->setGeometry(MV->geometry());


}
//----------------------------------------------
void KumKuznec::CreateMenu(void)
{

menu =MV->menuBar()->addMenu(QString::fromUtf8("&Задание"));

QAction* ClearPicture =new QAction(QString::fromUtf8("&Новое"),MV);
menu->addAction(ClearPicture);

QAction* LoadPicture =new QAction(QString::fromUtf8("&Загрузить"),MV);
menu->addAction(LoadPicture);

QAction* SavePicture =new QAction(QString::fromUtf8("&Сохранить"),MV);
menu->addAction(SavePicture);



//QAction* aPrint =new QAction(QString::fromUtf8("&Печать в файл"),MV);
//menu->addAction(aPrint);

//QAction* aToDubl =new QAction(QString::fromUtf8("&Запомнить стартовый"),MV);
//menu->addAction(aToDubl);

//QAction* aFromDubl =new QAction(QString::fromUtf8("&Восстановить стартовый"),MV);
//menu->addAction(aFromDubl);

connect ( ClearPicture, SIGNAL(triggered()), this, SLOT(ClearPicture()) );
connect ( LoadPicture, SIGNAL(triggered()), this, SLOT(LoadFromFileActivated()) );
connect ( SavePicture, SIGNAL(triggered()), this, SLOT(SaveToFileActivated()) );
//connect ( aToDubl, SIGNAL(triggered()), this, SLOT(ToDubl()) );
//connect ( aFromDubl, SIGNAL(triggered()), this, SLOT(FromDubl()) );
//connect ( aPrint, SIGNAL(triggered()), this, SLOT(PrintToPDF()) );
//MenuHigth = 20;
MenuHigth =  MV->menuBar()->height()-1;

menu1 =MV->menuBar()->addMenu(QString::fromUtf8("&Вид"));
QAction* UpAction =new QAction(QString::fromUtf8("&Крупнее"),MV);
menu1->addAction(UpAction);
connect ( UpAction, SIGNAL(triggered()), this, SLOT(ZoomUp()) );
QAction* DownAction =new QAction(QString::fromUtf8("&Мельче"),MV);
menu1->addAction(DownAction);
connect ( DownAction, SIGNAL(triggered()), this, SLOT(ZoomDown()) );

//QAction* FullPicture =new QAction(QString::fromUtf8("&Весь чертеж"),MV);
//menu1->addAction(FullPicture);
//connect ( FullPicture, SIGNAL(triggered()), this, SLOT(ToWindow()) );


QAction* Home =new QAction(QString::fromUtf8("&Восстановить"),MV);
menu1->addAction(Home);
connect ( Home, SIGNAL(triggered()), this, SLOT(ToCenter()) );

QAction* FindK =new QAction(QString::fromUtf8("&Найти кузнечика"),MV);
menu1->addAction(FindK);
connect ( FindK, SIGNAL(triggered()), this, SLOT(FindKuznec()) );

//QAction* Net =new QAction(QString::fromUtf8("&Сетка"),MV);
//menu1->addAction(Net);
//connect ( Net, SIGNAL(triggered()), this, //SLOT(ShowNetWindow()) );
/*
QAction* AInfo =new QAction(QString::fromUtf8("&Информация"),MV);
menu1->addAction(AInfo);
connect ( AInfo, SIGNAL(triggered()), this, SLOT(Info()) );
QAction* HorizPicture =new QAction(QString::fromUtf8("&Горизонтальное окно"),MV);
menu1->addAction(HorizPicture);
connect ( HorizPicture, SIGNAL(triggered()), this, SLOT(HorizWindow()) );

QAction* VertPicture =new QAction(QString::fromUtf8("&Вертикальное окно"),MV);
menu1->addAction(VertPicture);
connect ( VertPicture, SIGNAL(triggered()), this, SLOT(VertWindow()) );*/


}

//------------------------------
void KumKuznec:: HorizWindow()
{
//SetWindowSize(10,35,462,260);
}//------------------------------
void KumKuznec:: VertWindow()
{
//SetWindowSize(10,25,260,462);
}
//-------------------------------------
/*
void KumKuznec::CreateNetWindow(void)
{
int NetWinX0  = 40;
int NetWinY0  = 80;
int NetWinSizeX  = 160;
int NetWinSizeY  = 170;
NetWindow = new QMainWindow(MV,Qt::Tool);;
NetWindow->MV->setWindowTitle(QString::fromUtf8("Сетка"));
//NetWindow->setWindowModality(Qt::ApplicationModal);
NetWindow->setGeometry(NetWinX0,NetWinY0,NetWinSizeX,NetWinSizeY);
NetWindow->setMinimumSize(NetWinSizeX,NetWinSizeY);
NetWindow->setMaximumSize(NetWinSizeX,NetWinSizeY);


BoxNet = new QCheckBox(QString::fromUtf8("Показывать сетку?"),NetWindow);
BoxNet->move(20,10 );
BoxNet->resize(150,20);
BoxNet->pult.cpp:419: error: lvalue required as left operand of assignment
show();

BoxSqware = new QCheckBox(QString::fromUtf8("Сетка квадратная?"),NetWindow);
BoxSqware->move(20,30 );
BoxSqware->resize(150,20);
BoxSqware->show();




QLabel *lX = new QLabel(QString::fromUtf8("Начальная точка.X   Шаг.X "),NetWindow,0);
	lX->setGeometry( 45, 60, 150, 20 ) );

QLabel *lY = new QLabel(QString::fromUtf8("Начальная точка.Y   Шаг.Y"),NetWindow,0);
	lY->setGeometry( QRect(5, 110,150, 20 ) );

eX0 = new QLineEdit("",NetWindow);
 eX0->setGeometry( 25, 85 , 60,20 );

eStepX = new QLineEdit("",NetWindow);
 eStepX->setGeometry( 110, 85 , 60,20 );

 eY0 = new QLineEdit("",NetWindow);
 eY0->setGeometry( 25, 130 , 60,20 );

eStepY = new QLineEdit("",NetWindow);
 eStepY->setGeometry( 110, 130 , 60,20 );


QLabel *lX = new QLabel(QString::fromUtf8("     Шаг.X                Шаг.Y"),NetWindow,0);
	lX->setGeometry( QRect(5, 60, 150, 20 ) );

eStepX = new QLineEdit("",NetWindow);
 eStepX->setGeometry( 10, 85 , 60,20 );

eStepY = new QLineEdit("",NetWindow);
 eStepY->setGeometry( 90, 85 , 60,20 );
//  выключены
eX0 = new QLineEdit("",NetWindow);
 eX0->setGeometry( 25, 85 , 60,20 );
eX0->hide();
 eY0 = new QLineEdit("",NetWindow);
 eY0->setGeometry( 25, 130 , 60,20 );

eY0->hide();
//connect (eStepX, SIGNAL(editingFinished ()), this, SLOT(StepXChanged()));
//connect (eStepY, SIGNAL(editingFinished ()), this, SLOT(StepYChanged()));

connect (eStepX, SIGNAL(textChanged (QString)), this, SLOT(StepXChanged()));
connect (eStepY, SIGNAL(textChanged (QString)), this, SLOT(StepYChanged()));


QToolButton *btnCancel = new QToolButton(NetWindow);
btnCancel->setText(trUtf8("Отменить"));
//btnCancel->setToolTip(trUtf8("Отменить"));
btnCancel->move(10,130 );
btnCancel->resize(60,20);
btnCancel->show();

QToolButton *btnOK = new QToolButton(NetWindow);
btnOK->setText(trUtf8("OK"));
//btnOK->setToolTip(trUtf8("OK"));
btnOK->move(90,130 );
btnOK->resize(60,20);
btnOK->show();
connect(btnOK,SIGNAL(clicked()),this,SLOT(NetOKClick()));
connect(btnCancel,SIGNAL(clicked()),this,SLOT(NetCancelClick()));

 QLabel *lTemp = new QLabel(QString::fromUtf8("Температура"),InputWindow,0);
	lTemp->setGeometry( QRect( 8, 30, 75, 20 ) );

QLabel *lRad = new QLabel(QString::fromUtf8("� адиация"),InputWindow,0);
	lRad->setGeometry( QRect(95, 30, 75, 20 ) );

	QLabel *lBukv = new QLabel(QString::fromUtf8("Буквы (↓,↑)"),InputWindow,0);
	lBukv->setGeometry( QRect(155, 30, 60, 20 ) );

}
*/
//--------------------------------------
/*
void KumKuznec::ShowNetWindow(void)
{
if (SqwareNetFlag)
BoxSqware->setCheckState(Qt::Checked);
else
BoxSqware->setCheckState(Qt::Unchecked);

if (NetShowFlag)
BoxNet->setCheckState(Qt::Checked);
else
BoxNet->setCheckState(Qt::Unchecked);


char ctmp[200];
QString tmp;

sprintf(ctmp,"%7.2f",SdwigNetX);
tmp = QString(ctmp);
eX0->setText(tmp);

sprintf(ctmp,"%7.2f",SdwigNetY);
tmp = QString(ctmp);
eY0->setText(tmp);

sprintf(ctmp,"%7.2f",StepX);
tmp = QString(ctmp);
eStepX->setText(tmp);

sprintf(ctmp,"%7.2f",StepY);
tmp = QString(ctmp);
eStepY->setText(tmp);


NetWindow->show();
}
*/
//--------------------------
/*
void KumKuznec::NetSqware(int mode)
{
if (mode == 0)
SqwareNetFlag = false;
else
SqwareNetFlag = true;
qWarning("mode %i",SqwareNetFlag);
}
*/
//--------------------------------------------------------
void KumKuznec::StepXChanged(void)
{
	if (BoxSqware->isChecked())
	{
	eStepY->setText(eStepX->displayText());
	}
}
//---------------------
void KumKuznec::StepYChanged(void)
{
	if (BoxSqware->isChecked())
	{
	eStepX->setText(eStepY->displayText());
	}
}
//---------------------------------------------------------
void KumKuznec::NetCancelClick(void)
{
NetWindow->close();
}
//---------------------------------------------------------
void KumKuznec::NetOKClick(void)
{
if (BoxNet->isChecked())
NetShowFlag = true;
else
NetShowFlag = false;

if (BoxSqware->isChecked())
SqwareNetFlag = true;
else
SqwareNetFlag = false;

bool ok,ok1;


StepX = eStepX->displayText().toFloat(&ok);
StepY = eStepY->displayText().toFloat(&ok1);
//SdwigNetX = eX0->displayText().toFloat(&ok2);
//SdwigNetY = eY0->displayText().toFloat(&ok3);
SdwigNetX = 0.;
SdwigNetY = 0.;

if (!(ok && ok1))
{
QString tmp = QString::fromUtf8("Проверьте данные!");
QMessageBox::warning( 0, QString::fromUtf8("Ошибки ввода чисел"),tmp , 0,0,0);
return;
}

NetWindow->close();
DrawNet();
}

//---------------------------------------
void KumKuznec::DrawText(QString Text, qreal Scale)
{
QString t = Text;
KumKuznecText  *KText = new KumKuznecText();
KText->x = CurX;
KText->y = CurY;
KText->text = t;
KText->color.Red = CurColor.Red;
KText->color.Green = CurColor.Green;
KText->color.Blue = CurColor.Blue;
KText->Size = Scale;

QFont font ( "Courier", -1, -1,  false );

//QGraphicsTextItem *m_text = scene->addText(t,font);
QGraphicsSimpleTextItem *m_text = scene->addSimpleText(t,font);
//m_text->setDefaultTextColor(QColor(CurColor.Red,CurCoizelor.Green,CurColor.Blue));
m_text->setBrush(QColor(CurColor.Red,CurColor.Green,CurColor.Blue));
m_text->setPen(QColor(CurColor.Red,CurColor.Green,CurColor.Blue));
qreal x0 = CurX; 
//- Scale*0.2; //Цифры подобраны экспериментально
qreal y0 = -CurY-Scale*1.4;

m_text->setFont(font);
texts.append(m_text);
kumtexts.append(KText);		
    QList<QGraphicsItem *> scene_items = scene->items();
	
//scene_items.last()->scale(0.1*Scale,0.1*Scale);
    scene_items.last()->setScale(0.1 * Scale);
scene_items.last()->setPos(x0,y0);

CurZ += 0.01;
scene_items.last()->setZValue(CurZ);

qreal TextWidth = t.length()*Scale;

bool OldPenPosition = PenPosition;
PenPosition = false;
GoVector(TextWidth,0);
PenPosition = OldPenPosition;
GoVector(0,0);    // Чтобы цвет пера был правильным
WindowRedraw();
}

//-------------------------------------
/*
void KumKuznec::CreateInfoWindow(void)
{
QPalette PallBlue;
PallBlue.setColor(QPalette::BrightText,QColor(0,0,100));
PallBlue.setBrush(QPalette::WindowText,QBrush(QColor(0,0,200)));
InfoWindow = new QMainWindow(MV,Qt::Tool);;
InfoWindow->MV->setWindowTitle(QString::fromUtf8("Информация"));
//NetWindow->setWindowModality(Qt::ApplicationModal);
InfoWindow->setGeometry(40,80,210,210);
InfoWindow->setMinimumSize(210,210);
InfoWindow->setMaximumSize(210,210);

QLabel *lWin = new QLabel(QString::fromUtf8("ОКНО"),InfoWindow,0);
	lWin->setGeometry( QRect(90, 1, 110, 20 ) );
	lWin->setPalette(PallBlue);


QLabel *lX = new QLabel(QString::fromUtf8("XМИН= "),InfoWindow,0);
	lX->setGeometry( QRect(10, 20, 110, 20 ) );
	lX->setPalette(PallBlue);
QLabel *lY = new QLabel(QString::fromUtf8("YМИН= "),InfoWindow,0);
	lY->setGeometrpult.cpp:419: error: lvalue required as left operand of assignment
y( QRect(110, 20,110, 20 ) );
	lY->setPalette(PallBlue);
	lX0 = new QLabel("1.12",InfoWindow,0);
	lX0->setGeometry( QRect(60, 20,50, 20 ) );
	
	lX1 = new QLabel("1.12",InfoWindow,0);
	lX1->setGeometry( QRect(60, 40,50, 20 ) );

QLabel *lXX1 = new QLabel(QString::fromUtf8("XМАКС= "),InfoWindow,0);
	lXX1->setGeometry( QRect(10, 40, 110, 20 ) );
	lXX1->setPalette(PallBlue);
QLabel *lYY1 = new QLabel(QString::fromUtf8("YМАКС= "),InfoWindow,0);
	lYY1->setGeometry( QRect(110, 40,110, 20 ) );
	lYY1->setPalette(PallBlue);
	
	lY0 = new QLabel("1.12",InfoWindow,0);
	lY0->setGeometry( QRect(160, 20,50, 20 ) );
	
	lY1 = new QLabel("1.12",InfoWindow,0);
	lY1->setGeometry( QRect(160, 40,50, 20 ) );


QLabel *lXXCen = new QLabel(QString::fromUtf8("XЦЕН= "),InfoWindow,0);
	lXXCen->setGeometry( QRect(10, 60, 110, 20 ) );
	lXXCen->setPalette(PallBlue);
QLabel *lYYCen = new QLabel(QString::fromUtf8("YЦЕН = "),InfoWindow,0);
	lYYCen->setGeometry( QRect(110, 60,110, 20 ) );
	lYYCen->setPalette(PallBlue);
	
	lXCen = new QLabel("1.12",InfoWindow,0);
	lXCen->setGeometry( QRect(60, 60,50, 20 ) );
	
	lYCen = new QLabel("1.12",InfoWindow,0);
	lYCen->setGeometry( QRect(160, 60,50, 20 ) );

QLabel *lXXNet = new QLabel(QString::fromUtf8("ШАГ СЕТКИ "),InfoWindow,0);
	lXXNet->setGeometry( QRect(10, 90, 110, 20 ) );
	lXXNet->setPalette(PallBlue);
QLabel *lXXNetUzX0 = new QLabel(QString::fromUtf8("Узел X0="),InfoWindow,0);
	lXXNetUzX0->setGeometry( QRect(10, 110, 110, 20 ) );
	lXXNetUzX0->setPalette(PallBlue);
	lXXNetUzX0->hide();
QLabel *lXXNetUzY0 = new QLabel(QString::fromUtf8("Y0="),InfoWindow,0);
	lXXNetUzY0->setGeometry( QRect(130, 110, 110, 20 ) );
	lXXNetUzY0->setPalette(PallBlue);
	lXXNetUzY0->hide();
//QLabel *lXXNetUzDX = new QLabel(QString::fromUtf8(" DX="),InfoWindow,0);
//	lXXNetUzDX->setGeometry( QRect(10, 110, 110, 20 ) );
//	lXXNetUzDX->setPalette(PallBlue);
//QLabel *lXXNetUzDY = new QLabel(QString::fromUtf8("DY="),InfoWindow,0);
//	lXXNetUzDY->setGeometry( QRect(130, 110, 110, 20 ) );
//	lXXNetUzDY->setPalette(PallBlue);

	lXNetX0 = new QLabel("1.12",InfoWindow,0);
	lXNetX0->setGeometry( QRect(70, 110,50, 20 ) );
	lXNetX0->hide();
	lYNetY0 = new QLabel("1.12",InfoWindow,0);
	lYNetY0->setGeometry( QRect(160, 110,50, 20 ) );
	lYNetY0->hide();
	
	lXNetDX = new QLabel("1.12",InfoWindow,0);
	lXNetDX->setGeometry( QRect(90, 90,50, 20 ) );
	
	lYNetDY = new QLabel("1.12",InfoWindow,0);
	lYNetDY->setGeometry( QRect(140, 90,50, 20 ) );


	//lZ0 = new QLabel("1.12",InfoWindow,0);
	//lZ0->setGeometry( QRect(120, 80,50, 20 ) );


 lColor = new QLabel("",InfoWindow,0);
	lColor->setGeometry( QRect(10, 130,120, 20 ) );

lPen = new QLabel("",InfoWindow,0);
	lPen->setGeometry( QRect(10, 150,120, 20 ) );


QToolButton *btnOK = new QToolButton(InfoWindow);
btnOK->setText(trUtf8("OK"));
//btnOK->setToolTip(trUtf8("OK"));
btnOK->move(80,175);
btnOK->resize(60,20);
btnOK->show();
connect(btnOK,SIGNAL(clicked()),this,SLOT(InfoOKClick()));

}
*/

//---------------------------------------------------------
void KumKuznec::InfoOKClick(void)
{
InfoWindow->close();
}
//-----------------------------------------
void KumKuznec::CreateXYLabel(void)
{
lXCur = new QLabel(QString::fromUtf8("X= "),MV,0);
	lXCur->setGeometry( QRect(ButtonSdwig+90, 0, 80, 20 ) );
lXCur->hide();

lYCur = new QLabel(QString::fromUtf8("Y= "),MV,0);
	lYCur->setGeometry( QRect(ButtonSdwig+90, 12, 80, 20 ) );
lYCur->hide();
//qWarning("XYYYYYYYY");
}
//---------------------------------------------
void KumKuznec::ShowCoord(qreal xScene,qreal yScene)
{
QString tmp;
char ctmp[50];
sprintf(ctmp,"X = %7.4f",xScene);
tmp = QString(ctmp);
//QMessageBox::information( 0, "", tmp, 0,0,0);
lXCur->setText(tmp);
sprintf(ctmp,"Y = %7.4f",yScene);
tmp = QString(ctmp);
lYCur->setText(tmp);

QPalette PallBlue;
PallBlue.setColor(QPalette::BrightText,QColor(1,1,1));
PallBlue.setBrush(QPalette::WindowText,QBrush(QColor(1,1,1)));
lXCur->setPalette (PallBlue);
lYCur->setPalette (PallBlue);

lXCur->show();
lYCur->show();
}
//-------------------------------------
void KumKuznec::HideCoord(void)
{
lXCur->hide();
lYCur->hide();
}

void KumKuznec::LoadFromFileActivated()
{
    QSettings sett("NIISI RAS", "Grasshopper");
    QString cd = sett.value("Last", curDir).toString();



 QString	VodFileN=QFileDialog::getOpenFileName(this, QString::fromUtf8 ("Открыть файл"), curDir, "(*.kz)");

 sett.setValue("Last", VodFileN);
 if(LoadFromFile(VodFileN)>0){
	QMessageBox::information( 0, "", trUtf8("Чтение записи: ") + VodFileN, 0,0,0);
        return;
	};
};

void KumKuznec::SaveToFileActivated()
{
    QSettings sett("NIISI RAS", "Grasshopper");
    QString cd = sett.value("Last", curDir).toString();
 QFileDialog dialog(MV,trUtf8("Сохранить файл"),curDir, "(*.kz)");
 dialog.setAcceptMode(QFileDialog::AcceptSave);
 if(!dialog.exec())return;
 QString VodFile=dialog.selectedFiles().first();
 if(VodFile.right(3)!=".kz")VodFile+=".kz";
 sett.setValue("Last", VodFile);
  QFile vFile(VodFile);
  	if  (!vFile.open(QIODevice::WriteOnly))
	{
	QMessageBox::information( 0, "", trUtf8("Ошибка записи: ") + VodFile, 0,0,0);
        return;
	};
if(!borderEnable)leftBorder=-1024;
if(!borderEnable)rightBorder=1024;

  //Zapis v fayl
 vFile.write(";Step forward Step Back\n");
 vFile.write(QString::number(Fstep).toLatin1()+" "+QString::number(Bstep).toLatin1()+"\n");
 vFile.write(";StartPos\n");
 vFile.write(QString::number(startPos).toLatin1()+"\n");
 vFile.write(";Left border Right border\n");
 vFile.write(QString::number(leftBorder).toLatin1()+" "+QString::number(rightBorder).toLatin1()+"\n");
 vFile.write(";Flags\n");
 for(int i=0;i<flags_pos.count();i++)
 vFile.write(QString::number(flags_pos[i]).toLatin1()+" ");
 vFile.write("\n;END\n");
fileName=QFileInfo(vFile).fileName ();
MV->setWindowTitle ( QString::fromUtf8("Кузнечик - ")+QFileInfo(vFile).fileName ());
//kuznHeader->setWMTitle( QString::fromUtf8("Кузнечик - ")+QFileInfo(vFile).fileName ());
};
//--------------------------------------------------------------
void KumKuznec::MousePress(qreal x,qreal y,bool LeftButtonFlag,qreal xScene, qreal yScene)
{

qWarning("Draw::MousePress %i",LeftButtonFlag);

qDebug()<<"Center"<<center();
  if (LeftButtonFlag)
  {
  LeftMousePressFlag = true;
  OldX = x;
  OldY = y;
  MVxmin = MV->x()-1000;
  MVxmax = MVxmin+MV->width()+2000;
  MVymin = MV->y();
  MVymax = MVymin + MV->height();
  QApplication::setOverrideCursor(Qt::PointingHandCursor);  
  };


}
//---------------------------------
void KumKuznec::MouseRelease(qreal x,qreal y,bool LeftButtonFlag)
{
  if (LeftButtonFlag)
  {
   LeftMousePressFlag = false;
   QApplication::restoreOverrideCursor();

   //DrawNet();
//qWarning("DrawNet"); 
  }else RightMousePressFlag=false;
//HideCoord();
//qWarning("HideCoord");
}
//-------------------------------------------
void KumKuznec::MouseMove(int x, int y,bool LeftButtonFlag)
{
//if(!LeftMousePressFlag){if(!RightMousePressFlag)return;ShowCoord(xScene,-yScene);return;};
if(moving)return;
if(!LeftMousePressFlag)return;
//qDebug()<<"MMove "<<x;
if((abs(x-OldX)<5)&&(abs(y - OldY)<5)){qDebug()<<"return 1";return;};

//QApplication::setOverrideCursor(Qt::PointingHandCursor);  
qreal centY=center().y();
WindowX0 = WindowX0+(view->mapToScene(QPoint(OldX,OldY)).x()-view->mapToScene(QPoint(x,y)).x());
//WindowY0 =WindowY0;
QRectF curentRect;


//curentRect.setRect(WindowX0,WindowY0,SizeX/WindowZoom,sceneSizeY());
qDebug()<<"sceneSizeY()"<<sceneSizeY();
//scene->setSceneRect(curentRect);
qDebug()<<"XZdvig"<<- (x - OldX)/WindowZoom;
moving=true;
view->centerOn(center().x()- (x - OldX)/WindowZoom,-1);
scene->update();
view->update();
scene->invalidate();
OldX = x;
OldY = y;
qDebug()<<"WX0"<<WindowX0;


if(sledi.count()>0)
	{
	sledi.last()->show();
	//qDebug()<<sledi.last()->isEnabled()<<"RECT:"<<sledi.last()->sceneBoundingRect();
	};
//WindowRedraw();
//qWarning("Draw::MouseMove %f %f",x,y);
moving=false;
}
void KumKuznec::createFlags()
{
 qDebug()<<"Create f";
 for(int i=0;i<flags.count();i++)
 {
 if(flags[i]->flagItem)scene->removeItem(flags[i]->flagItem);
 };
 flags.clear();
 for(int i=0;i<flags_pos.count();i++)
 {
 KuznFlag* curFlag=new KuznFlag();
 curFlag->flagItem=new QGraphicsPolygonItem(mFlag->polygon());
 curFlag->f_pos=flags_pos[i];
// curFlag->flagItem->scale(BASEZOOM*2,BASEZOOM*2);
 curFlag->flagItem->setScale(BASEZOOM * 2);
 curFlag->flagItem->setZValue(99);
 curFlag->flagItem->setBrush(QBrush(QColor(150,100,100)));
 curFlag->flagItem->setPos(curFlag->f_pos*ZOOMMULTIP,-ZOOMMULTIP);
 flags.append(curFlag);
 scene->addItem(curFlag->flagItem);
 };

};

void KumKuznec::ColorUncolor()
     {
	int id=colored();
	
	if(id>-1) //Убираем закраску
	 {
	  if(!zakr[id]->zakraska){qDebug()<<"KumKuznec::ColorUncolor():No rect!";return;};
	  scene->removeItem(zakr[id]->zakraska);
	  zakr.removeAt(id);
	  return;
	 }else //красим
         {
	  zakr.append(new KuznZakr());
	  zakr.last()->zakraska=new QGraphicsRectItem(CurX-0.2*ZOOMMULTIP,-0.2*ZOOMMULTIP,0.4*ZOOMMULTIP,0.4*ZOOMMULTIP);
	  zakr.last()->zakraska->setBrush(QBrush(QColor(100,100,100)));
	  zakr.last()->pos=CurX;
	  scene->addItem(zakr.last()->zakraska);
	 };
     };
void KumKuznec::FindKuznec()
{
qDebug()<<"WZ:"<<WindowZoom<<"SizeX/Zoom"<<(SizeX/2)/WindowZoom;


view->centerOn(CurX,WindowY0-SizeY/2);
WindowX0=CurX-SizeX/(WindowZoom*2);
WindowRedraw();
};

void KumKuznec::addSled(int start,int fin)
{
int maxColor=200;
int delitel=sledi.count()+1;
int step=maxColor/delitel;
sledi.append(new KuznSled(start,fin*ZOOMMULTIP));
scene->addItem(sledi.last());
sledi.last()->moveBy(0,-1.2*ZOOMMULTIP);
sledi.last()->setZValue(1);
for(int i=0;i<sledi.count();i++)
	{
	if(maxColor-step*(i+1) >150)sledi[i]->hide();
	sledi[i]->setInt(maxColor-step*(i+1));
	
	};
scene->update();
};
void KumKuznec::closeEvent ( QCloseEvent * event )
{
qDebug()<<"KUZNEC CLOSE CloseEvent";
if((Kpult->libMode)||(autoClose))
	{
        qDebug()<<"Lib mode: "<<Kpult->libMode<<" autoClose: "<<autoClose;
	 close();
         event->accept();
	 return;
	};

int ret = QMessageBox::warning(this, QString::fromUtf8("Кузнечик"),
                        QString::fromUtf8("Закрыть исполнитель кузнечик?"),
                        QMessageBox::Yes | QMessageBox::Default,
                        QMessageBox::No,
                        QMessageBox::Cancel | QMessageBox::Escape);
     if (ret == QMessageBox::Yes) {

        Kpult->AutoClose();
	 Kpult->close();
         event->accept();
     } else {
         event->ignore();
     }
};
void KumKuznec::resizeSlot(QSize * oldSize,QSize* newSize)
{
    QSize newSize_p=*newSize;

    resizeEvent(new QResizeEvent(newSize_p,size()));
};
