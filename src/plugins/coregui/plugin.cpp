#include "plugin.h"
#include "mainwindow.h"
#include "extensionsystem/pluginmanager.h"



namespace CoreGUI {

Plugin::Plugin() :
    KPlugin()
{
    m_mainWindow = 0;
    plugin_editor = 0;
    plugin_NativeGenerator = plugin_BytecodeGenerator = 0;
    b_nosessions = false;
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

QString Plugin::initialize(const QStringList & parameters)
{
    const QStringList BlacklistedThemes = QStringList()
            << "iaora-kde" << "iaora-qt";
    const QString currentStyleName = qApp->style()->objectName().toLower();
//    qDebug() << currentStyleName;
    if (BlacklistedThemes.contains(currentStyleName)) {
        qApp->setStyle("Cleanlooks");
    }
    if (parameters.contains("classicicon",Qt::CaseInsensitive)) {
        QApplication::setWindowIcon(QIcon(QApplication::instance()->property("sharePath").toString()+"/coregui/kumir2-icon-classic.png"));
    }
    else {
        QApplication::setWindowIcon(QIcon(QApplication::instance()->property("sharePath").toString()+"/coregui/kumir2-icon.png"));
    }

    b_nosessions = parameters.contains("nosessions",Qt::CaseInsensitive);

    m_kumirStateLabel = new QLabel();
    m_genericCounterLabel = new QLabel();
    m_mainWindow = new MainWindow(this);
    plugin_editor = qobject_cast<EditorInterface*>(myDependency("Editor"));

    plugin_BytecodeGenerator = qobject_cast<GeneratorInterface*>(myDependency("KumirCodeGenerator"));
    plugin_NativeGenerator = qobject_cast<GeneratorInterface*>(myDependency("KumirNativeGenerator"));
    plugin_browser = qobject_cast<BrowserInterface*>(myDependency("Browser"));
    if (!plugin_editor)
        return "Can't load editor plugin!";
//    if (!plugin_NativeGenerator)
//        return "Can't load c-generator plugin!";
    m_terminal = new Term(0);

    connect(m_terminal, SIGNAL(message(QString)),
            m_mainWindow, SLOT(showMessage(QString)));

    QDockWidget * termWindow = m_mainWindow->addSecondaryComponent(tr("Input/Output terminal"),
                                        m_terminal,
                                        QList<QAction*>(),
                                        QList<QAction*>(),
                                        MainWindow::Terminal);
#ifndef Q_OS_MAC
    termWindow->toggleViewAction()->setShortcut(QKeySequence("F12"));
#endif
    m_kumirProgram = new KumirProgram(this);
    m_kumirProgram->setBytecodeGenerator(plugin_BytecodeGenerator);
    m_kumirProgram->setNativeGenerator(plugin_NativeGenerator);
    m_kumirProgram->setEditorPlugin(plugin_editor);
    m_kumirProgram->setTerminal(m_terminal, termWindow);

    connect(m_kumirProgram, SIGNAL(giveMeAProgram()), this, SLOT(prepareKumirProgramToRun()), Qt::DirectConnection);

    KPlugin * kumirRunner = myDependency("KumirCodeRun");
    Q_CHECK_PTR(kumirRunner);
    m_kumirProgram->setBytecodeRun(kumirRunner);

    QList<ExtensionSystem::KPlugin*> actors = loadedPlugins("Actor*");
    actors += loadedPlugins("st_funct");
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
    if (!parameters.contains("nostartpage", Qt::CaseInsensitive)) {
        const QString browserEntryPoint = QApplication::instance()->property("sharePath").toString()+"/coregui/startpage/russian/index.html";
        m_browserObjects["mainWindow"] = m_mainWindow;
        m_startPage = plugin_browser->createBrowser(QUrl::fromLocalFile(browserEntryPoint), m_browserObjects);
        m_startPage.widget->setProperty("uncloseable", true);
    }
    if (parameters.contains("notabs", Qt::CaseInsensitive)) {
        m_mainWindow->disableTabs();
    }

    return "";
}

void Plugin::changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState state)
{
    if (state==ExtensionSystem::GS_Unlocked) {
        m_kumirStateLabel->setText(tr("Editing"));
        m_mainWindow->clearMessage();
        m_mainWindow->setFocusOnCentralWidget();
    }
    else if (state==ExtensionSystem::GS_Observe) {
        m_kumirStateLabel->setText(tr("Observe"));
        m_mainWindow->showMessage(m_kumirProgram->endStatus());
        m_mainWindow->setFocusOnCentralWidget();
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

void Plugin::prepareKumirProgramToRun()
{
    plugin_editor->ensureAnalized(m_kumirProgram->documentId());
}

void Plugin::start()
{
    if (!b_nosessions && ExtensionSystem::PluginManager::instance()->showWorkspaceChooseOnLaunch()) {
        if (!ExtensionSystem::PluginManager::instance()->showWorkspaceChooseDialog()) {
            qApp->quit();
        }
    }
    else {
        ExtensionSystem::PluginManager::instance()->switchToDefaultWorkspace();
    }
    PluginManager::instance()->switchGlobalState(ExtensionSystem::GS_Unlocked);
    m_mainWindow->show();
}

void Plugin::stop()
{
    StdLib::Connector::instance()->stopListen();
}


void Plugin::saveSession() const
{
    m_mainWindow->saveSettings();
}


void Plugin::restoreSession()
{
    m_mainWindow->loadSettings();
    if (!b_nosessions)
        m_mainWindow->restoreSession();
    else
        m_mainWindow->newProgram();
}

Plugin::~Plugin()
{
    m_startPage.widget->deleteLater();
}

} // namespace CoreGUI

Q_EXPORT_PLUGIN2(CoreGui, CoreGUI::Plugin)
