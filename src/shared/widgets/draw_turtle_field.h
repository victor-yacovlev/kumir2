//
//  draw_turtle_field.h
//  Kumir2
//
//  Created by Denis Khachko on 14.12.15.
//
//

#ifndef Kumir2_draw_turtle_field_h
#define Kumir2_draw_turtle_field_h
#include <QWidget>
// Qt includes
#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QDoubleSpinBox>

#ifdef WIDGETS_LIBRARY
#define WIDGETS_EXPORT Q_DECL_EXPORT
#else
#define WIDGETS_EXPORT Q_DECL_IMPORT
#endif

class WIDGETS_EXPORT DrawNavigator : public QGraphicsView
{
    Q_OBJECT
public:
    DrawNavigator( QWidget * parent =0 );
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
        if(50/Zoom>=1)zoomText->setPlainText("1:"+QString::number(50/Zoom,'f',2));
        else zoomText->setPlainText(QString::number(Zoom/50,'f',2)+":1");
        qDebug()<<"Zoom"<<Zoom;
        //double pixel_per_cell=DRAW->NetStepX()/(1/c_scale);
        if(isAutonet->isChecked()){
        netStepXS->setMaximum(300*(1/Zoom));
        netStepXS->setMinimum(5*(1/Zoom));
        netStepYS->setMaximum(300*(1/Zoom));
        netStepYS->setMinimum(5*(1/Zoom));

        netStepXS->setValue(netStepX);
        netStepYS->setValue(netStepY);
        

        netStepXS->setSingleStep(Zoom/500);
        }else
        {
            netStepXS->setMaximum(99999);
            netStepXS->setMinimum(0.00001);
          //  netStepXS->setValue(netStepX);
           // netStepYS->setValue(netStepY);
        }
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
