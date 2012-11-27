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
    Q_OBJECT
    Q_INTERFACES(Shared::ActorInterface)
public:
    St_functPlugin();
    ~St_functPlugin();

    QStringList  funcList() const ; // List of public functions;
    QString name() const;
    inline bool requiresGui() const { return false; }
    inline QList<QVariant> algOptResults() const{return QVariantList();}//optional results
    inline class QWidget* mainWidget() { return 0; }
    inline class QWidget* pultWidget() { return 0; }
    inline QString mainIconName() const { return ""; }
    inline QString pultIconName() const { return ""; }
    inline QList<class QMenu*> moduleMenus() const { return QList<class QMenu*>(); }
    inline void setAnimationEnabled(bool) {}
    inline void reset() {}
    inline QString errorText() const { return QString(); }
    inline QVariant result() const { return QVariant(); }
public slots:
    inline EvaluationStatus evaluate(quint32 , const QVariantList & ) {return ES_Error;}
protected:
    QString initialize(const QStringList &arguments);

    void start();
    void stop();
};

}


#endif
