#include "robotitem.h"
#include "util.h"
#include "robotview.h"
#include "graphicsimageitem.h"
#include "cellgraphicsitem.h"
#include <iostream>

namespace Robot25D {

RobotItem::RobotItem(Robot25D::RobotModel * model, const QDir & imagesDir, RobotView *view)
    : QThread(NULL)
    , _view(view)
    , _timerId(0)
{
    _model = model;
    connect(_model, SIGNAL(robotMoved()), this, SLOT(handleModelRobotMoved()));
    connect(_model, SIGNAL(robotCrashed()), this, SLOT(handleModelRobotCrashed()));
    connect(_model, SIGNAL(robotTurnedLeft()), this, SLOT(handleModelRobotTurnedLeft()));
    connect(_model, SIGNAL(robotTurnedRight()), this, SLOT(handleModelRobotTurnedRight()));
    connect(_model, SIGNAL(cellPainted(int,int)), this, SLOT(handleModelCellPainted(int,int)));

    _animationType = NoAnimation;
    _pulse = 0.0;
    _currentStep = 0;
    _animated = false;

    const QString imagesRoot = imagesDir.absolutePath() + "/";
    _framesPerTurn = 4;
    qint16 framesCount = _framesPerTurn * 4;
    for ( int i=1; i<=framesCount; i++ ) {
        Q_ASSERT ( QFile::exists(imagesRoot+""+QString::number(i)+".png") );
        QImage frame(imagesRoot+""+QString::number(i)+".png");
        _movie.append(frame);
    }
    Q_ASSERT ( QFile::exists(imagesRoot+"broken_north.png") );
    Q_ASSERT ( QFile::exists(imagesRoot+"broken_south.png") );
    Q_ASSERT ( QFile::exists(imagesRoot+"broken_east.png") );
    Q_ASSERT ( QFile::exists(imagesRoot+"broken_west.png") );
    _brokenPixmaps[North] = QImage(imagesRoot+"broken_north.png");
    _brokenPixmaps[South] = QImage(imagesRoot+"broken_south.png");
    _brokenPixmaps[East] = QImage(imagesRoot+"broken_east.png");
    _brokenPixmaps[West] = QImage(imagesRoot+"broken_west.png");

    _currentFrame = 0;

    _pulse = 0.0;
    _animationType = NoAnimation;
    setSpeed(50);


    _currentView = new GraphicsImageItem();
    _targetView = new GraphicsImageItem();
    _currentView->setParentItem(view);
    _targetView->setParentItem(view);
    _currentView->setVisible(true);
    _targetView->setVisible(false);

    _mutex_image = new QMutex;
    _mutex_animation = new QMutex;

    reset();
}

void RobotItem::setSpeed(int msec)
{
    _duration = msec;
    if (_timerId)
        killTimer(_timerId);
    _timerId = startTimer(msec);
}

void RobotItem::waitForAnimated()
{
    forever {
        _mutex_animation->lock();
        AnimationType anim = _animationType;
        _mutex_animation->unlock();
        if (anim==NoAnimation)
            break;
        QApplication::processEvents();
        msleep(1);
    }
}

void RobotItem::handleAnimationFinished()
{
    if (_animationType==SetPosition) {
        _currentView->setVisible(false);
        GraphicsImageItem *tmp = _currentView;
        _currentView = _targetView;
        _targetView = tmp;
    }
    emit evaluationFinished();
}

void RobotItem::handleModelRobotMoved()
{
    Point2Di point = _model->scenePosition();
    Point3Dr target = calculateRobotPosition(point);
    if (_animated) {
        _mutex_animation->lock();
        _animationType = SetPosition;
        _moveTargetPoint = target;
        _mutex_animation->unlock();
    }
    else {
        setPosition(target);
        emit evaluationFinished();
    }
}

void RobotItem::handleModelRobotCrashed()
{
    setFrameNo(frameNo());
    _currentView->update();
}

Point3Dr RobotItem::calculateRobotPosition(const Point2Di point) const
{
    const qreal SceneRotationAngle = -PI/4+PI/24;
    const qreal SceneSlopeAngle = PI/6;
    const qreal CellSize = 45.0;
    qreal robotX = CellSize*point.x;
    qreal robotY = CellSize*point.y;
    qreal cellVisualWidth = CellSize/cos(SceneRotationAngle);
    qreal cellVisualHeight = cellVisualWidth*sin(SceneSlopeAngle);
    qreal robotYOffset = currentImage().height()
            - cellVisualHeight
            + (cellVisualHeight-currentImage().width()/2);
    qreal robotXOffset = (cellVisualWidth-currentImage().height())/2;
    QPointF roboPos = mapToIsometricCoordinates(robotX,robotY,0);
    robotXOffset += 8;
    qreal yy = roboPos.y()-robotYOffset;
    qreal xx = roboPos.x()-robotXOffset;
    Point3Dr r;
    r.x = xx;
    r.y = yy;
    r.z = _view->baseZOrder(point.x, point.y);
    return r;
}

QImage RobotItem::currentImage() const
{
    QImage img;
    _mutex_image->lock();
    if (_model->isBroken()) {
        img = _brokenPixmaps[_model->direction()];
    }
    else {
        img = _movie[_currentFrame];
    }
    _mutex_image->unlock();
    return img;
}


Point3Dr RobotItem::position() const
{
    Point3Dr p;
    p.x = _currentView->x();
    p.y = _currentView->y();
    p.z = _currentView->zValue();
    return p;
}


qint16 RobotItem::frameNo() const
{
    qint16 result = 0;
    _mutex_image->lock();
    result = _currentFrame;
    _mutex_image->unlock();
    return result;
}

bool RobotItem::isAnimated() const
{
    return _animated;
}


void RobotItem::setPosition(const Point3Dr &point)
{
    _currentView->setPos(point.x+_view->_offset.x(), point.y+_view->_offset.y());
    _currentView->setZValue(point.z);
}


void RobotItem::handleModelRobotTurnedLeft()
{
    _startFrame = frameNo();
    _endFrame = frameNo() + _framesPerTurn;

    if (_animated) {
        _mutex_animation->lock();
        _animationType = ChangeFrameNo;
        _mutex_animation->unlock();
     }
    else {
        setFrameNo(_endFrame);
        emit evaluationFinished();
    }
}

void RobotItem::handleModelRobotTurnedRight()
{
    int start = frameNo();
    int end = start - _framesPerTurn;

    _startFrame = start;
    _endFrame = end;

    if (_animated) {
        _mutex_animation->lock();
        _animationType = ChangeFrameNo;
        _mutex_animation->unlock();
    }
    else {
        setFrameNo(_endFrame);
        emit evaluationFinished();
    }
}

void RobotItem::handleModelCellPainted(int x, int y)
{
    if (_animated) {
        _mutex_animation->lock();
        _animationType = DoPaint;
        _animatedCellPosition.x = x;
        _animatedCellPosition.y = y;
        _mutex_animation->unlock();
    }
    else {
        _model->cellAt(x, y).paintState = _view->_grass.size()-1;
        CellGraphicsItem *item = _model->cellAt(x, y).cellItem;
        item->setBrush(_view->_grass[_model->cellAt(x, y).paintState]);
        item->update();
        emit evaluationFinished();
    }
}


void RobotItem::setFrameNo(qint16 no)
{
    _mutex_image->lock();
    qint16 prevNo = _currentFrame;
    _currentFrame = no;
    while (_currentFrame<0) {
        _currentFrame += _framesPerTurn * 4;
    }
    while (_currentFrame >= (_framesPerTurn*4)) {
        _currentFrame -= _framesPerTurn * 4;
    }
    _mutex_image->unlock();
    _currentView->setImage(currentImage());
    if (_currentFrame!=prevNo)
        _currentView->update();
}

void RobotItem::setAnimated(bool v)
{
    _mutex_animation->lock();
    _animationType = NoAnimation;
    //    an_animation->stop();
    _pulse = 0.0;
    _animated = v;
    _mutex_animation->unlock();
}

qreal RobotItem::pulse() const
{
    return _pulse;
}

void RobotItem::timerEvent(QTimerEvent *event) {
    _mutex_animation->lock();
    if (_animationType==NoAnimation) {
        event->ignore();
    }
    else {
        event->accept();
        setPulse(qreal(_currentStep) / qreal(_duration));
        _currentStep += 8;
        if (_currentStep>=_duration)
        {
            handleAnimationFinished();
            _animationType = NoAnimation;
            _pulse = 0.0;
            _currentStep = 0;
        }
    }
    _mutex_animation->unlock();
}

void RobotItem::setPulse(qreal v)
{
    _pulse = v;
    if (_animationType==ChangeFrameNo) {
        qreal start = _startFrame;
        qreal end = _endFrame;
        qreal distance = end - start;
        qreal distanceDone = distance * v;
        qreal currentValue = start + distanceDone;
        qint16 val = distance>=0.0? qCeil(currentValue) : qFloor(currentValue);
        setFrameNo(val);
    }
    else if (_animationType==SetPosition) {
        if (_pulse==0) {
            QImage cleanImage(currentImage().size(), QImage::Format_ARGB32);
            cleanImage.fill(0);
            _targetView->setImage(cleanImage);
            _targetView->setVisible(true);
            _targetView->setPos(_moveTargetPoint.x+_view->_offset.x(), _moveTargetPoint.y+_view->_offset.y());
            _targetView->setZValue(_moveTargetPoint.z);
        }
        QImage pix = currentImage();
        QImage pix1;
        QImage pix2;
        QPoint sourceGPos = _currentView->pos().toPoint();
        QPoint targetGPos = _targetView->pos().toPoint();

        QRect sourceRect(sourceGPos.x(),
                         sourceGPos.y(),
                         pix.width(),
                         pix.height());
        QRect targetRect(targetGPos.x(),
                         targetGPos.y(),
                         pix.width(),
                         pix.height());
        QPair<QImage, QImage> pixmaps = splitPixmap(pix,
                                                    sourceRect,
                                                    targetRect,
                                                    v);
        pix1 = pixmaps.first;
        pix2 = pixmaps.second;
        _currentView->setImage(pix1);
        _targetView->setImage(pix2);
        _currentView->update();
        _targetView->update();
    }
    else if (_animationType==DoPaint) {
        Point2Di pnt = _animatedCellPosition;
        quint8 curState = _model->cellAt(pnt.x, pnt.y).paintState;
        quint8 maxState = _view->_grass.size()-1;
        quint8 newState = qCeil(v * maxState);
        if (curState!=newState) {
            CellGraphicsItem *item = _model->cellAt(pnt.x, pnt.y).cellItem;
            item->setBrush(_view->_grass[newState]);
            item->update();
        }
    }
}

void RobotItem::prepareForDelete()
{
    if (_view->scene()) {
        _view->scene()->removeItem(_currentView);
        _view->scene()->removeItem(_targetView);
    }
    killTimer(_timerId);

}

RobotItem::~RobotItem()
{
    delete _currentView;
    delete _targetView;
    delete _mutex_image;
    delete _mutex_animation;

}

void RobotItem::reset()
{
    setPosition(calculateRobotPosition(_model->scenePosition()));
    setFrameNo(0);
}

}
