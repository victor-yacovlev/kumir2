/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#include <QtCore>
#include <QtGui>
#include <iostream>
#include "isometricrobotmodule.h"
#include "sch_environment.h"
#include "sch_game.h"
#include "remotecontrol.h"

namespace ActorIsometricRobot {


IsometricRobotModule::IsometricRobotModule(ExtensionSystem::KPlugin * parent)
    : IsometricRobotModuleBase(parent)
    , _window(0)
    , _robotView(0)
    , _parentObject(parent)
    , _remoteControl(0)
    , _remoteControlWidget(0)
{   
}

void IsometricRobotModule::createGui()
{
    _window = new Robot25DWindow(_model, _parentObject->myResourcesDir(), 0);
    _remoteControlWidget = new QWidget(0);
    QVBoxLayout*  rcLayout = new QVBoxLayout;
    _remoteControlWidget->setLayout(rcLayout);
    const QString rcFileName = _parentObject->myResourcesDir().absoluteFilePath("robot25d-rc.svg");
    _remoteControl = new SvgRemoteControl(_parentObject, this, rcFileName, _remoteControlWidget);
    rcLayout->addWidget(_remoteControl);

    _robotView = _window->robotView();
    connect(m_actionRobot25DLoadEnvironment, SIGNAL(triggered()),
            _window, SLOT(handleLoadAction()));
    connect(m_actionRobot25DResetEnvironment, SIGNAL(triggered()),
            this, SLOT(reset()));

}

QString IsometricRobotModule::loadEnvironmentFromFile(const QString &fileName)
{
    QString message;
    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString script = "a = ";
        script += QString::fromUtf8(f.readAll());
        f.close();
        Schema::Environment env;
        QScriptEngine engine;
        QScriptValue v = engine.evaluate(script);
        if (Schema::parceJSON(v, env)) {
            _model->loadEnvironment(env);
        }
        else {
            message = QString::fromUtf8("Невозможно загрузить %1: это не обстановка Вертуна").arg(QFileInfo(fileName).fileName());
        }
    }
    else {
        message = QString::fromUtf8("Невозможно загрузить %1: файл не читается").arg(QFileInfo(fileName).fileName());
    }
    return message;
}

void IsometricRobotModule::loadDefaultEnvironment()
{
    static const QString DefaultGameFileName = myResourcesDir().absoluteFilePath("default.pm.json");
    QFile f(DefaultGameFileName);
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString script = "a = ";
        script += QString::fromUtf8(f.readAll());
        f.close();
        Schema::Game g;
        QScriptEngine engine;
        QScriptValue v = engine.evaluate(script);
        if (Schema::parceJSON(v, g)) {
            Schema::Environment env = g.tasks.first().environment;
            _model->loadEnvironment(env);
        }
    }
}



QString IsometricRobotModule::initialize(const QStringList &configurationParameters, const ExtensionSystem::CommandLine &cmdLine)
{
    if (!configurationParameters.contains("tablesOnly")) {
        _model = new Robot25D::RobotModel;

        bool hasGui=true;
#ifdef Q_OS_LINUX
        hasGui = 0 != getenv("DISPLAY");
#endif
        if (hasGui) {
            createGui();
        }
        else {
            const QString envFileName = cmdLine.value('e').toString();
            if (envFileName.isEmpty()) {
                loadDefaultEnvironment();
            }
            else {
                loadEnvironmentFromFile(envFileName);
            }
        }
    }
    return "";
}

QList<ExtensionSystem::CommandLineParameter> IsometricRobotModule::acceptableCommandLineParameters()
{
    return QList<ExtensionSystem::CommandLineParameter>();
}

void IsometricRobotModule::reset()
{
    _model->reset();
    if (_robotView) {
        bool wasAnimated = _robotView->isAnimated();
        _robotView->reset();
        _robotView->setAnimated(wasAnimated);
    }
}

void IsometricRobotModule::loadActorData(QIODevice *source)
{
    QTextStream ts(source);
    ts.setCodec("UTF-8");
    ts.setAutoDetectUnicode(true);
    const QString data = ts.readAll();
    Schema::Environment env;
    Schema::parceJSON(data, env);
    _model->loadEnvironment(env);
}

void IsometricRobotModule::setAnimationEnabled(bool enabled)
{
    if (_robotView) {
        _robotView->setAnimated(enabled);
    }
}


QWidget* IsometricRobotModule::mainWidget() const
{
    return _window;
}

void IsometricRobotModule::runGoForward()
{
    setError("");
    if (!_model->goForward()) {
        setError(_model->lastError(QLocale::Russian));
    }
    if (_robotView) {
        _robotView->waitForAnimated();
    }
}


void IsometricRobotModule::runTurnRight()
{
    setError("");
    _model->turnRight();
    if (_robotView) {
        _robotView->waitForAnimated();
    }
}


void IsometricRobotModule::runTurnLeft()
{
    setError("");
    _model->turnLeft();
    if (_robotView) {
        _robotView->waitForAnimated();
    }
}


void IsometricRobotModule::runDoPaint()
{
    setError("");
    _model->doPaint();
    if (_robotView) {
        _robotView->waitForAnimated();
    }
}


bool IsometricRobotModule::runIsCellPainted()
{
    setError("");
    return _model->isPainted();
}

bool IsometricRobotModule::runIsFlagged(int x, int y)
{
    setError("");
    return _model->isFlagged(x - 1, y - 1);
}


bool IsometricRobotModule::runIsCellClean()
{
    setError("");
    return ! _model->isPainted();
}


bool IsometricRobotModule::runIsWallAhead()
{
    setError("");
    return _model->isWall();
}


bool IsometricRobotModule::runIsFreeAhead()
{
    setError("");
    return ! _model->isWall();
}

bool IsometricRobotModule::runIsPainted(const int x, const int y)
{
    setError("");
    return _model->isPainted(x - 1, y - 1);
}

bool IsometricRobotModule::runIsMarked(const int x, const int y)
{
    setError("");
    return _model->isPointed(x - 1, y - 1);
}

int IsometricRobotModule::runPositionX()
{
    setError("");
    return _model->positionX() + 1;
}

int IsometricRobotModule::runPositionY()
{
    setError("");
    return _model->positionY() + 1;
}

int IsometricRobotModule::runSizeX()
{
    setError("");
    return _model->sizeX();
}

int IsometricRobotModule::runSizeY()
{
    setError("");
    return _model->sizeY();
}

    
} // $namespace
