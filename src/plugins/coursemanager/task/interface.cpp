#include "interface.h"

void ControlInterface::start(QString csName)
{
    CSname=csName;
    w.setCS(CSName());
   w.setup();
   // w.show();


};
void ControlInterface::setCSmode(int mode)
{
    if(mode==5)w.setTeacher(true);
    if(mode==-1)
    {
        qDebug()<<"LockCheck";
        w.lockCheck();
    };
};
void ControlInterface::setCSinterface(CSInterface* interface)
{
    csInterface=interface;
    w.setInterface(interface);
};
QString ControlInterface::ispName()
{
    return "";
};
void ControlInterface::checkFinished(int mark)
 {
    Mark=mark;qDebug()<<"Get Mark"<<mark;
    w.setMark(mark);
 }
Q_EXPORT_PLUGIN2(controlInterface,ControlInterface);
