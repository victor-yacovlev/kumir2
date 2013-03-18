#include "robotitem.h"
#include "util.h"
#include "robotview.h"
#include "graphicsimageitem.h"
#include "cellgraphicsitem.h"


namespace Robot25D {

RobotItem::RobotItem(RobotView *view)
    : QThread(NULL)
    , m_view(view)
    , i_timerId(0)
{
    e_animationType = NoAnimation;
    r_pulse = 0.0;
    i_currentStep = 0;
    b_animated = false;
    b_broken = false;
    const QString imagesRoot =
            qApp->property("sharePath").toString()+"/actors/robot25d/";
    i_framesPerTurn = 4;
    qint16 framesCount = i_framesPerTurn * 4;
    for ( int i=1; i<=framesCount; i++ ) {
        Q_ASSERT ( QFile::exists(imagesRoot+""+QString::number(i)+".png") );
        QImage frame(imagesRoot+""+QString::number(i)+".png");
        m_movie.append(frame);
    }
    Q_ASSERT ( QFile::exists(imagesRoot+"broken_north.png") );
    Q_ASSERT ( QFile::exists(imagesRoot+"broken_south.png") );
    Q_ASSERT ( QFile::exists(imagesRoot+"broken_east.png") );
    Q_ASSERT ( QFile::exists(imagesRoot+"broken_west.png") );
    m_brokenPixmaps[North] = QImage(imagesRoot+"broken_north.png");
    m_brokenPixmaps[South] = QImage(imagesRoot+"broken_south.png");
    m_brokenPixmaps[East] = QImage(imagesRoot+"broken_east.png");
    m_brokenPixmaps[West] = QImage(imagesRoot+"broken_west.png");

    i_currentFrame = 0;
    e_direction = South;

    m_scenePosition.x = 0;
    m_scenePosition.y = 0;

    //    an_animation = new QPropertyAnimation(this, "pulse", NULL);
    //    an_animation->setDuration(250);
    //    an_animation->setStartValue(0.0);
    //    an_animation->setEndValue(1.0);
    r_pulse = 0.0;
    e_animationType = NoAnimation;
    setSpeed(50);
    //    connect (an_animation, SIGNAL(finished()), this, SLOT(handleAnimationFinished()));
    //    connect (this, SIGNAL(startAnimationRequest()), an_animation, SLOT(start()));


    g_currentView = new GraphicsImageItem();
    g_targetView = new GraphicsImageItem();
    //    m_view->scene()->addItem(g_currentView);
    //    m_view->scene()->addItem(g_targetView);
    g_currentView->setParentItem(view);
    g_targetView->setParentItem(view);
    g_currentView->setVisible(true);
    g_targetView->setVisible(false);

    mutex_image = new QMutex;
    mutex_animation = new QMutex;

}

void RobotItem::setSpeed(int msec)
{
//    qDebug() << "Set speed: " << msec;
    i_duration = msec;
    if (i_timerId)
        killTimer(i_timerId);
    i_timerId = startTimer(msec);
}

void RobotItem::waitForAnimated()
{
    forever {
        mutex_animation->lock();
        AnimationType anim = e_animationType;
        mutex_animation->unlock();
        if (anim==NoAnimation)
            break;
        msleep(1);
    }
}

void RobotItem::handleAnimationFinished()
{
//    qDebug() << "Animation finished";
    if (e_animationType==SetPosition) {
        g_currentView->setVisible(false);
        GraphicsImageItem *tmp = g_currentView;
        g_currentView = g_targetView;
        g_targetView = tmp;
    }
    emit evaluationFinished();
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
    r.z = m_view->baseZOrder(point.x, point.y);
    return r;
}

QImage RobotItem::currentImage() const
{
    QImage img;
    mutex_image->lock();
    if (b_broken) {
        img = m_brokenPixmaps[e_direction];
    }
    else {
        img = m_movie[i_currentFrame];
    }
    mutex_image->unlock();
    return img;
}


Point3Dr RobotItem::position() const
{
    Point3Dr p;
    p.x = g_currentView->x();
    p.y = g_currentView->y();
    p.z = g_currentView->zValue();
    return p;
}

Point2Di RobotItem::scenePosition() const
{
    return m_scenePosition;
}

bool RobotItem::broken() const
{
    bool result = false;
    mutex_image->lock();
    result = b_broken;
    mutex_image->unlock();
    return result;
}

qint16 RobotItem::frameNo() const
{
    qint16 result = 0;
    mutex_image->lock();
    result = i_currentFrame;
    mutex_image->unlock();
    return result;
}

bool RobotItem::animated() const
{
    return b_animated;
}

RobotItem::Direction RobotItem::direction() const
{
    return e_direction;
}

void RobotItem::setDirection(Direction v)
{
    e_direction = v;
    if (e_direction==South) {
        setFrameNo(0);
    }
    else if (e_direction==East) {
        setFrameNo(4);
    }
    else if (e_direction==North) {
        setFrameNo(8);
    }
    else if (e_direction==West) {
        setFrameNo(12);
    }
    else {
        qFatal("Unknown direction!");
    }
}

void RobotItem::setPosition(const Point3Dr &point)
{
    g_currentView->setPos(point.x+m_view->p_offset.x(), point.y+m_view->p_offset.y());
    g_currentView->setZValue(point.z);
}

void RobotItem::setScenePosition(const Point2Di &point)
{
    m_scenePosition = point;
    Point3Dr p = calculateRobotPosition(point);
    setPosition(p);
}

void RobotItem::moveTo(const Point2Di &point)
{
    //    Q_ASSERT(frameNo() % 4 == 0);
    Point3Dr target = calculateRobotPosition(point);
    m_scenePosition = point;
    if (b_animated) {
        mutex_animation->lock();
        e_animationType = SetPosition;
        m_moveTargetPoint = target;
        mutex_animation->unlock();
        //        emit startAnimationRequest();
    }
    else {
        //        setPosition(target);
//        qDebug() << "Emit command finished";
        emit evaluationFinished();
    }
}

void RobotItem::turnLeft()
{
    i_startFrame = frameNo();
    i_endFrame = frameNo() + i_framesPerTurn;

    if (e_direction==North)
        e_direction = West;
    else if (e_direction==West) {
        e_direction = South;
    }
    else if (e_direction==South) {
        e_direction = East;
    }
    else if (e_direction==East) {
        e_direction = North;
    }
    else {
        qFatal("Unknown direction!");
    }
    if (b_animated) {
        mutex_animation->lock();
        e_animationType = ChangeFrameNo;
        mutex_animation->unlock();
        //        emit startAnimationRequest();
    }
    else {
        //        setFrameNo(i_endFrame);
//        qDebug() << "Emit command finished";
        emit evaluationFinished();
    }
}

void RobotItem::turnRight()
{
    int start = frameNo();
    int end = start - i_framesPerTurn;
    //    i_startFrame = frameNo();
    //    i_endFrame = frameNo() - i_framesPerTurn;

    i_startFrame = start;
    i_endFrame = end;
    if (e_direction==North)
        e_direction = East;
    else if (e_direction==East) {
        e_direction = South;
    }
    else if (e_direction==South) {
        e_direction = West;
    }
    else if (e_direction==West) {
        e_direction = North;
    }
    else {
        qFatal("Unknown direction!");
    }
    if (b_animated) {
        mutex_animation->lock();
        e_animationType = ChangeFrameNo;
        mutex_animation->unlock();
        //        emit startAnimationRequest();
    }
    else {
        //        setFrameNo(i_endFrame);
//        qDebug() << "Emit command finished";
        emit evaluationFinished();
    }
}

void RobotItem::doPaint()
{
    Point2Di pnt = m_scenePosition;
    m_view->m_field[pnt.y][pnt.x].painted = true;
    if (b_animated) {
        mutex_animation->lock();
        e_animationType = DoPaint;
        mutex_animation->unlock();
        //        emit startAnimationRequest();
    }
    else {
        m_view->m_field[pnt.y][pnt.x].paintState = m_view->lbr_grass.size()-1;
        CellGraphicsItem *item = m_view->m_field[pnt.y][pnt.x].cellItem;
        item->setBrush(m_view->lbr_grass[m_view->m_field[pnt.y][pnt.x].paintState]);
        item->update();
        //        QGraphicsPolygonItem *item = m_view->m_field[pnt.y][pnt.x].cellItem;
        //        item->setBrush(m_view->lbr_grass.last());
//        qDebug() << "Emit command finished";
        emit evaluationFinished();
    }
}

void RobotItem::setBroken(bool v)
{
    mutex_image->lock();
    b_broken = v;
    mutex_image->unlock();
    setFrameNo(frameNo());
    g_currentView->update();
}

void RobotItem::setFrameNo(qint16 no)
{
    //    qDebug() << "Set frame no" << no;
    mutex_image->lock();
    qint16 prevNo = i_currentFrame;
    i_currentFrame = no;
    while (i_currentFrame<0) {
        i_currentFrame += i_framesPerTurn * 4;
    }
    while (i_currentFrame >= (i_framesPerTurn*4)) {
        i_currentFrame -= i_framesPerTurn * 4;
    }
    mutex_image->unlock();
    g_currentView->setImage(currentImage());
    if (i_currentFrame!=prevNo)
        g_currentView->update();
}

void RobotItem::setAnimated(bool v)
{
    mutex_animation->lock();
    e_animationType = NoAnimation;
    //    an_animation->stop();
    r_pulse = 0.0;
    b_animated = v;
    mutex_animation->unlock();
}

qreal RobotItem::pulse() const
{
    return r_pulse;
}

void RobotItem::timerEvent(QTimerEvent *event) {
    mutex_animation->lock();
    if (e_animationType==NoAnimation) {
        event->ignore();
    }
    else {
        event->accept();
        setPulse(qreal(i_currentStep) / qreal(i_duration));
        i_currentStep += 8;
        if (i_currentStep>=i_duration)
        {
            handleAnimationFinished();
            e_animationType = NoAnimation;
            r_pulse = 0.0;
            i_currentStep = 0;
        }
    }
    mutex_animation->unlock();
}

void RobotItem::setPulse(qreal v)
{
    //    qDebug() << "Set pulse: " << v;
    r_pulse = v;
    if (e_animationType==ChangeFrameNo) {
        qreal start = i_startFrame;
        qreal end = i_endFrame;
        qreal distance = end - start;
        qreal distanceDone = distance * v;
        qreal currentValue = start + distanceDone;
        qint16 val = distance>=0.0? qCeil(currentValue) : qFloor(currentValue);
        setFrameNo(val);
    }
    else if (e_animationType==SetPosition) {
        if (r_pulse==0) {
            QImage cleanImage(currentImage().size(), QImage::Format_ARGB32);
            cleanImage.fill(0);
            g_targetView->setImage(cleanImage);
            g_targetView->setVisible(true);
            g_targetView->setPos(m_moveTargetPoint.x+m_view->p_offset.x(), m_moveTargetPoint.y+m_view->p_offset.y());
            g_targetView->setZValue(m_moveTargetPoint.z);
        }
        QImage pix = currentImage();
        QImage pix1;
        QImage pix2;
        QPoint sourceGPos = g_currentView->pos().toPoint();
        QPoint targetGPos = g_targetView->pos().toPoint();

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
        g_currentView->setImage(pix1);
        g_targetView->setImage(pix2);
        g_currentView->update();
        g_targetView->update();
    }
    else if (e_animationType==DoPaint) {
        Point2Di pnt = m_scenePosition;
        quint8 curState = m_view->m_field[pnt.y][pnt.x].paintState;
        quint8 maxState = m_view->lbr_grass.size()-1;
        quint8 newState = qCeil(v * maxState);
        if (curState!=newState) {
            CellGraphicsItem *item = m_view->m_field[pnt.y][pnt.x].cellItem;
            item->setBrush(m_view->lbr_grass[newState]);
            item->update();
        }
    }
}

void RobotItem::prepareForDelete()
{
    if (m_view->scene()) {
        m_view->scene()->removeItem(g_currentView);
        m_view->scene()->removeItem(g_targetView);
    }
    killTimer(i_timerId);

}

RobotItem::~RobotItem()
{
    delete g_currentView;
    delete g_targetView;
    delete mutex_image;
    delete mutex_animation;

}

}
