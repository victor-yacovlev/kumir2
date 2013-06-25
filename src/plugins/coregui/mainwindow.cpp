#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tabwidgetelement.h"
#include "extensionsystem/pluginmanager.h"
#include "aboutdialog.h"
#include "kumirprogram.h"
#include "dataformats/kumfile.h"
#include "statusbar.h"


#include <algorithm>

namespace CoreGUI {


MainWindow::MainWindow(Plugin * p) :
    QMainWindow(0),
    ui(new Ui::MainWindow),
    m_plugin(p),
    statusBar_(new StatusBar)
{
    setMinimumHeight(280);
    setStatusBar(statusBar_);

    b_notabs = false;
    b_workspaceSwitching = false;
    ui->setupUi(this);

    const QString qtcreatorIconsPath = QApplication::instance()->property("sharePath")
            .toString() + "/icons/from_qtcreator/";

//    ui->actionNewProgram->setIcon(actionIcon("document-new"));
    ui->actionOpen->setIcon(QIcon(qtcreatorIconsPath+"fileopen.png"));
    ui->actionSave->setIcon(QIcon(qtcreatorIconsPath+"filesave.png"));


    ui->menuFile->setWindowTitle(ui->menuFile->title());
    ui->menuHelp->setWindowTitle(ui->menuHelp->title());

    connect(ui->actionNewProgram, SIGNAL(triggered()), this, SLOT(newProgram()));
    connect(ui->actionNewText, SIGNAL(triggered()), this, SLOT(newText()));

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(closeCurrentTab()));

    connect(ui->actionSwitch_workspace, SIGNAL(triggered()), this, SLOT(switchWorkspace()));

    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(setupActionsForTab()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(setupContentForTab()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(setupStatusbarForTab()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(checkCounterValue()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(setTitleForTab(int)));

    QObject * runnerObject =
            ExtensionSystem::PluginManager::instance()->findKPlugin<Shared::RunInterface>();
    if (runnerObject) {
        connect(runnerObject, SIGNAL(updateStepsCounter(quint64)), this, SLOT(checkCounterValue()));
    }

    connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(showPreferences()));

    gr_fileActions = new QActionGroup(this);
    gr_fileActions->addAction(ui->actionSave);
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveCurrentFile()));
    connect(ui->actionSave_as, SIGNAL(triggered()), this, SLOT(saveCurrentFileAs()));

    gr_otherActions = new QActionGroup(this);
    gr_otherActions->setExclusive(false);

    a_notAvailable = new QAction(this);
    a_notAvailable->setText(tr("No actions for this tab"));
    a_notAvailable->setEnabled(false);

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

    i_timerId = startTimer(1000);

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

    ui->tabWidget->setAcceptDrops(true);
    ui->tabWidget->installEventFilter(this);
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
    if (e->type() == QEvent::DragEnter && o == ui->tabWidget) {
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
                    static const QStringList validSuffices = b_notabs
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
            const bool canAccept = b_notabs
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
    if (e->type() == QEvent::Drop && o == ui->tabWidget) {
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

    if (b_notabs) {
        ui->actionNewProgram->setEnabled(false);
        ui->actionOpen->setEnabled(false);
        ui->actionRecent_files->setEnabled(false);
    }
    else {
        for (int i=0; i<ui->tabWidget->count(); i++) {
            TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->widget(i));
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

    if (b_notabs) {
        ui->actionNewProgram->setEnabled(true);
        ui->actionOpen->setEnabled(true);
        ui->actionRecent_files->setEnabled(true);
    }
    else {
        for (int i=0; i<ui->tabWidget->count(); i++) {
            TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->widget(i));
            if (twe && twe->type==Program && twe->kumirProgram()) {
                twe->setProperty("uncloseable", false);
            }
        }
        setupActionsForTab();
    }
}

void MainWindow::activateDocumentTab(int documentId)
{
    for (int i=0; i<ui->tabWidget->count(); i++) {
        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->widget(i));
        if (twe && twe->type == Program && twe->documentId==documentId) {
            ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }
}

void MainWindow::checkCounterValue()
{
    using namespace ExtensionSystem;
    using namespace Shared;
    GlobalState state = PluginManager::instance()->currentGlobalState();
    if (state==GS_Unlocked) {
        if (ui->tabWidget->count()==0)
            return;
        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
        if (!twe)
            return;
        if (twe->type==Program) {
            int id = twe->documentId;
            quint32 errorsCount = m_plugin->plugin_editor->errorsLinesCount(id);
            statusBar_->setErrorsCounter(errorsCount);
//            if (errorsCount==0) {
//                m_plugin->m_genericCounterLabel->setText(tr("No errors"));
//                m_plugin->m_genericCounterLabel->setStyleSheet(StatusbarWidgetCSS);
//            }
//            else {
//                m_plugin->m_genericCounterLabel->setText(tr("Errors: %1").arg(errorsCount));
//                m_plugin->m_genericCounterLabel->setStyleSheet(StatusbarWidgetCSS+
//                                                               "QLabel {"
//                                                               "    color: red;"
//                                                               "}"
//                                                               );
//            }
        }
        else {
            statusBar_->setErrorsCounter(0);
//            m_plugin->m_genericCounterLabel->setText("");
        }
    }
    else {
        RunInterface * runner =
                PluginManager::instance()->findPlugin<RunInterface>();
        if (runner) {
            ulong stepsCounted = runner->stepsCounted();
            ulong stepsDone =
                    state==GS_Observe && runner->error().length()==0
                    ? stepsCounted  // all steps successfully finished
                    : stepsCounted - 1; // all but last unfinished step
            if (stepsCounted == 0) {
                stepsDone = 0;  // just begin of evaluation
            }
            statusBar_->setStepsDoneCounter(stepsDone);
//            const QString text = tr("Steps done: %1").arg(stepsDone);
//            m_plugin->m_genericCounterLabel->setText(text);
//            m_plugin->m_genericCounterLabel->setStyleSheet(StatusbarWidgetCSS);
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
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
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
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
    if (twe->type==WWW)
        return true;
    const QString fileName = twe->url.toLocalFile();
    bool result = 0;
    if (fileName.isEmpty()) {
        result = saveCurrentFileAs();
    }
    else {
        result = saveCurrentFileTo(fileName);
    }
    if (result) {
//        twe->setProperty("title", QFileInfo(twe->property("realFileName").toString()).fileName());
        setTitleForTab(ui->tabWidget->currentIndex());
    }
    return result;
}

void MainWindow::setTitleForTab(int index)
{
    if (index<0 || index>=ui->tabWidget->count())
        return;
    QWidget * currentTabWidget = ui->tabWidget->widget(index);

    if (!currentTabWidget)
        return;

    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(currentTabWidget);
    QString title = twe->property("title").toString();
    setWindowTitle(title + " - "+ tr("Kumir"));
    ui->tabWidget->setTabText(index, title);
}

void MainWindow::setupActionsForTab()
{
    QWidget * currentTabWidget = ui->tabWidget->currentWidget();

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
    for (int i=0; i<ui->tabWidget->count(); i++) {
        TabWidgetElement * w =
                qobject_cast<TabWidgetElement*>(ui->tabWidget->widget(i));
        if (w && w->component) {
            w->component->disconnect(statusBar_);
        }
    }
    QWidget * currentTabWidget = ui->tabWidget->currentWidget();
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
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
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
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
    QMenu * tabMenu = 0;

    if (twe->type!=WWW) {
        for (int i=0; i<twe->menus.size(); i++) {
            if (twe->menus[i]->title().trimmed()==ui->menuEdit->title().trimmed()) {
                tabMenu = twe->menus[i];
                break;
            }
        }
    }

    if (tabMenu) {
        ui->menuEdit->menuAction()->setMenu(tabMenu);
    }
    else {
        ui->menuEdit->clear();
        ui->menuEdit->addAction(a_notAvailable);
    }
}

void MainWindow::prepareInsertMenu()
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
    QMenu * tabMenu = 0;

    if (twe->type!=WWW) {
        for (int i=0; i<twe->menus.size(); i++) {
            if (twe->menus[i]->title().trimmed()==ui->menuInsert->title().trimmed()) {
                tabMenu = twe->menus[i];
                break;
            }
        }
    }

    if (tabMenu) {
        ui->menuInsert->menuAction()->setMenu(tabMenu);
    }
    else {
        ui->menuInsert->clear();
        ui->menuInsert->addAction(a_notAvailable);
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
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
    QString fileName = twe->property("realFileName").toString();
    QString initialPath;
    if (fileName.isEmpty()) {
        QString lastFileName = m_plugin->mySettings()->value(Plugin::RecentFileKey).toString();
        if (lastFileName.isEmpty()) {
            initialPath = QDir::currentPath();
        }
        else {
            initialPath = QFileInfo(lastFileName).absoluteDir().absolutePath();
        }
        const QString suffix = twe->type==Program ? fileNameSuffix : ".txt";
        initialPath += "/" + suggestNewFileName(suffix, initialPath);
    }
    else {
        initialPath = fileName;
    }
    QStringList filter;    
    if (twe->type==Program) {
        filter << tr("%1 programs (*%2)").arg(languageName).arg(fileNameSuffix);
    }    
    if (twe->type==Text) {
        filter << tr("Text files (*.txt)");
    }
    filter << tr("All files (*)");
    fileName = QFileDialog::getSaveFileName(this, tr("Save file"), initialPath, filter.join(";;"));
    if (!fileName.isEmpty()) {
        if (twe->type==Program && !fileName.endsWith(fileNameSuffix))
            fileName += fileNameSuffix;
        if (saveCurrentFileTo(fileName)) {
            m_plugin->mySettings()->setValue(Plugin::RecentFileKey, fileName);
            twe->setProperty("fileName", fileName);
            twe->setProperty("realFileName", fileName);
            twe->setProperty("title", QFileInfo(fileName).fileName());
            int index = ui->tabWidget->indexOf(twe);
            ui->tabWidget->setTabText(index, QFileInfo(fileName).fileName());
            addToRecent(fileName);
            m_plugin->mySettings()->setValue(Plugin::RecentFileKey, fileName);
            setTitleForTab(ui->tabWidget->currentIndex());
            return true;
        }
    }
    return false;
}

bool MainWindow::saveCurrentFileTo(const QString &fileName)
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
    int documentId = twe->documentId;
    QString error = m_plugin->plugin_editor->saveDocument(documentId, fileName);
    if (error.isEmpty()) {
        twe->saved = QDateTime::currentDateTime();
        twe->url = QUrl::fromLocalFile(fileName);
        m_plugin->plugin_editor->setDocumentChangesSaved(documentId);        
        return true;
    }
    else {
        QMessageBox::critical(this, tr("Can't save file"), error);
        return false;
    }
}

void MainWindow::handleDocumentCleanChanged(bool v)
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(sender());
    int index = ui->tabWidget->indexOf(twe);
    QString text = ui->tabWidget->tabText(index);
    if (text.endsWith("*"))
        text = text.left(text.length()-1);
    if (!v)
        text += "*";
    twe->setProperty("title", text);
    setTitleForTab(index);
}

void MainWindow::handleTabTitleChange(const QString &title)
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(sender());
    int index = ui->tabWidget->indexOf(twe);
    twe->setProperty("title", title);
    setTitleForTab(index);
}

void MainWindow::closeCurrentTab()
{
    closeTab(ui->tabWidget->currentIndex());
    if (ui->tabWidget->currentWidget())
        ui->tabWidget->currentWidget()->setFocus();
}

bool MainWindow::closeTab(int index)
{
    if (index==-1 || index>=ui->tabWidget->count())
        return true;
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->widget(index));
    if (twe->property("uncloseable").toBool()) {
        return false;
    }
    if (twe->type!=WWW) {
        int documentId = twe->documentId;
        bool notSaved = m_plugin->plugin_editor->hasUnsavedChanges(documentId);
        QMessageBox::StandardButton r;
        if (!notSaved) {
            r = QMessageBox::Discard;
        }
        else {
            ui->tabWidget->setCurrentIndex(index);
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
            m_plugin->plugin_editor->closeDocument(documentId);
            ui->tabWidget->removeTab(index);
            return true;
        }
        else if (r==QMessageBox::Save) {
            if (saveCurrentFile()) {
                m_plugin->plugin_editor->closeDocument(documentId);
                ui->tabWidget->removeTab(index);
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
        ui->tabWidget->removeTab(index);
        return true;
    }

}

void MainWindow::showPreferences()
{
    ExtensionSystem::PluginManager::instance()->showSettingsDialog();
}


QIcon MainWindow::actionIcon(const QString &name)
{
    const QString iconsPath = qApp->property("sharePath").toString()+"/icons/";
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
    if (b_notabs && !closeTab(ui->tabWidget->currentIndex())) {
        return;
    }
    AnalizerInterface * analizer =
            PluginManager::instance()->findPlugin<AnalizerInterface>();
    KPlugin * analizerPlugin =
            PluginManager::instance()->findKPlugin<AnalizerInterface>();
    const QString analizerPluginName = analizerPlugin->pluginSpec().name;
    const QString suffix = analizer->defaultDocumentFileNameSuffix();
    DocumentType type = Program;
    EditorComponent doc = m_plugin->plugin_editor->newDocument(
                analizerPluginName,
                QDir::currentPath(),
                false
                );
    QWidget* vc = doc.widget;
    int id = doc.id;
    vc->setProperty("documentId", id);
    QString fileName = suggestNewFileName(suffix);
    vc->setProperty("title",QFileInfo(fileName).fileName());
    vc->setProperty("fileName", QDir::current().absoluteFilePath(fileName));
    TabWidgetElement * e = addCentralComponent(
                fileName,
                vc,
                doc.toolbarActions,
                doc.menus,
//                doc.statusbarWidgets,
                type,
                true);
    e->documentId = doc.id;
    ui->tabWidget->setCurrentWidget(e);
    e->setFocus();

}


void MainWindow::newText()
{
    QString fileName = suggestNewFileName(".txt");
    newText(fileName, "");
}

void MainWindow::newText(const QString &fileName, const QString & text)
{
    Shared::EditorComponent doc = m_plugin->plugin_editor->newDocument("", QDir::currentPath(), true);
    QWidget * vc = doc.widget;
    int id = doc.id;
    vc->setProperty("documentId", id);
    vc->setProperty("fileName", fileName);
    vc->setProperty("title",QFileInfo(fileName).fileName());
    TabWidgetElement * e = addCentralComponent(
                QFileInfo(fileName).fileName(),
                vc,
                doc.toolbarActions,
                doc.menus,
//                doc.statusbarWidgets,
                Text,
                true);
    e->documentId = doc.id;
    ui->tabWidget->setCurrentWidget(e);
    if (!text.isEmpty()) {
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),
                                  ui->tabWidget->tabText(ui->tabWidget->currentIndex())
                                  +"*");
    }
    e->setFocus();
}

QString MainWindow::suggestNewFileName(const QString &suffix, const QString & dirName) const
{
    QDir d;
    if (dirName.isEmpty())
        d = QDir::current();
    else
        d = QDir(dirName);
    QStringList fileNames = d.entryList(QStringList() << "*"+suffix);
    for (int i=0; i<ui->tabWidget->count(); i++) {
        fileNames << ui->tabWidget->tabText(i);
    }
    QString result = "newfile";
    int index = 0;
    while (fileNames.contains(result+suffix)) {
        index ++;
        result = QString("newfile_%1").arg(index);
    }
    return result+suffix;
}


TabWidgetElement * MainWindow::addCentralComponent(
    const QString &title
    , QWidget *c
    , const QList<QAction*> & toolbarActions
    , const QList<QMenu*> & menus
//    , const QList<QWidget*> & statusbarWidgets
    , DocumentType type
    , bool enableToolBar)
{
    class KumirProgram * kumir = 0;
    class PascalProgram * pascal = 0;
    if (type==Program) {
        kumir = m_plugin->kumirProgram_;
    }
    TabWidgetElement * element = new TabWidgetElement(
                c,
                enableToolBar,
                toolbarActions,
                menus,
//                statusbarWidgets,
                type,
                gr_fileActions,
                gr_otherActions,
                kumir,
                pascal
                );


    connect(element, SIGNAL(changeTitle(QString)), this, SLOT(handleTabTitleChange(QString)));
    connect(element, SIGNAL(documentCleanChanged(bool)), this, SLOT(handleDocumentCleanChanged(bool)));
    createTopLevelMenus(menus, true);
    ui->tabWidget->addTab(element, title);


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
    QWidget * currentTabWidget = ui->tabWidget->currentWidget();

    if (!currentTabWidget)
        return;

    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(currentTabWidget);
    if (twe->type==Program) {
        const int id = twe->documentId;
        const QString fileName = twe->property("fileName").toString();
        const AnalizerInterface * analizer = m_plugin->plugin_editor->analizer(id);
        int analizerId = m_plugin->plugin_editor->analizerDocumentId(id);
        const AST::DataPtr ast = analizer->abstractSyntaxTree(analizerId);
        m_plugin->kumirProgram_->setAST(ast);
        m_plugin->kumirProgram_->setSourceFileName(fileName);
        m_plugin->kumirProgram_->setDocumentId(id);
    }
    else {
        m_plugin->kumirProgram_->setAST(AST::DataPtr());
    }
}

void MainWindow::disableTabs()
{
    ui->tabWidget->disableTabs();
    b_notabs = true;
    ui->actionClose->setVisible(false);
    ui->actionClose->setEnabled(false);
    ui->actionNewText->setVisible(false);
    ui->actionNewText->setEnabled(false);
    ui->actionSave_all->setEnabled(false);
    ui->actionSave_all->setVisible(false);
}

void MainWindow::loadSettings()
{
    QRect r = m_plugin->mySettings()->value(Plugin::MainWindowGeometryKey, QRect(-1,-1,0,0)).toRect();
    if (r.width()>0) {
        resize(r.size());
        move(r.topLeft());
    }
    restoreState(m_plugin->mySettings()->value(Plugin::MainWindowStateKey).toByteArray());
    ui->splitter->restoreState(m_plugin->mySettings()->value(Plugin::MainWindowSplitterStateKey).toByteArray());
    bool showConsole = m_plugin->mySettings()->value(Plugin::MainWindowShowConsoleKey, true).toBool();
    ui->actionShow_Console_Pane->setChecked(showConsole);
    m_plugin->showConsolePane(showConsole);
}

void MainWindow::saveSettings()
{
    QRect r(pos(), size());
    SettingsPtr sett = m_plugin->mySettings();
    sett->setValue(Plugin::MainWindowGeometryKey, r);
    sett->setValue(Plugin::MainWindowStateKey, saveState());
    sett->setValue(Plugin::MainWindowSplitterStateKey, ui->splitter->saveState());
    sett->setValue(Plugin::MainWindowShowConsoleKey, ui->actionShow_Console_Pane->isChecked());
}

void MainWindow::restoreSession()
{

    bool hasUnsavedChanges = false;
    for (int i=0; i<ui->tabWidget->count(); i++) {
        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->widget(i));
        if (twe->type!=WWW) {
            int documentId = twe->property("documentId").toInt();
            hasUnsavedChanges = hasUnsavedChanges || m_plugin->plugin_editor->hasUnsavedChanges(documentId);
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

    for (int index=ui->tabWidget->count()-1; index>=0; index--) {

        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->widget(index));
        if (twe->type!=WWW) {
            int documentId = twe->property("documentId").toInt();
            m_plugin->plugin_editor->closeDocument(documentId);
        }
        ui->tabWidget->removeTab(index);
    }

    if (m_plugin->startPage_.widget) {

        addCentralComponent(
                    tr("Start"),
                    m_plugin->startPage_.widget,
                    m_plugin->startPage_.toolbarActions,
                    m_plugin->startPage_.menus,
//                    QList<QWidget*>(),
                    WWW,
                    false);
    }

    QDir sessionDir(QDir::currentPath()+"/.session");
    QStringList es = sessionDir.entryList(
                QStringList() << "*.document",
                QDir::Files,
                QDir::Name
                );

    for (int i=0; i<es.size(); i++) {
        const QString e = es[i];
        QFile f(sessionDir.absoluteFilePath(e));
        if (f.open(QIODevice::ReadOnly)) {
            QDataStream stream(&f);
            QString type, title;
            QString url;
            stream >> type >> title >> url;
            if (type.toLower()=="url") {
                // TODO implement me
            }
            else {
                QString analizerName;
                DocumentType doctype = Text;
                if (type.toLower()=="kumir") {
                    analizerName = "KumirAnalizer";
                    doctype = Program;
                }
                else if (type.toLower()=="pascal") {
                    analizerName = "PascalAnalizer";
                    doctype = Program;
                }
                EditorComponent doc = m_plugin->plugin_editor->newDocument(analizerName, QDir::currentPath(), false);
                QByteArray editorSession = f.readAll();
                QWidget * vc = doc.widget;
                vc->setProperty("documentId", doc.id);
                TabWidgetElement * twe = addCentralComponent(
                            title,
                            vc,
                            doc.toolbarActions,
                            doc.menus,
//                            doc.statusbarWidgets,
                            doctype,
                            true
                            );
                twe->documentId = doc.id;
                twe->setProperty("fileName", title);
                twe->component->setProperty("fileName", title);
                twe->setProperty("realFileName", url);
                twe->component->setProperty("realFileName", url);
                ui->tabWidget->setTabText(ui->tabWidget->count()-1, QFileInfo(title).fileName());
                m_plugin->plugin_editor->restoreState(doc.id, editorSession);

            }
            f.close();
            if (e.endsWith("-active.document"))
                sessionIndex = i+1;
        }
    }

    ui->tabWidget->setCurrentIndex(sessionIndex);
    setupContentForTab();

}

void MainWindow::closeEvent(QCloseEvent *e)
{
    saveSettings();
    m_plugin->saveSession();
    if (m_plugin->sessionsDisableFlag_ && b_notabs) {
        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
        if (twe->type!=WWW) {
            int documentId = twe->documentId;
            bool notSaved = m_plugin->plugin_editor->hasUnsavedChanges(documentId);
            if (!notSaved) {
                e->accept();
                return;
            }
        }
    }
    QMessageBox::StandardButton r = QMessageBox::Discard;
    QStringList unsavedFiles;
    for (int i=0; i<ui->tabWidget->count(); i++) {
        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->widget(i));
        if (twe->type!=WWW) {
            int documentId = twe->documentId;
            bool notSaved = m_plugin->plugin_editor->hasUnsavedChanges(documentId);
            if (notSaved) {
                QString title = ui->tabWidget->tabText(i);
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
        for (int i=0; i<ui->tabWidget->count(); i++) {
            ui->tabWidget->setCurrentIndex(i);
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
    qApp->quit();
}

void MainWindow::switchWorkspace() {
    ExtensionSystem::PluginManager::instance()->showWorkspaceChooseDialog();
}

void MainWindow::saveSession() const
{
    QDir::current().mkpath(".session");
    QDir sessionDir(QDir::currentPath()+"/.session");
    QStringList es = sessionDir.entryList(
                QStringList() << "*.document",
                QDir::Files,
                QDir::Name
                );
    foreach (const QString e, es) {
        sessionDir.remove(e);
    }
    for (int i=1; i<ui->tabWidget->count(); i++) {
        const QString sessionFileName = QString("%1%2.document")
                .arg(i, 2, 10, QChar('0'))
                .arg(i==ui->tabWidget->currentIndex()? "-active" : "");
        QFile sessionFile(sessionDir.absoluteFilePath(sessionFileName));
        if (sessionFile.open(QIODevice::WriteOnly)) {
            QDataStream stream(&sessionFile);
            TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->widget(i));
            if (twe->type==Program)
                stream << QString("kumir");
            else if (twe->type==WWW)
                stream << QString("www");
            else
                stream << QString("text");
            QString title = twe->property("fileName").toString();
            QString fileName = twe->property("realFileName").toString();
            stream << title;
            stream << fileName;
            if (twe->documentId!=-1 && twe->type!=WWW) {
                QByteArray editorSession = m_plugin->plugin_editor->saveState(twe->documentId);
                sessionFile.write(editorSession);
            }
            sessionFile.close();
        }
    }
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
    if (b_notabs) {
        TabWidgetElement * twe =
                qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
        if (twe->type != WWW) {
            int documentId = twe->documentId;
            EditorInterface * editor =
                    PluginManager::instance()->findPlugin<EditorInterface>();
            bool hasUnsavedChanges = editor->hasUnsavedChanges(documentId);
            if (hasUnsavedChanges) {
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
    }
    QStringList filters;    
    AnalizerInterface * analizer =
            PluginManager::instance()->findPlugin<AnalizerInterface>();
    const QString languageName = analizer->languageName();
    const QString fileNameSuffix = analizer->defaultDocumentFileNameSuffix();
    filters << tr("%1 programs (*%2)").arg(languageName).arg(fileNameSuffix);
    if (!b_notabs) {
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
    ui->bottomWidget->setVisible(true);
    int h = ui->bottomWidget->height();
    QList<int> splitterItemHeights = ui->splitter->sizes();
    if (splitterItemHeights[1] < h) {
        int diff = h - splitterItemHeights[1];
        splitterItemHeights[1] = h;
        splitterItemHeights[0] -= diff;
        ui->splitter->setSizes(splitterItemHeights);
    }
    ui->actionShow_Console_Pane->setChecked(true);
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
    if (b_notabs) {
        TabWidgetElement * twe =
                qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
        if (twe->type != WWW) {
            int documentId = twe->documentId;
            EditorInterface * editor =
                    PluginManager::instance()->findPlugin<EditorInterface>();
            bool hasUnsavedChanges = editor->hasUnsavedChanges(documentId);
            if (hasUnsavedChanges) {
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
    }
    QString fn = fullPath.trimmed();
//    QStringList r = recentFiles(true);
//    if (index>=0 && index<r.size())
//        loadFromUrl("file://"+r[index]);
    if (!fn.isEmpty() && QFile(fn).exists())
        loadFromUrl("file://"+fn);
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
    if (type==Program) {
        QFileInfo f(url.toLocalFile());
        if (f.isReadable()) {
            using namespace ExtensionSystem;
            using namespace Shared;
            AnalizerInterface * analizer =
                    PluginManager::instance()->findPlugin<AnalizerInterface>();
            KPlugin * analizerPlugin =
                    PluginManager::instance()->findKPlugin<AnalizerInterface>();
            const QString analizerPluginName = analizerPlugin->pluginSpec().name;
            EditorComponent doc = m_plugin->plugin_editor->newDocument(
                        analizerPluginName,
                        QFileInfo(f).absoluteDir().absolutePath(),
                        false
                        );
            QWidget * vc = doc.widget;
            int id = doc.id;
            m_plugin->plugin_editor->loadDocument(
                        id,
                        f.absoluteFilePath(),
                        analizer->indentsSignificant()
                        );
            vc->setProperty("documentId", id);
            QString fileName = QFileInfo(url.toLocalFile()).fileName();
            vc->setProperty("fileName", url.toLocalFile());
            vc->setProperty("realFileName", url.toLocalFile());
            vc->setProperty("title", fileName);
            if (b_notabs) {
                while(ui->tabWidget->count()) ui->tabWidget->removeTab(0);
            }
            result = addCentralComponent(
                        fileName,
                        vc,
                        doc.toolbarActions,
                        doc.menus,
//                        doc.statusbarWidgets,
                        Program,
                        true);
            result->documentId = id;
            result->url = url;
            ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
            ui->tabWidget->currentWidget()->setFocus();
            setupContentForTab();
        }
    }    
    else if (type==Text) {
        QFile f(url.toLocalFile());
        if (f.open(QIODevice::Text|QIODevice::ReadOnly)) {
            QTextStream ts(&f);
            ts.setAutoDetectUnicode(true);
            QString data = ts.readAll();
            f.close();
            EditorComponent doc = m_plugin->plugin_editor->newDocument("", "", false);
            QWidget* vc = doc.widget;
            int id = doc.id;
            vc->setProperty("documentId", id);
            QString fileName = QFileInfo(url.toLocalFile()).fileName();
            vc->setProperty("fileName", url.toLocalFile());
            vc->setProperty("realFileName", url.toLocalFile());
            vc->setProperty("title", fileName);
            if (b_notabs) {
                while(ui->tabWidget->count()) ui->tabWidget->removeTab(0);
            }
            result = addCentralComponent(
                        fileName,
                        vc,
                        doc.toolbarActions,
                        doc.menus,
//                        doc.statusbarWidgets,
                        Text,
                        true);
            result->documentId = id;
            result->url = url;
            ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
            ui->tabWidget->currentWidget()->setFocus();
        }
    }
    else if (type==WWW) {
        BrowserComponent browser = m_plugin->plugin_browser->createBrowser(url, m_plugin->m_browserObjects);
        if (b_notabs) {
            while(ui->tabWidget->count()) ui->tabWidget->removeTab(0);
        }
        result = addCentralComponent(
                    url.toString(),
                    browser.widget,
                    browser.toolbarActions,
                    browser.menus,
//                    QList<QWidget*>(),
                    WWW,
                    true);
        result->url = url;
        ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
        ui->tabWidget->currentWidget()->setFocus();
    }
    setTitleForTab(ui->tabWidget->currentIndex());
    ExtensionSystem::PluginManager::instance()->switchGlobalState(ExtensionSystem::GS_Unlocked);
    return result;
}

TabWidgetElement* MainWindow::loadFromCourseManager(
        const GuiInterface::ProgramSourceText &data
        )
{
    typedef GuiInterface::ProgramSourceText ST;
    TabWidgetElement * result = nullptr;
    for (int i=0; i<ui->tabWidget->count(); i++) {
        TabWidgetElement * courseTab =
                qobject_cast<TabWidgetElement*>(ui->tabWidget->widget(i));
        if (courseTab && courseTab->isCourseManagerTab()) {
            result = courseTab;
            break;
        }
    }
    if (data.language == ST::Kumir) {
        QWidget * vc = nullptr;
        int id = -1;
        if (result) {
            // Reuse opened course manager tab
            id = result->documentId;
            vc = result->component;
        }
        else {
            // Create new course manager tab
            EditorComponent doc =
                    m_plugin->plugin_editor->newDocument(
                        "KumirAnalizer",
                        QFileInfo(data.url.toLocalFile())
                            .absoluteDir().absolutePath(),
                        false
                        );
            vc = doc.widget;
            id = doc.id;
            QList<QAction*> toolBarActions = doc.toolbarActions;
            QList<QMenu*> menus = doc.menus;
            result = addCentralComponent(
                        data.title,
                        vc,
                        toolBarActions,
                        menus,
//                        doc.statusbarWidgets,
                        Program,
                        true
                        );
        }
        m_plugin->plugin_editor->loadDocument(id, data.content);
        result->saved = data.saved;
        result->changed = data.changed;
        result->url = data.url;
        result->documentId = id;
        vc->setProperty("title", data.title);
        vc->setProperty("documentId", id);
        vc->setProperty("fromCourseManager", true);
        vc->setProperty("realFileName", data.url.toLocalFile());
    }
    ui->tabWidget->setCurrentWidget(result);
    return result;
}

Shared::GuiInterface::ProgramSourceText
MainWindow::courseManagerProgramSource() const
{
    typedef GuiInterface::ProgramSourceText ST;
    ST result;
    result.language = ST::Kumir; // TODO implement for other languages
    TabWidgetElement * tab = nullptr;
    for (int i=0; i<ui->tabWidget->count(); i++) {
        TabWidgetElement * courseTab =
                qobject_cast<TabWidgetElement*>(ui->tabWidget->widget(i));
        if (courseTab && courseTab->isCourseManagerTab()) {
            tab = courseTab;
            break;
        }
    }
    if (tab) {
        result.saved = tab->saved;
        result.changed = tab->changed;
        result.url = tab->url;
        result.title = tab->title();
        int id = tab->documentId;
        result.content = m_plugin->plugin_editor->documentContent(id);
    }
    return result;
}

} // namespace CoreGUI

