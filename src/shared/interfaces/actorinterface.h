#ifndef ACTORINTERFACE_H
#define ACTORINTERFACE_H
#include <QtCore>
//Kumir actor interface

namespace Shared
{


/** This MUST be returned by concrete function slot */
enum EvaluationStatus {
    ES_Error, // Evaluation error
    ES_Async, // Wait for sync signal
    ES_NoResult, // Succes, but no any result
    ES_StackResult, // Success, has stack value result
    ES_RezResult, // Success, has indirect result
    ES_StackRezResult // Success, has both indirect and stack value result
};


class ActorInterface
{
public:
    virtual QStringList funcList() const =0; // List of public functions;
    virtual QString name() const =0;
    virtual QString mainIconName() const = 0;
    virtual QString pultIconName() const = 0;
    virtual QVariant     result() const =0;
    virtual QVariantList algOptResults() const =0;//optional results
    virtual class QWidget * mainWidget()=0;
    virtual class QWidget * pultWidget()=0;
    virtual QList<class QMenu*> moduleMenus() const = 0;
    virtual void reset() = 0;
    virtual void setAnimationEnabled(bool enabled) = 0;
    virtual EvaluationStatus evaluate(quint32 no, const QVariantList & args) = 0;
    virtual QString errorText() const = 0;
};

}
Q_DECLARE_INTERFACE(Shared::ActorInterface, "kumir2.Actor")
#endif // ACTORINTERFACE_H
