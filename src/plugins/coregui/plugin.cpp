#include "plugin.h"
#include "mainwindow.h"
#include "extensionsystem/pluginmanager.h"
#include "kumirvariableswebobject.h"
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
    m_pascalProgram = 0;
    m_startPage.widget = 0;
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

    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");

    const QStringList BlacklistedThemes = QStringList()
            << "iaorakde" << "iaoraqt" << "iaora";
    const QString currentStyleName = qApp->style()->objectName().toLower();
//    qDebug() << currentStyleName;
    if (BlacklistedThemes.contains(currentStyleName)) {
        qApp->setStyle("Cleanlooks");
    }
#ifdef Q_OS_MAC
    qApp->setStyle("Cleanlooks");
#endif
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
    m_terminal = new Term(0);


    connect(m_terminal, SIGNAL(message(QString)),
            m_mainWindow, SLOT(showMessage(QString)));

    QDockWidget * termWindow = m_mainWindow->addSecondaryComponent(tr("Input/Output terminal"),
                                        m_terminal,
                                        QList<QAction*>(),
                                        QList<QMenu*>(),
                                        MainWindow::Terminal);
#ifndef Q_OS_MAC
    termWindow->toggleViewAction()->setShortcut(QKeySequence("F12"));
#endif

    if (ExtensionSystem::PluginManager::instance()->loadedConstPlugins("PascalAnalizer").isEmpty()) {
        m_mainWindow->disablePascalProgram();
    }
    else {
        m_pascalProgram = new PascalProgram(this);
        m_pascalProgram->setEditorPlugin(plugin_editor);
        m_pascalProgram->setTerminal(m_terminal, termWindow);
    }

    m_kumirProgram = new KumirProgram(this);
    m_kumirProgram->setBytecodeGenerator(plugin_BytecodeGenerator);
    m_kumirProgram->setNativeGenerator(plugin_NativeGenerator);
    m_kumirProgram->setEditorPlugin(plugin_editor);
    m_kumirProgram->setTerminal(m_terminal, termWindow);


    QMap<QString,QObject*> variablesBrowserObjects;
    variablesBrowserObjects["variablesObject"] = m_kumirProgram->variablesWebObject();
    m_kumirProgram->variablesWebObject()->setBrowserPlugin(plugin_browser);

    struct Shared::BrowserComponent variablesBrowser =
            plugin_browser->createBrowser(
                QUrl::fromLocalFile(
                    QApplication::instance()->property("sharePath").toString()+
                    "/coregui/variableswindow_kumir/index.html"
                ),
                variablesBrowserObjects);
    connect(m_kumirProgram->variablesWebObject(), SIGNAL(jsRequest(QString,QVariantList)),
            variablesBrowser.widget, SLOT(evaluateCommand(QString,QVariantList)));
    variablesBrowser.widget->setMinimumWidth(430);

    QDockWidget * variablesWindow = m_mainWindow->addSecondaryComponent(
                tr("Variables"),
                variablesBrowser.widget,
                QList<QAction*>(),
                QList<QMenu*>(),
                MainWindow::Control
                );

    connect(m_kumirProgram->variablesWebObject(), SIGNAL(newWindowCreated(Shared::BrowserComponent)),
            this, SLOT(handleNewVariablesWindow(Shared::BrowserComponent)));
    connect(m_kumirProgram->variablesWebObject(), SIGNAL(windowCloseRequest(QWidget*)),
            this, SLOT(handleCloseVariablesWindow(QWidget*)));
    connect(m_kumirProgram->variablesWebObject(), SIGNAL(windowRaiseRequest(QWidget*)),
            this, SLOT(handleRaiseVariablesWindow(QWidget*)));


    variablesWindow->toggleViewAction()->setShortcut(QKeySequence("F2"));

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
            QList<QMenu*> actorMenus = actor->moduleMenus();
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

    connect(m_terminal, SIGNAL(openTextEditor(QString,QString)),
            m_mainWindow, SLOT(newText(QString,QString)));


    QDir docsRoot(qApp->property("sharePath").toString()+"/webapps/helpviewer/data/russian/");
    const QStringList entryList = docsRoot.entryList();
    QStringList documents;
    for (int i=0; i<entryList.size(); i++) {
        const QString entry = entryList[i];
        if (entry.endsWith(".xml")) {
            const QString document = "data/russian/"+entry;
            documents << document;
        }
    }

    m_helpBrowser = plugin_browser->createBrowser(
                QUrl("http://localhost/helpviewer/index.html?documents="+documents.join(",")),
                m_browserObjects);

//    m_helpBrowser = plugin_browser->createBrowser(
//                QUrl("http://lpm.org.ru/~victor/helpviewer/index.html?documents=data/russian/system.xml,data/russian/language.xml"),
//                m_browserObjects);

//    m_helpBrowser = plugin_browser->createBrowser(
//                QUrl("http://localhost/helpviewer/index.html?documents=data/russian/system.xml,data/russian/language.xml&printable=true"),
//                m_browserObjects);

    QDockWidget * helpWindow = m_mainWindow->addSecondaryComponent(
                tr("Help"),
                m_helpBrowser.widget,
                QList<QAction*>(),
                QList<QMenu*>(),
                MainWindow::Help
                );

    helpWindow->toggleViewAction()->setShortcut(QKeySequence("F1"));
    connect(m_mainWindow->ui->actionUsage, SIGNAL(triggered()),
            helpWindow->toggleViewAction(), SLOT(trigger()));
    connect(helpWindow->toggleViewAction(), SIGNAL(toggled(bool)),
            m_mainWindow->ui->actionUsage, SLOT(setChecked(bool)));


    connect(m_kumirProgram, SIGNAL(activateDocumentTab(int)),
            m_mainWindow, SLOT(activateDocumentTab(int)));

    return "";
}

void Plugin::handleNewVariablesWindow(const Shared::BrowserComponent &browser)
{
    QDockWidget * window = m_mainWindow->addSecondaryComponent(tr("Variables"),
                                        browser.widget,
                                        QList<QAction*>(),
                                        QList<QMenu*>(),
                                        MainWindow::SubControl);
    connect(browser.widget, SIGNAL(titleChanged(QString)),
            window, SLOT(setWindowTitle(QString)));
    connect(m_kumirProgram->variablesWebObject(), SIGNAL(jsRequest(QString,QVariantList)),
            browser.widget, SLOT(evaluateCommand(QString,QVariantList)));
    window->setMinimumSize(300, 120);
    window->resize(300, 180);
    window->show();
    l_variablesChildBrowsers << browser;
    l_variablesChildWindows << window;
}

void Plugin::handleCloseVariablesWindow(QWidget *w)
{
    int index = -1;
    for (int i=0; i<l_variablesChildBrowsers.size(); i++) {
        if (l_variablesChildBrowsers[i].widget==w) {
            index = i;
            break;
        }
    }
    l_variablesChildBrowsers[index].widget->deleteLater();
    l_variablesChildWindows[index]->deleteLater();
    l_variablesChildBrowsers.removeAt(index);
    l_variablesChildWindows.removeAt(index);
}

void Plugin::handleRaiseVariablesWindow(QWidget *w)
{
    int index = -1;
    for (int i=0; i<l_variablesChildBrowsers.size(); i++) {
        if (l_variablesChildBrowsers[i].widget==w) {
            index = i;
            break;
        }
    }
    l_variablesChildWindows[index]->show();
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
    StdLib::Connector::instance()->stopListen();
}


void Plugin::saveSession() const
{
    m_mainWindow->saveSettings();
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
        m_mainWindow->newProgram();
    }
}

Plugin::~Plugin()
{
    m_startPage.widget->deleteLater();
}

} // namespace CoreGUI

Q_EXPORT_PLUGIN2(CoreGui, CoreGUI::Plugin)
