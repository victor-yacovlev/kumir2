#include "mainwindow.h"
#include "row.h"
#include "ui_mainwindow.h"
#include "tabwidgetelement.h"
#include "extensionsystem/pluginmanager.h"
#include "aboutdialog.h"
#include "kumirprogram.h"
#include "dataformats/kumfile.h"
#include "statusbar.h"
#include "tabwidget.h"
#include "systemopenfilesettings.h"

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
    prevBottomSize_(DefaultConsoleHeight)
{
    ui->setupUi(this);
    connect(ui->splitter, SIGNAL(splitterMoved(int,int)), this, SLOT(handleSplitterMoved(int,int)));
    QWidget * centralContainer = new QWidget;
    centralRow_ = new Row(this, "MainWindow/CentralRow");
    bottomRow_ = new Row(this, "MainWindow/BottomRow");
    connect(bottomRow_, SIGNAL(visiblityRequest()), this, SLOT(ensureBottomVisible()));
    centralContainer->setLayout(new QVBoxLayout);
    centralContainer->layout()->addWidget(centralRow_);
    centralContainer->layout()->setContentsMargins(0, 0, 0, 0);
#ifdef Q_OS_MAC
    centralContainer->layout()->setContentsMargins(0, 8, 0, 0);
#endif
    ui->splitter->addWidget(centralContainer);
    ui->splitter->addWidget(bottomRow_);

    tabWidget_ = new TabWidget(this);
    centralRow_->addComponent(tabWidget_, true);

    helpPlace_ = new Widgets::DockWindowPlace(this, "MainWindow/HelpDockPlace");
    centralRow_->addComponent(helpPlace_, false);
    connect(helpPlace_, SIGNAL(visiblityRequest(bool,QSize)),
            centralRow_, SLOT(handleVisiblityRequest(bool,QSize)),
            Qt::DirectConnection);

    debuggerPlace_ = new Widgets::DockWindowPlace(this, "MainWindow/DebuggerDockPlace");
    bottomRow_->addComponent(debuggerPlace_, false);
    connect(debuggerPlace_, SIGNAL(visiblityRequest(bool,QSize)),
            bottomRow_, SLOT(handleVisiblityRequest(bool,QSize)),
            Qt::DirectConnection);

    consoleAndCourcesPlace_ = new Widgets::DockWindowPlace(this, "MainWindow/ConsoleDockPlace");
    bottomRow_->addComponent(consoleAndCourcesPlace_, true);
    connect(consoleAndCourcesPlace_, SIGNAL(visiblityRequest(bool,QSize)),
            bottomRow_, SLOT(handleVisiblityRequest(bool,QSize)),
            Qt::DirectConnection);

    actorsPlace_ = new Widgets::DockWindowPlace(this, "MainWindow/ActorsDockPlace");
    bottomRow_->addComponent(actorsPlace_, false);
    connect(actorsPlace_, SIGNAL(visiblityRequest(bool,QSize)),
            bottomRow_, SLOT(handleVisiblityRequest(bool,QSize)),
            Qt::DirectConnection);

    connect(ui->actionShow_Console_Pane, SIGNAL(triggered(bool)), this, SLOT(setBottomVisible(bool)));

    setStatusBar(statusBar_);
    setMinimumHeight(380);
    tabsDisabledFlag_ = false;
    b_workspaceSwitching = false;


    const QString qtcreatorIconsPath = ExtensionSystem::PluginManager::instance()->sharePath()
            + "/icons/from_qtcreator/";

    ui->actionNewProgram->setIcon(QIcon(qtcreatorIconsPath+"filenew.png"));
    ui->actionOpen->setIcon(QIcon(qtcreatorIconsPath+"fileopen.png"));
    ui->actionSave->setIcon(QIcon(qtcreatorIconsPath+"filesave.png"));
    ui->actionSave->setProperty("role", "save");


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

    QObject * runnerObject =
            ExtensionSystem::PluginManager::instance()->findKPlugin<Shared::RunInterface>();
    if (runnerObject) {
//        connect(runnerObject, SIGNAL(updateStepsCounter(quint64)), this, SLOT(checkCounterValue()));
    }

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

//    statusBar()->setStyleSheet(
//                "QStatusBar { "
//                "   border-top: 1px solid darkgray;"
//                "}"
//                "QToolButton {"
//                "   icon-size: 16px;"
//                "   border: none;"
//                "}"
//                );

////    setMinimumSize(980, 480);
//    statusBar()->addWidget(m_plugin->m_kumirStateLabel);
//    statusBar()->addWidget(m_plugin->m_genericCounterLabel);
//    m_plugin->m_kumirStateLabel->setFixedWidth(140);
//    m_plugin->m_kumirStateLabel->setStyleSheet(StatusbarWidgetCSS);
//    m_plugin->m_kumirStateLabel->setAlignment(Qt::AlignCenter);
//    m_plugin->m_genericCounterLabel->setFixedWidth(260);
//    m_plugin->m_genericCounterLabel->setStyleSheet(StatusbarWidgetCSS);
//    m_plugin->m_genericCounterLabel->setAlignment(Qt::AlignCenter);
//    m_message = new QLabel(this);
//    m_message->setAlignment(Qt::AlignCenter);
//    m_message->setStyleSheet(StatusbarWidgetCSS);
//    statusBar()->addWidget(m_message, 1);

    i_timerId = startTimer(250);

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
    connect(ui->actionUsage, SIGNAL(triggered()), this, SLOT(showUserManual()));

    installEventFilter(this);
#ifndef Q_OS_MAC
    installEventFilter(menuBar());
#endif

    ui->actionRecent_files->setMenu(new QMenu());
    connect(ui->menuFile, SIGNAL(aboutToShow()), this, SLOT(prepareRecentFilesMenu()));


#ifdef Q_OS_MAC

#endif
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
    AnalizerInterface * analizer = manager->findPlugin<AnalizerInterface>();

    if (!analizer || analizer->languageName()!=QString::fromUtf8("Кумир")) {
        ui->menuInsert->deleteLater();
        ui->menuInsert = nullptr;
    }

    createSettingsDialog();
    ui->menuFile->removeAction(ui->actionSave_all); // Not implemented yet
    ui->menuFile->removeAction(ui->actionRestore_previous_session); // Not implemented yet
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
        if (twe->editorInstance) {
            Editor::InstanceInterface * editor = twe->editorInstance;
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
    checkCounterValue();
    e->accept();
}



bool MainWindow::saveCurrentFile()
{
    TabWidgetElement * twe = currentTab();
    if (!twe->editorInstance)
        return true;
    const QString fileName = twe->editorInstance->documentContents().sourceUrl.toLocalFile();
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

void MainWindow::setTitleForTab(int index)
{
    if (index<0 || index>=tabWidget_->count())
        return;
    QWidget * currentTabWidget = tabWidget_->widget(index);

    if (!currentTabWidget)
        return;

    TabWidgetElement * twe = currentTab();
    QString title = twe->title();

    QString appName = tr("Kumir");
    using namespace Shared;
    using namespace ExtensionSystem;
    PluginManager * manager = PluginManager::instance();
    AnalizerInterface * analizer = manager->findPlugin<AnalizerInterface>();
    if (analizer) {
        appName = analizer->languageName();
    }
    setWindowTitle(title + " - "+ appName);
    tabWidget_->setTabText(index, title);
}

void MainWindow::updateBrowserTitle(const QString &title, const Shared::Browser::InstanceInterface * sender)
{
    for (int i=0; i<tabWidget_->count(); i++) {
        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->widget(i));
        if (twe->browserInstance == sender) {
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
    ui->actionSave->setEnabled(twe->type!=WWW);
    ui->actionSave_as->setEnabled(twe->type!=WWW);

    ui->actionClose->setEnabled(!twe->property("uncloseable").toBool());

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
        ui->menuRun->addActions(m_plugin->kumirProgram_->actions()->actions());
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
            if (twe->menus[i]->title().trimmed()==ui->menuEdit->title().trimmed()) {
                tabMenu = twe->menus[i];
                break;
            }
        }
//    }

    if (tabMenu) {
        ui->menuEdit->menuAction()->setMenu(tabMenu);
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
            if (twe->menus[i]->title().trimmed()==ui->menuInsert->title().trimmed()) {
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

bool MainWindow::saveCurrentFileAs()
{
    using namespace ExtensionSystem;
    using namespace Shared;
    AnalizerInterface * analizer =
            PluginManager::instance()->findPlugin<AnalizerInterface>();
    const QString languageName = analizer->languageName();
    const QString fileNameSuffix = analizer->defaultDocumentFileNameSuffix();
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->currentWidget());
    QString fileName = twe->editorInstance->documentContents().sourceUrl.toLocalFile();
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
        initialPath += "/" + suggestNewFileName(suffix, twe->editorInstance->analizer(), initialPath);
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
    try {
        twe->editorInstance->saveDocument(fileName);
    }
    catch (const QString & error) {
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
    if (twe->type!=WWW) {
        bool notSaved = twe->editorInstance->isModified();
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
    settingsDialog_ = new Widgets::MultiPageDialog(this);
    settingsDialog_->setWindowTitle(tr("Preferences"));
    settingsDialog_->setMinimumSize(700, 500);
    PluginManager * manager = PluginManager::instance();
    QList<KPlugin*> plugins = manager->loadedPlugins();
    foreach (KPlugin * p, plugins) {
        QWidget * page = p->settingsEditorPage();
        if (page) {
            settingsDialog_->addPage(page);
        }
    }
#ifndef Q_OS_MACX
    SystemOpenFileSettings * openFileSettings = new SystemOpenFileSettings;
    settingsDialog_->addPage(openFileSettings);
#endif
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
    connect(vc, SIGNAL(requestHelpForAlgorithm(QString)),
            this, SLOT(showAlgorithmHelp(QString)));

    QString fileName = suggestNewFileName(suffix, editor->analizer());
    TabWidgetElement * e = addCentralComponent(
                fileName,
                vc,
                editor->toolBarActions(),
                editor->menus(),
                type);
    e->editorInstance = editor;
    tabWidget_->setCurrentWidget(e);
    setTitleForTab(tabWidget_->indexOf(e));
    e->setFocus();

}

void MainWindow::showAlgorithmHelp(const QString &name)
{
    m_plugin->helpWindow_->activate();
    m_plugin->helpViewer_->selectAlgorithm(name);
}

void MainWindow::newText()
{
    QString fileName = suggestNewFileName(".txt", nullptr);
    newText(fileName, "");
}

void MainWindow::newText(const QString &fileName, const QString & text)
{    
    Shared::Editor::InstanceInterface * editor =
            m_plugin->plugin_editor->newDocument("", QDir::currentPath());

    KumFile::Data data;
    data.canonicalSourceLanguageName = "";
    data.sourceUrl = fileName.isEmpty()
            ? QUrl() : QUrl::fromLocalFile(fileName);
    data.visibleText = text;
    editor->loadDocument(data);

    QWidget * vc = editor->widget();
    connect(vc, SIGNAL(message(QString)), this, SLOT(showMessage(QString)));

    TabWidgetElement * e = addCentralComponent(
                QFileInfo(fileName).fileName(),
                vc,
                editor->toolBarActions(),
                editor->menus(),
                Text);
    e->editorInstance = editor;
    tabWidget_->setCurrentWidget(e);
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
    TabWidgetElement * element = new TabWidgetElement(c, type != WWW,
                                                      toolbarActions, menus, type,
                                                      gr_fileActions, gr_otherActions, kumir);

    connect(element, SIGNAL(documentCleanChanged(bool)), this, SLOT(handleDocumentCleanChanged(bool)));
    connect(element, SIGNAL(titleChanged(QString)), this, SLOT(handleTabTitleChanged()));
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
        bool found = false;
        for (int j=0; j<menus.size(); j++) {
            const QString menuTitle = menus[j]->title().trimmed();
            if (menuTitle==title) {
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
            mb->insertMenu(ui->menuWindow->menuAction(), menu);
        }
    }
}

void MainWindow::setupContentForTab()
{
    QWidget * currentTabWidget = tabWidget_->currentWidget();

    if (!currentTabWidget)
        return;

    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(currentTabWidget);
    m_plugin->kumirProgram_->setEditorInstance(twe->editorInstance);
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
//    if (settings_) saveSettings();
    settings_ = settings;    
    loadSettings(keys);
}

void MainWindow::loadSettings(const QStringList & keys)
{
    QRect r = settings_->value(Plugin::MainWindowGeometryKey,
                               QRect(QPoint(-1, -1), QSize(940, 540))).toRect();
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
    centralRow_->updateSettings(settings_, keys);
    bottomRow_->updateSettings(settings_, keys);
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
    centralRow_->save();
    bottomRow_->save();
}

void MainWindow::restoreSession()
{
    if (!settings_)
        return;
    bool hasUnsavedChanges = false;
    for (int i=0; i<tabWidget_->count(); i++) {
        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->widget(i));
        if (twe->editorInstance) {
            hasUnsavedChanges = hasUnsavedChanges || twe->editorInstance->isModified();
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
        if (twe->editorInstance) {
            bool notSaved = twe->editorInstance->isModified();
            if (!notSaved || twe->isCourseManagerTab()) {
                e->accept();
                ExtensionSystem::PluginManager::instance()->shutdown();
                return;
            }
        }
    }
    QMessageBox::StandardButton r = QMessageBox::Discard;
    QStringList unsavedFiles;
    for (int i=0; i<tabWidget_->count(); i++) {
        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(tabWidget_->widget(i));
        if (twe->editorInstance) {
            bool notSaved = twe->editorInstance->isModified();
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


    e->accept();
    ExtensionSystem::PluginManager::instance()->shutdown();
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
    if (!ui->actionUsage->isChecked())
        ui->actionUsage->trigger();
}

void MainWindow::fileOpen()
{
    using namespace ExtensionSystem;
    using namespace Shared;
    if (tabsDisabledFlag_) {
        TabWidgetElement * twe =
                qobject_cast<TabWidgetElement*>(tabWidget_->currentWidget());
        if (twe->editorInstance && twe->editorInstance->isModified()) {
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

void MainWindow::ensureBottomVisible()
{
    ui->actionShow_Console_Pane->setChecked(true);
    setBottomVisible(true);
}

void MainWindow::handleSplitterMoved(int, int)
{
    ui->actionShow_Console_Pane->setChecked(ui->splitter->sizes()[1] > 0);
}

void MainWindow::setBottomVisible(bool v)
{
    if (v) {
        int top = ui->splitter->sizes()[0];
        int bottom = ui->splitter->sizes()[1];
        int minTopH = ui->splitter->widget(0)->minimumSizeHint().height();
        if (prevBottomSize_ == 0)
            prevBottomSize_ = bottomRow_->minimumSizeHint().height();
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
        if (twe->editorInstance && twe->editorInstance->isModified()) {
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
            type = WWW;
        else
            type = Text;
    }
    else {
        type = WWW;
    }
    if (addToRecentFiles && type!=WWW)
        addToRecent(url.toLocalFile());
    if (type==Program || type==Text) {
        QFileInfo f(url.toLocalFile());
        Shared::Editor::InstanceInterface * editor = nullptr;
        try {
            editor = m_plugin->plugin_editor->loadDocument(url.toLocalFile());
        }
        catch (const QString & e) {
            QMessageBox::critical(this, tr("Can't open file"), e);
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
            result->editorInstance = editor;
            tabWidget_->setCurrentIndex(tabWidget_->count()-1);
            tabWidget_->currentWidget()->setFocus();
            setupContentForTab();
        }
    }        
    else if (type==WWW && m_plugin->plugin_browser) {
        Shared::Browser::InstanceInterface * browser =
                m_plugin->plugin_browser->createBrowser(url, m_plugin->m_browserObjects);
        browser->setTitleChangeHandler(this, SLOT(updateBrowserTitle(QString, const Shared::Browser::InstanceInterface*)));
        if (tabsDisabledFlag_) {
            while(tabWidget_->count()) tabWidget_->removeTab(0);
        }
        result = addCentralComponent(
                    url.toString(),
                    browser->widget(),
                    QList<QAction*>(),
                    QList<QMenu*>(),
                    WWW);
        result->browserInstance = browser;
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
    if (data.language == ST::Kumir) {
        KumFile::Data src = data.content;
        src.canonicalSourceLanguageName = "kum";

        if (courseManagerTab) {
            // Reuse existing tab
            Shared::Editor::InstanceInterface * editor =
                    courseManagerTab->editorInstance;
            editor->loadDocument(src);
        }
        else {
            Shared::Editor::InstanceInterface * editor =
                    m_plugin->plugin_editor->loadDocument(src);
            // Create new course manager tab
            courseManagerTab = addCentralComponent(
                        data.title,
                        editor->widget(),
                        editor->toolBarActions(),
                        editor->menus(),
                        Program
                        );
            courseManagerTab->editorInstance = editor;
            courseManagerTab->setCourseManagerTab(true);
        }
        courseManagerTab->setCourseTitle(data.title);

    }
    tabWidget_->setCurrentWidget(courseManagerTab);
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
        result.content = courseManagerTab->editorInstance->documentContents();
        result.url = result.content.sourceUrl;
        result.title = courseManagerTab->title();
    }
    return result;
}

} // namespace CoreGUI

