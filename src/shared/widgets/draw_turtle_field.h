//
//  draw_turtle_field.h
//  Kumir2
//
//  Created by Denis Khachko on 14.12.15.
//
//

#ifndef Kumir2_draw_turtle_field_h
#define Kumir2_draw_turtle_field_h

// Qt includes
#include <QtCore>
#include <QtGui>
#include <QDoubleSpinBox>

class DrawNavigator : public QGraphicsView
{
    Q_OBJECT
public:
    DrawNavigator( QWidget * parent  );
    void setZoom(double zoom)
    {
        Zoom=zoom;
    }
    void reDraw(const double zoom ,const double netStepX,const double netStepY)
    {
        setZoom(zoom);
        updateSelf(netStepX,netStepY);
    }
    void updateSelf(double netStepX,double netStepY)
    {
        if(50/Zoom>=1)zoomText->setPlainText("1:"+QString::number(50/Zoom));
        else zoomText->setPlainText(QString::number(Zoom/50)+":1");
        qDebug()<<"Zoom"<<Zoom;
        //double pixel_per_cell=DRAW->NetStepX()/(1/c_scale);
        
        netStepXS->setMaximum(300*(1/Zoom));
        netStepXS->setMinimum(5*(1/Zoom));
        netStepYS->setMaximum(300*(1/Zoom));
        netStepYS->setMinimum(5*(1/Zoom));
        // mainLineX->setLine(5,netLab->pos().y()+30,5,5+netStepX*Zoom+netLab->pos().y()+30);
        // mainLineY->setLine(mainLineX->line().x2(),
        //                    mainLineX->line().y2(),
        //                     mainLineX->line().x2()+netStepY*Zoom,
        //                     mainLineX->line().y2());
        // netStepYS->move(mainLineY->line().x1()+netStepY*Zoom/2,mainLineX->line().y2()+10);
        // netStepXS->move(15,mainLineX->line().x2()+15+netStepX*Zoom/2);
        netStepXS->setValue(netStepX);
        netStepYS->setValue(netStepY);
        
        //   zoomLab->setPos(5,mainLineX->line().y2()+10);
        //  zoomText->setPos(5,mainLineX->line().y2()+25);
        //  zoomUp->move(zoomText->pos().x()+5,zoomText->pos().y()+25 );
        //  zoomNormal->move(zoomText->pos().x()+27,zoomText->pos().y()+25 );
        //   zoomDown->move(zoomText->pos().x()+57,zoomText->pos().y()+25 );
        //   zoomFullDraw->move(zoomUp->pos().x(),zoomDown->pos().y()+zoomDown->height() );
        
        update();
    }
    //void setDraw(DrawModule* draw);
    public:
    QDoubleSpinBox* netStepXS;
    QDoubleSpinBox* netStepYS;
    QToolButton *zoomUp,*zoomDown,*zoomNormal,*zoomFullDraw;
    public slots:
    void XvalueChange(double value);
    void YvalueChange(double value);
    void autoNet(int state);
    signals:
    void redrawNet();
    void autoNetChange(bool value);
    void netStepChange(double value);
private:
    double Zoom;
    QGraphicsScene* myScene;
    QGraphicsLineItem * mainLineX,*mainLineY;
    QGraphicsTextItem * zoomText,*zoomLab,*netLab;

     
    QCheckBox * isAutonet;
    QLabel * isAnLabel;
    
    
};

#endif
