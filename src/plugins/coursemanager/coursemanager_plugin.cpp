#include "coursemanager_plugin.h"
#include <dataformats/kumfile.h>
#include "task/mainwindow.h"

namespace CourseManager {

   
    
Plugin::Plugin()
    : ExtensionSystem::KPlugin()
    , mainWindow_(nullptr)
    , actionPerformCheck_(nullptr)
    , settingsEditorPage_(nullptr)
{
    courseMenu=new QMenu(trUtf8("Практикум"));
    MenuList.append(courseMenu);
    MW=new MainWindowTask();
    MW->setup();
    mainWindow_=MW;
    field_no=0;

}
QList<QMenu*>  Plugin::menus()const
{
    
    return MenuList; 
}; 

QString Plugin::getText()
{
 GI * gui = ExtensionSystem::PluginManager::instance()->findPlugin<GI>();
    gui->programSource();
    return gui->programSource().content.visibleText+QChar('\n')+gui->programSource().content.hiddenText;
}    
void Plugin::setPreProgram(QVariant param)
{
  if(param.toString().right(4)==".kum")
  {
      setTextFromFile(param.toString());
  }
  else
  {GI * gui = ExtensionSystem::PluginManager::instance()->findPlugin<GI>();
   Shared::GuiInterface::ProgramSourceText text;
   text.content=KumFile::fromString(param.toString()); 
    text.language=Shared::GuiInterface::ProgramSourceText::Kumir;
      gui->setProgramSource(text);
  }
}   
    
bool Plugin::setTextFromFile(QString fname)    
{
    QFile file(fname);
    if(!file.open(QIODevice::ReadOnly))return false;
    
    GI * gui = ExtensionSystem::PluginManager::instance()->findPlugin<GI>();
    Shared::GuiInterface::ProgramSourceText text;
    QDataStream ds(&file);
    ds>>text.content;
    text.language=Shared::GuiInterface::ProgramSourceText::Kumir;
    gui->setProgramSource(text);
    return true;
}
    
bool  Plugin::startNewTask(QStringList isps)
    {return true;};    
QWidget* Plugin::mainWindow() const
{
    return mainWindow_;

}
AI * Plugin::getActor(QString name)
{
    QList<AI*> Actors= ExtensionSystem::PluginManager::instance()->findPlugins<AI>();
    if(name=="Robot")name=QString::fromUtf8("Робот");//Pach for K1
    for(int i=0;i<Actors.count();i++)
    {   
        if(Actors.at(i)->name()==name)return  Actors.at(i); 
    }
    return NULL;
}

void Plugin::checkNext(KumZadanie* task)
{
    
    GI * gui = ExtensionSystem::PluginManager::instance()->findPlugin<GI>();
    for(int i=0;i<task->isps.count();i++)
    {
        AI* actor=getActor(task->isps.at(i));
        if(!actor)
        {
            QMessageBox::information( NULL, "", QString::fromUtf8("Нет исполнтеля:")+task->isps.at(i), 0,0,0); 
            return;
        }
        //TODO LOAD FIELDS;
        QFile* field_data=new QFile(task->field(task->isps.at(i), field_no));
        actor->loadActorData(field_data);
    }
    gui->startTesting();    
};
void Plugin::startProgram(QVariant param,KumZadanie* task)
{
    field_no=0;
    checkNext( task);
};
QAction* Plugin::actionPerformCheck() const
{
    return actionPerformCheck_;
}

QWidget* Plugin::settingsEditorPage()
{
    if (!settingsEditorPage_) {
        typedef ExtensionSystem::DeclarativeSettingsPage::Entry Entry;
        typedef QMap<QString,Entry> EntryMap;

        EntryMap entries; // TODO implement me

        settingsEditorPage_ = new ExtensionSystem::DeclarativeSettingsPage(
                    pluginSpec().name,      // Plugin name
                    tr("Course Manager"),   // Title in setting window
                    mySettings(),           // Settings object
                    entries                 // A map of configurable items
                    );
    }
    return settingsEditorPage_;
}

void Plugin::setEnabled(bool value)
{

}

void Plugin::setTestingResult(ProgramRunStatus status, int value)
{
    MW->setMark(value);
    if (status==ProgramRunStatus::AbortedOnError || status==ProgramRunStatus::UserTerminated)
        MW->setMark(0);
    
qDebug()<<"Set testing results"<<value;
}

void Plugin::saveSession() const
{

}

void Plugin::restoreSession()
{

}

void Plugin::changeCurrentDirectory(const QString &path)
{

}

void Plugin::changeGlobalState(ExtensionSystem::GlobalState old,
                               ExtensionSystem::GlobalState current)
{
    if(current==ExtensionSystem::GlobalState::GS_Running)MW->lockControls();
   if(current==ExtensionSystem::GlobalState::GS_Observe)MW->unlockControls();  
}

QString Plugin::initialize(const QStringList &arguments)
{
    QList<QAction*> actions;
    actions=MW->getActions();
    for(int i=0;i<actions.count();i++)
    {
        courseMenu->addAction(actions.at(i));  
    }
    MW->setCS(trUtf8("Кумир"));
    MW->setInterface(this);
    qRegisterMetaType<Shared::CoursesInterface::ProgramRunStatus>
            ("CourseManager.ProgramRunStatus");
    QString error;
    isp_no=0;
    field_no=0;
    return error;
}

void Plugin::updateSettings()
{
    if (settingsEditorPage_) {
        settingsEditorPage_->setSettingsObject(mySettings());
    }
}


} // namespace CourseManager

Q_EXPORT_PLUGIN2(CourseManager, CourseManager::Plugin)
