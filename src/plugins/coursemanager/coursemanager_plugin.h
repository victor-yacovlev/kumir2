#ifndef COURSEMANAGER_COURSEMANAGER_PLUGIN_H
#define COURSEMANAGER_COURSEMANAGER_PLUGIN_H

#include <QtGlobal>
#include <QObject>
#include <extensionsystem/kplugin.h>
#include <interfaces/coursesinterface.h>
#include <extensionsystem/declarativesettingspage.h>
#include "extensionsystem/pluginmanager.h"
#include "shared/interfaces/guiinterface.h"
typedef Shared::GuiInterface GI;
//#include "task/mainwindow.h"
class MainWindowTask;
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
    int startNewTask(QStringList isps){return 0;};
    void setPreProgram(QVariant param){qDebug()<<"setPreProgram! NOT IMPLEMENTED!!";};
    void startProgram(QVariant param){qDebug()<<"startProgram! NOT IMPLEMENTED!!";};
public slots:
    void setEnabled(bool value);
    void setTestingResult(ProgramRunStatus status, int value);

private /*methods*/:
    void saveSession() const;
    void restoreSession();
    void changeCurrentDirectory(const QString & path);
    void changeGlobalState(ExtensionSystem::GlobalState old,
                           ExtensionSystem::GlobalState current);
    QString initialize(const QStringList &arguments);
    void updateSettings();

private /*fields*/:
    QWidget* mainWindow_;
    QAction* actionPerformCheck_;
    ExtensionSystem::DeclarativeSettingsPage* settingsEditorPage_;
    QMenu* courseMenu;
    QList<QMenu*> MenuList;
    MainWindowTask* MW;

};

} // namespace CourseManager

#endif // COURSEMANAGER_COURSEMANAGER_PLUGIN_H
