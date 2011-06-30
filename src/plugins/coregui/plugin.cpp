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
    plugin_NativeGenerator = plugin_BytecodeGenerator = 0;
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
    m_kumirStateLabel = new QLabel();
    m_genericCounterLabel = new QLabel();
    ExtensionSystem::PluginManager::instance()->changeWorkingDirectory(workspaceDir);
    m_mainWindow = new MainWindow(this);
    plugin_editor = qobject_cast<EditorInterface*>(myDependency("Editor"));

    plugin_BytecodeGenerator = qobject_cast<GeneratorInterface*>(myDependency("KumirCodeGenerator"));
    plugin_NativeGenerator = qobject_cast<GeneratorInterface*>(myDependency("KumirNativeGenerator"));
    plugin_browser = qobject_cast<BrowserInterface*>(myDependency("Browser"));
    if (!plugin_editor)
        return "Can't load editor plugin!";
    if (!plugin_NativeGenerator)
        return "Can't load c-generator plugin!";
    m_terminal = new Terminal(0);
    QDockWidget * termWindow = m_mainWindow->addSecondaryComponent(tr("Input/Output terminal"),
                                        m_terminal,
                                        QList<QAction*>(),
                                        QList<QAction*>(),
                                        MainWindow::Terminal);
    m_kumirProgram = new KumirProgram(this);
    m_kumirProgram->setBytecodeGenerator(plugin_BytecodeGenerator);
    m_kumirProgram->setNativeGenerator(plugin_NativeGenerator);
    m_kumirProgram->setEditorPlugin(plugin_editor);
    m_kumirProgram->setTerminal(m_terminal, termWindow);

    KPlugin * kumirRunner = myDependency("KumirCodeRun");
    Q_CHECK_PTR(kumirRunner);
    m_kumirProgram->setBytecodeRun(kumirRunner);

    QList<ExtensionSystem::KPlugin*> actors = loadedPlugins("Actor*");
    foreach (ExtensionSystem::KPlugin* o, actors) {
        ActorInterface * actor = qobject_cast<ActorInterface*>(o);
        l_plugin_actors << actor;
        QDockWidget * w = 0;
        if (actor->mainWidget()) {
            QWidget * actorWidget = actor->mainWidget();
            QList<QAction*> actorMenus = actor->menuActions();
            bool priv = o->property("privilegedActor").toBool();
            w = m_mainWindow->addSecondaryComponent(actor->name(),
                                                actorWidget,
                                                QList<QAction*>(),
                                                actorMenus,
                                                priv? MainWindow::StandardActor : MainWindow::WorldActor);
        }
        m_kumirProgram->addActor(o, w);
    }
    const QString browserEntryPoint = QApplication::instance()->property("sharePath").toString()+"/coregui/startpage/russian/index.html";
    m_browserObjects["mainWindow"] = m_mainWindow;
    m_startPage = plugin_browser->createBrowser(QUrl::fromLocalFile(browserEntryPoint), m_browserObjects);
    m_startPage.widget->setProperty("uncloseable", true);
    m_mainWindow->restoreSession();
    return "";
}

void Plugin::changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState state)
{
    if (state==ExtensionSystem::GS_Unlocked) {
        m_kumirStateLabel->setText(tr("Editing"));
        m_mainWindow->clearMessage();
    }
    else if (state==ExtensionSystem::GS_Observe) {
        m_kumirStateLabel->setText(tr("Observe"));
        m_mainWindow->showMessage(m_kumirProgram->endStatus());
    }
    else if (state==ExtensionSystem::GS_Running) {
        m_kumirStateLabel->setText(tr("Running"));
        m_mainWindow->clearMessage();
    }
    else if (state==ExtensionSystem::GS_Pause) {
        m_kumirStateLabel->setText(tr("Pause"));
    }
    else if (state==ExtensionSystem::GS_Input) {
        m_kumirStateLabel->setText(tr("Pause"));
    }

    m_kumirProgram->switchGlobalState(old, state);


}

void Plugin::start()
{
    PluginManager::instance()->switchGlobalState(ExtensionSystem::GS_Unlocked);
    m_mainWindow->show();
}

void Plugin::stop()
{
    StdLib::Connector::instance()->stopListen();
}

Plugin::~Plugin()
{
    m_startPage.widget->deleteLater();
}

} // namespace CoreGUI

Q_EXPORT_PLUGIN2(CoreGui, CoreGUI::Plugin)
