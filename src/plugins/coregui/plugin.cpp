#include "plugin.h"
#include "mainwindow.h"
#include "switchworkspacedialog.h"
#include "extensionsystem/pluginmanager.h"



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
QString Plugin::RecentFileKey = "History/FileDialog";
QString Plugin::RecentFilesKey = "History/RecentFiles";
QString Plugin::MainWindowGeometryKey = "Geometry/MainWindow";
QString Plugin::MainWindowStateKey = "State/MainWindow";
QString Plugin::DockVisibleKey = "DockWindow/Visible";
QString Plugin::DockFloatingKey = "DockWindow/Floating";
QString Plugin::DockGeometryKey = "DockWindow/Geometry";
QString Plugin::DockSideKey = "DockWindow/Side";

QString Plugin::initialize(const QStringList &)
{
    QApplication::setWindowIcon(QIcon(QApplication::instance()->property("sharePath").toString()+"/coregui/kumir2-icon.png"));
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
    ExtensionSystem::PluginManager::instance()->changeWorkingDirectory(workspaceDir);
    m_mainWindow = new MainWindow(this);
    plugin_editor = qobject_cast<EditorInterface*>(myDependency("Editor"));
    plugin_CppGenerator = qobject_cast<GeneratorInterface*>(myDependency("KumirCppGenerator"));
    plugin_browser = qobject_cast<BrowserInterface*>(myDependency("Browser"));
//    plugin_BytecodeGenerator = qobject_cast<GeneratorInterface*>(myDependency("KumirBytecodeGenerator"));
    if (!plugin_editor)
        return "Can't load editor plugin!";
    if (!plugin_CppGenerator)
        return "Can't load c-generator plugin!";
    m_terminal = new Terminal(0);
    m_mainWindow->addSecondaryComponent(tr("Input/Output terminal"),
                                        m_terminal,
                                        QList<QAction*>(),
                                        QList<QAction*>(),
                                        MainWindow::Terminal);
    QList<ExtensionSystem::KPlugin*> actors = loadedPlugins("Actor*");
    foreach (QObject * o, actors) {
        ActorInterface * actor = qobject_cast<ActorInterface*>(o);
        l_plugin_actors << actor;
        if (actor->mainWidget()) {
            QWidget * actorWidget = actor->mainWidget();
            QList<QAction*> actorMenus = actor->menuActions();
            bool priv = o->property("privilegedActor").toBool();
            m_mainWindow->addSecondaryComponent(actor->name(),
                                                actorWidget,
                                                QList<QAction*>(),
                                                actorMenus,
                                                priv? MainWindow::StandardActor : MainWindow::WorldActor);
        }
    }
    m_kumirProgram = new KumirProgram(this);
    m_kumirProgram->setBytecodeGenerator(plugin_BytecodeGenerator);
    m_kumirProgram->setCppGenerator(plugin_CppGenerator);
    m_kumirProgram->setTerminal(m_terminal);
    const QString browserEntryPoint = QApplication::instance()->property("sharePath").toString()+"/coregui/startpage/russian/index.html";
    m_browserObjects["mainWindow"] = m_mainWindow;
    m_startPage = plugin_browser->createBrowser(QUrl::fromLocalFile(browserEntryPoint), m_browserObjects);
    m_startPage.widget->setProperty("uncloseable", true);
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

Plugin::~Plugin()
{
    m_startPage.widget->deleteLater();
}

} // namespace CoreGUI

Q_EXPORT_PLUGIN2(CoreGui, CoreGUI::Plugin)
