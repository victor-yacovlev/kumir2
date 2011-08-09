#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "extensionsystem/pluginmanager.h"

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
        QVBoxLayout * l = new QVBoxLayout;
        l->setContentsMargins(0,0,0,0);
        l->setSpacing(0);
        setLayout(l);
        if (enableToolBar) {
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
                tb->addActions(kumir->actions()->actions());
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
        l->addWidget(w);
    }
    QWidget * component;
    QList<QMenu*> menus;
    QList<QWidget*> statusbarWidgets;
    MainWindow::DocumentType type;
    int documentId;
signals:
    void changeTitle(const QString & txt);
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
    b_workspaceSwitching = false;
    ui->setupUi(this);

    ui->menuNew->setIcon(actionIcon("document-new"));
    ui->actionOpen->setIcon(actionIcon("document-open"));
    ui->actionSave->setIcon(actionIcon("document-save"));


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

    statusBar()->setStyleSheet("QStatusBar { border-top: 1px solid darkgray; }");

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

    installEventFilter(this);

}

QString MainWindow::StatusbarWidgetCSS =
"QLabel {"
"   font-size: 15px;"
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

void MainWindow::saveCurrentFile()
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
    const QString fileName = twe->property("realFileName").toString();
    if (fileName.isEmpty()) {
        saveCurrentFileAs();
    }
    else {
        saveCurrentFileTo(fileName);
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

void MainWindow::saveCurrentFileAs()
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
        saveCurrentFileTo(fileName);
        m_plugin->mySettings()->setValue(Plugin::RecentFileKey, fileName);
        twe->setProperty("fileName", fileName);
        twe->setProperty("realFileName", fileName);
        int index = ui->tabWidget->indexOf(twe);
        ui->tabWidget->setTabText(index, QFileInfo(fileName).fileName());
        addToRecent(fileName);
    }
}

void MainWindow::saveCurrentFileTo(const QString &fileName)
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->currentWidget());
    int documentId = twe->property("documentId").toInt();
    m_plugin->plugin_editor->saveDocument(documentId, fileName);
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

void MainWindow::closeTab(int index)
{
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->widget(index));
    ui->tabWidget->removeTab(index);
    if (twe->type!=WWW) {
        int documentId = twe->property("documentId").toInt();
        m_plugin->plugin_editor->closeDocument(documentId);
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

        QObject * o = m_plugin->myDependency("Analizer");
        QString defaultText;
        QString suffix = ".txt";
        DocumentType type = Text;
        if (QString(o->metaObject()->className()).toLower().contains("kumir")) {
            defaultText = QString::fromUtf8("алг\nнач\n\nкон");
            suffix = ".kum";
            type = Kumir;
        }
        else if (QString(o->metaObject()->className()).toLower().contains("pascal")) {
            defaultText = "program;\n\nbegin\n\t\nend.";
            type = Pascal;
            suffix = ".pas";
        }
        const QString initialText = m_plugin->mySettings()->value(Plugin::InitialTextKey, defaultText).toString();
        Shared::EditorComponent doc = m_plugin->plugin_editor->newDocument("Analizer", initialText);
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
    Shared::EditorComponent doc = m_plugin->plugin_editor->newDocument("", "");
    QWidget * vc = doc.widget;
    int id = doc.id;
    vc->setProperty("documentId", id);
    QString fileName = suggestNewFileName(".txt");
    vc->setProperty("fileName", fileName);
    TabWidgetElement * e = addCentralComponent(
                fileName,
                vc,
                doc.toolbarActions,
                doc.menus,
                doc.statusbarWidgets,
                Text,
                true);
    ui->tabWidget->setCurrentWidget(e);
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
    Q_UNUSED(type);
    l_dockWindows << dock;
    ui->menuWindow->addAction(dock->toggleViewAction());
    if (type!=Terminal) {
        QMenu * componentMenu = new QMenu(title, this);
        componentMenu->addActions(menuActions);
        ui->menubar->insertMenu(ui->menuWindow->menuAction(), componentMenu);
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
}

void MainWindow::disableTabs()
{
    ui->tabWidget->disableTabs();
}

void MainWindow::restoreSession(const QByteArray & data)
{
    QRect r = m_plugin->mySettings()->value(Plugin::MainWindowGeometryKey, QRect(-1,-1,0,0)).toRect();
    if (r.width()>0) {
        resize(r.size());
        move(r.topLeft());
    }
    restoreState(m_plugin->mySettings()->value(Plugin::MainWindowStateKey).toByteArray());
    for (int i=0; i<l_dockWindows.size(); i++) {
//        l_dockWindows[i]->restoreState(m_plugin->mySettings(), this, i==0);
//        addDockWidget(Qt::BottomDockWidgetArea, l_dockWindows[i]);
    }
//    restoreState(m_plugin->mySettings()->value(Plugin::MainWindowStateKey).toByteArray());

    if (m_plugin->b_nosessions) {
        newProgram();
    }
    else {

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
                    QStringList() << "*.kum" << "*.pas" << "*.txt" << "*.url",
                    QDir::Files,
                    QDir::Name
                    );
        for (int i=0; i<es.size(); i++) {
            const QString e = es[i];
            if (e.endsWith(".url")) {
                // TODO implement me
            }
            else {
                TabWidgetElement * twe = loadFromUrl(QUrl::fromLocalFile(sessionDir.absoluteFilePath(e)), false);
                const QString index = QFileInfo(e).baseName();
                QFile state(sessionDir.absoluteFilePath(index+".dstate"));
                if (state.open(QIODevice::ReadOnly|QIODevice::Text)) {
                    QTextStream ts(&state);
                    ts.setCodec("UTF-8");
                    ts.setAutoDetectUnicode(true);
                    QStringList lines = ts.readAll().split("\n", QString::SkipEmptyParts);
                    state.close();
                    for (int j=0; j<lines.size(); j++) {
                        const QStringList pair = lines[j].split("=");
                        if (pair.size()==2) {
                            const QString key = pair[0].trimmed().toLower();
                            const QString value = pair[1].trimmed();
                            if (key=="filename") {
                                twe->setProperty("fileName", value);
                                twe->component->setProperty("fileName", value);
                                ui->tabWidget->setTabText(ui->tabWidget->count()-1, QFileInfo(value).fileName());
                            }
                            if (key=="realfilename") {
                                twe->setProperty("realFileName", value);
                                twe->component->setProperty("realFileName", value);
                            }
                        }
                    }
                    int documentId = twe->property("documentId").toInt();
                    m_plugin->plugin_editor->restoreState(documentId, lines.join("\n"));
                }
            }
        }

        if (data.size()>0) {
            int tabIndex = quint8(data[0]);
            ui->tabWidget->setCurrentIndex(qMin(tabIndex, ui->tabWidget->count()-1));
            setupContentForTab();
        }
    }

}

void MainWindow::switchWorkspace() {
    ExtensionSystem::PluginManager::instance()->showWorkspaceChooseDialog();
}

QByteArray MainWindow::saveSession() const
{
    QRect r(pos(), size());
    QSettings * sett = m_plugin->mySettings();
    sett->setValue(Plugin::MainWindowGeometryKey, r);
    sett->setValue(Plugin::MainWindowStateKey, saveState());

    QByteArray result;

    if (!m_plugin->b_nosessions) {

        result.append(quint8(ui->tabWidget->currentIndex()));

        QDir::current().mkdir(".session");
        QDir sessionDir(QDir::currentPath()+"/.session");
        QStringList es = sessionDir.entryList(
                    QStringList() << "*.kum" << "*.pas" << "*.txt" << "*.url" << ".dstate",
                    QDir::Files,
                    QDir::Name
                    );
        foreach (const QString e, es) {
            sessionDir.remove(e);
        }

        for (int i=1; i<ui->tabWidget->count(); i++) {
            TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(ui->tabWidget->widget(i));
            QString suffix = "txt";
            if (twe->type==Kumir)
                suffix = "kum";
            if (twe->type==Pascal)
                suffix = "pas";
            if (twe->type==WWW)
                suffix = "url";
            int documentId = twe->property("documentId").toInt();
            if (twe->type!=WWW) {
                m_plugin->plugin_editor->saveDocument(documentId, QString(".session/%1.%2").arg(i).arg(suffix));
                QFile state(QString(".session/%1.dstate").arg(i));
                if (state.open(QIODevice::WriteOnly|QIODevice::Text)) {
                    QTextStream ts(&state);
                    ts.setCodec("UTF-8");
                    ts.setGenerateByteOrderMark(true);
                    ts << "fileName=" << twe->property("fileName").toString() << "\n";
                    ts << "realFileName=" << twe->property("realFileName").toString() << "\n";
                    ts << m_plugin->plugin_editor->saveState(documentId);
                }
                state.close();
            }
            else
            {
                QFile url(QString(".session/%1.%2").arg(i).arg(suffix));
                if (url.open(QIODevice::WriteOnly|QIODevice::Text)) {
                    QTextStream ts(&url);
                    ts.setCodec("UTF-8");
                    ts.setGenerateByteOrderMark(true);
                    ts << twe->property("realFileName").toString();
                }
                url.close();
            }

        }
    //    for (int i=0; i<l_dockWindows.size(); i++) {
    //        l_dockWindows[i]->saveState(sett);
    //    }
    }

    return result;
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
    r = r.mid(0, qMax(5, r.size()));
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
