#include "coursemanager_plugin.h"
#include <dataformats/kumfile.h>
#include "task/mainwindow.h"
#include "interfaces/analizerinterface.h"
#include "interfaces/runinterface.h"
#ifdef Q_OS_LINUX
#include <iostream>
#endif
namespace CourseManager {

   
    
Plugin::Plugin()
    : ExtensionSystem::KPlugin()
    , mainWindow_(nullptr)
    , actionPerformCheck_(nullptr)
    , settingsEditorPage_(nullptr)
    , cur_task(nullptr)
{
    
#ifdef Q_OS_LINUX
    QProcessEnvironment pe;
    pe=QProcessEnvironment::systemEnvironment();
    // qDebug()<<"PE"<<pe.toStringList();
    qDebug()<<"Display"<<pe.value("DISPLAY");
    if(!pe.keys().indexOf("DISPLAY")>0 ||pe.value("DISPLAY").isEmpty() ) //NO DISPLAY
    {
        qDebug()<<"CourseManager:Console mode";

        DISPLAY=false;
        field_no=0;
        return;
    }
    qDebug()<<"CourseManager:Console Mode";
#endif
    DISPLAY=true;
    courseMenu=new QMenu(trUtf8("Практикум"));
    MenuList.append(courseMenu);
    rescentMenu=new QMenu(trUtf8("Недавние тетради/курсы..."));
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
int Plugin::loadWorkBook(QString wbfilename)
    {
        QDomDocument workXml;
        QFile f(wbfilename);
        


        if(f.atEnd())
        {
           
            return 3;
        };
   
        QString error;
        int str,pos;
        workXml.setContent(f.readAll(),true,&error,&str,&pos);
        qDebug()<<"File parce:"<<error<<"str"<<str<<" pos"<<pos;
        
        QDomElement root=workXml.documentElement ();
        if(root.tagName()!="COURSE")
        {
       
            return 4;
        };
        QDomElement fileEl=root.firstChildElement("FILE");
        QString krsFile=fileEl.attribute("fileName");
               
        QString fileN=fileEl.attribute("fileName");
     
        QDomNodeList marksElList=root.elementsByTagName("MARK"); //Оценки
        //qDebug()<<"Loading marks "<<marksElList.count();
        for(int i=0;i<marksElList.count();i++)
        {
            int taskId=marksElList.at(i).toElement().attribute("testId").toInt();
            int mark=marksElList.at(i).toElement().attribute("mark").toInt();
            qDebug()<<"task:"<<taskId<<" mark:"<<mark;
            course->setMark(taskId,mark);
      
        };
        
        //qDebug()<<"Loading user prgs...";
        QDomNodeList prgElList=root.elementsByTagName("USER_PRG");//Программы
        for(int i=0;i<prgElList.count();i++)
        {
            int taskId=prgElList.at(i).toElement().attribute("testId").toInt();
            qDebug()<<"Tassk id"<<taskId;
            QString prg =prgElList.at(i).toElement().attribute("prg");
            QModelIndex tIdx=course->getIndexById(taskId);
            
      
            course->setUserText(taskId,prg);
            
        };
        
        QDomNodeList prgElListT=root.elementsByTagName("TESTED_PRG");//Программы тестированные
        for(int i=0;i<prgElListT.count();i++)
        {
            int taskId=prgElListT.at(i).toElement().attribute("testId").toInt();
            QString prg =prgElListT.at(i).toElement().attribute("prg");
            
            course->setUserTestedText(taskId,prg);
            
        };
        return 0;
    }
int  Plugin::loadCourseFromConsole(QString wbname,QString cbname)
    {
        
        QFileInfo fi(cbname);
        if(!fi.exists())
        {
            
            return 1;
        };
        QFileInfo fi2(wbname);
        if(!fi2.exists())
        {
            
            return 2;
        };
 
        course=new courseModel();
   
        int tasks=course->loadCourse(cbname);
        qDebug()<<"Tasks "<<tasks<<" loaded";
        int wb_error=loadWorkBook(wbname);
   //     if(fileName.right(9)==".work.xml")//Загрузка оценок и программ
   //     {
   //         loadMarks(fileName);
   //         this->show();
   //         return;
    //    }else
   //         cursWorkFile.setFileName("");
   //     loadCourseData(fileName);
        QString cText=course->courceDescr();
        
 

        fprintf(stdout, "TODO: Load from file");
        return wb_error;
        
    }

void Plugin::start()
{
      qDebug()<<"Starts with coursemanager";
};
void Plugin::rebuildRescentMenu()
    {
        rescentMenu->clear();
        qDebug()<<mySettings()->locationDirectory();
        QStringList lastFiles= mySettings()->value("Courses/LastFiles","").toString().split(";");
        qDebug()<<lastFiles;
        if(lastFiles.count()==0)rescentMenu->setEnabled(false);else  rescentMenu->setEnabled(true);
        bool hasAnyItem=false;
        
        for(int i=0;i<lastFiles.count();i++) {
            if(lastFiles[i].trimmed()=="")continue;
     

            
            QAction *action = rescentMenu->addAction(QFileInfo(lastFiles[i]).fileName(),MW,SLOT(openRescent()));
            action->setProperty("fullName", lastFiles[i]);
             hasAnyItem = true;
            Q_UNUSED(action);
        }
 rescentMenu->setEnabled(hasAnyItem);
        
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
      QUrl base=QUrl(MW->baseCourseFile());//path to kurs.xml file
      base.setScheme("Course");
      text.url=base;
      text.title=MW->task.name;
      qDebug()<<base.isLocalFile()<<base.path ();
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
    QUrl base=QUrl(MW->baseCourseFile());//path to kurs.xml file
    base.setScheme("Course");
    text.url=base;
    text.title=MW->task.name;
    qDebug()<<base.isLocalFile()<<base.path ();
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
    bool Plugin::isSafeToQuit(){return MW->safeToQuit();};

void Plugin::setTestingResult(ProgramRunStatus status, int value)
{
    

    if (status==AbortedOnError || status==UserTerminated)
    {
        MW->setMark(1);
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
    qDebug()<<"DIPLSY"<<DISPLAY;
    if(!DISPLAY)
    {
        if(!runtimeArguments.value('w').isValid())return trUtf8("Нет тетради");
        if(!runtimeArguments.value('c').isValid())return trUtf8("Нет учебника");
        
        qDebug()<<"LOAD WORK BOOK ERR CODE:"<<loadCourseFromConsole(runtimeArguments.value('w').toString(),runtimeArguments.value('c').toString());
        return "";
    }
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
    if(!DISPLAY)return;
    

    
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
