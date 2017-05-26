#include "defaultstartpage.h"
#include "plugin.h"
#include "mainwindow.h"
#include "ui_defaultstartpage.h"
#include <kumir2-libs/extensionsystem/pluginmanager.h>
#include <kumir2/coursesinterface.h>

#include <QtCore>
#if QT_VERSION >= 0x050000
#   include <QtWidgets>
#else
#   include <QtGui>
#endif


CoreGUI::DefaultStartPage::DefaultStartPage(Plugin* plugin, MainWindow* mw, QWidget *parent)
    : QWidget(parent)
    , _plugin(plugin)
    , _mainWindow(mw)
    , ui(new Ui::DefaultStartPage)
{
    this->setObjectName("startPageRoot");
    ui->setupUi(this);

    CoursesInterface *cs =
            ExtensionSystem::PluginManager::instance()->findPlugin<CoursesInterface>();
    _coursesAvailable = 0 != cs;

    ui->coursesBox->setVisible(_coursesAvailable);

    relayoutBlocks(this->width());

    ui->headerText->setText(_mainWindow->applicationTitle() + " " + _plugin->applicationVersionString());

    installEventFilter(this);
    createConnections();
    reloadStyleSheet();
}

QString CoreGUI::DefaultStartPage::startPageTabStyle() const
{
//    const QString cssFileName = _plugin->myResourcesDir().absoluteFilePath("default_startpage_tabstyle.css");
//    QFile cssFile(cssFileName);
//    if (cssFile.open(QIODevice::ReadOnly|QIODevice::Text)) {
//        const QString cssData = QString::fromUtf8(cssFile.readAll());
//        cssFile.close();
//        return cssData;
//    }
    // TODO uncomment after tab bar styled
    return "";
}

QString CoreGUI::DefaultStartPage::startPageTitle() const
{
    return windowTitle();
}


void CoreGUI::DefaultStartPage::createConnections()
{
    connect(ui->btnNewProgram, SIGNAL(clicked()),
            _mainWindow, SLOT(newProgram()));
    connect(ui->btnLoadProgram, SIGNAL(clicked()),
            _mainWindow, SLOT(fileOpen()));
    connect(ui->btnShowCourses, SIGNAL(clicked()),
            this, SLOT(handleShowCoursesPressed()));
    connect(ui->btnShowHelp, SIGNAL(clicked()),
            this, SLOT(handleShowHelpPressed()));
}

void CoreGUI::DefaultStartPage::reloadStyleSheet()
{
    const QString cssFileName = _plugin->myResourcesDir().absoluteFilePath("default_startpage.css");
    QFile cssFile(cssFileName);
    if (cssFile.open(QIODevice::ReadOnly|QIODevice::Text)) {
        const QString cssData = QString::fromUtf8(cssFile.readAll());
        cssFile.close();
        setStyleSheet(cssData);
    }
}

void CoreGUI::DefaultStartPage::relayoutBlocks(const int width)
{
    static const int OneCol = 400;
    static const int TwoCol = 780;

    int columns;
    int programsRow, programsColumn, coursesRow, coursesColumn, helpRow, helpColumn;

    if (_coursesAvailable) {
        columns = 3;
        if (width < OneCol)            columns = 1;
        else if (width < TwoCol)       columns = 2;
        if (3 == columns) {
            programsRow = coursesRow = helpRow = 0;
            programsColumn = 0;
            coursesColumn = 1;
            helpColumn = 2;
        }
        else if (2 == columns) {
            programsRow = coursesRow = 0;
            helpRow = 1;
            programsColumn = 0;
            coursesColumn = 1;
            helpColumn = 0;
        }
        else if (1 == columns) {
            programsColumn = coursesColumn = helpColumn = 0;
            programsRow = 0;
            coursesRow = 1;
            helpRow = 2;
        }
    }
    else {
        columns = width < OneCol ? 1 : 2;
        if (2 == columns) {
            programsRow = helpRow = 0;
            programsColumn = 0;
            helpColumn = 1;
        }
        else {
            programsRow = 0;
            helpRow = 1;
            programsColumn = helpColumn = 0;
        }
    }

    ui->centralViewLayout->removeWidget(ui->programsBox);
    ui->centralViewLayout->removeWidget(ui->coursesBox);
    ui->centralViewLayout->removeWidget(ui->headerBox);

    ui->centralViewLayout->addWidget(ui->programsBox, programsRow, programsColumn, 1, 1);
    if (_coursesAvailable) {
        ui->centralViewLayout->addWidget(ui->coursesBox, coursesRow, coursesColumn, 1, 1);
    }
    ui->centralViewLayout->addWidget(ui->helpBox, helpRow, helpColumn, 1, 1);

    updateContents();
}

void CoreGUI::DefaultStartPage::updateContents()
{
    updateRecentFiles();    
    if (_coursesAvailable) {
        updateRecentCourses();
    }
    updateHelp();
}

void CoreGUI::DefaultStartPage::updateRecentFiles()
{
    const QStringList names = _mainWindow->recentFiles(false);
    const QStringList paths = _mainWindow->recentFiles(true);

    Q_FOREACH(QObject * child, ui->programsList->children()) {
        QPushButton * btn = qobject_cast<QPushButton*>(child);
        if (btn) {
            ui->programsList->layout()->removeWidget(btn);
            btn->disconnect();
            btn->deleteLater();
        }
    }

    Q_ASSERT(names.size() == paths.size());
    for (int i=0; i<names.size(); ++i) {
        const QString & name = names[i];
        const QString & path = paths[i];
        QPushButton * item = new QPushButton;
        item->setCursor(Qt::PointingHandCursor);
        if (0 == i && 1 == names.size())
            item->setProperty("single", true);
        else if (0 == i)
            item->setProperty("first", true);
        else if (names.size()-1 == i)
            item->setProperty("last", true);
        QVBoxLayout * l = qobject_cast<QVBoxLayout*>(ui->programsList->layout());
        l->insertWidget(l->count()-1, item);
        item->setText(makeShortText(name, ui->programsList->width(), item->fontMetrics()));
        item->setToolTip(path);
        connect(item, SIGNAL(clicked()), this, SLOT(handleRecentFileClicked()));
    }

}

void CoreGUI::DefaultStartPage::updateRecentCourses()
{
    const QStringList names = _plugin->coursesList(false);
    const QStringList paths = _plugin->coursesList(true);

    Q_FOREACH(QObject * child, ui->coursesList->children()) {
        QPushButton * btn = qobject_cast<QPushButton*>(child);
        if (btn) {
            ui->coursesList->layout()->removeWidget(btn);
            btn->disconnect();
            btn->deleteLater();
        }
    }

    Q_ASSERT(names.size() == paths.size());
    for (int i=0; i<names.size(); ++i) {
        const QString & name = names[i];
        const QString & path = paths[i];
        QPushButton * item = new QPushButton;
        item->setCursor(Qt::PointingHandCursor);
        if (0 == i && 1 == names.size())
            item->setProperty("single", true);
        else if (0 == i)
            item->setProperty("first", true);
        else if (names.size()-1 == i)
            item->setProperty("last", true);
        QVBoxLayout * l = qobject_cast<QVBoxLayout*>(ui->coursesList->layout());
        l->insertWidget(l->count()-1, item);
        item->setText(makeShortText(name, ui->coursesList->width(), item->fontMetrics()));
        item->setToolTip(path);
        connect(item, SIGNAL(clicked()), this, SLOT(handleRecentCourseClicked()));
    }
}

void CoreGUI::DefaultStartPage::updateHelp()
{
    const QStringList names = _plugin->helpList();

    Q_FOREACH(QObject * child, ui->helpList->children()) {
        QPushButton * btn = qobject_cast<QPushButton*>(child);
        if (btn) {
            ui->coursesList->layout()->removeWidget(btn);
            btn->disconnect();
            btn->deleteLater();
        }
    }

    for (int i=0; i<names.size(); ++i) {
        const QString & name = names[i];
        QPushButton * item = new QPushButton;
        item->setCursor(Qt::PointingHandCursor);
        if (0 == i && 1 == names.size())
            item->setProperty("single", true);
        else if (0 == i)
            item->setProperty("first", true);
        else if (names.size()-1 == i)
            item->setProperty("last", true);
        QVBoxLayout * l = qobject_cast<QVBoxLayout*>(ui->helpList->layout());
        l->insertWidget(l->count()-1, item);
        item->setText(makeShortText(name, ui->coursesList->width(), item->fontMetrics()));
        item->setProperty("helpIndex", i);
        connect(item, SIGNAL(clicked()), this, SLOT(handleHelpItemClicked()));
    }
}

QString CoreGUI::DefaultStartPage::makeShortText(const QString &s, const int maxWidth, const QFontMetrics & fm)
{
    static const int Margins = 30;
    const int availableWidth = maxWidth - Margins;
    QString ss = s;
    int w = fm.width(ss);
    if (w <= availableWidth) {
        return ss;
    }
    while (ss.length() > 0) {
        ss.remove(ss.length()-1, 1);
        w = fm.width(ss + "...");
        if (w <= availableWidth) {
            break;
        }
    }
    return ss + "...";
}


bool CoreGUI::DefaultStartPage::eventFilter(QObject *obj, QEvent *evt)
{
    if (obj == this && QEvent::MouseButtonPress == evt->type()) {
        QMouseEvent * event = static_cast<QMouseEvent*>(evt);
        if (Qt::RightButton == event->button()) {
    #ifndef QT_NO_DEBUG
            reloadStyleSheet();
            qDebug() << "Reload style sheet";
    #endif
        }
    }
    else if (obj == this && QEvent::Resize == evt->type()) {
        QResizeEvent * event = static_cast<QResizeEvent*>(evt);
        relayoutBlocks(event->size().width());
    }
    else if (obj == this && QEvent::Show == evt->type()) {
        updateContents();
    }
    return false;
}

void CoreGUI::DefaultStartPage::handleShowCoursesPressed()
{
    _plugin->showCoursesWindow(QString());
}

void CoreGUI::DefaultStartPage::handleShowHelpPressed()
{
    _plugin->showHelpWindow(-1);
}

void CoreGUI::DefaultStartPage::handleRecentFileClicked()
{
    QPushButton * btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    const QString & path = btn->toolTip();
    _mainWindow->loadRecentFile(path);
}

void CoreGUI::DefaultStartPage::handleRecentCourseClicked()
{
    QPushButton * btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    const QString & path = btn->toolTip();
    _plugin->showCoursesWindow(path);
}

void CoreGUI::DefaultStartPage::handleHelpItemClicked()
{
    QPushButton * btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    const int index = btn->property("helpIndex").toInt();
    _plugin->showHelpWindow(index);
}


