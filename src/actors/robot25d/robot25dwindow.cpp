#include "robot25dwindow.h"

#include <QtScript>

Robot25DWindow::Robot25DWindow(QWidget *parent) :
    QGraphicsView(parent)
{
    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);
    mousePressPosition_ = QPoint(-1, -1);
    setScene(new QGraphicsScene);
    const QString resPath = qApp->property("sharePath").toString()+"/actors/robot25d/";
    QImage bgImg = QImage(resPath+"grass_0.png");
    QBrush bgBrush = QBrush(bgImg);
//    QBrush bgBrush = QBrush(QColor(Qt::black));
    setBackgroundBrush(bgBrush);
    setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_robotView = new Robot25D::RobotView(false, true, false, QSize(400,300));
    scene()->addItem(m_robotView);

    loadGame(resPath+"/default.pm.json");
//    m_robotView->setAnimated(true);    
}


QSize Robot25DWindow::sizeHint() const
{
    return QGraphicsView::sizeHint() + QSize(30, 40);
}

void Robot25DWindow::mousePressEvent(QMouseEvent *event)
{
    const QScrollBar * vert = verticalScrollBar();
    const QScrollBar * horiz = horizontalScrollBar();
    bool scrollable = vert->maximum() + horiz->maximum() > 0;
    if (scrollable) {
        if (event->button() == Qt::LeftButton) {
            setCursor(Qt::ClosedHandCursor);
            mousePressPosition_ = event->pos();
        }
        else {
            setCursor(Qt::OpenHandCursor);
        }
        event->accept();
    }
    else {
        setCursor(Qt::ArrowCursor);
        event->ignore();
    }
}

void Robot25DWindow::mouseReleaseEvent(QMouseEvent *event)
{
    const QScrollBar * vert = verticalScrollBar();
    const QScrollBar * horiz = horizontalScrollBar();
    bool scrollable = vert->maximum() + horiz->maximum() > 0;
    if (scrollable) {
        mousePressPosition_ = QPoint(-1, -1);
        setCursor(Qt::OpenHandCursor);
        event->accept();
    }
    else {
        setCursor(Qt::ArrowCursor);
        event->ignore();
    }
}

void Robot25DWindow::mouseMoveEvent(QMouseEvent *event)
{
    QScrollBar * vert = verticalScrollBar();
    QScrollBar * horiz = horizontalScrollBar();
    bool scrollable = vert->maximum() + horiz->maximum() > 0;
    if (scrollable) {
        if (mousePressPosition_ != QPoint(-1, -1)) {
            setCursor(Qt::ClosedHandCursor);
            const QPoint diff = event->pos() - mousePressPosition_;
            const int dx = diff.x();
            const int dy = diff.y();
            if (dx != 0) {
                horiz->setValue(horiz->value() - dx);
            }
            if (dy != 0) {
                vert->setValue(vert->value() - dy);
            }
            mousePressPosition_ = event->pos();
        }
        event->accept();
    }
    else {
        setCursor(Qt::ArrowCursor);
        event->ignore();
    }
}

void Robot25DWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    QScrollBar * vert = verticalScrollBar();
    QScrollBar * horiz = horizontalScrollBar();
    bool scrollable = vert->maximum() + horiz->maximum() > 0;
    if (scrollable) {
        emit resizeRequest(sizeHint());
        event->accept();
    }
    else {
        event->ignore();
    }
}

void Robot25DWindow::wheelEvent(QWheelEvent *event)
{
    event->ignore();
}

Robot25DWindow::~Robot25DWindow()
{

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

//            statusBar()->showMessage(QString::fromUtf8("Невозможно загрузить %1: это не ПиктоМир-игра").arg(QFileInfo(fileName).fileName()));
        }
    }
    else {
//        statusBar()->showMessage(QString::fromUtf8("Невозможно загрузить %1: файл не читается").arg(QFileInfo(fileName).fileName()));
    }
}

//QSize Robot25DWindow::minimumSizeHint() const
//{
//    return QSize(100, 100);
//}

//QSize Robot25DWindow::sizeHint() const
//{
//    return sizeHint() + QSize(30, 30);
//}

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
//            statusBar()->showMessage(QString::fromUtf8("Невозможно загрузить %1: это не обстановка Вертуна").arg(QFileInfo(fileName).fileName()));
        }
    }
    else {
//        statusBar()->showMessage(QString::fromUtf8("Невозможно загрузить %1: файл не читается").arg(QFileInfo(fileName).fileName()));
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

    setWindowTitle(QString::fromUtf8("%1 - Вертун").arg(m_game.tasks[index].title));
}
