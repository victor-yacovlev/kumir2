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
                                     , MainWindow::DocumentType t
                                     , QActionGroup * gr_fileActions
                                     , QActionGroup * gr_otherActions
                                     , class KumirProgram * kumir
                                     , class PascalProgram * pascal
                                     )
        : QWidget()
        , component(w)
        , menus(ms)
        , type(t)
    {
        setProperty("uncloseable", w->property("uncloseable"));
        setProperty("documentId", w->property("documentId"));
        setProperty("fileName", w->property("fileName"));
        setProperty("realFileName", w->property("realFileName"));
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
    MainWindow::DocumentType type;
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
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(setupActionsForTab()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(setupContentForTab()));

    connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(showPreferences()));

    gr_fileActions = new QActionGroup(this);
    gr_fileActions->addAction(ui->actionSave);
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveCurrentFile()));
    connect(ui->actionSave_as, SIGNAL(triggered()), this, SLOT(saveCurrentFileAs()));

    gr_otherActions = new QActionGroup(this);

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
    if (twe->type!=WWW) {
        int documentId = twe->property("documentId").toInt();
        m_plugin->plugin_editor->closeDocument(documentId);
    }
    ui->tabWidget->removeTab(index);

}

void MainWindow::showPreferences()
{
    ExtensionSystem::PluginManager::instance()->showSettingsDialog();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    QRect r(pos(), size());
    m_plugin->mySettings()->setValue(Plugin::MainWindowGeometryKey, r);
    // TODO check for unsaved changes
    while (ui->tabWidget->count()>1) {
        closeTab(ui->tabWidget->count()-1);
    }
    e->accept();
}

void MainWindow::showEvent(QShowEvent *e)
{
    QRect r = m_plugin->mySettings()->value(Plugin::MainWindowGeometryKey, QRect(-1,-1,0,0)).toRect();
    if (r.width()>0) {
        resize(r.size());
        move(r.topLeft());
    }
    e->accept();
}

QIcon MainWindow::actionIcon(const QString &name)
{
    const QString iconsPath = qApp->property("sharePath").toString()+"/coregui/icons/";
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
    TabWidgetElement * e = addCentralComponent(fileName, vc, doc.toolbarActions, doc.menus, type, true);
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
    TabWidgetElement * e = addCentralComponent(fileName, vc, doc.toolbarActions, doc.menus, Text, true);
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
    , DocumentType type
    , bool enableToolBar)
{
    class KumirProgram * kumir = 0;
    class PascalProgram * pascal = 0;
    if (type==Kumir) {
        kumir = m_plugin->m_kumirProgram;
    }
    TabWidgetElement * element = new TabWidgetElement(c,enableToolBar,toolbarActions,menus,type,gr_fileActions,gr_otherActions,kumir,pascal);
    connect(element, SIGNAL(changeTitle(QString)), this, SLOT(handleTabTitleChange(QString)));
    createTopLevelMenus(menus, true);
    ui->tabWidget->addTab(element, title);
    return element;
}

void MainWindow::addSecondaryComponent(const QString & title
                           , QWidget * c
                           , const QList<QAction*> & toolbarActions
                           , const QList<QMenu*> & menus
                           , DockWindowType type)
{
    QDockWidget * dock = new QDockWidget(title, this);
    dock->setWidget(c);
    Q_UNUSED(toolbarActions);
    Q_UNUSED(menus);
    if (type==Terminal) {
        addDockWidget(Qt::BottomDockWidgetArea, dock);
    }
    else {
        dock->setFloating(true);
    }
    dock->setVisible(type==Terminal);
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
            menuBar()->insertMenu(menus.last()->menuAction(), menu);
        }
    }
}

void MainWindow::setupContentForTab()
{
    QWidget * currentTabWidget = ui->tabWidget->currentWidget();

    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(currentTabWidget);
    if (twe->type==Kumir) {
        const int id = twe->property("documentId").toInt();
        const QString fileName = twe->property("fileName").toString();
        const AST::Data * ast = m_plugin->plugin_editor->analizer(id)->abstractSyntaxTree(id);
        m_plugin->m_kumirProgram->setAST(ast);
        m_plugin->m_kumirProgram->setSourceFileName(fileName);
    }
}

void MainWindow::setupActionsForTab()
{
    QWidget * currentTabWidget = ui->tabWidget->currentWidget();

    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(currentTabWidget);
    ui->actionSave->setEnabled(twe->type!=WWW);
    ui->actionSave_as->setEnabled(twe->type!=WWW);
    QList<QMenu*> menus;
    for (int i=0; i<menuBar()->children().size(); i++) {
        QMenu * m = qobject_cast<QMenu*>(menuBar()->children()[i]);
        if (m)
            menus << m;
    }
    for (int i=0; i<twe->menus.size(); i++) {
        QMenu * tabMenu = twe->menus[i];
        QMenu * myMenu = 0;
        for (int j=0; j<menus.size(); j++) {
            if (tabMenu->title().trimmed()==menus[j]->title().trimmed()) {
                myMenu = menus[j];
                break;
            }
        }
        Q_CHECK_PTR(myMenu);
        myMenu->menuAction()->setMenu(tabMenu);
    }
}

void MainWindow::restoreSession()
{
    QStringList sessionFiles = m_plugin->mySettings()->value(Plugin::SessionFilesListKey).toStringList();
    int tabIndex = m_plugin->mySettings()->value(Plugin::SessionTabIndexKey, 0).toInt();
    tabIndex = qMax(0, qMin(sessionFiles.size()-1, tabIndex));
    for (int i=0; i<sessionFiles.size(); i++) {
        loadFromUrl(QUrl::fromLocalFile(QDir::current().absoluteFilePath(sessionFiles[i])));
    }
    if (sessionFiles.isEmpty()) {
        addCentralComponent(tr("Start"), m_plugin->m_startPage.widget, m_plugin->m_startPage.toolbarActions, m_plugin->m_startPage.menus, WWW, false);
    }
    ui->tabWidget->setCurrentIndex(tabIndex);
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
        loadFromUrl(QUrl::fromLocalFile(fileName));
    }
}

QStringList MainWindow::recentFiles(bool fullPaths) const
{
    QStringList r = m_plugin->mySettings()->value(Plugin::RecentFilesKey).toStringList();
    if (fullPaths)
        return r;
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
    r.removeAll(fileName);
    r.prepend(fileName);
    r = r.mid(0, qMax(5, r.size()));
    m_plugin->mySettings()->setValue(Plugin::RecentFilesKey, r);
}

void MainWindow::loadRecentFile(int index)
{
    QStringList r = recentFiles(true);
    if (index>=0 && index<r.size())
        loadFromUrl("file://"+r[index]);
}

void MainWindow::loadFromUrl(const QUrl & url)
{
    DocumentType type;
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
            addCentralComponent(fileName, vc, doc.toolbarActions, doc.menus, Kumir, true);
            ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
            ui->tabWidget->currentWidget()->setFocus();
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
            addCentralComponent(fileName, vc, doc.toolbarActions, doc.menus, Pascal, true);
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
            addCentralComponent(fileName, vc, doc.toolbarActions, doc.menus, Text, true);
            ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
            ui->tabWidget->currentWidget()->setFocus();
        }
    }
    else if (type==WWW) {
        BrowserComponent browser = m_plugin->plugin_browser->createBrowser(url, m_plugin->m_browserObjects);
        addCentralComponent(url.toString(), browser.widget, browser.toolbarActions, browser.menus, WWW, true);
        ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
        ui->tabWidget->currentWidget()->setFocus();
    }
}

} // namespace CoreGUI

#include "mainwindow.moc"
