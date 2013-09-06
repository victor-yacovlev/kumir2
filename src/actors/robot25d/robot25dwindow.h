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
    explicit Robot25DWindow(QWidget *parent = 0);
    ~Robot25DWindow();
    inline Robot25D::RobotView * robotView() { return m_robotView; }
    inline void lock() { group_lockedActionsDuringEvaluate->setEnabled(false); }
    inline void unlock() { group_lockedActionsDuringEvaluate->setEnabled(true); }

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
    Schema::Game m_game;
    QActionGroup * group_lockedActionsDuringEvaluate;
    Robot25D::RobotView * m_robotView;

    QPoint mousePressPosition_;

};

#endif // ROBOT25DWINDOW_H
