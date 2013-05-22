#ifndef COURSESINTERFACE_H
#define COURSESINTERFACE_H

#include <QtCore>
#include <QWidget>
#include <QMenu>

namespace Shared
{

class CoursesInterface
{
    Q_ENUMS(ProgramRunStatus)
public:
    enum ProgramRunStatus {
        SuccessfullyFinished,
        UserTerminated,
        AbortedOnError
    };

    virtual QWidget* mainWindow() const = 0;
    virtual QAction* actionPerformCheck() const = 0;
    virtual QList<QMenu*> menus() const = 0;

public slots:
    virtual void setEnabled(bool value) = 0;
    virtual void setTestingResult(ProgramRunStatus status, int value) = 0;

};

}

Q_DECLARE_INTERFACE(Shared::CoursesInterface, "kumir2.CourseManager")


#endif // COURSESINTERFACE_H
