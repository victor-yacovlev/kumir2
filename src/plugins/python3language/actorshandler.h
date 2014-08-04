#ifndef PYTHON3LANGUAGE_ACTORSHANDLER_H
#define PYTHON3LANGUAGE_ACTORSHANDLER_H

#include <QtCore>

#include "interfaces/actorinterface.h"

namespace Python3Language {

using namespace Shared;

class ActorsHandler : public QObject
{
    Q_OBJECT
public:
    static ActorsHandler* instance(QObject *parent = 0);

    QVariant call(int moduleId, int functionId, const QVariantList & arguments);
    void reset();

    inline int size() const { return names_.size(); }
    inline bool contains(const QString & name) const { return names_.contains(name); }
    inline QString moduleWrapper(int moduleId) const { return wrappers_[moduleId]; }
    inline QString moduleName(int moduleId) const { return names_[moduleId]; }

private Q_SLOTS:
    void handleActorSync();

private /*types*/:
    typedef QPair<ActorInterface*, ActorInterface::RecordSpecification> TypeSpec;

private /*methods*/:
    explicit ActorsHandler(QObject *parent = 0);
    void initializeActors();
    void addActor(ActorInterface * actor);

    QVariant encodeActorCustomTypeArgument(const QVariant & from) const;
    QVariant decodeActorCustomTypeValue(
            const QString &typeName,
            const QVariant & from
            ) const;

    static QString toCamelCase(const QString & s);
    static QString toPythonicName(const QString & s);
    static QString createActorWrapper(const int id,
                                      const ActorInterface * actor,
                                      const QString & camelCaseName
                                      );
    static QString createActorMethod(const int moduleId,
                                     const ActorInterface::Function & func,
                                     const QString & actorCamelCaseName
                                     );

    static QString createClassMethod(const int moduleId,
                                     const ActorInterface::Function & func,
                                     const QString & actorCamelCaseName
                                     );


    static bool isOperator(const QByteArray & asciiName);
    static QString toPythonOperator(const QByteArray & asciiName);

private /*fields*/:
    static ActorsHandler* self;    
    QVector<QString> names_;
    QVector<QString> wrappers_;
    QVector<ActorInterface*> actors_;
    QSemaphore* syncSemaphore_;
    QVariant actorCallResult_;
    QMap<QString,TypeSpec> actorCustomTypes_;
};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_ACTORSHANDLER_H
