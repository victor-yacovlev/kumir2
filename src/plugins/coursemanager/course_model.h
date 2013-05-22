#ifndef COURSE_MODEL_H
#define COURSE_MODEL_H
#include <QAbstractItemModel>
#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QFont>
#include <QDomDocument>
#include <QIcon>
class KumTask
{
public:
    KumTask(int id,QString title,QString desc,int chapter_id)
    {
        Id=id;
        Title=title;
        Desc=desc;
        Chapter_id=chapter_id;

    }
    int getId(){return Id;}
    QString getTitle(){return Title;}
    QStringList getIsps(){return ispsNeeded;}
    QStringList getEnvFiles(){return envFiles;}
    int Chapter(){return Chapter_id;}
    QString Description(){return Desc;}
    void appendEnvFile(const QString envFile){envFiles.append(envFile);};
    int next(){return nextId;};
    void setNext(int id){nextId=id;}
    void addSubTask(KumTask* task)
    {
        subTasks.append(task);
    };
     int subTasksCount()
     {
         return subTasks.count();
     };
private:
    int Id;
    QString Title;
    QStringList ispsNeeded;
    QStringList envFiles;
    QString Desc;
    int Chapter_id;
    QList<int> dependes;
    int nextId;
    QString controlScript;

    QList<KumTask*> subTasks;
};

class courseChanges
{
public:
    courseChanges(){};
    void cleanChanges(){marksChanged.clear();};
    void setMark(int testId,int newMark)
    {
        marksChanged.insert(testId,newMark);
    };
    QMap<int,int> marksChanged;
};


class courseModel:
        public QAbstractItemModel
{
    Q_OBJECT
public:
       courseModel( )
           : QAbstractItemModel() {
           itemFont=QFont("Helvetica [Cronyx]");
#ifdef Q_OS_WIN32
           markIcons.append(QIcon(":/out_stand.png"));
           markIcons.append(QIcon(":/1.png"));
           markIcons.append(QIcon(":/2.png"));
           markIcons.append(QIcon(":/3.png"));
           markIcons.append(QIcon(":/4.png"));
           markIcons.append(QIcon(":/5.png"));
           markIcons.append(QIcon(":/6.png"));
           markIcons.append(QIcon(":/7.png"));
           markIcons.append(QIcon(":/8.png"));
           markIcons.append(QIcon(":/9.png"));
           markIcons.append(QIcon(":/10.png"));
           markIcons.append(QIcon(":/m.png"));

           markIcons.append(QIcon(":/folder_close.png"));
           markIcons.append(QIcon(":/folder_1.png"));
           markIcons.append(QIcon(":/folder_2.png"));
           markIcons.append(QIcon(":/folder_3.png"));
           markIcons.append(QIcon(":/folder_4.png"));
           markIcons.append(QIcon(":/folder_5.png"));
           markIcons.append(QIcon(":/folder_6.png"));
           markIcons.append(QIcon(":/folder_7.png"));
           markIcons.append(QIcon(":/folder_8.png"));
           markIcons.append(QIcon(":/folder_9.png"));
           markIcons.append(QIcon(":/folder_10.png"));
           markIcons.append(QIcon(":/folder_open.png"));
           return;
#endif
           markIcons.append(QIcon(":/out_stand.svg"));
           markIcons.append(QIcon(":/1.png"));
           markIcons.append(QIcon(":/2.png"));
           markIcons.append(QIcon(":/3.png"));
           markIcons.append(QIcon(":/4.png"));
           markIcons.append(QIcon(":/5.png"));
           markIcons.append(QIcon(":/6.png"));
           markIcons.append(QIcon(":/7.png"));
           markIcons.append(QIcon(":/8.png"));
           markIcons.append(QIcon(":/9.png"));
           markIcons.append(QIcon(":/10.png"));
           markIcons.append(QIcon(":/m.png"));

           markIcons.append(QIcon(":/folder_close.svg"));
           markIcons.append(QIcon(":/folder_1.png  "));
           markIcons.append(QIcon(":/folder_2.svg"));
           markIcons.append(QIcon(":/folder_3.svg"));
           markIcons.append(QIcon(":/folder_4.svg"));
           markIcons.append(QIcon(":/folder_5.svg"));
           markIcons.append(QIcon(":/folder_6.svg"));
           markIcons.append(QIcon(":/folder_7.svg"));
           markIcons.append(QIcon(":/folder_8.svg"));
           markIcons.append(QIcon(":/folder_9.svg"));
           markIcons.append(QIcon(":/folder_10.svg"));
           markIcons.append(QIcon(":/folder_open.svg"));
           isTeacher=false;


           };

       int rowCount(const QModelIndex &parent = QModelIndex()) const;
           QVariant data(const QModelIndex &index, int role) const;
           QVariant headerData(int section, Qt::Orientation orientation,
                               int role = Qt::DisplayRole) const;
           QModelIndex index(int row, int column, const QModelIndex &parent) const;
           QModelIndex parent(const QModelIndex &child) const;
           int columnCount(const QModelIndex &parent)const ;
           Qt::ItemFlags flags(const QModelIndex &index) const;


 public:
         int loadCourse(QString fileName);
         QString getTaskText(QModelIndex index);
          QString getTaskCheck(QModelIndex index);
          QString courceDescr()
          {
              QDomElement titleEl=root.firstChildElement("DESC");
              if(titleEl.isNull())return "";

              return titleEl.text();
          }

         QModelIndexList indexes()
         {
             return persistentIndexList();
         }
         void setIspEnvs(QModelIndex index,QString isp,QStringList Envs);
         void setIsps(QModelIndex index,QStringList isp);
         void setUserText(QModelIndex index,const QString &text)
         {
             QDomNode el=nodeById(index.internalId(),root);

             QDomElement userTextEl=el.firstChildElement("USER_PRG");
             if(userTextEl.isNull()) //USER PRG пока нет - создаем
             {
                 userTextEl=courceXml.createElement("USER_PRG");
                 el.appendChild(userTextEl);
             }
             //заполняем
             QDomAttr userPrg=courceXml.createAttribute("prg");
             userPrg.setValue(text);
             userTextEl.setAttributeNode(userPrg);
             qDebug()<<"SET USER PRG"<<index.internalId()<<" test "<<text;
         }
         void setUserText(int id,const QString &text)
         {
             QDomNode el=nodeById(id,root);

             QDomElement userTextEl=el.firstChildElement("USER_PRG");
             if(userTextEl.isNull()) //USER PRG пока нет - создаем
             {
                 userTextEl=courceXml.createElement("USER_PRG");
                 el.appendChild(userTextEl);
             }
             //заполняем
             QDomAttr userPrg=courceXml.createAttribute("prg");
             userPrg.setValue(text);
             userTextEl.setAttributeNode(userPrg);
             qDebug()<<"SET USER PRG"<<id << " "<<text;
         }
         void setUserTestedText(int id,const QString &text)
         {
             QDomNode el=nodeById(id,root);

             QDomElement userTextEl=el.firstChildElement("TESTED_PRG");
             if(userTextEl.isNull()) //USER PRG пока нет - создаем
             {
                 userTextEl=courceXml.createElement("TESTED_PRG");
                 el.appendChild(userTextEl);
             }
             //заполняем
             QDomAttr userPrg=courceXml.createAttribute("prg");
             userPrg.setValue(text);
             userTextEl.setAttributeNode(userPrg);
             qDebug()<<"SET TESTED PRG"<<id;
         }

         QString getUserText(int curTaskId)
         {
            QDomNode  node=nodeById(curTaskId,root);
            QDomElement userTextEl=node.firstChildElement("USER_PRG");
            if(userTextEl.isNull()) {qDebug()<<"Null user Prg"<<curTaskId;return "";};
            QString userPrg=userTextEl.attribute("prg");
          //  qDebug()<<"User program>>>>>>>:"<<userPrg;
             return userPrg;
         };

         QString getUserTestedText(int curTaskId)
         {
            QDomNode  node=nodeById(curTaskId,root);
            QDomElement userTextEl=node.firstChildElement("TESTED_PRG");
            if(userTextEl.isNull()) {qDebug()<<"Null user  tested Prg"<<curTaskId;return "";};
            QString userPrg=userTextEl.attribute("prg");
          //  qDebug()<<"User Tested program program>>>>>>>:"<<userPrg;
             return userPrg;
         };
         QString getTitle(int curTaskId)
         {
            QDomNode  node=nodeById(curTaskId,root);

             return node.toElement().attribute("name","");
         };

          QString name()
          {
              return root.toElement().attribute("name","");
          }


         void setTitle(int curTaskId,QString title)
         {
             QDomNode el=nodeById(curTaskId,root);

             el.toElement().setAttribute("name",title);

         };

          void setTag(int curTaskId,QString data,QString tag)
          {
              QDomNode  node=nodeById(curTaskId,root);
              if(node.isNull())
              {
                  qDebug()<<"Set NODE NO NODE";
                  return;
              };
              QDomElement readyEl=node.firstChildElement(tag);
              if (readyEl.isNull())
              {
                  qDebug()<<"Create NEW"<<tag;
                  QDomElement markEl=courceXml.createElement(tag);
                  node.appendChild(markEl);
                  readyEl=node.firstChildElement(tag);

              };
              QDomText text=courceXml.createTextNode(data);
              for(int i=0;i<readyEl.childNodes().count();i++)
                 {
                  qDebug()<<"SWAP"<<tag;
                  if(readyEl.childNodes().at(i).isText())
                    {
                     QDomNode elText=readyEl.childNodes().at(i);
                     readyEl.replaceChild(text,elText);
                     break;
                    }
                 }
             if(readyEl.childNodes().count()==0)
              {
                 readyEl.appendChild(text);
              }

          }

         void setDesc(int curTaskId,QString desc)
         {
      setTag(curTaskId,desc,"DESC");

         };
         void setProgram(int curTaskId,QString prg)
         {
      setTag(curTaskId,prg,"PROGRAM");

         };

         QModelIndex getIndexById(int id)
         {
            QDomNode  node=nodeById(id,root);
            if(node.isNull())return index(0,0,QModelIndex());
            int col=node.columnNumber ();
            int row=node.lineNumber();
            if(id!=0)return index(row,col,getIndexById(idByNode(node.parentNode())));
              else return index(row,col,QModelIndex());

         };
         QString csName(int index);
         QString progFile(int index);
         QStringList Modules(int index);
         QString Script(int index,QString isp);
         QStringList Fields(int index,QString isp);
         int taskMark(int id)const
         {


         return taskMark(nodeById(id,root));

         };
         int taskMark(QDomNode  node)const
         {

          if(node.isNull())return 0;
          QDomElement readyEl=node.firstChildElement("MARK");
          if (readyEl.isNull()){
              return 0;
              qDebug()<<"Node is null;";
                };
          qDebug()<<"MRK:"<<readyEl.text();
         return readyEl.text().toInt();

         };
         void setParMark(QDomElement  pnode)
         {
             QDomNodeList childs=pnode.elementsByTagName("T");
             int min_m=11;
             bool hasNull=false;
             for(int i=0;i<childs.count();i++)
             {
                 int tmark=taskMark(childs.at(i));
                if(tmark==0)hasNull=true;
                if(min_m>taskMark(childs.at(i))&& tmark>0 )min_m=taskMark(childs.at(i));
             }
             if(min_m<11 and hasNull)min_m=11;
             if(min_m>0)setMark(idByNode(pnode),min_m);


         }
         void setMark(int id,int mark)
         {
           //  if(id==0)return;
          QDomNode  node=nodeById(id,root);
          if(node.isNull())return;
          QDomElement readyEl=node.firstChildElement("MARK");
            QDomText text=courceXml.createTextNode(QString::number(mark));
          if (readyEl.isNull())
          {
              QDomElement markEl=courceXml.createElement("MARK");
              node.appendChild(markEl);
              readyEl=node.firstChildElement("MARK");
              readyEl.appendChild(text);
              qDebug()<<"Is null";
              setParMark(node.parentNode().toElement());
              return;
          };

          for(int i=0;i<readyEl.childNodes().count();i++)
             {
              if(readyEl.childNodes().at(i).isText())
                {
                 QDomNode elText=readyEl.childNodes().at(i);
                 readyEl.replaceChild(text,elText);
                 break;
                }
             }
          if(readyEl.childNodes().count()==0)
          {
              readyEl.appendChild(text);
          }

          qDebug()<<"Task "<<id<<" mark"<<taskMark(node)<<" set"<<mark;
            setParMark(node.parentNode().toElement());
         };

         QStringList getScripts(int id);
         bool isTask(int id) const
         {
             QDomNode task=nodeById(id,root);
             if(task.toElement().attribute("root")=="true")return false;
             return true;
         };
         int getMaxId()
         {
             int max=0;
            QDomNodeList list=courceXml.elementsByTagName("T");
            for(int i=0;i<list.count();i++)
            {

               int depId=list.at(i).toElement().attribute("id","").toInt();
               if(depId>max)max=depId;
            }

            return max+10;
         }
         int setChildsId(QDomNode par,int first_id)
         {
            QDomNodeList Childs=par.childNodes();
            int cur_off=0;
            for(int i=0;i<Childs.count();i++)
            {
                cur_off++;
                if(Childs.at(i).nodeName()=="T")
                {
                  Childs.at(i).toElement().setAttribute("id",first_id+cur_off);
                  QDomNodeList dep_list=Childs.at(i).toElement().elementsByTagName ("DEPEND");
                  qDebug()<<"Dep count"<<dep_list.count();
                  Childs.at(i).toElement().removeChild(Childs.at(i).firstChildElement ("DEPEND"));
                  for(int j=0;j<dep_list.count();j++)
                  {

                      qDebug()<<"dep remove";
                  };

                  cur_off=cur_off+setChildsId(Childs.at(i),cur_off+1);
                }
            }
            return cur_off;
         };

         void addSiblingTask(int id)
         {
          QDomNode task=nodeById(id,root);
          QDomNode copy=task.cloneNode();
           int copyid=getMaxId();
          copy.toElement().setAttribute("id",copyid);
          setChildsId(copy,copyid+1);

           task.parentNode().toElement().insertAfter(copy,task);

          setMark(copyid,0);
          cash.clear();
         // buildCash();
         };
         void addDeepTask(int id)
         {
          if(id==0)
             {
              QDomDocument baseNode;
              baseNode.setContent(QString::fromUtf8("<T xml:id=\"2\" xml:name=\"Новое задание\">\n<DESC>Нет Описания</DESC>\n<CS>Кумир</CS>\n <ISP xml:ispname=\"Robot\">\n</ISP>\n<READY>false</READY>\n</T>\n"));
              QDomElement newNode=baseNode.firstChildElement();
              QDomNode copy=newNode.cloneNode(true);
              QDomNode impCopy=courceXml.importNode(copy,true);
              int copyid=getMaxId();

           impCopy.toElement().setAttribute("xml:id",copyid);


           root.toElement().insertAfter(impCopy,root.lastChild());
           setMark(copyid,0);

           cash.clear();
           buildCash();
           emit dataChanged(QModelIndex(),createIndex(rowCount()+1,1,copyid));
              return;
             };
          QDomNode task=nodeById(id,root);
          QDomNode copy=task.cloneNode(true);
          QDomNodeList taskChilds=task.childNodes();
           int copyid=getMaxId();

        copy.toElement().setAttribute("id",copyid);



          for(int i=0;i<taskChilds.count();i++)
           {
              QDomNode child=task.childNodes().item(i);
              QDomNode chCopy=child.cloneNode();
           if(chCopy.nodeName()!="T")copy.toElement().appendChild(chCopy);
          // qDebug()<<"Node app"<<chCopy.nodeName();
           };
          task.toElement().insertBefore(copy,task.firstChild());
          setMark(copyid,0);
         cash.clear();
         buildCash();
         };
         void removeNode(int id)
         {
            QDomNode task=nodeById(id,root);
            task.parentNode().removeChild(task);
            cash.clear();
            buildCash();
         };

         bool  taskAvailable(int id) const
         {
             return taskAvailable(nodeById(id,root));
         }
         bool taskAvailable(QDomNode task) const
         {

             if(task.isNull())return false;
             QDomElement depEl=task.firstChildElement("DEPEND");
             while(!depEl.isNull())
             {
                 QDomElement idEl=depEl.firstChildElement("ID");
                 QDomElement markEl=depEl.firstChildElement("MIN");
                 QDomElement markMaxEl=depEl.firstChildElement("MAX");
                 if(idEl.isNull() || markEl.isNull() )
                       {
                     depEl=depEl.nextSiblingElement("DEPEND");
                     continue;
                       }
                 int depId=idEl.text().toInt();
                 QDomNode depNode=nodeById(depId,root);//Узел от которого зависим

                 int needMark=markEl.text().toInt();
                 int maxMark=99;
                 if(!markMaxEl.isNull())maxMark=markMaxEl.text().toInt();
                 //qDebug()<<"Need mark"<<needMark<<"Task Mark"<<taskMark(depId);
                 if(taskMark(depNode)<needMark ||taskMark(depNode)>maxMark )
                 {
                    // qDebug()<<"task id:"<<id<<" unavailable";
                     return false;
                 }
                 depEl=depEl.nextSiblingElement("DEPEND");
             };
             return true;
         }

         QDomDocument* document(){return &courceXml;};
         void setTeacher(bool flag)
         {
           isTeacher=flag;
         };
         bool hasUpSib(QModelIndex &index)
         {
             QDomNode el=nodeById(index.internalId(),root);
            return !el.previousSiblingElement("T").isNull();
         };
         bool hasDownSib(QModelIndex &index)
         {
             QDomNode el=nodeById(index.internalId(),root);
            return !el.nextSiblingElement("T").isNull();
         };
        QModelIndex moveUp(QModelIndex &index)
         {
             if(!hasUpSib(index))return index;
             QDomNode el=nodeById(index.internalId(),root);
            QDomNode per=el.previousSiblingElement("T");
            el.parentNode().toElement().insertBefore(el,per);
            cash.clear();
            buildCash();
            return createMyIndex(index.row()-1,index.column(),index.parent());
         };
        QModelIndex moveDown(QModelIndex &index)
         {
             if(!hasDownSib(index))return index;
             QDomNode el=nodeById(index.internalId(),root);
            QDomNode per=el.nextSiblingElement("T");
            el.parentNode().toElement().insertAfter(el,per);
            cash.clear();
            buildCash();
            return createMyIndex(index.row()+1,index.column(),index.parent());
         };
        QString rootText()
        {
          return root.attribute("name");
        }
        void setRootText(QString text)
        {
           root.setAttribute("name",text);
        };
        void buildCash()
        {
          QDomNodeList list =courceXml.elementsByTagName("T");
          for(int i=0;i<list.count();i++)
          {
          cash.insert(list.at(i).toElement().attribute("id").toInt(),list.at(i));
          }
        };

private:
         QIcon iconByMark(int mark,bool isFolder)const;
         QDomNode nodeByRowColumn(int row,int column,QDomNode* parent) const;
         QDomNode nodeById(int id,QDomNode parent) const;
         QModelIndex createMyIndex(int row,int column,QModelIndex parent) const;
         int idByNode(QDomNode node) const
         {
             bool ok;
           int id=node.toElement().attribute("id","").toInt(&ok);
           if(!ok)return -1;
           return id;
         }
         int subTasks(QDomNode parent) const
         {
             QDomNodeList childs=parent.childNodes();
             int count=0;
             for(int i=0;i<childs.count();i++)
             {
                if(childs.at(i).nodeName()=="T")
                 {
                    count++;

                }
             }
            // qDebug()<<"Childs"<<count;
             return count;
         }
       int domRow(QDomNode &child) const;
       QString courseFileName;
       int taskCount;
       QString courseName;
       QString courseDesc;
       QFont itemFont;
       QDomDocument courceXml;
       QDomElement root;
       QList<QIcon> markIcons;
       bool isTeacher;
     QHash<int,QDomNode> cash;
};

#endif // COURSE_MODEL_H
