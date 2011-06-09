#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "extensionsystem/visualcomponent.h"

namespace CoreGUI {

MainWindow::MainWindow(Plugin * p) :
    QMainWindow(0),
    ui(new Ui::MainWindow),
    m_plugin(p)
{
    ui->setupUi(this);
    ui->menuFile->setWindowTitle(ui->menuFile->title());
    ui->menuHelp->setWindowTitle(ui->menuHelp->title());

    connect(ui->actionNewProgram, SIGNAL(triggered()), this, SLOT(newProgram()));
    connect(ui->actionNewText, SIGNAL(triggered()), this, SLOT(newText()));

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
    if (QString(o->metaObject()->className()).toLower().contains("kumir")) {
        defaultText = QString::fromUtf8("алг\nнач\n\nкон");
        suffix = ".kum";
    }
    else if (QString(o->metaObject()->className()).toLower().contains("pascal")) {
        defaultText = "program;\n\nbegin\n\t\nend.";
        suffix = ".pas";
    }
    const QString initialText = m_plugin->mySettings()->value(Plugin::InitialTextKey, defaultText).toString();
    QPair<int, VisualComponent*> doc = m_plugin->plugin_editor->newDocument("Analizer", initialText);
    VisualComponent* vc = doc.second;
    int id = doc.first;
    vc->setProperty("documentId", id);
    QString fileName = suggestNewFileName(suffix);
    vc->setProperty("fileName", fileName);
    addCentralComponent(fileName, vc);
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
    addCentralComponent(fileName, vc);
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

void MainWindow::addCentralComponent(const QString &title, VisualComponent *c)
{
    ui->tabWidget->addTab(c, title);
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
        const QString title = c->menuActions()[i].menuText;
        bool found = false;
        for (int j=0; j<menus.size(); j++) {
            if (menus[j]->title().trimmed()==title.trimmed()) {
                found = true;
                break;
            }
        }
        if (!found) {
            QMenu * menu = new QMenu(title, this);
            menu->setWindowTitle(menu->title());
            menu->setTearOffEnabled(true);
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
    if (currentTabWidget)
        vc = qobject_cast<VisualComponent*>(currentTabWidget);
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
        loadFromFile(sessionFiles[i]);
    }
    if (sessionFiles.isEmpty()) {
        newProgram();
    }
    ui->tabWidget->setCurrentIndex(tabIndex);
}

void MainWindow::loadFromFile(const QString &)
{

}

} // namespace CoreGUI
