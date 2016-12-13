#include "robotmodel.h"
#include "sch_environment.h"

namespace Robot25D {

Direction RobotModel::direction() const
{
    return _direction;
}

void RobotModel::turnLeft()
{
    _lastError = Robot25D::NoRuntimeError;
    if (_direction==North)
        _direction = West;
    else if (_direction==West) {
        _direction = South;
    }
    else if (_direction==South) {
        _direction = East;
    }
    else if (_direction==East) {
        _direction = North;
    }
    else {
        qFatal("Unknown direction!");
    }
    Q_EMIT robotTurnedLeft();
}

void RobotModel::turnRight()
{
    _lastError = Robot25D::NoRuntimeError;
    if (_direction==North)
        _direction = East;
    else if (_direction==East) {
        _direction = South;
    }
    else if (_direction==South) {
        _direction = West;
    }
    else if (_direction==West) {
        _direction = North;
    }
    else {
        qFatal("Unknown direction!");
    }
    Q_EMIT robotTurnedRight();
}

void RobotModel::updateCell(int x, int y, bool painted)
{
    _field[y][x].painted = painted;
    Q_EMIT cellPainted(x, y);
}

RobotCell &RobotModel::cellAt(int x, int y)
{   
    return _field[y][x];
}

Field &RobotModel::field()
{
    return _field;
}

void RobotModel::moveTo(int x, int y)
{
    Point2Di p;
    p.x = x;
    p.y = y;
    setScenePosition(p);
    Q_EMIT robotMoved();
}

void RobotModel::doPaint()
{
    _lastError = Robot25D::NoRuntimeError;
    updateCell(scenePosition().x, scenePosition().y, true);
}

bool RobotModel::goForward()
{
    _lastError = Robot25D::NoRuntimeError;
    qint16 nextX = scenePosition().x;
    qint16 nextY = scenePosition().y;
    if (direction()==North)
        nextY--;
    else if (direction()==South)
        nextY++;
    else if (direction()==West)
        nextX--;
    else
        nextX++;
    bool wall = false;
    if (nextX<0 || nextX>=_field[0].size() || nextY<0 || nextY>=_field.size())
        wall = true;
    if (direction()==North)
        wall = wall || _field[scenePosition().y][scenePosition().x].wallUp;
    if (direction()==South)
        wall = wall || _field[scenePosition().y][scenePosition().x].wallDown;
    if (direction()==West)
        wall = wall || _field[scenePosition().y][scenePosition().x].wallLeft;
    if (direction()==East)
        wall = wall || _field[scenePosition().y][scenePosition().x].wallRight;

    bool status;
    if (wall) {
        setBroken(true);
        _lastError = Robot25D::WallCollisionError;
        status = false;
    }
    else {
        _lastError = Robot25D::NoRuntimeError;
        Point2Di p;
        p.x = nextX;
        p.y = nextY;
        _scenePosition = p;
        status = true;
        Q_EMIT robotMoved();
    }
    return status;
}

bool RobotModel::isWall() const
{
    bool v = false;
    Point2Di rp = scenePosition();
    RobotCell cell = _field[rp.y][rp.x];
    if (direction()==North)
        v = cell.wallUp;
    if (direction()==South)
        v = cell.wallDown;
    if (direction()==East)
        v = cell.wallRight;
    if (direction()==West)
        v = cell.wallLeft;
    return v;
}

bool RobotModel::isPainted() const
{
    Point2Di rp = scenePosition();
    RobotCell cell = _field[rp.y][rp.x];
    bool v = cell.painted;
    return v;
}

bool RobotModel::isPainted(int x, int y) const
{
    if (y>=_field.size() || y<0)
        return false;
    if (x>=_field[y].size() || x<0)
        return false;
    RobotCell cell = _field[y][x];
    return cell.painted;
}

bool RobotModel::isPointed(int x, int y) const
{
    if (y>=_field.size() || y<0)
        return false;
    if (x>=_field[y].size() || x<0)
        return false;
    RobotCell cell = _field[y][x];
    return cell.pointed;
}

bool RobotModel::isFlagged(int x, int y) const
{
    if (y>=_field.size() || y<0)
        return false;
    if (x>=_field[y].size() || x<0)
        return false;
    const RobotCell & cell = _field[y][x];
    return cell.flag;
}

int RobotModel::sizeX() const
{
    if (_field.size()==0)
        return 0;
    return _field[0].size();
}

int RobotModel::sizeY() const
{
    return _field.size();
}

int RobotModel::positionX() const
{
    Point2Di rp = scenePosition();
    return rp.x;
}

int RobotModel::positionY() const
{
    Point2Di rp = scenePosition();
    return rp.y;
}

void RobotModel::reset()
{
    for (int i=0;i<_originalField.size(); i++)
    {
        for (int j=0;j<_originalField[i].size(); j++) {
            updateCell(j,i,_originalField[i][j].painted);
        }
    }
    setScenePosition(_originalRobotPosition);
    setDirection(_originalRobotDirection);
    setBroken(false);
    Q_EMIT fieldChanged();
}

void RobotModel::setDirection(Direction e)
{
    _direction = e;
}

Point2Di RobotModel::scenePosition() const
{
    return _scenePosition;
}

void RobotModel::setScenePosition(const Point2Di &point)
{
    _scenePosition = point;
}

bool RobotModel::isBroken() const
{
    return _broken;
}

void RobotModel::setBroken(bool v)
{
    _broken = v;
    if (v) {
        Q_EMIT robotCrashed();
    }
}

bool RobotModel::loadEnvironment(const Schema::Environment &env)
{
    _field = QVector< QVector<RobotCell> > (env.size.height(), QVector<RobotCell>(env.size.width()) );
    // Initialize empty field
    for (int y=0; y<env.size.height(); y++) {
        for (int x=0; x<env.size.width(); x++) {
            QPoint coord(x,y);
            _field[y][x].painted = env.painted.contains(coord);
            _field[y][x].pointed = env.pointed.contains(coord);
            _field[y][x].flag = env.flags.contains(coord);
            _field[y][x].wallLeft = x==0;
            _field[y][x].wallRight = x==env.size.width()-1;
            _field[y][x].wallUp = y==0;
            _field[y][x].wallDown = y==env.size.height()-1;
            _field[y][x].cellItem = NULL;
        }
    }
    // Create vertical walls
    for (int y=0; y<env.size.height(); y++) {
        for (int x=1; x<env.size.width(); x++) {
            QPoint coord1(x-1, y);
            QPoint coord2(x,   y);
            QPair<QPoint,QPoint> variant1(coord1, coord2);
            QPair<QPoint,QPoint> variant2(coord2, coord1);
            bool wall = env.walls.contains(variant1) || env.walls.contains(variant2);
            _field[y][x-1].wallRight = wall;
            _field[y][x  ].wallLeft  = wall;
        }
    }
    // Create horizontal walls
    for (int x=0; x<env.size.width(); x++) {
        for (int y=1; y<env.size.height(); y++) {
            QPoint coord1(x, y-1);
            QPoint coord2(x, y  );
            QPair<QPoint,QPoint> variant1(coord1, coord2);
            QPair<QPoint,QPoint> variant2(coord2, coord1);
            bool wall = env.walls.contains(variant1) || env.walls.contains(variant2);
            _field[y-1][x].wallDown = wall;
            _field[y  ][x].wallUp   = wall;
        }
    }
    Direction dir = South;
    if (env.direction==Schema::Environment::North)
        dir = North;
    else if (env.direction==Schema::Environment::West)
        dir = West;
    else if (env.direction==Schema::Environment::East)
        dir = East;

    // Actual creation
    for (int i=0; i<_field.size(); i++ ) {
        for (int j=0; j<_field[0].size(); j++ ) {
            _field[i][j].baseZOrder = 10.0*i+10.0*j;
        }
    }
    _originalField = _field;
    if (!_field.isEmpty()) {
        _originalRobotDirection = _direction = dir;
        _scenePosition.x = _originalRobotPosition.x = env.position.x();
        _scenePosition.y = _originalRobotPosition.y = env.position.y();
    }    
    Q_EMIT fieldChanged();
    return true;
}

Schema::Environment RobotModel::environment() const
{
    Schema::Environment env;
    env.size = QSize(_field[0].size(), _field.size());
    if (direction()==North)
        env.direction = Schema::Environment::North;
    else if (direction()==West)
        env.direction = Schema::Environment::West;
    else if (direction()==East)
        env.direction = Schema::Environment::East;
    else
        env.direction = Schema::Environment::South;
    env.position = QPoint(scenePosition().x, scenePosition().y);

    for (int y=0; y<_field.size(); y++) {
        for (int x=0; x<_field[y].size(); x++) {
            if (_field[y][x].painted) {
                env.painted.insert(QPoint(x,y));
            }
            if (_field[y][x].pointed) {
                env.pointed.insert(QPoint(x,y));
            }
        }
    }
    // Save vertical walls
    for (int y=0; y<env.size.height(); y++) {
        for (int x=1; x<env.size.width(); x++) {
            if (_field[y][x].wallLeft) {
                env.walls.insert(QPair<QPoint,QPoint>(QPoint(x-1, y),QPoint(x, y)));
            }
        }
    }
    // Save horizontal walls
    for (int x=0; x<env.size.width(); x++) {
        for (int y=1; y<env.size.height(); y++) {
            if (_field[y][x].wallUp) {
                env.walls.insert(QPair<QPoint,QPoint>(QPoint(x, y-1),QPoint(x, y)));
            }
        }
    }
    return env;
}

quint16 RobotModel::unpaintedPoints() const
{
    quint16 result = 0;
    for (int y=0; y<_field.size(); y++) {
        for (int x=0; x<_field[y].size(); x++) {
            if (_field[y][x].pointed && !_field[y][x].painted)
                result ++;
        }
    }
    return result;
}

QString RobotModel::lastError(QLocale::Language language) const
{
    if (_lastError==Robot25D::WallCollisionError) {
        if (language==QLocale::Russian)
            return QString::fromUtf8("Робот ударился об стену и разбился");
        else
            return "Wall collision caused to robot damage";
    }
    return "";
}

RobotModel::RobotModel()
    : QObject()
{
    _direction = _originalRobotDirection = South;
    _lastError = NoRuntimeError;
    _scenePosition.x = _scenePosition.y = 0;
    _originalRobotPosition.x = _originalRobotPosition.y = 0;
    _broken = false;
}

} // namespace Robot25D
