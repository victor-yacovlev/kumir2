#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "extensionsystem/pluginmanager.h"
#include "confirmclosedialod.h"
#include "aboutdialog.h"
#include "kumirprogram.h"



#include <algorithm>

namespace CoreGUI {


class TabWidgetElement
        : public QWidget
{
    Q_OBJECT
public:
    inline explicit TabWidgetElement(QWidget * w
                                     , bool enableToolBar
                                     , QList<QAction*> toolbarActions
                                     , QList<QMenu*> ms
                                     , QList<QWidget*> sws
                                     , MainWindow::DocumentType t
                                     , QActionGroup * gr_fileActions
                                     , QActionGroup * gr_otherActions
                                     , class KumirProgram * kumir
                                     , class PascalProgram * pascal
                                     )
        : QWidget()
        , component(w)
        , menus(ms)
        , statusbarWidgets(sws)
        , type(t)
        , documentId(-1)
    {
        setProperty("uncloseable", w->property("uncloseable"));
        setProperty("documentId", w->property("documentId"));
        setProperty("fileName", w->property("fileName"));
        setProperty("realFileName", w->property("realFileName"));
        documentId = w->property("documentId").toInt();
        if (type==MainWindow::WWW) {
            connect(w, SIGNAL(titleChanged(QString)), this, SIGNAL(changeTitle(QString)));
        }
        else {
            connect(w, SIGNAL(documentCleanChanged(bool)), this, SIGNAL(documentCleanChanged(bool)));
        }
        QVBoxLayout * l = new QVBoxLayout;
        l->setContentsMargins(0,0,0,0);
        l->setSpacing(0);
        setLayout(l);
        if (enableToolBar) {
            if (!toolbarActions.isEmpty()) {
                QToolBar * tb = new QToolBar(this);
#ifndef Q_OS_MAC
                tb->setStyleSheet("QToolBar { border: 0px }");
#endif
                l->addWidget(tb);
                if (type!=MainWindow::WWW) {
                    tb->addActions(gr_fileActions->actions());
                }
                tb->addSeparator();
                foreach (QAction * a, toolbarActions)
                    tb->addAction(a);
                if (type==MainWindow::Kumir) {
                    tb->addSeparator();
                    QList<QAction*> acts = kumir->actions()->actions();
                    for (int i=0; i<acts.size(); i++) {
                        if (acts[i]->isSeparator() || !acts[i]->icon().isNull())
                            tb->addAction(acts[i]);
                    }
                }
                if (type==MainWindow::Pascal) {
                    tb->addSeparator();
                    Q_UNUSED(pascal);
                    //                tb->addActions(pascal->actions()->actions());
                }
                if (!gr_otherActions->actions().isEmpty()) {
                    tb->addSeparator();
                    tb->addActions(gr_otherActions->actions());
                }
            }

        }
        l->addWidget(w);
    }
    QWidget * component;
    QList<QMenu*> menus;
    QList<QWidget*> statusbarWidgets;
    MainWindow::DocumentType type;
    int documentId;
signals:
    void changeTitle(const QString & txt);
    void documentCleanChanged(bool v);
protected:
    inline void focusInEvent(QFocusEvent *e) {
        QWidget::focusInEvent(e);
        component->setFocus();
    }

};

MainWindow::MainWindow(Plugin * p) :
    QMainWindow(0),
    ui(new Ui::MainWindow),
    m_plugin(p)
{
    b_notabs = false;
    b_workspaceSwitching = false;
    ui->setupUi(this);

    ui->actionNewProgram->setIcon(actionIcon("document-new"));
    ui->actionOpen->setIcon(actionIcon("document-open"));
    ui->actionSave->setIcon(actionIcon("document-save"));


    ui->menuFile->setWindowTitle(ui->menuFile->title());
    ui->menuHelp->setWindowTitle(ui->menuHelp->title());

    connect(ui->actionNewProgram, SIGNAL(triggered()), this, SLOT(newProgram()));
    connect(ui->actionNewText, SIGNAL(triggered()), this, SLOT(newText()));
    connect(ui->actionNew_pascal_program, SIGNAL(triggered()), this, SLOT(newPascalProgram()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(closeCurrentTab()));

    connect(ui->actionSwitch_workspace, SIGNAL(triggered()), this, SLOT(switchWorkspace()));

    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(setupActionsForTab()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(setupContentForTab()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(setupStatusbarForTab()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(checkCounterValue()));

    connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(showPreferences()));

    gr_fileActions = new QActionGroup(this);
    gr_fileActions->addAction(ui->actionSave);
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveCurrentFile()));
    connect(ui->actionSave_as, SIGNAL(triggered()), this, SLOT(saveCurrentFileAs()));

    gr_otherActions = new QActionGroup(this);

    a_notAvailable = new QAction(this);
    a_notAvailable->setText(tr("No actions for this tab"));
    a_notAvailable->setEnabled(false);

    menu_empty = new QMenu(this);
    menu_empty->addAction(a_notAvailable);


    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);

    statusBar()->setStyleSheet("QStatusBar { border-top: 1px solid darkgray; } ");

    statusBar()->addWidget(m_plugin->m_kumirStateLabel);
    statusBar()->addWidget(m_plugin->m_genericCounterLabel);
    m_plugin->m_kumirStateLabel->setFixedWidth(120);
    m_plugin->m_kumirStateLabel->setStyleSheet(StatusbarWidgetCSS);
    m_plugin->m_kumirStateLabel->setAlignment(Qt::AlignCenter);
    m_plugin->m_genericCounterLabel->setFixedWidth(120);
    m_plugin->m_genericCounterLabel->setStyleSheet(StatusbarWidgetCSS);
    m_plugin->m_genericCounterLabel->setAlignment(Qt::AlignCenter);
    m_message = new QLabel(this);
    m_message->setAlignment(Qt::AlignCenter);
    m_message->setStyleSheet(StatusbarWidgetCSS);
    statusBar()->addWidget(m_message, 1);

    i_timerId = startTimer(1000);

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
    connect(ui->actionUsage, SIGNAL(triggered()), this, SLOT(showUserManual()));

    installEventFilter(this);
    installEventFilter(menuBar());

    ui->actionRecent_files->setMenu(new QMenu());
    connect(ui->actionRecent_files->menu(), SIGNAL(aboutToShow()), this, SLOT(prepareRecentFilesMenu()));

}

QString MainWindow::StatusbarWidgetCSS =
"QLabel {"
"   font-size: 12px;"
"   padding: 4px;"
"}"
;

void MainWindow::changeFocusOnMenubar()
{
    QMenu * firstMenu = menuBar()->findChild<QMenu*>();
    if (!menuBar()->hasFocus()) {
        menuBar()->setFocus();
        if (firstMenu) {
            menuBar()->setActiveAction(firstMenu->menuAction());
        }
    }
    else {
        menuBar()->setActiveAction(0);
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
    return false;
}


void MainWindow::checkCounterValue()
{
    ExtensionSystem::GlobalState state = ExtensionSystem::PluginManager::instance()->currentGlobalState();
    if (state==ExtensionSystem::GS_Unlocked) {
        if (ui->tabWidget->count()==0)
            return;
        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
        if (!twe)
            return;
        if (twe->type==Kumir || twe->type==Pascal) {
            int id = twe->documentId;
            quint32 errorsCount = m_plugin->plugin_editor->errorsCount(id);
            if (errorsCount==0) {
                m_plugin->m_genericCounterLabel->setText(tr("No errors"));
                m_plugin->m_genericCounterLabel->setStyleSheet(StatusbarWidgetCSS);
            }
            else {
                m_plugin->m_genericCounterLabel->setText(tr("Errors: %1").arg(errorsCount));
                m_plugin->m_genericCounterLabel->setStyleSheet(StatusbarWidgetCSS+
                                                               "QLabel {"
                                                               "    color: red;"
                                                               "}"
                                                               );
            }
        }
        else {
            m_plugin->m_genericCounterLabel->setText("");
        }
    }
    else {
        // TODO steps done
    }
}

void MainWindow::showMessage(const QString &text)
{
    m_message->setText(text);
}

void MainWindow::clearMessage()
{
    m_message->setText("");
}

void MainWindow::setFocusOnCentralWidget()
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
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
    const QString fileName = twe->property("realFileName").toString();
    if (fileName.isEmpty()) {
        return saveCurrentFileAs();
    }
    else {
        return saveCurrentFileTo(fileName);
    }
}

void MainWindow::setupActionsForTab()
{
    QWidget * currentTabWidget = ui->tabWidget->currentWidget();

    if (!currentTabWidget)
        return;

    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(currentTabWidget);
    ui->actionSave->setEnabled(twe->type!=WWW);
    ui->actionSave_as->setEnabled(twe->type!=WWW);

    prepareEditMenu();
    prepareInsertMenu();
    prepareRunMenu();
}

void MainWindow::setupStatusbarForTab()
{
    foreach (QWidget * w, l_tabDependentStatusbarWidgets) {
        statusBar()->removeWidget(w);
    }
    QWidget * currentTabWidget = ui->tabWidget->currentWidget();
    if (!currentTabWidget)
        return;
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(currentTabWidget);
    foreach (QWidget * w, twe->statusbarWidgets) {
        l_tabDependentStatusbarWidgets << w;
        w->setStyleSheet(StatusbarWidgetCSS);
        statusBar()->addPermanentWidget(w);
        w->show();
    }
}

void MainWindow::prepareRecentFilesMenu()
{
    ui->actionRecent_files->menu()->clear();
    QStringList r = m_plugin->mySettings()->value(Plugin::RecentFilesKey).toStringList();
    for (int i=0; i<r.size(); i++) {
        if (!r[i].trimmed().isEmpty()) {
            QAction * a = ui->actionRecent_files->menu()->addAction(r[i]);
            a->setProperty("index", i);
            connect(a, SIGNAL(triggered()), this, SLOT(loadRecentFile()));
        }
    }
}

void MainWindow::loadRecentFile()
{
    QAction * a = qobject_cast<QAction*>(sender());
    if (a && a->property("index").isValid()) {
        int index = a->property("index").toInt();
        loadRecentFile(index);
    }
}

void MainWindow::prepareRunMenu()
{
    ui->menuRun->clear();
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
    if (!twe)
        return;
    if (twe->type==Kumir) {
        ui->menuRun->addActions(m_plugin->m_kumirProgram->actions()->actions());
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
        ui->menuEdit->menuAction()->setMenu(menu_empty);
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
        ui->menuInsert->menuAction()->setMenu(menu_empty);
    }
}

bool MainWindow::saveCurrentFileAs()
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
    QString fileName = twe->property("fileName").toString();
    QStringList filter;
    if (twe->type==Kumir) {
        filter << tr("Kumir programs (*.kum)");
    }
    if (twe->type==Pascal) {
        filter << tr("Pascal programs (*.pas *.pp");
    }
    if (twe->type==Text) {
        filter << tr("Text files (*.txt)");
    }
    filter << tr("All files (*)");
    fileName = QFileDialog::getSaveFileName(this, tr("Save file"), fileName, filter.join(";;"));
    if (!fileName.isEmpty()) {
        if (twe->type==Kumir && !fileName.endsWith(".kum"))
            fileName += ".kum";
        if (twe->type==Pascal && !(fileName.endsWith(".pas") || fileName.endsWith(".pp")))
            fileName += ".pas";
        if (saveCurrentFileTo(fileName)) {
            m_plugin->mySettings()->setValue(Plugin::RecentFileKey, fileName);
            twe->setProperty("fileName", fileName);
            twe->setProperty("realFileName", fileName);
            int index = ui->tabWidget->indexOf(twe);
            ui->tabWidget->setTabText(index, QFileInfo(fileName).fileName());
            addToRecent(fileName);
            return true;
        }
    }
    return false;
}

bool MainWindow::saveCurrentFileTo(const QString &fileName)
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
    int documentId = twe->property("documentId").toInt();
    QString error = m_plugin->plugin_editor->saveDocument(documentId, fileName);
    if (error.isEmpty()) {
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
    ui->tabWidget->setTabText(index, text);
}

void MainWindow::handleTabTitleChange(const QString &title)
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(sender());
    int index = ui->tabWidget->indexOf(twe);
    ui->tabWidget->setTabText(index, title);
}

void MainWindow::closeCurrentTab()
{
    closeTab(ui->tabWidget->currentIndex());
}

bool MainWindow::closeTab(int index)
{
    if (index==-1 || index>=ui->tabWidget->count())
        return true;
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->widget(index));
    if (twe->type!=WWW) {
        int documentId = twe->property("documentId").toInt();
        bool notSaved = m_plugin->plugin_editor->hasUnsavedChanges(documentId);
        ConfirmCloseDialod::Result r;
        if (!notSaved) {
            r = ConfirmCloseDialod::SaveNothing;
        }
        else {
            ui->tabWidget->setCurrentIndex(index);
            ConfirmCloseDialod dialog(false, this);
            dialog.setMainText(b_notabs
                               ? tr("Before creating new program:")
                               : tr("Before closing tab:")
                                   );
            dialog.setCancelText(b_notabs
                               ? tr("Cancel \"New program\"")
                               : tr("Cancel tab close"));
            dialog.setDiscardText(tr("Close without saving"));
            dialog.exec();
            r = dialog.result();
        }
        if (r==ConfirmCloseDialod::SaveNothing) {
            m_plugin->plugin_editor->closeDocument(documentId);
            ui->tabWidget->removeTab(index);
            return true;
        }
        else if (r==ConfirmCloseDialod::SaveFiles) {
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

void MainWindow::disablePascalProgram()
{
    ui->actionNew_pascal_program->setEnabled(false);
    ui->actionNew_pascal_program->setVisible(false);
}

void MainWindow::newProgram()
{
    if (b_notabs && !closeTab(ui->tabWidget->currentIndex())) {
        return;
    }
    QString defaultText = QString::fromUtf8("алг\nнач\n\nкон");
    QString suffix = ".kum";
    DocumentType type = Kumir;
    const QString initialText = m_plugin->mySettings()->value(Plugin::InitialTextKey, defaultText).toString();
    Shared::EditorComponent doc = m_plugin->plugin_editor->newDocument("KumirAnalizer", initialText);
    QWidget* vc = doc.widget;
    int id = doc.id;
    vc->setProperty("documentId", id);
    QString fileName = suggestNewFileName(suffix);
    vc->setProperty("fileName", QDir::current().absoluteFilePath(fileName));
    TabWidgetElement * e = addCentralComponent(
                fileName,
                vc,
                doc.toolbarActions,
                doc.menus,
                doc.statusbarWidgets,
                type,
                true);
    ui->tabWidget->setCurrentWidget(e);
    e->setFocus();

}

void MainWindow::newPascalProgram()
{
    if (b_notabs && !closeTab(ui->tabWidget->currentIndex())) {
        return;
    }
    QString defaultText = QString::fromUtf8("program;\n\nbegin\n\nend.\n");
    QString suffix = ".pas";
    DocumentType type = Pascal;
    const QString initialText = m_plugin->mySettings()->value(Plugin::InitialTextKey, defaultText).toString();
    Shared::EditorComponent doc = m_plugin->plugin_editor->newDocument("PascalAnalizer", initialText);
    QWidget* vc = doc.widget;
    int id = doc.id;
    vc->setProperty("documentId", id);
    QString fileName = suggestNewFileName(suffix);
    vc->setProperty("fileName", QDir::current().absoluteFilePath(fileName));
    TabWidgetElement * e = addCentralComponent(
                fileName,
                vc,
                doc.toolbarActions,
                doc.menus,
                doc.statusbarWidgets,
                type,
                true);
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
    Shared::EditorComponent doc = m_plugin->plugin_editor->newDocument("", text, true);
    QWidget * vc = doc.widget;
    int id = doc.id;
    vc->setProperty("documentId", id);
    vc->setProperty("fileName", fileName);
    TabWidgetElement * e = addCentralComponent(
                QFileInfo(fileName).fileName(),
                vc,
                doc.toolbarActions,
                doc.menus,
                doc.statusbarWidgets,
                Text,
                true);
    ui->tabWidget->setCurrentWidget(e);
    if (!text.isEmpty()) {
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),
                                  ui->tabWidget->tabText(ui->tabWidget->currentIndex())
                                  +"*");
    }
    e->setFocus();
}

QString MainWindow::suggestNewFileName(const QString &suffix) const
{
    QDir d = QDir::current();
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
    , const QList<QWidget*> & statusbarWidgets
    , DocumentType type
    , bool enableToolBar)
{
    class KumirProgram * kumir = 0;
    class PascalProgram * pascal = 0;
    if (type==Kumir) {
        kumir = m_plugin->m_kumirProgram;
    }
    TabWidgetElement * element = new TabWidgetElement(
                c,
                enableToolBar,
                toolbarActions,
                menus,
                statusbarWidgets,
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



QDockWidget * MainWindow::addSecondaryComponent(const QString & title
                                                , QWidget * c
                                                , const QList<QAction*> & toolbarActions
                                                , const QList<QAction*> & menuActions
                                                , DockWindowType type)
{
    QDockWidget * dock = new QDockWidget(title, this);
    dock->setVisible(false);
    dock->setWidget(c);
    dock->setObjectName(title);
    Q_UNUSED(toolbarActions);
    if (type!=SubControl && type!=Help) {
        ui->menuWindow->addAction(dock->toggleViewAction());
        l_dockWindows << dock;
    }
    if (type!=Terminal) {
        if (type!=SubControl && type!=Control && type!=Help) {
            QMenu * componentMenu = new QMenu(title, this);
            componentMenu->addActions(menuActions);
            ui->menubar->insertMenu(ui->menuWindow->menuAction(), componentMenu);
        }
        dock->setFloating(true);
    }
    else {
        dock->setFloating(false);
        addDockWidget(Qt::BottomDockWidgetArea, dock, Qt::Horizontal);
    }
    return dock;
}



void MainWindow::createTopLevelMenus(const QList<QMenu*> & c, bool tabDependent)
{
    QList<QMenu*> menus;
    for (int i=0; i<menuBar()->children().size(); i++) {
        QMenu * m = qobject_cast<QMenu*>(menuBar()->children()[i]);
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
            QMenu * menu = new QMenu(title, menuBar());
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
            menuBar()->insertMenu(ui->menuWindow->menuAction(), menu);
        }
    }
}

void MainWindow::setupContentForTab()
{
    QWidget * currentTabWidget = ui->tabWidget->currentWidget();

    if (!currentTabWidget)
        return;

    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(currentTabWidget);
    if (twe->type==Kumir) {
        const int id = twe->property("documentId").toInt();
        const QString fileName = twe->property("fileName").toString();
        const AnalizerInterface * analizer = m_plugin->plugin_editor->analizer(id);
        int analizerId = m_plugin->plugin_editor->analizerDocumentId(id);
        const AST::Data * ast = analizer->abstractSyntaxTree(analizerId);
        m_plugin->m_kumirProgram->setAST(ast);
        m_plugin->m_kumirProgram->setSourceFileName(fileName);
        m_plugin->m_kumirProgram->setDocumentId(id);
    }
    else {
        m_plugin->m_kumirProgram->setAST(0);
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
}

void MainWindow::saveSettings()
{
    QRect r(pos(), size());
    QSettings * sett = m_plugin->mySettings();
    sett->setValue(Plugin::MainWindowGeometryKey, r);
    sett->setValue(Plugin::MainWindowStateKey, saveState());
}

void MainWindow::restoreSession()
{
    int sessionIndex = 0;

    for (int index=ui->tabWidget->count()-1; index>=0; index--) {

        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->widget(index));
        if (twe->type!=WWW) {
            int documentId = twe->property("documentId").toInt();
            m_plugin->plugin_editor->closeDocument(documentId);
        }
        ui->tabWidget->removeTab(index);
    }

    addCentralComponent(
                tr("Start"),
                m_plugin->m_startPage.widget,
                m_plugin->m_startPage.toolbarActions,
                m_plugin->m_startPage.menus,
                QList<QWidget*>(),
                WWW,
                false);

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
                    doctype = Kumir;
                }
                else if (type.toLower()=="pascal") {
                    analizerName = "PascalAnalizer";
                    doctype = Pascal;
                }
                EditorComponent doc = m_plugin->plugin_editor->newDocument(analizerName, "");
                QByteArray editorSession = f.readAll();
                QWidget * vc = doc.widget;
                vc->setProperty("documentId", doc.id);
                TabWidgetElement * twe = addCentralComponent(
                            title,
                            vc,
                            doc.toolbarActions,
                            doc.menus,
                            doc.statusbarWidgets,
                            doctype,
                            true
                            );
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
    if (m_plugin->b_nosessions && b_notabs) {
        TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
        if (twe->type!=WWW) {
            int documentId = twe->property("documentId").toInt();
            bool notSaved = m_plugin->plugin_editor->hasUnsavedChanges(documentId);
            if (!notSaved) {
                e->accept();
                return;
            }
        }
    }
    ConfirmCloseDialod dialog(!m_plugin->b_nosessions, this);
    dialog.exec();
    if (dialog.result()==ConfirmCloseDialod::Cancel) {
        e->ignore();
    }
    else if (dialog.result()==ConfirmCloseDialod::SaveNothing) {
        if (!m_plugin->b_nosessions) {
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
        }
        e->accept();
    }
    else if (dialog.result()==ConfirmCloseDialod::SaveSession) {
        saveSession();
        e->accept();
    }
    else if (dialog.result()==ConfirmCloseDialod::SaveFiles) {
        for (int i=0; i<ui->tabWidget->count(); i++) {
            ui->tabWidget->setCurrentIndex(i);
            if (!saveCurrentFile()) {
                e->ignore();
                return;
            }
        }
        e->accept();
    }
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
            if (twe->type==Kumir)
                stream << QString("kumir");
            else if (twe->type==Pascal)
                stream << QString("pascal");
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

void MainWindow::fileOpen()
{
    const QString recent = m_plugin->mySettings()->value(Plugin::RecentFileKey, QDir::currentPath()).toString();
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Load file..."),
                                                    recent,
                                                    QString(tr("Kumir programs (*.kum);;Pascal programs (*.pas *.pp);;Web pages (*.html *.htm);;Text files (*.txt);;All files (*)"))
                                                    );
    if (!fileName.isEmpty()) {
        m_plugin->mySettings()->setValue(Plugin::RecentFileKey, fileName);
        addToRecent(fileName);
        loadFromUrl(QUrl::fromLocalFile(fileName), true);
    }
}

QStringList MainWindow::recentFiles(bool fullPaths) const
{
    QStringList r = m_plugin->mySettings()->value(Plugin::RecentFilesKey).toStringList();
    if (fullPaths) {
        QStringList result;
        foreach (const QString & s, r) {
            if (QFileInfo(s).isRelative())
                result << QDir::current().absoluteFilePath(s);
            else
                result << QFileInfo(s).fileName();
        }
        return result;
    }
    else {
        QStringList result;
        foreach (const QString & s, r) {
            result << QFileInfo(s).fileName();
        }

        return result;
    }
}

void MainWindow::addToRecent(const QString &fileName)
{
    QStringList r = m_plugin->mySettings()->value(Plugin::RecentFilesKey).toStringList();
    QString entry;
    if (fileName.startsWith(QDir::currentPath())) {
        entry = fileName.mid(QDir::currentPath().length()+1);
    }
    r.removeAll(entry);
    r.prepend(entry);
    r = r.mid(0, qMin(10, r.size()));
    m_plugin->mySettings()->setValue(Plugin::RecentFilesKey, r);
}

void MainWindow::loadRecentFile(int index)
{
    QStringList r = recentFiles(true);
    if (index>=0 && index<r.size())
        loadFromUrl("file://"+r[index]);
}

TabWidgetElement * MainWindow::loadFromUrl(const QUrl & url, bool addToRecentFiles)
{
    DocumentType type;
    TabWidgetElement * result = 0;
    if (url.scheme().startsWith("file")) {
        const QString fileName = url.toLocalFile();
        const QString suffix = QFileInfo(fileName).suffix();
        if (suffix=="kum")
            type = Kumir;
        else if (suffix=="pas" || suffix=="pp")
            type = Pascal;
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
    if (type==Kumir) {
        QFile f(url.toLocalFile());
        if (f.open(QIODevice::Text|QIODevice::ReadOnly)) {
            QTextStream ts(&f);
            ts.setCodec("UTF-16LE");
            ts.setAutoDetectUnicode(true);
            QStringList lines = ts.readAll().split("\n");
            for (int i=0; i<lines.size(); i++) {
                while (lines[i].startsWith(".")||lines[i].startsWith(" ")) {
                    lines[i] = lines[i].mid(1);
                }
            }
            f.close();
            EditorComponent doc = m_plugin->plugin_editor->newDocument("KumirAnalizer", lines.join("\n"));
            QWidget * vc = doc.widget;
            int id = doc.id;
            vc->setProperty("documentId", id);
            QString fileName = QFileInfo(url.toLocalFile()).fileName();
            vc->setProperty("fileName", url.toLocalFile());
            vc->setProperty("realFileName", url.toLocalFile());
            result = addCentralComponent(
                        fileName,
                        vc,
                        doc.toolbarActions,
                        doc.menus,
                        doc.statusbarWidgets,
                        Kumir,
                        true);
            ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
            ui->tabWidget->currentWidget()->setFocus();
            setupContentForTab();
        }
    }
    else if (type==Pascal) {
        QFile f(url.toLocalFile());
        if (f.open(QIODevice::Text|QIODevice::ReadOnly)) {
            QTextStream ts(&f);
            ts.setAutoDetectUnicode(true);
            QString data = ts.readAll();
            f.close();
            EditorComponent doc = m_plugin->plugin_editor->newDocument("PascalAnalizer", data);
            QWidget* vc = doc.widget;
            int id = doc.id;
            vc->setProperty("documentId", id);
            QString fileName = QFileInfo(url.toLocalFile()).fileName();
            vc->setProperty("fileName", url.toLocalFile());
            vc->setProperty("realFileName", url.toLocalFile());
            result = addCentralComponent(
                        fileName,
                        vc,
                        doc.toolbarActions,
                        doc.menus,
                        doc.statusbarWidgets,
                        Pascal,
                        true);
            ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
            ui->tabWidget->currentWidget()->setFocus();
        }
    }
    else if (type==Text) {
        QFile f(url.toLocalFile());
        if (f.open(QIODevice::Text|QIODevice::ReadOnly)) {
            QTextStream ts(&f);
            ts.setAutoDetectUnicode(true);
            QString data = ts.readAll();
            f.close();
            EditorComponent doc = m_plugin->plugin_editor->newDocument("", data);
            QWidget* vc = doc.widget;
            int id = doc.id;
            vc->setProperty("documentId", id);
            QString fileName = QFileInfo(url.toLocalFile()).fileName();
            vc->setProperty("fileName", url.toLocalFile());
            vc->setProperty("realFileName", url.toLocalFile());
            result = addCentralComponent(
                        fileName,
                        vc,
                        doc.toolbarActions,
                        doc.menus,
                        doc.statusbarWidgets,
                        Text,
                        true);
            ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
            ui->tabWidget->currentWidget()->setFocus();
        }
    }
    else if (type==WWW) {
        BrowserComponent browser = m_plugin->plugin_browser->createBrowser(url, m_plugin->m_browserObjects);
        result = addCentralComponent(
                    url.toString(),
                    browser.widget,
                    browser.toolbarActions,
                    browser.menus,
                    QList<QWidget*>(),
                    WWW,
                    true);
        ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
        ui->tabWidget->currentWidget()->setFocus();
    }
    return result;
}

} // namespace CoreGUI

#include "mainwindow.moc"
