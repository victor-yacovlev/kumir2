#include "mainwindow.h"
#include "side.h"
#include "ui_mainwindow.h"
#include "tabwidgetelement.h"
#include "extensionsystem/pluginmanager.h"
#include "aboutdialog.h"
#include "kumirprogram.h"
#include "dataformats/kumfile.h"
#include "statusbar.h"
#include "tabwidget.h"
#include "systemopenfilesettings.h"
#include "guisettingspage.h"
#include "widgets/iconprovider.h"
#include "toolbarcontextmenu.h"

#include <algorithm>
#include <QSharedPointer>

namespace CoreGUI {

static const int DefaultConsoleHeight = 100;

MainWindow::MainWindow(Plugin * p) :
    QMainWindow(0),
    ui(new Ui::MainWindow),
    m_plugin(p),
    statusBar_(new StatusBar),
    tabWidget_(0),
    prevBottomSize_(DefaultConsoleHeight),
    menubarContextMenu_(0),
    afterShowTimerId2_(0),
    afterShowTimerId3_(0)
{   

    debuggerWindow_ = 0;

    ui->setupUi(this);    
    ui->menuEdit->setProperty("menuRole", "edit");
    ui->menuInsert->setProperty("menuRole", "insert");


    tabWidget_ = new TabWidget(this);    
    helpAndCoursesPlace_ = new Widgets::DockWindowPlace(this, "MainWindow/HelpDockPlace");    
    debuggerPlace_ = new Widgets::DockWindowPlace(this, "MainWindow/DebuggerDockPlace");    
    consolePlace_ = new Widgets::DockWindowPlace(this, "MainWindow/ConsoleDockPlace");    
    actorsPlace_ = new Widgets::DockWindowPlace(this, "MainWindow/ActorsDockPlace");    
    centralSide_ = new Side(this, "MainWindow/CentralRow");    
    secondarySide_ = new Side(this, "MainWindow/BottomRow");    
    ui->splitter->setCollapsible(1, true);

    connect(secondarySide_, SIGNAL(visiblityRequest()), this, SLOT(ensureSeconrarySideVisible()));    
    connect(ui->actionShow_Console_Pane, SIGNAL(triggered(bool)), this, SLOT(setConsoleVisible(bool)));    

    setStatusBar(statusBar_);    
    setMinimumHeight(380);

    tabsDisabledFlag_ = false;    
    b_workspaceSwitching = false;


    ui->actionNewProgram->setIcon(Widgets::IconProvider::self()->iconForName("file-new"));
    ui->actionNewProgram->setObjectName("file-new");
    ui->actionOpen->setIcon(Widgets::IconProvider::self()->iconForName("file-open"));
    ui->actionOpen->setObjectName("file-open");
    ui->actionSave->setIcon(Widgets::IconProvider::self()->iconForName("file-save"));
    ui->actionSave->setObjectName("file-save");


    ui->menuFile->setWindowTitle(ui->menuFile->title());
    ui->menuHelp->setWindowTitle(ui->menuHelp->title());

    connect(ui->actionNewProgram, SIGNAL(triggered()), this, SLOT(newProgram()));
    connect(ui->actionNewText, SIGNAL(triggered()), this, SLOT(newText()));

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(closeCurrentTab()));

    connect(ui->actionSwitch_workspace, SIGNAL(triggered()), this, SLOT(switchWorkspace()));
    connect(tabWidget_, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(setupActionsForTab()));
    connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(setupContentForTab()));
    connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(setupStatusbarForTab()));
    connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(checkCounterValue()));
    connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(setTitleForTab(int)));

    connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(showPreferences()));

    gr_fileActions = new QActionGroup(this);
    gr_fileActions->addAction(ui->actionNewProgram);
    gr_fileActions->addAction(ui->actionOpen);
    gr_fileActions->addAction(ui->actionSave);
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveCurrentFile()));
    connect(ui->actionSave_as, SIGNAL(triggered()), this, SLOT(saveCurrentFileAs()));
    gr_otherActions = new QActionGroup(this);
    gr_otherActions->setExclusive(false);
    a_notAvailable = new QAction(this);
    a_notAvailable->setText(tr("No actions for this tab"));
    a_notAvailable->setEnabled(false);

    menuNA1_ = new QMenu(this);
    menuNA1_->addAction(a_notAvailable);

    a_notAvailable2 = new QAction(this);
    a_notAvailable2->setText(tr("No actions for this tab"));
    a_notAvailable2->setEnabled(false);

    menuNA2_ = new QMenu(this);
    menuNA2_->addAction(a_notAvailable2);

    a_notAvailable3 = new QAction(this);
    a_notAvailable3->setText(tr("No actions for this tab"));
    a_notAvailable3->setEnabled(false);

    menuNA3_ = new QMenu(this);
    menuNA3_->addAction(a_notAvailable3);

    menu_empty = new QMenu(this);
    menu_empty->addAction(a_notAvailable);

    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);

    i_timerId = startTimer(250);   

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));   
    connect(ui->actionUsage, SIGNAL(triggered()), this, SLOT(showUserManual()));

    installEventFilter(this);

#ifndef Q_OS_MAC
    installEventFilter(menuBar());
#endif

    ui->actionRecent_files->setMenu(new QMenu());
    connect(ui->menuFile, SIGNAL(aboutToShow()), this, SLOT(prepareRecentFilesMenu()));

    if (m_plugin->sessionsDisableFlag_) {
        ui->actionRestore_previous_session->setEnabled(false);
        ui->actionRestore_previous_session->setVisible(false);
    }
    else {
        connect(ui->actionRestore_previous_session, SIGNAL(triggered()), this, SLOT(restoreSession()));
    }

    tabWidget_->setAcceptDrops(true);
    tabWidget_->installEventFilter(this);

    using namespace Shared;
    using namespace ExtensionSystem;
    PluginManager * manager = PluginManager::instance();

    createSettingsDialog();
    ui->menuFile->removeAction(ui->actionSave_all); // Not implemented yet
    ui->menuFile->removeAction(ui->actionRestore_previous_session); // Not implemented yet

    ui->actionMake_native_executable->setVisible(manager->isPluginLoaded("LLVMCodeGenerator"));
    connect(ui->actionMake_native_executable, SIGNAL(triggered()),
            this, SLOT(makeNativeExecutable()));

    menubarContextMenu_ = new ToolbarContextMenu(this);
    ui->menubar->setContextMenuWidget(menubarContextMenu_);
    topLevelMenus_ << ui->menuFile << ui->menuEdit << ui->menuInsert
                   << ui->menuRun << ui->menuRun << ui->menuHelp;


    bool useGlobalMenu = false;
#ifdef Q_OS_MAC
    useGlobalMenu = true;
#endif
#ifdef Q_OS_LINUX
    const char * session = ::getenv("SESSION");
    if (session && QString::fromLatin1(session).toLower() == "ubuntu") {
        useGlobalMenu = true;
    }
#endif
    if (useGlobalMenu) {
        QMenu * fakeMenu = ui->menubar->addMenu(menubarContextMenu_->showAction()->text());
        menubarContextMenu_->showAction()->setText(tr("Configure menu items..."));
        fakeMenu->addAction(menubarContextMenu_->showAction());
    }
    else {
        ui->menubar->addAction(menubarContextMenu_->showAction());
    }
}

void MainWindow::addMenuBeforeHelp(QMenu *menu)
{
    ui->menubar->insertMenu(ui->menuHelp->menuAction(), menu);
    int helpIndex = topLevelMenus_.indexOf(ui->menuHelp);
    if (-1 != helpIndex) {
        topLevelMenus_.insert(helpIndex, menu);
    }
    else {
        topLevelMenus_.append(menu);
    }
}

void MainWindow::setupMenuBarContextMenu()
{

    menubarContextMenu_->setSettingsObject(m_plugin->mySettings(), "MenuBarItems");

    Q_FOREACH(QMenu* menu, topLevelMenus_) {
        if (menu) {
            QAction * menuAction = menu->menuAction();
            const QString objectName = menu->objectName();
            menuAction->setObjectName(objectName);
            menubarContextMenu_->addProxy(menuAction);
        }
    }

    menubarContextMenu_->finalize();
}

void MainWindow::updateVisibleMenus()
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(sender());
    if (twe && twe == tabWidget_->currentWidget()) {
        updateVisibleMenus(twe);
    }
}

void MainWindow::updateVisibleMenus(int index)
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->widget(index));
    if (twe) {
        updateVisibleMenus(twe);
    }
}

void MainWindow::updateVisibleMenus(TabWidgetElement *twe)
{
    menubarContextMenu_->setExplicitImportNames(twe->availableMenuNames());
}

void MainWindow::handleBreakpointCnagedOrInserted(bool enabled, quint32 lineNo, quint32 ignoreCount, const QString &condition)
{
    using namespace Shared::Editor;
    InstanceInterface * editor = qobject_cast<InstanceInterface*>(sender());
    const QString fileName = editor->documentContents().sourceUrl.toLocalFile();
    KumirProgram::runner()->insertOrChangeBreakpoint(enabled, fileName, lineNo, ignoreCount, condition);
}

void MainWindow::handleBreakpointRemoved(quint32 lineNo)
{
    using namespace Shared::Editor;
    InstanceInterface * editor = qobject_cast<InstanceInterface*>(sender());
    const QString fileName = editor->documentContents().sourceUrl.toLocalFile();
    KumirProgram::runner()->removeBreakpoint(fileName, lineNo);
}


//QString MainWindow::StatusbarWidgetCSS =
//        "QLabel {"
//        "   font-size: 16px;"
//        "   padding: 4px;"
//        "   border: 2px solid darkgray;"
//        "   margin: 0;"
//        "}"
//;

void MainWindow::changeFocusOnMenubar()
{
    QMenuBar * mb = menuBar();
    QMenu * firstMenu = mb->findChild<QMenu*>();
    if (!mb->hasFocus()) {
        mb->setFocus();
        if (firstMenu) {
            mb->setActiveAction(firstMenu->menuAction());
        }
    }
    else {
        mb->setActiveAction(0);
        setFocusOnCentralWidget();
    }
}

void MainWindow::prepareLayoutChange()
{

    helpAndCoursesPlace_->disconnect(SIGNAL(resizeRequest(QSize)));
    helpAndCoursesPlace_->disconnect(SIGNAL(visiblityRequest(bool,QSize)));

    debuggerPlace_->disconnect(SIGNAL(visiblityRequest(bool,QSize)));    
    debuggerPlace_->disconnect(SIGNAL(visiblityRequest(bool,QSize)));

    consolePlace_->disconnect(SIGNAL(visiblityRequest(bool,QSize)));
    consolePlace_->disconnect(SIGNAL(visiblityRequest(bool,QSize)));

    actorsPlace_->disconnect(SIGNAL(visiblityRequest(bool,QSize)));
    actorsPlace_->disconnect(SIGNAL(visiblityRequest(bool,QSize)));

    centralSide_->disconnect(SIGNAL(splitterMoved(int,int)), this, SLOT(checkForConsoleHiddenBySplitter(int,int)));    
    ui->splitter->disconnect(SIGNAL(splitterMoved(int,int)), this, SLOT(checkForConsoleHiddenBySplitter(int,int)));


    helpAndCoursesPlace_->setParent(0);    
    debuggerPlace_->setParent(0);    
    consolePlace_->setParent(0);    
    actorsPlace_->setParent(0);   

    centralSide_->setParent(0);    
    secondarySide_->setParent(0);

}

QMap<QWidget*,QSize> MainWindow::saveSizes() const
{
    QMap<QWidget*,QSize> result;
    if (helpAndCoursesPlace_->isVisible())
        result[helpAndCoursesPlace_] = helpAndCoursesPlace_->size();
    if (debuggerPlace_->isVisible())
        result[debuggerPlace_] = debuggerPlace_->size();
    if (consolePlace_->isVisible())
        result[consolePlace_] = consolePlace_->size();
    if (actorsPlace_->isVisible())
        result[actorsPlace_] = actorsPlace_->size();

    return result;
}

void MainWindow::restoreSizes(const QMap<QWidget *, QSize> &sizes, const Qt::Orientation o)
{
    const int W = centralWidget()->width();
    const int H = centralWidget()->height();
    if (o == Qt::Vertical) {
        // Rows first
        int bottomH = sizes.contains(consolePlace_)
                ? sizes[consolePlace_].height() : 0;
        if (sizes.contains(debuggerPlace_))
            bottomH = qMax(bottomH, sizes[debuggerPlace_].height());
        if (sizes.contains(actorsPlace_))
            bottomH = qMax(bottomH, sizes[actorsPlace_].height());
        QList<int> centralRowSizes, bottomRowSizes;
        if (sizes.contains(helpAndCoursesPlace_)) {
            centralRowSizes << 0 << sizes[helpAndCoursesPlace_].width();
            centralRowSizes[0] = W - centralRowSizes[1] - centralSide_->handleWidth();
        }
        else {
            centralRowSizes << W << 0;
        }

        int bottomSplitters = 0;
        bottomRowSizes << 0 << 0 << 0;
        if (sizes.contains(debuggerPlace_)) {
            bottomSplitters ++;
            bottomRowSizes[0] = sizes[debuggerPlace_].width();
        }
        if (sizes.contains(actorsPlace_)) {
            bottomSplitters ++;
            bottomRowSizes[2] = sizes[actorsPlace_].width();
        }
        bottomRowSizes[1] = W - bottomRowSizes[0] - bottomRowSizes[2] - bottomSplitters * secondarySide_->handleWidth();
        QList<int> mainSizes; mainSizes << 0 << 0;
        if (!sizes.contains(helpAndCoursesPlace_) && sizes.contains(actorsPlace_)) {
            if (sizes.contains(consolePlace_) && consolePlace_->height() > 0)
                bottomH = consolePlace_->height();
            else
                bottomH = H / 2;
        }
        mainSizes[1] = bottomH;
        mainSizes[0] = H - bottomH - ui->splitter->handleWidth();
        centralSide_->setSizes(centralRowSizes);
        secondarySide_->setSizes(bottomRowSizes);
        ui->splitter->setSizes(mainSizes);
    }
    else {
        // Columns first
        int rightW = sizes.contains(helpAndCoursesPlace_)
                ? sizes[helpAndCoursesPlace_].width() : 0;
        QList<int> centralColSizes, rightColSizes;
        centralColSizes << 0 << 0;
        if (sizes.contains(consolePlace_)) {
            centralColSizes[1] = sizes[consolePlace_].height();
            centralColSizes[0] = H - centralColSizes[1] - centralSide_->handleWidth();
        }
        else {
            centralColSizes[0] = H - centralSide_->handleWidth();
        }

        rightColSizes << 0 << 0;
        if (sizes.contains(actorsPlace_)) {
            rightColSizes[1] = sizes[actorsPlace_].height();
            rightColSizes[0] = H - secondarySide_->handleWidth() - rightColSizes[1];
            rightW = qMax(rightW, sizes[actorsPlace_].width());
        }
        else {
            rightColSizes[1] = sizes[actorsPlace_].height();
        }


        QList<int> mainSizes; mainSizes << 0 << 0;
        if (rightW > 0) {
            mainSizes[1] = rightW;
            mainSizes[0] = W - rightW - ui->splitter->handleWidth();
        }
        else {
            mainSizes[0] = W;
        }
        ui->splitter->setSizes(mainSizes);
        centralSide_->setSizes(centralColSizes);
        secondarySide_->setSizes(rightColSizes);
    }
}

void MainWindow::switchToRowFirstLayout()
{
    QMap<QWidget*,QSize> visibleSizes = saveSizes();

    prepareLayoutChange();

    ui->splitter->setOrientation(Qt::Vertical);
    centralSide_->setOrientation(Qt::Horizontal);
    secondarySide_->setOrientation(Qt::Horizontal);

    debuggerWindow_->changeDockPlace(debuggerPlace_);

    ui->splitter->addWidget(centralSide_);
    ui->splitter->addWidget(secondarySide_);


    centralSide_->addComponent(tabWidget_, true);
    centralSide_->addComponent(helpAndCoursesPlace_, false);

    secondarySide_->addComponent(debuggerPlace_, false);
    secondarySide_->addComponent(consolePlace_, true);

    secondarySide_->addComponent(actorsPlace_, false);

    connect(helpAndCoursesPlace_, SIGNAL(visiblityRequest(bool,QSize)),
            centralSide_, SLOT(handleVisiblityRequest(bool,QSize)),
            Qt::DirectConnection);
    connect(helpAndCoursesPlace_, SIGNAL(resizeRequest(QSize)),
            centralSide_, SLOT(forceResizeItem(QSize)),
            Qt::DirectConnection);

    connect(debuggerPlace_, SIGNAL(visiblityRequest(bool,QSize)),
            secondarySide_, SLOT(handleVisiblityRequest(bool,QSize)),
            Qt::DirectConnection);
    connect(debuggerPlace_, SIGNAL(resizeRequest(QSize)),
            centralSide_, SLOT(forceResizeItem(QSize)),
            Qt::DirectConnection);

    connect(consolePlace_, SIGNAL(visiblityRequest(bool,QSize)),
            secondarySide_, SLOT(handleVisiblityRequest(bool,QSize)),
            Qt::DirectConnection);
    connect(consolePlace_, SIGNAL(resizeRequest(QSize)),
            centralSide_, SLOT(forceResizeItem(QSize)),
            Qt::DirectConnection);

    connect(actorsPlace_, SIGNAL(visiblityRequest(bool,QSize)),
            secondarySide_, SLOT(handleVisiblityRequest(bool,QSize)),
            Qt::DirectConnection);
    connect(actorsPlace_, SIGNAL(resizeRequest(QSize)),
            centralSide_, SLOT(forceResizeItem(QSize)),
            Qt::DirectConnection);

    connect(ui->splitter, SIGNAL(splitterMoved(int,int)), this, SLOT(checkForConsoleHiddenBySplitter(int,int)));

    centralSide_->setCollapsible(0, false);
    centralSide_->setCollapsible(1, /*true*/ false);
    secondarySide_->setCollapsible(0, /*true*/ false);
    secondarySide_->setCollapsible(1, /*true*/ false);
    centralSide_->setVisible(true);
    restoreSizes(visibleSizes, Qt::Vertical);
}

void MainWindow::switchToColumnFirstLayout()
{
    QMap<QWidget*,QSize> visibleSizes = saveSizes();

    prepareLayoutChange();

    ui->splitter->setOrientation(Qt::Horizontal);
    centralSide_->setOrientation(Qt::Vertical);
    secondarySide_->setOrientation(Qt::Vertical);

    debuggerWindow_->changeDockPlace(helpAndCoursesPlace_);

    ui->splitter->addWidget(centralSide_);
    ui->splitter->addWidget(secondarySide_);
    ui->splitter->setCollapsible(0, false);
    ui->splitter->setCollapsible(1, /*true*/ false);

    centralSide_->addComponent(tabWidget_, true);
    centralSide_->addComponent(consolePlace_, true);

    secondarySide_->addComponent(helpAndCoursesPlace_, false);
    secondarySide_->addComponent(actorsPlace_, false);

    connect(helpAndCoursesPlace_, SIGNAL(visiblityRequest(bool,QSize)),
            secondarySide_, SLOT(handleVisiblityRequest(bool,QSize)),
            Qt::DirectConnection);
    connect(helpAndCoursesPlace_, SIGNAL(resizeRequest(QSize)),
            secondarySide_, SLOT(forceResizeItem(QSize)),
            Qt::DirectConnection);

    connect(debuggerPlace_, SIGNAL(visiblityRequest(bool,QSize)),
            secondarySide_, SLOT(handleVisiblityRequest(bool,QSize)),
            Qt::DirectConnection);
    connect(debuggerPlace_, SIGNAL(resizeRequest(QSize)),
            secondarySide_, SLOT(forceResizeItem(QSize)),
            Qt::DirectConnection);

    connect(consolePlace_, SIGNAL(visiblityRequest(bool,QSize)),
            centralSide_, SLOT(handleVisiblityRequest(bool,QSize)),
            Qt::DirectConnection);
    connect(consolePlace_, SIGNAL(resizeRequest(QSize)),
            secondarySide_, SLOT(forceResizeItem(QSize)),
            Qt::DirectConnection);

    connect(actorsPlace_, SIGNAL(visiblityRequest(bool,QSize)),
            secondarySide_, SLOT(handleVisiblityRequest(bool,QSize)),
            Qt::DirectConnection);
    connect(actorsPlace_, SIGNAL(resizeRequest(QSize)),
            secondarySide_, SLOT(forceResizeItem(QSize)),
            Qt::DirectConnection);

    connect(centralSide_, SIGNAL(splitterMoved(int,int)), this, SLOT(checkForConsoleHiddenBySplitter(int,int)));

    centralSide_->setCollapsible(0, false);
    centralSide_->setCollapsible(1, /*true*/ false);
    secondarySide_->setCollapsible(0, /*true*/ false);
    secondarySide_->setCollapsible(1, /*true*/ false);
    centralSide_->setVisible(true);
    restoreSizes(visibleSizes, Qt::Horizontal);
}

QSize MainWindow::minimumSizeHint() const
{
    int minW = statusBar_->minimumSizeHint().width() + 10;
    int minH = qMax(200, centralWidget()->minimumHeight()) +
            ui->menubar->height() +
            statusBar_->minimumHeight() + 10;
    int minDockedH = m_plugin->terminal_->minimumHeight();
    int minDockedW = 0;
//    for (int i=0; i<m_plugin->secondaryWindows_.size(); i++) {
//        Widgets::SecondaryWindow * w = m_plugin->secondaryWindows_[i];
//        if (w->isVisible() && !w->isFloating()) {
//            minDockedH = qMax(minDockedH, w->minimumHeight());
//            minDockedW = qMax(minDockedW, w->minimumWidth());
//        }
//    }
    int minBottom = m_plugin->terminal_->minimumWidth() +
            minDockedW + 10;
    minW = qMax(minW, minBottom);
    minH += minDockedH;
    return QSize(minW, qMax(380, minH));
}

void MainWindow::resizeEvent(QResizeEvent *evt)
{
    const QSize min = minimumSizeHint();
    const QSize sz = evt->size();
    if (sz.width() < min.width() || sz.height() < min.height()) {
        evt->accept();
    }
    else {
        resize(sz);
        evt->accept();
    }
}

bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
#ifndef Q_OS_MAC
    if (o==this && e->type()==QEvent::KeyPress) {
        QKeyEvent * ke = (QKeyEvent*)(e);
        if (ke->key()==Qt::Key_F10 && ke->modifiers()==0)
        {
            changeFocusOnMenubar();
            return true;
        }
        if (ke->key()==Qt::Key_Alt || ke->key()==Qt::Key_AltGr) {
            return true;
        }
    }
#endif
    if (e->type() == QEvent::DragEnter && o == tabWidget_) {
        QDragEnterEvent * event = static_cast<QDragEnterEvent*>(e);
        const QMimeData * data = event->mimeData();
        if (data->hasUrls() && data->urls().size()>0)
        {
//            qDebug() << "Matched: " << data->urls();
            QList<QUrl> validUrls;
            foreach (const QUrl & url, data->urls()) {
                if (url.isValid()
                        && !url.isEmpty()
                        && url.scheme()=="file"
                        )
                {
                    static const QStringList validSuffices = tabsDisabledFlag_
                            ? ( QStringList() << "kum" )
                            : ( QStringList() << "kum" << "txt"
                                << "html" << "htm" );
                    const QFileInfo fileInfo(url.toLocalFile());
                    if (
                            fileInfo.isReadable()
                            && fileInfo.isFile()
                            && validSuffices.contains(
                                fileInfo.completeSuffix() )
                            )
                    {
                        validUrls.append(url);
                    }
                }
            }
            const bool canAccept = tabsDisabledFlag_
                    ? validUrls.size() == 1
                    : validUrls.size() >= 1;
            if (canAccept) {
                event->accept();
            }
            else {
                event->ignore();
            }
            return true;
        }
    }
    if (e->type() == QEvent::Drop && o == tabWidget_) {
//        qDebug() << "Drop!";
        QDropEvent * event = static_cast<QDropEvent*>(e);
        const QMimeData * data = event->mimeData();
        foreach (const QUrl & url, data->urls()) {
            loadFromUrl(url, true);
        }
        e->accept();
        return true;
    }
//    if (e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease) {
//        QKeyEvent * ke = (QKeyEvent*)(e);
//        if (m_plugin->m_terminal->isActiveComponent()) {
//            if (ke->matches(QKeySequence::Copy) ||
//                    ke->matches(QKeySequence::Paste) ||
//                    ke->matches(QKeySequence::Cut) ||
//                    ke->matches(QKeySequence::SelectAll)
//                    )
//            {
//                return true;
//            }
//        }
//    }
    return false;
}


void MainWindow::lockActions()
{
    ui->actionRestore_previous_session->setEnabled(false);
    ui->actionSwitch_workspace->setEnabled(false);

    ui->actionPreferences->setEnabled(false);

    if (tabsDisabledFlag_) {
        ui->actionNewProgram->setEnabled(false);
        ui->actionOpen->setEnabled(false);
        ui->actionRecent_files->setEnabled(false);
    }
    else {
        for (int i=0; i<tabWidget_->count(); i++) {
            TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->widget(i));
            if (twe && twe->type==Program && twe->kumirProgram()) {
                if (twe->kumirProgram()->isRunning()) {
                    twe->setProperty("uncloseable", true);
                }
            }
        }
        setupActionsForTab();
    }
}

void MainWindow::unlockActions()
{

    ui->actionRestore_previous_session->setEnabled(true);
    ui->actionSwitch_workspace->setEnabled(true);

    ui->actionPreferences->setEnabled(true);

    if (tabsDisabledFlag_) {
        ui->actionNewProgram->setEnabled(true);
        ui->actionOpen->setEnabled(true);
        ui->actionRecent_files->setEnabled(true);
    }
    else {
        for (int i=0; i<tabWidget_->count(); i++) {
            TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->widget(i));
            if (twe && twe->type==Program && twe->kumirProgram()) {
                twe->setProperty("uncloseable", false);
            }
        }
        setupActionsForTab();
    }
}

TabWidgetElement * MainWindow::currentTab()
{
    if (tabWidget_->count() == 0)
        return nullptr;
    else
        return qobject_cast<TabWidgetElement*>(tabWidget_->currentWidget());
}

void MainWindow::checkCounterValue()
{
    using namespace ExtensionSystem;
    using namespace Shared;
    PluginInterface::GlobalState state = PluginManager::instance()->currentGlobalState();
    if (state==PluginInterface::GS_Unlocked) {
        TabWidgetElement * twe = currentTab();
        if (!twe)
            return;
        if (twe->editor()) {
            Editor::InstanceInterface * editor = twe->editor();
            quint32 errorsCount = editor->errorLinesCount();
            statusBar_->setErrorsCounter(errorsCount);
        }
        else {
            statusBar_->setErrorsCounter(0);
        }
    }
    else {
        RunInterface * runner =
                PluginManager::instance()->findPlugin<RunInterface>();
        if (runner) {
            ulong stepsCounted = runner->stepsCounted();
            ulong stepsDone =
                    state==PluginInterface::GS_Observe && runner->error().length()==0
                    ? stepsCounted  // all steps successfully finished
                    : stepsCounted - 1; // all but last unfinished step
            if (stepsCounted == 0) {
                stepsDone = 0;  // just begin of evaluation
            }
            statusBar_->setStepsDoneCounter(stepsDone);
        }
    }
}



void MainWindow::showMessage(const QString &text)
{
    statusBar_->setMessage(text);
//    m_message->setText(text);
}

void MainWindow::clearMessage()
{
    statusBar_->unsetMessage();
//    m_message->setText("");
}

void MainWindow::setFocusOnCentralWidget()
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->currentWidget());
    if (twe)
        twe->component->setFocus();
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    e->accept();
    if (afterShowTimerId2_ == e->timerId()) {
        killTimer(afterShowTimerId2_);
        afterShowTimerId2_ = 0;
        setFirstTimeWindowLayout_stage2();
    }
    if (afterShowTimerId3_ == e->timerId()) {
        killTimer(afterShowTimerId3_);
        afterShowTimerId3_ = 0;
        setFirstTimeWindowLayout_stage3();
    }
    checkCounterValue();
}



bool MainWindow::saveCurrentFile()
{
    TabWidgetElement * twe = currentTab();
    if (!twe->editor())
        return true;
    const QString fileName = twe->editor()->documentContents().sourceUrl.toLocalFile();
    bool result = 0;
    if (fileName.isEmpty()) {
        result = saveCurrentFileAs();
    }
    else {
        result = saveCurrentFileTo(fileName);
    }
    if (result) {
        setTitleForTab(tabWidget_->currentIndex());
    }
    return result;
}

void MainWindow::handleTabTitleChanged()
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(sender());
    Q_ASSERT(twe);
    int index = tabWidget_->indexOf(twe);
    Q_ASSERT(index >= 0 && index < tabWidget_->count());
    setTitleForTab(index);
}

QString MainWindow::applicationTitle() const
{
    QString appName = tr("Kumir");
    using namespace Shared;
    using namespace ExtensionSystem;
    PluginManager * manager = PluginManager::instance();
    AnalizerInterface * analizer = manager->findPlugin<AnalizerInterface>();
    if (analizer) {
        appName = analizer->languageName();
    }
    return appName;
}

void MainWindow::setTitleForTab(int index)
{
    if (index<0 || index>=tabWidget_->count())
        return;
    QWidget * currentTabWidget = tabWidget_->widget(index);

    if (!currentTabWidget)
        return;

    TabWidgetElement * twe = currentTab();
    QString title = twe->title();

    setWindowTitle(title + " - "+ applicationTitle());
    tabWidget_->setTabText(index, title);
}

void MainWindow::updateStartPageTitle(const QString &title, const Shared::Browser::InstanceInterface * sender)
{
    for (int i=0; i<tabWidget_->count(); i++) {
        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->widget(i));
        if (twe->startPage() == sender) {
            tabWidget_->setTabText(i, title);
            if (tabWidget_->currentIndex() == i)
                setTitleForTab(i);
            return;
        }
    }
}

void MainWindow::setupActionsForTab()
{
    QWidget * currentTabWidget = tabWidget_->currentWidget();

    if (!currentTabWidget)
        return;

    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(currentTabWidget);
    ui->actionSave->setEnabled(twe->type!=StartPage);
    ui->actionSave_as->setEnabled(twe->type!=StartPage);

    ui->actionClose->setEnabled(!twe->property("uncloseable").toBool());
    ui->actionMake_native_executable->setEnabled(Program==twe->type);

    prepareEditMenu();
    prepareInsertMenu();
    prepareRunMenu();
}

void MainWindow::setupStatusbarForTab()
{
    for (int i=0; i<tabWidget_->count(); i++) {
        TabWidgetElement * w =
                qobject_cast<TabWidgetElement*>(tabWidget_->widget(i));
        if (w && w->component) {
            w->component->disconnect(statusBar_);
        }
    }
    QWidget * currentTabWidget = tabWidget_->currentWidget();
    if (!currentTabWidget)
        return;
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(currentTabWidget);
    if (twe->type == Program || twe->type == Text) {
        connect(twe->component,
                SIGNAL(cursorPositionChanged(uint,uint)),
                statusBar_,
                SLOT(setEditorCursorPosition(uint,uint))
                );
        connect(twe->component,
                SIGNAL(keyboardLayoutChanged(QLocale::Language,bool,bool,bool)),
                statusBar_,
                SLOT(setEditorKeyboardLayout(QLocale::Language,bool,bool,bool))
                );
        connect(twe->component, SIGNAL(recordMacroChanged(bool)),
                statusBar_, SLOT(setRecordIndicator(bool)));

    }
}

void MainWindow::prepareRecentFilesMenu()
{
    ui->actionRecent_files->menu()->clear();
    QStringList r = m_plugin->mySettings()->value(Plugin::RecentFilesKey).toStringList();
    bool hasAnyItem = false;
    for (int i=0; i<r.size(); i++) {
        QFile f(r[i]);
#ifndef Q_OS_WIN32
        if (!r[i].trimmed().isEmpty() && f.exists()) {
#else
        if (!r[i].trimmed().isEmpty()) {
#endif
            QAction * a = ui->actionRecent_files->menu()->addAction(QFileInfo(r[i]).fileName());
            a->setProperty("index", i);
            if (QFileInfo(r[i]).isRelative())
                r[i] = QDir::currentPath()+"/"+r[i];
            a->setProperty("fullPath", r[i]);
            connect(a, SIGNAL(triggered()), this, SLOT(loadRecentFile()));
            hasAnyItem = true;
        }
    }
    ui->actionRecent_files->setEnabled(hasAnyItem);
}

void MainWindow::loadRecentFile()
{
    QAction * a = qobject_cast<QAction*>(sender());
    if (a && a->property("fullPath").isValid()) {
//        int index = a->property("index").toInt();
        const QString fullPath = a->property("fullPath").toString();
        loadRecentFile(fullPath);
    }
}

void MainWindow::prepareRunMenu()
{
    ui->menuRun->clear();
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->currentWidget());
    if (!twe)
        return;
    if (twe->type==Program) {
        KumirProgram * program = m_plugin->kumirProgram_;
        ui->menuRun->addActions(program->actions()->actions());
        if (program->breakpointActions() && program->breakpointActions()->actions().size()>0) {
            ui->menuRun->addSeparator();
            ui->menuRun->addActions(program->breakpointActions()->actions());
        }
    }
    else {
        ui->menuRun->addAction(a_notAvailable);
    }
}

void MainWindow::prepareEditMenu()
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->currentWidget());
    QMenu * tabMenu = 0;

//    if (twe->type!=WWW) {
        for (int i=0; i<twe->menus.size(); i++) {
            const QString menuRole = twe->menus[i]->property("menuRole").toString();
            if ("edit" == menuRole) {
                tabMenu = twe->menus[i];
                break;
            }
        }
//    }

    if (tabMenu) {
        ui->menuEdit->menuAction()->setMenu(tabMenu);
//        foreach (const QObject * child, ui->menubar->children()) {
//            qDebug() << child->metaObject()->className();
//            if (QByteArray("QMenu") == child->metaObject()->className()) {
//                const QMenu * m = qobject_cast<const QMenu*>(child);
//                qDebug() << m->title();
//            }
//        }

    }
    else {
        ui->menuEdit->menuAction()->setMenu(menuNA2_);
        ui->menuEdit->clear();
        ui->menuEdit->addAction(a_notAvailable2);
    }
}

void MainWindow::prepareInsertMenu()
{
    if (!ui->menuInsert) {
        return;
    }
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->currentWidget());
    QMenu * tabMenu = 0;

//    if (twe->type!=WWW) {
        for (int i=0; i<twe->menus.size(); i++) {
            const QString menuRole = twe->menus[i]->property("menuRole").toString();
            if ("insert" == menuRole) {
                tabMenu = twe->menus[i];
                break;
            }
        }
//    }

    if (tabMenu) {
        ui->menuInsert->menuAction()->setMenu(tabMenu);
    }
    else {
        ui->menuInsert->clear();
        ui->menuInsert->addAction(a_notAvailable3);
    }
}

void MakeNativeExecutableWorker::run()
{
    const AST::DataPtr ast = analizer->compiler()->abstractSyntaxTree();
    QString mimeType;
    generator->generateExecutable(ast, buffer, mimeType, fileSuffix);
//    sleep(5);
}

void MakeNativeExecutableWorker::cancel()
{
    canceledMutex.lock();
    canceled = true;
    canceledMutex.unlock();
}

bool MakeNativeExecutableWorker::isCanceled() const
{
    bool result = false;
    canceledMutex.lock();
    result = canceled;
    canceledMutex.unlock();
    return result;
}

void MainWindow::makeNativeExecutable()
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->currentWidget());
    twe->editor()->ensureAnalized();

    MakeNativeExecutableWorker * worker = new MakeNativeExecutableWorker();
    connect(worker, SIGNAL(finished()), this, SLOT(saveNativeExecutable()));
    worker->analizer = twe->editor()->analizer();
    worker->generator = KumirProgram::kumirNativeGenerator();
    worker->fileName = twe->editor()->documentContents().sourceUrl.toLocalFile();
    worker->canceled = false;
    worker->progressDialog = new QMessageBox(
                QMessageBox::NoIcon,
                tr("Please wait..."),
                tr("Executable file generation in progress."),
                QMessageBox::Cancel,
                this
                );
    worker->progressDialog->button(QMessageBox::Cancel)->setText(tr("Cancel"));
    connect(worker->progressDialog->button(QMessageBox::Cancel), SIGNAL(clicked()),
            worker, SLOT(cancel()));
    worker->progressDialog->show();
    worker->start(QThread::IdlePriority);

}

void MainWindow::saveNativeExecutable()
{
    MakeNativeExecutableWorker * worker = qobject_cast<MakeNativeExecutableWorker*>(sender());
    worker->progressDialog->close();
    if (worker->isCanceled()) {
        worker->progressDialog->deleteLater();
        worker->deleteLater();
        return;
    }
    QString fileName = worker->fileName;
    QString fileSuffix = worker->fileSuffix;
#ifndef Q_OS_WIN32
    fileSuffix = "bin";
#endif
    if (fileSuffix.startsWith(".")) {
        fileSuffix.remove(0, 1);
    }
    if (!fileName.isEmpty() && fileName.endsWith(".kum")) {
        fileName = fileName.mid(0, fileName.length()-4);
        if (fileSuffix.length() > 0)
            fileName += "." + fileSuffix;
    }
    else if (!fileName.isEmpty() && !fileSuffix.isEmpty()) {
        fileName += "." + fileSuffix;
    }
    QString initialPath;
    if (fileName.isEmpty()) {
        QString lastFileName = m_plugin->mySettings()->value(Plugin::RecentFileKey).toString();
        if (lastFileName.isEmpty()) {
            initialPath = QDir::currentPath();
        }
        else {
            initialPath = QFileInfo(lastFileName).absoluteDir().absolutePath();
        }
        initialPath += "/" + suggestNewFileName(fileSuffix, worker->analizer, initialPath);
    }
    else {
        initialPath = fileName;
    }
    QStringList filter;
    filter << tr("Native executables (*.%1)").arg(fileSuffix);
    fileName = QFileDialog::getSaveFileName(this, tr("Save native executable"), initialPath, filter.join(";;"));
    if (!fileName.isEmpty()) {
#ifdef Q_OS_WIN32
        if (!fileName.endsWith("." + fileSuffix)) {
            fileName += "." + fileSuffix;
        }
#endif
        QString settingsEntry = fileName;
        if (settingsEntry.endsWith("." + fileSuffix)) {
            settingsEntry = settingsEntry.left(settingsEntry.length()-4);
        }
        settingsEntry += ".kum";
        m_plugin->mySettings()->setValue(Plugin::RecentFileKey, settingsEntry);
        QFile f(fileName);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(worker->buffer);
            f.close();
            QFile::Permissions ps = f.permissions();
            ps |= QFile::ExeGroup | QFile::ExeOwner | QFile::ExeOther;
            QFile::setPermissions(fileName, ps);
        }
    }
    worker->progressDialog->deleteLater();
    worker->deleteLater();
}

bool MainWindow::saveCurrentFileAs()
{
    using namespace ExtensionSystem;
    using namespace Shared;
    AnalizerInterface * analizer =
            PluginManager::instance()->findPlugin<AnalizerInterface>();
    const QString languageName = analizer->languageName();
    const QString fileNameSuffix = analizer->defaultDocumentFileNameSuffix();
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->currentWidget());
    QString fileName = twe->editor()->documentContents().sourceUrl.toLocalFile();
    QString initialPath;
    if (fileName.isEmpty()) {
        QString lastFileName = m_plugin->mySettings()->value(Plugin::RecentFileKey).toString();
        if (lastFileName.isEmpty()) {
            initialPath = QDir::currentPath();
        }
        else {
            initialPath = QFileInfo(lastFileName).absoluteDir().absolutePath();
        }
        const QString suffix = twe->type==Program ? fileNameSuffix : "txt";
        initialPath += "/" + suggestNewFileName(suffix, twe->editor()->analizer(), initialPath);
    }
    else {
        initialPath = fileName;
    }
    QStringList filter;    
    if (twe->type==Program) {
        filter << tr("%1 programs (*.%2)").arg(languageName).arg(fileNameSuffix);
    }    
    if (twe->type==Text) {
        filter << tr("Text files (*.txt)");
    }
    filter << tr("All files (*)");
    fileName = QFileDialog::getSaveFileName(this, tr("Save file"), initialPath, filter.join(";;"));
    if (!fileName.isEmpty()) {
        if (twe->type==Program && !fileName.endsWith("." + fileNameSuffix))
            fileName += "." + fileNameSuffix;
        if (saveCurrentFileTo(fileName)) {
            m_plugin->mySettings()->setValue(Plugin::RecentFileKey, fileName);
            twe->setProperty("fileName", fileName);
            twe->setProperty("realFileName", fileName);
            twe->setProperty("title", QFileInfo(fileName).fileName());
            int index = tabWidget_->indexOf(twe);
            tabWidget_->setTabText(index, QFileInfo(fileName).fileName());
            addToRecent(fileName);
            m_plugin->mySettings()->setValue(Plugin::RecentFileKey, fileName);
            setTitleForTab(tabWidget_->currentIndex());
            return true;
        }
    }
    return false;
}

bool MainWindow::saveCurrentFileTo(const QString &fileName)
{
    TabWidgetElement * twe = currentTab();
    QString error;
    twe->editor()->saveDocument(fileName, &error);
    if (error.length() > 0) {
        QMessageBox::critical(this, tr("Can't save file"), error);
        return false;
    }
    return true;
}

void MainWindow::handleDocumentCleanChanged(bool v)
{
//    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(sender());
//    int index = tabWidget_->indexOf(twe);
//    QString text = tabWidget_->tabText(index);
//    if (text.endsWith("*"))
//        text = text.left(text.length()-1);
//    if (!v)
//        text += "*";
//    twe->setProperty("title", text);
//    setTitleForTab(index);
}


void MainWindow::closeCurrentTab()
{
    closeTab(tabWidget_->currentIndex());
    if (tabWidget_->currentWidget())
        tabWidget_->currentWidget()->setFocus();
}

bool MainWindow::closeTab(int index)
{
    if (index==-1 || index>=tabWidget_->count())
        return true;
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->widget(index));
    if (twe->property("uncloseable").toBool()) {
        return false;
    }
    if (twe->type!=StartPage) {
        bool notSaved = twe->editor()->isModified();
        QMessageBox::StandardButton r;
        if (!notSaved || twe->isCourseManagerTab()) {
            r = QMessageBox::Discard;
        }
        else {
            tabWidget_->setCurrentIndex(index);
            QMessageBox messageBox(
                        QMessageBox::Question,
                        tr("Close editor"),
                        tr("Save current text?"),
                        QMessageBox::NoButton,
                        this
                        );
            QPushButton * btnSave =
                    messageBox.addButton(tr("Save"), QMessageBox::AcceptRole);
            QPushButton * btnDiscard =
                    messageBox.addButton(tr("Don't save"), QMessageBox::DestructiveRole);
            QPushButton * btnCancel =
                    messageBox.addButton(tr("Cancel closing"), QMessageBox::RejectRole);
            messageBox.setDefaultButton(btnSave);
            messageBox.exec();
            if (messageBox.clickedButton()==btnSave) {
                r = QMessageBox::Save;
            }
            if (messageBox.clickedButton()==btnDiscard) {
                r = QMessageBox::Discard;
            }
            if (messageBox.clickedButton()==btnCancel) {
                r = QMessageBox::Cancel;
            }
        }
        if (r==QMessageBox::Discard) {
            twe->deleteLater();
            tabWidget_->removeTab(index);
            return true;
        }
        else if (r==QMessageBox::Save) {
            if (saveCurrentFile()) {
                twe->deleteLater();
                tabWidget_->removeTab(index);
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
    }
    else {
        tabWidget_->removeTab(index);
        return true;
    }

}

void MainWindow::createSettingsDialog()
{
    using namespace ExtensionSystem;
    using namespace Shared;
    settingsDialog_ = new Widgets::MultiPageDialog(this);
    settingsDialog_->setWindowTitle(tr("Preferences"));
    settingsDialog_->setMinimumSize(800, 500);    
    PluginManager * manager = PluginManager::instance();
    QList<KPlugin*> plugins = manager->loadedPlugins();

    QList<QWidget*> generalPages;
    QList<QWidget*> languagePages;
    QList<QWidget*> actorPages;

    foreach (KPlugin * p, plugins) {
        QWidget * page = p->settingsEditorPage();
        QList<QWidget*> pages = p->settingsEditorPages();
        if (page && !pages.contains(page)) {
            pages.prepend(page);
        }
        if (!pages.isEmpty()) {
            AnalizerInterface * analizer = qobject_cast<AnalizerInterface*>(p);
            RunInterface * runner = qobject_cast<RunInterface*>(p);
            ActorInterface * actor = qobject_cast<ActorInterface*>(p);
            QList<QWidget*> * targetGroup = & generalPages;
            if (actor) {
                targetGroup = & actorPages;
            }
            else if (analizer || runner) {
                targetGroup = & languagePages;
            }
            targetGroup->append(pages);
        }
    }
    AnalizerInterface * analizer = manager->findPlugin<AnalizerInterface>();
#ifndef Q_OS_MACX
    if (analizer && "kum" == analizer->defaultDocumentFileNameSuffix()) {
        SystemOpenFileSettings * openFileSettings = new SystemOpenFileSettings;
        generalPages.append(openFileSettings);
    }
#endif
    const QString languageGroupTitle = analizer
            ? tr("%1 language settings").arg(analizer->languageName())
            : tr("Runtime settings");
    foreach (QWidget *p, generalPages) {
        settingsDialog_->addPage(tr("General settings"), p);
    }
    foreach (QWidget *p, languagePages) {
        settingsDialog_->addPage(languageGroupTitle, p);
    }
    foreach (QWidget *p, actorPages) {
        settingsDialog_->addPage(tr("Actor settings"), p);
    }
}

void MainWindow::showPreferences()
{    
    settingsDialog_->exec();
}


QIcon MainWindow::actionIcon(const QString &name)
{

    const QString iconsPath = ExtensionSystem::PluginManager::instance()->sharePath()+"/icons/";
    return QIcon::fromTheme(name, QIcon(iconsPath+name+".png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newProgram()
{
    using namespace ExtensionSystem;
    using namespace Shared;
    if (tabsDisabledFlag_ && !closeTab(tabWidget_->currentIndex())) {
        return;
    }
    AnalizerInterface * analizer =
            PluginManager::instance()->findPlugin<AnalizerInterface>();
    const QString suffix = analizer->defaultDocumentFileNameSuffix();
    DocumentType type = Program;
    Shared::Editor::InstanceInterface * editor =
            m_plugin->plugin_editor->newDocument(suffix, QDir::currentPath());

    QWidget* vc = editor->widget();
    connect(vc, SIGNAL(message(QString)), this, SLOT(showMessage(QString)));
    connect(vc, SIGNAL(requestHelpForAlgorithm(QString, QString)),
            this, SLOT(showAlgorithmHelp(QString, QString)));

    QString fileName = suggestNewFileName(suffix, editor->analizer());
    TabWidgetElement * e = addCentralComponent(
                fileName,
                vc,
                editor->toolBarActions(),
                editor->menus(),
                type);
    e->setEditor(editor);
    QObject * editorObject = dynamic_cast<QObject*>(editor);
    connect(editorObject, SIGNAL(breakpointCnagedOrInserted(bool,quint32,quint32,QString)),
            this, SLOT(handleBreakpointCnagedOrInserted(bool,quint32,quint32,QString)));
    connect(editorObject, SIGNAL(breakpointRemoved(quint32)),
            this, SLOT(handleBreakpointRemoved(quint32)));
    tabWidget_->setCurrentWidget(e);
    setupContentForTab();
    setTitleForTab(tabWidget_->indexOf(e));
    e->setFocus();

}

void MainWindow::showAlgorithmHelp(const QString &package, const QString & function)
{
    m_plugin->helpWindow_->activate();
    m_plugin->helpViewer_->navigateToApiFunction(package, function);
}

void MainWindow::newText()
{
    QString fileName = suggestNewFileName("txt", nullptr);
    newText(fileName, "");
}

void MainWindow::newText(const QString &fileName, const QString & text)
{    
    Shared::Editor::InstanceInterface * editor =
            m_plugin->plugin_editor->newDocument("", QDir::currentPath());

    Shared::Analizer::SourceFileInterface::Data data;
    data.canonicalSourceLanguageName = "";
    data.sourceUrl = fileName.isEmpty()
            ? QUrl() : QUrl::fromLocalFile(fileName);
    data.visibleText = text;
    QString error;
    editor->loadDocument(data, &error);

    QWidget * vc = editor->widget();
    connect(vc, SIGNAL(message(QString)), this, SLOT(showMessage(QString)));

    TabWidgetElement * e = addCentralComponent(
                QFileInfo(fileName).fileName(),
                vc,
                editor->toolBarActions(),
                editor->menus(),
                Text);
    e->setEditor(editor);
    tabWidget_->setCurrentWidget(e);
    setupContentForTab();
    setTitleForTab(tabWidget_->indexOf(e));
    e->setFocus();
}

QString MainWindow::suggestNewFileName(const QString &suffix,
                                       Shared::Analizer::InstanceInterface * analizer,
                                       const QString & dirName) const
{
    QDir d;
    if (dirName.isEmpty())
        d = QDir::current();
    else
        d = QDir(dirName);
    QStringList fileNames = d.entryList(QStringList() << "*."+suffix);
    for (int i=0; i<tabWidget_->count(); i++) {
        fileNames << tabWidget_->tabText(i);
    }
    QString result;
    if (!analizer || !analizer->helper() || analizer->helper()->suggestFileName().isEmpty())
        result = "newfile";
    else
        result = analizer->helper()->suggestFileName();
    int index = 0;
    while (fileNames.contains(result+"."+suffix)) {
        index ++;
        result = QString("newfile_%1").arg(index);
    }
    return result+"."+suffix;
}


TabWidgetElement * MainWindow::addCentralComponent(
    const QString &title
    , QWidget *c
    , const QList<QAction*> & toolbarActions
    , const QList<QMenu*> & menus
    , DocumentType type)
{
    class KumirProgram * kumir = 0;
    if (type==Program) {
        kumir = m_plugin->kumirProgram_;
    }
    TabWidgetElement * element = new TabWidgetElement(c,
                                                      m_plugin->mySettings(),
                                                      type != StartPage,
                                                      toolbarActions, menus, type,
                                                      gr_fileActions, gr_otherActions, kumir);

    connect(element, SIGNAL(documentCleanChanged(bool)), this, SLOT(handleDocumentCleanChanged(bool)));
    connect(element, SIGNAL(titleChanged(QString)), this, SLOT(handleTabTitleChanged()));
    connect(element, SIGNAL(explicitImportNamesRequest()), this, SLOT(updateVisibleMenus()));
    createTopLevelMenus(menus, true);
    tabWidget_->addTab(element, title);
    return element;
}


void MainWindow::createTopLevelMenus(const QList<QMenu*> & c, bool tabDependent)
{
    QList<QMenu*> menus;
    QMenuBar * mb = menuBar();
    for (int i=0; i<mb->children().size(); i++) {
        QMenu * m = qobject_cast<QMenu*>(mb->children()[i]);
        if (m)
            menus << m;
    }
    for (int i=0; i<c.size(); i++) {
        const QString title = c[i]->title().trimmed();
        const QString role = c[i]->property("menuRole").toString();
        bool found = false;
        for (int j=0; j<menus.size(); j++) {
            const QString menuRole = menus[j]->property("menuRole").toString();
            if (menuRole==role) {
                found = true;
                break;
            }
        }
        if (!found) {
            QMenu * menu = new QMenu(title, mb);
            menu->setWindowTitle(menu->title());
            //            menu->setTearOffEnabled(true);
            if (tabDependent) {
            }
            else {
                QList<QAction*> actions;
                for (int k=0; k<c[i]->children().size(); k++) {
                    QAction * a = qobject_cast<QAction*>(c[i]->children()[k]);
                    if (a)
                        actions << a;
                }
                menu->addActions(actions);
            }
            addMenuBeforeHelp(menu);
        }
    }
}

void MainWindow::setupContentForTab()
{
    QWidget * currentTabWidget = tabWidget_->currentWidget();

    if (!currentTabWidget)
        return;

    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(currentTabWidget);
    Shared::Editor::InstanceInterface * editor = twe->editor();
    m_plugin->kumirProgram_->setEditorInstance(editor);
    setupActionsForTab();
}

void MainWindow::disableTabs()
{
    tabWidget_->disableTabs();
    tabsDisabledFlag_ = true;
    ui->actionClose->setVisible(false);
    ui->actionClose->setEnabled(false);
    ui->actionNewText->setVisible(false);
    ui->actionNewText->setEnabled(false);
    ui->actionSave_all->setEnabled(false);
    ui->actionSave_all->setVisible(false);
}

void MainWindow::updateSettings(SettingsPtr settings, const QStringList & keys)
{
    settings_ = settings;    
    loadSettings(keys);
    for (int i=0; i<tabWidget_->count(); i++) {
        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->widget(i));
        if (twe) {
            twe->updateSettingsObject(settings);
        }
    }
}

bool MainWindow::isColumnFirstLayout() const
{
    if (!settings_) return true;
    const QString layoutChoice =
            settings_->value(GUISettingsPage::LayoutKey, GUISettingsPage::ColumnsFirstValue).toString();
    return layoutChoice == GUISettingsPage::ColumnsFirstValue;
}

void MainWindow::loadSettings(const QStringList & keys)
{
    if (keys.contains(GUISettingsPage::LayoutKey)) {
        if (isColumnFirstLayout()) {
            switchToColumnFirstLayout();
        }
        else {
            switchToRowFirstLayout();
        }
    }
    QRect r = settings_->value(Plugin::MainWindowGeometryKey,
                               QRect(QPoint(-1, -1), QSize(0, 0))).toRect();

    if (r.width() * r.height() == 0) {
        setFirstTimeWindowLayout();
    }
    else {
        // Restore saved layout
        if (r.width()>0 &&
                (keys.contains(Plugin::MainWindowGeometryKey) || keys.isEmpty())
                ) {
            resize(r.size());
            QPoint ps;
            if (r.topLeft() != QPoint(-1, -1)) {
                ps = r.topLeft();
            }
            else {
                const QSize screenSize = QApplication::desktop()->availableGeometry().size();
                int x = screenSize.width() - r.width();
                int y = screenSize.height() - r.height();
                x /= 2;
                y /= 2;
                ps = QPoint(x, y);
            }
            move(ps);
        }
        if (keys.size() == 1 && "MainWindowLayout" == keys[0]) {
            // do nothing on hot layout change
        }
        else {
            centralSide_->updateSettings(settings_, keys);
            centralSide_->setVisible(true); // always visible even has null default settings
            secondarySide_->updateSettings(settings_, keys);
        }
        if (keys.contains(Plugin::MainWindowSplitterStateKey+"0") || keys.isEmpty()) {
            QList<int> sizes;
            sizes << 0 << 0;
            sizes[0] = settings_->value(Plugin::MainWindowSplitterStateKey+"0", 0).toInt();
            sizes[1] = settings_->value(Plugin::MainWindowSplitterStateKey+"1", 0).toInt();
            prevBottomSize_ = settings_->value("SavedBottomSize", DefaultConsoleHeight).toInt();
            if (sizes[0] + sizes[1] > 0) {
            }
            else {
                sizes[1] = prevBottomSize_;
                sizes[0] = height() - sizes[1];
            }
            ui->splitter->setSizes(sizes);
            ui->actionShow_Console_Pane->setChecked(sizes[1] > 0);
        }
    }
    menubarContextMenu_->loadSettings();
}

void MainWindow::setFirstTimeWindowLayout()
{
    const QRect workspaceSize = QApplication::desktop()->availableGeometry();
    bool maximizeWindow =
            workspaceSize.width() <= 1280 || workspaceSize.height() <= 700;
    if (maximizeWindow) {
        showMaximized();
    }
    else {
        resize(1280, 700);
        const int x = (workspaceSize.width() - 1280) / 2;
        const int y = (workspaceSize.height() - 700) / 2;
        move(x, y);
    }
    afterShowTimerId2_ = startTimer(200);
    // Then process setFirstTimeWindowLayout_stage2()
}

void MainWindow::setFirstTimeWindowLayout_stage2()
{
//    switchToRowFirstLayout();
    afterShowTimerId3_ = startTimer(300);
    // Then process setFirstTimeWindowLayout_stage3()
}

void MainWindow::setFirstTimeWindowLayout_stage3()
{
    setConsoleVisible(true);
    m_plugin->showActorWindow("Robot");
    showHelp();

    const int AreaWidth =
            width()
            - layout()->contentsMargins().left()
            - layout()->contentsMargins().right()
            ;
    const int AreaHeight =
            height()
            - layout()->contentsMargins().top()
            - layout()->contentsMargins().bottom()
            - menuBar()->height()
            - statusBar()->height()
            ;
    static const int InitialBottomHeight = 120;
    const int InitialTopHeight = AreaHeight - InitialBottomHeight - ui->splitter->handleWidth();
    QList<int> terminalSplitterSizes;
    terminalSplitterSizes.append(InitialTopHeight);
    terminalSplitterSizes.append(InitialBottomHeight);
    centralSide_->setSizes(terminalSplitterSizes);
    const int editorWidth = (AreaWidth - ui->splitter->handleWidth()) / 2;
    const int rightWidth = AreaWidth - editorWidth;
    QList<int> horizontalSplitterSizes;
    horizontalSplitterSizes.append(editorWidth);
    horizontalSplitterSizes.append(rightWidth);
    ui->splitter->setSizes(horizontalSplitterSizes);
    static const int InitialRobotHeight = 220;
    const int helpHeight =
            secondarySide_->height() -
            secondarySide_->handleWidth() -
            InitialRobotHeight;
    QList<int> sidePanelSplitterSizes;
    sidePanelSplitterSizes.append(helpHeight);
    sidePanelSplitterSizes.append(InitialRobotHeight);
    secondarySide_->setSizes(sidePanelSplitterSizes);
    m_plugin->helpViewer_->setInitialView();
}

void MainWindow::saveSettings()
{
    if (!settings_)
        return;
    QRect r(pos(), size());  
    settings_->setValue(Plugin::MainWindowGeometryKey, r);
    const QList<int> sizes = ui->splitter->sizes();
    settings_->setValue(Plugin::MainWindowSplitterStateKey+"0", sizes[0]);
    settings_->setValue(Plugin::MainWindowSplitterStateKey+"1", sizes[1]);
    settings_->setValue("SavedBottomSize", prevBottomSize_);
    centralSide_->save();
    secondarySide_->save();
    menubarContextMenu_->saveSettings();
}

void MainWindow::restoreSession()
{
    if (!settings_)
        return;
    bool hasUnsavedChanges = false;
    for (int i=0; i<tabWidget_->count(); i++) {
        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->widget(i));
        if (twe->editor()) {
            hasUnsavedChanges = hasUnsavedChanges || twe->editor()->isModified();
        }
        if (hasUnsavedChanges)
            break;
    }

    if (hasUnsavedChanges) {        
        QMessageBox messageBox(
                    QMessageBox::Question,
                    tr("Restore previous session"),
                    tr("Are you sure to restore previous session? All unsaved changes will be lost."),
                    QMessageBox::NoButton,
                    this
                    );
        messageBox.addButton(tr("Yes"), QMessageBox::AcceptRole);
        QPushButton * btnNo = messageBox.addButton(tr("No"), QMessageBox::RejectRole);
        messageBox.setDefaultButton(btnNo);
        messageBox.exec();
        if (messageBox.clickedButton()==btnNo) {
            return;
        }
    }

    int sessionIndex = 0;  

    tabWidget_->setCurrentIndex(sessionIndex);
    setupContentForTab();

}

void MainWindow::closeEvent(QCloseEvent *e)
{
//    saveSettings();
//    m_plugin->saveSession();
    if (m_plugin->sessionsDisableFlag_ && tabsDisabledFlag_) {
        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->currentWidget());
        if (twe->editor()) {
            bool notSaved = twe->editor()->isModified();
            if (!notSaved || twe->isCourseManagerTab()) {
                if (ExtensionSystem::PluginManager::instance()->shutdown()) {
                    e->accept();
#ifdef Q_OS_WIN32
                    qApp->quit();
#endif
                }
                else {
                    e->ignore();
                }
                return;
            }
        }
    }
    QMessageBox::StandardButton r = QMessageBox::Discard;
    QStringList unsavedFiles;
    for (int i=0; i<tabWidget_->count(); i++) {
        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->widget(i));
        if (twe->editor()) {
            bool notSaved = twe->editor()->isModified();
            if (notSaved && !twe->isCourseManagerTab()) {
                QString title = tabWidget_->tabText(i);
                if (title.endsWith("*")) {
                    title = title.left(title.length()-1);
                }
                unsavedFiles << "    "+title;
            }
        }
    }
    if (!unsavedFiles.isEmpty()) {
        QString messageText = tr("The following files have changes:\n%1\nSave them?").arg(unsavedFiles.join("\n"));
        QMessageBox messageBox(
                    QMessageBox::Question,
                    tr("Close Kumir"),
                    messageText,
                    QMessageBox::NoButton,
                    this
                    );
        QPushButton * btnSave =
                messageBox.addButton(tr("Save"), QMessageBox::AcceptRole);
        QPushButton * btnDiscard =
                messageBox.addButton(tr("Don't save"), QMessageBox::DestructiveRole);
        QPushButton * btnCancel =
                messageBox.addButton(tr("Cancel closing"), QMessageBox::RejectRole);
        messageBox.setDefaultButton(btnCancel);
        messageBox.exec();
        if (messageBox.clickedButton()==btnSave) {
            r = QMessageBox::Save;
        }
        if (messageBox.clickedButton()==btnDiscard) {
            r = QMessageBox::Discard;
        }
        if (messageBox.clickedButton()==btnCancel) {
            r = QMessageBox::Cancel;
        }

    }
    if (r==QMessageBox::Cancel) {
        e->ignore();
        return;
    }

    if (r==QMessageBox::Save) {
        // Save files
        for (int i=0; i<tabWidget_->count(); i++) {
            tabWidget_->setCurrentIndex(i);
            if (!saveCurrentFile()) {
                e->ignore();
                return;
            }
        }
    }

    if (!m_plugin->sessionsDisableFlag_) {
        // Clear previous session

        QDir sessionDir(QDir::currentPath()+"/.session");
        QStringList es = sessionDir.entryList(
                    QStringList() << "*.document",
                    QDir::Files,
                    QDir::Name
                    );
        foreach (const QString e, es) {
            sessionDir.remove(e);
        }
        QDir::current().remove(".session");

        // Save current session
        saveSession();
    }


    if (ExtensionSystem::PluginManager::instance()->shutdown()) {
        e->accept();
#ifdef Q_OS_WIN32
        qApp->quit();
#endif
    }
    else {
        e->ignore();
    }
}

void MainWindow::switchWorkspace() {
    m_plugin->showWorkspaceChooseDialog();
}

void MainWindow::saveSession() const
{

}

void MainWindow::showAbout()
{
    AboutDialog about(this);
    about.exec();
}

void MainWindow::showUserManual()
{
// Implemented in Plugin class
}

void MainWindow::showHelp()
{
    using namespace Shared::Analizer;

    TabWidgetElement * twe =
            qobject_cast<TabWidgetElement*>(tabWidget_->currentWidget());

    bool foundApiDocumentation = false;

    if (twe->editor() && twe->editor()->supportsContextHelp()) {
        ApiHelpItem item = twe->editor()->contextHelpItem();
        if (ApiHelpItem::Function == item.kind) {
            m_plugin->helpViewer_->navigateToApiFunction(item.packageName, item.itemName);
            foundApiDocumentation = true;
        }
    }
    if (!foundApiDocumentation) {
        m_plugin->helpViewer_->clearNavigationFilters();
    }
    m_plugin->helpWindow_->activate();
    if (twe->editor() && twe->editor()->supportsContextHelp() && !m_plugin->helpWindow_->isSeparateWindow()) {
        // Return focus back to editor in case if context menu
        twe->editor()->widget()->activateWindow();
        twe->editor()->widget()->setFocus(Qt::MouseFocusReason);
    }
}

void MainWindow::fileOpen()
{
    using namespace ExtensionSystem;
    using namespace Shared;
    if (tabsDisabledFlag_) {
        TabWidgetElement * twe =
                qobject_cast<TabWidgetElement*>(tabWidget_->currentWidget());
        if (twe->editor() && twe->editor()->isModified()) {
            QMessageBox::StandardButton r = QMessageBox::Cancel;
            QMessageBox messageBox(
                        QMessageBox::Question,
                        tr("Open another file"),
                        tr("Save current text?"),
                        QMessageBox::NoButton,
                        this
                        );
            QPushButton * btnSave =
                    messageBox.addButton(tr("Save"), QMessageBox::AcceptRole);
            QPushButton * btnDiscard =
                    messageBox.addButton(tr("Don't save"), QMessageBox::DestructiveRole);
            QPushButton * btnCancel =
                    messageBox.addButton(tr("Cancel opening another file"), QMessageBox::RejectRole);
            messageBox.setDefaultButton(btnSave);
            messageBox.exec();
            if (messageBox.clickedButton()==btnSave) {
                r = QMessageBox::Save;
            }
            if (messageBox.clickedButton()==btnDiscard) {
                r = QMessageBox::Discard;
            }
            if (messageBox.clickedButton()==btnCancel) {
                r = QMessageBox::Cancel;
            }
            if (r==QMessageBox::Cancel)
                return;
            if (r==QMessageBox::Save) {
                if (!saveCurrentFile())
                    return;
            }
        }
    }
    QStringList filters;    
    AnalizerInterface * analizer =
            PluginManager::instance()->findPlugin<AnalizerInterface>();
    const QString languageName = analizer->languageName();
    const QString fileNameSuffix = analizer->defaultDocumentFileNameSuffix();
    filters << tr("%1 programs (*%2)").arg(languageName).arg(fileNameSuffix);
    if (!tabsDisabledFlag_) {
        filters << tr("Web pages (*.html *.htm)");
        filters << tr("Text files (*.txt)");
    }
    filters << tr("All files (*)");
    const QString filter = filters.join(";;");
    const QString recent = m_plugin->mySettings()->value(Plugin::RecentFileKey, QDir::currentPath()).toString();
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Load file..."),
                                                    recent,
                                                    filter
                                                    );
    if (!fileName.isEmpty()) {
        m_plugin->mySettings()->setValue(Plugin::RecentFileKey, fileName);
        addToRecent(fileName);
        loadFromUrl(QUrl::fromLocalFile(fileName), true);
    }
}



void MainWindow::ensureSeconrarySideVisible()
{
    QList<int> szs = ui->splitter->sizes();
    if (szs.size() > 0 && 0 == szs[1]) {
        if (isColumnFirstLayout()) {
            szs[0] -= secondarySide_->minimumSizeHint().width();
            szs[1] = secondarySide_->minimumSizeHint().width();
        }
        else {
            szs[0] -= secondarySide_->minimumSizeHint().height();
            szs[1] = secondarySide_->minimumSizeHint().height();
        }
        ui->splitter->setSizes(szs);
    }
//    ui->actionShow_Console_Pane->setChecked(true);
//    setConsoleVisible(true);
}

void MainWindow::checkForConsoleHiddenBySplitter(int, int)
{
    QSplitter * splitter = qobject_cast<QSplitter*>(sender());
    int sz = splitter->sizes()[1];
    ui->actionShow_Console_Pane->setChecked(sz > 0);
}

void MainWindow::setConsoleVisible(bool v)
{
    bool columnLayout = ui->splitter->orientation() == Qt::Horizontal;
    if (! columnLayout) {
        // Rows first layout
        if (v) {
            int top = ui->splitter->sizes()[0];
            int bottom = ui->splitter->sizes()[1];
            int minTopH = ui->splitter->widget(0)->minimumSizeHint().height();
            if (prevBottomSize_ == 0)
                prevBottomSize_ = secondarySide_->minimumSizeHint().height();
            int diff = prevBottomSize_ - bottom;
            if (diff > 0) {
                int spaceAtTop = top - minTopH;
                int spaceToEat = qMin(diff, spaceAtTop);
                top -= spaceToEat;
                bottom += diff;
                diff -= spaceToEat;
                ui->splitter->setSizes(QList<int>() << top << bottom);
            }
        }
        else {
            int top = ui->splitter->sizes()[0];
            int bottom = ui->splitter->sizes()[1];
            prevBottomSize_ = bottom;
            int summ = top + bottom;
            ui->splitter->setSizes(QList<int>() << summ << 0);
        }
    }
    else {
        // Columns first layout
        if (v) {
            int top = centralSide_->sizes()[0];
            int bottom = centralSide_->sizes()[1];
            int minTopH = centralSide_->widget(0)->minimumSizeHint().height();
            if (0 == prevBottomSize_)
                prevBottomSize_ = minTopH;
            int diff = prevBottomSize_ - bottom;
            if (diff > 0) {
                int spaceAtTop = top - minTopH;
                int spaceToEat = qMin(diff, spaceAtTop);
                top -= spaceToEat;
                bottom += diff;
                diff -= spaceToEat;
                centralSide_->setSizes(QList<int>() << top << bottom);
            }
        }
        else {
            int top = centralSide_->sizes()[0];
            int bottom = centralSide_->sizes()[1];
            prevBottomSize_ = bottom;
            int summ = top + bottom;
            centralSide_->setSizes(QList<int>() << summ << 0);
        }

    }
}

QStringList MainWindow::recentFiles(bool fullPaths) const
{
    QStringList r = m_plugin->mySettings()->value(Plugin::RecentFilesKey).toStringList();
    if (fullPaths) {
        QStringList result;
        foreach (const QString & s, r) {
            QFile f(s);
            if (f.exists()) {
                if (QFileInfo(s).isRelative())
                    result << QDir::current().absoluteFilePath(s);
                else
                    result << QFileInfo(s).absoluteFilePath();
            }
        }
        return result;
    }
    else {
        QStringList result;
        foreach (const QString & s, r) {
            QFile f(s);
            if (f.exists()) {
                result << QFileInfo(s).fileName();
            }
        }

        return result;
    }
}

void MainWindow::addToRecent(const QString &fileName)
{
    QStringList r = m_plugin->mySettings()->value(Plugin::RecentFilesKey).toStringList();
    QString entry;
    entry = QFileInfo(fileName).absoluteFilePath();
    r.removeAll(entry);
    r.prepend(entry);
    r = r.mid(0, qMin(10, r.size()));
    m_plugin->mySettings()->setValue(Plugin::RecentFilesKey, r);
}

void MainWindow::loadRecentFile(const QString & fullPath)
{
    if (tabsDisabledFlag_) {
        TabWidgetElement * twe =
                qobject_cast<TabWidgetElement*>(tabWidget_->currentWidget());
        if (twe->editor() && twe->editor()->isModified()) {
            QMessageBox::StandardButton r = QMessageBox::Cancel;
            QMessageBox messageBox(
                        QMessageBox::Question,
                        tr("Open another file"),
                        tr("Save current text?"),
                        QMessageBox::NoButton,
                        this
                        );
            QPushButton * btnSave =
                    messageBox.addButton(tr("Save"), QMessageBox::AcceptRole);
            QPushButton * btnDiscard =
                    messageBox.addButton(tr("Don't save"), QMessageBox::DestructiveRole);
            QPushButton * btnCancel =
                    messageBox.addButton(tr("Cancel opening another file"), QMessageBox::RejectRole);
            messageBox.setDefaultButton(btnSave);
            messageBox.exec();
            if (messageBox.clickedButton()==btnSave) {
                r = QMessageBox::Save;
            }
            if (messageBox.clickedButton()==btnDiscard) {
                r = QMessageBox::Discard;
            }
            if (messageBox.clickedButton()==btnCancel) {
                r = QMessageBox::Cancel;
            }
            if (r==QMessageBox::Cancel)
                return;
            if (r==QMessageBox::Save) {
                if (!saveCurrentFile())
                    return;
            }
        }
    }
    QString fn = fullPath.trimmed();
//    QStringList r = recentFiles(true);
//    if (index>=0 && index<r.size())
//        loadFromUrl("file://"+r[index]);
    if (!fn.isEmpty() && QFile(fn).exists())
        loadFromUrl(fn);
}

TabWidgetElement * MainWindow::loadFromUrl(const QUrl & url, bool addToRecentFiles)
{
    DocumentType type;
    TabWidgetElement * result = 0;
    if (url.scheme().startsWith("file")) {
        const QString fileName = url.toLocalFile();
        const QString suffix = QFileInfo(fileName).suffix();
        AnalizerInterface * analizer =
                PluginManager::instance()->findPlugin<AnalizerInterface>();
        const QString programSuffix = analizer->defaultDocumentFileNameSuffix().remove(".");
        using namespace ExtensionSystem;
        using namespace Shared;
        if (suffix==programSuffix)
            type = Program;
        else if (suffix=="html" || suffix=="htm")
            type = StartPage;
        else
            type = Text;
    }
    else {
        type = StartPage;
    }
    if (addToRecentFiles && type!=StartPage)
        addToRecent(url.toLocalFile());
    if (type==Program || type==Text) {
        QFileInfo f(url.toLocalFile());
        Shared::Editor::InstanceInterface * editor = nullptr;
        QString error;

        editor = m_plugin->plugin_editor->loadDocument(url.toLocalFile(), &error);

        if (error.length() > 0) {
            QMessageBox::critical(this, tr("Can't open file"), error);
            return nullptr;
        }
        if (editor) {
            QWidget * vc = editor->widget();
            connect(vc, SIGNAL(message(QString)), this, SLOT(showMessage(QString)));
            connect(vc, SIGNAL(requestHelpForAlgorithm(QString)),
                    this, SLOT(showAlgorithmHelp(QString)));
            QString fileName = QFileInfo(url.toLocalFile()).fileName();
            if (tabsDisabledFlag_) {
                while(tabWidget_->count()) tabWidget_->removeTab(0);
            }
            result = addCentralComponent(
                        fileName,
                        vc,
                        editor->toolBarActions(),
                        editor->menus(),
                        type);
            result->setEditor(editor);
            QObject * editorObject = dynamic_cast<QObject*>(editor);
            connect(editorObject, SIGNAL(breakpointCnagedOrInserted(bool,quint32,quint32,QString)),
                    this, SLOT(handleBreakpointCnagedOrInserted(bool,quint32,quint32,QString)));
            connect(editorObject, SIGNAL(breakpointRemoved(quint32)),
                    this, SLOT(handleBreakpointRemoved(quint32)));
            tabWidget_->setCurrentIndex(tabWidget_->count()-1);
            tabWidget_->currentWidget()->setFocus();
            setupContentForTab();
            editor->forceCompleteCompilation();
        }
    }        
    else if (type==StartPage && m_plugin->plugin_browser) {
        Shared::Browser::InstanceInterface * browser =
                m_plugin->plugin_browser->createBrowser(url, m_plugin->m_browserObjects);
        browser->setTitleChangeHandler(this, SLOT(updateStartPageTitle(QString, const Shared::Browser::InstanceInterface*)));
        if (tabsDisabledFlag_) {
            while(tabWidget_->count()) tabWidget_->removeTab(0);
        }
        result = addCentralComponent(
                    url.toString(),
                    browser->widget(),
                    QList<QAction*>(),
                    QList<QMenu*>(),
                    StartPage);
        result->setStartPage(browser);
        tabWidget_->setCurrentIndex(tabWidget_->count()-1);
        tabWidget_->currentWidget()->setFocus();
    }
    setTitleForTab(tabWidget_->currentIndex());
    ExtensionSystem::PluginManager::instance()->switchGlobalState(PluginInterface::GS_Unlocked);
    return result;
}

TabWidgetElement* MainWindow::loadFromCourseManager(
        const GuiInterface::ProgramSourceText &data
        )
{
    typedef GuiInterface::ProgramSourceText ST;
    TabWidgetElement * courseManagerTab = nullptr;
    for (int i=0; i<tabWidget_->count(); i++) {
        TabWidgetElement * courseTab =
                qobject_cast<TabWidgetElement*>(tabWidget_->widget(i));
        if (courseTab && courseTab->isCourseManagerTab()) {
            courseManagerTab = courseTab;
            break;
        }
    }
    if (tabsDisabledFlag_) {
        courseManagerTab = qobject_cast<TabWidgetElement*>(tabWidget_->widget(0));
        Q_CHECK_PTR(courseManagerTab);
        courseManagerTab->setCourseManagerTab(true);
        courseManagerTab->setCourseTitle(data.title);
    }

        Shared::Analizer::SourceFileInterface::Data src = data.content;
        src.canonicalSourceLanguageName =
                ExtensionSystem::PluginManager::instance()
                ->findPlugin<Shared::AnalizerInterface>()
                ->defaultDocumentFileNameSuffix();
        src.sourceUrl = data.url;

        if (courseManagerTab) {
            // Reuse existing tab
            Shared::Editor::InstanceInterface * editor =
                    courseManagerTab->editor();
            QString error;
            editor->loadDocument(src, &error);
            editor->forceCompleteCompilation();
        }
        else {
            QString error;
            Shared::Editor::InstanceInterface * editor =
                    m_plugin->plugin_editor->loadDocument(src, &error);
            // Create new course manager tab
            courseManagerTab = addCentralComponent(
                        data.title,
                        editor->widget(),
                        editor->toolBarActions(),
                        editor->menus(),
                        Program
                        );
            courseManagerTab->setEditor(editor);
            courseManagerTab->setCourseManagerTab(true);
            editor->forceCompleteCompilation();
        }
        courseManagerTab->setCourseTitle(data.title);
        const int tabIndex = tabWidget_->indexOf(courseManagerTab);
        setTitleForTab(tabIndex);

    tabWidget_->setCurrentWidget(courseManagerTab);
    setupContentForTab();
    return courseManagerTab;
}

Shared::GuiInterface::ProgramSourceText
MainWindow::courseManagerProgramSource() const
{
    typedef GuiInterface::ProgramSourceText ST;
    ST result;
    result.language = ST::Kumir; // TODO implement for other languages
    TabWidgetElement * courseManagerTab = nullptr;
    for (int i=0; i<tabWidget_->count(); i++) {
        TabWidgetElement * courseTab =
                qobject_cast<TabWidgetElement*>(tabWidget_->widget(i));
        if (courseTab && courseTab->isCourseManagerTab()) {
            courseManagerTab = courseTab;
            break;
        }
    }
    if (courseManagerTab) {
        result.content = courseManagerTab->editor()->documentContents();
        result.url = result.content.sourceUrl;
        result.title = courseManagerTab->title();
    }
    return result;
}

} // namespace CoreGUI

