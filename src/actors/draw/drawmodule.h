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
        void drawNet(double startx,double endx,double starty,double endy,QColor color,double step); 
        void setDraw(DrawModule* draw){DRAW=draw;};
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
    double NetStep() const
    {
        return netStep;
    }
    void setNetStep(double step)
    {
        netStep=step;
    }
    double zoom()
    {
        return CurView->zoom();
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
    double netStep;
    QColor netColor;
    bool autoNet;
    




};
        

} // namespace ActorDraw

#endif // DRAWMODULE_H
