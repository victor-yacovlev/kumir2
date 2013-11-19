#ifndef ROBOTITEM_H
#define ROBOTITEM_H

#include <QtCore>
#include <QtGui>

namespace Robot25D {

struct Point3Dr {
    qreal x;
    qreal y;
    qreal z;
};
struct Point2Di {
    qint16 x;
    qint16 y;
};

class GraphicsImageItem;


class RobotItem :
        public QThread
{
    Q_OBJECT;
    Q_ENUMS(Direction);
    Q_PROPERTY(Point3Dr position READ position WRITE setPosition);
    Q_PROPERTY(qint16 frameNo READ frameNo WRITE setFrameNo);
    Q_PROPERTY(bool broken READ broken WRITE setBroken);
    Q_PROPERTY(bool animated READ animated WRITE setAnimated);
    Q_PROPERTY(Direction direction READ direction WRITE setDirection);
    Q_PROPERTY(Point2Di scenePosition READ scenePosition WRITE setScenePosition);
    Q_PROPERTY(qreal pulse READ pulse WRITE setPulse);
    Q_PROPERTY(int speed READ speed WRITE setSpeed);
public:
    RobotItem(class RobotView *view);
    enum Direction { South, North, West, East };
    Point3Dr position() const;
    Point2Di scenePosition() const;
    bool broken() const;
    qint16 frameNo() const;
    bool animated() const;
    Direction direction() const;
    qreal pulse() const;
    inline int speed() const { return i_duration; }
    void prepareForDelete();
    void waitForAnimated();
    ~RobotItem();
public slots:
    void setSpeed(int msec);
    void setPosition(const Point3Dr &point);
    void setScenePosition(const Point2Di &point);
    void moveTo(const Point2Di &point);
    void turnLeft();
    void turnRight();
    void doPaint();
    void setBroken(bool v);
    void setFrameNo(qint16 no);
    void setAnimated(bool v);
    void setDirection(Direction v);
    void setPulse(qreal v);
signals:
    void evaluationFinished();
    void startAnimationRequest();
protected:
    QImage currentImage() const;
    virtual void timerEvent(QTimerEvent *);
protected slots:
    void handleAnimationFinished();
private:
    Direction e_direction;
    bool b_animated;
    bool b_broken;
    Point3Dr calculateRobotPosition(const Point2Di point) const;
    class RobotView *m_view;
    QList<QImage> m_movie;
    QMap<Direction, QImage> m_brokenPixmaps;
    qint16 i_currentFrame;
    qint16 i_framesPerTurn;
	Point3Dr m_moveTargetPoint;

    quint16 i_duration;
	quint16 i_currentStep;
    int i_timerId;

    Point2Di m_scenePosition;

//    QPropertyAnimation *an_animation;

    qreal r_pulse;
    enum AnimationType { NoAnimation, ChangeFrameNo, SetPosition, DoPaint } e_animationType;
    qint16 i_startFrame, i_endFrame;

    GraphicsImageItem *g_currentView;
    GraphicsImageItem *g_targetView;

    QMutex *mutex_image;
    QMutex *mutex_animation;
};

}

Q_DECLARE_METATYPE(Robot25D::Point3Dr);
Q_DECLARE_METATYPE(Robot25D::Point2Di);

#endif // ROBOTITEM_H
