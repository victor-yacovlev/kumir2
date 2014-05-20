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
#include "pult.h"
const double DegreesPerMinute = 7.0;
const double DegreesPerSecond = DegreesPerMinute / 60;
const int MaxMinutes = 45;
const int MaxSeconds = 620;
const int UpdateInterval = 5;





linkLight::linkLight ( QWidget* parent):QWidget(parent)
				{
				 posX=1;
				 posY=1;
				 onLine=true;
				 text=trUtf8("СВЯЗЬ");
				};

void linkLight::paintEvent ( QPaintEvent * event )
      {
      QPainter painter(this);
      painter.setRenderHint (QPainter::Antialiasing,true );
      QRectF rectangle(posX, posY, 10.0, 10.0);
      
      QPen pen(Qt::green);  // creates a default pen

      QBrush brush(Qt::green);
      if(!onLine)brush.setColor(QColor(20,60,20));
      brush.setStyle(Qt::SolidPattern);
      painter.setPen(pen);
      painter.setBrush(brush);
      painter.drawEllipse(rectangle);

      brush.setColor(QColor(10,10,10));
      painter.setPen(QPen(QColor(10,10,10)));
      painter.setBrush(brush);
      QFont font("Arial");
      font.setBold(true);
      painter.setFont(font);
	int cur_pos=0;
      for(int i=0;i<text.length();i++)
	{painter.drawText(posX,posY+TEXT_STEP*i+26,QString(text[i]));
	cur_pos=posY+TEXT_STEP*i+26;};
       QRectF rectangle2(posX, cur_pos+7, 10, 10);
      brush.setColor(QColor(30,0,0));	
      if(!onLine)brush.setColor(QColor(250,50,50));
      pen.setColor(Qt::red);
      
      brush.setStyle(Qt::SolidPattern);
      painter.setPen(pen);
      painter.setBrush(brush);
      painter.drawEllipse(rectangle2);
 

      };



MainButton::MainButton ( QWidget* parent):QWidget(parent)
				{direction=UP;
				 posX=1;
				 posY=1;
				 buttonImageUp.load(":/1.png");
				 buttonImageDown.load(":/1.png");
				 downFlag=false;
				 Parent=parent;
				 int mid=buttonImageUp.width()/2;
				// upArrow.append(QLine(mid,30,mid-15,40));
				// upArrow.append(QLine(mid,30,mid+15,40));
				// downArrow.append(QLine(mid,40,mid-15,30));
				// downArrow.append(QLine(mid,40,mid+15,30));
				// leftArrow.append(QLine(30,mid,40,mid-15));
				// leftArrow.append(QLine(30,mid,40,mid+15));
				// rightArrow.append(QLine(40,mid,30,mid-15));
				// rightArrow.append(QLine(40,mid,30,mid+15));
				 text="";
                                 checked=false;
                                 checkable=false;
				 mouseOver=false;	
				 icon=false;
				 resize(71,71);
				};

void MainButton::paintEvent ( QPaintEvent * event )
      {
      QPainter painter(this);
      painter.setRenderHint (QPainter::Antialiasing,true );
      if(!downFlag)
		{
		painter.drawImage(QPoint(posX,posY),buttonImageUp);
		if(icon)painter.drawImage(1,1,buttonIcon);
		QPen blackPen(QColor(40,40,40));
		blackPen.setWidth(3);
		painter.setPen(blackPen);
		drawAddons(&painter);
		if(mouseOver)
			{
			QLinearGradient grad( 1, 1, 5, 65);
			grad.setColorAt(0.7,QColor(200,190,222));
			grad.setColorAt(0.3,QColor(191,208,208));

			QBrush solidBrush(grad);
			painter.setBrush(solidBrush);
			painter.setOpacity(0.1);
			painter.drawRect(3,3,155,50);
			};
		
		}
	else 	{
		painter.drawImage(QPoint(posX,posY),buttonImageDown);
		if(icon){
			//buttonIcon.invertPixels();
			painter.drawImage(3,3,buttonIcon);
			//buttonIcon.invertPixels();
			};
		QPen whitePen(QColor(170,170,170));
		whitePen.setWidth(3);
		painter.setPen(whitePen);
                //drawAddons(&painter);
		
      		};
      };

void MainButton::drawAddons(QPainter* painter)
	{
		if(direction==UP)painter->drawLines(upArrow);
		if(direction==DOWN)painter->drawLines(downArrow);
		if(direction==LEFT)painter->drawLines(leftArrow);		
		if(direction==RIGHT)painter->drawLines(rightArrow);
		if(direction==TEXT)
			{
			if(!downFlag)painter->setPen(QColor(10,10,10));
			QFont font("FreeSans");
      			font.setBold(true);
      			painter->setFont(font);
			QStringList textLines=text.split("|");
			int start_pos=42-7*textLines.count();
			for(int i=0;i<textLines.count();i++)painter->drawText(7,start_pos+TEXT_STEP*i,textLines[i]);
			};

	};
		bool MainButton::loadIcon(QString iconFile)
			{
			icon=true;
			return buttonIcon.load(iconFile);
			};
void MainButton::mousePressEvent ( QMouseEvent * event )
      {
	qWarning("MousePress1");
	emit pressed();
        if(checkable)checked=!checked;
        downFlag=true;
	repaint();
      };
void MainButton::mouseReleaseEvent ( QMouseEvent * event )
      {
	if(checkable){if(!checked)downFlag=false;}else downFlag=false;
	if(mouseOver)emit clicked();
	repaint();
      };


 void MainButton::enterEvent ( QEvent * event )
  {
   if(!mouseOver){mouseOver=true;repaint();};
  
   
  };
 void MainButton::leaveEvent ( QEvent * event )
  {
  if(mouseOver){mouseOver=false;repaint();};
  };





loggerButton::loggerButton ( QWidget* parent):QWidget(parent)
				{isUpArrow=false;
				 posX=1;
				 posY=1;
				 buttonImageUp.load(":/butt.png");
				 buttonImageDown.load(":/buttd.png");
				 downFlag=false;
				 Parent=parent;
				 int mid=buttonImageUp.width()/2;
				 upArrow.append(QLine(mid,10,mid-11,15));
				 upArrow.append(QLine(mid,10,mid+11,15));
				 downArrow.append(QLine(mid,15,mid-11,10));
				 downArrow.append(QLine(mid,15,mid+11,10));
				};

void loggerButton::paintEvent ( QPaintEvent * event )
      {
      QPainter painter(this);
      painter.setRenderHint (QPainter::Antialiasing,true );
      if(!downFlag)
		{
		painter.drawImage(QPoint(posX,posY),buttonImageUp);
		QPen blackPen(QColor(50,50,50));
		blackPen.setWidth(3);
		painter.setPen(blackPen);
		if(isUpArrow)painter.drawLines(upArrow);else painter.drawLines(downArrow);
		}
	else 	{
		painter.drawImage(QPoint(posX,posY),buttonImageDown);
		QPen whitePen(QColor(170,170,170));
		whitePen.setWidth(3);
		painter.setPen(whitePen);
		if(isUpArrow)painter.drawLines(upArrow);else painter.drawLines(downArrow);
      		};
      };


void loggerButton::mousePressEvent ( QMouseEvent * event )
      {
	qWarning("MousePress");
	emit pressed();
        downFlag=true;
	repaint();
      };
void loggerButton::mouseReleaseEvent ( QMouseEvent * event )
      {
      	downFlag=false;
	repaint();
      };





pultLogger::pultLogger ( QWidget* parent):QWidget(parent)
{
mainFrame= new QFrame(parent);
mainFrame->setLineWidth(2);
mainFrame->setFrameShadow(QFrame::Sunken);
mainFrame->setFrameShape(QFrame::Panel);
mainFrame->setPalette(QPalette(QColor(50,50,50),QColor(100,100,100)));
mainFrame->setBackgroundRole(QPalette::Window);
mainFrame->setAutoFillBackground(true);

dummyFrame= new QFrame(mainFrame);
dummyFrame->setLineWidth(0);
dummyFrame->setFrameShadow(QFrame::Sunken);
dummyFrame->setFrameShape(QFrame::Panel);
dummyFrame->setPalette(QPalette(QColor(50,50,50),QColor(100,100,100)));
dummyFrame->setBackgroundRole(QPalette::Window);
dummyFrame->setAutoFillBackground(true);
dummyFrame->move(2,2);
W=150;H=160;
respFrame= new QFrame(mainFrame);
respFrame->setLineWidth(0);
respFrame->setFrameShadow(QFrame::Sunken);
respFrame->setFrameShape(QFrame::Panel);
respFrame->setPalette(QPalette(QColor(50,50,50),QColor(120,110,110)));
respFrame->setBackgroundRole(QPalette::Window);
respFrame->setAutoFillBackground(true);
respFrame->move(W-RESP_PANEL,2);


downButton=new loggerButton(parent);
downButton->move(0,H-24);
downButton->resize(140,24);
downButton->show();

upButton=new loggerButton(parent);
upButton->move(0,8);
upButton->resize(140,26);
upButton->upArrowType(true);
upButton->show();

pos=4;
connect(downButton,SIGNAL(pressed()),this,SLOT(downBtnPressed()));
connect(upButton,SIGNAL(pressed()),this,SLOT(upBtnPressed()));
//buttonSize=50;
 
};
pultLogger::~pultLogger ()
{
//delete mainFrame;
};

        void pultLogger::Move(uint x,uint y)
	{
	mainFrame->move( x, y+26);
	//dummyFrame->move( x+1, y+27);
        int offset=(W-LOGGER_BUTTONS)/2;
	downButton->move(x+offset,y+H-24);
	upButton->move(x+offset,y);
	//testLabel->move(4,30);
//	qDebug()<<"upBtn geom "<<upButton->geometry();
	qWarning("WidgetMove");
	move(x,y);
	};

	void pultLogger::setSizes(uint w,uint h)
	{
	mainFrame->resize ( w, h-50);
	dummyFrame->resize ( w-RESP_PANEL, h);
	respFrame->resize ( RESP_PANEL, h);
        respFrame->move(w-RESP_PANEL,2);
	qDebug()<<"dummy geom"<<dummyFrame->geometry();
	resize(w,h);
	W=w;H=h;
	buttonSize=w;
	};

void pultLogger::appendText(QString kumCommand,QString text,QString replay)
{
while(pos>H-68)downBtnPressed();
lines.append(logLine(kumCommand,text,replay,dummyFrame,respFrame,pos));
pos=pos+TEXT_STEP;
};


void pultLogger::upBtnPressed()
{

 if(lines.count()==0)return;
 if(lines.first().pos()>2)return;

 for(int i=0;i<lines.count();i++)lines[i].moveDown();
 pos=pos+SCROLL_STEP;
 qWarning("TEXT MOVE");
};
void pultLogger::downBtnPressed()
{
  if(pos<14)return;
  for(int i=0;i<lines.count();i++)lines[i].moveUp();
  pos=pos-SCROLL_STEP;
};

void pultLogger::ClearLog()
{
 for(int i=0;i<lines.count();i++)lines[i].removeLabels();
lines.clear();
pos=4;

};
void pultLogger::CopyLog()
{
if(lines.count()==0)return;
QClipboard * cp=QApplication::clipboard ();
QString text;
for(int i=0;i<lines.count();i++)if(!lines[i].KumCommand().isEmpty())text+=lines[i].KumCommand()+"\n";
cp->setText(text);
};


GrasshopperPult::GrasshopperPult( QDir resdir,QWidget* parent, Qt::WindowFlags fl )
		: QWidget ( parent, fl ), Ui::TurtlePult()
{
//	setStyleSheet("background-color:#7EB676;");
	autoClose=false;
	setupUi ( this );
//	this->hide();
//	setWindowFlags(Qt::Dialog);

	Logger=new pultLogger(this);

	Logger->setSizes(164,150);
	Logger->Move(40,5);	
        greenLight=new linkLight(this);
        greenLight->move(15,29);
        greenLight->resize(12,104);

       
     

        LeftB->hide();
        turnLeft=new MainButton(this);
	turnLeft->setGeometry(LeftB->geometry());
        turnLeft->loadIcon(resdir.absoluteFilePath("160_55l.png"));
 
       // RightB->hide();
        turnRight=new MainButton(this);
	turnRight->setGeometry(RightB->geometry());
        turnRight->loadIcon(resdir.absoluteFilePath("160_55r.png"));
        
//        StenaB->hide();
        askStena=new MainButton(this);
//        askStena->move(StenaB->pos());
        askStena->setCheckable(true);
	askStena->setText(trUtf8("  "));
	askStena->loadIcon(resdir.absoluteFilePath("stena.png"));
	askStena->setCheckable(true);
	askStena->hide();
//        SvobodnoB->hide();
        askFree=new MainButton(this);
//        askFree->move(SvobodnoB->pos());
        askFree->setCheckable(true);
	askFree->setText(trUtf8(" "));
	askFree->loadIcon(":/icons/svobodno.png");
	askFree->setCheckable(true);
	askFree->hide();
  

        recolorB=new MainButton(this);
	recolorB->setGeometry(colorB->geometry());  
	recolorB->setText(trUtf8("  "));
	recolorB->loadIcon(resdir.absoluteFilePath("recolor.png"));
	colorB->hide();


        QIcon toKumirIco(":/kumir.png");
	toKumir->setIcon(toKumirIco);
	
//	CenterB->setIcon(QIcon(":/icons/robo_field.png"));
//	connect(buttFwd,SIGNAL(clicked()),this,SLOT(Up()));


	//connect(buttBack,SIGNAL(clicked()),this,SLOT(Down()));
	connect(turnLeft,SIGNAL(clicked()),this,SLOT(Left()));
	connect(turnRight,SIGNAL(clicked()),this,SLOT(Right()));



	connect(ClearLog,SIGNAL(clicked()),Logger,SLOT(ClearLog()));
        connect(ClearLog,SIGNAL(clicked()),this,SLOT(resetKuznec()));

        connect(toKumir,SIGNAL(clicked()),this,SLOT(logToKumir()));
        connect(recolorB,SIGNAL(clicked()),this,SLOT(ColorUnColor()));
//	connect(CopyLog,SIGNAL(clicked()),Logger,SLOT(CopyLog()));
    setMinimumSize(100,200);
	link=true;
};


void GrasshopperPult::noLink(){
	link=false;
	greenLight->setLink(link);greenLight->repaint();qWarning("NoLINK");
	ClearLog->setEnabled(false);toKumir->setEnabled(false);};
void GrasshopperPult::LinkOK(){qDebug()<<"LinkOK";link=true;greenLight->setLink(link);greenLight->repaint();ClearLog->setEnabled(true);toKumir->setEnabled(true);};

void GrasshopperPult::Up()
  {
  if(!greenLight->link())return;


  

  
  };
void GrasshopperPult::Down()
  {
  if(!greenLight->link())return;



  };
void GrasshopperPult::Left()
  {
   if(!greenLight->link())return;
	if(kuznecObj->canBack())
	{
  Logger->appendText(QString::fromUtf8("назад ")+QString::number(BackNumber->intValue()),QString::fromUtf8("назад ")+QString::number(BackNumber->intValue()),"OK");
 // kuznecObj->grad=-gradVal->value();
  kuznecObj->MoveBack();
       }else
	{
  Logger->appendText(QString::fromUtf8("назад ")+QString::number(BackNumber->intValue()),QString::fromUtf8("назад ")+QString::number(BackNumber->intValue()),QString::fromUtf8("ОТКАЗ"));
	};
  };
void GrasshopperPult::Right()
  {
   if(!greenLight->link())return;
   if(kuznecObj->canFwd()){
  Logger->appendText(QString::fromUtf8("вперед ")+QString::number(FwdNumber->intValue()),QString::fromUtf8("вперед ")+QString::number(FwdNumber->intValue()),"OK");
 // kuznecObj->grad=gradVal->value();
  kuznecObj->MoveFwd();
	}else
	{
  Logger->appendText(QString::fromUtf8("вперед ")+QString::number(FwdNumber->intValue()),QString::fromUtf8("вперед ")+QString::number(FwdNumber->intValue()),QString::fromUtf8("ОТКАЗ"));
	
	};
  };

void GrasshopperPult::SwStena()
  {
   if(askFree->isChecked () ) askFree->setChecked(false);	
  };
void GrasshopperPult::SwSvobodno()
  {
   if(askStena->isChecked () )askStena->setChecked(false);
  };
void GrasshopperPult::TempS()
  {
  if(!greenLight->link())return;
 
  };
void GrasshopperPult::RadS()
  {
  if(!greenLight->link())return;

  };

void GrasshopperPult::resetKuznec()
 {
  kuznecObj->Reset();
 };
void GrasshopperPult::newClient(QString message)
{
 label->setText(QString::fromUtf8("Подключился ")+message);
 qDebug()<<"Connected";
 toKumir->setEnabled(true);
}

void GrasshopperPult::clientDisconnect()
{
 label->setText(QString::fromUtf8("Клиент отключился"));
qDebug()<<"Disconect";
 LinkOK();
 toKumir->setEnabled(false);
 
};


void GrasshopperPult::logToKumir()
{
emit sendText(Logger->log());
//Server->sendCmdAllClients(QString::fromUtf8("Вставить Текст,")+Logger->log()+"\n");
};


void GrasshopperPult::closeEvent ( QCloseEvent * event )
{
if((libMode)||(autoClose))
	{
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
	 kuznecObj->AutoClose();
	 kuznecObj->close();
         event->accept();
     } else {
         event->ignore();
     }
};

void GrasshopperPult::ColorUnColor()
{
 Logger->appendText(QString::fromUtf8("перекрасить"),QString::fromUtf8("перекрасить"),"OK");
kuznecObj->ColorUncolor();
};

void GrasshopperPult::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setBrush(QColor("#7EB676"));
    p.drawRect(QRect(0,0,width(),height()));
    p.end();
    event->accept();
}
