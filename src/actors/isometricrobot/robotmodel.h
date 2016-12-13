#ifndef ROBOT25D_ROBOTMODEL_H
#define ROBOT25D_ROBOTMODEL_H

#include "robotcell.h"
#include "roboterrors.h"
#include "sch_environment.h"


#include <QObject>

namespace Robot25D {

class RobotView;
class RobotItem;

struct Point2Di {
    qint16 x;
    qint16 y;
};

enum Direction { South, North, West, East };
typedef QVector< QVector<RobotCell> > Field;

class RobotModel : public QObject
{
    Q_OBJECT
public:    
    Direction direction() const;
    void setDirection(Direction e);

    Point2Di scenePosition() const;
    void setScenePosition(const Point2Di &point);

    bool isBroken() const;
    void setBroken(bool v);

    QString lastError(QLocale::Language language) const;

    void turnLeft();
    void turnRight();
    void updateCell(int x, int y, bool painted);

    RobotCell& cellAt(int x, int y);
    Field& field() ;

    void moveTo(int x, int y);
    void moveRobot(int x, int y);
    void doPaint();
    bool goForward();
    bool isWall() const;
    bool isPainted() const;
    bool isPainted(int x, int y) const;
    bool isPointed(int x, int y) const;
    bool isFlagged(int x, int y) const;
    int sizeX() const;
    int sizeY() const;
    int positionX() const;
    int positionY() const;
    quint16 unpaintedPoints() const;

    void reset();

    RobotModel();
    bool loadEnvironment(const Schema::Environment &env);
    Schema::Environment environment() const;

Q_SIGNALS:
    void robotCrashed();
    void robotMoved();
    void robotTurnedLeft();
    void robotTurnedRight();
    void cellPainted(int x, int y);
    void fieldChanged();

private:
    Direction _direction;
    Point2Di _scenePosition;
    Direction _originalRobotDirection;
    Point2Di _originalRobotPosition;
    bool _broken;
    Field _field;
    Field _originalField;
    RuntimeError _lastError;
};

} // namespace Robot25D

#endif // ROBOT25D_ROBOTMODEL_H
