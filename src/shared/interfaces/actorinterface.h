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
    enum FieldType { Int, Real, Bool, Char, String };
    typedef QPair<QString, FieldType> Field;
    typedef QList<Field> Record;
    typedef QPair<QString,Record> CustomType;
    typedef QList<CustomType> TypeList;
    inline virtual QStringList funcList() const { return QStringList(); } // List of public functions;
    inline virtual TypeList typeList() const { return TypeList(); }
    inline virtual QString customValueToString(
            const CustomType & clazz,
            const QVariant & value /* invalid value of a list of field values */
            ) const { return QString(); }
    inline virtual QVariant customValueFromString(
            const CustomType & clazz,
            const QString & stringg
            ) const { return QVariant::Invalid; }
    inline virtual QString name() const { return QString(); }
    inline virtual QString mainIconName() const {return QString(); }
    inline virtual QString pultIconName() const {return QString(); }
    inline virtual QVariant     result() const { return QVariant::Invalid; }
    inline virtual QVariantList algOptResults() const { return QVariantList(); }
    inline virtual class QWidget * mainWidget() { return 0; }
    inline virtual class QWidget * pultWidget() { return 0; }
    inline virtual QList<class QMenu*> moduleMenus() const { return QList<class QMenu*>(); }
    inline virtual void reset() {}
    inline virtual void setAnimationEnabled(bool ) {}
    inline virtual EvaluationStatus evaluate(quint32 , const QVariantList & ) { return ES_Error; }
    inline virtual QString errorText() const { return QString(); }
    inline virtual void connectSync(QObject * receiver, const char * method) {}
};

}
Q_DECLARE_INTERFACE(Shared::ActorInterface, "kumir2.Actor")
#endif // ACTORINTERFACE_H
