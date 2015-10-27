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
        DrawNavigator( QWidget * parent = 0 );
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
            netStepXS->setMaximum(100*(1/Zoom));
            netStepXS->setMinimum(5*(1/Zoom));
            netStepYS->setMaximum(100*(1/Zoom));
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
        void setDraw(DrawModule* draw);
           
        
    public slots:
        void XvalueChange(double value);
        void YvalueChange(double value);
    
    private:
        double Zoom;
        QGraphicsScene* myScene;
        QGraphicsLineItem * mainLineX,*mainLineY;
        QGraphicsTextItem * zoomText,*zoomLab,*netLab;
        QDoubleSpinBox* netStepXS;
        QDoubleSpinBox* netStepYS;
        DrawModule* DRAW;
        QToolButton *zoomUp,*zoomDown,*zoomNormal,*zoomFullDraw;

        
    };
    
    class DrawView
    : public QGraphicsView
    {
    public:
        DrawView( QWidget * parent = 0 ){c_scale=1;pressed=false;press_pos=QPoint();firstResize=true;};
        void setDraw(DrawModule* draw){DRAW=draw;};
        double zoom()const
        {return c_scale;};
        void setZoom(double zoom);
        void setNet();//RESIZE NET
        void forceRedraw()
        {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() +1);
            horizontalScrollBar()->setValue(horizontalScrollBar()->value()-1);
        }
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
        bool firstResize;
 
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
            QGraphicsLineItem* line=addLine(lineF);
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
            for(int i=0;i<texts.count();i++)
                removeItem(texts.at(i));
            texts.clear();
            
        }
        bool isLineAt(const QPointF &pos,qreal radius);
        qreal drawText(const QString &Text, qreal widthChar,QPointF from,QColor color);//Returns offset of pen.
        QRectF getRect();
        int saveToFile(const QString& p_FileName);
        int loadFromFile(const QString& p_FileName);
    protected:
       // void resizeEvent ( QResizeEvent * event );
    private:
        bool isUserLine(QGraphicsItem*);//Return true if item is user item;
        QList<QGraphicsLineItem*> lines;
        QList<QGraphicsLineItem*> Netlines;
        QList<QGraphicsLineItem*> linesDubl;
        QList<QGraphicsSimpleTextItem*> texts;
        DrawModule* DRAW;
        
       
    
    }; 
class DrawModule
    : public DrawModuleBase
{
    Q_OBJECT
public /* methods */:
    DrawModule(ExtensionSystem::KPlugin * parent);
    inline void terminateEvaluation() {}
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
        mPen->setScale(factor);
        qDebug()<<"PenScale"<<factor<<"mPen->scale"<<mPen->scale();
        mutex.unlock();
    }
    static ExtensionSystem::SettingsPtr DrawSettings();
    QColor axisColor()
    {
        return QColor(DrawSettings()->value("AxisColor","blue").toString());
    }
    QString initialize(const QStringList &configurationParameters, const ExtensionSystem::CommandLine &runtimeParameters);
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
    bool runIsLineAtCircle(const qreal x, const qreal y, const qreal radius);
    void zoomFullDraw();
    
    void drawNet();
    
    void zoomIn();
    void zoomOut();
    void zoomNorm();
    
    void showNavigator(bool state);
    void openFile();
    void saveFile();


    /* ========= CLASS PRIVATE ========= */
private:
    void createGui();
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
    QToolButton *showToolsBut;
    QDir curDir;



};
        

} // namespace ActorDraw

#endif // DRAWMODULE_H
