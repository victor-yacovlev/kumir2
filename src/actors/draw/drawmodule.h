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
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>

class DrawNavigator;
namespace ActorDraw {
    

    class DrawModule;
    

    
    class DrawView
    : public QGraphicsView
    {
    public:
        DrawView( QWidget * parent = 0 ){c_scale=1;pressed=false;press_pos=QPoint();firstResize=true;
            net=true;smallNetLabel=new QLabel(this);smallNetLabel->hide(); smallNetLabel->setText(trUtf8("Слишком мелкая сетка"));};
        void setDraw(DrawModule* draw){DRAW=draw;};
        double zoom()const
        {return c_scale;};
        void setZoom(double zoom);
        void setNet();//RESIZE NET
        bool isNet() const
        {
            return net;
            
        }
        void forceRedraw()
        {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() +1);
            horizontalScrollBar()->setValue(horizontalScrollBar()->value()-1);
            
           // verticalScrollBar()->setValue(horizontalScrollBar()->value() +1);
          //  verticalScrollBar()->setValue(horizontalScrollBar()->value()-1);
           
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
        bool net;
        QPoint press_pos;
        bool firstResize;
        double lastStep;
        QLabel* smallNetLabel;
 
    };    
    class DrawScene
    : public QGraphicsScene
    {
    public:
        DrawScene ( QObject * parent = 0 ){};
        void drawNet(double startx,double endx,double starty,double endy,QColor color,const double step,const double stepY,bool net,qreal nw,qreal aw);
        void setDraw(DrawModule* draw){DRAW=draw;};
        void addDrawLine(QLineF lineF,QColor color,qreal width)
        {
            if(lineF.length()==0)return;
            QGraphicsLineItem* line=addLine(lineF);
            QPen mp=QPen(QColor(color));
            mp.setWidthF(width);
            mp.setCosmetic(true);
            line->setPen(mp);
            line->setZValue(90);
            lines.append(line); 
            
            
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
        void DestroyNet();
        void drawOnlyAxis(double startx ,double endx,double starty,double endy,qreal aw);
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
        QList<QGraphicsLineItem*> linesDubl;//Базовый чертеж
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
    // static DrawModule * self;
   // static ExtensionSystem::SettingsPtr MySettings();
    bool isAutoNet() const
    {
        return autoNet;
    }
    void setAutoNet(bool state)
    {
        autoNet=state;
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
    DrawView * getCurView()
    {
        return CurView;
    }
    static ExtensionSystem::SettingsPtr DrawSettings();
    QColor axisColor()
    {
        return QColor(DrawSettings()->value("AxisColor","#999900").toString());
    }
    void redrawPicture()
    {
     
            CurView->setViewportUpdateMode (QGraphicsView::FullViewportUpdate);
            CurView->forceRedraw();
            CurScene->update(CurScene->sceneRect());
            CurView->repaint();
            CurView->viewport()->update();
            CurView->setViewportUpdateMode (QGraphicsView::NoViewportUpdate);
       
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
    void autoNetChange(bool value);
    void netStepChange(double value);
    void zoomIn();
    void zoomOut();
    void zoomNorm();
    
    void showNavigator(bool state);
    void openFile();
    void saveFile();
    void redraw();


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
    bool animate;
    QTimer *redrawTimer;


};
        

} // namespace ActorDraw

#endif // DRAWMODULE_H
