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

// Qt includes
#include <QtCore>
#include <QtGui>

namespace ActorDraw {

    
    void DrawScene::drawNet(double startx ,double endx,double starty,double endy,QColor color,double step)
    {
        for ( int i = 0; i < Netlines.count(); i++)
        {
         if(i==0)
         {
             QPointF pos=Netlines[0]->line().p1 () ;
              QPointF tail=Netlines.last()->line().p2 () ;
             //qDebug()<<"POS"<<pos<<"Tail"<<tail;
             if((pos.x()<startx  && pos.y()<starty)&&(tail.x()>endx-2*step && tail.y()>endy-2*step))return;
         }
         delete Netlines[i];   
        }
        
        Netlines.clear();
        double fx1=startx-5*step,fx2,fy1,fy2; 
        while (fx1 < endx+2*step)
		{
			fx1 = fx1 + step;
			fx2 = fx1;
			fy1 = starty-step;
			fy2 = endy;
            
			Netlines.append(addLine(fx1, fy1 , fx2, fy2 ));
			Netlines.last()->setZValue(0.5);
			Netlines.last()->setPen(QPen(color));
            if(fx1==0)
            {
                QPen axisPen=QPen(color);
                axisPen.setWidth(3);
                Netlines.last()->setPen(axisPen);   
            }
		}
        fy1 = starty-5*step;
        
		while (fy1 < endy+2*step)
		{
			fy1 = fy1 + step;
			fy2 = fy1;
			fx1 = startx-step;
			fx2 = endx;
            
            Netlines.append(addLine(fx1, fy1 , fx2, fy2 ));
			Netlines.last()->setZValue(0.5);
			Netlines.last()->setPen(QPen(color));
            if(fy1==0)
            {
                QPen axisPen=QPen(color);
                axisPen.setWidth(3);
                Netlines.last()->setPen(axisPen);   
            }
            
		}
    }
void DrawView::resizeEvent ( QResizeEvent * event )
    {
      //  qDebug()<<"resizeEvent";
         DRAW->drawNet();
    };
void DrawView:: scrollContentsBy ( int dx, int dy )
    {
        Q_UNUSED(dx);
        Q_UNUSED(dy);
    //    qDebug() << QString("SCROLL EVENT ") << dx << " " << dy;
    }    
    
    
DrawModule::DrawModule(ExtensionSystem::KPlugin * parent)
    : DrawModuleBase(parent)
{
    //QVBoxLayout *layout = new QVBoxLayout;
 
    CurView=new DrawView();
    netStep=10;
    netColor=QColor("gray");
   // layout->addWidget(CurView);
    CurScene=new DrawScene(CurView);
    CurView->setScene(CurScene);
   // CurScene->addItem(new QGraphicsLineItem(-10000, -10000,10000,1000));
    CurView->setDraw(this);
    CurView->centerOn(0,0);
    drawNet();
    CurScene->setBackgroundBrush (QBrush(QColor("lightgreen")));
    // Module constructor, called once on plugin load
    // TODO implement me
}

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
    Q_UNUSED(settings);  // Remove this line on implementation
    Q_UNUSED(keys);  // Remove this line on implementation
}

/* public slot */ void DrawModule::reset()
{
    // Resets module to initial state before program execution
    // TODO implement me
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
    
}

/* public slot */ void DrawModule::runReleasePen()
{
    /* алг поднять перо */
    // TODO implement me
    
}

/* public slot */ void DrawModule::runSetPenColor(const Color& color)
{
    /* алг установить цвет(цвет color) */
    // TODO implement me
    Q_UNUSED(color)  // Remove this line on implementation;
    qDebug()
            << "DrawModule::runSetPenColor( { cssValue = \""
            << color.cssValue
            << "\" } )";
}

/* public slot */ void DrawModule::runMoveTo(const qreal x, const qreal y)
{
    /* алг сместиться в точку(вещ x, вещ y) */
    // TODO implement me
    Q_UNUSED(x)  // Remove this line on implementation;
    Q_UNUSED(y)  // Remove this line on implementation;
    
}

/* public slot */ void DrawModule::runMoveBy(const qreal dX, const qreal dY)
{
    /* алг сместиться на вектор(вещ dX, вещ dY) */
    // TODO implement me
    Q_UNUSED(dX)  // Remove this line on implementation;
    Q_UNUSED(dY)  // Remove this line on implementation;
    
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
        QPointF start=CurView->mapToScene(CurView->viewport()->rect().topLeft().x(),CurView->viewport()->rect().topLeft().y());
        QPointF end=CurView->mapToScene(CurView->viewport()->rect().bottomRight().x(),CurView->viewport()->rect().bottomRight().y());
           CurScene->drawNet(start.x(),end.x(),start.y(),end.y(), netColor,netStep); 
    };



} // namespace ActorDraw
