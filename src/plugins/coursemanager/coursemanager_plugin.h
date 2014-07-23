#ifndef COURSEMANAGER_COURSEMANAGER_PLUGIN_H
#define COURSEMANAGER_COURSEMANAGER_PLUGIN_H

#include <QtGlobal>
#include <QObject>
#include <extensionsystem/kplugin.h>
#include <interfaces/coursesinterface.h>
#include <widgets/declarativesettingspage.h>
#include "extensionsystem/pluginmanager.h"
#include "shared/interfaces/guiinterface.h"
#include "shared/interfaces/actorinterface.h"
typedef Shared::GuiInterface GI;
typedef Shared::ActorInterface AI;
//#include "task/mainwindow.h"
class MainWindowTask;
class KumZadanie;
namespace CourseManager {
    

class Plugin
        : public ExtensionSystem::KPlugin
        , public Shared::CoursesInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "kumir2.CourseManager" FILE "")
#endif
    Q_INTERFACES(Shared::CoursesInterface)
public:
    explicit Plugin();
    QWidget* mainWindow() const;
    QAction* actionPerformCheck() const;
    QWidget* settingsEditorPage();
    QList<QMenu*> menus() const;
    QString getText();
    void setParam(QString paramname,QString param){};
    bool startNewTask(QStringList isps,KumZadanie* task);
    void setPreProgram(QVariant param);
    void startProgram(QVariant param,KumZadanie* task);
    void checkNext(KumZadanie* task);
    void showMW()
        {
            mainWindow()->showNormal();
        }
    inline void activateCourseFromList(QString file) { loadCource(file); }
     inline QStringList getListOfCourses() const { 
         return mySettings()->value("Courses/LastFiles","").toString().split(";",QString::SkipEmptyParts); 
     }
public slots:
    void setEnabled(bool value);
    void setTestingResult(ProgramRunStatus status, int value);
    void nextField();
    void prevField();
    void lockContrls();
private /*methods*/:
    void saveSession() const;
    void restoreSession();
    void changeCurrentDirectory(const QString & path);
    void changeGlobalState(ExtensionSystem::GlobalState old,
                           ExtensionSystem::GlobalState current);
    QString initialize(const QStringList &configurationArguments,
                       const ExtensionSystem::CommandLine & runtimeArguments);
    void updateSettings(const QStringList & keys);

private /*fields*/:
    void loadCource(QString file);
    AI * getActor(QString name);
    QWidget* mainWindow_;
    QAction* actionPerformCheck_;
    QAction* nextFld;
    QAction* prevFld;
    Widgets::DeclarativeSettingsPage* settingsEditorPage_;
    QMenu* courseMenu;
    QList<QMenu*> MenuList;
    MainWindowTask* MW;
    bool setTextFromFile(QString fname);
    void fixOldKumTeacherMark(QDataStream* ds);//Inserts into start program, techer part marker.
    int isp_no,field_no;
    void selectNext(KumZadanie* task);
    KumZadanie* cur_task;

};

} // namespace CourseManager

#endif // COURSEMANAGER_COURSEMANAGER_PLUGIN_H
