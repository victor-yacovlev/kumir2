#include "coursemanager_plugin.h"

namespace CourseManager {

Plugin::Plugin()
    : ExtensionSystem::KPlugin()
    , mainWindow_(nullptr)
    , actionPerformCheck_(nullptr)
    , settingsEditorPage_(nullptr)
{
    courseMenu=new QMenu(trUtf8("Практикум"));
    MenuList.append(courseMenu);
}
QList<QMenu*>  Plugin::Menus()const
{
    
    return MenuList; 
}; 

QWidget* Plugin::mainWindow() const
{
    return mainWindow_;
}

QAction* Plugin::actionPerformCheck() const
{
    return actionPerformCheck_;
}

QWidget* Plugin::settingsEditorPage()
{
    if (!settingsEditorPage_) {
        typedef ExtensionSystem::DeclarativeSettingsPage::Entry Entry;
        typedef QMap<QString,Entry> EntryMap;

        EntryMap entries; // TODO implement me

        settingsEditorPage_ = new ExtensionSystem::DeclarativeSettingsPage(
                    pluginSpec().name,      // Plugin name
                    tr("Course Manager"),   // Title in setting window
                    mySettings(),           // Settings object
                    entries                 // A map of configurable items
                    );
    }
    return settingsEditorPage_;
}

void Plugin::setEnabled(bool value)
{

}

void Plugin::setTestingResult(ProgramRunStatus status, int value)
{

}

void Plugin::saveSession() const
{

}

void Plugin::restoreSession()
{

}

void Plugin::changeCurrentDirectory(const QString &path)
{

}

void Plugin::changeGlobalState(ExtensionSystem::GlobalState old,
                               ExtensionSystem::GlobalState current)
{

}

QString Plugin::initialize(const QStringList &arguments)
{
    qRegisterMetaType<Shared::CoursesInterface::ProgramRunStatus>
            ("CourseManager.ProgramRunStatus");
    QString error;

    return error;
}

void Plugin::updateSettings()
{
    if (settingsEditorPage_) {
        settingsEditorPage_->setSettingsObject(mySettings());
    }
}


} // namespace CourseManager

Q_EXPORT_PLUGIN2(CourseManager, CourseManager::Plugin)
