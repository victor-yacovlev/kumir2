#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "interface.h"

 #include <QMacStyle>
MainWindowTask::MainWindowTask(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowTask)
{

cursFile="";
  course=NULL;
    curDir="";
    progChange.clear();
    connect(qApp,SIGNAL(aboutToQuit()),this,SLOT(aboutToQuit()));

}
 void MainWindowTask::setup()
 {
     ui->setupUi(this);
     ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
     ui->treeView->setIconSize(QSize(25,25));

     ui->treeView->setStyleSheet("icon-size: 25px;font-size: 14px;");
     settings = new QSettings("NIISI RAS","Kumir");
     settings->setIniCodec("UTF-8");
     customMenu.hide();
     connect(ui->loadCurs,SIGNAL(activated()),this,SLOT(loadCourse()));
     connect(ui->actionSave,SIGNAL(activated()),this,SLOT(saveCourse()));
     connect(ui->treeView,SIGNAL(clicked(QModelIndex)),this,SLOT(showText(QModelIndex)));
       connect(ui->do_task,SIGNAL(triggered()),this,SLOT(startTask()));
       qDebug()<<"Check Connect tttttttttttttttttt";
       connect(ui->checkTask,SIGNAL(triggered()),this,SLOT(checkTask()));
      connect(ui->actionReset,SIGNAL(triggered()),this,SLOT(resetTask()));
        connect(ui->actionClose,SIGNAL(triggered()),this,SLOT(Close()));
           connect(ui->actionTested,SIGNAL(triggered()),this,SLOT(returnTested()));
           connect(ui->treeView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuRequested(QPoint)));

           customMenu.addAction(ui->actionAdd);
           customMenu.addAction(ui->actionRemove);
           customMenu.addAction(ui->actionEdit);
       customMenu.addSeparator();
       customMenu.addAction(ui->actionup);
       customMenu.addAction(ui->actionDown);
         customMenu.addAction(ui->addDeep);
       connect(ui->actionup,SIGNAL(triggered()),this,SLOT(moveUp()));
       connect(ui->actionDown,SIGNAL(triggered()),this,SLOT(moveDown()));
           connect(ui->actionAdd,SIGNAL(triggered()),this,SLOT(addTask()));
            connect(ui->addDeep,SIGNAL(triggered()),this,SLOT(addDeepTask()));
           connect(ui->actionSaveK,SIGNAL(triggered()),this,SLOT(saveKurs()));
            connect(ui->actionSaveKas,SIGNAL(triggered()),this,SLOT(saveKursAs()));
           connect(ui->actionRemove,SIGNAL(triggered()),this,SLOT(deleteTask()));
         //  newDialog=new newKursDialog();
          // connect(ui->actionNewK,SIGNAL(triggered()),this,SLOT(newKurs()));
         //  editDialog = new EditDialog(this);
          // connect(ui->actionEdit,SIGNAL(triggered()),this,SLOT(editTask()));
           ui->menuKurs->menuAction()->setVisible(false);
          // ui->menuKurs->menuAction()->setEnabled(false);
           setEditTaskEnabled(false);
           ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
           editRoot=new QLineEdit(ui->treeView);
           editRoot->hide();
           connect(editRoot,SIGNAL(editingFinished ()),this,SLOT(endRootEdit()));
           ui->menuKurs->hide();
           ui->treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
isTeacher=false;
       onTask=false;
       cursFile="";

       //ui->textBrowser->setVisible(false);
 };
QList<QAction*> MainWindowTask::getActions()
{
    QList<QAction*> toRet;
    toRet.append(ui->loadCurs);
    toRet.append(ui->checkTask);
    toRet.append(ui->actionReset);  
    return toRet;
}
MainWindowTask::~MainWindowTask()
{
    delete ui;
}

void MainWindowTask::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindowTask::loadCourseData(const QString fileName)
{

course=new courseModel();
connect(course,SIGNAL(dataChanged (QModelIndex ,QModelIndex )),ui->treeView,SLOT(dataChanged(QModelIndex,QModelIndex)));
int tasks=course->loadCourse(fileName);
course->setTeacher(isTeacher);
if(tasks==-1)
{
//QMessageBox::information( 0, "", trUtf8("Ошибка открытия файла: ") + fileName, 0,0,0);
return;
};
ui->treeView->setModel(course);
curTaskIdx=QModelIndex();
onTask=false;
ui->actionSave->setEnabled(true);

changes.cleanChanges();
cursFile=fileName;
};
void MainWindowTask::loadMarks(const QString fileName)
{
    QDomDocument workXml;
    QFile f(fileName);
    if  (!f.open(QIODevice::ReadOnly))
    {
        QMessageBox::information( 0, "", trUtf8("Ошибка открытия файла: ") + fileName, 0,0,0);
        return;

    };
if(f.atEnd())
    {
    QMessageBox::information( 0, "", trUtf8("Ошибка открытия файла ,файл пуст: ") + fileName, 0,0,0);
    return;
    };
cursWorkFile.setFileName(f.fileName());
QString error;
int str,pos;
workXml.setContent(f.readAll(),true,&error,&str,&pos);
qDebug()<<"File parce:"<<error<<"str"<<str<<" pos"<<pos;

QDomElement root=workXml.documentElement ();
if(root.tagName()!="COURSE")
  {
        QMessageBox::information( 0, "", trUtf8("Ошибка загрузки файла: ") + fileName, 0,0,0);
        return;
  };
QDomElement fileEl=root.firstChildElement("FILE");
QString krsFile=fileEl.attribute("fileName");
loadCourseData(krsFile);//Gruzim kurs

if(cursFile!=krsFile){//Esli ne udalos po puti - ishem v toyje direktorii
    QFileInfo finf(fileEl.attribute("fileName"));
    QFileInfo cfi(cursWorkFile);
    qDebug()<<"PATH"<<cfi.dir().canonicalPath()+"/"+finf.fileName();
    krsFile=cfi.dir().canonicalPath()+"/"+finf.fileName();
    curDir=cfi.dir().canonicalPath();
    loadCourseData(krsFile);

}

QString fileN=fileEl.attribute("fileName");
qDebug()<<"KURS ZAGRUZILI";
if(cursFile!=krsFile){
    QMessageBox::information( 0, "", trUtf8("Не наеден файл курса:") + fileEl.attribute("fileName"), 0,0,0);
    fileN=getFileName(krsFile);
    loadCourseData(fileN);
    if(cursFile!=fileN)return;
    }
QFileInfo fi_kurs=QFileInfo(krsFile);
curDir=fi_kurs.absolutePath();
QDomNodeList marksElList=root.elementsByTagName("MARK"); //Оценки
qDebug()<<"Loading marks "<<marksElList.count();
for(int i=0;i<marksElList.count();i++)
 {
 int taskId=marksElList.at(i).toElement().attribute("testId").toInt();
 int mark=marksElList.at(i).toElement().attribute("mark").toInt();
 qDebug()<<"task:"<<taskId<<" mark:"<<mark;
 course->setMark(taskId,mark);
 changes.setMark(taskId,mark);
 };

qDebug()<<"Loading user prgs...";
QDomNodeList prgElList=root.elementsByTagName("USER_PRG");//Программы
for(int i=0;i<prgElList.count();i++)
 {
 int taskId=prgElList.at(i).toElement().attribute("testId").toInt();
 qDebug()<<"Tassk id"<<taskId;
 QString prg =prgElList.at(i).toElement().attribute("prg");
 QModelIndex tIdx=course->getIndexById(taskId);

 if(progChange.indexOf(taskId)==-1){

     progChange.append(taskId);
      };
 course->setUserText(taskId,prg);

 };

QDomNodeList prgElListT=root.elementsByTagName("TESTED_PRG");//Программы тестированные
for(int i=0;i<prgElListT.count();i++)
 {
 int taskId=prgElListT.at(i).toElement().attribute("testId").toInt();
 QString prg =prgElListT.at(i).toElement().attribute("prg");

 course->setUserTestedText(taskId,prg);

 };
};


void MainWindowTask::loadCourse()
{
    editRoot->hide();
    ui->splitter->setEnabled(true);
    QString dir=settings->value("Directories/Kurs","").toString();
    QDir chD(dir);
    if(!chD.exists())dir=QDir::homePath();
//    QFileDialog dialog(this,trUtf8("Открыть файл"),dir, "(*.kurs.xml *.work.xml)");
//     dialog.setAcceptMode(QFileDialog::AcceptOpen);
//     if(!dialog.exec())return;


     QString	File=QFileDialog::getOpenFileName(this, QString::fromUtf8 ("Открыть файл"), dir, "(*.kurs.xml *.work.xml)");
     QFileInfo fi(File);




     baseKursFile=fi;
     curDir=fi.absolutePath ();
     settings->setValue("Directories/Kurs", curDir);
     qDebug()<<"curDir"<<curDir;
     QString fileName=File;
     progChange.clear();
     if(fileName.right(9)==".work.xml")//Загрузка оценок и программ
     {
         loadMarks(fileName);
         return;
     }else
     cursWorkFile.setFileName("");
     loadCourseData(fileName);
     QString cText=course->courceDescr();


  if(cText.right(4)==".htm" ||cText.right(5)==".html" )
  {
      loadHtml(cText);
  }else ui->webView->setHtml(cText);
  if(isTeacher)ui->actionEdit->setEnabled(true);
  setWindowTitle(course->name()+trUtf8(" - Практикум"));

};



void MainWindowTask::setUpDown(QModelIndex index)
{
    if(!isTeacher){ui->actionup->setEnabled(false);
        ui->actionDown->setEnabled(false);
    }
    return;
    if(course->hasUpSib(index))ui->actionup->setEnabled(true);
      else ui->actionup->setEnabled(false);
      if(course->hasDownSib(index))ui->actionDown->setEnabled(true);
        else ui->actionDown->setEnabled(false);
};


void MainWindowTask::moveUp()

{
    curTaskIdx=course->moveUp(curTaskIdx);
    QModelIndex par=curTaskIdx.parent();
    ui->treeView->setCurrentIndex(curTaskIdx);
    setUpDown(curTaskIdx);


   ui->treeView->collapse(par);
   ui->treeView->expand(par);
saveBaseKurs();
};
void MainWindowTask:: moveDown()
{ ui->treeView->setCurrentIndex(curTaskIdx);

    curTaskIdx=course->moveDown(curTaskIdx);
    ui->treeView->setCurrentIndex(curTaskIdx);
    setUpDown(curTaskIdx);
  //ui->treeView->dataChanged(curTaskIdx,ui->treeView->indexAbove(curTaskIdx));
   //showText(curTaskIdx);
   // ui->treeView->update(curTaskIdx);

   ui->treeView->collapse(curTaskIdx.parent());
   ui->treeView->expand(curTaskIdx.parent());
};


void MainWindowTask::showText(const QModelIndex & index )
 {
      editRoot->hide();
    qDebug()<<"TASK IDX:"<<curTaskIdx.internalId();
    if(index.internalId()>0)setEditTaskEnabled(true);
    else setEditTaskEnabled(false);
    setUpDown(index);
    if(index==curTaskIdx)return;
//    if(onTask)
//    {
//        QMessageBox::StandardButton ans;
//        ans = QMessageBox::question(this, trUtf8("Задание"), trUtf8("Вы хотите сменить задание?"),
//                                                                                                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
//        if ((ans == QMessageBox::Cancel)||(ans == QMessageBox::No))
//        {
//            qDebug()<<"Cancel";
//        if(curTaskIdx.internalId()>0)ui->treeView->setCurrentIndex(curTaskIdx);
//            return;};

//    }
    if(curTaskIdx.internalId()>0)
    {
        markProgChange();
    }
    onTask=false;
    ui->checkTask->setEnabled(false);
    QString taskText=course->getTaskText(index);
 //ui->textBrowser->setText(taskText);



 if(taskText.right(4)==".htm" ||taskText.right(5)==".html" )
 {
     loadHtml(taskText);
 }else ui->webView->setHtml(taskText);
  qDebug()<<"TaskText:"<<course->getTaskText(index);
 curTaskIdx=index;

 if(course->isTask(curTaskIdx.internalId()) && curTaskIdx.internalId()>0 && course->taskAvailable(curTaskIdx.internalId()))
   {
     ui->do_task->setEnabled(true);
     startTask();
     ui->do_task->setEnabled(false);
      ui->actionReset->setEnabled(true);
      QString testedText=course->getUserTestedText(curTaskIdx.internalId());
      qDebug()<<"TESTED TEXT"<<testedText;
      if(testedText!="")ui->actionTested->setEnabled(true);
   }
 else {
           ui->do_task->setEnabled(false);
           ui->actionReset->setEnabled(false);
           ui->actionTested->setEnabled(false);
       };
 };

void MainWindowTask::loadHtml(QString fileName)
{
    qDebug()<<"LoadHtml"<<fileName;
    if(fileName.isEmpty())return;
    QFile inp(curDir+'/'+fileName);
    if  (!inp.open(QIODevice::ReadOnly))
    {
    QMessageBox::information( 0, "", trUtf8("Ошибка чтения: ") + fileName, 0,0,0);
    return;
    };
    QString htmlData=QString::fromUtf8(inp.readAll());
    //ui->textBrowser->setHtml(htmlData);

    ui->webView->setHtml(htmlData,QUrl("file://"+curDir+'/'+fileName));
    inp.close();

};
void MainWindowTask::startTask()
 {editRoot->hide();
    qDebug()<<"StartTask";
     if(curTaskIdx.internalId()<=0)
     {
         QMessageBox::about(NULL, trUtf8("Не выбрано задание"),trUtf8("Необходимо выбрать задание"));
         return;
     }
     if(course->csName(curTaskIdx.internalId())!=CS)
     {
         QMessageBox::about(NULL, trUtf8("Неправильное окружение"),trUtf8("Необходим ")+course->csName(curTaskIdx.internalId()));
         return;
     }

     QString progFile=course->progFile(curTaskIdx.internalId());
//interface->setTesting(loadTestAlg(course->getTaskCheck(curTaskIdx)));
QFileInfo ioDir(curDir+"/"+course->progFile(curTaskIdx.internalId()));
qDebug()<<"PRG FILE"<<course->progFile(curTaskIdx.internalId());
if(ioDir.isFile())
     {
     interface->setParam("input dir",ioDir.absoluteFilePath());
     };
     task.isps=course->Modules(curTaskIdx.internalId());
     task.name=course->getTitle(curTaskIdx.internalId());
       qDebug()<<"ISPS"<<task.isps;
       for(int i=0;i<task.isps.count();i++)
       {
      // task.Scripts.append(loadScript(course->Script(curTaskIdx.internalId(),task.isps[i])));
       QStringList t_fields=course->Fields(curTaskIdx.internalId(),task.isps[i]);
       qDebug()<<"fields"<<t_fields;
       task.fields.clear();
        for(int j=0;j<t_fields.count();j++)
            {
                qDebug()<<"Cur Dir"<<curDir;
            task.fields.insertMulti(task.isps[i],curDir+'/'+t_fields[j]);
            qDebug()<<curDir+'/'+t_fields[j];

            };
        qDebug()<<"Fields!!!!"<<task.fields;
        }
       qDebug()<<"MODULES:"<<course->Modules(curTaskIdx.internalId());
  if(!interface->startNewTask(course->Modules(curTaskIdx.internalId())))QMessageBox::about(NULL, trUtf8("Невозможно выполнить задание"),trUtf8("Нет неоходимых исполнителей"));
  if(course->getUserText(curTaskIdx.internalId())!="")
  {
      interface->setPreProgram(QVariant(course->getUserText(curTaskIdx.internalId())));
      ui->actionReset->setEnabled(true);
  }
   else
      if(!progFile.isEmpty())interface->setPreProgram(QVariant(curDir+'/'+progFile));



//qDebug()<<"Scripts "<<task.Scripts;
 ui->do_task->setEnabled(false);
 ui->checkTask->setEnabled(true);
 onTask=true;
 //ui->loadCurs->setEnabled(false);
 qDebug()<<"end load task";
 if(progChange.indexOf(curTaskIdx.internalId())==-1)progChange.append(curTaskIdx.internalId());
 };
 void MainWindowTask::checkTask()
 {
     qDebug()<<"CheckTASK";
     if(!onTask){qDebug()<<"!onTASK";return;};


markProgChange();
if(!cursWorkFile.exists()){

    QMessageBox::information( 0, "", trUtf8("Нужно завести файл рабочей тетради "), 0,0,0);
    saveCourse();
      };
course->setMark(curTaskIdx.internalId(),0);

     qDebug()<<"task"<<task.isps;
interface->startProgram(QVariant("TODO LOAD SCRIPT"),&task);
   //ui->loadCurs->setEnabled(false);

 };

 void MainWindowTask::lockControls()
{
    ui->splitter->setEnabled(false);
    ui->checkTask->setEnabled(false);
    ui->loadCurs->setEnabled(false);  
};

void MainWindowTask::unlockControls()
{
    ui->splitter->setEnabled(true);
    ui->checkTask->setEnabled(true);
    ui->loadCurs->setEnabled(true);  
};

void  MainWindowTask::setMark(int mark)
{
    ui->loadCurs->setEnabled(true);
    if(!onTask)return;
    ui->loadCurs->setEnabled(true);
  qDebug()<<"ui->cource enabled!";
    ui->splitter->setEnabled(true);
    ui->actionTested->setEnabled(true);
    qDebug()<<"ui->treeView enabled!";
    ui->checkTask->setEnabled(true);
    if((course->taskMark(curTaskIdx.internalId())<mark) &&  (course->taskMark(curTaskIdx.internalId())>0))return;

    course->setUserTestedText(curTaskIdx.internalId(),interface->getText());
    qDebug()<<"Mark:"<<mark;
    course->setMark(curTaskIdx.internalId(),mark);
    changes.setMark(curTaskIdx.internalId(),mark);
    ui->treeView->dataChanged(curTaskIdx,curTaskIdx);
    //if(mark==10)onTask=false;else onTask=true;

};
QString MainWindowTask::loadScript(QString file_name)
{
    qDebug()<<"Script file name:"<<file_name;
    if(file_name.isEmpty())return "";
    QFile file(curDir+"/"+file_name);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::about(NULL, trUtf8("Ошибка"),trUtf8("Невозможно открыть ")+curDir+"/"+file_name);
        return "";
    };
    return file.readAll();
};


void MainWindowTask::saveCourse()
{
    editRoot->hide();
   markProgChange();
    QFileDialog dialog(this,trUtf8("Сохранить изменения"),curDir, "(*.work.xml)");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if(!dialog.exec())return;
    QFileInfo fi(dialog.selectedFiles().first());
    //curDir=fi.absolutePath ();
    qDebug()<<"curDir"<<curDir;
    QString fileName=dialog.selectedFiles().first();
    QString type=fileName.right(9);
    if(type!=".work.xml")fileName+=".work.xml";
    cursWorkFile.setFileName(fileName);
    saveCourseFile();

};
void MainWindowTask::saveCourseFile()
{
    qDebug()<<"Save cource file";
    QDomDocument saveXml;

    QDomElement	crsEl=saveXml.createElement("COURSE");
    QDomElement	fileEl=saveXml.createElement("FILE");
    QDomAttr crsAtt=saveXml.createAttribute("fileName");
    crsAtt.setValue(cursFile);
    fileEl.setAttributeNode(crsAtt);

    crsEl.appendChild(fileEl);
    saveXml.appendChild(crsEl);
    QDomElement	mrksEl=saveXml.createElement("MARKS");

    //USER PROGRAMS n TESTED PROGRAMS
    for(int i=0;i<progChange.count();i++){
    QDomElement	prgEl=saveXml.createElement("USER_PRG");
    QDomAttr testIdprg=saveXml.createAttribute("testId");
    testIdprg.setValue(QString::number(progChange[i]));
    QDomAttr userPrg=saveXml.createAttribute("prg");
    userPrg.setValue(course->getUserText(progChange[i]));
    prgEl.setAttributeNode(testIdprg);
    prgEl.setAttributeNode(userPrg);
    crsEl.appendChild(prgEl);

    QDomElement	prgElT=saveXml.createElement("TESTED_PRG");
    QDomAttr testIdprgT=saveXml.createAttribute("testId");
    testIdprgT.setValue(QString::number(progChange[i]));
    QDomAttr userPrgT=saveXml.createAttribute("prg");
    userPrgT.setValue(course->getUserTestedText(progChange[i]));
    prgElT.setAttributeNode(testIdprg);
    prgElT.setAttributeNode(userPrgT);
    crsEl.appendChild(prgElT);



     }
    //END USER PROGRAMS




    QMapIterator<int, int> i(changes.marksChanged);
    while (i.hasNext()) {
        i.next();
        QDomElement mrk=saveXml.createElement("MARK");
        QDomAttr testId=saveXml.createAttribute("testId");
        testId.setValue(QString::number(i.key()));
        QDomAttr mvalue=saveXml.createAttribute("mark");
        mvalue.setValue(QString::number(i.value()));
        mrk.setAttributeNode(testId);
        mrk.setAttributeNode(mvalue);
        mrksEl.appendChild(mrk);
    }
    crsEl.appendChild(mrksEl);





          if  (!cursWorkFile.open(QIODevice::WriteOnly))
          {
          QMessageBox::information( 0, "", trUtf8("Ошибка записи: ") + cursWorkFile.fileName(), 0,0,0);
          return;
          };
    cursWorkFile.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    cursWorkFile.write(saveXml.toByteArray());
    cursWorkFile.close();

};

void MainWindowTask::markProgChange()
{

    course->setUserText(curTaskIdx,interface->getText());
    if(progChange.indexOf(curTaskIdx.internalId())==-1)progChange.append(curTaskIdx.internalId());
    if(!cursWorkFile.exists())
    {
      qDebug()<<"!cursWorkFile";
    }else saveCourseFile();

}
QString MainWindowTask::getFileName(QString fileName)
{
    QFileInfo finf(fileName);
    qDebug()<<"GET FILE!";
//    QFileDialog dialog(this,trUtf8("Открыть файл"),curDir, finf.fileName()+" *.kurs.xml");
//    dialog.setAcceptMode(QFileDialog::AcceptOpen);
//    if(!dialog.exec())return "";
//    QFileInfo fi(dialog.selectedFiles().first());



    QString	File=QFileDialog::getOpenFileName(this, QString::fromUtf8 ("Открыть файл"), curDir,  finf.fileName()+" *.kurs.xml");
    QFileInfo fi(File);


    return File;
};

void MainWindowTask::resetTask()
{
    QString progFile=course->progFile(curTaskIdx.internalId());


     if(!progFile.isEmpty())interface->setPreProgram(QVariant(curDir+'/'+progFile));
    course->setUserText(curTaskIdx,"");


};
void MainWindowTask::Close()
{
    qDebug()<<cursFile;
    if(cursFile!="")markProgChange();
    saveBaseKurs();
    close();
};
void MainWindowTask::closeEvent(QCloseEvent *event)
{
  if(!course)return;
  qDebug()<<"START CLOSE TASK WINDOW";
  if(!cursWorkFile.exists())
  {
      QMessageBox::StandardButton ans;
            ans = QMessageBox::question(this, trUtf8("Курсы"), trUtf8("Вы хотите сохранить работу?"),
                              QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
            if ((ans == QMessageBox::No))

            {
                event->accept();
                if(course)delete course;
                course=NULL;
                return;
             };
            if(ans == QMessageBox::Cancel){event->ignore();
                return;};

            saveCourse();
  }
        markProgChange();
        qDebug()<<"CLOSE TASK WINDOW";
        close();

};
void MainWindowTask::returnTested()
{
   interface->setPreProgram(QVariant(course->getUserTestedText(curTaskIdx.internalId())));
};
QString MainWindowTask::loadTestAlg(QString file_name)
{
    if(file_name.isEmpty())return "";
    QFile file(curDir+"/"+file_name);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {

        QMessageBox::about(NULL, trUtf8("Ошибка"),trUtf8("Невозможно открыть ")+curDir+"/"+file_name);
        return "";
    };
    QTextStream ts(&file);
    ts.setCodec("UTF-16LE");
    QString testalg= ts.readAll();
    file.close();

    qDebug()<<"Test alg"<<testalg;
    return testalg;
}
void MainWindowTask::aboutToQuit()
{
  close();
};
void MainWindowTask:: customContextMenuRequested(QPoint  pos)
{
editRoot->hide();
    if(!isTeacher)return;

    curTaskIdx=ui->treeView->currentIndex();
    if(curTaskIdx.internalId()==0)
    {setEditTaskEnabled(false);
        ui->actionEdit->setEnabled(true);
        ui->addDeep->setEnabled(true);
    }else setEditTaskEnabled(true);
        if(!baseKursFile.isReadable())
    {
            return;
        };
    pos.setY(pos.y()+ui->mainToolBar->height());
    customMenu.move(pos+this->pos()+ui->centralWidget->pos());
    customMenu.show();
  qDebug()<<"Menu Request!!!";
};
void MainWindowTask::addTask()
{
    qDebug()<<"Add task";
       QModelIndex par=curTaskIdx.parent();
course->addSiblingTask(curTaskIdx.internalId());
ui->treeView->collapse(par);
ui->treeView->expand(par);
};
void MainWindowTask::addDeepTask(){
    qDebug()<<"Add deep task";
       QModelIndex par=curTaskIdx.parent();
course->addDeepTask(curTaskIdx.internalId());
ui->treeView->collapse(par);
ui->treeView->expand(par);
saveBaseKurs();
if(curTaskIdx.internalId()==0)
 {
      loadCourseData(baseKursFile.absoluteFilePath());
 }
};
void MainWindowTask::deleteTask()
{
    QModelIndex par=curTaskIdx.parent();
course->removeNode(curTaskIdx.internalId());
ui->treeView->collapse(par);
ui->treeView->expand(par);
};

void MainWindowTask::saveKurs()
{
    qDebug()<<"Save Kurs teacher";
    QFile cursKursFile("../../test.kurs.xml");
    if  (!cursKursFile.open(QIODevice::WriteOnly))
    {
    QMessageBox::information( 0, "", trUtf8("Ошибка записи: ") + cursKursFile.fileName(), 0,0,0);
    return;
    };

    cursKursFile.write(course->document()->toByteArray());
    cursKursFile.close();
    ui->actionEdit->setEnabled(true);
};
void MainWindowTask::saveBaseKurs()
{
    QFile cursKursFile(baseKursFile.absoluteFilePath());
    if  (!cursKursFile.open(QIODevice::WriteOnly))
    {
    QMessageBox::information( 0, "", trUtf8("Ошибка записи: ") + cursKursFile.fileName(), 0,0,0);
    return;
    };

    cursKursFile.write(course->document()->toByteArray());
    cursKursFile.close();
}

void MainWindowTask::saveKursAs()
{
    QString dir=curDir;
    QDir chD(curDir);
    if(!chD.exists())dir=QDir::homePath ();
     QFileDialog dialog(this,trUtf8("Сохранить файл курса"),dir, "(*.kurs.xml )");
     dialog.setAcceptMode(QFileDialog::AcceptSave);

     if(!dialog.exec())return;
     QFileInfo fi(dialog.selectedFiles().first());
     baseKursFile=fi;






     saveBaseKurs();

;
};
void MainWindowTask:: setTeacher(bool mode)
{ui->menuKurs->menuAction()->setVisible(false);
    ui->actionup->setVisible(false);
     ui->actionDown->setVisible(false);
    isTeacher=mode;};
void MainWindowTask::editTask()
{
//if(curTaskIdx.internalId()==0)//ROOT
//    {
//
//    QRect rect=ui->treeView->visualRect(curTaskIdx);
//    editRoot->resize(rect.width(),rect.height());
//     editRoot->setText(course->rootText());
//    editRoot->move(rect.topLeft());
//    editRoot->show();
//    return;
//     };
//    QModelIndex par=curTaskIdx.parent();
//
//editDialog->setTitle(course->getTitle(curTaskIdx.internalId()));
//editDialog->setDesc(course->getTaskText(curTaskIdx));
//editDialog->setProgram(course->progFile(curTaskIdx.internalId()));
//QStringList isps=course->Modules(curTaskIdx.internalId());
//
//if(isps.count()>0)editDialog->setUseIsps(isps.first());
//   else editDialog->setUseIsps("");
//
//if(course->Modules(curTaskIdx.internalId()).count()>0)editDialog->setEnvs(course->Fields(curTaskIdx.internalId(),isps.first()));
// else editDialog->setEnvs(QStringList());
//editDialog->setCurDir(curDir);
// if(editDialog->exec ())
// {
//  course->setUserText(curTaskIdx.internalId(),"");
//  course->setTitle(curTaskIdx.internalId(),editDialog->getTitle());
//  course->setDesc(curTaskIdx.internalId(),editDialog->getDesc());
//  qDebug()<<"Desc"<<editDialog->getDesc();
//  course->setProgram(curTaskIdx.internalId(),editDialog->getProgram());
//  qDebug()<<"EDIT ISPS"<<editDialog->getUseIsps();
//
//
//  course->setIsps(curTaskIdx,editDialog->getUseIsps());
//  qDebug()<<"PRG"<<editDialog->getProgram();
//  if(course->Modules(curTaskIdx.internalId()).count()>0)course->setIspEnvs(curTaskIdx,course->Modules(curTaskIdx.internalId()).first(),editDialog->getEnvs());
// showText(curTaskIdx);
//  ui->treeView->collapse(par);
//  ui->treeView->expand(par);
//  qDebug()<<"Set task isps:"<<course->Modules(curTaskIdx.internalId());
//  qDebug()<<"EDIT DIALOG EXEC OK";
//  saveBaseKurs();
//  resetTask();
// };
//
};

void MainWindowTask::setEditTaskEnabled(bool flag)
{
   // ui->actionEdit->setEnabled(flag);
    ui->actionup->setEnabled(flag);
    ui->actionDown->setEnabled(flag);

    ui->actionSaveKas->setEnabled(flag);
    ui->actionAdd->setEnabled(flag);
    ui->actionRemove->setEnabled(flag);
    ui->addDeep->setEnabled(flag);
};

void MainWindowTask::newKurs()
{

// if(newDialog->exec())
// {
//    QFile newKurs(newDialog->fileName());
//
//    if  (!newKurs.open(QIODevice::WriteOnly))
//    {
//    QMessageBox::information( 0, "", trUtf8("Ошибка записи: ") + newKurs.fileName(), 0,0,0);
//    return;
//    };
//    QString toWr="<?xml version='1.0' encoding='UTF-8'?>\n";
//    newKurs.write(toWr.toUtf8());
//    toWr="<KURS xml:id=\"0\" xml:name=\""+newDialog->name()+"\">\n";
//    newKurs.write(toWr.toUtf8());
//
//    toWr=QString::fromUtf8("<T xml:id=\"1\" xml:name=\"Новое задание\">\n<DESC>Нет Описания</DESC>\n<CS>Кумир</CS>\n <ISP xml:ispname=\"Robot\">\n</ISP>\n<READY>false</READY>\n</T>\n");
//    newKurs.write(toWr.toUtf8());
//
//    toWr="</KURS>\n";
//    newKurs.write(toWr.toUtf8());
//    newKurs.close();
//    loadCourseData(newKurs.fileName());
//    baseKursFile=QFileInfo(newKurs);
//    curDir=baseKursFile.absolutePath();
//    ui->actionEdit->setEnabled(true);
// }

};
  void  MainWindowTask::createMoveMenu()
{

  }
void MainWindowTask::endRootEdit()
{
    course->setRootText(editRoot->text());
   editRoot->hide();
}
 void MainWindowTask::cancelRootEdit()
 {  editRoot->hide();
 };
void MainWindowTask::lockCheck()
{
    ui->checkTask->setDisabled(true);
}
