#ifndef SCH_ENVIRONMENT_H
#define SCH_ENVIRONMENT_H

#include <QtCore>
#include <QtScript>

extern uint qHash(const QPoint &p);
extern uint qHash(const QPair<QPoint,QPoint> &pp);

namespace Schema {

struct Environment
{
    /** Field size */
    QSize size;
    /** Set of painted cells coordinates */
    QSet<QPoint> painted;
    /** Set of pointed cells coordinates */
    QSet<QPoint> pointed;
    /** Set of walls: each wall determined by a pair of neighbour cells */
    QSet< QPair<QPoint,QPoint> > walls;
    /** Initial actor position */
    QPoint position;
    /** Initial actor direction */
    enum Direction { South=0, East=1, North=2, West=3 } direction;

};

extern bool parceKumirFil(const QString &data, Environment &environment);
extern bool parceJSON(const QString &data, Environment &environment);
extern bool parceJSON(const QScriptValue &value, Environment &environment);
extern QString generateJSON(const Environment &env);

extern bool isEqual(const Environment &a, const Environment &b);

} // namespace Schema

#endif // SCH_ENVIRONMENT_H
