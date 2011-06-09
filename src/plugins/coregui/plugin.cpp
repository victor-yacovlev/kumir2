#include "plugin.h"
#include "mainwindow.h"
#include "switchworkspacedialog.h"


namespace CoreGUI {

Plugin::Plugin() :
    KPlugin()
{
    m_mainWindow = 0;
    plugin_editor = 0;
    plugin_CppGenerator = plugin_BytecodeGenerator = 0;
    m_workspaceDialog = 0;
}

QString Plugin::InitialTextKey = "InitialText";
QString Plugin::SessionFilesListKey = "Session/Files";
QString Plugin::SessionTabIndexKey = "Session/TabIndex";

QString Plugin::initialize(const QStringList &)
{
    QString workspaceDir;
    m_workspaceDialog = new SwitchWorkspaceDialog(mySettings());
    if (mySettings()->value(SwitchWorkspaceDialog::SkipChooseWorkspaceKey, false).toBool()) {
        workspaceDir = mySettings()->value(SwitchWorkspaceDialog::CurrentWorkspaceKey, QString(QDir::homePath()+"/Kumir/")).toString();
    }
    else {
        m_workspaceDialog->setCurrentWorkspace(mySettings()->value(SwitchWorkspaceDialog::CurrentWorkspaceKey, QString(QDir::homePath()+"/Kumir/")).toString());
        if (m_workspaceDialog->exec()==QDialog::Accepted) {
            workspaceDir = m_workspaceDialog->currentWorkspace();
        }
    }
    QDir::root().mkpath(workspaceDir);
    QDir::setCurrent(workspaceDir);
    m_mainWindow = new MainWindow(this);
    plugin_editor = qobject_cast<EditorInterface*>(myDependency("Editor"));
    plugin_CppGenerator = qobject_cast<GeneratorInterface*>(myDependency("KumirCppGenerator"));
//    plugin_BytecodeGenerator = qobject_cast<GeneratorInterface*>(myDependency("KumirBytecodeGenerator"));
    if (!plugin_editor)
        return "Can't load editor plugin!";
    if (!plugin_CppGenerator)
        return "Can't load c-generator plugin!";
    QList<ExtensionSystem::KPlugin*> actors = loadedPlugins("Actor*");
    foreach (QObject * o, actors) {
        l_plugin_actors << qobject_cast<ActorInterface*>(o);
    }
    m_mainWindow->restoreSession();
    return "";
}

void Plugin::start()
{
    m_mainWindow->show();
}

void Plugin::stop()
{
    m_mainWindow->close();
}

} // namespace CoreGUI

Q_EXPORT_PLUGIN2(CoreGui, CoreGUI::Plugin)
