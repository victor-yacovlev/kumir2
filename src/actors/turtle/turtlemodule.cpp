/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents.
You should change it corresponding to functionality.
*/

// Self include
#include "turtlemodule.h"

// Kumir includes
#include "extensionsystem/kplugin.h"

// Qt includes
#include <QtCore>
#include <QtGui>

namespace ActorTurtle {

TurtleModule::TurtleModule(ExtensionSystem::KPlugin * parent)
    : TurtleModuleBase(parent)
{
    Turtle=new turtle();
    Tpult=new TurtlePult();
    Tpult->turtleObj=Turtle;
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
    return Tpult;
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
    // Resets module to initial state before program execution
    // TODO implement me
}

/* public slot */ void TurtleModule::setAnimationEnabled(bool enabled)
{
    // Sets GUI animation flag on run
    // NOTE this method just setups a flag and might be called anytime, even module not needed
    // TODO implement me
    Q_UNUSED(enabled);  // Remove this line on implementation
}

/* public slot */ void TurtleModule::runTailUp()
{
    /* алг поднять хвост */
    // TODO implement me
    
}

/* public slot */ void TurtleModule::runTailDown()
{
    /* алг опустить хвост */
    // TODO implement me
    
}

/* public slot */ void TurtleModule::runForward(const int dist)
{
    /* алг вперед(цел dist) */
    // TODO implement me
    Q_UNUSED(dist)  // Remove this line on implementation;
    
}

/* public slot */ void TurtleModule::runBack(const int dist)
{
    /* алг назад(цел dist) */
    // TODO implement me
    Q_UNUSED(dist)  // Remove this line on implementation;
    
}

/* public slot */ void TurtleModule::runLeft(const int angle)
{
    /* алг влево(цел angle) */
    // TODO implement me
    Q_UNUSED(angle)  // Remove this line on implementation;
    
}

/* public slot */ void TurtleModule::runRight(const int angle)
{
    /* алг вправо(цел angle) */
    // TODO implement me
    Q_UNUSED(angle)  // Remove this line on implementation;
    
}



} // namespace ActorTurtle
