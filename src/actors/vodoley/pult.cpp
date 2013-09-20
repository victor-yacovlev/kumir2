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


OvenTimer::OvenTimer(QWidget *parent)
    : QWidget(parent)
{
    // finishTime = QDateTime::currentDateTime();
    //    updateTimer = new QTimer(this);
    // connect(updateTimer, SIGNAL(timeout()), this, SLOT(update()));

    // finishTimer = new QTimer(this);
    // finishTimer->setSingleShot(true);
    //  connect(finishTimer, SIGNAL(timeout()), this, SIGNAL(timeout()));
    //   connect(finishTimer, SIGNAL(timeout()), updateTimer, SLOT(stop()));
    gradValue=45;
    QFont font;
    font.setPointSize(8);
    setFont(font);
}

void OvenTimer::setDuration(int secs)
{
    gradValue=gradValue+secs;
    if(gradValue>360)gradValue=gradValue-360;
    if(gradValue<0)gradValue=360-gradValue;
    emit angChange(gradValue);
    qDebug()<<"Set "<<secs;
}
int OvenTimer::duration() const
{    
    return gradValue;
}
void OvenTimer::setValue(int value) 
{
    gradValue=value;
    update();
}
void OvenTimer::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mouseFlag=true;
    QPointF point = event->pos() - rect().center();
    old_mouse_pos= point;
    qDebug()<<"Mouse pos" << point;
    update();
}
void OvenTimer:: mouseMoveEvent ( QMouseEvent * event )
{
    Q_UNUSED(event);
    if(!mouseFlag)return;
    QPointF point = event->pos() - rect().center();
    QPointF delta=old_mouse_pos-point;
    qDebug()<<"Mouse Delta"<<old_mouse_pos-point<<" x coord"<<point.x();
    if(point.x()>0)
        setDuration(-delta.y());else setDuration(delta.y());

    old_mouse_pos=point;
    update();
}
void OvenTimer::mouseReleaseEvent ( QMouseEvent * event )
{
    Q_UNUSED(event);
    mouseFlag=false;
}
void OvenTimer::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int side = qMin(width(), height());

    painter.setViewport((width() - side) / 2, (height() - side) / 2,
                        side, side);
    painter.setWindow(-50, -50, 100, 100);

    draw(&painter);
}
void OvenTimer::draw(QPainter *painter)
{
    static const int triangle[3][2] = {
        {-2, -49},{+2, -49},{0, -47}
    };
    QPen thickPen(palette().foreground(), 1.5);
    QPen thinPen(palette().foreground(), 0.5);
    QColor niceBlue(200, 180, 130);
    QColor sand(220,190,150);
    QColor sandLight(244,229,111);
    painter->setPen(thinPen);
    painter->setBrush(palette().foreground());
    painter->drawPolygon(QPolygon(3, &triangle[0][0]));
    QConicalGradient coneGradient(0, 0, -90.0);
    coneGradient.setColorAt(0.0, sand);
    coneGradient.setColorAt(0.2, niceBlue);
    coneGradient.setColorAt(0.5, Qt::white);
    coneGradient.setColorAt(1.0, sand);

    painter->setBrush(coneGradient);
    painter->drawEllipse(-46, -46, 92, 92);
    QRadialGradient haloGradient(0, 0, 20, 0, 0);
    haloGradient.setColorAt(0.0, sandLight);
    haloGradient.setColorAt(0.8, sand);
    haloGradient.setColorAt(0.9, Qt::white);
    haloGradient.setColorAt(1.0, Qt::black);

    painter->setPen(Qt::NoPen);
    painter->setBrush(haloGradient);
    painter->drawEllipse(-20, -20, 40, 40);
    QLinearGradient knobGradient(-7, -25, 7, -25);
    knobGradient.setColorAt(0.0, Qt::black);
    knobGradient.setColorAt(0.2, niceBlue);
    knobGradient.setColorAt(0.3, sandLight);
    knobGradient.setColorAt(0.8, Qt::white);
    knobGradient.setColorAt(1.0, Qt::black);

    painter->rotate(duration() );
    qDebug()<<"Duration "<<duration();
    painter->setBrush(knobGradient);
    painter->setPen(thinPen);
    painter->drawRoundRect(-7, -25, 14, 50, 99, 49);

    for (int i = 0; i <= 359; ++i) {
        if (i % 60 == 0) {
            painter->setPen(thickPen);
            painter->drawLine(0, -41, 0, -44);
            painter->drawText(-15, -41, 30, 30,
                              Qt::AlignHCenter | Qt::AlignTop,
                              QString::number(i));
        } else {
            if (i % 5 == 0){
                painter->setPen(thinPen);
                painter->drawLine(0, -42, 0, -44);};
        }
        painter->rotate(-DegreesPerMinute);
    }
}



linkLight::linkLight ( QWidget* parent) :
		QWidget(parent)
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



MainButton::MainButton ( QWidget* parent) :
		QWidget(parent)
{
	direction=UP;
	posX=1;
	posY=1;
	buttonImageUp.load(":/icons/71_71grMet.png");
	buttonImageDown.load(":/icons/71_71grMet_d.png");
	downFlag=false;
	Parent=parent;
	int mid=buttonImageUp.width()/2;
	Q_UNUSED(mid);
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
}

void MainButton::paintEvent ( QPaintEvent * event )
{
    Q_UNUSED(event);
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
			painter.drawRect(3,3,75,55);
		}
		
	}
	else 	{
		painter.drawImage(QPoint(posX,posY),buttonImageDown);
		if(icon){
			//buttonIcon.invertPixels();
			painter.drawImage(3,3,buttonIcon);
			//buttonIcon.invertPixels();
		}
		QPen whitePen(QColor(170,170,170));
		whitePen.setWidth(3);
		painter.setPen(whitePen);
		//drawAddons(&painter);
		
	}
}

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
	}

}

bool MainButton::loadIcon(QString iconFile)
{
	icon=true;
	return buttonIcon.load(iconFile);
}

void MainButton::mousePressEvent ( QMouseEvent * event )
{
	Q_UNUSED(event);
	qWarning("MousePress");
	emit pressed();
	if(checkable)checked=!checked;
	downFlag=true;
	repaint();
}

void MainButton::mouseReleaseEvent ( QMouseEvent * event )
{
	Q_UNUSED(event);
	if(checkable){if(!checked)downFlag=false;}else downFlag=false;
	if(mouseOver)emit clicked();
	repaint();
}


void MainButton::enterEvent ( QEvent * event )
{
    Q_UNUSED(event);
    if(!mouseOver){mouseOver=true;repaint();};
}

void MainButton::leaveEvent ( QEvent * event )
{
    Q_UNUSED(event);
    if(mouseOver){mouseOver=false;repaint();}
}





loggerButton::loggerButton ( QWidget* parent) :
		QWidget(parent)
{
	isUpArrow=false;
	posX=1;
	posY=1;
	buttonImageUp.load(qApp->property("sharePath").toString()+"/actors/vodoley/butt_v.png");
	buttonImageDown.load(qApp->property("sharePath").toString()+"/actors/vodoley/buttd_v.png");
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
	}
}


void loggerButton::mousePressEvent ( QMouseEvent * event )
{
	Q_UNUSED(event);
	qWarning("MousePress");
	emit pressed();
	downFlag=true;
	repaint();
}
void loggerButton::mouseReleaseEvent ( QMouseEvent * event )
{
	Q_UNUSED(event);
	downFlag=false;
	repaint();
}





pultLogger::pultLogger ( QWidget* parent) :
		QWidget(parent)
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

}

pultLogger::~pultLogger ()
{
    //delete mainFrame;
}

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
}

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
}

void pultLogger::appendText(QString kumCommand,QString text,QString replay)
{
	while(pos>H-68)downBtnPressed();
	lines.append(logLine(kumCommand,text,replay,dummyFrame,respFrame,pos));
	pos=pos+TEXT_STEP;
}


void pultLogger::upBtnPressed()
{

    if(lines.count()==0)return;
    if(lines.first().pos()>2)return;

    for(int i=0;i<lines.count();i++)lines[i].moveDown();
    pos=pos+SCROLL_STEP;
    qWarning("TEXT MOVE");
}

void pultLogger::downBtnPressed()
{
    if(pos<14)return;
    for(int i=0;i<lines.count();i++)lines[i].moveUp();
    pos=pos-SCROLL_STEP;
}

void pultLogger::ClearLog()
{
    for(int i=0;i<lines.count();i++)lines[i].removeLabels();
    lines.clear();
    pos=4;

}

void pultLogger::CopyLog()
{
    if(lines.count()==0)return;
    QClipboard * cp=QApplication::clipboard ();
    QString text;
    for(int i=0;i<lines.count();i++)if(!lines[i].KumCommand().isEmpty())text+=lines[i].KumCommand()+"\n";
    cp->setText(text);
}

VodoleyPult::VodoleyPult ( QWidget* parent, Qt::WFlags fl )
	: QWidget ( parent, fl ), Ui::VodoleyPult()
{
	autoClose=false;
	setupUi ( this );
	libMode=true;
	//setWindowFlags(Qt::Dialog);

	Logger=new pultLogger(this);


    this->setMinimumSize(250,450);
	Logger->setSizes(164,150);
	Logger->Move(40,5);	
	greenLight=new linkLight(this);
	greenLight->move(15,29);
	greenLight->resize(12,104);

	UpB->show();
	BtoC=new MainButton(this);
	BtoC->move(UpB->pos());
	BtoC->setGeometry(UpB->geometry());
  
	BtoC->loadIcon(qApp->property("sharePath").toString()+"/actors/vodoley/b2c.png");

	DownB->hide();
	buttBack=new MainButton(this);
	buttBack->move(DownB->pos());
	buttBack->setGeometry(DownB->geometry());
	qDebug()<<qApp->property("sharePath").toString()+"/actors/vodoley/bout.png"<<buttBack->loadIcon(qApp->property("sharePath").toString()+"/actors/vodoley/bout.png");

	AtoCb->hide();
	AtoC=new MainButton(this);
	AtoC->setGeometry(AtoCb->geometry());
	AtoC->loadIcon(qApp->property("sharePath").toString()+"/actors/vodoley/a2c.png");

	AoutB->hide();
	turnRight=new MainButton(this);
	turnRight->setGeometry(AoutB->geometry());
	turnRight->loadIcon(qApp->property("sharePath").toString()+"/actors/vodoley/aout.png");
	CoutB->hide();
	Coutb=new MainButton(this);
	Coutb->setGeometry(CoutB->geometry());
	Coutb->loadIcon(qApp->property("sharePath").toString()+"/actors/vodoley/cout.png");

	//        StenaB->hide();


	//        SvobodnoB->hide();
	askFree=new MainButton(this);
	//        askFree->move(SvobodnoB->pos());
	askFree->setCheckable(true);
	askFree->setText(trUtf8(" "));
	askFree->loadIcon(qApp->property("sharePath").toString()+"/actors/vodoley/svobodno.png");
	askFree->setCheckable(true);
	askFree->hide();



	AtoBb->hide();
	AtoB=new MainButton(this);
	AtoB->setGeometry(AtoBb->geometry());
	AtoB->setText(trUtf8(" "));
	if(!AtoB->loadIcon(qApp->property("sharePath").toString()+"/actors/vodoley/a2b.png"))qWarning("Image not loaded!");
	
	CtoBb->hide();
	CtoB=new MainButton(this);
	CtoB->setGeometry(CtoBb->geometry());
	CtoB->setText(trUtf8(" "));
	if(!CtoB->loadIcon(qApp->property("sharePath").toString()+"/actors/vodoley/c2b.png"))qWarning("Image not loaded!");


	CtoAb->hide();
	CtoA=new MainButton(this);
	CtoA->setGeometry(CtoAb->geometry());
	CtoA->setText(trUtf8(" "));
	if(!CtoA->loadIcon(qApp->property("sharePath").toString()+"/actors/vodoley/c2a.png"))qWarning("Image not loaded!");

	TempB->hide();
	buttTemp=new MainButton(this);
	buttTemp->setGeometry(TempB->geometry());
    
	buttTemp->setText(" ");
	buttTemp->loadIcon(qApp->property("sharePath").toString()+"/actors/vodoley/b2a.png");

	QIcon toKumirIco(qApp->property("sharePath").toString()+"/actors/vodoley/edit-copy.png");
	toKumir->setIcon(toKumirIco);
	ClearLog->setIcon(QIcon(qApp->property("sharePath").toString()+"/actors/vodoley/edit-clear-list..png"));


	FillA->hide();
	aFill=new MainButton(this);
	//        askFree->move(SvobodnoB->pos());
	//aFill->setCheckable(false);
	aFill->setText(trUtf8(" "));
	aFill->loadIcon(qApp->property("sharePath").toString()+"/actors/vodoley/afill.png");
	aFill->setGeometry(FillA->geometry());

	FillB->hide();
	bFill=new MainButton(this);
	bFill->setText(trUtf8(" "));
	bFill->loadIcon(qApp->property("sharePath").toString()+"/actors/vodoley/bfill.png");
	bFill->setGeometry(FillB->geometry());

	FillC->hide();
	cFill=new MainButton(this);
	cFill->setText(trUtf8(" "));
	cFill->loadIcon(qApp->property("sharePath").toString()+"/actors/vodoley/cfill.png");
	cFill->setGeometry(FillC->geometry());

	//	CenterB->setIcon(QIcon(":/icons/robo_field.png"));
	connect(BtoC,SIGNAL(clicked()),this,SLOT(Up()));

	//connect(gradVal,SIGNAL(valueChanged(int)),GradInput,SLOT(setValue(int)));
	connect(buttBack,SIGNAL(clicked()),this,SLOT(BOutS()));
	connect(AtoC,SIGNAL(clicked()),this,SLOT(AtoCS()));
	connect(turnRight,SIGNAL(clicked()),this,SLOT(AOutS()));
	connect(Coutb,SIGNAL(clicked()),this,SLOT(COutS()));

	connect(AtoB,SIGNAL(clicked()),this,SLOT(AtoBS()));
	connect(CtoB,SIGNAL(clicked()),this,SLOT(CtoBS()));
	connect(CtoA,SIGNAL(clicked()),this,SLOT(CtoAS()));

	connect(buttTemp,SIGNAL(clicked()),this,SLOT(TempS()));

	//connect(askStena,SIGNAL(clicked()),this,SLOT(SwStena()));
	connect(askFree,SIGNAL(clicked()),this,SLOT(SwSvobodno()));
	//connect(CenterB,SIGNAL(clicked()),this,SLOT(CenterButton()));

	connect(ClearLog,SIGNAL(clicked()),Logger,SLOT(ClearLog()));
	connect(ClearLog,SIGNAL(clicked()),this,SLOT(resetVodoley()));

	connect(toKumir,SIGNAL(clicked()),this,SLOT(logToKumir()));
	//	connect(CopyLog,SIGNAL(clicked()),Logger,SLOT(CopyLog()));
	connect(aFill,SIGNAL(clicked()),this,SLOT(FillAs()));
	connect(bFill,SIGNAL(clicked()),this,SLOT(FillBs()));
	connect(cFill,SIGNAL(clicked()),this,SLOT(FillCs()));
    this->setMinimumSize(QSize(250,400));

	link=true;
}


void VodoleyPult::noLink(){
	link=false;
	greenLight->setLink(link);greenLight->repaint();qWarning("NoLINK");
	ClearLog->setEnabled(false);toKumir->setEnabled(false);}
void VodoleyPult::LinkOK(){qDebug()<<"LinkOK";link=true;greenLight->setLink(link);greenLight->repaint();ClearLog->setEnabled(true);toKumir->setEnabled(true);};

void VodoleyPult::Up()
{
    if(!greenLight->link())return;
    Logger->appendText(trUtf8("перелей из B в C"),trUtf8("перелей из B в C"),"OK");
    VodoleyObj->MoveFromTo(1,2);
}

void VodoleyPult::BOutS()
{
    if(!greenLight->link())return;
    Logger->appendText(trUtf8("вылей B"),trUtf8("вылей B"),"OK");
    VodoleyObj->MoveFromTo(1,3);
}

void VodoleyPult::COutS()
{
    if(!greenLight->link())return;
    Logger->appendText(trUtf8("вылей C"),trUtf8("вылей C"),"OK");
    VodoleyObj->MoveFromTo(2,3);
}

void VodoleyPult::AOutS()
{
    if(!greenLight->link())return;

    Logger->appendText(trUtf8("вылей A"),trUtf8("вылей A"),"OK");
    VodoleyObj->MoveFromTo(0,3);

}

void VodoleyPult::AtoCS()
{
    if(!greenLight->link())return;

    Logger->appendText(trUtf8("перелей из A в C"),trUtf8("перелей из A в C"),"OK");
    VodoleyObj->MoveFromTo(0,2);
}

void VodoleyPult::FillAs()
{
    if(!greenLight->link())return;

    Logger->appendText(trUtf8("наполни A"),trUtf8("наполни А"),"OK");
    VodoleyObj->FillA();
}

void VodoleyPult::FillBs()
{
    if(!greenLight->link())return;

    Logger->appendText(trUtf8("наполни B"),trUtf8("наполни B"),"OK");
    VodoleyObj->FillB();
}

void VodoleyPult::FillCs()
{
    if(!greenLight->link())return;

    Logger->appendText(trUtf8("наполни C"),trUtf8("наполни C"),"OK");
    VodoleyObj->FillC();
}

void VodoleyPult::TempS()
{
    if(!greenLight->link())return;

    Logger->appendText(trUtf8("перелей из B в A"),trUtf8("перелей из B в A"),"OK");
    VodoleyObj->MoveFromTo(1,0);
}

void VodoleyPult::AtoBS()
{
    if(!greenLight->link())return;

    Logger->appendText(trUtf8("перелей из A в B"),trUtf8("перелей из A в B"),"OK");
    VodoleyObj->MoveFromTo(0,1);
}



void VodoleyPult::CtoAS()
{
    if(!greenLight->link())return;

    Logger->appendText(trUtf8("перелей из C в A"),trUtf8("перелей из C в A"),"OK");
    VodoleyObj->MoveFromTo(2,0);
}


void VodoleyPult::CtoBS()
{
    if(!greenLight->link())return;

    Logger->appendText(trUtf8("перелей из C в B"),trUtf8("перелей из C в B"),"OK");
    VodoleyObj->MoveFromTo(2,1);
}


void VodoleyPult::resetVodoley()
{

    VodoleyObj->reset();

}

void VodoleyPult::newClient(QString message)
{
    label->setText(QString::fromUtf8("Подключился ")+message);
    toKumir->setEnabled(true);
}

void VodoleyPult::clientDisconnect()
{
    label->setText(QString::fromUtf8("Клиент отключился"));

    LinkOK();
   // toKumir->setEnabled(false);

}


void VodoleyPult::logToKumir()
{
	emit sendText(Logger->log());

}



//void VodoleyPult::closeEvent ( QCloseEvent * event )
//{
//	if((libMode)||(autoClose))
//	{
//		close();
//		event->accept();
//		return;
//	}
//	int ret = QMessageBox::warning(this, QString::fromUtf8("Водолей"),
//								   QString::fromUtf8("Закрыть исполнитель Водолей?"),
//								   QMessageBox::Yes | QMessageBox::Default,
//								   QMessageBox::No,
//								   QMessageBox::Cancel | QMessageBox::Escape);
//	if (ret == QMessageBox::Yes) {
//		VodoleyObj->AutoClose();
//		VodoleyObj->close();
//		event->accept();
//	} else {
//		event->ignore();
//	}
//}


void VodoleyPult::lockCButtons()
{
	CtoB->setEnabled(false);
	CtoA->setEnabled(false);
	AtoC->setEnabled(false);
	BtoC->setEnabled(false);
	cFill->setEnabled(false);
	Coutb->setEnabled(false);
}

void VodoleyPult::UnLockCButtons()
{
	CtoB->setEnabled(true);
	CtoA->setEnabled(true);
	AtoC->setEnabled(true);
	BtoC->setEnabled(true);
	cFill->setEnabled(true);
	Coutb->setEnabled(true);
}

void VodoleyPult::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#32BAC3"));
    p.drawRect(0,0,width(),height());
    p.restore();
    QWidget::paintEvent(event);
}
