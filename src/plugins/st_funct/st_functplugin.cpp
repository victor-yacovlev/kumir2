#include <QtCore>
#include <QtGui>

#include "st_functplugin.h"


namespace st_funct {
using namespace Shared;
  St_functPlugin::St_functPlugin()
    {
      errText="";
    };
  QList<Alg>  St_functPlugin::FuncList()
  {
      QList<Alg> tmpL;
      Alg cur_alg;
      cur_alg.kumirHeader=QObject::trUtf8("алг sin(вещ град)");
      cur_alg.id=0;
      tmpL.append(cur_alg);


      return tmpL;
  }

  QString St_functPlugin::name()
  {
  return "Standart functions";
  };
  QVariant     St_functPlugin::result()
  {
      //return Res;
  };
  void  St_functPlugin::runFunct(QString alg,QList<QVariant> params){};
  QList<QVariant> St_functPlugin::algOptResults(){};
   QWidget* St_functPlugin::mainWidget()
   {

   };
   void St_functPlugin::reset() {errText="";};
   QString St_functPlugin::errorText() const {return errText;};

   QString St_functPlugin::initialize(const QStringList &arguments)
   {
   };
   void St_functPlugin::start(){};
   void St_functPlugin::stop(){};
}

Q_EXPORT_PLUGIN(st_funct::St_functPlugin);
