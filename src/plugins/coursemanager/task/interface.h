#ifndef INTERFACE_H
#define INTERFACE_H
#include "../taskControlInterface.h"
#include <mainwindow.h>
class CSInterface;
class ControlInterface: public QObject,public  taskControlInterface
{
 Q_OBJECT
 Q_INTERFACES(taskControlInterface)

 public:

     void start(QString csName);
     void setCSmode(int mode);
     void setCSinterface(CSInterface* interface);
     void setWindowGeometry(QRect rect){w.setGeometry(rect);};
     void show(){w.showNormal();}
     void checkFinished(int mark);
     KumZadanie* Task(){return &w.task;};
     QString Isp(int no){return w.task.Isp(no);};
     QString ispName();
  //   QString setParam(QString param,QVariant value){};
     QString CSName(){return CSname;};

    MainWindowTask w;

public:
    CSInterface*  Interface(){return csInterface;};
private:
    QString CSname;
     CSInterface* csInterface;
     int Mark;


};
#endif // INTERFACE_H
