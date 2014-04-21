#ifndef ACTORINTERFACE_H
#define ACTORINTERFACE_H
#include <QtCore>
//Kumir actor interface

namespace Shared
{


/** This MUST be returned by concrete function slot */
enum EvaluationStatus {
    ES_Error, /* Evaluation error */
    ES_Async, /* Wait for sync signal */
    ES_NoResult, /* Succes, but no any result */
    ES_StackResult, /* Success, has stack value result */
    ES_RezResult, /* Success, has indirect result */
    ES_StackRezResult /* Success, has both indirect and stack value result */
};



class ActorInterface
{
public /*types*/:

    /** Scalar type */
    enum FieldType { Void, Int, Real, Bool, Char, String, RecordType };

    /** Named record-type field */
    typedef QPair<QByteArray, FieldType> Field;

    /** Record type specification */
    typedef QList<Field> Record;

    /** Named record type */
    // TODO deprecated; use RecordSpecification instead
    typedef QPair<QString,Record> CustomType;

    /** Named record type */
    struct RecordSpecification {
        QByteArray asciiName;
        QMap<QLocale::Language, QString> localizedNames;
        Record record;
    };

    /** Type list (for convience usage) */
    typedef QList<RecordSpecification> TypeList;

    /** Argument access type */
    enum ArgumentAccessType { InArgument, OutArgument, InOutArgument };

    /** Function argument specification */
    struct Argument {
        ArgumentAccessType accessType;
        FieldType type;
        RecordSpecification typeSpecification; /* optional; might be empty */
        QByteArray asciiName; /* optional; might be empty; might contain spaces */
        QMap<QLocale::Language, QString> localizedNames; /* optional */
        quint8 dimension; /* array dimension (from 1 to 3) or 0 in scalar case */

        inline Argument()
            : accessType(InArgument), type(Void), dimension(0u) {}

        inline Argument(const QByteArray & name, const FieldType t)
            : accessType(InArgument), type(t), asciiName(name), dimension(0u) {}
    };

    /** Argument list (for convience usage) */
    typedef QList<Argument> ArgumentList;

    /** Function access type */
    enum FunctionAccessType { PublicFunction, TeacherModeFunction };

    /** Function specification */
    struct Function {
        quint16 id;
        FunctionAccessType accessType;
        FieldType returnType;
        RecordSpecification returnTypeSpecification; /* in case of RT is record */
        QByteArray asciiName; /* required; might contain spaces */
        QMap<QLocale::Language, QString> localizedNames; /* optional */
        ArgumentList arguments;
    };

    /** Function list (for convience usage) */
    typedef QList<Function> FunctionList;

public /*methods*/:

    /* === Generic actor information === */

    /** Generic actor name */
    virtual QByteArray asciiModuleName() const = 0;

    /** Localized (Russian) actor name */
    virtual QString localizedModuleName(const QLocale::Language lang) const = 0;

    /** List of available functions in programming language independent format */
    inline virtual FunctionList functionList() const { return FunctionList(); }

    /** List of custom-provided scalar types */
    inline virtual TypeList typeList() const { return TypeList(); }

    /** List of actor dependencies */
    inline virtual QList<ActorInterface*> usesList() const { return QList<ActorInterface*>(); }

    /** Default parameters for template names */
    inline virtual QVariantList defaultTemplateParameters() const { return QVariantList(); }


    /* === Actor control methods === */

    /** Reset actor to initial state before execution starts */
    inline virtual void reset() {}

    /** Set/unset animation enabled flag */
    inline virtual void setAnimationEnabled(bool on) { Q_UNUSED(on); }

    /** Load actor data from external resource
     * @param source ready-to-read (i.e. in opened state abstract IO device
     */
    inline virtual void loadActorData(QIODevice * source) { Q_UNUSED(source); }


    /* === Actor initialization methods === */

    /** Connect actor's 'sync' signal to receiver's public slot
     * This method required due to Qt signals can not be declared in interfaces
     * @param receiver  QObject-derived signal receiver
     * @param methid  parameterless Qt slot created by macro SLOT(...)
     */
    inline virtual void connectSync(QObject * receiver, const char * method) {
        Q_UNUSED(receiver); Q_UNUSED(method);
    }

    /* === Actor utilities === */

    /** Converts custom-type scalar value to string representation */
    inline virtual QString customValueToString(
            const QByteArray & clazz,
            const QVariant & value /* invalid value of a list of field values */
            ) const { Q_UNUSED(clazz); Q_UNUSED(value); return QString(); }

    /** Converts string representation in custom-type value */
    inline virtual QVariant customValueFromString(
            const QByteArray & clazz,
            const QString & stringg
            ) const { Q_UNUSED(clazz); Q_UNUSED(stringg); return QVariant::Invalid; }


    /* === Method execution === */

    /** Evaluates actor method
     * The function calls actor's method with specified arguments
     * If method is aynchronous the immediately returns, so its return value
     * should be accessed after receiving 'sync' signal.
     * Otherwise function result can be retrieved immediately after call
     * @param id internal function ID
     * @param arguments arguments passed to method
     * @return state of method evaluation (see EvaluationStatus for more details)
     */
    inline virtual EvaluationStatus evaluate(quint32 id, const QVariantList & arguments) { Q_UNUSED(id); Q_UNUSED(arguments); return ES_Error; }

    /** Last evaluated method return-value */
    inline virtual QVariant result() const { return QVariant::Invalid; }

    /** Last evaluated method error text, or an empty string in case of success */
    inline virtual QString errorText() const { return QString(); }

    /** Out-Argument and InOut-Argument values left from last evaluated method */
    inline virtual QVariantList algOptResults() const { return QVariantList(); }


    /*=== Actor GUI specification === */

    /** Main actor's window (if exists) */
    inline virtual class QWidget * mainWidget() { return 0; }

    /** Control window (if exists) */
    inline virtual class QWidget * pultWidget() { return 0; }

    /** Main window icon file name (without prefix and extension) */
    inline virtual QString mainIconName() const {return QString(); }

    /** Control window icon file name (without prefix and extension) */
    inline virtual QString pultIconName() const {return QString(); }

    /** List of toplevel GUI-menus provided by actor */
    inline virtual QList<class QMenu*> moduleMenus() const { return QList<class QMenu*>(); }


    /*=== Generic C++ methods === */
    /** The destructor. Must be declared virtual */
    inline virtual ~ActorInterface() {}
};

template <class StringType>
StringType actorCanonicalName(const StringType & fullyQualifiedName)
{
    if (! fullyQualifiedName.contains("%") )
        return fullyQualifiedName;
    int pos = fullyQualifiedName.indexOf("%");
    return fullyQualifiedName.left(pos).simplified();
}

}
Q_DECLARE_INTERFACE(Shared::ActorInterface, "kumir2.Actor")
#endif // ACTORINTERFACE_H
