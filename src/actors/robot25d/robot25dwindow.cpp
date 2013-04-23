#include "robot25dwindow.h"
#include "ui_robot25dwindow.h"
#include <QtScript>

Robot25DWindow::Robot25DWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Robot25DWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(new QGraphicsScene);
    const QString resPath = qApp->property("sharePath").toString()+"/actors/robot25d/";
//    QImage bgImg = QImage(resPath+"background.png");
//    QBrush bgBrush = QBrush(bgImg);
    QBrush bgBrush = QBrush(QColor(Qt::black));
    ui->graphicsView->setBackgroundBrush(bgBrush);
    ui->graphicsView->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);
    m_robotView = new Robot25D::RobotView(false, true, false, QSize(400,300));
    ui->graphicsView->scene()->addItem(m_robotView);

    ui->actionLoad->setIcon(QIcon::fromTheme("document-open", QIcon(resPath+"document-open.png")));
    ui->actionPrev->setIcon(QIcon::fromTheme("go-previous", QIcon(resPath+"go-previous.png")));
    ui->actionNext->setIcon(QIcon::fromTheme("go-next", QIcon(resPath+"go-next.png")));
    ui->actionReset->setIcon(QIcon::fromTheme("view-refresh", QIcon(resPath+"view-refresh.png")));
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->toolBar->setIconSize(QSize(22,22));
    loadGame(resPath+"/default.pm.json");
//    m_robotView->setAnimated(true);
    connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(handleLoadAction()));
    connect(ui->actionNext, SIGNAL(triggered()), this, SLOT(handleNextAction()));
    connect(ui->actionPrev, SIGNAL(triggered()), this, SLOT(handlePrevAction()));
    connect(ui->actionReset, SIGNAL(triggered()), m_robotView, SLOT(reset()));
    group_lockedActionsDuringEvaluate = new QActionGroup(this);
    group_lockedActionsDuringEvaluate->addAction(ui->actionLoad);
    group_lockedActionsDuringEvaluate->addAction(ui->actionNext);
    group_lockedActionsDuringEvaluate->addAction(ui->actionPrev);
    group_lockedActionsDuringEvaluate->addAction(ui->actionReset);
    ui->toolBar->setVisible(false);
    ui->menubar->setVisible(false);
    ui->statusbar->setVisible(false);
}

void Robot25DWindow::statusMessage(const QString &msg)
{
    statusBar()->showMessage(msg);
}

Robot25DWindow::~Robot25DWindow()
{
    delete ui;
}

void Robot25DWindow::loadGame(const QString &fileName)
{
    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString script = "a = ";
        script += QString::fromUtf8(f.readAll());
        f.close();
        Schema::Game g;
        QScriptEngine engine;
        QScriptValue v = engine.evaluate(script);
        if (Schema::parceJSON(v, g)) {
            m_game = g;
            setTaskIndex(g.index);
        }
        else {
            statusBar()->showMessage(QString::fromUtf8("Невозможно загрузить %1: это не ПиктоМир-игра").arg(QFileInfo(fileName).fileName()));
        }
    }
    else {
        statusBar()->showMessage(QString::fromUtf8("Невозможно загрузить %1: файл не читается").arg(QFileInfo(fileName).fileName()));
    }
}

void Robot25DWindow::loadEnvironment(const QString &fileName)
{
    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        const QString content = QString::fromUtf8(f.readAll());
        f.close();
        Schema::Game g;
        Schema::Task oneTask;
        if (Schema::parceKumirFil(content, oneTask.environment)) {
            g.tasks.append(oneTask);
            m_game = g;
            setTaskIndex(0);
        }
        else {
            statusBar()->showMessage(QString::fromUtf8("Невозможно загрузить %1: это не обстановка Вертуна").arg(QFileInfo(fileName).fileName()));
        }
    }
    else {
        statusBar()->showMessage(QString::fromUtf8("Невозможно загрузить %1: файл не читается").arg(QFileInfo(fileName).fileName()));
    }
}

void Robot25DWindow::handleLoadAction()
{
    QSettings s;
    QString lastDir = s.value("Robot25D/LastDir", QApplication::applicationDirPath()+"/Addons/robot25d/resources/default.pm.json").toString();
    const QString fileName = QFileDialog::getOpenFileName(this
                                                          , QString::fromUtf8("Загрузить обстановку")
                                                          , lastDir
                                                          , QString::fromUtf8("Все поддерживаемые файлы (*.pm.json *.fil);;(Игры ПиктоМир >= 0.12 (*.pm.json);;Отдельные обстановки (*.fil)")
                                                          );
    if (!fileName.isEmpty()) {
        s.setValue("Robot25D/LastDir", fileName);
        if (fileName.endsWith(".pm.json"))
            loadGame(fileName);
        else
            loadEnvironment(fileName);
    }
}

void Robot25DWindow::handleNextAction()
{
    setTaskIndex(m_game.index+1);
}

void Robot25DWindow::handlePrevAction()
{
    setTaskIndex(m_game.index-1);
}

void Robot25DWindow::setTaskIndex(int index)
{
    index = qMax(0, qMin(m_game.tasks.size()-1, index));
    m_robotView->loadEnvironment(m_game.tasks[index].environment);
    m_game.index = index;
    ui->actionPrev->setEnabled(index>0);
    ui->actionNext->setEnabled(index<m_game.tasks.size()-1);
    setWindowTitle(QString::fromUtf8("%1 - Вертун").arg(m_game.tasks[index].title));
}
