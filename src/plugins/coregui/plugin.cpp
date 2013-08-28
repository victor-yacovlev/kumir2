#include "tabwidgetelement.h"
#include "switchworkspacedialog.h"
#include "plugin.h"
#include "mainwindow.h"
#include "extensionsystem/pluginmanager.h"
#include "widgets/secondarywindow.h"
#include "debuggerview.h"
#include "ui_mainwindow.h"
#include "statusbar.h"
#include "tabwidget.h"
#ifdef Q_OS_MACX
#include "mac-fixes.h"
#endif

namespace CoreGUI {

using namespace Shared;

Plugin::Plugin() :
    KPlugin()
{
    mainWindow_ = 0;
    plugin_editor = 0;
    plugin_NativeGenerator = plugin_BytecodeGenerator = 0;
    sessionsDisableFlag_ = false;
    kumirProgram_ = 0;
    startPage_ = 0;
    helpViewer_ = 0;
    courseManager_ = 0;
    helpWindow_ = 0;
}

QString Plugin::InitialTextKey = "InitialText";
QString Plugin::SessionFilesListKey = "Session/Files";
QString Plugin::SessionTabIndexKey = "Session/TabIndex";
QString Plugin::RecentFileKey = "History/FileDialog";
QString Plugin::RecentFilesKey = "History/RecentFiles";
QString Plugin::MainWindowGeometryKey = "Geometry/MainWindow";
QString Plugin::MainWindowShowConsoleKey = "State/ConsoleToggle";
QString Plugin::MainWindowStateKey = "State/MainWindow";
QString Plugin::MainWindowSplitterStateKey = "State/MainWindowSplitter";
QString Plugin::DockVisibleKey = "DockWindow/Visible";
QString Plugin::DockFloatingKey = "DockWindow/Floating";
QString Plugin::DockGeometryKey = "DockWindow/Geometry";
QString Plugin::DockSideKey = "DockWindow/Side";



QString Plugin::initialize(const QStringList & parameters, const ExtensionSystem::CommandLine &)
{

    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
    qRegisterMetaType<KumFile::Data>("KumirFile.Data");
    qRegisterMetaType<Shared::GuiInterface::ProgramSourceText::Language>
            ("Gui.ProgramSourceText.Language");
    qRegisterMetaType<Shared::GuiInterface::ProgramSourceText>
            ("Gui.ProgramSourceText");


    const QStringList BlacklistedThemes = QStringList()
            << "iaorakde" << "iaoraqt" << "iaora";
    const QString currentStyleName = qApp->style()->objectName().toLower();
    if (BlacklistedThemes.contains(currentStyleName)) {
        qApp->setStyle("Cleanlooks");
    }

    QString iconSuffix;
    for (int i=0; i<parameters.count(); i++) {
        const QString param = parameters[i];
        if (param.startsWith("icon=")) {
            iconSuffix = "-"+param.mid(5);
        }
    }
    QApplication::setWindowIcon(QIcon(QApplication::instance()->property("sharePath").toString()+"/coregui/kumir2-icon"+iconSuffix+".png"));


    sessionsDisableFlag_ = parameters.contains("nosessions",Qt::CaseInsensitive);

    m_kumirStateLabel = new QLabel();
    m_genericCounterLabel = new QLabel();
    mainWindow_ = new MainWindow(this);
#ifdef Q_OS_MACX
   // void * mac_mainWindow = (class NSView*)(m_mainWindow->winId());
    //MacFixes::setLionFullscreenButton(mac_mainWindow);
#endif

    plugin_editor = PluginManager::instance()->findPlugin<EditorInterface>();


    plugin_BytecodeGenerator = qobject_cast<GeneratorInterface*>(myDependency("KumirCodeGenerator"));
    plugin_NativeGenerator = qobject_cast<GeneratorInterface*>(myDependency("KumirNativeGenerator"));
    plugin_browser = qobject_cast<BrowserInterface*>(myDependency("Browser"));
    if (!plugin_editor)
        return "Can't load editor plugin!";
    terminal_ = new Term(mainWindow_);
    mainWindow_->consoleAndCourcesPlace_->addPersistentWidget(terminal_);

    connect(terminal_, SIGNAL(showWindowRequest()),
            mainWindow_, SLOT(ensureBottomVisible()));



    const QString qtcreatorIconsPath = QApplication::instance()->property("sharePath")
            .toString() + "/icons/from_qtcreator/";

    const QString showConsoleIconPath = qtcreatorIconsPath + "category_core.png";
    const QString clearConsoleIconPath = qtcreatorIconsPath + "clean_pane_small.png";

    const QIcon showConsoleIcon = QIcon(showConsoleIconPath);
    const QIcon cleanConsoleIcon = QIcon(clearConsoleIconPath);


    QToolButton * btnShowConsole = new QToolButton(mainWindow_);
    mainWindow_->ui->actionShow_Console_Pane->setIcon(showConsoleIcon);
    btnShowConsole->setDefaultAction(mainWindow_->ui->actionShow_Console_Pane);
    btnShowConsole->setToolTip(mainWindow_->ui->actionShow_Console_Pane->text());
    mainWindow_->statusBar_->addButtonToLeft(btnShowConsole);
    QToolButton * btnSaveTerm = new QToolButton(mainWindow_);
    btnSaveTerm->setPopupMode(QToolButton::InstantPopup);
    QMenu * menuSaveTerm = new QMenu(btnSaveTerm);
    btnSaveTerm->setToolTip("Save console output");
    btnSaveTerm->setMenu(menuSaveTerm);
    btnSaveTerm->setIcon(terminal_->actionSaveLast()->icon());
    menuSaveTerm->addAction(terminal_->actionSaveLast());
    menuSaveTerm->addAction(terminal_->actionSaveAll());
//    mainWindow_->statusBar()->insertWidget(0, btnSaveTerm);
    mainWindow_->statusBar_->addButtonToLeft(btnSaveTerm);
    QToolButton * btnClearTerm = new QToolButton(mainWindow_);
    terminal_->actionClear()->setIcon(cleanConsoleIcon);
    btnClearTerm->setDefaultAction(terminal_->actionClear());
//    mainWindow_->statusBar()->insertWidget(1, btnClearTerm);
    mainWindow_->statusBar_->addButtonToLeft(btnClearTerm);
    QToolButton * btnEditTerm = nullptr;
    if (!parameters.contains("notabs",Qt::CaseInsensitive)) {
        btnEditTerm = new QToolButton(mainWindow_);
        btnEditTerm->setDefaultAction(terminal_->actionEditLast());
        mainWindow_->statusBar_->addButtonToLeft(btnEditTerm);
    }
#ifdef Q_OS_MAC
    static const char * statusBarButtonCSS =
            "QToolButton {"
            "   border: 0;"
            "}"
            "QToolButton:checked, QToolButton:pressed {"
            "   border: 1px solid gray;"
            "   border-radius: 5px;"
            "   background-color: lightgray;"
            "}"
            ;
    btnShowConsole->setStyleSheet(statusBarButtonCSS);
    btnSaveTerm->setStyleSheet(statusBarButtonCSS);
    btnClearTerm->setStyleSheet(statusBarButtonCSS);
    if (btnEditTerm) {
        btnEditTerm->setStyleSheet(statusBarButtonCSS);
    }
#endif


    kumirProgram_ = new KumirProgram(this);
    kumirProgram_->setTerminal(terminal_, 0);


    connect(kumirProgram_, SIGNAL(giveMeAProgram()), this, SLOT(prepareKumirProgramToRun()), Qt::DirectConnection);

    helpViewer_ = new DocBookViewer::DocBookView(mainWindow_);
    helpViewer_->updateSettings(mySettings(), "HelpViewer");
    static const QString helpPath =
            QApplication::instance()->property("sharePath").toString() +
            "/userdocs/";

    helpViewer_->addDocument(QUrl::fromLocalFile(helpPath + "default.xml"));


    helpWindow_ = new Widgets::SecondaryWindow(
                helpViewer_,
                mainWindow_->helpPlace_,
                mainWindow_,
                mySettings(),
                "HelpViewerWindow");    
    helpWindow_->setWindowTitle(tr("Help"));
    helpWindow_->toggleViewAction()->setShortcut(QKeySequence("F1"));
    connect(mainWindow_->ui->actionUsage, SIGNAL(triggered()),
            helpWindow_->toggleViewAction(), SLOT(trigger()));
    connect(helpWindow_->toggleViewAction(), SIGNAL(toggled(bool)),
            mainWindow_->ui->actionUsage, SLOT(setChecked(bool)));    


    courseManager_ = ExtensionSystem::PluginManager::instance()
            ->findPlugin<Shared::CoursesInterface>();

    if (courseManager_) {
        foreach (QMenu* menu, courseManager_->menus()) {
            mainWindow_->ui->menubar->insertMenu(mainWindow_->ui->menuHelp->menuAction(), menu);
        }

        Widgets::SecondaryWindow * coursesWindow = new Widgets::SecondaryWindow(
                    courseManager_->mainWindow(),
                    mainWindow_->consoleAndCourcesPlace_,
                    mainWindow_,
                    mySettings(),
                    "CoursesWindow"
                    );

        mainWindow_->ui->menuWindow->addAction(coursesWindow->toggleViewAction());
        mainWindow_->gr_otherActions->addAction(coursesWindow->toggleViewAction());
    }

    KPlugin * kumirRunner = ExtensionSystem::PluginManager::instance()
            ->findKPlugin<RunInterface>();
    plugin_kumirCodeRun = qobject_cast<RunInterface*>(kumirRunner);

    QList<ExtensionSystem::KPlugin*> actors = loadedPlugins("Actor*");
    actors += loadedPlugins("st_funct");
    foreach (ExtensionSystem::KPlugin* o, actors) {
        ActorInterface * actor = qobject_cast<ActorInterface*>(o);
        l_plugin_actors << actor;
        QWidget * w = 0;
        const QString actorHelpFile = helpPath + o->pluginSpec().name + ".xml";
        if (QFile(actorHelpFile).exists()) {
            helpViewer_->addDocument(QUrl::fromLocalFile(actorHelpFile));
        }
        if (actor->mainWidget()) {
            QWidget * actorWidget = actor->mainWidget();
            QList<QMenu*> actorMenus = actor->moduleMenus();
            bool priv = o->property("privilegedActor").toBool();
//            w = m_mainWindow->addSecondaryComponent(actor->name(),
//                                                actorWidget,
//                                                QList<QAction*>(),
//                                                actorMenus,
//                                                priv? MainWindow::StandardActor : MainWindow::WorldActor);
            Widgets::SecondaryWindow * actorWindow = new Widgets::SecondaryWindow(
                        actorWidget,
                        mainWindow_->actorsPlace_,
                        mainWindow_,
                        o->pluginSettings(),
                        o->pluginSpec().name
                        );

            const QString actorName = actor->name();
            actorWindow->setWindowTitle(actorName);
            w = actorWindow;
            mainWindow_->ui->menuWindow->addAction(actorWindow->toggleViewAction());
            if (!actor->mainIconName().isEmpty()) {
                const QString iconFileName = QCoreApplication::instance()->property("sharePath").toString()+"/icons/actors/"+actor->mainIconName()+".png";
                const QString smallIconFileName = QCoreApplication::instance()->property("sharePath").toString()+"/icons/actors/"+actor->mainIconName()+"_22x22.png";
                QIcon mainIcon = QIcon(iconFileName);
                if (QFile::exists(smallIconFileName))
                    mainIcon.addFile(smallIconFileName, QSize(22,22));
                actorWindow->setWindowIcon(mainIcon);
                actorWindow->toggleViewAction()->setIcon(mainIcon);
                mainWindow_->gr_otherActions->addAction(actorWindow->toggleViewAction());

            }

            foreach (QMenu* menu, actorMenus) {
                mainWindow_->ui->menubar->insertMenu(mainWindow_->ui->menuHelp->menuAction(), menu);
            }

            if (actor->pultWidget()) {
                Widgets::SecondaryWindow * pultWindow = new Widgets::SecondaryWindow(
                            actor->pultWidget(),
                            mainWindow_,
                            mySettings(),
                            actor->name()+"Pult");
                pultWindow->setWindowTitle(actor->name()+" - "+tr("Remote Control"));
                mainWindow_->ui->menuWindow->addAction(pultWindow->toggleViewAction());
                if (!actor->pultIconName().isEmpty()) {
                    const QString iconFileName = QCoreApplication::instance()->property("sharePath").toString()+"/icons/actors/"+actor->pultIconName()+".png";
                    const QString smallIconFileName = QCoreApplication::instance()->property("sharePath").toString()+"/icons/actors/"+actor->pultIconName()+"_22x22.png";
                    QIcon pultIcon = QIcon(iconFileName);
                    if (QFile::exists(smallIconFileName))
                        pultIcon.addFile(smallIconFileName, QSize(22,22));
                    pultWindow->setWindowIcon(pultIcon);
                    pultWindow->toggleViewAction()->setIcon(pultIcon);
                    mainWindow_->gr_otherActions->addAction(pultWindow->toggleViewAction());
                }
            }

        }

    }

    if (!parameters.contains("nostartpage", Qt::CaseInsensitive)) {
        startPage_ = plugin_browser->createBrowser();
        startPage_->setTitleChangeHandler(mainWindow_, SLOT(updateBrowserTitle(QString, const Shared::Browser::InstanceInterface*)));
        const QString browserEntryPoint = QApplication::instance()->property("sharePath").toString()+"/coregui/startpage/russian/index.html";
        (*startPage_)["mainWindow"] = mainWindow_;
        startPage_->go(browserEntryPoint);
        m_browserObjects["mainWindow"] = mainWindow_;
        dynamic_cast<QObject*>(startPage_)->setProperty("uncloseable", true);
    }
    if (parameters.contains("notabs", Qt::CaseInsensitive)) {
        mainWindow_->disableTabs();
    }

    connect(terminal_, SIGNAL(openTextEditor(QString,QString)),
            mainWindow_, SLOT(newText(QString,QString)));

    debugger_ = new DebuggerView(plugin_kumirCodeRun);
    Widgets::SecondaryWindow * debuggerWindow = new Widgets::SecondaryWindow(
                debugger_,
                mainWindow_->debuggerPlace_,
                mainWindow_,
                mySettings(),
                "DebuggerWindow");


    debuggerWindow->setWindowTitle(tr("Variables"));
    debuggerWindow->toggleViewAction()->setShortcut(QKeySequence("F2"));
    connect(mainWindow_->ui->actionVariables, SIGNAL(triggered()),
            debuggerWindow->toggleViewAction(), SLOT(trigger()));
    connect(debuggerWindow->toggleViewAction(), SIGNAL(toggled(bool)),
            mainWindow_->ui->actionVariables, SLOT(setChecked(bool)));

    return "";
}


void Plugin::updateSettings(const QStringList & keys)
{
    foreach (Widgets::SecondaryWindow * window, secondaryWindows_) {
        window->setSettingsObject(mySettings());
    }
    if (mainWindow_)
        mainWindow_->updateSettings(mySettings(), keys);
    if (helpViewer_ && keys.contains("HelpViewer")) {
        helpViewer_->updateSettings(mySettings(), "HelpViewer");
    }
}


void Plugin::changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState state)
{
    if (state==ExtensionSystem::GS_Unlocked) {
//        m_kumirStateLabel->setText(tr("Editing"));
        mainWindow_->clearMessage();
        mainWindow_->setFocusOnCentralWidget();
        mainWindow_->unlockActions();
        debugger_->reset();
        debugger_->setDebuggerEnabled(false);
    }
    else if (state==ExtensionSystem::GS_Observe) {
//        m_kumirStateLabel->setText(tr("Observe"));
        mainWindow_->showMessage(kumirProgram_->endStatus());
        mainWindow_->setFocusOnCentralWidget();
        mainWindow_->unlockActions();
    }
    else if (state==ExtensionSystem::GS_Running) {
//        m_kumirStateLabel->setText(tr("Running"));
        mainWindow_->clearMessage();
        mainWindow_->lockActions();
    }
    else if (state==ExtensionSystem::GS_Pause) {
//        m_kumirStateLabel->setText(tr("Pause"));
        mainWindow_->lockActions();
        debugger_->setDebuggerEnabled(true);
    }
    else if (state==ExtensionSystem::GS_Input) {
//        m_kumirStateLabel->setText(tr("Pause"));
        mainWindow_->lockActions();
    }
    kumirProgram_->switchGlobalState(old, state);
    terminal_->changeGlobalState(old, state);
    mainWindow_->statusBar_->setState(state);


}

void Plugin::prepareKumirProgramToRun()
{
    TabWidgetElement * twe = mainWindow_->currentTab();
    kumirProgram_->setEditorInstance(twe->editorInstance);
}

bool Plugin::showWorkspaceChooseDialog()
{
    SwitchWorkspaceDialog * dialog = new
            SwitchWorkspaceDialog(ExtensionSystem::PluginManager::instance()->globalSettings());
    if (dialog->exec() == QDialog::Accepted) {
        ExtensionSystem::PluginManager::instance()->switchToWorkspace(
                    dialog->currentWorkspace()
        );
        return true;
    }
    else {
        return false;
    }
}

void Plugin::start()
{  
    bool showDialogOnStartup = ! ExtensionSystem::PluginManager::instance()->globalSettings()
            ->value(ExtensionSystem::PluginManager::SkipChooseWorkspaceKey, false).toBool();
    if (!sessionsDisableFlag_ && showDialogOnStartup) {
        if (!showWorkspaceChooseDialog()) {
            ExtensionSystem::PluginManager::instance()->shutdown();
        }
    }
    else {
        ExtensionSystem::PluginManager::instance()->switchToDefaultWorkspace();
        updateSettings(QStringList());
    }
    PluginManager::instance()->switchGlobalState(ExtensionSystem::GS_Unlocked);
    mainWindow_->show();
}

void Plugin::stop()
{

}


void Plugin::saveSession() const
{
    mainWindow_->saveSettings();
//    mySettings()->setValue("BottomSplitterGeometry", bottomSplitter_->saveGeometry());
//    mySettings()->setValue("BottomSplitterState", bottomSplitter_->saveState());
    foreach (Widgets::SecondaryWindow * secWindow, secondaryWindows_)
        secWindow->saveState();
}


void Plugin::restoreSession()
{
    if (!sessionsDisableFlag_) {
        if (startPage_ && mainWindow_->tabWidget_->count()==0) {
            TabWidgetElement * twe = mainWindow_->addCentralComponent(
                        tr("Start"),
                        startPage_->widget(),
                        QList<QAction*>(),
                        QList<QMenu*>(),
                        MainWindow::WWW,
                        false
                        );
            twe->browserInstance = startPage_;
        }
    }
    else {
        mainWindow_->newProgram();
    }
    foreach (Widgets::SecondaryWindow * secWindow, secondaryWindows_)
        secWindow->restoreState();
    mainWindow_->loadSettings(QStringList());
}

Plugin::~Plugin()
{

}


void Plugin::setProgramSource(const ProgramSourceText &source)
{
    if (mainWindow_) {
        mainWindow_->loadFromCourseManager(source);
    }
}

GuiInterface::ProgramSourceText Plugin::programSource() const
{
    return mainWindow_->courseManagerProgramSource();
}

void Plugin::startTesting()
{
    kumirProgram_->setCourseManagerRequest();
    kumirProgram_->testingRun();
}

} // namespace CoreGUI

Q_EXPORT_PLUGIN2(CoreGui, CoreGUI::Plugin)
