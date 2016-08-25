#ifndef ROBOTITEM_H
#define ROBOTITEM_H

#include <QtCore>
#include <QtGui>
#include "robotmodel.h"

namespace Robot25D {

struct Point3Dr {
    qreal x;
    qreal y;
    qreal z;
};


class GraphicsImageItem;


class RobotItem
        : public QThread
{
    Q_OBJECT
public:
    RobotItem(RobotModel * model, const QDir & imagesDir, class RobotView *view);
    Point3Dr position() const;    
    qint16 frameNo() const;
    bool isAnimated() const;
    qreal pulse() const;
    inline int speed() const { return _duration; }
    void prepareForDelete();
    void waitForAnimated();
    Point3Dr calculateRobotPosition(const Point2Di point) const;
    ~RobotItem();
public slots:
    void reset();
    void setSpeed(int msec);
    void setPosition(const Point3Dr &point);
    void setFrameNo(qint16 no);
    void setAnimated(bool v);
    void setPulse(qreal v);
signals:
    void evaluationFinished();
    void startAnimationRequest();
protected:
    QImage currentImage() const;
    virtual void timerEvent(QTimerEvent *);
protected slots:
    void handleAnimationFinished();
    void handleModelRobotMoved();
    void handleModelRobotCrashed();
    void handleModelRobotTurnedLeft();
    void handleModelRobotTurnedRight();
    void handleModelCellPainted(int x, int y);
private:
    bool _animated;
    class RobotView *_view;
    QList<QImage> _movie;
    QMap<Direction, QImage> _brokenPixmaps;
    qint16 _currentFrame;
    qint16 _framesPerTurn;
    Point3Dr _moveTargetPoint;

    quint16 _duration;
    quint16 _currentStep;
    int _timerId;

    qreal _pulse;
    enum AnimationType { NoAnimation, ChangeFrameNo, SetPosition, DoPaint } _animationType;
    qint16 _startFrame, _endFrame;
    Point2Di _animatedCellPosition;

    GraphicsImageItem *_currentView;
    GraphicsImageItem *_targetView;

    QMutex *_mutex_image;
    QMutex *_mutex_animation;

    RobotModel * _model;
};

}


#endif // ROBOTITEM_H
