#include "editorstandalonewindow.h"
#include "editorplugin.h"
#include "extensionsystem/pluginmanager.h"

namespace Editor {

QString EditorStandaloneWindow::RecentPathSettingsKey = "Paths/RecentFile";
QString EditorStandaloneWindow::WindowRectSettingsKey = "Geometry/StandaloneWindow";

EditorStandaloneWindow::EditorStandaloneWindow(class EditorPlugin * plugin
                                               , QSettings * settings
                                               , const QString &startTextFileName
                                               )
    : QMainWindow(0)
    , m_settings(settings)
{
    loadSettings();

    QMenu * file = menuBar()->addMenu(tr("File"));

    QAction * newP = new QAction(this);
    newP->setShortcut(QKeySequence(QKeySequence::New));
    newP->setIcon(QIcon::fromTheme("document-new"));
    newP->setText(tr("New Kumir program"));
    connect(newP, SIGNAL(triggered()), this, SLOT(newProgram()));
    file->addAction(newP);

    file->addSeparator();

    QAction * load = new QAction(this);
    load->setShortcut(QKeySequence(QKeySequence::Open));
    load->setIcon(QIcon::fromTheme("document-open"));
    load->setText(tr("Load..."));
    connect(load, SIGNAL(triggered()), this, SLOT(openDocument()));
    file->addAction(load);

    QAction * save = new QAction(this);
    save->setShortcut(QKeySequence(QKeySequence::Save));
    save->setIcon(QIcon::fromTheme("document-save"));
    save->setText(tr("Save"));
    connect(save, SIGNAL(triggered()), this, SLOT(saveDocument()));
    file->addAction(save);

    QAction * saveAs = new QAction(this);
    saveAs->setShortcut(QKeySequence(QKeySequence::SaveAs));
    saveAs->setIcon(QIcon::fromTheme("document-save-as"));
    saveAs->setText(tr("Save as..."));
    connect(saveAs, SIGNAL(triggered()), this, SLOT(saveDocumentAs()));
    file->addAction(saveAs);

    file->addSeparator();

    QAction * prefs = new QAction(this);
    prefs->setShortcut(QKeySequence(QKeySequence::Preferences));
    prefs->setMenuRole(QAction::PreferencesRole);
    prefs->setText(tr("Preferences..."));
    connect(prefs, SIGNAL(triggered()), this, SLOT(showSettings()));
    file->addAction(prefs);

    file->addSeparator();

    QAction * ex = new QAction(this);
    ex->setShortcut(QKeySequence(QKeySequence::Quit));
    ex->setText(tr("Exit"));
    ex->setMenuRole(QAction::QuitRole);
    connect(ex, SIGNAL(triggered()), qApp, SLOT(quit()));
    file->addAction(ex);


    i_analizerId = -1;
    m_editor = 0;
    m_plugin = plugin;

    if (!startTextFileName.isEmpty() && loadFromFile(startTextFileName)) {
    }
    else {
        newProgram();
    }
}

void EditorStandaloneWindow::closeEvent(QCloseEvent *e)
{
    saveSettings();
    e->accept();
}

void EditorStandaloneWindow::showSettings()
{
    ExtensionSystem::PluginManager::instance()->showSettingsDialog();
}


void EditorStandaloneWindow::loadSettings()
{
    QRect r = m_settings->value(WindowRectSettingsKey, QRect(0,0,800,600)).toRect();
    resize(r.size());
    move(r.topLeft());
}

void EditorStandaloneWindow::saveSettings()
{
    QRect r(pos(), size());
    m_settings->setValue(WindowRectSettingsKey, r);
}

void EditorStandaloneWindow::newProgram()
{
    if (i_analizerId!=-1)
        m_plugin->closeDocument(i_analizerId);
    i_analizerId = -1;
    m_editor = 0;
    QPair<int,Shared::VisualComponent*> p = m_plugin->newDocument("Analizer","");
    i_analizerId = p.first;
    m_editor = p.second;
    setupEditor();
}

bool EditorStandaloneWindow::loadFromFile(const QString &fileName)
{
    QFile f (fileName);
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream ts(&f);
        ts.setCodec("UTF-16");
        ts.setAutoDetectUnicode(true);
        QString data = ts.readAll();
        f.close();
        s_fileName = fileName;
        if (i_analizerId!=-1) {
            m_plugin->closeDocument(i_analizerId);
        }
        i_analizerId = -1;
        m_editor = 0;
        QPair<int,Shared::VisualComponent*> p = m_plugin->newDocument("Analizer", data);
        i_analizerId = p.first;
        m_editor = p.second;
        setupEditor();
        return true;
    }
    else {
        return false;
    }
}

QString EditorStandaloneWindow::saveToFile(const QString &fileName)
{
    return m_plugin->saveDocument(i_analizerId, fileName);
}

void EditorStandaloneWindow::openDocument()
{
    QString recentPath = m_settings->value(RecentPathSettingsKey, QDir::currentPath()).toString();
    QString fileName = QFileDialog::getOpenFileName( this
                                                    ,tr("Load Kumir program...")
                                                    ,recentPath
                                                    ,tr("Kumir programs (*.kum)")
                                                    );
    if (!fileName.isEmpty()) {
        m_settings->setValue(RecentPathSettingsKey, fileName);
        if (loadFromFile(fileName)) {
        }
        else {
            QMessageBox::warning(this, tr("Error"), tr("Can't load file %1").arg(fileName));
        }
    }
}

void EditorStandaloneWindow::saveDocument()
{
    if (s_fileName.isEmpty())
        saveDocumentAs();
    else {
        QString error = saveToFile(s_fileName);
        if (!error.isEmpty()) {
            QMessageBox::warning(this, tr("Error"), error);
        }
    }
}

void EditorStandaloneWindow::saveDocumentAs()
{
    QString recentPath = m_settings->value(RecentPathSettingsKey, QDir::currentPath()).toString();
    QString fileName = QFileDialog::getSaveFileName(this
                                                    , tr("Save Kumir program...")
                                                    , recentPath
                                                    , tr("Kumir programs (*.kum)")
                                                    );
    if (!fileName.isEmpty()) {
        m_settings->setValue(RecentPathSettingsKey, fileName);
        QString error = saveToFile(s_fileName);
        if (!error.isEmpty()) {
            QMessageBox::warning(this, tr("Error"), error);
        }
        else {
            s_fileName = fileName;
        }
    }
}

void EditorStandaloneWindow::setupEditor()
{
    QList<Shared::MenuActionsGroup> menus = m_editor->menuActions();
    foreach (const Shared::MenuActionsGroup & m, menus) {
        QMenu * menu = 0;
        for (int i=0; i<menuBar()->children().size(); i++) {
            QMenu * mm = qobject_cast<QMenu*>(menuBar()->children().at(i));
            if (mm && mm->title()==m.menuText) {
                menu = mm;
                break;
            }
        }
        if (!menu)
            menu = menuBar()->addMenu(m.menuText);
        menu->addActions(m.actions);
    }
    setCentralWidget(m_editor);
    m_editor->setFocus(Qt::ActiveWindowFocusReason);
}

} // namespace Editor
