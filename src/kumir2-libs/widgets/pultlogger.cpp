//
//  pultlogger.cpp
//  Kumir2
//
//  Created by Denis Khachko on 22.05.14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "pultlogger.h"

#define SCROLL_STEP 10
#define RESP_PANEL 50
#define LOGGER_BUTTONS 140
#define TEXT_STEP 14
#undef TEXT
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define TEXTT 5
#define TEXT 5

logLine::logLine(QString KumCommand,
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

void logLine::moveUp()
{
    textLabel->move(textLabel->x(),textLabel->y()-SCROLL_STEP);
    respLabel->move(respLabel->x(),respLabel->y()-SCROLL_STEP);
}

void logLine::moveDown()
{
    textLabel->move(textLabel->x(),textLabel->y()+SCROLL_STEP);
    respLabel->move(respLabel->x(),respLabel->y()+SCROLL_STEP);
}

void pultLogger::appendText(QString kumCommand, QString text, QString replay)
{
    while(pos>H-68)downBtnPressed();
    lines.append(logLine(kumCommand+'\n',text,replay,dummyFrame,respFrame,pos));
    pos=pos+TEXT_STEP;
}

void MainButton::setText(QString t)
{
    text=t;direction=TEXT;
}

loggerButton::loggerButton (QDir dir, QWidget* parent):QWidget(parent)
{isUpArrow=false;
	posX=1;
	posY=1;
    buttonImageUp.load(dir.absoluteFilePath("butt.png"));
	buttonImageDown.load(dir.absoluteFilePath("buttd.png")); 
    downFlag=false;
	Parent=parent;
	int mid=buttonImageUp.width()/2;
	upArrow.append(QLine(mid,10,mid-11,15));
	upArrow.append(QLine(mid,10,mid+11,15));
	downArrow.append(QLine(mid,15,mid-11,10));
	downArrow.append(QLine(mid,15,mid+11,10));
   
};
void loggerButton::loadButtons(QDir dir)
{
    
    //    qDebug()<<"Dir"<<dir.absoluteFilePath("butt.png");
    //	  //  buttonImageUp.load("/Volumes/Untitled 1/Users/mordol/Kumir2.0/SshGitRep/BuildSecSt12/Debug/Kumir.app/Contents/Resources/actors/turtle/butt.png");
    //	buttonImageDown.load(dir.absoluteFilePath("buttd.png"));    
}
void loggerButton::paintEvent ( QPaintEvent * event )
{
    Q_UNUSED(event);
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
	Q_UNUSED(event);
	qWarning("MousePress");
	emit pressed();
	downFlag=true;
	repaint();
};
void loggerButton::mouseReleaseEvent ( QMouseEvent * event )
{
	Q_UNUSED(event);
	downFlag=false;
	repaint();
};





pultLogger::pultLogger ( QDir resDir,QWidget* parent):QWidget(parent)
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
    
    
	downButton=new loggerButton(resDir,parent);
    //  upButton->loadButtons(resDir);
	downButton->move(0,H-24);
	downButton->resize(140,24);
	downButton->show();
	upButton=new loggerButton(resDir,parent);
    //  upButton->loadButtons(resDir);
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
	qDebug()<<"upBtn geom "<<upButton->geometry();
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




linkLight::linkLight ( QWidget* parent):QWidget(parent)
{
    posX=1;
    posY=1;
    onLine=true;
    text=trUtf8("СВЯЗЬ");
};

void linkLight::paintEvent ( QPaintEvent * event )
{
    Q_UNUSED(event);
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




MainButton::MainButton (QDir dir, QWidget* parent):QWidget(parent)
{direction=UP;
    posX=1;
    posY=1;
    buttonImageUp.load(dir.absoluteFilePath("71_71grMet.png"));
    buttonImageDown.load(dir.absoluteFilePath("71_71grMet_d.png"));
    downFlag=false;
    Parent=parent;
    int mid=buttonImageUp.width()/2;
    upArrow.append(QLine(mid,30,mid-15,40));
    upArrow.append(QLine(mid,30,mid+15,40));
    downArrow.append(QLine(mid,40,mid-15,30));
    downArrow.append(QLine(mid,40,mid+15,30));
    leftArrow.append(QLine(30,mid,40,mid-15));
    leftArrow.append(QLine(30,mid,40,mid+15));
    rightArrow.append(QLine(40,mid,30,mid-15));
    rightArrow.append(QLine(40,mid,30,mid+15));
    text="";
    iconoffs=0;
    checked=false;
    checkable=false;
    mouseOver=false;
    icon=false;
    qmode=false;
    resize(71,71);
    SetQu=false;
    posQ=QPoint(10,71/2);
};

void MainButton::paintEvent ( QPaintEvent * event )
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint (QPainter::Antialiasing,true );
    QPoint m_posQ=posQ;
    if(!downFlag)
    {
		painter.drawImage(QPoint(posX,posY),buttonImageUp);
		if(icon)painter.drawImage(posX+iconoffs,posX+iconoffs,buttonIcon);
		QPen blackPen(QColor(40,40,40));
		blackPen.setWidth(3);
		painter.setPen(blackPen);
		drawAddons(&painter);
		if(mouseOver)
        {
		//	QLinearGradient grad( 1, 1, 5, 65);
		//	grad.setColorAt(0.7,QColor(200,190,222));
		//	grad.setColorAt(0.3,QColor(191,208,208));
            
		//	QBrush solidBrush(grad);
		//	painter.setBrush(solidBrush);
		//	painter.setOpacity(0.1);
			//painter.drawRect(3,3,65,65);
        };
		
    }
	else 	{
		painter.drawImage(QPoint(posX,posY),buttonImageDown);
		if(icon){
			
			painter.drawImage(posX+3+iconoffs,posY+3+iconoffs,buttonIcon);
		
        };
        m_posQ=m_posQ+QPoint(3,3);
		QPen whitePen(QColor(170,170,170));
		whitePen.setWidth(3);
		painter.setPen(whitePen);
        drawAddons(&painter);
		
    };
    if(SetQu)
    {
    QPen QLPen(QColor(170,170,170));
    if(posQ.y()>30)QLPen=QPen(QColor(163,163,163));
    if(qmode)
    {
    QLPen=QPen(QColor(230,255,230));
 
    }
    painter.setPen(QLPen);
    painter.drawText(m_posQ,"?");
    }
};

void MainButton::drawAddons(QPainter* painter)
{
    if(direction==UP)painter->drawLines(upArrow);
    if(direction==DOWN)painter->drawLines(downArrow);
    if(direction==LEFT)painter->drawLines(leftArrow);
    if(direction==RIGHT)painter->drawLines(rightArrow);
    if(direction==5)
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
{Q_UNUSED(event);
	Q_UNUSED(event);
	qWarning("MousePress");
	emit pressed();
    if(checkable)checked=!checked;
    downFlag=true;
	repaint();
};
void MainButton::mouseReleaseEvent ( QMouseEvent * event )
{Q_UNUSED(event);
	if(checkable){if(!checked)downFlag=false;}else downFlag=false;
	if(mouseOver)emit clicked();
	repaint();
};


void MainButton::enterEvent ( QEvent * event )
{Q_UNUSED(event);
    if(!mouseOver){mouseOver=true;repaint();};
    
    qWarning("mouseOnWidget");
};
void MainButton::leaveEvent ( QEvent * event )
{Q_UNUSED(event);
    if(mouseOver){mouseOver=false;repaint();qWarning("mouseOffWidget");};
};






