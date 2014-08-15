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

#include "guisettingspage.h"

#ifdef Q_OS_MACX
#include "mac-fixes.h"
#endif
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
    #include <signal.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif

namespace CoreGUI {

using namespace Shared;

Plugin::Plugin() :
    KPlugin()
{
#if defined(Q_OS_WIN32)
    ipcShm_ = 0;
#endif
    mainWindow_ = 0;
    m_kumirStateLabel = 0;
    m_genericCounterLabel = 0;
    plugin_editor = 0;
    plugin_NativeGenerator = plugin_BytecodeGenerator = 0;
    plugin_browser = 0;
    plugin_kumirCodeRun = 0;
    startPage_ = 0;
    helpWindow_ = 0;
    coursesWindow_ = 0;
    terminal_ = 0;
    kumirProgram_ = 0;
    debugger_ = 0;
    sessionsDisableFlag_ = false;
    helpViewer_ = 0;
    courseManager_ = 0;
    guiSettingsPage_ = 0;
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

Plugin* Plugin::instance_ = 0;

QList<CommandLineParameter> Plugin::acceptableCommandLineParameters() const
{
    QList<CommandLineParameter> result;
    result << CommandLineParameter(
                  true,
                  tr("PROGRAM.kum"),
                  tr("Source file name"),
                  QVariant::String,
                  false
                  );
    return result;
}

QString Plugin::initialize(const QStringList & parameters, const ExtensionSystem::CommandLine & cmd)
{
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    instance_ = this;
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    if (cmd.size() > 0 && cmd.value(size_t(0)).isValid()) {
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        fileNameToOpenOnReady_ = cmd.value(size_t(0)).toString();
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        if (! QFileInfo(fileNameToOpenOnReady_).isAbsolute()) {
            qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
            fileNameToOpenOnReady_ =
                    QDir::current().absoluteFilePath(fileNameToOpenOnReady_);
            qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        }
    }
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    qRegisterMetaType<Shared::Analizer::SourceFileInterface::Data>("KumirFile.Data");
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    qRegisterMetaType<Shared::GuiInterface::ProgramSourceText::Language>
            ("Gui.ProgramSourceText.Language");
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    qRegisterMetaType<Shared::GuiInterface::ProgramSourceText>
            ("Gui.ProgramSourceText");
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    connect(this, SIGNAL(externalProcessCommandReceived(QString)),
            this, SLOT(handleExternalProcessCommand(QString)),
            Qt::QueuedConnection);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;


    const QStringList BlacklistedThemes = QStringList()
            << "iaorakde" << "iaoraqt" << "iaora";
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    const QString currentStyleName = qApp->style()->objectName().toLower();
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    if (BlacklistedThemes.contains(currentStyleName)) {
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        qApp->setStyle("Cleanlooks");
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    }
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    QString iconSuffix;
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    for (int i=0; i<parameters.count(); i++) {
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        const QString param = parameters[i];
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        if (param.startsWith("icon=")) {
            qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
            iconSuffix = "-"+param.mid(5);
            qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        }
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    }
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    QApplication::setWindowIcon(QIcon(myResourcesDir().absoluteFilePath("kumir2-icon"+iconSuffix+".png")));


    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    sessionsDisableFlag_ = parameters.contains("nosessions",Qt::CaseInsensitive);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    m_kumirStateLabel = new QLabel();
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    m_genericCounterLabel = new QLabel();
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    qDebug() << "Creating main window";
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    mainWindow_ = new MainWindow(this);
    qDebug() << "Main window created";
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

#ifdef Q_OS_MACX
   // void * mac_mainWindow = (class NSView*)(m_mainWindow->winId());
    //MacFixes::setLionFullscreenButton(mac_mainWindow);
#endif

    plugin_editor = PluginManager::instance()->findPlugin<EditorInterface>();
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;


    plugin_BytecodeGenerator = qobject_cast<GeneratorInterface*>(myDependency("KumirCodeGenerator"));
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    plugin_NativeGenerator = qobject_cast<GeneratorInterface*>(myDependency("KumirNativeGenerator"));
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    plugin_browser = qobject_cast<BrowserInterface*>(myDependency("Browser"));
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    if (!plugin_editor)
        return "Can't load editor plugin!";
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    qDebug() << "Creating and connection I/O terminal";
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    terminal_ = new Term(mainWindow_);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    mainWindow_->consolePlace_->addPersistentWidget(terminal_,
                                                              tr("Input/Output"));
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    connect(terminal_, SIGNAL(showWindowRequest()),
            mainWindow_, SLOT(ensureBottomVisible()));
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    connect(terminal_, SIGNAL(message(QString)),
            mainWindow_, SLOT(showMessage(QString)));
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    qDebug() << "Done creating and connecting terminal";
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    qDebug() << "Creating near terminal icons";
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    const QString qtcreatorIconsPath =
            ExtensionSystem::PluginManager::instance()->sharePath()
            + "/icons/from_qtcreator/";
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    const QString showConsoleIconPath = qtcreatorIconsPath + "category_core.png";
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    const QString clearConsoleIconPath = qtcreatorIconsPath + "clean_pane_small.png";
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    const QIcon showConsoleIcon = QIcon(showConsoleIconPath);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    const QIcon cleanConsoleIcon = QIcon(clearConsoleIconPath);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;


    QToolButton * btnShowConsole = new QToolButton(mainWindow_);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    mainWindow_->ui->actionShow_Console_Pane->setIcon(showConsoleIcon);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    btnShowConsole->setDefaultAction(mainWindow_->ui->actionShow_Console_Pane);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    btnShowConsole->setToolTip(mainWindow_->ui->actionShow_Console_Pane->text());
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    mainWindow_->statusBar_->addButtonToLeft(btnShowConsole);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    QToolButton * btnSaveTerm = new QToolButton(mainWindow_);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    btnSaveTerm->setPopupMode(QToolButton::InstantPopup);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    QMenu * menuSaveTerm = new QMenu(btnSaveTerm);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    btnSaveTerm->setToolTip(tr("Save console output"));
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    btnSaveTerm->setMenu(menuSaveTerm);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    btnSaveTerm->setIcon(QIcon(qtcreatorIconsPath + "filesave.png"));
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    menuSaveTerm->addAction(terminal_->actionSaveLast());
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    menuSaveTerm->addAction(terminal_->actionSaveAll());
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
//    mainWindow_->statusBar()->insertWidget(0, btnSaveTerm);
    mainWindow_->statusBar_->addButtonToLeft(btnSaveTerm);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    QToolButton * btnCopyTerm = new QToolButton(mainWindow_);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    btnCopyTerm->setPopupMode(QToolButton::InstantPopup);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    QMenu * menuCopyTerm = new QMenu(btnCopyTerm);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    btnCopyTerm->setToolTip(tr("Copy to clipboard console output"));
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    btnCopyTerm->setMenu(menuCopyTerm);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    btnCopyTerm->setIcon(QIcon(qtcreatorIconsPath + "editcopy.png"));
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    menuCopyTerm->addAction(terminal_->actionCopyLast());
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    menuCopyTerm->addAction(terminal_->actionCopyAll());
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    mainWindow_->statusBar_->addButtonToLeft(btnCopyTerm);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    QToolButton * btnClearTerm = new QToolButton(mainWindow_);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    terminal_->actionClear()->setIcon(cleanConsoleIcon);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    btnClearTerm->setDefaultAction(terminal_->actionClear());
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
//    mainWindow_->statusBar()->insertWidget(1, btnClearTerm);
    mainWindow_->statusBar_->addButtonToLeft(btnClearTerm);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    QToolButton * btnEditTerm = nullptr;
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    if (!parameters.contains("notabs",Qt::CaseInsensitive)) {
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        btnEditTerm = new QToolButton(mainWindow_);
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        btnEditTerm->setDefaultAction(terminal_->actionEditLast());
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        mainWindow_->statusBar_->addButtonToLeft(btnEditTerm);
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    }
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    qDebug() << "Created near terminal icons";
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
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

    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    kumirProgram_ = new KumirProgram(this);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    kumirProgram_->setTerminal(terminal_, 0);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;


    connect(kumirProgram_, SIGNAL(giveMeAProgram()), this, SLOT(prepareKumirProgramToRun()), Qt::DirectConnection);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    helpViewer_ = new DocBookViewer::DocBookView(mainWindow_);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    helpViewer_->updateSettings(mySettings(), "HelpViewer");
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    static const QString helpPath =
           ExtensionSystem::PluginManager::instance()->sharePath() +
            "/userdocs/";
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    const QString applicationLanucher = QDir::fromNativeSeparators(qApp->arguments().at(0));
    QString applicationName =
            applicationLanucher.startsWith(qApp->applicationDirPath())
            ? applicationLanucher.mid(qApp->applicationDirPath().length() + 1)
            : applicationLanucher;
#ifdef Q_OS_WIN32
    if (applicationName.endsWith(".exe")) {
        applicationName.remove(applicationName.length()-4, 4);
    }
#endif

#ifndef Q_OS_MACX
    QString indexFileName = applicationName + ".xml";
    indexFileName.replace("kumir2-", "index-");
#else
    QString indexFileName = "index-macx.xml";
#endif

    Shared::AnalizerInterface * analizerPlugin =
            ExtensionSystem::PluginManager::instance()
            ->findPlugin<Shared::AnalizerInterface>();

    if (analizerPlugin) {
        const QString apiHelpListingRole = analizerPlugin->languageName();
        helpViewer_->setRole(DocBookViewer::ProgramListing, apiHelpListingRole);
        helpViewer_->setRole(DocBookViewer::FuncSynopsys, apiHelpListingRole);
        helpViewer_->setRole(DocBookViewer::Section, apiHelpListingRole);
        helpViewer_->setRole(DocBookViewer::Type, apiHelpListingRole);
        helpViewer_->setRole(DocBookViewer::Code, apiHelpListingRole);
    }

    indexFileName = helpPath + indexFileName;
    if (QFile::exists(indexFileName)) {
        helpViewer_->addDocument(QUrl::fromLocalFile(indexFileName));
    }

    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    helpWindow_ = Widgets::SecondaryWindow::createSecondaryWindow(
                helpViewer_,
                tr("Help"),
                QIcon(), // TODO help window icon
                mainWindow_,
                mainWindow_->helpAndCoursesPlace_,
                "HelpViewerWindow",
                true
                );
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    connect(mainWindow_->ui->actionUsage, SIGNAL(triggered()),
            mainWindow_, SLOT(showHelp()));
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    secondaryWindows_ << helpWindow_;
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;


    courseManager_ = ExtensionSystem::PluginManager::instance()
            ->findPlugin<Shared::CoursesInterface>();
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    if (courseManager_) {
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        foreach (QMenu* menu, courseManager_->menus()) {
            qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
            mainWindow_->ui->menubar->insertMenu(mainWindow_->ui->menuHelp->menuAction(), menu);
        }
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        coursesWindow_ = Widgets::SecondaryWindow::createSecondaryWindow(
                    courseManager_->mainWindow(),
                    tr("Courses"),
                    QIcon(), // TODO courses icon
                    mainWindow_,
                    mainWindow_->helpAndCoursesPlace_,
                    "CoursesWindow",
                    true
                    );
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        QAction * showCourses =
                mainWindow_->ui->menuWindow->addAction(
                    tr("Courses"),
                    coursesWindow_, SLOT(activate())
                    );
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        showCourses->setObjectName("window-courses");
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

        const QString courseIconFileName = ExtensionSystem::PluginManager::instance()->sharePath()+"/icons/course.png";
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        QIcon courseIcon(courseIconFileName);
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        showCourses->setIcon(courseIcon);
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

        mainWindow_->gr_otherActions->addAction(showCourses);
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        secondaryWindows_ << coursesWindow_;
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    }
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    KPlugin * kumirRunner = ExtensionSystem::PluginManager::instance()
            ->findKPlugin<RunInterface>();
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    plugin_kumirCodeRun = qobject_cast<RunInterface*>(kumirRunner);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    connect(kumirRunner, SIGNAL(showActorWindowRequest(QByteArray)),
            this, SLOT(showActorWindow(QByteArray)));
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    QList<ExtensionSystem::KPlugin*> actors = loadedPlugins("Actor*");
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    actors += loadedPlugins("st_funct");
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    foreach (ExtensionSystem::KPlugin* o, actors) {
        ActorInterface * actor = qobject_cast<ActorInterface*>(o);
        const QString actorName = Shared::actorCanonicalName(actor->localizedModuleName(QLocale::Russian));
        const QString actorObjectName = Shared::actorCanonicalName(actor->asciiModuleName()).replace(" ", "-").toLower();
        l_plugin_actors << actor;
        QWidget * w = 0;
        const QString actorHelpFile = helpPath + o->pluginSpec().name + ".xml";
        if (!actor->localizedModuleName(QLocale::Russian).startsWith("_") && QFile(actorHelpFile).exists()) {
            helpViewer_->addDocument(QUrl::fromLocalFile(actorHelpFile));
        }
        if (actor->mainWidget()) {
            QWidget * actorWidget = actor->mainWidget();
            QList<QMenu*> actorMenus = actor->moduleMenus();
            const QString iconFileName = ExtensionSystem::PluginManager::instance()->sharePath()+"/icons/actors/"+actor->mainIconName()+".png";
            const QString smallIconFileName = ExtensionSystem::PluginManager::instance()->sharePath()+"/icons/actors/"+actor->mainIconName()+"_22x22.png";
            QIcon mainIcon = QIcon(iconFileName);
            if (QFile::exists(smallIconFileName))
                mainIcon.addFile(smallIconFileName, QSize(22,22));

            const QSizePolicy actorSizePolicy = actorWidget->sizePolicy();
            const QSizePolicy::Policy horz = actorSizePolicy.horizontalPolicy();
            const QSizePolicy::Policy vert = actorSizePolicy.verticalPolicy();
            bool resizableX = horz != QSizePolicy::Fixed;
            bool resizableY = vert != QSizePolicy::Fixed;
            bool resizable = resizableX && resizableY;

            Widgets::SecondaryWindow * actorWindow =
                    Widgets::SecondaryWindow::createSecondaryWindow(
                        actorWidget,
                        actorName,
                        mainIcon,
                        mainWindow_,
                        mainWindow_->actorsPlace_,
                        o->pluginSpec().name,
                        resizable
                        );

            secondaryWindows_ << actorWindow;

            QAction * showActor =
                    mainWindow_->ui->menuWindow->addAction(
                        actorName,
                        actorWindow,
                        SLOT(activate())
                        );

            showActorActions_[actor->asciiModuleName()] = showActor;

            showActor->setObjectName("window-actor-" + actorObjectName);
            mainWindow_->gr_otherActions->addAction(showActor);
            if (!actor->mainIconName().isEmpty()) {
                showActor->setIcon(mainIcon);
            }

            foreach (QMenu* menu, actorMenus) {
                mainWindow_->ui->menubar->insertMenu(mainWindow_->ui->menuHelp->menuAction(), menu);
            }

            if (actor->pultWidget()) {
                const QString iconFileName = ExtensionSystem::PluginManager::instance()->sharePath()+"/icons/actors/"+actor->pultIconName()+".png";
                const QString smallIconFileName = ExtensionSystem::PluginManager::instance()->sharePath()+"/icons/actors/"+actor->pultIconName()+"_22x22.png";
                QIcon pultIcon = QIcon(iconFileName);
                if (QFile::exists(smallIconFileName))
                    pultIcon.addFile(smallIconFileName, QSize(22,22));

                Widgets::SecondaryWindow * pultWindow =
                        Widgets::SecondaryWindow::createSecondaryWindow(
                            actor->pultWidget(),
                            actorName + " - " + tr("Remote Control"),
                            pultIcon,
                            mainWindow_,
                            nullptr,
                            o->pluginSpec().name + "Pult",
                            false
                            );
                secondaryWindows_ << pultWindow;

                QAction * showPult =
                        mainWindow_->ui->menuWindow->addAction(
                            actorName + " - " + tr("Remote Control"),
                            pultWindow,
                            SLOT(activate())
                            );
                showPult->setObjectName("window-control-" + actorObjectName);

                mainWindow_->gr_otherActions->addAction(showPult);

                if (!actor->pultIconName().isEmpty()) {
                    showPult->setIcon(pultIcon);
                }
            }

        }

    }
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    if (!parameters.contains("nostartpage", Qt::CaseInsensitive)) {
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        startPage_ = plugin_browser->createBrowser();
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        startPage_->setTitleChangeHandler(mainWindow_, SLOT(updateBrowserTitle(QString, const Shared::Browser::InstanceInterface*)));        
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        (*startPage_)["mainWindow"] = mainWindow_;
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        (*startPage_)["gui"] = this;
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        m_browserObjects["mainWindow"] = mainWindow_;
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        startPage_->widget()->setProperty("uncloseable", true);
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        if (startPage_ && mainWindow_->tabWidget_->count()==0) {
            qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
            QMenu * editMenu = new QMenu(mainWindow_->ui->menuEdit->title(), mainWindow_);
            qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
            QMenu * insertMenu = new QMenu(mainWindow_->ui->menuInsert->title(), mainWindow_);
            qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
            QAction * editNotAvailable = editMenu->addAction(mainWindow_->tr("No actions for this tab"));
            qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
            QAction * insertNotAvailable = insertMenu->addAction(mainWindow_->tr("No actions for this tab"));
            qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
            editNotAvailable->setEnabled(false);
            qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
            insertNotAvailable->setEnabled(false);
            qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
            TabWidgetElement * twe = mainWindow_->addCentralComponent(
                        tr("Start"),
                        startPage_->widget(),
                        QList<QAction*>(),
                        QList<QMenu*>() << editMenu << insertMenu,
                        MainWindow::WWW
                        );
            qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
            twe->setBrowser(startPage_);
            qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
            const QString browserEntryPoint = myResourcesDir().absoluteFilePath("startpage/russian/index2.html");
            qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
            const QUrl browserEntryUrl = QUrl::fromLocalFile(browserEntryPoint);
            startPage_->go(browserEntryUrl);
            qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        }
    }
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    if (parameters.contains("notabs", Qt::CaseInsensitive)) {
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        mainWindow_->disableTabs();
    }

    connect(terminal_, SIGNAL(openTextEditor(QString,QString)),
            mainWindow_, SLOT(newText(QString,QString)), Qt::DirectConnection);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    debugger_ = new DebuggerView(plugin_kumirCodeRun);
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    Widgets::SecondaryWindow * debuggerWindow =
            Widgets::SecondaryWindow::createSecondaryWindow(
                debugger_,
                tr("Variables"),
                QIcon(), // TODO window icon
                mainWindow_,
                mainWindow_->debuggerPlace_,
                "DebuggerWindow",
                true
                );
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    mainWindow_->debuggerWindow_ = debuggerWindow;
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    secondaryWindows_ << debuggerWindow;
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    connect(mainWindow_->ui->actionVariables, SIGNAL(triggered()),
            debuggerWindow, SLOT(activate()));
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;

    const QString layoutChoice =
            mySettings()->value(GUISettingsPage::LayoutKey, GUISettingsPage::ColumnsFirstValue).toString();
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    if (layoutChoice == GUISettingsPage::ColumnsFirstValue) {
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        mainWindow_->switchToColumnFirstLayout();
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    }
    else {
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
        mainWindow_->switchToRowFirstLayout();
        qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    }

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    struct sigaction act;
    act.sa_sigaction = handleSIGUSR1;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    act.sa_mask = set;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, 0);
#endif  
#if defined(Q_OS_WIN32)
    ipcShm_ = new QSharedMemory(QFileInfo(qApp->arguments().at(0)).fileName(), this);
    if (ipcShm_->create(2048, QSharedMemory::ReadWrite)) {
        ipcShm_->lock();
        memset(ipcShm_->data(), 0, ipcShm_->size());
        ipcShm_->unlock();
        startTimer(250);
    }
#endif
    qDebug() << "LINE DEBUG: " << QFileInfo(QString(__FILE__)).fileName() << ":" << __LINE__;
    return "";
}

void Plugin::handleExternalProcessCommand(const QString &command)
{
    int spacePos = command.indexOf(' ');
    QString cmd;
    QString arg;
    if (spacePos == -1) {
        cmd = command.trimmed();
    }
    else {
        cmd = command.left(spacePos).trimmed();
        arg = command.mid(spacePos+1).trimmed();
    }
    if (cmd.toLower() == QString("open")) {
        mainWindow_->loadFromUrl(QUrl::fromLocalFile(arg), true);
    }
}

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
void Plugin::handleSIGUSR1(int , siginfo_t * info, void * )
{
    sigval_t val = info->si_value;
    ::usleep(1000u);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(val.sival_int);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    ::connect(sock, (const sockaddr*) &addr, sizeof(addr));
    QByteArray data;
    char buf[256];
    ssize_t sz;
    while (1) {
        sz = recv(sock, buf, 256, 0);
        if (sz <= 0) {
            break;
        }
        QByteArray block(buf, sz);
        data += block;
    }
    const QString message = QString::fromUtf8(data);
    emit instance_->externalProcessCommandReceived(message);
}
#endif

#if defined(Q_OS_WIN32)
void Plugin::timerEvent(QTimerEvent *e)
{
    e->accept();
    ipcShm_->lock();
    QString command = QString::fromUtf8((char*)ipcShm_->data());
    if (command.length() > 0 ) {
        qDebug() << "Have a message in SHM : " << command;
        emit externalProcessCommandReceived(command);
    }
    memset(ipcShm_->data(), 0, ipcShm_->size());
    ipcShm_->unlock();
}
#endif


void Plugin::updateSettings(const QStringList & keys)
{
    foreach (Widgets::SecondaryWindow * window, secondaryWindows_) {
        const QString prefix = window->settingsKey() + "/";
        bool hasPrefix = false;
        foreach (const QString & key, keys) {
            if (key.startsWith(prefix)) {
                hasPrefix = true;
                break;
            }
        }
        if (keys.isEmpty() || hasPrefix) {
            QStringList windowKeys;
            foreach (const QString & key , keys) {
                if (key.startsWith(prefix)) {
                    windowKeys.push_back(key);
                }
            }
            window->updateSettings(mySettings(), windowKeys);
        }
    }
    if (mainWindow_)
        mainWindow_->updateSettings(mySettings(), keys);
}

QWidget* Plugin::settingsEditorPage()
{
    if (!guiSettingsPage_) {
        guiSettingsPage_ = new GUISettingsPage(mySettings(), 0);
        connect(guiSettingsPage_, SIGNAL(settingsChanged(QStringList)),
                this, SLOT(updateSettings(QStringList)));
    }
    return guiSettingsPage_;
}

void Plugin::showActorWindow(const QByteArray &asciiName)
{
    if (showActorActions_.contains(asciiName)) {
        showActorActions_[asciiName]->trigger();
    }
}

void Plugin::changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState state)
{
    using namespace Shared;
    if (state==PluginInterface::GS_Unlocked) {
//        m_kumirStateLabel->setText(tr("Editing"));
        mainWindow_->clearMessage();
        mainWindow_->setFocusOnCentralWidget();
        mainWindow_->unlockActions();
        debugger_->reset();
        debugger_->setDebuggerEnabled(false);
    }
    else if (state==PluginInterface::GS_Observe) {
//        m_kumirStateLabel->setText(tr("Observe"));
        mainWindow_->showMessage(kumirProgram_->endStatusText());
        mainWindow_->setFocusOnCentralWidget();
        mainWindow_->unlockActions();
        debugger_->setDebuggerEnabled(kumirProgram_->endStatus() == KumirProgram::Exception);
    }
    else if (state==PluginInterface::GS_Running) {
//        m_kumirStateLabel->setText(tr("Running"));
        mainWindow_->clearMessage();
        mainWindow_->lockActions();
    }
    else if (state==PluginInterface::GS_Pause) {
//        m_kumirStateLabel->setText(tr("Pause"));
        mainWindow_->lockActions();
        debugger_->setDebuggerEnabled(true);
    }
    else if (state==PluginInterface::GS_Input) {
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
    kumirProgram_->setEditorInstance(twe->editor());
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
        if (!sessionsDisableFlag_) ExtensionSystem::PluginManager::instance()->switchToDefaultWorkspace();
        else {
            updateSettings(QStringList());
            restoreSession();
        }
    }
    PluginManager::instance()->switchGlobalState(PluginInterface::GS_Unlocked);
    mainWindow_->show();
    if (fileNameToOpenOnReady_.length() > 0) {
        mainWindow_->loadFromUrl(QUrl::fromLocalFile(fileNameToOpenOnReady_), true);
    }
}

void Plugin::stop()
{

}


void Plugin::saveSession() const
{
    mainWindow_->saveSession();
    mainWindow_->saveSettings();
    foreach (Widgets::SecondaryWindow * secWindow, secondaryWindows_)
        secWindow->saveState();
}


void Plugin::restoreSession()
{
    if (!sessionsDisableFlag_) {        
    }
    else {
        mainWindow_->newProgram();
    }
    foreach (Widgets::SecondaryWindow * secWindow, secondaryWindows_)
        secWindow->restoreState();
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

void Plugin::showCoursesWindow(const QString & id)
{
    if (courseManager_ && !id.isEmpty()) {
        courseManager_->activateCourseFromList(id);
    }
    if (coursesWindow_) {
        coursesWindow_->activate();
    }
}

QStringList Plugin::coursesList(bool fullPaths) const
{
    const QStringList files = courseManager_->getListOfCourses();
    if (fullPaths)
        return files;
    else {
        QStringList result;
        for (int i=0; i<files.size(); i++) {
            result << QFileInfo(files[i]).fileName();
        }
        return result;
    }
}

void Plugin::showHelpWindow(int index)
{
    if (helpWindow_) {
        helpWindow_->activate();
    }
    if (helpViewer_) {
        helpViewer_->activateBookIndex(index);
    }
}

QStringList Plugin::helpList() const
{
    return helpViewer_ ? helpViewer_->booksList() : QStringList();
}

QString Plugin::wsName() const
{
#ifdef Q_WS_X11
    return "x11";
#endif
#ifdef Q_WS_WIN32
    return "win32";
#endif
#ifdef Q_WS_MAC
    return "mac";
#endif
    return "";
}


} // namespace CoreGUI

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(CoreGui, CoreGUI::Plugin)
#endif
