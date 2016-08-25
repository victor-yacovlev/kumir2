#ifndef ROBOT25DWINDOW_H
#define ROBOT25DWINDOW_H

#include <QtCore>
#include <QtGui>

#include "robotview.h"
#include "sch_game.h"

class Robot25DWindow :
        public QGraphicsView
{
    Q_OBJECT

public:
    explicit Robot25DWindow(Robot25D::RobotModel * model, const QDir & imagesDir, QWidget *parent = 0);
    ~Robot25DWindow();
    inline Robot25D::RobotView * robotView() { return _robotView; }
    inline void lock() { _groupLockedActionsDuringEvaluate->setEnabled(false); }
    inline void unlock() { _groupLockedActionsDuringEvaluate->setEnabled(true); }

    QSize sizeHint() const;

    void loadGame(const QString &fileName);
    void loadEnvironment(const QString &fileName);

signals:
    void resizeRequest(const QSize & sz);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private slots:
    void handleLoadAction();
    void handleNextAction();
    void handlePrevAction();
    void setTaskIndex(int index);

private:
    Schema::Game _game;
    QActionGroup * _groupLockedActionsDuringEvaluate;
    Robot25D::RobotView * _robotView;

    QPoint _mousePressPosition;
    Robot25D::RobotModel *_model;

};

#endif // ROBOT25DWINDOW_H
