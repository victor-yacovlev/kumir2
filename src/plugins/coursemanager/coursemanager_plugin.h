#ifndef COURSEMANAGER_COURSEMANAGER_PLUGIN_H
#define COURSEMANAGER_COURSEMANAGER_PLUGIN_H

#include <QtGlobal>
#include <QObject>
#include <extensionsystem/kplugin.h>
#include <interfaces/coursesinterface.h>
#include <extensionsystem/declarativesettingspage.h>
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
    QString initialize(const QStringList &arguments);
    void updateSettings();

private /*fields*/:
    AI * getActor(QString name);
    QWidget* mainWindow_;
    QAction* actionPerformCheck_;
    QAction* nextFld;
    QAction* prevFld;
    ExtensionSystem::DeclarativeSettingsPage* settingsEditorPage_;
    QMenu* courseMenu;
    QList<QMenu*> MenuList;
    MainWindowTask* MW;
    bool setTextFromFile(QString fname);
    void fixOldKumTeacherMark(QDataStream* ds);
    int isp_no,field_no;
    void selectNext(KumZadanie* task);
    KumZadanie* cur_task;

};

} // namespace CourseManager

#endif // COURSEMANAGER_COURSEMANAGER_PLUGIN_H
