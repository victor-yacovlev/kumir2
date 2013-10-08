/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents.
You should change it corresponding to functionality.
*/

#ifndef DRAWMODULE_H
#define DRAWMODULE_H

// Base class include
#include "drawmodulebase.h"

// Kumir includes
#include "extensionsystem/kplugin.h"

// Qt includes
#include <QtCore>
#include <QtGui>

namespace ActorDraw {

    class DrawModule;
    
    class DrawNavigator : public QGraphicsView
    {
        Q_OBJECT
    public:
        DrawNavigator( QWidget * parent = 0 ){
            myScene=new QGraphicsScene(this);
            this->setScene(myScene);
            zoomText=myScene->addText("Test");
            QBrush curBackground=QBrush(QColor("lightgreen"));
            
            myScene->setBackgroundBrush (curBackground);
            mainLineX=myScene->addLine(5,5,5,25);
            setSceneRect(0,0,140,190);
            netStepXS=new QDoubleSpinBox(this);
            netStepYS=new QDoubleSpinBox(this);
            netStepXS->move(15,10);
            netStepYS->move(5,mainLineX->line().y2()+15);
            zoomText->setPos(0,netStepYS->pos ().y()+40);
            mainLineY = myScene->addLine(mainLineX->line().x2(),
                                        mainLineX->line().y2(),
                                        mainLineX->line().x2()+10,
                                        mainLineX->line().y2());
            //centerOn(70,70);
            netStepXS->setDecimals(4);
            netStepYS->setDecimals(4);
            netStepXS->setMinimum(0.0001);
            netStepYS->setMinimum(0.0001);
            netStepXS->setMaximum(1000);
            netStepYS->setMaximum(1000);            
            connect(netStepXS,SIGNAL(valueChanged(double)),this,SLOT(XvalueChange(double)));
            connect(netStepYS,SIGNAL(valueChanged(double)),this,SLOT(YvalueChange(double)));
            };
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
            zoomText->setPlainText(QString::number(1/Zoom));
            mainLineX->setLine(5,5,5,5+netStepX*Zoom);
            mainLineY->setLine(mainLineX->line().x2(),
                               mainLineX->line().y2(),
                               mainLineX->line().x2()+netStepY*Zoom,
                               mainLineX->line().y2());
            netStepYS->move(mainLineY->line().x1()+netStepY*Zoom/2,mainLineX->line().y2()+10);
            netStepXS->move(15,mainLineX->line().x2()-5+netStepX*Zoom/2);
            netStepXS->setValue(netStepX);
            netStepYS->setValue(netStepY);
            update();
        }
        void setDraw(DrawModule* draw){DRAW=draw;};
    public slots:
        void XvalueChange(double value);
        void YvalueChange(double value);
    private:
        double Zoom;
        QGraphicsScene* myScene;
        QGraphicsLineItem * mainLineX,*mainLineY;
        QGraphicsTextItem * zoomText;
        QDoubleSpinBox* netStepXS;
        QDoubleSpinBox* netStepYS;
        DrawModule* DRAW;
        
    };
    
    class DrawView
    : public QGraphicsView
    {
    public:
        DrawView( QWidget * parent = 0 ){c_scale=1;pressed=false;press_pos=QPoint();};
        void setDraw(DrawModule* draw){DRAW=draw;};
        double zoom()const
        {return c_scale;};
        void setZoom(double zoom);
    protected:
       // void scrollContentsBy ( int dx, int dy );
        void resizeEvent ( QResizeEvent * event );
        void wheelEvent ( QWheelEvent * event );
        void mousePressEvent ( QMouseEvent * event );
        void mouseReleaseEvent ( QMouseEvent * event );
        void mouseMoveEvent ( QMouseEvent * event ); 
    private:
        DrawModule* DRAW;
        double c_scale;
        bool pressed;
        QPoint press_pos;
    };    
    class DrawScene
    : public QGraphicsScene
    {
    public:
        DrawScene ( QObject * parent = 0 ){};
        void drawNet(double startx,double endx,double starty,double endy,QColor color,const double step,const double stepY); 
        void setDraw(DrawModule* draw){DRAW=draw;};
        void addDrawLine(QLineF lineF,QColor color)
        {
            QGraphicsLineItem* line=addLine(lineF);//CRASH TUT
            line->setPen(QPen(QColor(color)));
            line->setZValue(90);
            lines.append(line); 
            
            //qDebug()<<"Lines count:"<<lines.count();
        }
        void reset()
        {
            for(int i=0;i<lines.count();i++)
                removeItem(lines.at(i));
            lines.clear();
        }
    protected:
       // void resizeEvent ( QResizeEvent * event );
    private:
        QList<QGraphicsLineItem*> lines;
        QList<QGraphicsLineItem*> Netlines;
        QList<QGraphicsLineItem*> linesDubl;
        DrawModule* DRAW;
       
    
    }; 
class DrawModule
    : public DrawModuleBase
{
    Q_OBJECT
public /* methods */:
    DrawModule(ExtensionSystem::KPlugin * parent);
    static QList<ExtensionSystem::CommandLineParameter> acceptableCommandLineParameters();
    QWidget* mainWidget() const;
    QWidget* pultWidget() const;
    bool isAutoNet() const
    {
        return autoNet;
    }
    double NetStepX() const
    {
        return netStepX;
    }
    void setNetStepX(double step)
    {
        netStepX=step;
    }
    double NetStepY() const
    {
        return netStepY;
    }
    void setNetStepY(double step)
    {
        netStepY=step;
    }
    double zoom()
    {
        return CurView->zoom();
    }
    
    QGraphicsPolygonItem* Pen()
    {
        return mPen;
    }
    void scalePen(double factor)
    {
        mutex.lock();
        mPen->scale(factor,factor);
        mutex.unlock();
    }
public slots:
    void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current);
    void loadActorData(QIODevice * source);
    void reloadSettings(ExtensionSystem::SettingsPtr settings, const QStringList & keys);
    void reset();
    void setAnimationEnabled(bool enabled);
    void runSetupPen();
    void runReleasePen();
    void runSetPenColor(const Color& color);
    void runMoveTo(const qreal x, const qreal y);
    void runMoveBy(const qreal dX, const qreal dY);
    void runAddCaption(const qreal width, const QString& text);
    
    
    void drawNet();



    /* ========= CLASS PRIVATE ========= */
private:
    void CreatePen(void);
    DrawScene* CurScene;
    DrawView* CurView;
    QGraphicsPolygonItem* mPen;
    double netStepX,netStepY;
    QColor netColor;
    bool autoNet;
    bool penIsDrawing;
    Color penColor;
    QMutex mutex;
    DrawNavigator* navigator;




};
        

} // namespace ActorDraw

#endif // DRAWMODULE_H
