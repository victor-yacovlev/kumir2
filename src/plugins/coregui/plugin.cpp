#include "plugin.h"
#include "mainwindow.h"
#include "extensionsystem/pluginmanager.h"
#include "widgets/secondarywindow.h"
#include "debuggerwindow.h"
#include "ui_mainwindow.h"
#include "statusbar.h"
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
    startPage_.widget = 0;
    helpViewer_ = 0;
    courseManager_ = 0;
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


class BottomRightTabWidget
        : public QTabWidget
{
public:
    inline BottomRightTabWidget(QWidget * parent)
        : QTabWidget(parent)
    {
        setTabPosition(QTabWidget::South);
    }
protected:
    inline void tabInserted(int)
    {
        tabBar()->setVisible(count() > 1);
    }
    inline void tabRemoved(int)
    {
        tabBar()->setVisible(count() > 1);
    }
};


QString Plugin::initialize(const QStringList & parameters)
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

    plugin_editor = qobject_cast<EditorInterface*>(myDependency("Editor"));

    plugin_BytecodeGenerator = qobject_cast<GeneratorInterface*>(myDependency("KumirCodeGenerator"));
    plugin_NativeGenerator = qobject_cast<GeneratorInterface*>(myDependency("KumirNativeGenerator"));
    plugin_browser = qobject_cast<BrowserInterface*>(myDependency("Browser"));
    if (!plugin_editor)
        return "Can't load editor plugin!";
//    if (!plugin_NativeGenerator)
//        return "Can't load c-generator plugin!";
    m_terminal = new Term(mainWindow_);
    m_terminal->sizePolicy().setHorizontalStretch(5);
    m_terminal->sizePolicy().setHorizontalPolicy(QSizePolicy::Ignored);
    connect(m_terminal, SIGNAL(showWindowRequest()), mainWindow_, SLOT(ensureBottomVisible()));


    connect(m_terminal, SIGNAL(message(QString)),
            mainWindow_, SLOT(showMessage(QString)));
    int minimumTerminalHeight = m_terminal->minimumHeight();
    mainWindow_->ui->bottomWidget->setMinimumHeight(minimumTerminalHeight);

//    QDockWidget * termWindow = m_mainWindow->addSecondaryComponent(tr("Input/Output terminal"),
//                                        m_terminal,
//                                        QList<QAction*>(),
//                                        QList<QMenu*>(),
//                                        MainWindow::Terminal);
    mainWindow_->ui->bottomWidget->setLayout(new QHBoxLayout);
    QSplitter * bottomSplitter = new QSplitter(mainWindow_->ui->bottomWidget);
    bottomSplitter->setStyleSheet(""
                                  "QSplitter[stylable=\"true\"] {"
                                  "    width: 16px;"
                                  "    background-image: url(:/coregui/horizontal-splitter-background.png);"
                                  "}"
                                  "QSplitter[stylable=\"true\"]::handle {"
                                  "    width: 16px;"
                                  "    background-image: url(:/coregui/horizontal-splitter-handle.png);"
                                  "}"
                );
    bottomSplitter->setProperty("stylable", true);
    bottomSplitter_ = bottomSplitter;
    mainWindow_->ui->bottomWidget->layout()->setContentsMargins(0,0,0,0);
//    m_mainWindow->ui->bottomWidget->layout()->addWidget(m_terminal);
    mainWindow_->ui->bottomWidget->layout()->addWidget(bottomSplitter);
    bottomSplitter->setOrientation(Qt::Horizontal);
    bottomSplitter->addWidget(m_terminal);
    actorsDockPlace_ = new BottomRightTabWidget(bottomSplitter);
    bottomSplitter->addWidget(actorsDockPlace_);

#ifndef Q_OS_MAC
//    termWindow->toggleViewAction()->setShortcut(QKeySequence("F12"));
#endif

    connect(mainWindow_->ui->actionShow_Console_Pane,
            SIGNAL(triggered(bool)),
            this,
            SLOT(showConsolePane(bool)));

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
    btnSaveTerm->setIcon(m_terminal->actionSaveLast()->icon());
    menuSaveTerm->addAction(m_terminal->actionSaveLast());
    menuSaveTerm->addAction(m_terminal->actionSaveAll());
//    mainWindow_->statusBar()->insertWidget(0, btnSaveTerm);
    mainWindow_->statusBar_->addButtonToLeft(btnSaveTerm);
    QToolButton * btnClearTerm = new QToolButton(mainWindow_);
    m_terminal->actionClear()->setIcon(cleanConsoleIcon);
    btnClearTerm->setDefaultAction(m_terminal->actionClear());
//    mainWindow_->statusBar()->insertWidget(1, btnClearTerm);
    mainWindow_->statusBar_->addButtonToLeft(btnClearTerm);
    if (!parameters.contains("notabs",Qt::CaseInsensitive)) {
        QToolButton * btnEditTerm = new QToolButton(mainWindow_);
        btnEditTerm->setDefaultAction(m_terminal->actionEditLast());
        mainWindow_->statusBar()->insertWidget(1, btnEditTerm);
    }


    kumirProgram_ = new KumirProgram(this);
    kumirProgram_->setBytecodeGenerator(plugin_BytecodeGenerator);
    kumirProgram_->setEditorPlugin(plugin_editor);
    kumirProgram_->setTerminal(m_terminal, 0);


    connect(kumirProgram_, SIGNAL(giveMeAProgram()), this, SLOT(prepareKumirProgramToRun()), Qt::DirectConnection);

    helpViewer_ = new DocBookViewer::DocBookView(mainWindow_);
    helpViewer_->updateSettings(mySettings(), "HelpViewer");
    static const QString helpPath =
            QApplication::instance()->property("sharePath").toString() +
            "/userdocs/";

    helpViewer_->addDocument(QUrl::fromLocalFile(helpPath + "default.xml"));

    Widgets::SecondaryWindow * helpWindow = new Widgets::SecondaryWindow(
                helpViewer_,
                0,
                mainWindow_,
                mySettings(),
                "HelpViewerWindow");
    secondaryWindows_ << helpWindow;
    helpWindow->setWindowTitle(tr("Help"));

    helpWindow->toggleViewAction()->setShortcut(QKeySequence("F1"));
    connect(mainWindow_->ui->actionUsage, SIGNAL(triggered()),
            helpWindow->toggleViewAction(), SLOT(trigger()));
    connect(helpWindow->toggleViewAction(), SIGNAL(toggled(bool)),
            mainWindow_->ui->actionUsage, SLOT(setChecked(bool)));

    courseManager_ = ExtensionSystem::PluginManager::instance()
            ->findPlugin<Shared::CoursesInterface>();

    if (courseManager_) {
        foreach (QMenu* menu, courseManager_->menus()) {
            mainWindow_->ui->menubar->insertMenu(mainWindow_->ui->menuHelp->menuAction(), menu);
        }

        Widgets::SecondaryWindow * coursesWindow = new Widgets::SecondaryWindow(
                    courseManager_->mainWindow(),
                    nullptr,
                    mainWindow_,
                    mySettings(),
                    "CoursesWindow"
                    );

        secondaryWindows_ << coursesWindow;
        mainWindow_->ui->menuWindow->addAction(coursesWindow->toggleViewAction());
        mainWindow_->gr_otherActions->addAction(coursesWindow->toggleViewAction());
    }

    KPlugin * kumirRunner = ExtensionSystem::PluginManager::instance()
            ->findKPlugin<RunInterface>();
    plugin_kumirCodeRun = qobject_cast<RunInterface*>(kumirRunner);
    kumirProgram_->setBytecodeRun(kumirRunner);
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
            QWidget * place = new QWidget(actorsDockPlace_);
            place->installEventFilter(this);
            place->setLayout(new QHBoxLayout);
            place->layout()->setContentsMargins(0,0,0,0);
            place->sizePolicy().setHorizontalStretch(1);
            place->sizePolicy().setHorizontalPolicy(QSizePolicy::Fixed);
//            bottomSplitter->addWidget(place);
            place->setWindowTitle(actor->name());
            place->setVisible(false);

            Widgets::SecondaryWindow * actorWindow = new Widgets::SecondaryWindow(
                        actorWidget,
                        place,
                        mainWindow_,
                        o->pluginSettings(),
                        o->pluginSpec().name,
                        true, true
                        );
            connect(actorWindow, SIGNAL(docked(QWidget*,QString)),
                    this, SLOT(handleSecondaryWindowDocked(QWidget*,QString)));
            connect(actorWindow, SIGNAL(undocked(QWidget*)),
                    this, SLOT(handleSecondaryWindowUndocked(QWidget*)));
            secondaryWindows_ << actorWindow;
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
                            NULL,
                            mainWindow_,
                            mySettings(),
                            actor->name()+"Pult", false, false);
                secondaryWindows_ << pultWindow;
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
        const QString browserEntryPoint = QApplication::instance()->property("sharePath").toString()+"/coregui/startpage/russian/index.html";
        m_browserObjects["mainWindow"] = mainWindow_;
        startPage_ = plugin_browser->createBrowser(QUrl::fromLocalFile(browserEntryPoint), m_browserObjects, true);
        startPage_.widget->setProperty("uncloseable", true);
    }
    if (parameters.contains("notabs", Qt::CaseInsensitive)) {
        mainWindow_->disableTabs();
    }

    connect(m_terminal, SIGNAL(openTextEditor(QString,QString)),
            mainWindow_, SLOT(newText(QString,QString)));

    QWidget * debuggerPlace = new QWidget(mainWindow_);
    debuggerPlace->setLayout(new QHBoxLayout);
    debuggerPlace->layout()->setContentsMargins(0,0,0,0);
    debuggerPlace->sizePolicy().setHorizontalStretch(1);
    debuggerPlace->sizePolicy().setHorizontalPolicy(QSizePolicy::Fixed);
    bottomSplitter->insertWidget(0, debuggerPlace);
    debuggerPlace->setVisible(false);


    debugger_ = new DebuggerWindow(plugin_kumirCodeRun);
    Widgets::SecondaryWindow * debuggerWindow = new Widgets::SecondaryWindow(
                debugger_,
                debuggerPlace,
                mainWindow_,
                mySettings(),
                "DebuggerWindow");
    secondaryWindows_ << debuggerWindow;
    debuggerWindow->setWindowTitle(tr("Variables"));
    debuggerWindow->toggleViewAction()->setShortcut(QKeySequence("F2"));
    connect(mainWindow_->ui->actionVariables, SIGNAL(triggered()),
            debuggerWindow->toggleViewAction(), SLOT(trigger()));
    connect(debuggerWindow->toggleViewAction(), SIGNAL(toggled(bool)),
            mainWindow_->ui->actionVariables, SLOT(setChecked(bool)));

    connect(kumirRunner, SIGNAL(debuggerReset()),
            debugger_, SLOT(reset()));
    connect(kumirRunner, SIGNAL(debuggerPopContext()),
            debugger_, SLOT(popContext()));
    connect(kumirRunner,
            SIGNAL(debuggerPushContext(QString,QStringList,QStringList,QList<int>)),
            debugger_,
            SLOT(pushContext(QString,QStringList,QStringList,QList<int>)));
    connect(kumirRunner,
            SIGNAL(debuggerUpdateLocalVariable(QString,QString)),
            debugger_,
            SLOT(updateLocalVariable(QString,QString)));
    connect(kumirRunner,
            SIGNAL(debuggerUpdateGlobalVariable(QString,QString,QString)),
            debugger_,
            SLOT(updateGlobalVariable(QString,QString,QString)));
    connect(kumirRunner,
            SIGNAL(debuggerUpdateLocalTableBounds(QString,QList<int>)),
            debugger_,
            SLOT(updateLocalTableBounds(QString,QList<int>)));
    connect(kumirRunner,
            SIGNAL(debuggerUpdateGlobalTableBounds(QString,QString,QList<int>)),
            debugger_,
            SLOT(updateGlobalTableBounds(QString,QString,QList<int>)));
    connect(kumirRunner,
            SIGNAL(debuggerSetLocalReference(QString,QString,QList<int>,int,QString)),
            debugger_,
            SLOT(setLocalReference(QString,QString,QList<int>,int,QString)));
    connect(kumirRunner,
            SIGNAL(debuggerForceUpdateValues()),
            debugger_,
            SLOT(updateAllValues()));
    connect(kumirRunner,
            SIGNAL(debuggerUpdateLocalTableValue(QString,QList<int>)),
            debugger_,
            SLOT(updateLocalTableValue(QString,QList<int>)));
    connect(kumirRunner,
            SIGNAL(debuggerUpdateGlobalTableValue(QString,QString,QList<int>)),
            debugger_,
            SLOT(updateGlobalTableValue(QString,QString,QList<int>)));

    connect(kumirRunner,
            SIGNAL(debuggerSetGlobals(QString,QStringList,QStringList,QList<int>)),
            debugger_,
            SLOT(setGlobals(QString,QStringList,QStringList,QList<int>))
            );

    connect(kumirProgram_, SIGNAL(activateDocumentTab(int)),
            mainWindow_, SLOT(activateDocumentTab(int)));

    kumirProgram_->setDebuggerWindow(debugger_);



    return "";
}

void Plugin::handleSecondaryWindowDocked(QWidget * w, const QString & title)
{
    actorsDockPlace_->addTab(w, w->windowTitle());
    actorsDockPlace_->setCurrentWidget(w);
    actorsDockPlace_->setVisible(true);
    mainWindow_->ui->actionShow_Console_Pane->setChecked(true);
    showConsolePane(true);
}

void Plugin::handleSecondaryWindowUndocked(QWidget * w)
{
    int index = actorsDockPlace_->indexOf(w);
    actorsDockPlace_->removeTab(index);
    if (actorsDockPlace_->count() == 0) {
        actorsDockPlace_->setVisible(false);
        if (m_terminal->isEmpty()) {
            mainWindow_->ui->actionShow_Console_Pane->setChecked(false);
            showConsolePane(false);
        }
    }
}

void Plugin::showConsolePane(bool v)
{
    mainWindow_->ui->bottomWidget->setVisible(v);
}


void Plugin::updateSettings()
{
    foreach (Widgets::SecondaryWindow * window, secondaryWindows_) {
        window->setSettingsObject(mySettings());
    }
    if (helpViewer_) {
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
    }
    else if (state==ExtensionSystem::GS_Input) {
//        m_kumirStateLabel->setText(tr("Pause"));
        mainWindow_->lockActions();
    }
    kumirProgram_->switchGlobalState(old, state);
    m_terminal->changeGlobalState(old, state);
    mainWindow_->statusBar_->setState(state);


}

void Plugin::prepareKumirProgramToRun()
{
    plugin_editor->ensureAnalized(kumirProgram_->documentId());
}

void Plugin::start()
{
    if (!sessionsDisableFlag_ && ExtensionSystem::PluginManager::instance()->showWorkspaceChooseOnLaunch()) {
        if (!ExtensionSystem::PluginManager::instance()->showWorkspaceChooseDialog()) {
            qApp->quit();
        }
    }
    else {
//    else if (!sessionsDisableFlag_) {
//        ExtensionSystem::PluginManager::instance()->switchToDefaultWorkspace();
//    }
//    else {
        ExtensionSystem::PluginManager::instance()->switchToDefaultWorkspace();
        updateSettings();
        restoreSession();
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
    mySettings()->setValue("BottomSplitterGeometry", bottomSplitter_->saveGeometry());
    mySettings()->setValue("BottomSplitterState", bottomSplitter_->saveState());
    foreach (Widgets::SecondaryWindow * secWindow, secondaryWindows_)
        secWindow->saveState();
}


void Plugin::restoreSession()
{
    mainWindow_->loadSettings();
    if (!sessionsDisableFlag_) {
        if (startPage_.widget && mainWindow_->ui->tabWidget->count()==0) {
            mainWindow_->addCentralComponent(
                        tr("Start"),
                        startPage_.widget,
                        startPage_.toolbarActions,
                        startPage_.menus,
//                        QList<QWidget*>(),
                        MainWindow::WWW,
                        false
                        );
        }
    }
    else {
        QObject * dep = myDependency("Analizer");
        Q_CHECK_PTR(dep);
        QString analizerName = QString::fromAscii(dep->metaObject()->className());
        mainWindow_->newProgram();
    }
    foreach (Widgets::SecondaryWindow * secWindow, secondaryWindows_)
        secWindow->restoreState();
    bottomSplitter_->restoreGeometry(mySettings()->value("BottomSplitterGeometry").toByteArray());
    bottomSplitter_->restoreState(mySettings()->value("BottomSplitterState").toByteArray());
}

Plugin::~Plugin()
{
    startPage_.widget->deleteLater();
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
