#ifndef ROBOT25DWINDOW_H
#define ROBOT25DWINDOW_H

#include <QtCore>
#include <QtGui>

#include "robotview.h"
#include "sch_game.h"

namespace Ui {
    class Robot25DWindow;
}

class Robot25DWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit Robot25DWindow(QWidget *parent = 0);
    ~Robot25DWindow();
    inline Robot25D::RobotView * robotView() { return m_robotView; }
    inline void lock() { group_lockedActionsDuringEvaluate->setEnabled(false); }
    inline void unlock() { group_lockedActionsDuringEvaluate->setEnabled(true); }

    void loadGame(const QString &fileName);
    void loadEnvironment(const QString &fileName);
    void statusMessage(const QString & msg);
private slots:
    void handleLoadAction();
    void handleNextAction();
    void handlePrevAction();
    void setTaskIndex(int index);

private:
    Schema::Game m_game;
    Ui::Robot25DWindow *ui;
    QActionGroup * group_lockedActionsDuringEvaluate;
    Robot25D::RobotView * m_robotView;

};

#endif // ROBOT25DWINDOW_H
