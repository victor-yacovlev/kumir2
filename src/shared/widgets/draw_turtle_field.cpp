//
//  draw_turtle_field.cpp
//  Kumir2
//
//  Created by Denis Khachko on 15.12.15.
//
//
#include <QWidget>
#include "draw_turtle_field.h"



DrawNavigator::DrawNavigator( QWidget * parent ){
    myScene=new QGraphicsScene(this);
    this->setScene(myScene);
    zoomText=myScene->addText("Test");
    QBrush curBackground=QBrush(QColor("lightblue"));
    
    myScene->setBackgroundBrush (curBackground);
    netLab=myScene->addText(trUtf8("Сетка:"));
    zoomLab=myScene->addText(trUtf8("Масштаб:"));
    
    //  mainLineX=myScene->addLine(5,netLab->pos().y()+10,5,25);
    // mainLineX->setVisible(false);
    setSceneRect(0,0,110,170);
    
    netStepXS=new QDoubleSpinBox(this);
    netStepXS->setLocale(QLocale(QLocale::C));
    netStepYS=new QDoubleSpinBox(this);
    isAutonet=new QCheckBox(this);
    isAnLabel=new QLabel(this);
    isAutonet->setChecked(true);
    isAnLabel->setText(trUtf8(" авто"));
    netStepXS->move(15,netLab->pos ().y()+25);
    isAutonet->move(15,netStepXS->pos ().y()+25);
    isAnLabel->move(isAutonet->pos ().x()+15, isAutonet->pos ().y());
    //  netStepYS->move(5,mainLineX->line().y2()+15);
    zoomLab->setPos(0,netStepXS->pos ().y()+40);
    zoomText->setPos(0,netStepXS->pos ().y()+55);
    // mainLineY = myScene->addLine(mainLineX->line().x2(),
    //  mainLineX->line().y2(),
    //  mainLineX->line().x2()+10,
    //  mainLineX->line().y2());
    //centerOn(70,70);
    netStepXS->setDecimals(4);
    netStepYS->setDecimals(4);
    netStepXS->setMinimum(0.0001);
    netStepYS->setMinimum(0.0001);
    netStepXS->setMaximum(1000);
    netStepYS->setMaximum(1000);
    connect(netStepXS,SIGNAL(valueChanged(double)),this,SLOT(XvalueChange(double)));
    connect(netStepYS,SIGNAL(valueChanged(double)),this,SLOT(YvalueChange(double)));
    connect(isAutonet,SIGNAL(stateChanged(int)),this,SLOT(autoNet(int)));
    netStepYS->hide();
    zoomUp=new QToolButton(this);
    zoomDown=new QToolButton(this);
    zoomNormal=new QToolButton(this);
    
    zoomUp->setText("+");
    zoomNormal->setText("1:1");
    zoomDown->setText(" - ");
    
    zoomUp->move(zoomText->pos().x()+10,zoomText->pos().y()+35 );
    zoomNormal->move(zoomText->pos().x()+zoomUp->height(),zoomText->pos().y()+35 );
    zoomDown->move(zoomNormal->pos().x()+zoomNormal->height(),zoomText->pos().y()+35 );
    
    
    zoomFullDraw=new QToolButton(this);
    zoomFullDraw->setText(trUtf8("Весь чертеж"));
    zoomFullDraw->move(zoomUp->pos().x(),zoomDown->pos().y()+zoomDown->height() );
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    
};


void DrawNavigator::XvalueChange(double value)
{
    emit netStepChange(value);
    netStepYS->setSingleStep(value/10);
     netStepXS->setSingleStep(Zoom/500);
    qDebug()<<"Zoom"<<Zoom<<" Step"<<netStepXS->singleStep();
    netStepYS->setValue(value);
    
    
}
void DrawNavigator::YvalueChange(double value)
{
   // double oldValue=DRAW->NetStepX();
   // DRAW->setNetStepX(value);
    qDebug()<<value;
   // if(oldValue!=value)DRAW->drawNet();
   // if((int)value==1)netStepXS->setSingleStep(0.5);
   // netStepXS->setSingleStep(value/10);
    
}

void DrawNavigator::autoNet(int state)
{
    emit autoNetChange(isAutonet->isChecked());

}


