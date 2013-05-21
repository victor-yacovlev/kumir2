#ifndef COURSEMANAGER_COURSEMANAGER_PLUGIN_H
#define COURSEMANAGER_COURSEMANAGER_PLUGIN_H

#include <QtGlobal>
#include <QObject>
#include <extensionsystem/kplugin.h>
#include <interfaces/coursesinterface.h>
#include <extensionsystem/declarativesettingspage.h>

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
    QList<QMenu*> Menus() const; 
       
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

};

} // namespace CourseManager

#endif // COURSEMANAGER_COURSEMANAGER_PLUGIN_H
