/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents.
You should change it corresponding to functionality.
*/

#include "rfield.h"
#include "cfield.h"
#include "srobot.h"
#include "robotmodule.h"
#include "robotview.h"
#include "pult.h"
#include "kumir2-libs/widgets/pultlogger.h"

static const int AnimTime = 70;
#define MAX_COLUMNS 255
#define MAX_ROWS 128

namespace ActorRobot
{
const RobotModule *RobotModule::self = 0;

RobotModule::RobotModule(ExtensionSystem::KPlugin *parent) :
	RobotModuleBase(parent)
{
	self = this;
	inDock = false;
	animation = true;
	pressed = false;
	m_mainWidget = 0;
	m_pultWidget = 0;
}


void RobotModule::createGui()
{
	field = new RoboField(0, this);
	field->createField(7, 7);

	field->setRoboPos(0, 0);
	field->createRobot();
	view = new RobotView(field);
	view-> setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
	m_mainWidget = view;
	m_pultWidget = new RoboPult();

	connect(m_pultWidget, SIGNAL(goUp()), this, SLOT(runGoUp()));
	connect(m_pultWidget, SIGNAL(goDown()), this, SLOT(runGoDown()));
	connect(m_pultWidget, SIGNAL(goLeft()), this, SLOT(runGoLeft()));
	connect(m_pultWidget, SIGNAL(goRight()), this, SLOT(runGoRight()));
	connect(m_pultWidget, SIGNAL(doPaint()), this, SLOT(runDoPaint()));
	connect(m_pultWidget, SIGNAL(hasLeftWall()), this, SLOT(runIsWallAtLeft()));
	connect(m_pultWidget, SIGNAL(hasRightWall()), this, SLOT(runIsWallAtRight()));
	connect(m_pultWidget, SIGNAL(hasUpWall()), this, SLOT(runIsWallAtTop()));
	connect(m_pultWidget, SIGNAL(hasDownWall()), this, SLOT(runIsWallAtBottom()));
	connect(m_pultWidget, SIGNAL(noLeftWall()), this, SLOT(runIsFreeAtLeft()));
	connect(m_pultWidget, SIGNAL(noRightWall()), this, SLOT(runIsFreeAtRight()));
	connect(m_pultWidget, SIGNAL(noUpWall()), this, SLOT(runIsFreeAtTop()));
	connect(m_pultWidget, SIGNAL(noDownWall()), this, SLOT(runIsFreeAtBottom()));
	connect(m_pultWidget, SIGNAL(Rad()), this, SLOT(runRadiation()));
	connect(m_pultWidget, SIGNAL(Temp()), this, SLOT(runTemperature()));
	connect(m_pultWidget, SIGNAL(Colored()), this, SLOT(runIsColor()));
	connect(m_pultWidget, SIGNAL(Clean()), this, SLOT(runIsClear()));
	connect(m_pultWidget, SIGNAL(Color()), this, SLOT(runDoPaint()));
	connect(m_pultWidget, SIGNAL(robReset()), this, SLOT(reset()));


	connect(m_pultWidget, SIGNAL(copyTextToKumir(QString)), this, SLOT(copyFromPult(QString)));
	connect(this, SIGNAL(sendToPultLog(QVariant)), m_pultWidget, SLOT(addToResultLog(QVariant)));
	startField = field->Clone();
	field->drawField(FIELD_SIZE_SMALL);
	field->dropWasEdit();
	m_actionRobotEditEnvironment->setCheckable(true);
	connect(m_actionRobotLoadEnvironment, SIGNAL(triggered()), this, SLOT(loadEnv()));
	connect(m_actionRobotRevertEnvironment, SIGNAL(triggered()), this, SLOT(resetEnv()));
	connect(m_actionRobotSaveEnvironment, SIGNAL(triggered()), this, SLOT(saveEnv()));
	connect(m_actionRobotEditEnvironment, SIGNAL(triggered()), this, SLOT(editEnv()));
	connect(m_actionRobotNewEnvironment, SIGNAL(triggered()), this, SLOT(newEnv()));
	connect(m_actionRobotAutoWindowSize, SIGNAL(triggered()), this, SLOT(setWindowSize()));
	connect(m_actionRobotSave2Png, SIGNAL(triggered()), this, SLOT(save2png()));

	prepareNewWindow();
	rescentMenu = new QMenu();
	m_actionRobotLoadRescent->setMenu(rescentMenu);
	view->setWindowTitle(trUtf8("Робот - нет файла"));
}

void RobotModule::copyFromPult(QString log)
{
	QClipboard *clipboard = QApplication::clipboard();
	QString originalText = clipboard->text();

	clipboard->setText(log);
};

/* public slot */ void RobotModule::loadActorData(QIODevice *source)
{
	// Set actor specific data (like environment)
	// The source should be ready-to-read QIODevice like QBuffer or QFile
	if (!DISPLAY) { //console mode
		curConsoleField = new ConsoleField(10, 15);
		if (curConsoleField->loadFromDataStream(source) != 0) {
			qDebug() << "ERROR LOADING FIELD FROM STREAM ";
		}
		return;
	}
	qDebug() << "Load env";
	if (field->loadFromDataStream(source) != 0) {
		return ;
	}
	m_pultWidget->Logger->ClearLog();
	m_mainWidget->setWindowTitle(trUtf8("Робот - ") + source->objectName());
	startField = field->Clone();
	field->dropWasEdit();

	reset();
}


void RobotModule::reset()
{
	/* TODO
	This method is called when actor resets its state before program starts.
	*/
	//delete field;
	qDebug() << "Reset!!";
	if (!DISPLAY) { //console mode
		qDebug() << "Reset::console mode";
		return;
	}

	field->destroyRobot();
	field->deleteLater();
	if (field->isEditMode()) {
		field->setMode(NORMAL_MODE);
		view->showButtons(false);
		view->repaint();
		m_actionRobotEditEnvironment->setChecked(false);
		startField = field->Clone();
	}
	field = startField->Clone();
	field->setRoboPos(startField->robotX(), startField->robotY());
	field->createRobot();
	field->drawField(FIELD_SIZE_SMALL);
	view->setScene(field);
	field->drawField(FIELD_SIZE_SMALL);
	view->setField(field);
}

void RobotModule::changeGlobalState(
	ExtensionSystem::GlobalState old,
	ExtensionSystem::GlobalState current
) {
	Q_UNUSED(old);
	using namespace Shared;
	qDebug() << "RobotModuleBase::changeGlobalState";
	view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	if (current == PluginInterface::GS_Running) {
		view->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
		m_actionRobotRevertEnvironment->setEnabled(false);
		m_actionRobotLoadEnvironment->setEnabled(false);
		m_actionRobotLoadRescent->setEnabled(false);
		m_actionRobotRevertEnvironment->setEnabled(false);
		m_actionRobotSaveEnvironment->setEnabled(false);
		m_actionRobotEditEnvironment->setEnabled(false);
		m_actionRobotNewEnvironment->setEnabled(false);
	}
	if (current == PluginInterface::GS_Unlocked || current == PluginInterface::GS_Observe) {
		m_actionRobotRevertEnvironment->setEnabled(true);
		m_actionRobotLoadEnvironment->setEnabled(true);
		m_actionRobotLoadRescent->setEnabled(true);
		m_actionRobotRevertEnvironment->setEnabled(true);
		m_actionRobotSaveEnvironment->setEnabled(true);
		m_actionRobotEditEnvironment->setEnabled(true);
		m_actionRobotNewEnvironment->setEnabled(true);
		view->FindRobot();
	}
	field->destroyNet();
	field->drawNet();
	view->update();
	field->update();
	msleep(2 * AnimTime);
	view->update();
	field->update();
	qApp->processEvents();//Redraw event fix
};

void RobotModule::reloadSettings(
	ExtensionSystem::SettingsPtr settings,
	const QStringList &keys
) {
	Q_UNUSED(keys);
	qDebug() << "reload settings";
	field->reloadSettings();
	CurCellSize = settings->value("Robot/CellSize", FIELD_SIZE_SMALL).toInt();
	view->reloadSett(settings);
	if (RobotModule::robotSettings()->value("Robot/SFF").isValid()) {
		if (LoadFromFile(RobotModule::robotSettings()->value("Robot/SFF").toString()) != 0) {
			createEmptyField(7, 7);

		}
		setWindowSize();
	}
	createRescentMenu();
}

void RobotModule::setAnimationEnabled(bool enabled)
{
	animation = enabled;
}


QWidget *RobotModule::mainWidget() const
{
	return m_mainWidget;
}

class AAA : public QWidget
{
public:
	explicit AAA(QWidget *pult): QWidget(), pult_(pult)
	{
		setLayout(new QVBoxLayout);
		layout()->setContentsMargins(0, 0, 0, 0);
		layout()->addWidget(pult);
	}

	QSize minimumSizeHint() const
	{
		return pult_->minimumSizeHint();
	}
private:
	QWidget *pult_;
};


QWidget *RobotModule::pultWidget() const
{
	static QWidget *dummy = new AAA(m_pultWidget);
	return dummy;
	return m_pultWidget;
}

QList<ExtensionSystem::CommandLineParameter>
RobotModule::acceptableCommandLineParameters()
{
	QList<ExtensionSystem::CommandLineParameter> params;
	params.append(
		ExtensionSystem::CommandLineParameter(
			true, 'f', "field", tr("Robot field file name"),
			QVariant::String, false
		)
	);
	return params;
}

QString RobotModule::initialize(
	const QStringList &configurationParameters,
	const ExtensionSystem::CommandLine &runtimeParameters
) {
	QString fName = "";
	if (runtimeParameters.value('f').isValid()) {
		fName = runtimeParameters.value('f').toString();
		qDebug() << "FIELD: |" << fName << "| ";
	}

	DISPLAY = true;

#ifdef Q_OS_LINUX
	QProcessEnvironment pe = QProcessEnvironment::systemEnvironment();
	qDebug() << "Display" << pe.value("DISPLAY");
	if (pe.keys().indexOf("DISPLAY") < 0 || pe.value("DISPLAY").isEmpty()) {
		qDebug() << "Robot:Console mode";
		curConsoleField = new ConsoleField(10, 15);
		DISPLAY = false;
		if (!fName.isEmpty()) {
			qDebug() << "LOAD FIELD ERR CODE:" << curConsoleField->loadFromFile(fName);
		}
		return "";
	}

	qDebug() << "Robot:GuiMode";
#endif

	if (!configurationParameters.contains("tablesOnly")) {
		createGui();
		redrawTimer = new QTimer();
		connect(redrawTimer, SIGNAL(timeout()), this, SLOT(getTimer()));
		redrawTimer->start(30);
	}

	if (!fName.isEmpty()) {
		int err = LoadFromFile(fName);
		if (err != 0) {
			return "Error loading: '" + fName + "', code=" + err;
		}
	} else if (RobotModule::robotSettings()->value("Robot/SFF").isValid()) {
		QString sName = RobotModule::robotSettings()->
			value("Robot/SFF").toString();
		int err = LoadFromFile(sName);
		if (err != 0) {
			qDebug() << "Error loading: '" + sName + "', code=" + err;
			createEmptyField(7, 7);
		}
	}

	ExtensionSystem::SettingsPtr sett = robotSettings();
	if (sett->value("Robot/Dir").isValid()) {
		curDir = sett->value("Robot/Dir").toString();
		curPDir = curDir;
	}

	if (!configurationParameters.contains("tablesOnly")) {
		field->reloadSettings();
		reloadSettings(robotSettings(), QStringList());
	}

	return "";
}

void RobotModule::runGoUp()
{
	if (!DISPLAY) {
		qDebug() << "Go up";
		if (!curConsoleField->goUp()) {
			setError(trUtf8("Робот разбился: сверху стена!"));
		}
		return;
	}
	mutex.lock();
	qDebug() << "Robot up";
	QString status = "OK";
	if (!field->stepUp()) {
		field->setCrash(UP_CRASH);
		setError(trUtf8("Робот разбился: сверху стена!"));
		status = trUtf8("Отказ");
	}
	if (sender() == m_pultWidget) {
		m_pultWidget->Logger->appendText(trUtf8("вверх"), QString::fromUtf8("вверх     "), status);
	}
	if (animation) {
		msleep(AnimTime);
	}
	msleep(qrand() % 10);
	mutex.unlock();
	// view->update();
	return;
}


void RobotModule::runGoDown()
{
	if (!DISPLAY) {
		qDebug() << "Go down";
		if (!curConsoleField->goDown()) {
			setError(trUtf8("Робот разбился: снизу стена!"));
		}
		return;
	}
	mutex.lock();
	qDebug() << "Robot down";
	QString status = "OK";
	if (!field->stepDown()) {
		setError(trUtf8("Робот разбился: снизу стена!"));
		field->setCrash(DOWN_CRASH);
		status = trUtf8("Отказ");
	}
	if (sender() == m_pultWidget) {
		m_pultWidget->Logger->appendText(trUtf8("вниз"), QString::fromUtf8("вниз     "), status);
	}
	if (animation) {
		msleep(AnimTime);
	}
	msleep(qrand() % 10);
	mutex.unlock();
	return;
}


void RobotModule::runGoLeft()
{

	qDebug() << "Robot left";

	if (!DISPLAY) {
		if (!curConsoleField->goLeft()) {
			setError(trUtf8("Робот разбился: слева стена!"));
		}
		return;
	}
	mutex.lock();
	QString status = "OK";
	if (!field->stepLeft()) {
		field->setCrash(LEFT_CRASH);
		setError(trUtf8("Робот разбился: слева стена!"));
		status = trUtf8("Отказ");
	};
	if (sender() == m_pultWidget) {
		m_pultWidget->Logger->appendText(trUtf8("влево"), QString::fromUtf8("влево     "), status);
	}
	if (animation) {
		msleep(AnimTime);
	}
	msleep(qrand() % 10);
	mutex.unlock();
	return;
}


void RobotModule::runGoRight()
{
	if (!DISPLAY) {
		if (!curConsoleField->goRight()) {
			setError(trUtf8("Робот разбился: справа стена!"));
		}
		return;
	}
	//  qDebug() << "Robot right";
	mutex.lock();
	QString status = "OK";
	if (!field->stepRight()) {
		field->setCrash(RIGHT_CRASH);
		status = trUtf8("Отказ");

		setError(trUtf8("Робот разбился: справа стена!"));
	}

	if (sender() == m_pultWidget) {
		m_pultWidget->Logger->appendText(trUtf8("вправо"), QString::fromUtf8("вправо     "), status);
	}
	if (animation) {
		msleep(AnimTime);
	}
	msleep(qrand() % 10);
	mutex.unlock();

	return;
}


void RobotModule::runDoPaint()
{

	if (!DISPLAY) {
		curConsoleField->getCurItem()->isColored = true;
		return;
	}

	if (!field->currentCell()->IsColored) {
		field->reverseColorCurrent();
	}
	QString status = "OK";
//    if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
//        emit sendToPultLog(status);
//    }
	if (sender() == m_pultWidget) {
		m_pultWidget->Logger->appendText(trUtf8("закрасить"), trUtf8("закрасить"), "OK");
	}
	//  if(animation)
//   {
	view->update();
	msleep(AnimTime);
	//  }
	return;
}


bool RobotModule::runIsWallAtTop()
{

	if (!DISPLAY) {
		return curConsoleField->isUpWall();
	}

	bool result = !field->currentCell()->canUp();
	QString status = result ? trUtf8("Да") : trUtf8("Нет");
//    if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
//        emit sendToPultLog(status);
//    }
	if (sender() == m_pultWidget)
		m_pultWidget->Logger->appendText(QString::fromUtf8("вывод \'Сверху стена: \',сверху стена,нс"),
			trUtf8("сверху стена"), status);
	return result;
}


bool RobotModule::runIsWallAtBottom()
{

	if (!DISPLAY) {
		return curConsoleField->isDownWall();
	}

	bool result = !field->currentCell()->canDown();
	QString status = result ? trUtf8("Да") : trUtf8("Нет");
//    if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
//        emit sendToPultLog(status);
//    }
	if (sender() == m_pultWidget)
		m_pultWidget->Logger->appendText(QString::fromUtf8("вывод \'Снизу стена: \',снизу стена,нс"),
			QString::fromUtf8("снизу стена"), status);

	return result;
}


bool RobotModule::runIsWallAtLeft()
{
	if (!DISPLAY) {
		return curConsoleField->isLeftWall();
	}

	bool result = !field->currentCell()->canLeft();
	QString status = result ? trUtf8("Да") : trUtf8("Нет");
//    if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
//        emit sendToPultLog(status);
//    }
	if (sender() == m_pultWidget)
		m_pultWidget->Logger->appendText(QString::fromUtf8("вывод \'Слева стена: \',слева стена,нс"),
			trUtf8("слева стена"), status);

	return result;
}


bool RobotModule::runIsWallAtRight()
{

	if (!DISPLAY) {
		return curConsoleField->isRightWall();
	}
	bool result = !field->currentCell()->canRight();
	QString status = result ? trUtf8("Да") : trUtf8("Нет");
//    if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
//        emit sendToPultLog(status);
//    }
	if (sender() == m_pultWidget) {
		if (result)
			m_pultWidget->Logger->appendText(QString::fromUtf8("вывод \'Справа стена: \',справа стена,нс"),
				trUtf8("справа стена"), trUtf8("Да"));
		else
			m_pultWidget->Logger->appendText(QString::fromUtf8("вывод \'Справа стена: \',справа стена,нс"),
				trUtf8("справа стена"), trUtf8("Нет"));
	}
	return result;
}

bool RobotModule::runIsFreeAtTop()
{
	if (!DISPLAY) {
		qDebug() << "Is free U:" << !curConsoleField->isUpWall();
		return !curConsoleField->isUpWall();
	}

	bool result = field->currentCell()->canUp();
	QString status = result ? trUtf8("Да") : trUtf8("Нет");
	if (sender() == m_pultWidget)
		m_pultWidget->Logger->appendText(QString::fromUtf8("вывод \'Сверху свободно: \',сверху свободно,нс"),
			trUtf8("сверху свободно"), status);
	return result;
}


bool RobotModule::runIsFreeAtBottom()
{

	if (!DISPLAY) {
		qDebug() << "Is free D:" << !curConsoleField->isDownWall();
		return !curConsoleField->isDownWall();
	}

	bool result = field->currentCell()->canDown();
	QString status = result ? trUtf8("Да") : trUtf8("Нет");
	if (sender() == m_pultWidget)
		m_pultWidget->Logger->appendText(QString::fromUtf8("вывод \'Снизу свободно: \',снизу свободно,нс"),
			trUtf8("снизу свободно"), status);
	return result;
}


bool RobotModule::runIsFreeAtLeft()
{
	if (!DISPLAY) {
		return !curConsoleField->isLeftWall();
	}

	bool result = field->currentCell()->canLeft();
	QString status = result ? trUtf8("Да") : trUtf8("Нет");
	if (sender() == m_pultWidget) {
		m_pultWidget->Logger->appendText(
			QString::fromUtf8("вывод \'Слева свободно: \',слева свободно,нс"),
			trUtf8("слева свободно"),
			status
		);
	}
	return result;
}


bool RobotModule::runIsFreeAtRight()
{
	if (!DISPLAY) {
		return !curConsoleField->isRightWall();
	}
	bool result = field->currentCell()->canRight();
	QString status = result ? trUtf8("Да") : trUtf8("Нет");
	if (sender() == m_pultWidget) {
		m_pultWidget->Logger->appendText(
			QString::fromUtf8("вывод \'Справа свободно: \',справа свободно,нс"),
			trUtf8("справа свободно"),
			status
		);
	}
	return result;
}

bool RobotModule::runIsColor()
{
	if (!DISPLAY) {
		return curConsoleField->getCurItem()->isColored;
	}

	bool result = field->currentCell()->isColored();
	QString status = result ? trUtf8("Да") : trUtf8("Нет");

	if (sender() == m_pultWidget) {
		m_pultWidget->Logger->appendText(
			QString::fromUtf8("вывод \'Клетка закрашена: \',клетка закрашена,нс"),
			QString::fromUtf8("клетка закрашена"),
			status
		);
	}
	return result;
}

bool RobotModule::runIsClear()
{
	if (!DISPLAY) {
		return !curConsoleField->getCurItem()->isColored;
	}

	bool result = !field->currentCell()->isColored();
	QString status = result ? trUtf8("да") : trUtf8("нет");
	if (sender() == m_pultWidget) {
		m_pultWidget->Logger->appendText(
			QString::fromUtf8("вывод \'Клетка чистая: \',клетка чистая,нс"),
			QString::fromUtf8("клетка чистая"),
			status
		);
	}
	return result;
}


qreal RobotModule::runRadiation()
{
	if (!DISPLAY) {
		return curConsoleField->getCurItem()->radiation;
	}

	double result = field->currentCell()->radiation;
	QString status = QString::number(result);

	if (sender() == m_pultWidget) {
		m_pultWidget->Logger->appendText(
			QString::fromUtf8("вывод \'Радиация: \',радиация,нс"),
			QString::fromUtf8("радиация"),
			status
		);
	}
	return result;
}

int RobotModule::runTemperature()
{
	if (!DISPLAY) {
		return curConsoleField->getCurItem()->temperature;
	}

	int result = field->currentCell()->temperature;
	QString status = QString::number(result);

	if (sender() == m_pultWidget) {
		m_pultWidget->Logger->appendText(
			QString::fromUtf8("вывод \'Температура: \',температура,нс"),
			QString::fromUtf8("температура"),
			status
		);
	}
	return result;
}


void RobotModule::runFieldSize(int &rows, int &cols)
{
	if (!DISPLAY) {
		rows = curConsoleField->Rows();
		cols = curConsoleField->Cols();
		return;
	}

	rows = field->rows();
	cols = field->columns();
}

bool RobotModule::runMark(int row, int col)
{
	qDebug() << "Get M R:" << row << " C:" << col;
	if (!DISPLAY) {
		uint32_t r = row - 1, c = col - 1;
		if (
			r >= curConsoleField->Rows() ||
			c >= curConsoleField->Cols()
		) {
			setError(trUtf8("Нет какой клетки!"));
			return false;
		}

		return curConsoleField->getItem(r, c)->mark;
	}

	if (
		row - 1 >= field->rows() ||
		col - 1 >= field->columns() ||
		row - 1 < 0 ||
		col - 1 < 0
	) {
		setError(trUtf8("Нет какой клетки!"));
		return false;
	}
	return field->cellAt(row - 1, col - 1)->mark;
}

bool RobotModule::runColored(int row, int col)
{
	qDebug() << "Get Col R:" << row << " C:" << col;
	if (!DISPLAY) {
		uint32_t r = row - 1, c = col - 1;
		if (
			r >= curConsoleField->Rows() ||
			c >= curConsoleField->Cols()
		) {
			setError(trUtf8("Нет какой клетки!"));
			return false;
		}

		return curConsoleField->getItem(r, c)->isColored;
	}

	if (row - 1 >= field->rows() || col - 1 >= field->columns()) {
		setError(trUtf8("Нет какой клетки!"));
		return false;
	}
	return  field->cellAt(row - 1, col - 1)->isColored();
}

void RobotModule::runRobotPos(int &row, int &col)
{
	if (!DISPLAY) {
		row = curConsoleField->robotRow() + 1;
		col = curConsoleField->robotCol() + 1;
		return;
	}

	if (row - 1 >= field->rows() || col - 1 >= field->columns()) {
		setError(trUtf8("Нет какой клетки!"));
		return;
	}
	col = field->robotX() + 1;
	row = field->robotY() + 1;
}

QChar RobotModule::runUpChar(int row, int col)
{
	if (!DISPLAY) {
		uint32_t r = row - 1, c = col - 1;
		if (
			r >= curConsoleField->Rows() ||
			c >= curConsoleField->Cols()
		) {
			setError(trUtf8("Нет какой клетки!"));
			return ' ';
		}

		return curConsoleField->getItem(r, c)->upChar;
	}


	if (row - 1 >= field->rows() || col - 1 >= field->columns()) {

		setError(trUtf8("Нет какой клетки!"));

		return ' ';
	}
	qDebug() << field->cellAt(row - 1, col - 1)->upChar;
	return field->cellAt(row - 1, col - 1)->upChar;
}

int RobotModule::runCellTemp(int row, int col)
{
	if (!DISPLAY) {
		uint32_t r = row - 1, c = col - 1;
		if (r >= curConsoleField->Rows() ||
			c >= curConsoleField->Cols()
		) {
			setError(trUtf8("Нет какой клетки!"));
			return 0;
		}

		return curConsoleField->getItem(r, c)->temperature;
	}


	if (row - 1 >= field->rows() || col - 1 >= field->columns()) {

		setError(trUtf8("Нет какой клетки!"));

		return ' ';
	}
	qDebug() << field->cellAt(row - 1, col - 1)->upChar;
	return field->cellAt(row - 1, col - 1)->temperature;
}

qreal RobotModule::runCellRad(int row, int col)
{
	if (!DISPLAY) {
		uint32_t r = row - 1, c = col - 1;
		if (
			r >= curConsoleField->Rows() ||
			c >= curConsoleField->Cols()
		) {
			setError(trUtf8("Нет какой клетки!"));
			return 0;
		}
		return curConsoleField->getItem(r, c)->radiation;
	}

	if (row - 1 >= field->rows() || col - 1 >= field->columns()) {
		setError(trUtf8("Нет какой клетки!"));
		return ' ';
	}

	qDebug() << field->cellAt(row - 1, col - 1)->upChar;
	return field->cellAt(row - 1, col - 1)->radiation;
}

QChar RobotModule::runDownChar(int row, int col)
{
	qDebug() << "Get DC R:" << row << " C:" << col << " ROWS" << curConsoleField->Rows();
	if (!DISPLAY) {
		uint32_t r = row - 1, c = col - 1;
		if (
			r >= curConsoleField->Rows() ||
			c >= curConsoleField->Cols()
		) {
			setError(trUtf8("Нет какой клетки!"));
			return ' ';
		}

		return curConsoleField->getItem(r, c)->downChar;
	}

	if (row - 1 >= field->rows() || col - 1 >= field->columns()) {

		setError(trUtf8("Нет какой клетки!"));

		return ' ';
	}

	qDebug() << field->cellAt(row - 1, col - 1)->downChar;
	return field->cellAt(row - 1, col - 1)->downChar;
}

int RobotModule::LoadFromFile(QString p_FileName)
{
	qDebug() << "LoadField " << p_FileName;

	if (field->loadFromFile(p_FileName) != 0)
		return 1;

	startField = field->Clone();
	field->dropWasEdit();

	QFileInfo fi(p_FileName);
	QString name = fi.fileName();

	QString Title = trUtf8("Robot") + " - " + name;
	m_mainWidget->setWindowTitle(Title);

	RobotModule::robotSettings()->setValue("Robot/SFF", p_FileName);
	field->drawField(FIELD_SIZE_SMALL);
	qDebug() << "File " << p_FileName ;

	return 0;
}

void RobotModule::editEnv()
{
	if (field->isEditMode()) {
		reset(); //reset to normal mode
		setWindowSize();
		return;
	}
	startField->setModeFlag(NEDIT_MODE);

	view->showButtons(true);
	view->repaint();
	reset();
	field->setMode(NEDIT_MODE);
	startField->setModeFlag(NORMAL_MODE);

	setWindowSize();
	reloadSettings(robotSettings(), QStringList());
	field->update();
}

void RobotModule::createEmptyField(int rows, int cols)
{
	Q_UNUSED(cols);
	Q_UNUSED(rows);
	field->createField(7, 7);
	field->setRoboPos(0, 0);
	field->createRobot();
	startField = field->Clone();
	field->drawField(robotSettings()->value("Robot/CellSize", FIELD_SIZE_SMALL).toInt());

	mainWidget()->setWindowTitle(QString::fromUtf8("Робот - нет файла"));
	field->dropWasEdit();
}

bool RobotModule::isSafeToQuit()
{
	if (field->WasEdit()) {
		QMessageBox::StandardButton r;

		QMessageBox messageBox(
			QMessageBox::Question,
			trUtf8("Робот"),
			trUtf8("Сохранить изменения?"),
			QMessageBox::NoButton, mainWidget()
		);

		QPushButton *btnSave = messageBox.addButton(trUtf8("Да"),
			QMessageBox::AcceptRole);
		QPushButton *btnDiscard = messageBox.addButton(trUtf8("Нет"),
			QMessageBox::DestructiveRole);
		QPushButton *btnCancel = messageBox.addButton(trUtf8("Отмена"),
			QMessageBox::RejectRole);

		messageBox.setDefaultButton(btnSave);
		messageBox.exec();

		if (messageBox.clickedButton() == btnSave) {
			r = QMessageBox::Save;
		}

		if (messageBox.clickedButton() == btnDiscard) {
			r = QMessageBox::Discard;
		}

		if (messageBox.clickedButton() == btnCancel) {
			r = QMessageBox::Cancel;
		}

		if (r == QMessageBox::Save) {
			saveEnv();
		}

		if (r == QMessageBox::Cancel) {
			return false;
		}
	}

	return true;
}

void RobotModule::loadEnv()
{
	if (field->WasEdit()) {
		if (QMessageBox::question(
					mainWidget(), "",
					QString::fromUtf8("Сохранить изменения?"),
					QMessageBox::Yes, QMessageBox::No, 0
				) == QMessageBox::Yes
		) {
			saveEnv();
		}
	}

	QString RobotFile = QFileDialog::getOpenFileName(
		mainWidget(),
		QString::fromUtf8("Открыть файл"),
		curDir, "(*.fil)"
	);

	qDebug() << "CurDir" << curDir;

	QFileInfo info(RobotFile);
	QDir dir = info.absoluteDir();

	curDir = dir.path();
	robotSettings()->setValue("Robot/Dir", QVariant(curDir));

	if (RobotFile.isEmpty())
		return;

	if (LoadFromFile(RobotFile) != 0) { //Get troubles when loading env.
		QMessageBox::information(
			mainWidget(), "",
			QString::fromUtf8("Ошибка открытия файла! ") + RobotFile,
			0, 0, 0
		);
		return;
	}

	updateLastFiles(RobotFile);

	setWindowSize();
	view->setWindowTitle(trUtf8("Робот  - ") + info.baseName());
	robotSettings()->setValue("Robot/SFF", QVariant(RobotFile));
}

void RobotModule::resetEnv()
{
	reset();
}

void RobotModule::save2png()
{
	QString RobotFile = QFileDialog::getSaveFileName(
		mainWidget(),
		QString::fromUtf8("Сохранить как изображение"),
		curPDir, "(*.png)"
	);

	QFileInfo info(RobotFile);
	QDir dir = info.absoluteDir();
	curPDir = dir.path();

	int cellSize = robotSettings()->value("Robot/CellSize", FIELD_SIZE_SMALL).toInt();
	float multip = robotSettings()->value("ImageSize", FIELD_SIZE_SMALL).toFloat() / cellSize;

	QImage image(field->width()*multip, field->height()*multip, QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&image);
	field->render(&painter);
	image.save(RobotFile);
}

void RobotModule::prepareNewWindow()
{
	NewWindow = new QDialog(mainWidget());
	QGridLayout *nwl = new QGridLayout;
	NewWindow->setLayout(nwl);
	NewWindow->setWindowModality(Qt::WindowModal);
	NewWindow->setWindowTitle(QString::fromUtf8("Новая обстановка"));
	QLabel *lKol = new QLabel(QString::fromUtf8("Количество:"), NewWindow, 0);
	nwl->addWidget(lKol, 0, 0, 1, 2, Qt::AlignCenter);

	QLabel *lXSize = new QLabel(QString::fromUtf8("Столбцов"), NewWindow, 0);
	nwl->addWidget(lXSize, 1, 1, 1, 1, Qt::AlignCenter);

	QLabel *lYSize = new QLabel(QString::fromUtf8("Строк"), NewWindow, 0);
	nwl->addWidget(lYSize, 1, 0, 1, 1, Qt::AlignCenter);

	NewWindow->setFixedSize(250, 150);
	eXSizeEdit = new QSpinBox(NewWindow);
	eXSizeEdit->setRange(2, MAX_COLUMNS);
	eYSizeEdit = new QSpinBox(NewWindow);

	eYSizeEdit->setRange(2, MAX_ROWS);
	nwl->addWidget(eYSizeEdit, 2, 0, 1, 1, Qt::AlignCenter);
	nwl->addWidget(eXSizeEdit, 2, 1, 1, 1, Qt::AlignCenter);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(NewWindow);

	btnOK1 = buttonBox->addButton(QDialogButtonBox::Ok);
	btnCancel1 = new QPushButton(trUtf8("Отмена"));
	buttonBox->addButton(btnCancel1, QDialogButtonBox::RejectRole);

	nwl->addWidget(buttonBox, 3, 0, 1, 2, Qt::AlignRight);
	connect(btnCancel1, SIGNAL(clicked()), NewWindow, SLOT(close()));
	connect(btnOK1, SIGNAL(clicked()), this, SLOT(createNewField()));
}

void RobotModule::createNewField()
{
	field->destroyField();
	field->destroyRobot();
	field->createField(eYSizeEdit->value(), eXSizeEdit->value());
	field->setRoboPos(0, 0);
	field->createRobot();
	startField = field->Clone();
	field->drawField(FIELD_SIZE_SMALL);

	mainWidget()->setWindowTitle(QString::fromUtf8("Робот - нет файла"));

	view->centerOn(field->m_height() / 2, field->m_width() / 2);
	NewWindow->close();

	if (!field->isEditMode())
		editEnv();

	view->update();
}

void RobotModule::newEnv()
{
	if (field->WasEdit()) {
		QMessageBox::StandardButton r;

		QMessageBox messageBox(
			QMessageBox::Question,
			trUtf8("Новая обстановка"),
			trUtf8("Сохранить текущую?"),
			QMessageBox::NoButton, mainWidget()
		);

		QPushButton *btnSave = messageBox.addButton(
			trUtf8("Сохранить"), QMessageBox::AcceptRole
		);
		QPushButton *btnDiscard = messageBox.addButton(
			trUtf8("Не сохранять"), QMessageBox::DestructiveRole
		);
		QPushButton *btnCancel = messageBox.addButton(
			trUtf8("Отмена"), QMessageBox::RejectRole
		);

		messageBox.setDefaultButton(btnSave);
		messageBox.exec();

		if (messageBox.clickedButton() == btnSave) {
			r = QMessageBox::Save;
		}

		if (messageBox.clickedButton() == btnDiscard) {
			r = QMessageBox::Discard;
		}

		if (messageBox.clickedButton() == btnCancel) {
			r = QMessageBox::Cancel;
		}

		if (r == QMessageBox::Save) {
			saveEnv();
		}

		if (r == QMessageBox::Cancel) {
			return;
		}
	}

	NewWindow->show();
}

void RobotModule::saveEnv()
{
	QString RobotFile = QFileDialog::getSaveFileName(
		mainWidget(),
		QString::fromUtf8("Сохранить файл"),
		curDir, "(*.fil)"
	);

	QFileInfo info(RobotFile);
	QDir dir = info.absoluteDir();
	curDir = dir.path();
	robotSettings()->setValue("Robot/Dir", QVariant(curDir));
	if (RobotFile.contains("*") || RobotFile.contains("?")) {
		QMessageBox::information(0, "", trUtf8("Недопустимый символ в имени файла!"), 0, 0, 0);
		return;
	}

	if (RobotFile.right(4) != ".fil") {
		RobotFile += ".fil";
	}

	SaveToFile(RobotFile);

	RobotModule::robotSettings()->setValue("Robot/StartField/File", RobotFile);
	updateLastFiles(RobotFile);

	mainWidget()->setWindowTitle(trUtf8("Робот - ") + info.baseName());
}

int RobotModule::SaveToFile(QString p_FileName)
{
	if (field->saveToFile(p_FileName) != 0) {
		QMessageBox::warning(0, tr("Robot"), QString::fromUtf8("Не удалось сохранить файл"));
		return 1;
	}

	return 0;
}


void RobotModule::createRescentMenu()
{
	rescentMenu->clear();
	QStringList lastFiles = mySettings()->value("Robot/LastFiles").toString().split(";");
	qDebug() << lastFiles;
	if (lastFiles.count() == 0) {
		rescentMenu->setEnabled(false);
	} else {
		rescentMenu->setEnabled(true);
	}

	for (int i = 0; i < lastFiles.count(); i++) {
		if (lastFiles[i] == "") {
			continue;
		}
		QAction *action = rescentMenu->addAction("&" + QString::number(i + 1) + " " + lastFiles[i], this, SLOT(openRecent()));
		Q_UNUSED(action);
	}
}

QSize RobotModule::minimumSize() const
{
	return QSize(
		mySettings()->value("Robot/CellSize").toInt() * 3,
		mySettings()->value("Robot/CellSize", FIELD_SIZE_SMALL).toInt() * 3
	);
}

void RobotModule::setWindowSize()
{
	QSize newSize = view->size();
	QRect baseFieldSize; //fieldSize in pixel. Zoom 1:1
	baseFieldSize.setHeight(field->rows()*mySettings()->value("Robot/CellSize", FIELD_SIZE_SMALL).toInt());
	baseFieldSize.setWidth(field->columns()*mySettings()->value("Robot/CellSize", FIELD_SIZE_SMALL).toInt());
	int editEnlarge = 0;

	if (field->isEditMode()) {
		editEnlarge = 1.8 * mySettings()->value("Robot/CellSize", FIELD_SIZE_SMALL).toInt();
	}

	view->setMinimumSize(minimumSize());

	if (baseFieldSize.height() < view->height()) {
		newSize.setHeight(baseFieldSize.height() + mySettings()->value("Robot/CellSize", FIELD_SIZE_SMALL).toInt() + editEnlarge);
	}

	if (baseFieldSize.width() < view->width()) {
		newSize.setWidth(baseFieldSize.width() + mySettings()->value("Robot/CellSize", FIELD_SIZE_SMALL).toInt() + editEnlarge);
	}

	if (baseFieldSize.height() > view->height() && field->rows() < 11) {
		newSize.setHeight(baseFieldSize.height() + mySettings()->value("Robot/CellSize", FIELD_SIZE_SMALL).toInt() + editEnlarge);

	}

	if (baseFieldSize.width() > view->width() && field->columns() < 17) {
		newSize.setWidth(baseFieldSize.width() + mySettings()->value("Robot/CellSize", FIELD_SIZE_SMALL).toInt() + editEnlarge);
	}

	view->setWindowSize(newSize);
}

void RobotModule::openRecent()
{
	QAction *s = qobject_cast<QAction *>(sender());
	QString txt = s->text();
	if (field->WasEdit()) {
		QMessageBox::StandardButton r;
		QMessageBox messageBox(
			QMessageBox::Question,
			tr("New field"),
			tr("Save current field?"),
			QMessageBox::NoButton, mainWidget()
		);

		QPushButton *btnSave = messageBox.addButton(tr("Save"),
			QMessageBox::AcceptRole);
		QPushButton *btnDiscard = messageBox.addButton(tr("Don't save"),
			QMessageBox::DestructiveRole);
		QPushButton *btnCancel =
			messageBox.addButton(tr("Cancel"), QMessageBox::RejectRole);

		messageBox.setDefaultButton(btnSave);
		messageBox.exec();

		if (messageBox.clickedButton() == btnSave) {
			r = QMessageBox::Save;
		}

		if (messageBox.clickedButton() == btnDiscard) {
			r = QMessageBox::Discard;
		}

		if (messageBox.clickedButton() == btnCancel) {
			r = QMessageBox::Cancel;
		}

		if (r == QMessageBox::Save) {
			saveEnv();
		}

		if (r == QMessageBox::Cancel) {
			return;
		}
	}

	txt.remove(0, 1);
	QStringList words = txt.split(" ");

	if (words.count() < 2)
		return;

	QString RobotFile = words[1];
	if (LoadFromFile(RobotFile) != 0) {
		QMessageBox::information(mainWidget(), "", QString::fromUtf8("Ошибка открытия файла! ") + RobotFile, 0, 0, 0);
	}
	reset();
	view->setWindowSize(view->size());

}

void RobotModule::updateLastFiles(const QString newFile)
{
	QStringList lastFiles = RobotModule::robotSettings()->value("Robot/LastFiles").toString().split(";");
	if (lastFiles.indexOf(newFile) < 0) {
		lastFiles.prepend(newFile);
	}
	int max_fid = std::min(lastFiles.count(), 11);
	QString sett = "";
	for (int i = 0; i < max_fid; i++) {
		sett += lastFiles.at(i) + ";";
	}
	RobotModule::robotSettings()->setValue("Robot/LastFiles", sett);
	createRescentMenu();
}

void RobotModule::updateRobot()
{
}

void RobotModule::getTimer()
{
	mutex.lock();
	field->update();
	view->update();
	qApp->processEvents();
	mutex.unlock();
}

} // namespace ActorRobot

