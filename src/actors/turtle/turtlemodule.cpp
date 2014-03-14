/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents.
You should change it corresponding to functionality.
*/

// Self include
#include "turtlemodule.h"
#include "turtle.h"
// Kumir includes
#include "extensionsystem/kplugin.h"

// Qt includes
#include <QtCore>
#include <QtGui>

namespace ActorTurtle {
    class AAA : public QWidget {
    public:
        inline explicit AAA(QWidget * pult): QWidget(), pult_(pult) {
            setLayout(new QVBoxLayout);
            layout()->setContentsMargins(0, 0, 0, 0);
            layout()->addWidget(pult);
        }
        inline QSize minimumSizeHint() const { return pult_->minimumSizeHint(); }
    private:
        QWidget * pult_;
    };
TurtleModule::TurtleModule(ExtensionSystem::KPlugin * parent)
    : TurtleModuleBase(parent)
{
    Turtle=new turtle(myResourcesDir());
    Tpult=new TurtlePult(myResourcesDir());
    Tpult->turtleObj=Turtle;
   
    animation=false;
    // Module constructor, called once on plugin load
    // TODO implement me
}

/* public static */ QList<ExtensionSystem::CommandLineParameter> TurtleModule::acceptableCommandLineParameters()
{
    // See "src/shared/extensionsystem/commandlineparameter.h" for constructor details
    return QList<ExtensionSystem::CommandLineParameter>();
}

/* public slot */ void TurtleModule::changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current)
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

/* public slot */ void TurtleModule::loadActorData(QIODevice * source)
{
    // Set actor specific data (like environment)
    // The source should be ready-to-read QIODevice like QBuffer or QFile
    Q_UNUSED(source);  // By default do nothing

}

/* public */ QWidget* TurtleModule::mainWidget() const
{

    return Turtle;
}

/* public */ QWidget* TurtleModule::pultWidget() const
{
    // Returns module control view widget, or nullptr if there is no control view
    // NOTE: the method is const and might be called at any time,
    //       so DO NOT create widget here, just return!
    
    // TODO implement me
    static QWidget * dummy = new AAA(Tpult);
    return dummy;
   
}

/* public slot */ void TurtleModule::reloadSettings(ExtensionSystem::SettingsPtr settings, const QStringList & keys)
{
    // Updates setting on module load, workspace change or appliyng settings dialog.
    // If @param keys is empty -- should reload all settings, otherwise load only setting specified by @param keys
    // TODO implement me
    Q_UNUSED(settings);  // Remove this line on implementation
    Q_UNUSED(keys);  // Remove this line on implementation
}

/* public slot */ void TurtleModule::reset()
{
    Turtle->reset();
}

/* public slot */ void TurtleModule::setAnimationEnabled(bool enabled)
{
    // Sets GUI animation flag on run
    // NOTE this method just setups a flag and might be called anytime, even module not needed
    // TODO implement me
    animation=enabled;
    qDebug()<<"Anim"<<animation;
   
}

/* public slot */ void TurtleModule::runTailUp()
{
Turtle->TailUp();
    
}

/* public slot */ void TurtleModule::runTailDown()
{
    Turtle->TailDown();
    
}

/* public slot */ void TurtleModule::runForward(const int dist)
{
    if(animation)msleep(5550);
    Turtle->step=dist;
    if(!Turtle->moveT())
        setError(trUtf8("Нельзя вперед!"));
    
}

/* public slot */ void TurtleModule::runBack(const int dist)
{
    if(animation)msleep(250);
    Turtle->step=-dist;
    if(!Turtle->moveT())
        setError(trUtf8("Нельзя назад!"));
    
}

/* public slot */ void TurtleModule::runLeft(const int angle)
{
    if(animation)msleep(250);
    Turtle->grad=-angle;
    Turtle->rotate();
    
}

/* public slot */ void TurtleModule::runRight(const int angle)
{
    if(animation)msleep(250);
    Turtle->grad=angle;
    Turtle->rotate();
    
}



} // namespace ActorTurtle
