#ifndef EDITOR_PLUGIN_H
#define EDITOR_PLUGIN_H

#include "extensionsystem/kplugin.h"
#include "interfaces/actorinterface.h"
using namespace Shared;
namespace st_funct {

class St_functPlugin
        : public ExtensionSystem::KPlugin
        , public Shared::ActorInterface

{
public:
    St_functPlugin();
    inline ~St_functPlugin(){};

    QList<Alg>  FuncList(); // List of public functions;
    QString name();
     QVariant     result();
    void   runFunct(QString alg,QList<QVariant> params);
     QList<QVariant> algOptResults();//optional results
     QWidget* mainWidget();
     void reset();
     QString errorText() const;

protected:
    QString initialize(const QStringList &arguments);
    void start();
    void stop();
private:
    QString errText;
    QVariant Res;

};

}

#endif
