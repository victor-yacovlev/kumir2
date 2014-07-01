/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents.
You should change it corresponding to functionality.
*/

// Self include
#include "vodoleymodule.h"
#include "pult.h"

// Kumir includes
#include "extensionsystem/kplugin.h"

// Qt includes
#include <QtCore>
#include <QtGui>

namespace ActorVodoley {

VodoleyModule * VodoleyModule::self = 0;

VodoleyModule::VodoleyModule(ExtensionSystem::KPlugin * parent)
    : VodoleyModuleBase(parent)
{
    self = this;
    MainWindow = 0;
}

void VodoleyModule::createGui()
{
    MainWindow=new Vodoley();
    // Module constructor, called once on plugin load
    // TODO implement me
    QList<QAction*> actions;
    actions.append(m_actionVodoleyNewEnvironment);
    actions.append(m_actionVodoleyLoadEnvironment);
    actions.append(m_actionVodoleySaveEnvironment);
    MainWindow->createActions(actions);
   // m_actionVodoleyNewEnvironment
}

QString VodoleyModule::initialize(const QStringList &configurationParameters, const ExtensionSystem::CommandLine &)
{
    if (!configurationParameters.contains("tablesOnly")) {
        createGui();
    }
    return "";
}

/* public static */ QList<ExtensionSystem::CommandLineParameter> VodoleyModule::acceptableCommandLineParameters()
{
    // See "src/shared/extensionsystem/commandlineparameter.h" for constructor details
    return QList<ExtensionSystem::CommandLineParameter>();
}

/* public slot */ void VodoleyModule::changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current)
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

/* public slot */ void VodoleyModule::loadActorData(QIODevice * source)
{
    // Set actor specific data (like environment)
    // The source should be ready-to-read QIODevice like QBuffer or QFile
    
    MainWindow->loadIoDevice(source);
    MainWindow->reset();

}

/* public */ QWidget* VodoleyModule::mainWidget() const
{
    return  MainWindow;
    //return nullptr;
}

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

/* public */ QWidget* VodoleyModule::pultWidget() const
{
    // Returns module control view widget, or nullptr if there is no control view
    // NOTE: the method is const and might be called at any time,
    //       so DO NOT create widget here, just return!
    // TODO implement me
    qDebug()<<"Pult"<<MainWindow->pult->size();
    qDebug() << "Pult visible " << MainWindow->pult->isVisible();


    static QWidget * dummy = new AAA(MainWindow->pult);
    return dummy;
//    return MainWindow->pult;
   // return nullptr;
}

/* public slot */ void VodoleyModule::reloadSettings(ExtensionSystem::SettingsPtr settings, const QStringList & keys)
{
    // Updates setting on module load, workspace change or appliyng settings dialog.
    // If @param keys is empty -- should reload all settings, otherwise load only setting specified by @param keys
    // TODO implement me
    Q_UNUSED(settings);  // Remove this line on implementation
    Q_UNUSED(keys);  // Remove this line on implementation
}

/* public slot */ void VodoleyModule::reset()
{
    // Resets module to initial state before program execution
    // TODO implement me
    MainWindow->reset();
}

/* public slot */ void VodoleyModule::setAnimationEnabled(bool enabled)
{
    // Sets GUI animation flag on run
    // NOTE this method just setups a flag and might be called anytime, even module not needed
    // TODO implement me
    Q_UNUSED(enabled);  // Remove this line on implementation
}

/* public slot */ void VodoleyModule::runFillA()
{
    /* алг наполни A */
    // TODO implement me
   MainWindow->FillA();
}

/* public slot */ void VodoleyModule::runFillB()
{
    /* алг наполни B */
    // TODO implement me
    MainWindow->FillB();
    
}

/* public slot */ void VodoleyModule::runFillC()
{
    /* алг наполни C */
    // TODO implement me
    MainWindow->FillC();
    
}

/* public slot */ void VodoleyModule::runEmptyA()
{
    /* алг вылей A */
    // TODO implement me
    MainWindow->MoveFromTo(0,3);//Move water from A to ....
    
}

/* public slot */ void VodoleyModule::runEmptyB()
{
    /* алг вылей B */
    // TODO implement me
    MainWindow->MoveFromTo(1,3);
    
}

/* public slot */ void VodoleyModule::runEmptyC()
{
    /* алг вылей C */
    // TODO implement me
    MainWindow->MoveFromTo(1,3);
    
}

/* public slot */ void VodoleyModule::runFromAToB()
{
    /* алг перелей из A в B */
    // TODO implement me
    MainWindow->MoveFromTo(0,1);
    
}

/* public slot */ void VodoleyModule::runFromAToC()
{
    /* алг перелей из A в C */
    // TODO implement me
    MainWindow->MoveFromTo(0,2);
}

/* public slot */ void VodoleyModule::runFromBToA()
{
    /* алг перелей из B в A */
    // TODO implement me
    MainWindow->MoveFromTo(1,0);
}

/* public slot */ void VodoleyModule::runFromBToC()
{
    /* алг перелей из B в C */
    // TODO implement me
    MainWindow->MoveFromTo(1,2);
    
}

/* public slot */ void VodoleyModule::runFromCToB()
{
    /* алг перелей из C в B */
    // TODO implement me
    MainWindow->MoveFromTo(2,1);
}

/* public slot */ void VodoleyModule::runFromCToA()
{
    /* алг перелей из C в A */
    // TODO implement me
    MainWindow->MoveFromTo(2,0);
    
}

/* public slot */ bool VodoleyModule::runTaskComplited()
{
    /* алг лог @@задание выполненно */
    // TODO implement me
    return MainWindow->ready();
    
}



} // namespace ActorVodoley
