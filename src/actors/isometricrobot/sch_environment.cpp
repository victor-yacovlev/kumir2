#include "sch_environment.h"


extern uint qHash(const QPoint &p)
{
    QByteArray ba;
    QDataStream s(&ba, QIODevice::WriteOnly);
    s << p;
    return qHash(ba);
}

extern uint qHash(const QPair<QPoint,QPoint> &pp)
{
    QByteArray ba;
    QDataStream s(&ba, QIODevice::WriteOnly);
    s << pp;
    return qHash(ba);
}


namespace Schema {

#define LEFT_WALL 	0x1
#define RIGHT_WALL 	0x2
#define DOWN_WALL 	0x4
#define UP_WALL   	0x8

extern bool parceKumirFil(const QString &data, Environment &environment)
{
    environment.size = QSize(0,0);
    environment.painted.clear();
    environment.pointed.clear();
    environment.walls.clear();
    environment.position = QPoint();
    environment.direction = Environment::South;
    int w = 0, h = 0;
    int roboX = 0, roboY = 0;
    bool sizeRead = false;
    bool roboPosRead = false;
    QStringList lines = data.split('\n');
    foreach (QString line, lines) {
        if (line.trimmed().startsWith(";") || line.trimmed().isEmpty())
            continue;
        if (!sizeRead) {
            QStringList sVals = line.trimmed().split(QRegExp("\\s+"));
            if (sVals.count()<2) {
                qDebug() << "Error at: " << __LINE__;
                return false;
            }
            bool ok;
            w = sVals.at(0).toInt(&ok);
            if (!ok) {
                qDebug() << "Error at: " << __LINE__;
                return false;
            }
            h = sVals.at(1).toInt(&ok);
            if (!ok) {
                qDebug() << "Error at: " << __LINE__;
                return false;
            }
            sizeRead = true;
            environment.size = QSize(w,h);
        }
        else if (!roboPosRead) {
            QStringList sVals = line.trimmed().split(QRegExp("\\s+"));
            if (sVals.count()<2) {
                qDebug() << "Error at: " << __LINE__;
                return false;
            }
            bool ok;
            roboX = sVals[0].toInt(&ok);
            if (!ok) {
                qDebug() << "Error at: " << __LINE__;
                return false;
            }
            roboY = sVals[1].toUInt(&ok);
            if (!ok) {
                qDebug() << "Error at: " << __LINE__;
                return false;
            }
            if (sVals.count()>2) {
                QString v = sVals[2];
                if (v.toLower()=="north")
                    environment.direction = Environment::North;
                else if (v.toLower()=="east")
                    environment.direction = Environment::East;
                else if (v.toLower()=="west")
                    environment.direction = Environment::West;

            }
            roboPosRead = true;
            environment.position = QPoint(roboX, roboY);
        }
        else {
            QStringList sVals = line.trimmed().split(QRegExp("\\s+"));
            if (sVals.count()<4) {
                qDebug() << "Error at: " << __LINE__;
                return false;
            }
            bool ok;
            int xx = sVals[0].toInt(&ok);
            if (!ok || xx<0 || xx>=w) {
                qDebug() << "Error at: " << __LINE__;
                return false;
            }
            int yy = sVals[1].toInt(&ok);
            if (!ok || yy<0 || yy>=h) {
                qDebug() << "Error at: " << __LINE__;
                return false;
            }
            int wall = sVals[2].toInt(&ok);
            if (!ok) {
                qDebug() << "Error at: " << __LINE__;
                return false;
            }
            bool wallLeft = (wall&LEFT_WALL) == LEFT_WALL;
            bool wallDown = (wall&DOWN_WALL) == DOWN_WALL;
            bool wallRight = (wall&RIGHT_WALL) == RIGHT_WALL;
            bool wallUp = (wall&UP_WALL) == UP_WALL;
            bool pointed = false;
            if (sVals.count()>8) {
                QString v = sVals[8];
                if (v=="1") {
                    pointed = true;
                }
            }
            if (wallLeft && xx>0) {
                QPoint coord1(xx-1, yy);
                QPoint coord2(xx  , yy);
                QPair<QPoint,QPoint> pair(coord1, coord2);
                if (!environment.walls.contains(pair)) {
                    environment.walls.insert(pair);
                }
            }
            if (wallRight && xx<(w-1)) {
                QPoint coord1(xx  , yy);
                QPoint coord2(xx+1, yy);
                QPair<QPoint,QPoint> pair(coord1, coord2);
                if (!environment.walls.contains(pair)) {
                    environment.walls.insert(pair);
                }
            }
            if (wallUp && yy>0) {
                QPoint coord1(xx, yy-1);
                QPoint coord2(xx, yy);
                QPair<QPoint,QPoint> pair(coord1, coord2);
                if (!environment.walls.contains(pair)) {
                    environment.walls.insert(pair);
                }
            }
            if (wallDown && yy<(h-1)) {
                QPoint coord1(xx, yy);
                QPoint coord2(xx, yy+1);
                QPair<QPoint,QPoint> pair(coord1, coord2);
                if (!environment.walls.contains(pair)) {
                    environment.walls.insert(pair);
                }
            }
            if (sVals[3]!="0") {
                environment.painted.insert(QPoint(xx,yy));
            }
            if (pointed) {
                environment.pointed.insert(QPoint(xx,yy));
            }
        }
    }
    return true;
}

extern QString generateJSON(const Environment &env)
{
    const QString templateAll = "{\n\t\"size\": %1,\n\t\"actor\": %2,\n\t\"painted\": %3,\n\t\"pointed\": %4,\n\t\"walls\": %5\n}";
    const QString templateSize = "{ \"width\": %1, \"height\": %2 }";
    const QString templateActor = "{ \"position\": %1, \"direction\": %2 }";
    const QString templatePoint = "{ \"x\": %1, \"y\": %2 }";
    const QString templatePair = "{ \"first\": %1, \"second\": %2 }";

    const QList<QPoint> painted = env.painted.toList();
    const QList<QPoint> pointed = env.pointed.toList();
    const QList< QPair<QPoint,QPoint> > walls = env.walls.toList();

    QString paintedStr = "[";
    if (painted.isEmpty()) {
        paintedStr += "]";
    }
    else {
        paintedStr += "\n";
        for (int i=0; i<painted.size(); i++) {
            const QPoint pnt = painted[i];
            paintedStr += "\t\t"+templatePoint.arg(pnt.x()).arg(pnt.y());
            if (i<painted.size()-1) {
                paintedStr += ",";
            }
            paintedStr += "\n";
        }
        paintedStr += "\t]";
    }

    QString pointedStr = "[";
    if (pointed.isEmpty()) {
        pointedStr += "]";
    }
    else {
        pointedStr += "\n";
        for (int i=0; i<pointed.size(); i++) {
            const QPoint pnt = pointed[i];
            pointedStr += "\t\t"+templatePoint.arg(pnt.x()).arg(pnt.y());
            if (i<pointed.size()-1) {
                pointedStr += ",";
            }
            pointedStr += "\n";
        }
        pointedStr += "\t]";
    }

    QString wallsStr = "[";
    if (walls.isEmpty()) {
        wallsStr += "]";
    }
    else {
        wallsStr += "\n";
        for (int i=0; i<walls.size(); i++) {
            const QPoint pnt1 = walls[i].first;
            const QPoint pnt2 = walls[i].second;
            wallsStr += "\t\t"+templatePair.arg(
                        templatePoint.arg(pnt1.x()).arg(pnt1.y())
                        ).arg(
                        templatePoint.arg(pnt2.x()).arg(pnt2.y())
                        );
            if (i<walls.size()-1) {
                wallsStr += ",";
            }
            wallsStr += "\n";
        }
        wallsStr += "\t]";
    }
    const QString directions[] = { "\"South\"", "\"East\"", "\"North\"", "\"West\"" };
    QString actorStr = templateActor.arg(
                templatePoint.arg(env.position.x()).arg(env.position.y())
                ).arg(
                directions[env.direction]
                );
    QString sizeStr = templateSize.arg(env.size.width()).arg(env.size.height());
    QString data = templateAll
            .arg(sizeStr)
            .arg(actorStr)
            .arg(paintedStr)
            .arg(pointedStr)
            .arg(wallsStr);
    return data;
}

QList<QPoint> parcePointList(const QScriptValue &value)
{
    QList<QPoint> result;
    if (value.isArray()) {
        int len = value.property("length").toInteger();
        for (int i=0; i<len; i++) {
            QScriptValue elem = value.property(i);
            if (elem.isObject()) {
                int x = elem.property("x").toInteger();
                int y = elem.property("y").toInteger();
                result << QPoint(x,y);
            }
        }
    }
    return result;
}

QList< QPair<QPoint,QPoint> > parcePointPairList(const QScriptValue &value)
{
    QList< QPair<QPoint,QPoint> > result;
    if (value.isArray()) {
        int len = value.property("length").toInteger();
        for (int i=0; i<len; i++) {
            QScriptValue elem = value.property(i);
            if (elem.isObject()) {
                QScriptValue first = elem.property("first");
                QScriptValue second = elem.property("second");
                if (first.isObject() && second.isObject()) {
                    int x1 = first.property("x").toInteger();
                    int y1 = first.property("y").toInteger();
                    int x2 = second.property("x").toInteger();
                    int y2 = second.property("y").toInteger();
                    QPoint pnt1(x1, y1);
                    QPoint pnt2(x2, y2);
                    QPair<QPoint, QPoint> pair(pnt1, pnt2);
                    result << pair;
                }
            }
        }
    }
    return result;
}

extern bool parceJSON(const QScriptValue &value, Environment &environment)
{
    QScriptValue size = value.property("size");
    if (size.isObject()) {
        int w = size.property("width").toInteger();
        int h = size.property("height").toInteger();
        w = qMax(1, w);
        h = qMax(1, h);
        environment.size = QSize(w, h);
    }
    else {
        return false;
    }
    QScriptValue actor = value.property("actor");
    if (actor.isObject()) {
        int x = 0;
        int y = 0;
        int direction = 0;
        QScriptValue position = actor.property("position");
        if (position.isObject()) {
            x = position.property("x").toInteger();
            y = position.property("y").toInteger();
        }
        direction = Environment::South;
        QScriptValue dirv = actor.property("direction");
        if (dirv.isString()) {
            QString dir = dirv.toString().trimmed().toLower();
            if (dir=="south") direction = Environment::South;
            if (dir=="east") direction = Environment::East;
            if (dir=="north") direction = Environment::North;
            if (dir=="west") direction = Environment::West;
        }
        x = qMin(environment.size.width()-1, qMax(0, x));
        y = qMin(environment.size.height()-1, qMax(0, y));
        environment.position = QPoint(x,y);
        environment.direction = Environment::Direction (direction);
    }
    else {
        return false;
    }
    environment.painted = QSet<QPoint>::fromList( parcePointList(value.property("painted")) );
    environment.pointed = QSet<QPoint>::fromList( parcePointList(value.property("pointed")) );
    environment.walls = QSet< QPair<QPoint,QPoint> >::fromList( parcePointPairList(value.property("walls")) );
    return true;
}

extern bool parceJSON(const QString &data, Environment &environment)
{
    QScriptEngine engine;
    engine.evaluate("var data = " + data);
    QScriptValue value = engine.evaluate("data");
    if (value.isObject()) {
        return parceJSON(value, environment);
    }
    else {
        return false;
    }
}

extern bool isEqual(const Environment &a, const Environment &b)
{
    bool difference = false;
    difference |= a.size!=b.size;
    difference |= a.position!=b.position;
    difference |= a.direction!=b.direction;

    QList<QPoint> apo = a.pointed.toList();
    QList<QPoint> bpo = a.pointed.toList();

    if (apo.size()==bpo.size()) {
        for (int i=0; i<apo.size(); i++) {
            difference |= apo[i]!=bpo[i];
        }
    }
    else {
        difference |= true;
    }

    apo = a.painted.toList();
    bpo = a.painted.toList();

    if (apo.size()==bpo.size()) {
        for (int i=0; i<apo.size(); i++) {
            difference |= apo[i]!=bpo[i];
        }
    }
    else {
        difference |= true;
    }

    QList< QPair<QPoint, QPoint> > aw = a.walls.toList();
    QList< QPair<QPoint, QPoint> > bw = b.walls.toList();

    if (aw.size()==bw.size()) {
        for (int i=0; i<aw.size(); i++) {
            difference |= aw[i].first!=bw[i].first;
            difference |= aw[i].second!=bw[i].second;
        }
    }
    else {
        difference |= true;
    }
    return !difference;
}

} // namespace Schema
