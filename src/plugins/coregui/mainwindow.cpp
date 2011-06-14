#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "extensionsystem/visualcomponent.h"


#include <algorithm>

namespace CoreGUI {

class TabWidgetElement
        : public QWidget
{
    Q_OBJECT
public:
    inline explicit TabWidgetElement(VisualComponent * c
                                     , bool enableToolBar
                                     , MainWindow::DocumentType type
                                     , QActionGroup * gr_fileActions
                                     , QActionGroup * gr_kumirActions
                                     , QActionGroup * gr_pascalActions
                                     , QActionGroup * gr_otherActions
                                     )
        : QWidget()
        , component(c)
    {
        setProperty("uncloseable", c->property("uncloseable"));
        QVBoxLayout * l = new QVBoxLayout;
        l->setContentsMargins(0,0,0,0);
        l->setSpacing(0);
        setLayout(l);
        if (enableToolBar) {
            QToolBar * tb = new QToolBar(this);
            l->addWidget(tb);
            if (type!=MainWindow::WWW) {
                tb->addActions(gr_fileActions->actions());
            }
            tb->addSeparator();
            foreach (QAction * a, c->toolbarActions())
                tb->addAction(a);
            if (type==MainWindow::Kumir) {
                tb->addSeparator();
                tb->addActions(gr_kumirActions->actions());
            }
            if (type==MainWindow::Pascal) {
                tb->addSeparator();
                tb->addActions(gr_pascalActions->actions());
            }
            if (!gr_otherActions->actions().isEmpty()) {
                tb->addSeparator();
                tb->addActions(gr_otherActions->actions());
            }

        }
        l->addWidget(c);
    }
    VisualComponent * component;
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

    gr_fileActions = new QActionGroup(this);
    gr_fileActions->addAction(ui->actionSave);

    gr_kumirActions = new QActionGroup(this);
    gr_pascalActions = new QActionGroup(this);

    gr_otherActions = new QActionGroup(this);

}

void MainWindow::closeCurrentTab()
{
    closeTab(ui->tabWidget->currentIndex());
}

void MainWindow::closeTab(int index)
{
    QWidget * w = ui->tabWidget->widget(index);
    ui->tabWidget->removeTab(index);
    w->deleteLater();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    QRect r(pos(), size());
    m_plugin->mySettings()->setValue(Plugin::MainWindowGeometryKey, r);
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
    QPair<int, VisualComponent*> doc = m_plugin->plugin_editor->newDocument("Analizer", initialText);
    VisualComponent* vc = doc.second;
    int id = doc.first;
    vc->setProperty("documentId", id);
    QString fileName = suggestNewFileName(suffix);
    vc->setProperty("fileName", fileName);
    addCentralComponent(fileName, vc, type, true);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
    ui->tabWidget->currentWidget()->setFocus();
}

void MainWindow::newText()
{
    QPair<int, VisualComponent*> doc = m_plugin->plugin_editor->newDocument("", "");
    VisualComponent* vc = doc.second;
    int id = doc.first;
    vc->setProperty("documentId", id);
    QString fileName = suggestNewFileName(".txt");
    vc->setProperty("fileName", fileName);
    addCentralComponent(fileName, vc, Text, true);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
    ui->tabWidget->currentWidget()->setFocus();
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


void MainWindow::addCentralComponent(const QString &title, VisualComponent *c, DocumentType type, bool enableToolBar)
{
    TabWidgetElement * element = new TabWidgetElement(c,enableToolBar,type,gr_fileActions,gr_kumirActions,gr_pascalActions,gr_otherActions);
    ui->tabWidget->addTab(element, title);
    createTopLevelMenus(c, true);
}



void MainWindow::createTopLevelMenus(VisualComponent *c, bool tabDependent)
{
    QList<QMenu*> menus;
    for (int i=0; i<menuBar()->children().size(); i++) {
        QMenu * m = qobject_cast<QMenu*>(menuBar()->children()[i]);
        if (m)
            menus << m;
    }
    for (int i=0; i<c->menuActions().size(); i++) {
        const QString title = c->menuActions()[i].menuText.trimmed();
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
            if (tabDependent)
                connect(menu, SIGNAL(aboutToShow()), this, SLOT(handleMenuAccess()));
            else {
                for (int k=0; k<c->menuActions()[i].actions.size(); k++) {
                    QAction * act = c->menuActions()[i].actions[k];
                    menu->addAction(act);
                }
            }
            menuBar()->insertMenu(menus.last()->menuAction(), menu);
        }
    }
}

void MainWindow::handleMenuAccess()
{
    QMenu * menu = qobject_cast<QMenu*>(sender());
    menu->clear();
    QWidget * currentTabWidget = ui->tabWidget->currentWidget();
    VisualComponent * vc = 0;
    TabWidgetElement * twe = qobject_cast<TabWidgetElement*>(currentTabWidget);
    if (!twe)
        return;
    vc = twe->component;
    if (vc) {
        MenuActionsGroup group;
        for (int i=0; i<vc->menuActions().size(); i++) {
            if (menu->title()==vc->menuActions()[i].menuText) {
                group = vc->menuActions()[i];
                break;
            }
        }
        for (int i=0; i<group.actions.size(); i++) {
            menu->addAction(group.actions[i]);
        }
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
        addCentralComponent(tr("Start"), m_plugin->m_startPage, WWW, false);
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
            QPair<int, VisualComponent*> doc = m_plugin->plugin_editor->newDocument("KumirAnalizer", lines.join("\n"));
            VisualComponent* vc = doc.second;
            int id = doc.first;
            vc->setProperty("documentId", id);
            QString fileName = QFileInfo(url.toLocalFile()).fileName();
            vc->setProperty("fileName", url.toLocalFile());
            addCentralComponent(fileName, vc, Kumir, true);
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
            QPair<int, VisualComponent*> doc = m_plugin->plugin_editor->newDocument("PascalAnalizer", data);
            VisualComponent* vc = doc.second;
            int id = doc.first;
            vc->setProperty("documentId", id);
            QString fileName = QFileInfo(url.toLocalFile()).fileName();
            vc->setProperty("fileName", url.toLocalFile());
            addCentralComponent(fileName, vc, Pascal, true);
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
            QPair<int, VisualComponent*> doc = m_plugin->plugin_editor->newDocument("", data);
            VisualComponent* vc = doc.second;
            int id = doc.first;
            vc->setProperty("documentId", id);
            QString fileName = QFileInfo(url.toLocalFile()).fileName();
            vc->setProperty("fileName", url.toLocalFile());
            addCentralComponent(fileName, vc, Text, true);
            ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
            ui->tabWidget->currentWidget()->setFocus();
        }
    }
    else if (type==WWW) {
        // TODO implement me
    }
}

} // namespace CoreGUI

#include "mainwindow.moc"
