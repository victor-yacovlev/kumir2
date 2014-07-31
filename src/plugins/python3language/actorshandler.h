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

private /*methods*/:
    explicit ActorsHandler(QObject *parent = 0);
    void initializeActors();
    static QPair<QString, QString> generateActorNames(const ActorInterface * actor);
    static QString createActorWrapper(const int id,
                                      const ActorInterface * actor,
                                      const QString & camelCaseName
                                      );

private /*fields*/:
    static ActorsHandler* self;
    QVector<QString> names_;
    QVector<QString> wrappers_;
    QVector<ActorInterface*> actors_;
    QSemaphore* syncSemaphore_;
    QVariant actorCallResult_;
};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_ACTORSHANDLER_H
