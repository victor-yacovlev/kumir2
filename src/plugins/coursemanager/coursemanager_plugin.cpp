#include "coursemanager_plugin.h"
#include <dataformats/kumfile.h>
#include "task/mainwindow.h"
#include "interfaces/analizerinterface.h"
#include "interfaces/runinterface.h"
namespace CourseManager {

   
    
Plugin::Plugin()
    : ExtensionSystem::KPlugin()
    , mainWindow_(nullptr)
    , actionPerformCheck_(nullptr)
    , settingsEditorPage_(nullptr)
    , cur_task(nullptr)
{
    courseMenu=new QMenu(trUtf8("Практикум"));
    MenuList.append(courseMenu);
    rescentMenu=new QMenu(trUtf8("Недавние тетради..."));
    // m_actionCourseLoadRescent->setMenu(rescentMenu);
    MW=new MainWindowTask();
    MW->setup(myResourcesDir(), mySettings());
   
    mainWindow_=MW;
   
    field_no=0;
    prevFld=new QAction(trUtf8("Предыдущая обстановка"),this);
    nextFld=new QAction(trUtf8("Следующая обстановка"),this);
    connect(nextFld,SIGNAL(triggered()),this,SLOT(nextField()));
    connect(prevFld,SIGNAL(triggered()),this,SLOT(prevField()));
    nextFld->setEnabled(false);
    prevFld->setEnabled(false);
    
    }
QList<QMenu*>  Plugin::menus()const
{
    
    return MenuList; 
}; 
void Plugin::rebuildRescentMenu()
    {
        rescentMenu->clear();
        qDebug()<<mySettings()->locationDirectory();
        QStringList lastFiles= mySettings()->value("Courses/LastFiles","").toString().split(";");
        qDebug()<<lastFiles;
        if(lastFiles.count()==0)rescentMenu->setEnabled(false);else  rescentMenu->setEnabled(true);
        
        
        for(int i=0;i<lastFiles.count();i++) {
            if(lastFiles[i]=="")continue;
            QAction *action = rescentMenu->addAction("&"+QString::number(i+1)+" "+lastFiles[i],MW,SLOT(openRescent()));
            Q_UNUSED(action);
        }

    };
QString Plugin::getText()
{
    GI * gui = ExtensionSystem::PluginManager::instance()->findPlugin<GI>();
    Shared::AnalizerInterface * analizer =
            ExtensionSystem::PluginManager::instance()->findPlugin<Shared::AnalizerInterface>();
    const QString text = analizer->sourceFileHandler()->toString(gui->programSource().content);
    qDebug()<<"Text"<< text;
    return text;
}    

void Plugin::setPreProgram(QVariant param)
{
    Shared::AnalizerInterface* analizer = ExtensionSystem::PluginManager::instance()->findPlugin<Shared::AnalizerInterface>();
    if (param.toString().endsWith("." + analizer->defaultDocumentFileNameSuffix()))
  {
      setTextFromFile(param.toString());
  }
  else
  {GI * gui = ExtensionSystem::PluginManager::instance()->findPlugin<GI>();
      Shared::AnalizerInterface * analizer =
              ExtensionSystem::PluginManager::instance()->findPlugin<Shared::AnalizerInterface>();
   Shared::GuiInterface::ProgramSourceText text;
   text.content=analizer->sourceFileHandler()->fromString(param.toString());
   if (analizer->defaultDocumentFileNameSuffix()=="kum") {
    text.content=KumFile::insertTeacherMark(text.content);
    text.language=Shared::GuiInterface::ProgramSourceText::Kumir;
   }
   else if (analizer->defaultDocumentFileNameSuffix()=="py") {
       text.language = Shared::GuiInterface::ProgramSourceText::Python;
   }
   gui->setProgramSource(text);
     
      ExtensionSystem::PluginManager::instance()->switchGlobalState(PluginInterface::GS_Unlocked);
  }
}   
void Plugin::fixOldKumTeacherMark(QDataStream* ds)
{
        
}
bool Plugin::setTextFromFile(QString fname)    
{
    QFile file(fname);
    if(!file.open(QIODevice::ReadOnly))return false;
    
    GI * gui = ExtensionSystem::PluginManager::instance()->findPlugin<GI>();
    Shared::AnalizerInterface * analizer =
            ExtensionSystem::PluginManager::instance()->findPlugin<Shared::AnalizerInterface>();
    Shared::GuiInterface::ProgramSourceText text;
    text.content = analizer->sourceFileHandler()->fromBytes(file.readAll());
    file.close();
    if (fname.endsWith(".kum")) {
        text.language=Shared::GuiInterface::ProgramSourceText::Kumir;
        text.content=KumFile::insertTeacherMark(text.content);
    }
    else if (fname.endsWith(".py")) {
        text.language = Shared::GuiInterface::ProgramSourceText::Python;
    }
    gui->setProgramSource(text);
    return true;
}
    
bool  Plugin::startNewTask(QStringList isps,KumZadanie* task)
    {
        field_no=0;      
        for(int i=0;i<isps.count();i++)
        {
            
            if(isps.at(i)==trUtf8("Файл ввода"))
            {
                Shared::RunInterface * runner = ExtensionSystem::PluginManager::instance()->findPlugin<Shared::RunInterface>();
                QFile* field_data=new QFile(task->field(isps.at(i), field_no));
                field_data->open(QIODevice::ReadOnly|QIODevice::Text);
                QTextStream * stdInStream = new QTextStream(field_data);
                stdInStream->setAutoDetectUnicode(true);
                runner->setStdInTextStream(stdInStream);
                continue;
            }
               
            
            AI* actor=getActor(isps.at(i));
            if(!actor)
            {
            
                return false;
            }
            //TODO LOAD FIELDS;
            QFile* field_data=new QFile(task->field(isps.at(i), field_no));
            qDebug()<<"Set field"<<task->field(isps.at(i), field_no);
           if(! field_data->open(QIODevice::ReadOnly))return false;
            field_data->setObjectName(task->name);
            actor->loadActorData(field_data);
            field_data->close();
        }
        if(task->minFieldCount()>1){
         nextFld->setEnabled(true);
         prevFld->setEnabled(false);
        }else
        {
            nextFld->setEnabled(false);
            prevFld->setEnabled(false);   
        }
      cur_task=task; 
        return true;
    };    
QWidget* Plugin::mainWindow() const
{
    return mainWindow_;

}
AI * Plugin::getActor(QString name)
{
    QList<AI*> Actors= ExtensionSystem::PluginManager::instance()->findPlugins<AI>();
    if(name=="Robot")name=QString::fromUtf8("Робот");//Pach for K1
    qDebug()<<"ActorName"<<name;
    for(int i=0;i<Actors.count();i++)
    {   
        qDebug()<<"Cname:"<<Actors.at(i)->localizedModuleName(QLocale::Russian);
        if(Actors.at(i)->localizedModuleName(QLocale::Russian)==name)return  Actors.at(i);
    }
    
    return NULL;
}
void Plugin::selectNext(KumZadanie* task)
    {
       
        for(int i=0;i<task->isps.count();i++)
        {
            if(task->isps.at(i)==trUtf8("Файл ввода"))
            {
                Shared::RunInterface * runner = ExtensionSystem::PluginManager::instance()->findPlugin<Shared::RunInterface>();
                QFile* field_data=new QFile(task->field(task->isps.at(i), field_no));
                field_data->open(QIODevice::ReadOnly|QIODevice::Text);
                QTextStream * stdInStream = new QTextStream(field_data);
                stdInStream->setAutoDetectUnicode(true);
                //TODO Доделать в кумире
                runner->setStdInTextStream(stdInStream);

                continue;
            }
            AI* actor=getActor(task->isps.at(i));
            if(!actor)
            {
                QMessageBox::information( NULL, "", QString::fromUtf8("Нет исполнтеля:")+task->isps.at(i), 0,0,0); 
                return;
            }
            //TODO LOAD FIELDS;
            QFile* field_data=new QFile(task->field(task->isps.at(i), field_no));
            if(!field_data->open(QIODevice::ReadOnly)){
                QMessageBox::information( NULL, "", QString::fromUtf8("Ошибка открытия обстановки!"), 0,0,0); 
                return;   
            }
            actor->loadActorData(field_data);
            field_data->close();
        }   
    }

void Plugin::checkNext(KumZadanie* task)
{
    
 GI * gui = ExtensionSystem::PluginManager::instance()->findPlugin<GI>();
    selectNext(task);
    gui->startTesting();    
};
void Plugin::loadCource(QString file)
    {
        MW->loadCourseFromFile(file);
    }
void Plugin::startProgram(QVariant param,KumZadanie* task)
{
    field_no=0;
    cur_task=task;
    selectNext( task);
    GI * gui = ExtensionSystem::PluginManager::instance()->findPlugin<GI>(); 
    gui->startTesting();  
};
QAction* Plugin::actionPerformCheck() const
{
    return actionPerformCheck_;
}

QWidget* Plugin::settingsEditorPage()
{
    return nullptr;
//    if (!settingsEditorPage_) {
//        typedef ExtensionSystem::DeclarativeSettingsPage::Entry Entry;
//        typedef QMap<QString,Entry> EntryMap;

//        EntryMap entries; // TODO implement me

//        settingsEditorPage_ = new ExtensionSystem::DeclarativeSettingsPage(
//                    pluginSpec().name,      // Plugin name
//                    tr("Course Manager"),   // Title in setting window
//                    mySettings(),           // Settings object
//                    entries                 // A map of configurable items
//                    );
//    }
//    return settingsEditorPage_;
}

void Plugin::setEnabled(bool value)
{

}

void Plugin::setTestingResult(ProgramRunStatus status, int value)
{
    

    if (status==AbortedOnError || status==UserTerminated)
    {
        MW->setMark(0);
        field_no=0;
        prevFld->setEnabled(field_no>0);
        nextFld->setEnabled((field_no+1)<cur_task->minFieldCount() && cur_task->minFieldCount()>0);
        return;
    };
    MW->setMark(value);
    field_no++;
    if(field_no<cur_task->minFieldCount() && value>7)checkNext(cur_task);
    prevFld->setEnabled(field_no>0);
    nextFld->setEnabled((field_no+1)<cur_task->minFieldCount() && cur_task->minFieldCount()>0);
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
void Plugin::nextField()
    {
      if(field_no<cur_task->minFieldCount() )
      {
          field_no++;
          selectNext(cur_task);
      }
       prevFld->setEnabled(field_no>0);
       nextFld->setEnabled((field_no+1)<cur_task->minFieldCount() && cur_task->minFieldCount()>0);
    };
void Plugin::prevField()
    {
        if(field_no>-1 )
        {
            field_no--;
            selectNext(cur_task);
        }  
        prevFld->setEnabled(field_no>0);
        nextFld->setEnabled(cur_task && field_no<cur_task->minFieldCount() && cur_task->minFieldCount()>0);
    };
void Plugin::lockContrls()
    {
        prevFld->setEnabled(false);
        nextFld->setEnabled(false);
    }
void Plugin::changeGlobalState(ExtensionSystem::GlobalState old,
                               ExtensionSystem::GlobalState current)
{
    if(current==PluginInterface::GS_Running)
    {MW->lockControls();
        nextFld->setEnabled(false);
        prevFld->setEnabled(false);
    };
   if(current==PluginInterface::GS_Observe)
   {MW->unlockControls();
       prevFld->setEnabled(field_no>0);
       nextFld->setEnabled(cur_task && field_no<cur_task->minFieldCount() && cur_task->minFieldCount()>0);
   };
}

QString Plugin::initialize(const QStringList &configurationArguments,
                           const ExtensionSystem::CommandLine & runtimeArguments)
{
    QList<QAction*> actions;
    actions=MW->getActions();
    for(int i=0;i<actions.count();i++)
    {
        courseMenu->addAction(actions.at(i));
        if(i==0)
        {
            courseMenu->addMenu(rescentMenu);
        }
    }
    Shared::AnalizerInterface * analizer =
    ExtensionSystem::PluginManager::instance()
    ->findPlugin<Shared::AnalizerInterface>();
    const QString languageName =
    analizer->languageName().toLower();
    
    MW->setCS(languageName);
    MW->setInterface(this);
    qRegisterMetaType<Shared::CoursesInterface::ProgramRunStatus>
            ("CourseManager.ProgramRunStatus");
    QString error;
    isp_no=0;
    field_no=0;
    courseMenu->addAction(nextFld);
    courseMenu->addAction(prevFld);
    rebuildRescentMenu();

    return error;
}

void Plugin::updateSettings(const QStringList & keys)
{
    if (settingsEditorPage_) {
        settingsEditorPage_->setSettingsObject(mySettings());
    }
    MW->updateSettings(keys, mySettings());
    rebuildRescentMenu();
}


} // namespace CourseManager

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(CourseManager, CourseManager::Plugin)
#endif
