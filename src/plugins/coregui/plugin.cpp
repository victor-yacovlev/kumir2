#include "plugin.h"
#include "mainwindow.h"
#include "extensionsystem/pluginmanager.h"
#include "widgets/secondarywindow.h"
#include "debuggerwindow.h"
#include "ui_mainwindow.h"
#ifdef Q_OS_MACX
#include "mac-fixes.h"
#endif

namespace CoreGUI {

Plugin::Plugin() :
    KPlugin()
{
    m_mainWindow = 0;
    plugin_editor = 0;
    plugin_NativeGenerator = plugin_BytecodeGenerator = 0;
    b_nosessions = false;
    m_kumirProgram = 0;
    m_startPage.widget = 0;
}

QString Plugin::InitialTextKey = "InitialText";
QString Plugin::SessionFilesListKey = "Session/Files";
QString Plugin::SessionTabIndexKey = "Session/TabIndex";
QString Plugin::RecentFileKey = "History/FileDialog";
QString Plugin::RecentFilesKey = "History/RecentFiles";
QString Plugin::MainWindowGeometryKey = "Geometry/MainWindow";
QString Plugin::MainWindowStateKey = "State/MainWindow";
QString Plugin::MainWindowSplitterStateKey = "State/MainWindowSplitter";
QString Plugin::DockVisibleKey = "DockWindow/Visible";
QString Plugin::DockFloatingKey = "DockWindow/Floating";
QString Plugin::DockGeometryKey = "DockWindow/Geometry";
QString Plugin::DockSideKey = "DockWindow/Side";

QString Plugin::initialize(const QStringList & parameters)
{

    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");

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


    b_nosessions = parameters.contains("nosessions",Qt::CaseInsensitive);

    m_kumirStateLabel = new QLabel();
    m_genericCounterLabel = new QLabel();
    m_mainWindow = new MainWindow(this);
#ifdef Q_OS_MACX
    void * mac_mainWindow = (class NSView*)(m_mainWindow->winId());
    MacFixes::setLionFullscreenButton(mac_mainWindow);
#endif

    plugin_editor = qobject_cast<EditorInterface*>(myDependency("Editor"));

    plugin_BytecodeGenerator = qobject_cast<GeneratorInterface*>(myDependency("KumirCodeGenerator"));
    plugin_NativeGenerator = qobject_cast<GeneratorInterface*>(myDependency("KumirNativeGenerator"));
    plugin_browser = qobject_cast<BrowserInterface*>(myDependency("Browser"));
    if (!plugin_editor)
        return "Can't load editor plugin!";
//    if (!plugin_NativeGenerator)
//        return "Can't load c-generator plugin!";
    m_terminal = new Term(m_mainWindow);
    m_terminal->sizePolicy().setHorizontalStretch(5);
    m_terminal->sizePolicy().setHorizontalPolicy(QSizePolicy::Ignored);


    connect(m_terminal, SIGNAL(message(QString)),
            m_mainWindow, SLOT(showMessage(QString)));

//    QDockWidget * termWindow = m_mainWindow->addSecondaryComponent(tr("Input/Output terminal"),
//                                        m_terminal,
//                                        QList<QAction*>(),
//                                        QList<QMenu*>(),
//                                        MainWindow::Terminal);
    m_mainWindow->ui->bottomWidget->setLayout(new QHBoxLayout);
    QSplitter * bottomSplitter = new QSplitter(m_mainWindow->ui->bottomWidget);
    m_bottomSplitter = bottomSplitter;
    m_mainWindow->ui->bottomWidget->layout()->setContentsMargins(0,0,0,0);
//    m_mainWindow->ui->bottomWidget->layout()->addWidget(m_terminal);
    m_mainWindow->ui->bottomWidget->layout()->addWidget(bottomSplitter);
    bottomSplitter->setOrientation(Qt::Horizontal);
    bottomSplitter->addWidget(m_terminal);

#ifndef Q_OS_MAC
//    termWindow->toggleViewAction()->setShortcut(QKeySequence("F12"));
#endif

    QToolButton * btnSaveTerm = new QToolButton(m_mainWindow);
    btnSaveTerm->setPopupMode(QToolButton::InstantPopup);
    QMenu * menuSaveTerm = new QMenu(btnSaveTerm);
    btnSaveTerm->setMenu(menuSaveTerm);
    btnSaveTerm->setIcon(m_terminal->actionSaveLast()->icon());
    menuSaveTerm->addAction(m_terminal->actionSaveLast());
    menuSaveTerm->addAction(m_terminal->actionSaveAll());
    m_mainWindow->statusBar()->insertWidget(0, btnSaveTerm);
    QToolButton * btnClearTerm = new QToolButton(m_mainWindow);
    btnClearTerm->setDefaultAction(m_terminal->actionClear());
    m_mainWindow->statusBar()->insertWidget(1, btnClearTerm);
    if (!parameters.contains("notabs",Qt::CaseInsensitive)) {
        QToolButton * btnEditTerm = new QToolButton(m_mainWindow);
        btnEditTerm->setDefaultAction(m_terminal->actionEditLast());
        m_mainWindow->statusBar()->insertWidget(1, btnEditTerm);
    }


    m_mainWindow->disablePascalProgram();


    m_kumirProgram = new KumirProgram(this);
    m_kumirProgram->setBytecodeGenerator(plugin_BytecodeGenerator);
    m_kumirProgram->setEditorPlugin(plugin_editor);
    m_kumirProgram->setTerminal(m_terminal, 0);


    connect(m_kumirProgram, SIGNAL(giveMeAProgram()), this, SLOT(prepareKumirProgramToRun()), Qt::DirectConnection);


    KPlugin * kumirRunner = myDependency("KumirCodeRun");
    plugin_kumirCodeRun = qobject_cast<RunInterface*>(kumirRunner);
    //Q_CHECK_PTR(kumirRunner);
    m_kumirProgram->setBytecodeRun(kumirRunner);
    QList<ExtensionSystem::KPlugin*> actors = loadedPlugins("Actor*");
    actors += loadedPlugins("st_funct");
    foreach (ExtensionSystem::KPlugin* o, actors) {
        ActorInterface * actor = qobject_cast<ActorInterface*>(o);
        l_plugin_actors << actor;
        QWidget * w = 0;
        if (actor->mainWidget()) {
            QWidget * actorWidget = actor->mainWidget();
            QList<QMenu*> actorMenus = actor->moduleMenus();
            bool priv = o->property("privilegedActor").toBool();
//            w = m_mainWindow->addSecondaryComponent(actor->name(),
//                                                actorWidget,
//                                                QList<QAction*>(),
//                                                actorMenus,
//                                                priv? MainWindow::StandardActor : MainWindow::WorldActor);
            QWidget * place = new QWidget(m_mainWindow);
            place->setLayout(new QHBoxLayout);
            place->layout()->setContentsMargins(0,0,0,0);
            place->sizePolicy().setHorizontalStretch(1);
            place->sizePolicy().setHorizontalPolicy(QSizePolicy::Fixed);
            bottomSplitter->addWidget(place);
            place->setVisible(false);

            Widgets::SecondaryWindow * actorWindow = new Widgets::SecondaryWindow(
                        actorWidget,
                        place,
                        m_mainWindow,
                        o->pluginSettings(),
                        o->pluginSpec().name,
                        true, true
                        );
            l_secondaryWindows << actorWindow;
            actorWindow->setWindowTitle(actor->name());
            w = actorWindow;
            m_mainWindow->ui->menuWindow->addAction(actorWindow->toggleViewAction());
            if (!actor->mainIconName().isEmpty()) {
                const QString iconFileName = QCoreApplication::instance()->property("sharePath").toString()+"/icons/actors/"+actor->mainIconName()+".png";
                const QString smallIconFileName = QCoreApplication::instance()->property("sharePath").toString()+"/icons/actors/"+actor->mainIconName()+"_22x22.png";
                QIcon mainIcon = QIcon(iconFileName);
                if (QFile::exists(smallIconFileName))
                    mainIcon.addFile(smallIconFileName, QSize(22,22));
                actorWindow->setWindowIcon(mainIcon);
                actorWindow->toggleViewAction()->setIcon(mainIcon);
                m_mainWindow->gr_otherActions->addAction(actorWindow->toggleViewAction());

            }

            foreach (QMenu* menu, actorMenus) {
                m_mainWindow->ui->menubar->insertMenu(m_mainWindow->ui->menuHelp->menuAction(), menu);
            }

            if (actor->pultWidget()) {
                Widgets::SecondaryWindow * pultWindow = new Widgets::SecondaryWindow(
                            actor->pultWidget(),
                            NULL,
                            m_mainWindow,
                            mySettings(),
                            actor->name()+"Pult", false, false);
                l_secondaryWindows << pultWindow;
                pultWindow->setWindowTitle(actor->name()+" - "+tr("Remote Control"));
                m_mainWindow->ui->menuWindow->addAction(pultWindow->toggleViewAction());
                if (!actor->pultIconName().isEmpty()) {
                    const QString iconFileName = QCoreApplication::instance()->property("sharePath").toString()+"/icons/actors/"+actor->pultIconName()+".png";
                    const QString smallIconFileName = QCoreApplication::instance()->property("sharePath").toString()+"/icons/actors/"+actor->pultIconName()+"_22x22.png";
                    QIcon pultIcon = QIcon(iconFileName);
                    if (QFile::exists(smallIconFileName))
                        pultIcon.addFile(smallIconFileName, QSize(22,22));
                    pultWindow->setWindowIcon(pultIcon);
                    pultWindow->toggleViewAction()->setIcon(pultIcon);
                    m_mainWindow->gr_otherActions->addAction(pultWindow->toggleViewAction());
                }
            }

        }
        m_kumirProgram->addActor(o, w);
    }

    if (!parameters.contains("nostartpage", Qt::CaseInsensitive)) {
        const QString browserEntryPoint = QApplication::instance()->property("sharePath").toString()+"/coregui/startpage/russian/index.html";
        m_browserObjects["mainWindow"] = m_mainWindow;
        m_startPage = plugin_browser->createBrowser(QUrl::fromLocalFile(browserEntryPoint), m_browserObjects, true);
        m_startPage.widget->setProperty("uncloseable", true);
    }
    if (parameters.contains("notabs", Qt::CaseInsensitive)) {
        m_mainWindow->disableTabs();
    }

    connect(m_terminal, SIGNAL(openTextEditor(QString,QString)),
            m_mainWindow, SLOT(newText(QString,QString)));


    QString uri = "data/russian/default.xml";

    foreach (const QString parameter, parameters) {
        if (parameter.startsWith("help=")) {
            const QString helpName = parameter.mid(5);
            uri = "data/russian/"+helpName;
        }
    }


    m_helpBrowser = plugin_browser->createBrowser(
                QUrl("http://localhost/helpviewer/index.html?document="+uri),
                m_browserObjects);


    Widgets::SecondaryWindow * helpWindow = new Widgets::SecondaryWindow(m_helpBrowser.widget,0,m_mainWindow,mySettings(),"HelpWindow");
    l_secondaryWindows << helpWindow;
    helpWindow->setWindowTitle(tr("Help"));

    helpWindow->toggleViewAction()->setShortcut(QKeySequence("F1"));
    connect(m_mainWindow->ui->actionUsage, SIGNAL(triggered()),
            helpWindow->toggleViewAction(), SLOT(trigger()));
    connect(helpWindow->toggleViewAction(), SIGNAL(toggled(bool)),
            m_mainWindow->ui->actionUsage, SLOT(setChecked(bool)));


    QWidget * debuggerPlace = new QWidget(m_mainWindow);
    debuggerPlace->setLayout(new QHBoxLayout);
    debuggerPlace->layout()->setContentsMargins(0,0,0,0);
    debuggerPlace->sizePolicy().setHorizontalStretch(1);
    debuggerPlace->sizePolicy().setHorizontalPolicy(QSizePolicy::Fixed);
    bottomSplitter->insertWidget(0, debuggerPlace);
    debuggerPlace->setVisible(false);


    m_debugger = new DebuggerWindow(plugin_kumirCodeRun);
    Widgets::SecondaryWindow * debuggerWindow = new Widgets::SecondaryWindow(
                m_debugger,
                debuggerPlace,
                m_mainWindow,
                mySettings(),
                "DebuggerWindow");
    l_secondaryWindows << debuggerWindow;
    debuggerWindow->setWindowTitle(tr("Variables"));
    debuggerWindow->toggleViewAction()->setShortcut(QKeySequence("F2"));
    connect(m_mainWindow->ui->actionVariables, SIGNAL(triggered()),
            debuggerWindow->toggleViewAction(), SLOT(trigger()));
    connect(debuggerWindow->toggleViewAction(), SIGNAL(toggled(bool)),
            m_mainWindow->ui->actionVariables, SLOT(setChecked(bool)));

    connect(kumirRunner, SIGNAL(debuggerReset()),
            m_debugger, SLOT(reset()));
    connect(kumirRunner, SIGNAL(debuggerPopContext()),
            m_debugger, SLOT(popContext()));
    connect(kumirRunner,
            SIGNAL(debuggerPushContext(QString,QStringList,QStringList,QList<int>)),
            m_debugger,
            SLOT(pushContext(QString,QStringList,QStringList,QList<int>)));
    connect(kumirRunner,
            SIGNAL(debuggerUpdateLocalVariable(QString,QString)),
            m_debugger,
            SLOT(updateLocalVariable(QString,QString)));
    connect(kumirRunner,
            SIGNAL(debuggerUpdateGlobalVariable(QString,QString,QString)),
            m_debugger,
            SLOT(updateGlobalVariable(QString,QString,QString)));
    connect(kumirRunner,
            SIGNAL(debuggerUpdateLocalTableBounds(QString,QList<int>)),
            m_debugger,
            SLOT(updateLocalTableBounds(QString,QList<int>)));
    connect(kumirRunner,
            SIGNAL(debuggerUpdateGlobalTableBounds(QString,QString,QList<int>)),
            m_debugger,
            SLOT(updateGlobalTableBounds(QString,QString,QList<int>)));
    connect(kumirRunner,
            SIGNAL(debuggerSetLocalReference(QString,QString,QList<int>,int,QString)),
            m_debugger,
            SLOT(setLocalReference(QString,QString,QList<int>,int,QString)));
    connect(kumirRunner,
            SIGNAL(debuggerForceUpdateValues()),
            m_debugger,
            SLOT(updateAllValues()));
    connect(kumirRunner,
            SIGNAL(debuggerUpdateLocalTableValue(QString,QList<int>)),
            m_debugger,
            SLOT(updateLocalTableValue(QString,QList<int>)));
    connect(kumirRunner,
            SIGNAL(debuggerUpdateGlobalTableValue(QString,QString,QList<int>)),
            m_debugger,
            SLOT(updateGlobalTableValue(QString,QString,QList<int>)));

    connect(kumirRunner,
            SIGNAL(debuggerSetGlobals(QString,QStringList,QStringList,QList<int>)),
            m_debugger,
            SLOT(setGlobals(QString,QStringList,QStringList,QList<int>))
            );

    connect(m_kumirProgram, SIGNAL(activateDocumentTab(int)),
            m_mainWindow, SLOT(activateDocumentTab(int)));

    m_kumirProgram->setDebuggerWindow(m_debugger);

    return "";
}

void Plugin::updateSettings()
{
    foreach (Widgets::SecondaryWindow * window, l_secondaryWindows) {
        window->setSettingsObject(mySettings());
    }
}


void Plugin::changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState state)
{
    if (state==ExtensionSystem::GS_Unlocked) {
        m_kumirStateLabel->setText(tr("Editing"));
        m_mainWindow->clearMessage();
        m_mainWindow->setFocusOnCentralWidget();
        m_mainWindow->unlockActions();
        m_debugger->reset();
    }
    else if (state==ExtensionSystem::GS_Observe) {
        m_kumirStateLabel->setText(tr("Observe"));
        m_mainWindow->showMessage(m_kumirProgram->endStatus());
        m_mainWindow->setFocusOnCentralWidget();
        m_mainWindow->unlockActions();
    }
    else if (state==ExtensionSystem::GS_Running) {
        m_kumirStateLabel->setText(tr("Running"));
        m_mainWindow->clearMessage();
        m_mainWindow->lockActions();
    }
    else if (state==ExtensionSystem::GS_Pause) {
        m_kumirStateLabel->setText(tr("Pause"));
        m_mainWindow->lockActions();
    }
    else if (state==ExtensionSystem::GS_Input) {
        m_kumirStateLabel->setText(tr("Pause"));
        m_mainWindow->lockActions();
    }

    m_kumirProgram->switchGlobalState(old, state);
    m_terminal->changeGlobalState(old, state);


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

}


void Plugin::saveSession() const
{
    m_mainWindow->saveSettings();
    mySettings()->setValue("BottomSplitterGeometry", m_bottomSplitter->saveGeometry());
    mySettings()->setValue("BottomSplitterState", m_bottomSplitter->saveState());
    foreach (Widgets::SecondaryWindow * secWindow, l_secondaryWindows)
        secWindow->saveState();
}


void Plugin::restoreSession()
{
    m_mainWindow->loadSettings();
    if (!b_nosessions) {
        if (m_startPage.widget) {
            m_mainWindow->addCentralComponent(
                        tr("Start"),
                        m_startPage.widget,
                        m_startPage.toolbarActions,
                        m_startPage.menus,
                        QList<QWidget*>(),
                        MainWindow::WWW,
                        false
                        );
        }
    }
    else {
        QObject * dep = myDependency("Analizer");
        Q_CHECK_PTR(dep);
        QString analizerName = QString::fromAscii(dep->metaObject()->className());
        if (analizerName.startsWith("Kumir"))
            m_mainWindow->newProgram();
        else if (analizerName.startsWith("Pascal"))
            m_mainWindow->newPascalProgram();
        else if (analizerName.startsWith("Python"))
            m_mainWindow->newPythonProgram();
    }
    foreach (Widgets::SecondaryWindow * secWindow, l_secondaryWindows)
        secWindow->restoreState();
    m_bottomSplitter->restoreGeometry(mySettings()->value("BottomSplitterGeometry").toByteArray());
    m_bottomSplitter->restoreState(mySettings()->value("BottomSplitterState").toByteArray());
}

Plugin::~Plugin()
{
    m_startPage.widget->deleteLater();
}

} // namespace CoreGUI

Q_EXPORT_PLUGIN2(CoreGui, CoreGUI::Plugin)
