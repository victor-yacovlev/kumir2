#include "coursemanager_plugin.h"
#include <dataformats/kumfile.h>
#include "task/mainwindow.h"
#include "interfaces/analizerinterface.h"
#include "interfaces/runinterface.h"
#include "interfaces/generatorinterface.h"
#include "interfaces/runinterface.h"
    #include <fstream>
    #include <iostream>

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
int Plugin::loadWorkBook(QString wbfilename,QString cbname)
    {
        QDomDocument workXml;
        QFile f(wbfilename);
        
        if  (!f.open(QIODevice::ReadOnly))
        {
            QMessageBox::information( 0, "", trUtf8("Ошибка открытия файла: ") + wbfilename, 0,0,0);
            return 5;
            
        };

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
            fprintf(stdout, "%s %d %d \n",cbname.toLatin1().data(),taskId,mark);
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
        cur_courseFileInfo=fi;
        course=new courseModel();
   
        int tasks=course->loadCourse(cbname);
        qDebug()<<"Tasks "<<tasks<<" loaded";
        int wb_error=loadWorkBook(wbname,fi.fileName());

        QString cText=course->courceDescr();
        
 

    //    fprintf(stdout, "TODO: Load from file");
        return wb_error;
        
    }
int Plugin::checkTaskFromConsole(const int taskID)
    {
        KumZadanie task;
        QString curDir=".";
        QFileInfo ioDir(curDir+'/'+course->progFile(taskID));
        qDebug()<<"PRG FILE"<<course->progFile(taskID);
        if(ioDir.isFile())
        {
            qDebug()<<"TODO: IO FILE";
        };
        task.isps=course->Modules(taskID);
        task.fields.clear();
        task.name=course->getTitle(taskID);
        for(int i=0;i<task.isps.count();i++)
        {
            QStringList t_fields=course->Fields(taskID,task.isps[i]);
            for(int j=0;j<t_fields.count();j++)
            {
    
                task.fields.insertMulti(task.isps[i],curDir+'/'+t_fields[j]);
                
            };

        }
        Shared::AnalizerInterface* analizer = ExtensionSystem::PluginManager::instance()->findPlugin<Shared::AnalizerInterface>();
         Shared::Analizer::SourceFileInterface::Data kumFile;
        if(course->getUserText(taskID)!="")
        {
           
            kumFile = analizer->sourceFileHandler()->fromString(course->getUserText(taskID));
        }
        else return 1;
        Shared::Analizer::InstanceInterface * analizer_i =
        analizer->createInstance();
        
        QString dirname = curDir;
        analizer_i->setSourceDirName(dirname);
        analizer_i->setSourceText(kumFile.visibleText + "\n" + kumFile.hiddenText);
        
        QList<Shared::Analizer::Error> errors = analizer_i->errors();
        for (int i=0; i<errors.size(); i++) {
            Shared::Analizer::Error e = errors[i];
            QString errorMessage = tr("Error: ") +
            task.name+
            ":" + QString::number(e.line+1) +
            ":" + QString::number(e.start+1) + "-" + QString::number(e.start+e.len) +
            ": " + e.message;
            std::cerr << errorMessage.toLocal8Bit().data();
            std::cerr << std::endl;
        }
        if(errors.size()>0)return 2;
       AST::DataPtr ast = analizer_i->compiler()->abstractSyntaxTree();
       Shared::GeneratorInterface * generator_ = ExtensionSystem::PluginManager::instance()->findPlugin<Shared::GeneratorInterface>();
       QString suffix;
       QString mimeType;
       QByteArray outData;
       generator_->generateExecutable(ast, outData, mimeType, suffix);
       QDir::setCurrent(curDir);
       Shared::RunInterface * runner = ExtensionSystem::PluginManager::instance()->findPlugin<Shared::RunInterface>();
        Shared::RunInterface::RunnableProgram program;
        program.executableData = outData;
        program.executableFileName = "";
        runner->loadProgram(program);
        for(int i=0;i<task.fields.count();i++)
        {
        field_no=i;
        selectNext(&task);
        runner->runTesting();
        }
        return 0;    //QVariant valueStackTopItem()
     
    }
void Plugin::start()
    {
      qDebug()<<"Starts with coursemanager";
        QList<int> taskIds=course->getIDs();
        for(int i=0;i<taskIds.count();i++)
        {
            field_no=0;
            int res=checkTaskFromConsole(taskIds[i]);
            qDebug()<<"Test result "<<res<<" taskId"<<taskIds[i];
        }
            
    
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
void Plugin::showError(QString err)
    {
     if(DISPLAY)
     {
            QMessageBox::information( NULL, "", err, 0,0,0);
     }
    else std::cerr << err.toLocal8Bit().data();
        
    }
void Plugin::selectNext(KumZadanie* task)
    {
       QString dirName="";
        if(!DISPLAY)dirName=cur_courseFileInfo.absolutePath () ;
        
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
                showError(QString::fromUtf8("Нет исполнтеля:")+task->isps.at(i));
                return;
            }
           
            QFile* field_data=new QFile(dirName+task->field(task->isps.at(i), field_no));
            qDebug()<<"Loadfield"<<dirName+task->field(task->isps.at(i),field_no);
            if(!field_data->open(QIODevice::ReadOnly)){
               showError(QString::fromUtf8("Ошибка открытия обстановки!"));
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
