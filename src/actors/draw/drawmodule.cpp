/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents.
You should change it corresponding to functionality.
*/

// Self include
#include "drawmodule.h"

// Kumir includes
#include "extensionsystem/kplugin.h"
#include "extensionsystem/pluginmanager.h"

// Qt includes
#include <QtCore>
#include <QtGui>


namespace ActorDraw {

    
    ExtensionSystem::SettingsPtr DrawModule::DrawSettings()
    {
        ExtensionSystem::PluginManager * pluginManager = ExtensionSystem::PluginManager::instance();
        ExtensionSystem::KPlugin * plugin = pluginManager->loadedPlugins("ActorDraw")[0];
        return pluginManager->settingsByObject(plugin);
    }
    
    static const qreal MAX_ZOOM = 1000000;
    
    
     DrawNavigator::DrawNavigator( QWidget * parent ){
        myScene=new QGraphicsScene(this);
        this->setScene(myScene);
        zoomText=myScene->addText("Test");
        QBrush curBackground=QBrush(QColor("lightblue"));
        
        myScene->setBackgroundBrush (curBackground);
        netLab=myScene->addText(trUtf8("Сетка:"));
        zoomLab=myScene->addText(trUtf8("Масштаб:"));
        mainLineX=myScene->addLine(5,netLab->pos().y()+10,5,25);
        setSceneRect(0,0,120,190);
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
        netStepYS->hide();
        zoomUp=new QToolButton(this);
        zoomDown=new QToolButton(this);
        zoomNormal=new QToolButton(this);
         
         zoomUp->setText("+");
         zoomNormal->setText("1:1");
         zoomDown->setText(" - ");
         
         zoomUp->move(zoomText->pos().x(),zoomText->pos().y()+25 );
         zoomNormal->move(zoomText->pos().x()+25,zoomText->pos().y()+25 );
         zoomDown->move(zoomText->pos().x()+50,zoomText->pos().y()+25 );
         
         
         zoomFullDraw=new QToolButton(this);
         zoomFullDraw->setText(trUtf8("Весь чертеж"));
        zoomFullDraw->move(zoomUp->pos().x(),zoomDown->pos().y()+zoomDown->height() );
         
    };
    
    void DrawNavigator::setDraw(DrawModule* draw){
        DRAW=draw;
        connect(zoomUp,SIGNAL(pressed()),DRAW,SLOT(zoomIn()));
        connect(zoomDown,SIGNAL(pressed()),DRAW,SLOT(zoomOut()));  
        connect(zoomNormal,SIGNAL(pressed()),DRAW,SLOT(zoomNorm()));
        connect(zoomFullDraw,SIGNAL(pressed()),DRAW,SLOT(zoomFullDraw()));
    };
    void DrawNavigator::XvalueChange(double value)
    {
        double oldValue=DRAW->NetStepY();
        DRAW->setNetStepY(value);  
        qDebug()<<value;
        if(oldValue!=value && value>netStepYS->minimum() )DRAW->drawNet();
        netStepYS->setSingleStep(value/10);
        netStepYS->setValue(value);

    }
    void DrawNavigator::YvalueChange(double value)
    {
        double oldValue=DRAW->NetStepX();
        DRAW->setNetStepX(value);  
        qDebug()<<value;
        if(oldValue!=value)DRAW->drawNet();
        if((int)value==1)netStepXS->setSingleStep(0.5);
        netStepXS->setSingleStep(value/10);
        
    }
    QRectF DrawScene::getRect()
    {
        QRectF boundRect=QRect(-2,-10,10,10);
  
        for(int i=0;i<lines.count();i++)
        {

            if(lines.at(i)->line().dx()>0)
            {
                if(lines.at(i)->line().p1().x()<boundRect.left() || (i==0))
                {
                    boundRect.setLeft(lines.at(i)->line().p1().x()); 
                    
                }
                if(lines.at(i)->line().p2().x()>boundRect.right() || (i==0))
                {
                    boundRect.setRight(lines.at(i)->line().p2().x()); 
                    
                }
         
            }
            if(lines.at(i)->line().dx()<0)
            {
                if(lines.at(i)->line().p2().x()<boundRect.left() || (i==0))
                {
                    boundRect.setLeft(lines.at(i)->line().p2().x()); 
                } 
                if(lines.at(i)->line().p1().x()>boundRect.right() || (i==0))
                {
                    boundRect.setRight(lines.at(i)->line().p1().x()); 
                    
                }
             
            }
            if(lines.at(i)->line().dy()>0)
            {
                qDebug()<<"Bott"<<boundRect.bottom()<<"Top:"<<boundRect.top()<<"line p1y:"<<lines.at(i)->line().p1().y()<<"line p2y:"<<lines.at(i)->line().p2().y();
                if(-lines.at(i)->line().p1().y()>boundRect.bottom() || (i==0))
                {
                    boundRect.setBottom(-lines.at(i)->line().p1().y()); 
                }
                if(lines.at(i)->line().p2().x()>boundRect.top() || (i==0))
                {
                    boundRect.setTop(-lines.at(i)->line().p2().y()); 
                    
                }
            }
            if(lines.at(i)->line().dy()<0)
            {
                qDebug()<<"Bott"<<boundRect.bottom()<<"Top:"<<boundRect.top()<<"line p1y:"<<lines.at(i)->line().p1().y()<<"line p2y:"<<lines.at(i)->line().p2().y();
                if(lines.at(i)->line().p2().y()<boundRect.bottom())
                {
                    boundRect.setBottom(-lines.at(i)->line().p2().y()); 
                } 
                if(lines.at(i)->line().p1().y()<boundRect.top() || (i==0))
                {
                    boundRect.setTop(lines.at(i)->line().p1().y()); 
                    
                }
            }
        }
        
        if(boundRect.width()!=boundRect.height())
        {
            qreal size=fmax(boundRect.width(),boundRect.width());
            boundRect.setWidth(size);
            boundRect.setHeight(size);
            
        }
        
        return(boundRect);
    };
    
    void DrawScene::drawNet(double startx ,double endx,double starty,double endy,QColor color,double stepX,double stepY)
    {
        
        QPointF pos,tail;    
        QColor AxisColor=DRAW->axisColor();
        for ( int i = 0; i < Netlines.count(); i++)
        {
       
         delete Netlines[i];   
       }
        
        qDebug()<<"RedrawNet";
        Netlines.clear();
        int lines=qRound(startx/stepX);
        startx=lines*stepX;
        double fx1=startx-7*stepX,fx2,fy1,fy2; 

       // return;
        while (fx1 < endx+7*stepX)
		{
			fx1 = fx1 + stepX;
			fx2 = fx1;
			fy1 = starty-7*stepX;
			fy2 = endy+7*stepX;
            
			Netlines.append(addLine(fx1, fy1 , fx2, fy2 ));
			Netlines.last()->setZValue(0.5);
			Netlines.last()->setPen(QPen(color));
            if(fx1>0-1/DRAW->zoom() && fx1<0+1/DRAW->zoom())
            {
                QPen axisPen=QPen(AxisColor);
               // axisPen.setWidth(3/DRAW->zoom());
                Netlines.last()->setPen(axisPen); 
                Netlines.last()->setZValue(1);
                Netlines.append(addLine(fx1+1/DRAW->zoom(), fy1 , fx2+1/DRAW->zoom(), fy2 ));
                Netlines.last()->setZValue(1);
                Netlines.last()->setPen(axisPen); 
            }
		}
      //  Netlines.append(addLine(-1, -1 , 1, 1 ));
       // Netlines.append(addLine(1, -1 , -1, 1 ));
        lines=starty/stepY;
        starty=lines*stepY;
        fy1 = starty-7*stepY;
        
		while (fy1 < endy+7*stepY)
		{
			fy1 = fy1 + stepY;
			fy2 = fy1;
			fx1 = startx-7*stepY;
			fx2 = endx+7*stepY;
            
            Netlines.append(addLine(fx1, fy1 , fx2, fy2 ));
			Netlines.last()->setZValue(0.5);
			Netlines.last()->setPen(QPen(color));
            if(fy1>0-1/DRAW->zoom() && fy1<0+1/DRAW->zoom())
            {
                QPen axisPen=QPen(AxisColor);
               // axisPen.setWidth(6/(DRAW->zoom()*2));
               // qDebug()<<"Width"<<6/(DRAW->zoom()*2);
                Netlines.last()->setPen(axisPen);
                Netlines.last()->setZValue(1);
                Netlines.append(addLine(fx1, fy1+1/DRAW->zoom() , fx2, fy2+1/DRAW->zoom() ));
                Netlines.last()->setZValue(1);
                Netlines.last()->setPen(axisPen);  
            }
            
		}

    }
void DrawView::resizeEvent ( QResizeEvent * event )
    {
        if(firstResize)
        {
                  qDebug()<<"FirstresizeEvent";
           QPointF maxRight=mapToScene(geometry().bottomRight());
            
            centerOn(maxRight.x()/2-maxRight.x()/5,-maxRight.y()/2+maxRight.y()/4);
        }
        firstResize=false;
        qDebug()<<"resizeEvent";
       // setViewportUpdateMode (QGraphicsView::NoViewportUpdate);
       if(!pressed)  DRAW->drawNet();
        update();
       // setViewportUpdateMode (QGraphicsView::SmartViewportUpdate);
    };
  
    
    void DrawView::mousePressEvent ( QMouseEvent * event )
    {
        pressed=true;
        press_pos=event->pos();
        qDebug()<<"Mouse press"<<mapToScene(press_pos);
    
    };
    void DrawView::mouseReleaseEvent ( QMouseEvent * event )
    {
        pressed=false;
        DRAW->drawNet();
    };
    void DrawView::mouseMoveEvent ( QMouseEvent * event )
    {
        if(pressed)
        {
            setViewportUpdateMode (QGraphicsView::SmartViewportUpdate);
            QPointF delta=mapToScene(press_pos)-mapToScene(event->pos());
            QPointF center = mapToScene(viewport()->rect().center());
            centerOn(center+delta/2);
           // press_pos=mapToScene(press_pos)+delta;
           // qDebug()<<"CenterOn"<<center+delta;
            update();
        }
    }; 
    void DrawView::setZoom(double zoom)
    {
        if(zoom>MAX_ZOOM)
        {return;}
        if(zoom<1/MAX_ZOOM)
        {return;}
       this->scale(zoom/c_scale,zoom/c_scale);
        
        c_scale=zoom;
    };
    
    void DrawView::setNet()
    {
        if(DRAW->isAutoNet())
        {
            double pixel_per_cell=DRAW->NetStepX()/(1/c_scale);
            if(pixel_per_cell>70)
            {
                DRAW->setNetStepX(DRAW->NetStepX()*0.5);
                DRAW->setNetStepY(DRAW->NetStepY()*0.5);
                
                DRAW->scalePen(0.5);
                
            }
            if(pixel_per_cell<20)
            {
                DRAW->setNetStepX(DRAW->NetStepX()*2);
                DRAW->setNetStepY(DRAW->NetStepY()*2);
                DRAW->drawNet();
                DRAW->scalePen(2);
                update();
            }

            qDebug()<<"c_scale"<<c_scale<<"NetStep"<<DRAW->NetStepX()<<"PPC"<<pixel_per_cell;
        } 
    };
    
    void	DrawView::wheelEvent ( QWheelEvent * event )
    {
        float numDegrees = event->delta() / 8;
        qDebug()<<"whell:"<<numDegrees;
        //        c_scale=c_scale*0.8;
        qDebug()<<"Scale"<<c_scale;
        
        //setRenderHint(QPainter::Antialiasing);
        if(numDegrees>0)
        {
            
 
            setZoom(zoom()*1.2);
            setNet();
              DRAW->drawNet();
        }
        else
        { 
            setZoom(zoom()*0.8);
            setNet();
            DRAW->drawNet();
            
        }
        
    }    

DrawModule::DrawModule(ExtensionSystem::KPlugin * parent)
    : DrawModuleBase(parent)
{
   
 
    CurView=new DrawView();
    netStepX=1;
    netStepY=1;
    autoNet=true;
    netColor=QColor("gray");
    penIsDrawing=false;
    CurScene=new DrawScene(CurView);
    navigator=new DrawNavigator(CurView);
    showToolsBut=new QToolButton(CurView);
    showToolsBut->move(20,20);
    showToolsBut->setCheckable(true);
    
    
    connect(showToolsBut,SIGNAL(toggled (bool)),this,SLOT(showNavigator(bool)));
    
    navigator->setDraw(this);
    navigator->setParent(CurView);
    navigator->setFixedSize(QSize(130,200));
    navigator->move(20,showToolsBut->pos().y()+showToolsBut->height());
    navigator->hide();
    
    CurScene->setDraw(this);
    CurView->setScene(CurScene);
    penColor.r = penColor.g = penColor.b = 0;
    penColor.a = 255;
    CurView->setDraw(this);
    CurView->centerOn(5,-5);
    CurView->setViewportUpdateMode (QGraphicsView::NoViewportUpdate);//For better perfomance; Manual Update;
    drawNet();
    CreatePen();
    CurView->setZoom(50);
    CurView->setNet();
    netStepX=1;
    netStepY=1;
    
    
    
   
    
    QBrush curBackground=QBrush(QColor(DrawSettings()->value("Draw/BackColor","lightgreen").toString()));

    CurScene->setBackgroundBrush (curBackground);

   
}
void DrawModule::showNavigator(bool state)
    {
        navigator->setVisible(state);
    };
    
/* public static */ QList<ExtensionSystem::CommandLineParameter> DrawModule::acceptableCommandLineParameters()
{
    // See "src/shared/extensionsystem/commandlineparameter.h" for constructor details
    return QList<ExtensionSystem::CommandLineParameter>();
}

/* public slot */ void DrawModule::changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current)
{
    // Called when changed kumir state. The states are defined as enum ExtensionSystem::GlobalState:
    /*
    namespace ExtensionSystem {
        enum GlobalState {
            GS_Unlocked, // Edit mode
            GS_Observe, // Observe mode
            GS_Running, // Running mode
            GS_Input,  // User input required
            GS_Pause  // Running paused
        };
    }
    */
    // TODO implement me
    using namespace ExtensionSystem;  // not to write "ExtensionSystem::" each time in this method scope
    Q_UNUSED(old);  // Remove this line on implementation
    Q_UNUSED(current);  // Remove this line on implementation
}

/* public slot */ void DrawModule::loadActorData(QIODevice * source)
{
    // Set actor specific data (like environment)
    // The source should be ready-to-read QIODevice like QBuffer or QFile
    Q_UNUSED(source);  // By default do nothing

}

/* public */ QWidget* DrawModule::mainWidget() const
{
    // Returns module main view widget, or nullptr if there is no any views
    // NOTE: the method is const and might be called at any time,
    //       so DO NOT create widget here, just return!
    // TODO implement me
    return  CurView;
    //return nullptr;
}

/* public */ QWidget* DrawModule::pultWidget() const
{
    // Returns module control view widget, or nullptr if there is no control view
    // NOTE: the method is const and might be called at any time,
    //       so DO NOT create widget here, just return!
    // TODO implement me
    return nullptr;
}

/* public slot */ void DrawModule::reloadSettings(ExtensionSystem::SettingsPtr settings, const QStringList & keys)
{
    // Updates setting on module load, workspace change or appliyng settings dialog.
    // If @param keys is empty -- should reload all settings, otherwise load only setting specified by @param keys
    // TODO implement me
    QBrush curBackground=QBrush(QColor(settings->value("Draw/BackColor","lightgreen").toString()));
    
    CurScene->setBackgroundBrush (curBackground);
    netColor=QColor(settings->value("LineColor","gray").toString());
    drawNet();
    Q_UNUSED(keys);  // Remove this line on implementation
}

/* public slot */ void DrawModule::reset()
{
    // Resets module to initial state before program execution
    // TODO implement me
    mPen->setBrush(QBrush(QColor("white")));
    penIsDrawing=false;
    mPen->setPos(0,0);
    CurScene->reset();
    CurView->update();
}

/* public slot */ void DrawModule::setAnimationEnabled(bool enabled)
{
    // Sets GUI animation flag on run
    // NOTE this method just setups a flag and might be called anytime, even module not needed
    // TODO implement me
    Q_UNUSED(enabled);  // Remove this line on implementation
}

/* public slot */ void DrawModule::runSetupPen()
{
    /* алг опустить перо */
    // TODO implement me
    mPen->setBrush(QBrush(QColor("black")));
    penIsDrawing=true;
}

/* public slot */ void DrawModule::runReleasePen()
{
    /* алг поднять перо */
    // TODO implement me

    mutex.lock();

    mPen->setBrush(QBrush(QColor(penColor.r, penColor.g, penColor.b, penColor.a)));

    penIsDrawing=false;
    mutex.unlock();
    
}

/* public slot */ void DrawModule::runSetPenColor(const Color& color)
{
    /* алг установить цвет(цвет color) */
    // TODO implement me
    Q_UNUSED(color)  // Remove this line on implementation;
    penColor=color;
    qDebug()
            << "DrawModule::runSetPenColor( { cssValue = \""
            << QColor(color.r, color.g, color.b, color.a).name()
            << "\" } )";
}

/* public slot */ void DrawModule::runMoveTo(const qreal x, const qreal y)
{
    mutex.lock();
    QPointF start=mPen->pos();
    mPen->setPos(x, -y);
    if(penIsDrawing)
    {
        CurScene->addDrawLine(QLineF(start,mPen->pos()), QColor(penColor.r, penColor.g, penColor.b, penColor.a));
    }
    CurView->update();
    mutex.unlock();
}

/* public slot */ void DrawModule::runMoveBy(const qreal dX, const qreal dY)
{
    /* алг сместиться на вектор(вещ dX, вещ dY) */
    // TODO implement me
    mutex.lock();
    QPointF start=mPen->pos();
    mPen->moveBy(dX, -dY);
    if(penIsDrawing)
        {
            CurScene->addDrawLine(QLineF(start,mPen->pos()), QColor(penColor.r, penColor.g, penColor.b, penColor.a));
        }
    CurView->update();
    mutex.unlock();
}

/* public slot */ void DrawModule::runAddCaption(const qreal width, const QString& text)
{
    /* алг надпись(вещ width, лит text) */
    // TODO implement me
    Q_UNUSED(width)  // Remove this line on implementation;
    Q_UNUSED(text)  // Remove this line on implementation;
    
}
void DrawModule::drawNet()
    {
        mutex.lock();
        QPointF start_d=CurView->mapToScene(CurView->geometry().topLeft());
        QPointF end_d=CurView->mapToScene(CurView->geometry().bottomRight());
        qDebug()<<"StartDeb"<<start_d<<end_d;
        // QPointF end=CurView->mapToScene(CurView->viewport()->rect().bottomRight().x(),CurView->viewport()->rect().bottomRight().y());
        QPointF start=CurView->sceneRect().topLeft();
        QPointF end=CurView->sceneRect().bottomRight();
           CurScene->drawNet(start.x(),end.x(),start.y(),end.y(), netColor,netStepX,NetStepY()); 
        CurView->setSceneRect(QRectF(QPointF(start_d.x()-(CurView->geometry().width()/2)*(1/zoom()),start_d.y()-(CurView->geometry().height()/2)*(1/zoom())),
                                     QPointF(end_d.x()+2000*(1/zoom()),end_d.y()+2000*(1/zoom()))));
        mutex.unlock();
        navigator->reDraw(zoom(),netStepY,NetStepX());
       
    };

    
    void DrawModule::zoomIn()
    {
        CurView->setZoom(CurView->zoom()*2);
        CurView->setNet();
        drawNet();
    };
    void DrawModule::zoomOut()
    {
        CurView->setZoom(CurView->zoom()*0.5);
        CurView->setNet();
        drawNet(); 
    };
    void DrawModule::zoomNorm()
    {
       
        mPen->scale(zoom()/50,zoom()/50);
        CurView->setZoom(50);
        CurView->setNet();
        setNetStepX(1);
        setNetStepY(1);
        CurView->centerOn(5,-5);
        CurView->setNet();
        drawNet();
    };
    
     void DrawModule::zoomFullDraw()
    {
        
        QPointF start_d=CurView->mapToScene(CurView->geometry().topLeft());
        QPointF end_d=CurView->mapToScene(CurView->geometry().bottomRight());
        qreal width=end_d.x()-start_d.x();

        QRectF sceneInfoRect=CurScene->getRect();
        qDebug()<<"SceneInfoRect:"<<sceneInfoRect<<"L"<<sceneInfoRect.left()<<"R"<<sceneInfoRect.right()<<"t"<<sceneInfoRect.top()<<"B"<<sceneInfoRect.bottom();
        //CurView->fitInView(sceneInfoRect);
        qreal width2=sceneInfoRect.width();
        CurView->setZoom((CurView->zoom()*(width/width2))*0.64);
        
        QRectF newRect(2*sceneInfoRect.left(),2*sceneInfoRect.top(),2*sceneInfoRect.right(),2*sceneInfoRect.bottom());
        qDebug()<<newRect;
       // CurView->setSceneRect(newRect);
       // CurView->setZoom((CurView->zoom()*(width/width2))*0.8);
        CurView->centerOn((sceneInfoRect.right()+sceneInfoRect.left())/2,(sceneInfoRect.bottom()+sceneInfoRect.top())/2);
        CurView->setNet();
        drawNet();       // CurView->centerOn(0,0);
        mPen->scale(zoom()/50,zoom()/50);
        
    };
    void DrawModule::CreatePen(void)
    {
        
        
        static const int points[] = {  0,0, 6,-3, 18,-15, 15,-18, 3,-6 };
        QPolygon polygon;
        polygon.setPoints(5, points);
        QPolygonF polygonf = QPolygonF(polygon);
        
        mPen = new QGraphicsPolygonItem ( );
        mPen->setPolygon(polygonf);
        mPen->setZValue(100);
        mPen->setBrush(QBrush(QColor("black")));
        CurScene->addItem(mPen);
        mPen->scale(0.5,0.5);
        mPen->scale(0.5,0.5);
        mPen->scale(0.5,0.5);
        mPen->scale(0.5,0.5);
        mPen->scale(0.5,0.5);

        mPen->setZValue(100);
        
        
    }

} // namespace ActorDraw
