#ifndef INTERFACE_H
#define INTERFACE_H
#include "taskControlInterface.h"
#include <mainwindow.h>
class ControlInterface: public QObject,public  taskControlInterface
{
 Q_OBJECT
 Q_INTERFACES(taskControlInterface)

 public:

     void start(QString csName);
     void setCSmode(int mode);
     void setCSinterface(class CSInterface* interface);
     inline void setWindowGeometry(QRect rect){w.setGeometry(rect);}
     inline void show(){w.showNormal();}
     void checkFinished(int mark);
     inline KumZadanie* Task(){return &w.task;}
     inline QString Isp(int no){return w.task.Isp(no);}
     QString ispName();

     inline QString CSName(){return CSname;}

    MainWindowTask w;

public:
    inline class CSInterface*  Interface(){return csInterface;}
private:
    QString CSname;
     class CSInterface* csInterface;
     int Mark;


};
#endif // INTERFACE_H
