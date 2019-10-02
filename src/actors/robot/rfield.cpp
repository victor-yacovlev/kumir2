#include "rfield.h"
#include "cfield.h"
#include "editline.h"
#include "srobot.h"
#include "robotview.h"
#include "robotmodule.h"
#include <assert.h>

namespace ActorRobot {

static const int MAX_CLICK_DELTA = 6;


FieldItm::FieldItm(QGraphicsItem *par, QGraphicsScene *scene)
{
	Q_UNUSED(par);
	sett = RobotModule::robotSettings();

	upWallLine = NULL;
	downWallLine = NULL;
	leftWallLine = NULL;
	rightWallLine = NULL;
	sepItmUp = NULL;
	sepItmDown = NULL;
	sepItmLeft = NULL;
	sepItmRight = NULL;
	Scene = scene;
	upWall = false;
	downWall = false;
	leftWall = false;
	rightWall = false;
	IsColored = false;
	mark = false;
	ColorRect = NULL;
	upChar = ' ';
	downChar = ' ';
	upCharItm = NULL;
	downCharItm = NULL;
	markItm = NULL;
	radItm = NULL;
	tempItm = NULL;

	font.setPixelSize(9);
	font.setPointSize(9);
	font.setStyle(QFont::StyleNormal);
	font.setBold(true);
	font.setStyleHint(QFont::SansSerif);
	radiation = 0;
	temperature = 0;
	font.setWeight(2);
	TextColor = QColor(sett->value("Robot/TextColor", "#FFFFFF").toString());
	upCharFld = NULL;
	downCharFld = NULL;

}

FieldItm::~FieldItm()
{
	cleanSelf();
}

void FieldItm::cleanSelf()
{
	if (upWallLine) {
		if (Scene) {
			Scene->removeItem(upWallLine);
		}
		if (upWallLine && upWallLine->scene() == Scene) {
			delete upWallLine;
		}
		upWallLine = NULL;
	}
	if (downWallLine) {
		if (Scene)
			if (downWallLine && downWallLine->scene() == Scene) {
				Scene->removeItem(downWallLine);
			}
		delete downWallLine;
		downWallLine = NULL;
	}
	if (leftWallLine) {
		if (Scene) {
			Scene->removeItem(leftWallLine);
		}
		if (leftWallLine && leftWallLine->scene() == Scene) {
			delete leftWallLine;
		}
		leftWallLine = NULL;
	}
	if (rightWallLine) {
		if (Scene) {
			Scene->removeItem(rightWallLine);
		}
		if (rightWallLine && rightWallLine->scene() == Scene) {
			delete rightWallLine;
		}
		rightWallLine = NULL;
	}
	if (ColorRect) {
		if (Scene) {
			Scene->removeItem(ColorRect);
		}
		delete ColorRect;
		ColorRect = NULL;
	}
	if (upCharItm) {
		if (Scene) {
			Scene->removeItem(upCharItm);
		}
		delete upCharItm;
		upCharItm = NULL;
	}
	if (downCharItm) {
		if (Scene) {
			Scene->removeItem(downCharItm);
		}
		delete downCharItm;
		downCharItm = NULL;
	}
	if (markItm) {
		if (Scene) {
			Scene->removeItem(markItm);
		}
		delete markItm;
		markItm = NULL;
	}
	if (upCharFld) {
		if (Scene) {
			Scene->removeItem(upCharFld);
		}
		upCharFld = NULL;
	}
	if (downCharFld) {
		if (Scene) {
			Scene->removeItem(downCharFld);
		}
		downCharFld = NULL;
	}
	if (radItm) {
		if (Scene) {
			Scene->removeItem(radItm);
		}
		radItm = NULL;
	}
	if (tempItm) {
		if (Scene) {
			Scene->removeItem(tempItm);
		}
		tempItm = NULL;
	}
}


void FieldItm::setTextColor()
{
	sett = RobotModule::robotSettings();
	TextColor = QColor(sett->value("TextColor", "#FFFFFF").toString());


	if (upCharItm) {
		upCharItm->setDefaultTextColor(TextColor);
	}
	if (downCharItm) {
		downCharItm->setDefaultTextColor(TextColor);
	}


};

void FieldItm::setLeftsepItem(FieldItm *ItmLeft)
{
	sepItmLeft = ItmLeft;
	ItmLeft->setRightsepItem(this); //Obratnaya ssilka u soseda
}

void FieldItm::setRightsepItem(FieldItm *ItmRight)
{
	sepItmRight = ItmRight;
}

void FieldItm::setUpsepItem(FieldItm *ItmUp)
{
	sepItmUp = ItmUp;
	ItmUp->setDownsepItem(this);
}

void FieldItm::setDownsepItem(FieldItm *ItmDown)
{
	sepItmDown = ItmDown;
}
void FieldItm::removeDownsepItem()
{
	sepItmDown = NULL;
};
void FieldItm::removeLeftsepItem()
{

	if (sepItmLeft) {
		sepItmLeft->removeRightsepItem();
	}
	sepItmLeft = NULL;
};
void FieldItm::removeRightsepItem()
{
	sepItmRight = NULL;
};
void FieldItm::setWalls(int wallByte)
{
	if ((wallByte & UP_WALL) == UP_WALL) {
		upWall = true;
	} else {
		upWall = false;
	}
	if ((wallByte & DOWN_WALL) == DOWN_WALL) {
		downWall = true;
	} else {
		downWall = false;
	}
	if ((wallByte & LEFT_WALL) == LEFT_WALL) {
		leftWall = true;
	} else {
		leftWall = false;
	}
	if ((wallByte & RIGHT_WALL) == RIGHT_WALL) {
		rightWall = true;
	} else {
		rightWall = false;
	}
}

int FieldItm::wallByte()
{
	int toret = 0;
	if (upWall) {
		toret += UP_WALL;
	}
	if (downWall) {
		toret += DOWN_WALL;
	}
	if (leftWall) {
		toret += LEFT_WALL;
	}
	if (rightWall) {
		toret += RIGHT_WALL;
	}
	return toret;
}

void FieldItm::showCharMark(qreal upLeftCornerX, qreal upLeftCornerY, int size)
{
	showUpChar(upLeftCornerX, upLeftCornerY, size);
	showDownChar(upLeftCornerX, upLeftCornerY, size);
	showMark(upLeftCornerX, upLeftCornerY, size);
}

void FieldItm::showRTItm(
		qreal upLeftCornerX, qreal upLeftCornerY,
		int size, int mode
) {
	Q_UNUSED(size);
	if (radItm) {
		if (Scene) {
			Scene->removeItem(radItm);
		}
		delete radItm;
		radItm = NULL;
	}

	if (tempItm) {
		if (Scene) {
			Scene->removeItem(tempItm);
		}
		delete tempItm;
		tempItm = NULL;
	}


	radItm = new EditLine(0, false);
	radItm->moveBy(upLeftCornerX + 1, upLeftCornerY + 1);
	radItm->setScale(0.25);
	radItm->setValue(radiation);
	Scene->addItem(radItm);
	radItm->setZValue(100);

	if (mode == RAD_MODE) {
		radItm->show();
	} else {
		radItm->hide();
	}

	tempItm = new EditLine(0, true);
	tempItm->moveBy(upLeftCornerX + 1, upLeftCornerY + 1);
	tempItm->setValue(temperature);
	Scene->addItem(tempItm);
	tempItm->setScale(0.25);
	tempItm->setZValue(100);
	if (mode == TEMP_MODE) {
		tempItm->show();
	} else {
		tempItm->hide();
	}
}

void FieldItm::hideRTItm()
{
	if (radItm) {
		radItm->hide();
	};
	if (tempItm) {
		tempItm->hide();
	};
}

void FieldItm::showCharFld(
	qreal upLeftCornerX, qreal upLeftCornerY,
	int field_size
) {
	if (upCharFld) {
		Scene->removeItem(upCharFld);
	}
	upCharFld = new QGraphicsRectItem(upLeftCornerX + 4, upLeftCornerY + 4, 7, 11);
	upCharFld->setPen(QPen(Qt::NoPen));
	upCharFld->setBrush(QBrush(QColor(60, 115, 175)));
	Scene->addItem(upCharFld);
	upCharFld->setZValue(0.2);

	if (downCharFld) {
		Scene->removeItem(downCharFld);
	}
	downCharFld = new QGraphicsRectItem(upLeftCornerX + 4, field_size + upLeftCornerY - 14, 7, 11);
	downCharFld->setPen(QPen(Qt::NoPen));
	downCharFld->setBrush(QBrush(QColor(60, 115, 175)));
	Scene->addItem(downCharFld);
	downCharFld->setZValue(0.2);
}

void FieldItm::hideCharFld()
{

	if (upCharFld != NULL) {
		Scene->removeItem(upCharFld);
	}
	if (!Scene) {
		qDebug() << "Scene is NULL!";
		return;
	}
	if (downCharFld) {
		Scene->removeItem(downCharFld);
	}
}

void FieldItm::showUpChar(qreal upLeftCornerX, qreal upLeftCornerY, int size)
{
	Q_UNUSED(size);
	if (upCharItm) {
		if (Scene) {
			Scene->removeItem(upCharItm);
		}
		delete upCharItm;
		upCharItm = NULL;
	}
	if (upChar.isPrint() && upChar != ' ') {
		upCharItm = Scene->addText(upChar, font);
		upCharItm->setDefaultTextColor(TextColor);
		float lettShift = sett->value("LettShift", "2").toFloat();
		upCharItm->setPos(upLeftCornerX, upLeftCornerY - 2 - lettShift);
		upCharItm->setZValue(1);
	}
}

void FieldItm::showDownChar(qreal upLeftCornerX, qreal upLeftCornerY, int size)
{
	if (downCharItm) {
		if (Scene) {
			Scene->removeItem(downCharItm);
		}
		delete downCharItm;
		downCharItm = NULL;
	}
	if (downChar.isPrint() && downChar != ' ') {
		downCharItm = Scene->addText(downChar, font);
		downCharItm->setDefaultTextColor(TextColor);
		float lettShift = sett->value("LettShift", "2").toFloat();
		downCharItm->setPos(upLeftCornerX, upLeftCornerY + size - 17 - lettShift);
#ifdef Q_OS_WIN
		downCharItm->setPos(upLeftCornerX, upLeftCornerY + size - 19 - lettShift);
#endif
		downCharItm->setZValue(1);
	}
}

void FieldItm::showMark(qreal upLeftCornerX, qreal upLeftCornerY, int size)
{
	if (mark) {
		if (markItm) {
			if (Scene) {
				Scene->removeItem(markItm);
			}
			delete markItm;
			markItm = NULL;
		}
		float xshift = sett->value("MarkShiftLeft", "6").toFloat();
		float yshift = sett->value("MarkShift", "3").toFloat();
		markItm = Scene->addText(QChar(9679), font);
		markItm->setDefaultTextColor(TextColor);
		markItm->setPos(upLeftCornerX + size - (size / 3) - 2 - xshift, upLeftCornerY - 18 + size - yshift);
#ifdef Q_OS_WIN
		markItm->setPos(upLeftCornerX + size - (size / 3) - xshift, upLeftCornerY - 20 + size - yshift);
#endif
		markItm->setZValue(1);
	}
}

void FieldItm::setColorRect(QGraphicsRectItem *Rect, QColor color)
{
	ColorRect = Rect;
	ColorRect->setPen(color);
	ColorRect->setBrush(QBrush(color));

	Scene->addItem(Rect);

	Rect->setZValue(0.2);
	IsColored = true;
}

void FieldItm::setUpLine(QGraphicsLineItem *Line, const QPen &pen)
{
	upWallLine = Line;
	upWallLine->setPen(pen);
	Scene->addItem(upWallLine);
	upWallLine->setZValue(1);
	Scene->update();
	update();
}

void FieldItm::setDownLine(QGraphicsLineItem *Line, const QPen &pen)
{
	downWallLine = Line;
	downWallLine->setPen(pen);
	Scene->addItem(downWallLine);
	downWallLine->setZValue(1);
	Scene->update();
	update();
}

void FieldItm::setLeftLine(QGraphicsLineItem *Line, const QPen &pen)
{
	leftWallLine = Line;
	leftWallLine->setPen(pen);
	Scene->addItem(leftWallLine);
	leftWallLine->setZValue(1);
	Scene->update();
	update();
}

void FieldItm::setRightLine(QGraphicsLineItem *Line, const QPen &pen)
{
	rightWallLine = Line;
	rightWallLine->setPen(pen);
	Scene->addItem(rightWallLine);

	rightWallLine->setZValue(1);
	Scene->update();
	update();
}

bool FieldItm::hasUpWall()
{
	return upWall;
}

bool FieldItm::hasDownWall()
{
	return downWall;
}
bool FieldItm::hasLeftWall()
{
	return leftWall;
}

bool FieldItm::hasRightWall()
{
	return rightWall;
}

bool FieldItm::canUp()
{
	if (!hasUpSep()) {
		return false;
	}
	return !(upWall || sepItmUp->hasDownWall());
}

bool FieldItm::canDown()
{
	if (!hasDownSep()) {
		return false;
	}
	return !(downWall || sepItmDown->hasUpWall());
}

bool FieldItm::canLeft()
{
	if (!hasLeftSep()) {
		return false;
	}
	return !(leftWall || sepItmLeft->hasRightWall());
}

bool FieldItm::canRight()
{
	if (!hasRightSep()) {
		return false;
	}
	return !(rightWall || sepItmRight->hasLeftWall());
}

bool FieldItm::hasUpSep()
{
	if (sepItmUp) {
		return true;
	}
	return false;
}

bool FieldItm::hasDownSep()
{
	if (sepItmDown) {
		return true;
	}
	return false;
}

bool FieldItm::hasLeftSep()
{
	if (sepItmLeft) {
		return true;
	}
	return false;
}

bool FieldItm::hasRightSep()
{
	if (sepItmRight) {
		return true;
	}
	return false;
}

void FieldItm::setUpWall(QGraphicsLineItem *Line, const QPen &pen)
{
	upWall = true;
	setUpLine(Line, pen);
}

void FieldItm::setDownWall(QGraphicsLineItem *Line, const QPen &pen)
{
	downWall = true;
	setDownLine(Line, pen);
}

void FieldItm::setLeftWall(QGraphicsLineItem *Line, const QPen &pen)
{
	leftWall = true;
	setLeftLine(Line, pen);
}

void FieldItm::setRightWall(QGraphicsLineItem *Line, const QPen &pen)
{
	rightWall = true;
	setRightLine(Line, pen);
}

void FieldItm::removeUpWall()
{
	if (upWallLine) {
		Scene->removeItem(upWallLine);
		delete upWallLine;
		upWallLine = NULL;
		qDebug("UwallRemoved");
	}
	upWall = false;
	if (hasUpSep())
		if (sepItmUp->hasDownWall()) {
			sepItmUp->removeDownWall();
		}
}

void FieldItm::removeDownWall()
{
	if (downWallLine) {
		Scene->removeItem(downWallLine);
		delete downWallLine;
		downWallLine = NULL;
		qDebug("DwallRemoved");
	};
	downWall = false;

	if (hasDownSep())
		if (sepItmDown->hasUpWall()) {
			sepItmDown->removeUpWall();
		}
}

void FieldItm::removeLeftWall()
{
	if (leftWallLine) {
		Scene->removeItem(leftWallLine);
		delete leftWallLine;
		leftWallLine = NULL;
		qDebug("LwallRemoved");
	};
	leftWall = false;

	if (hasLeftSep())
		if (sepItmLeft->hasRightWall()) {
			sepItmLeft->removeRightWall();
		}
}

void FieldItm::removeRightWall()
{
	if (rightWallLine) {
		QGraphicsScene *debug = rightWallLine->scene();
		Q_UNUSED(debug);
		rightWallLine->setVisible(false);
		Scene->removeItem(rightWallLine);
		delete rightWallLine;
		rightWallLine = NULL;
		qDebug("RwallRemoved");
	}
	rightWall = false;

	if (hasRightSep() && sepItmRight->hasLeftWall()) {
		sepItmRight->removeLeftWall();
	}
}

void FieldItm::removeColor()
{
	if (ColorRect) {
		Scene->removeItem(ColorRect);
		delete ColorRect;
		ColorRect = NULL;
	}
	IsColored = false;
}

void FieldItm::removeMark()
{
	if (markItm) {
		Scene->removeItem(markItm);
		delete markItm;
		markItm = NULL;
	}
	mark = false;
}

void FieldItm::removeUpChar()
{
	if (upCharItm) {
		Scene->removeItem(upCharItm);
		delete upCharItm;
		upCharItm = NULL;
	}
	upChar = ' ';
}

void FieldItm::removeDownChar()
{
	if (downCharItm) {
		Scene->removeItem(downCharItm);
		delete downCharItm;
		downCharItm = NULL;
	}
	downChar = ' ';
}

bool FieldItm::emptyCell()
{
	return
		!upWall && !downWall && !leftWall && !rightWall &&
		!IsColored && !mark &&
		(radiation == 0) && (temperature == 0) &&
		(upChar == ' ') && (downChar == ' ')
	;
}

void FieldItm::setScene(QGraphicsScene *scene)
{
	Scene = scene;
}

void FieldItm::wbWalls()
{
	if (UpWallLine()) {
		wallPen = UpWallLine()->pen();
		int w = 3;
		if (!hasUpSep()) {
			w = 4;
		}
		UpWallLine()->setPen(QPen(QBrush(QColor("blue")), w));
	}

	if (DownWallLine()) {
		wallPen = DownWallLine()->pen();
		int w = 3;
		if (!hasDownSep()) {
			w = 4;
		}
		DownWallLine()->setPen(QPen(QBrush(QColor("blue")), w));
	}

	if (LeftWallLine()) {
		wallPen = LeftWallLine()->pen();
		int w = 3;
		if (!hasLeftSep()) {
			w = 4;
		}
		LeftWallLine()->setPen(QPen(QBrush(QColor("blue")), w));
	}

	if (RightWallLine()) {
		wallPen = RightWallLine()->pen();
		int w = 3;
		if (!hasRightSep()) {
			w = 4;
		}
		RightWallLine()->setPen(QPen(QBrush(QColor("blue")), w));
	}

	if (downCharItm) {
		downCharItm->setDefaultTextColor("black");
	}
	if (upCharItm) {
		upCharItm->setDefaultTextColor("black");
	}
	if (markItm) {
		markItm->setDefaultTextColor("black");
	}
}

void FieldItm::colorWalls()
{
	if (UpWallLine()) {
		if (!hasUpSep()) {
			wallPen.setWidth(4);
		} else {
			wallPen.setWidth(3);
		}
		UpWallLine()->setPen(wallPen);
	};
	if (DownWallLine()) {
		if (!hasDownSep()) {
			wallPen.setWidth(4);
		} else {
			wallPen.setWidth(3);
		}
		DownWallLine()->setPen(wallPen);
	};
	if (LeftWallLine()) {
		if (!hasLeftSep()) {
			wallPen.setWidth(4);
		} else {
			wallPen.setWidth(3);
		}
		LeftWallLine()->setPen(wallPen);
	};
	if (RightWallLine()) {
		if (!hasRightSep()) {
			wallPen.setWidth(4);
		} else {
			wallPen.setWidth(3);
		}
		RightWallLine()->setPen(wallPen);
	};
	if (downCharItm) {
		downCharItm->setDefaultTextColor("white");
	}
	if (upCharItm) {
		upCharItm->setDefaultTextColor("white");
	}
	if (markItm) {
		markItm->setDefaultTextColor("white");
	}
}

FieldItm *FieldItm::Copy()
{
	FieldItm *copy = new FieldItm(0, Scene);
	copy->leftWall = leftWall;
	copy->rightWall = rightWall;
	copy->upWall = upWall;
	copy->downWall = downWall;

	copy->upChar = upChar;
	copy->downChar = downChar;
	copy->mark = mark;
	copy->radiation = radiation;
	copy->temperature = temperature;
	copy->IsColored = IsColored;
	copy->upCharFld = upCharFld;
	return copy;
};


RoboField::~RoboField()
{
	destroyField();
	destroyRobot();
	destroyNet();
	destroyScene();
	for (int i = 0; i < Items.size(); i++) {
		for (int j = 0; j < Items[i].size(); j++) {
			if (Items[i][j]) {
				delete Items[i][j];
			}
		}
	}
	Items.clear();
}

RoboField::RoboField(QWidget *parent, RobotModule *actor)
	: QGraphicsScene(parent)
{
	old_cell = QPair<int, int>(-1, -1);
	pressed = false;

	Parent = parent;
	mode = NORMAL_MODE;

	sett = RobotModule::robotSettings();

	reloadSettings();

	fieldSize = 30;
	this->setItemIndexMethod(NoIndex);
	robot = NULL;
	markMode = true;
	Actor = actor;
	wasEdit = false;

	qDebug() << RobotModule::self->myResourcesDir().absoluteFilePath("plus.png");
	PlusIcon = QIcon(RobotModule::self->myResourcesDir().absoluteFilePath("plus.png"));
	MinusIcon = QIcon(RobotModule::self->myResourcesDir().absoluteFilePath("minus.png"));

	showWall = new QGraphicsLineItem(0, 0, 0, 0);
	this->addItem(showWall);

	keyCursor = new QGraphicsLineItem(0, 0, 0, 0);
	this->addItem(keyCursor);
	keyCursor->hide();

	radSpinBox = new QDoubleSpinBox();
	radSpinBox->setRange(0, 99);
	radSpinBox->setValue(55);

	tempSpinBox = new QSpinBox();
	tempSpinBox->setRange(-273, 233);
	tempSpinBox->setValue(77);

	btnAddRow = new QToolButton();
	btnAddCol = new QToolButton();
	btnRemCol = new QToolButton();
	btnRemRow = new QToolButton();

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timerTic()));
}

void RoboField::reloadSettings()
{
	sett = RobotModule::robotSettings();

	TextColor = QColor(sett->value("TextColor", "#FFFFFF").toString());
	LineColor = QColor(sett->value("LineColor", "#C8C800").toString());
	EditLineColor = QColor(sett->value("LineColorEdit", "#C8C800").toString());
	WallColor = QColor(sett->value("WallColor", "#C8C810").toString());
	NormalColor = QColor(sett->value("NormalColor", "#289628").toString());
	EditColor = QColor(sett->value("EditColor", "#6496ff").toString());
	FillColor = QColor(sett->value("FillColor", "#9370db").toString());

	LetterShift = sett->value("LettShift", "2").toInt();
	MarkShift = sett->value("MarkShift", "3").toInt();
	MarkShiftLeft = sett->value("MarkShiftLeft", "6").toInt();

	BortWidth = sett->value("BortW", "6").toInt();
	GridWidth = sett->value("StW", "1").toInt();
	WallWidth = sett->value("WallW", "5").toInt();

	BortLine = QPen(WallColor, BortWidth);
	WallLine = QPen(WallColor, WallWidth);
	ShowLine = QPen(QColor(0, 255, 0, 125), WallWidth);

	QColor gridColor, bgColor;

	if (mode == NORMAL_MODE) {
		gridColor = LineColor;
		bgColor = NormalColor;
	} else {
		gridColor = EditLineColor;
		bgColor = EditColor;
	}

	QPen gridLine = QPen(gridColor, GridWidth);
	this->setBackgroundBrush(QBrush(bgColor));

	for (int i = 0; i < setka.count(); i++) {
		setka.at(i)->setPen(gridLine);
	}
}


void RoboField::showButtons(bool yes)
{
	if (!wAddCol) {
		return;
	}
	wAddCol->setVisible(yes);
	wAddRow->setVisible(yes);
	wRemCol->setVisible(yes);
	wRemRow->setVisible(yes);
	if (yes) {
		wAddRow->setPos((fieldSize * (columns()) / 2) - (wAddRow->size().height() + 3), fieldSize * (rows()) + 5);
		wRemRow->setPos(fieldSize * (columns()) / 2 + 2, fieldSize * ((float)rows()) + 5);

		wAddCol->setPos(fieldSize * ((float)columns()) + 7, (fieldSize * (rows()) / 2) - (wAddRow->size().width() + 1));
		wRemCol->setPos(fieldSize * ((float)columns()) + 7, fieldSize * (rows()) / 2 + 1);
	}
}

void RoboField::setMode(int Mode)
{
	mode = Mode;
	sett = RobotModule::robotSettings();
	QGraphicsView *view = views().first();
	if (mode == NORMAL_MODE) {
		if (this->items().indexOf(keyCursor) > -1) {
			this->removeItem(keyCursor);
		}

		radSpinBox->hide();
		tempSpinBox->hide();
		redrawEditFields();
		redrawRTFields();
		view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		showButtons(false);
		update();
	}
	if (mode == NEDIT_MODE) {
		if (this->items().indexOf(keyCursor) > -1) {
			this->removeItem(keyCursor);
		}
		radSpinBox->hide();
		tempSpinBox->hide();
		redrawEditFields();
		redrawRTFields();
		view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		showButtons(true);
		update();
	}

	if (mode == RAD_MODE) {
		if (this->items().indexOf(keyCursor) > -1) {
			this->removeItem(keyCursor);
		}

		tempSpinBox->hide();
		radSpinBox->setParent(view);
		radSpinBox->move(100, 2);

		radSpinBox->show();
		clickCell = QPair<int, int>(-1, -1);



		redrawEditFields();
		redrawRTFields();
		showButtons(false);
		update();

	}
	if (mode == TEMP_MODE) {
		if (this->items().indexOf(keyCursor) > -1) {
			this->removeItem(keyCursor);
		}

		radSpinBox->hide();
		tempSpinBox->setParent(view);
		tempSpinBox->move(100, 2);

		tempSpinBox->show();
		clickCell = QPair<int, int>(-1, -1);



		redrawEditFields();
		redrawRTFields();
		showButtons(false);
		update();

	}
	if (mode == TEXT_MODE) {
		tempSpinBox->hide();
		radSpinBox->hide();
		redrawRTFields();
		setTextEditMode(true);
		showButtons(false);
		update();
	}

	update();
	view->repaint();
	update();
}

void RoboField::editField()
{
	mode = NEDIT_MODE;
	showButtons(true);
};
/**
 * Создание пустого поля робота
 * @param x количество строк
 * @param y количество столбцов
 */
void RoboField::createField(int x, int y)
{
	destroyNet();
	destroyField();
	destroyRobot();
	//qDebug("Before items destroy");
	while (Items.count() > 0) {
		Items.removeFirst();
	}
	//qDebug("After items destroy");
	for (int i = 0; i < x; i++) {
		QList<FieldItm *> row;
		for (int j = 0; j < y; j++) {

			row.append(new FieldItm(0, this));
			FieldItm *thisItm = row.last();
			if (j > 0) {
				thisItm->setLeftsepItem(row[j - 1]);    //Устанавливаем соседей
			}
			if (i > 0) {
				thisItm->setUpsepItem(Items[i - 1].at(j));    //Устанавливаем соседей
			}
		};
		Items.append(row);
	};
	createResizeButtons();

	timer->stop();
}

void RoboField::addRow()
{
	qDebug() << "addRow";
	QList<FieldItm *> row;
	for (int i = 0; i < columns(); i++) {
		row.append(new FieldItm(0, this));
		FieldItm *thisItm = row.last();
		if (i > 0) {
			thisItm->setLeftsepItem(row[i - 1]);
		}
		thisItm->setUpsepItem(Items[Items.count() - 1].at(i));
	}
	Items.append(row);
	drawField(fieldSize);
	showButtons(true);
	qDebug() << "addRow";
}

void RoboField::remRow()
{

	if (Items.count() > 1) {
		for (int i = 0; i < columns(); i++) {
			Items[Items.count() - 2].at(i)->removeDownsepItem();
		}

		Items.removeLast();
		if (robotY() > rows() - 1) {
			setRoboPos(robotX(), robotY() - 1);
		}
		drawField(fieldSize);
		showButtons(true);
	}
}

void RoboField::remCol()
{
	if (Items.count() > 0 && (Items[0].count() > 1)) {
		for (int i = 0; i < Items.count(); i++) {
			Items[i].at(Items[i].count() - 1)->removeLeftsepItem();
			Items[i].removeLast();
		}
		if (robotX() > columns() - 1) {
			setRoboPos(robotX() - 1, robotY());
		}

		drawField(fieldSize);
		showButtons(true);
	}
}

void RoboField::addCol()
{
	btnAddCol->setDown(true);

	for (int i = 0; i < Items.count(); i++) {
		FieldItm *lastItm = Items[i].last();
		Items[i].append(new FieldItm(0, this));

		Items[i].last()->setLeftsepItem(lastItm);
		if (i > 0) {
			Items[i].last()->setUpsepItem(Items[i - 1].last());
		}

	}

	drawField(fieldSize);
	showButtons(true);
}

QPoint RoboField::upLeftCorner(int str, int stlb)
{
	int ddx = BortWidth - 2;
	return QPoint(stlb * fieldSize + ddx, str * fieldSize);
}

FieldItm *RoboField::getFieldItem(int str, int stlb) const
{
	if (rows() < str) {
		qDebug("RoboField:rows()<str");
		return NULL;
	}
	if (columns() < stlb) {
		qDebug("RoboField:columns()<str");
		return NULL;
	}

	if (str >= 0 && stlb >= 0 && str < Items.size() && stlb < Items[str].size()) {
		return Items[str].at(stlb);
	} else {
		return NULL;
	}

}

void RoboField::drawField(uint FieldSize)
{
	sett = RobotModule::robotSettings();
	keyCursor = new QGraphicsLineItem(0, 0, 0, 0);
	this->addItem(keyCursor);
	keyCursor->hide();
	if (rows() < 1 || columns() < 1) {
		return;
	}
	destroyField();

	QColor gridColor, bgColor;
	showWall = new QGraphicsLineItem(0, 0, 0, 0);
	if (mode == NORMAL_MODE) {
		bgColor = NormalColor;
		gridColor = LineColor;
	} else {
		bgColor = EditColor;
		gridColor = EditLineColor;
	}

	this->setBackgroundBrush(QBrush(bgColor));
	fieldSize = FieldSize;
	int xd = FieldSize, yd = FieldSize;

	drawNet();

	qDebug() << "Rows:" << rows() << ", Cols:" << columns();

	for (int i = 0; i < rows(); i++) {
		QList<FieldItm *> *row = &Items[i];
		for (int j = 0; j < columns(); j++) {
			int ulx = upLeftCorner(i, j).x(), uly = upLeftCorner(i, j).y();
			FieldItm *cell = row->at(j);

			cell->setScene(this);

			if (!cell->hasUpSep())
				cell->setUpLine(
					new QGraphicsLineItem(
						ulx, uly,
						ulx + xd, uly
					),
					BortLine
				);

			if (!cell->hasDownSep())
				cell->setDownLine(
					new QGraphicsLineItem(
						ulx, uly + yd,
						ulx + xd, uly + yd
					),
					BortLine
				);


			if (!cell->hasLeftSep())
				cell->setLeftLine(
					new QGraphicsLineItem(
						ulx, uly,
						ulx, uly + yd
					),
				BortLine);

			if (!cell->hasRightSep())
				cell->setRightLine(
					new QGraphicsLineItem(
						ulx + xd, uly,
						ulx + xd, uly + yd
					),
					BortLine
				);

			if (cell->hasDownWall()) {
				cell->setDownLine(
					new QGraphicsLineItem(
						ulx, uly + yd,
						ulx + xd, uly + yd
					),
					WallLine
				);

			}

			if (cell->hasUpWall()) {
				cell->setUpLine(
					new QGraphicsLineItem(
						ulx, uly,
						ulx + xd, uly
					),
					WallLine
				);
			}

			if (cell->hasLeftWall()) {
				cell->setLeftLine(
					new QGraphicsLineItem(
						ulx, uly,
						ulx, uly + yd
					),
					WallLine
				);
			}

			if (cell->hasRightWall()) {
				cell->setRightLine(
					new QGraphicsLineItem(
						ulx + xd, uly,
						ulx + xd, uly + yd
					),
					WallLine
				);
			}

			if (cell->isColored()) {
				cell->setColorRect(
					new QGraphicsRectItem(
						ulx, uly,
						xd, yd
					),
					FillColor
				);
			}

			cell->showCharMark(ulx, uly, fieldSize);
			cell->setTextColor();
		}
	}

	createResizeButtons();
	destroyRobot();
	createRobot();
}

void RoboField::createResizeButtons()
{
	btnAddRow = new QToolButton();
	btnAddCol = new QToolButton();
	btnRemCol = new QToolButton();
	btnRemRow = new QToolButton();

	btnAddRow->setCheckable(false);
	btnAddCol->setCheckable(false);
	btnAddRow->setIcon(PlusIcon);
	btnAddCol->setIcon(PlusIcon);

	btnRemCol->setCheckable(false);
	btnRemRow->setCheckable(false);
	btnRemCol->setIcon(MinusIcon);
	btnRemRow->setIcon(MinusIcon);

	btnAddRow->setAutoRaise(true);
	btnAddCol->setAutoRaise(true);
	btnRemCol->setAutoRaise(true);
	btnRemRow->setAutoRaise(true);

	wAddRow = addWidget(btnAddRow);
	wAddCol = addWidget(btnAddCol);
	wRemCol = addWidget(btnRemCol);
	wRemRow = addWidget(btnRemRow);
	wAddRow->setZValue(200);

	wAddRow->resize(fieldSize, 0.5 * fieldSize);
	wRemRow->resize(fieldSize, 0.5 * fieldSize);

	wAddCol->resize(0.5 * fieldSize, fieldSize);
	wRemCol->resize(0.5 * fieldSize, fieldSize);

	wAddCol->setZValue(200);
	wRemRow->setZValue(200);
	wRemCol->setZValue(200);

	showButtons(false);
}

void RoboField::destroyField()
{
	qDebug() << "cols" << columns();
	for (int i = 0; i < columns(); i++) {
		for (int j = 0; j < rows(); j++)  {
			FieldItm *itm = getFieldItem(j, i);
			if (itm) {
				itm->cleanSelf();
			}
		}
	}

	clear();
	setka.clear();
	robot = NULL;
	keyCursor = NULL;
	this->update();
}

void RoboField::setTextEditMode(bool flag)
{
	clickCell = QPair<int, int>(-1, -1);
	if (flag) {
		keyCursor = new QGraphicsLineItem(0, 0, 0, 0);
		this->addItem(keyCursor);
	}

	if (mode != TEXT_MODE && !flag) {
		timer->stop();
		if (keyCursor) {
			keyCursor->hide();
		}
	};
	radSpinBox->hide();
	redrawEditFields();
	redrawRTFields();
};


void RoboField::destroyRobot()
{
	if (robot) {
		removeItem(robot);
		delete robot;
		robot = NULL;
	}
}

void RoboField::destroyNet()
{

	for (int i = 0; i < setka.count(); i++) {
		this->removeItem(setka[i]);
	}
	setka.clear();
}

void RoboField::redrawEditFields()
{
	for (int i = 0; i < rows(); i++) { //Cikl po kletkam
		QList<FieldItm *> *row = &Items[i];
		for (int j = 0; j < columns(); j++) {

			if (mode == TEXT_MODE)
				row->at(j)->showCharFld(upLeftCorner(i, j).x(),
					upLeftCorner(i, j).y(), FIELD_SIZE_SMALL);
			else {
				row->at(j)->hideCharFld();
			}

		};
	};
};
void RoboField::redrawRTFields()
{
	for (int i = 0; i < rows(); i++) { //Cikl po kletkam
		QList<FieldItm *> *row = &Items[i];
		for (int j = 0; j < columns(); j++) {

			if (mode == RAD_MODE || mode == TEMP_MODE)
				row->at(j)->showRTItm(upLeftCorner(i, j).x(),
					upLeftCorner(i, j).y(), FIELD_SIZE_SMALL, mode);
			else {
				row->at(j)->hideRTItm();
			}

		};
	};
	update();
}

void RoboField::destroyScene()
{
	QList<QGraphicsItem *> items = this->items();
	while (items.count() > 0) {
		this->removeItem(items.first());
		items = this->items();
	}
	clickCell = QPair<int, int>(-1, -1);
}

void RoboField::debug()const
{
	QList<QGraphicsItem *> items = this->items();
	for (int i = 0; i < items.count(); i++) {
		qDebug() << "Scene debug" << items[i];
	}
}

void RoboField::createRobot()
{
	robot = new SimpleRobot();
	this->addItem(robot);
	robot->setPos(upLeftCorner(robo_y, robo_x).x(), upLeftCorner(robo_y, robo_x).y());
	connect(robot, SIGNAL(moved(QPointF)), this, SLOT(roboMoved(QPointF)));
}

void RoboField::reverseUpWall(int row, int col)
{
	if (!getFieldItem(row, col)->hasUpSep()) {
		return;
		qDebug("!UpSep");
	}
	if (!getFieldItem(row, col)->canUp()) {
		getFieldItem(row, col)->removeUpWall();
		qDebug("removeUp");
	} else {
		getFieldItem(row, col)->setUpWall(
			new QGraphicsLineItem(
				upLeftCorner(row, col).x(),
				upLeftCorner(row, col).y(),
				upLeftCorner(row, col).x() + fieldSize,
				upLeftCorner(row, col).y()
			),
			WallLine
		);
	}
	update();
	wasEdit = true;
}

void RoboField::reverseDownWall(int row, int col)
{
	if (!getFieldItem(row, col)->hasDownSep()) {
		return;
	}
	if (!getFieldItem(row, col)->canDown()) {
		getFieldItem(row, col)->removeDownWall();
	} else {
		getFieldItem(row, col)->setDownWall(
			new QGraphicsLineItem(
				upLeftCorner(row, col).x(),
				upLeftCorner(row, col).y() + fieldSize,
				upLeftCorner(row, col).x() + fieldSize,
				upLeftCorner(row, col).y() + fieldSize
			),
			WallLine
		);
	}
	update();
	wasEdit = true;
}

void RoboField::reverseLeftWall(int row, int col)
{
	if (!getFieldItem(row, col)->hasLeftSep()) {
		return;
	}
	if (!getFieldItem(row, col)->canLeft()) {
		getFieldItem(row, col)->removeLeftWall();
	} else {
		getFieldItem(row, col)->setLeftWall(
			new QGraphicsLineItem(
				upLeftCorner(row, col).x(),
				upLeftCorner(row, col).y(),
				upLeftCorner(row, col).x(),
				upLeftCorner(row, col).y() + fieldSize
			),
			WallLine
		);
	}
	wasEdit = true;
}

void RoboField::reverseRightWall(int row, int col)
{
	if (!getFieldItem(row, col)->hasRightSep()) {
		return;
	}
	if (!getFieldItem(row, col)->canRight()) {
		getFieldItem(row, col)->removeRightWall();
	} else {
		getFieldItem(row, col)->setRightWall(
			new QGraphicsLineItem(
				upLeftCorner(row, col).x() + fieldSize,
				upLeftCorner(row, col).y(),
				upLeftCorner(row, col).x() + fieldSize,
				upLeftCorner(row, col).y() + fieldSize
			),
			WallLine
		);
	}
	wasEdit = true;
}

void RoboField::showUpWall(int row, int col)
{
	this->removeItem(showWall);
	delete showWall;
	showWall = 0;

	if (mode > NEDIT_MODE) {
		showWall = new QGraphicsLineItem(0, 0, 0, 0);
		this->addItem(showWall);
		return;

	}
	showWall = new QGraphicsLineItem(
		upLeftCorner(row, col).x(),
		upLeftCorner(row, col).y(),
		upLeftCorner(row, col).x() + fieldSize,
		upLeftCorner(row, col).y()
	);
	showWall->setPen(ShowLine);
	showWall->setZValue(200);

	this->addItem(showWall);
	qDebug() << "Show up wall";
}

void RoboField::showDownWall(int row, int col)
{
	this->removeItem(showWall);
	delete showWall;
	if (mode > NEDIT_MODE) {
		showWall = new QGraphicsLineItem(0, 0, 0, 0);
		this->addItem(showWall);
		return;
	}
	showWall = new QGraphicsLineItem(
		upLeftCorner(row, col).x(),
		upLeftCorner(row, col).y() + fieldSize,
		upLeftCorner(row, col).x() + fieldSize,
		upLeftCorner(row, col).y() + fieldSize
	);
	showWall->setPen(ShowLine);
	showWall->setZValue(200);

	this->addItem(showWall);
}

void RoboField::showLeftWall(int row, int col)
{
	this->removeItem(showWall);
	delete showWall;
	showWall = 0;

	if (mode > NEDIT_MODE) {
		showWall = new QGraphicsLineItem(0, 0, 0, 0);
	} else {
		showWall = new QGraphicsLineItem(
			upLeftCorner(row, col).x(),
			upLeftCorner(row, col).y(),
			upLeftCorner(row, col).x(),
			upLeftCorner(row, col).y() + fieldSize
		);
		showWall->setZValue(200);
	}

	showWall->setPen(ShowLine);
	this->addItem(showWall);
}

void RoboField::showRightWall(int row, int col)
{
	this->removeItem(showWall);
	delete showWall;
	showWall = 0;

	if (mode > NEDIT_MODE) {
		showWall = new QGraphicsLineItem(0, 0, 0, 0);
		showWall->setPen(ShowLine);
		this->addItem(showWall);
		return;
	}

	showWall = new QGraphicsLineItem(
		upLeftCorner(row, col).x() + fieldSize,
		upLeftCorner(row, col).y(),
		upLeftCorner(row, col).x() + fieldSize,
		upLeftCorner(row, col).y() + fieldSize
	);
	showWall->setPen(ShowLine);
	showWall->setZValue(200);

	this->addItem(showWall);
}

void RoboField::reverseColor(int row, int col)
{
	if (getFieldItem(row, col)->isColored()) {
		getFieldItem(row, col)->removeColor();
	} else {
		getFieldItem(row, col)->setColorRect(
			new QGraphicsRectItem(
				upLeftCorner(row, col).x(), upLeftCorner(row, col).y(),
				fieldSize, fieldSize
			),
			FillColor
		);
	};
	if (mode != NORMAL_MODE) {
		wasEdit = true;
	}
}

void RoboField::reverseColorCurrent()
{
	reverseColor(robo_y, robo_x);
	if (mode != NORMAL_MODE) {
		wasEdit = true;
	}
}

void RoboField::reverseMark(int row, int col)
{
	if (getFieldItem(row, col)->mark) {
		getFieldItem(row, col)->removeMark();
	} else {
		getFieldItem(row, col)->mark = true;
		getFieldItem(row, col)->showCharMark(upLeftCorner(row, col).x(), upLeftCorner(row, col).y(), fieldSize);
	}
	wasEdit = true;
}

void RoboField::setRoboPos(int roboX, int roboY)
{
	robo_x = roboX;
	robo_y = roboY;
}

void RoboField::roboMoved(QPointF pos)
{
	int roboRow = pos.y() / FIELD_SIZE_SMALL;
	int roboCol = pos.x() / FIELD_SIZE_SMALL;
	if (roboRow >= rows()) {
		roboRow = rows() - 1;
	}
	if (roboRow < 0) {
		roboRow = 0;
	}
	if (roboCol >= columns()) {
		roboCol = columns() - 1;
	}
	if (roboCol < 0) {
		roboCol = 0;
	}
	robot->setPos(upLeftCorner(roboRow, roboCol).x(), upLeftCorner(roboRow, roboCol).y());
	robo_x = roboCol;
	robo_y = roboRow;
	wasEdit = true;
}

void RoboField::drawNet()
{
	//qDebug() << "BORT=" << BORT << ", FSS=" << FIELD_SIZE_SMALL;

	destroyNet();

	int ddx = 2 + FIELD_SIZE_SMALL / 2;
	QColor gridColor, bgColor;

	if (mode == NORMAL_MODE) {
		gridColor = LineColor;
		bgColor = NormalColor;
	} else {
		gridColor = EditLineColor;
		bgColor = EditColor;
	}

	this->setBackgroundBrush(QBrush(bgColor));
	QPen gridLine = QPen(gridColor, GridWidth);

	for (int i = -1; i < columns(); i++) { // Vertical lines
		setka.append(this->addLine(
			i * FIELD_SIZE_SMALL + ddx + FIELD_SIZE_SMALL / 2 + 2,
			-FIELD_SIZE_SMALL,
			i * FIELD_SIZE_SMALL + ddx + 2 + FIELD_SIZE_SMALL / 2,
			(rows() + 1) * FIELD_SIZE_SMALL,
			gridLine
		));
		setka.last()->setZValue(0.5);
	}

	for (int i = -1; i < rows(); i++) { // Horizontal lines
		setka.append(this->addLine(
			-FIELD_SIZE_SMALL,
			i * FIELD_SIZE_SMALL + ddx - 1 + FIELD_SIZE_SMALL / 2,
			(columns() + 1)* FIELD_SIZE_SMALL + 5,
			i * FIELD_SIZE_SMALL + ddx - 1 + FIELD_SIZE_SMALL / 2,
			gridLine
		));
		setka.last()->setZValue(0.5);
	}
}

qreal RoboField::m_height() const
{
	return FIELD_SIZE_SMALL *  rows();
}
qreal RoboField::m_width() const
{
	return FIELD_SIZE_SMALL *  rows();
}
/**
 * Загрузка обстановки из файла, отображение не производится.
 * @param fileName Имя файла
 * @return Код ошибки
 * 1 - Ошибка чтения файла
 * 2 - Пустой файл
 * 3 - Ошибка чтения размеров обстановки
 * 4 - Неверные размеры поля
 * 5 - Пустая строка
 * 6 - Неверное положение робота
 * 0< - Ошибка в строке
 * 10 - Ошибки основного прогона
 */
int RoboField::loadFromDataStream(QIODevice *l_File)
{
	QString tmp = "";
	wasEdit = false;
	int NStrok;
	NStrok = 0;
	QString l_String;
	//  long l_Err;
	int CurX, CurY;
	int SizeX, SizeY;
	destroyField();
	// Тестовый прогон




	// 1 stroka - razmery polya
	tmp = l_File->readLine();
	//QMessageBox::information( 0, "", tmp, 0,0,0);


	if (tmp.isNull() || tmp.isEmpty()) {
		l_File->close();

		return 2;
	}

	//QMessageBox::information( 0, "", tmp, 0,0,0);

	while (tmp.left(1) == ";" || tmp == "") {
		tmp = l_File->readLine();
		NStrok++;
		if (tmp.isNull()) {
			return 1;
		}
	}
	tmp = tmp.simplified();
	QStringList l_List = tmp.split(' ');

	if (l_List.count() != 2) {
		l_File->close();


		return 3;
	}

	SizeX = (l_List[0]).toInt();
	SizeY = (l_List[1]).toInt();

	if ((SizeX <= 0) || (SizeY <= 0)) {
		return 4;
	}
	//            field.destroyField();
	// field.createField(l_List[0].toInt(),l_List[1].toInt());

	// Вторая строка - положение робота

	tmp = l_File->readLine();



	if (tmp.isNull()) {
		l_File->close();

		return 5;
	}



	while (tmp.left(1) == ";" || tmp == "") {
		tmp = l_File->readLine();
		NStrok++;
		if (tmp.isNull()) {
			l_File->close();

			return 5;
		}
	}
	tmp = tmp.simplified();
	l_List = tmp.split(' ');

	// koordinaty robota
	// proverka
	if ((l_List[0]).toInt() < 0 || (l_List[1]).toInt() < 0) {


		l_File->close();
		return 6;
	}

	if ((l_List[0]).toInt() > SizeX || (l_List[1]).toInt() > SizeY) {

		l_File->close();
		return 6;
	}


	//  m_DefaultSett = l_Sett;

	while (!l_File->atEnd()) {
		//l_Err = l_File.readLine(l_String, 255);
		tmp = QString::fromUtf8(l_File->readLine());
		NStrok++;
		if (tmp.isNull()) {

			return 5;
		}
		if (tmp.left(1) == ";" || tmp == "") {
			continue;
		}
		tmp = tmp.simplified();
		l_List = tmp.split(' ');
		if (l_List.count() == 0) {
			continue;
		}

		if (l_List.count() > 9) {

			l_File->close();
			return -NStrok;
		}
		if (l_List.count() < 6) {
			l_File->close();
			qDebug("N Lexem<6");
			return -NStrok;
		};
		bool ok;
		CurX = l_List[0].toInt(&ok);
		if (!ok) {
			l_File->close();
			qDebug("Bad cur X<6");
			return -NStrok;
		};

		CurY = l_List[1].toInt(&ok);

		if (!ok) {
			l_File->close();
			qDebug("Bad curY <6");
			return -NStrok;
		};

		if (CurX < 0 || CurX > SizeX || CurY < 0 || CurY > SizeY) {

			l_File->close();
			return -NStrok;
		}

		if (l_List[4].toFloat() < 0) {

			l_File->close();
			return -NStrok;
		}


		if (l_List[5].toFloat() < MIN_TEMP) {

			l_File->close();
			return -NStrok;
		}


		if (l_List.count() >= 7) {

			QString tmp1 = l_List[6];
			//dlina lexemy dolzna ravnyatsa 1
			if (!(tmp1.length() == 1)) {

				l_File->close();
				return -NStrok;
			}


		}


		if (l_List.count() >= 8) {

			QString tmp1 = l_List[7];
			//dlina lexemy dolzna ravnyatsa 1
			if (!(tmp1.length() == 1)) {
				l_File->close();
				return -NStrok;
			}


		}

	}
	l_File->close();

	//реальный прогон
	//destroyField();

	if (!l_File->open(QIODevice::ReadOnly)) {

		return 10;
	}



	// 1 stroka - razmery polya
	tmp = l_File->readLine();

	if (tmp.isNull()) {
		l_File->close();

		return 10;
	}


	while (tmp.left(1) == ";" || tmp == "") {
		tmp = QString::fromUtf8(l_File->readLine());
		NStrok++;
		if (tmp.isNull()) {
			l_File->close();

			return 10;
		}
	}
	tmp = tmp.simplified();
	l_List = tmp.split(' ');

	if (l_List.count() != 2) {
		l_File->close();

		return -NStrok;
	}

	SizeX = (l_List[0]).toInt();
	SizeY = (l_List[1]).toInt();
	//   //NEW ROBO CODE
	createField(SizeY, SizeX);

	//END
	if ((l_List[0]).toInt() <= 0 || (l_List[1]).toInt() <= 0) {

		l_File->close();
		return - NStrok;
	}


	// Вторая строка - положение робота

	tmp = l_File->readLine();



	if (tmp.isNull()) {
		l_File->close();

		return 10;
	}



	while (tmp.left(1) == ";" || tmp == "") {
		tmp = l_File->readLine();
		NStrok++;
		if (tmp.isNull()) {
			l_File->close();

			return 10;
		}
	}
	tmp = tmp.simplified();
	l_List = tmp.split(' ');

	// koordinaty robota

	if ((l_List[0]).toInt() < 0 || (l_List[1]).toInt() < 0) {

		l_File->close();
		return - NStrok;
	}

	if ((l_List[0]).toInt() > SizeX || (l_List[1]).toInt() > SizeY) {

		l_File->close();
		return - NStrok;
	}

	robo_x = (l_List[0]).toInt();
	robo_y = (l_List[1]).toInt();

	//InitialX = m_x;
	//InitialY = m_y;




	//  delete []m_FieldDubl;



	while (!l_File->atEnd()) {
		tmp = QString::fromUtf8(l_File->readLine());
		NStrok++;
		if (tmp.isNull()) {

			l_File->close();
			return 10;
		}
		if (tmp.left(1) == ";" || tmp == "") {
			continue;
		}
		tmp = tmp.simplified();
		l_List = tmp.split(' ');
		if (l_List.count() == 0) {
			continue;
		}

		if (l_List.count() > 9) {

			l_File->close();
			return -NStrok;
		}
		CurX = l_List[0].toInt();
		CurY = l_List[1].toInt();
		if (CurX < 0 || CurX > SizeX || CurY < 0 || CurY > SizeY) {

			l_File->close();
			return -NStrok;
		}
		// TODO STENI
		if (getFieldItem(CurY, CurX)) {
			getFieldItem(CurY, CurX)->setWalls((l_List[2]).toInt());
		}

		//      int ix = (l_List[0]).toInt();
		//      int iy = (l_List[1]).toInt();

		if (!((l_List[3]).toInt() == 0)) {
			getFieldItem(CurY, CurX)->IsColored = true;
			// //QMessageBox::information( 0, "","test1" , 0,0,0);
		} else {
			if (getFieldItem(CurY, CurX)) {
				getFieldItem(CurY, CurX)->IsColored = false;
			}
		}
		qreal radiation = (l_List[4].replace(",", ".")).toDouble();
		if (getFieldItem(CurY, CurX)) {
			getFieldItem(CurY, CurX)->radiation = radiation;
		}

		if (l_List[4].toFloat() < 0) {

			l_File->close();
			return -NStrok;
		}
		qreal temperature = (l_List[5].replace(",", ".")).toDouble();
		if (getFieldItem(CurY, CurX)) {
			getFieldItem(CurY, CurX)->temperature = temperature;
		}

		if (l_List[5].toFloat() < MIN_TEMP) {

			l_File->close();
			return -NStrok;
		}



		if (l_List.count() >= 7) {

			QString tmp1 = l_List[6];
			//dlina lexemy dolzna ravnyatsa 1
			if (!(tmp1.length() == 1)) {

				l_File->close();
				return -NStrok;
			}
			//qDebug()<<QString::fromUtf8("Тест Up:")<<tmp1[0];
			if (tmp1[0] != '$') {
				if (getFieldItem(CurY, CurX)) {
					getFieldItem(CurY, CurX)->upChar = tmp1[0];
				}
			} else {
				if (getFieldItem(CurY, CurX)) {
					getFieldItem(CurY, CurX)->upChar = ' ' ;
				}
			}
		} else {
			if (getFieldItem(CurY, CurX)) {
				getFieldItem(CurY, CurX)->upChar = ' ' ;
			}
		}



		if (l_List.count() >= 8) {

			QString tmp1 = l_List[7];

			//dlina lexemy dolzna ravnyatsa 1
			if (!(tmp1.length() == 1)) {

				l_File->close();
				return -NStrok;
			}
			//qDebug()<<QString::fromUtf8("Тест Down:")<<tmp1[0];
			if (tmp1[0] != '$') {
				if (getFieldItem(CurY, CurX)) {
					getFieldItem(CurY, CurX)->downChar = tmp1[0];
				}
			} else {
				if (getFieldItem(CurY, CurX)) {
					getFieldItem(CurY, CurX)->downChar = ' ' ;
				}
			}
		} else {
			if (getFieldItem(CurY, CurX)) {
				getFieldItem(CurY, CurX)->downChar = ' ' ;
			}
		}



		if (l_List.count() >= 9) {

			QString tmp1 = l_List[8];
			//dlina lexemy dolzna ravnyatsa 1
			if (!(tmp1.length() == 1)) {

				l_File->close();
				return -NStrok;
			}
			if (tmp1[0] == '1') {
				if (getFieldItem(CurY, CurX)) {
					getFieldItem(CurY, CurX)->mark = true;
				}
			} else {
				if (getFieldItem(CurY, CurX)) {
					getFieldItem(CurY, CurX)->mark = false ;
				}
			}
		} else {
			if (getFieldItem(CurY, CurX)) {
				getFieldItem(CurY, CurX)->mark = false ;
			}
		}
	}

	l_File->close();

	wasEdit = false;
	return 0;
}

/**
 * Загрузка обстановки из файла, отображение не производится.
 * @param fileName Имя файла
 * @return Код ошибки
 * 1 - Ошибка чтения файла
 * 2 - Пустой файл
 * 3 - Ошибка чтения размеров обстановки
 * 4 - Неверные размеры поля
 * 5 - Пустя строка
 * 6 - Неверное положение робота
 * 0< - Ошибка в строке
 * 10 - Ошибки основного прогона
 */
int RoboField::loadFromFile(const QString &fileName)
{

	//destroyField();

	QFileInfo fi(fileName);
	QString name = fi.fileName();

	QString Title = QString::fromUtf8("Робот - ") + name;


	QString tmp = "";
	QString ctmp;


	QFile l_File(fileName);




	int NStrok;
	NStrok = 0;
	QString l_String;
	//  long l_Err;
	int CurX, CurY;
	int SizeX, SizeY;

	// Тестовый прогон

	if (!l_File.open(QIODevice::ReadOnly)) {

		return 1;
	}



	// 1 stroka - razmery polya
	tmp = l_File.readLine();
	//QMessageBox::information( 0, "", tmp, 0,0,0);


	if (tmp.isNull() || tmp.isEmpty()) {
		l_File.close();

		return 2;
	}

	//QMessageBox::information( 0, "", tmp, 0,0,0);

	while (tmp.left(1) == ";" || tmp == "") {
		tmp = l_File.readLine();
		NStrok++;
		if (tmp.isNull()) {
			return 1;
		}
	}
	tmp = tmp.simplified();
	QStringList l_List = tmp.split(' ');

	if (l_List.count() != 2) {
		l_File.close();


		return 3;
	}

	SizeX = (l_List[0]).toInt();
	SizeY = (l_List[1]).toInt();

	if ((SizeX <= 0) || (SizeY <= 0)) {
		return 4;
	}
	//            field.destroyField();
	// field.createField(l_List[0].toInt(),l_List[1].toInt());

	// Вторая строка - положение робота

	tmp = l_File.readLine();



	if (tmp.isNull()) {
		l_File.close();

		return 5;
	}



	while (tmp.left(1) == ";" || tmp == "") {
		tmp = l_File.readLine();
		NStrok++;
		if (tmp.isNull()) {
			l_File.close();

			return 5;
		}
	}
	tmp = tmp.simplified();
	l_List = tmp.split(' ');

	// koordinaty robota
	// proverka
	if ((l_List[0]).toInt() < 0 || (l_List[1]).toInt() < 0) {


		l_File.close();
		return 6;
	}

	if ((l_List[0]).toInt() > SizeX || (l_List[1]).toInt() > SizeY) {

		l_File.close();
		return 6;
	}


	//  m_DefaultSett = l_Sett;

	while (!l_File.atEnd()) {
		//l_Err = l_File.readLine(l_String, 255);
		tmp = QString::fromUtf8(l_File.readLine());
		NStrok++;
		if (tmp.isNull()) {

			return 5;
		}
		if (tmp.left(1) == ";" || tmp == "") {
			continue;
		}
		tmp = tmp.simplified();
		l_List = tmp.split(' ');
		if (l_List.count() == 0) {
			continue;
		}

		if (l_List.count() > 9) {

			l_File.close();
			return -NStrok;
		}
		if (l_List.count() < 6) {
			l_File.close();
			qDebug("N Lexem<6");
			return -NStrok;
		};
		bool ok;
		CurX = l_List[0].toInt(&ok);
		if (!ok) {
			l_File.close();
			qDebug("Bad cur X<6");
			return -NStrok;
		};

		CurY = l_List[1].toInt(&ok);

		if (!ok) {
			l_File.close();
			qDebug("Bad curY <6");
			return -NStrok;
		};

		if (CurX < 0 || CurX > SizeX || CurY < 0 || CurY > SizeY) {

			l_File.close();
			return -NStrok;
		}

		if (l_List[4].toFloat() < 0) {

			l_File.close();
			return -NStrok;
		}


		if (l_List[5].toFloat() < MIN_TEMP) {

			l_File.close();
			return -NStrok;
		}


		if (l_List.count() >= 7) {

			QString tmp1 = l_List[6];
			//dlina lexemy dolzna ravnyatsa 1
			if (!(tmp1.length() == 1)) {

				l_File.close();
				return -NStrok;
			}


		}


		if (l_List.count() >= 8) {

			QString tmp1 = l_List[7];
			//dlina lexemy dolzna ravnyatsa 1
			if (!(tmp1.length() == 1)) {
				l_File.close();
				return -NStrok;
			}


		}

	}
	l_File.close();
	destroyField();
	//реальный прогон
	//destroyField();

	if (!l_File.open(QIODevice::ReadOnly)) {

		return 10;
	}



	// 1 stroka - razmery polya
	tmp = l_File.readLine();

	if (tmp.isNull()) {
		l_File.close();

		return 10;
	}


	while (tmp.left(1) == ";" || tmp == "") {
		tmp = QString::fromUtf8(l_File.readLine());
		NStrok++;
		if (tmp.isNull()) {
			l_File.close();

			return 10;
		}
	}
	tmp = tmp.simplified();
	l_List = tmp.split(' ');

	if (l_List.count() != 2) {
		l_File.close();

		return -NStrok;
	}

	SizeX = (l_List[0]).toInt();
	SizeY = (l_List[1]).toInt();
	//   //NEW ROBO CODE
	createField(SizeY, SizeX);

	//END
	if ((l_List[0]).toInt() <= 0 || (l_List[1]).toInt() <= 0) {

		l_File.close();
		return - NStrok;
	}


	// Вторая строка - положение робота

	tmp = l_File.readLine();

	if (tmp.isNull()) {
		l_File.close();

		return 10;
	}

	while (tmp.left(1) == ";" || tmp == "") {
		tmp = l_File.readLine();
		NStrok++;
		if (tmp.isNull()) {
			l_File.close();

			return 10;
		}
	}
	tmp = tmp.simplified();
	l_List = tmp.split(' ');

	// koordinaty robota

	if ((l_List[0]).toInt() < 0 || (l_List[1]).toInt() < 0) {

		l_File.close();
		return - NStrok;
	}

	if ((l_List[0]).toInt() > SizeX || (l_List[1]).toInt() > SizeY) {

		l_File.close();
		return - NStrok;
	}

	robo_x = (l_List[0]).toInt();
	robo_y = (l_List[1]).toInt();

	while (!l_File.atEnd()) {
		tmp = QString::fromUtf8(l_File.readLine());
		NStrok++;
		if (tmp.isNull()) {

			l_File.close();
			return 10;
		}
		if (tmp.left(1) == ";" || tmp == "") {
			continue;
		}
		tmp = tmp.simplified();
		l_List = tmp.split(' ');
		if (l_List.count() == 0) {
			continue;
		}

		if (l_List.count() > 9) {

			l_File.close();
			return -NStrok;
		}
		CurX = l_List[0].toInt();
		CurY = l_List[1].toInt();
		if (CurX < 0 || CurX > SizeX || CurY < 0 || CurY > SizeY) {

			l_File.close();
			return -NStrok;
		}
		// TODO STENI
		if (getFieldItem(CurY, CurX)) {
			getFieldItem(CurY, CurX)->setWalls((l_List[2]).toInt());
		}

		//      int ix = (l_List[0]).toInt();
		//      int iy = (l_List[1]).toInt();

		if (!((l_List[3]).toInt() == 0)) {
			getFieldItem(CurY, CurX)->IsColored = true;
			// //QMessageBox::information( 0, "","test1" , 0,0,0);
		} else {
			if (getFieldItem(CurY, CurX)) {
				getFieldItem(CurY, CurX)->IsColored = false;
			}
		}
		qreal radiation = (l_List[4].replace(",", ".")).toDouble();
		if (getFieldItem(CurY, CurX)) {
			getFieldItem(CurY, CurX)->radiation = radiation;
		}

		if (l_List[4].toFloat() < 0) {

			l_File.close();
			return -NStrok;
		}
		qreal temperature = (l_List[5].replace(",", ".")).toDouble();
		if (getFieldItem(CurY, CurX)) {
			getFieldItem(CurY, CurX)->temperature = temperature;
		}

		if (l_List[5].toFloat() < MIN_TEMP) {

			l_File.close();
			return -NStrok;
		}



		if (l_List.count() >= 7) {

			QString tmp1 = l_List[6];
			//dlina lexemy dolzna ravnyatsa 1
			if (!(tmp1.length() == 1)) {

				l_File.close();
				return -NStrok;
			}
			//qDebug()<<QString::fromUtf8("Тест Up:")<<tmp1[0];
			if (tmp1[0] != '$') {
				if (getFieldItem(CurY, CurX)) {
					getFieldItem(CurY, CurX)->upChar = tmp1[0];
				}
			} else {
				if (getFieldItem(CurY, CurX)) {
					getFieldItem(CurY, CurX)->upChar = ' ' ;
				}
			}
		} else {
			if (getFieldItem(CurY, CurX)) {
				getFieldItem(CurY, CurX)->upChar = ' ' ;
			}
		}



		if (l_List.count() >= 8) {

			QString tmp1 = l_List[7];

			//dlina lexemy dolzna ravnyatsa 1
			if (!(tmp1.length() == 1)) {

				l_File.close();
				return -NStrok;
			}
			//qDebug()<<QString::fromUtf8("Тест Down:")<<tmp1[0];
			if (tmp1[0] != '$') {
				if (getFieldItem(CurY, CurX)) {
					getFieldItem(CurY, CurX)->downChar = tmp1[0];
				}
			} else {
				if (getFieldItem(CurY, CurX)) {
					getFieldItem(CurY, CurX)->downChar = ' ' ;
				}
			}
		} else {
			if (getFieldItem(CurY, CurX)) {
				getFieldItem(CurY, CurX)->downChar = ' ' ;
			}
		}



		if (l_List.count() >= 9) {

			QString tmp1 = l_List[8];
			//dlina lexemy dolzna ravnyatsa 1
			if (!(tmp1.length() == 1)) {

				l_File.close();
				return -NStrok;
			}
			if (tmp1[0] == '1') {
				if (getFieldItem(CurY, CurX)) {
					getFieldItem(CurY, CurX)->mark = true;
				}
			} else {
				if (getFieldItem(CurY, CurX)) {
					getFieldItem(CurY, CurX)->mark = false ;
				}
			}
		} else {
			if (getFieldItem(CurY, CurX)) {
				getFieldItem(CurY, CurX)->mark = false ;
			}
		}
	}

	l_File.close();

	qDebug() << "File " << fileName ;
	wasEdit = false;

	return 0;
}

int RoboField::saveToFile(QString fileName)
{
	QFileInfo fi(fileName);
	QString name = fi.fileName();

	qDebug() << "NewRobot Save file: " << fileName;
	//QString Title = QString::fromUtf8("Робот - ") + name;


	QChar Bukva;
	char ctmp[200];
	QFile l_File(fileName);
	if (!l_File.open(QIODevice::WriteOnly)) {
		return 1;
	}

	l_File.write("; Field Size: x, y\n");

	sprintf(ctmp, "%i %i\n", columns(), rows());
	l_File.write(ctmp);
	l_File.write("; Robot position: x, y\n");
	sprintf(ctmp, "%i %i\n", robo_x, robo_y);
	l_File.write(ctmp);
	l_File.write("; A set of special Fields: x, y, Wall, Color, Radiation, Temperature, Symbol, Symbol1, Point\n");

	for (int i = 0; i < rows(); i++) {
		for (int j = 0; j < columns(); j++) {

			Bukva = getFieldItem(i, j)->upChar;
			if (Bukva == ' ') {
				Bukva = '$';
			}
			qDebug() << QString::fromUtf8("Буква") << Bukva;
			QChar cc;
			cc = Bukva;


			Bukva = getFieldItem(i, j)->downChar;
			if (Bukva == ' ') {
				Bukva = '$';
				qDebug() << QString::fromUtf8("Буквы нет");
			} else {
				qDebug() << QString::fromUtf8("Буква ") << Bukva;
			}

			QChar cc1 = Bukva;


			char cc2[2];
			cc2[0] = 0;
			cc2[1] = 0;

			if (getFieldItem(i, j)->mark) {
				cc2[0] = '1';
			}
			//char cc = 'g';
			if (!getFieldItem(i, j)->emptyCell()) {
				qDebug() << " i:" << i << " j:" << j << " walls:" << getFieldItem(i, j)->wallByte();
				sprintf(ctmp, "%i %i %i %i %f %f ", j, i, getFieldItem(i, j)->wallByte(), getFieldItem(i, j)->IsColored, getFieldItem(i, j)->radiation, getFieldItem(i, j)->temperature);
				QString toWrite = QString(ctmp) + cc + " " + cc1 + " " + cc2 + "\n";
				l_File.write(toWrite.toUtf8());
				qDebug() << "ROBOT:WRITE 2 FILE";
			}
		}
	}
	l_File.write("; End Of File\n");

	l_File.close();
	if (fi.exists()) {
		//QMessageBox::information( 0, "", QString::fromUtf8("файл записан"), 0,0,0);

	} else {
		QMessageBox::information(0, "", QString::fromUtf8("Файл не записан - попробуйте снова!"), 0, 0, 0);
		return -1;
	}

	wasEdit = false;
	return 0;

}

QPointF RoboField::roboPosF() const
{
	if (robot) {
		return robot->scenePos() ;
	}
	return QPointF(0, 0);
}

void RoboField::setCrash(uint dir)
{
	assert (robot);
	robot->setCrash(dir);
}

void RoboField::finishMove(QPointF pos)
{
	assert (robot);

	if (robot->isMoving()) {
		roboMoved(pos);
		robot->setMoving(false);
	}
}

bool RoboField::stepUp()
{
	if (getFieldItem(robo_y, robo_x)->canUp()) {
		robot->setPos(QPointF(robot->pos().x(),
				robot->pos().y() - fieldSize));
		robo_y--;
		return true;
	} else {
		return false;
	}
}

bool RoboField::stepDown()
{
	if (getFieldItem(robo_y, robo_x)->canDown()) {
		robot->moveBy(0, fieldSize);
		robo_y++;
		return true;
	} else {
		return false;
	}
}

bool RoboField::stepLeft()
{
	if (getFieldItem(robo_y, robo_x)->canLeft()) {
		robot->setPos(QPointF(robot->pos().x() - fieldSize,
				robot->pos().y()));
		robo_x--;
		return true;
	} else {
		return false;
	}
}

bool RoboField::stepRight()
{
	if (getFieldItem(robo_y, robo_x)->canRight()) {
		robot->moveBy(fieldSize, 0);
		robo_x++;
		return true;
	} else {
		return false;
	}
}


/**
 * Обработка событий нажатий кнопок мыши, показ диалога редактирования *клетки
 * @param mouseEvent
 */
void RoboField::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	qDebug() << " Field Mouse event mode:" << mode;

	if (mode == 0) {
		pressed = true;
		oldRect = sceneRect();
		perssX = mouseEvent->pos().x();
		pressY = mouseEvent->pos().y();
		mouseEvent->accept();
		return;
	}

	//qDebug()<<" Edit mouse press event";
	QPointF scenePos = mouseEvent->scenePos();

	float rc = float(scenePos.y()) / float(fieldSize);
	if (rc < 0) {
		rc = -1;
	}

	float cc = float(scenePos.x() - 3) / float(fieldSize);
	if (cc < 0) {
		cc = -1;
	}

	int rowClicked = rc;
	int colClicked = cc;

	if (mode == TEXT_MODE) {
		if (rowClicked > rows() || colClicked > columns() || rowClicked < 0 || colClicked < 0) { //clik mimio polya
			return;
		}
		pressD = false;
		if (keyCursor) {
			this->removeItem(keyCursor);
		}
		clickCell = QPair<int, int>(rowClicked, colClicked);
		if ((scenePos.y() - rowClicked * fieldSize) > fieldSize / 2) {
			pressD = true;
			showCursorDown(rowClicked, colClicked);
		} else {
			showCursorUp(rowClicked, colClicked);
		}
		update();
		return;
	}
	if (mode == RAD_MODE) { //if radiation || temp edit mode
		if (rowClicked > rows() || colClicked > columns() || rowClicked < 0 || colClicked < 0) { //clik mimio polya
			return;
		}
		qDebug() << "RAD MODE CLick";
		if (clickCell != QPair<int, int>(rowClicked, colClicked)) {
			if (radSpinBox->isVisible())

			{
				clickCell = QPair<int, int>(rowClicked, colClicked);
				qDebug() << "SET F:" << clickCell.first << "SET SEC:" << clickCell.second;

				if (!(rowClicked >= rows() || colClicked >= columns() || rowClicked < 0 || colClicked < 0)) {
					getFieldItem(rowClicked, colClicked)->radiation = radSpinBox->value();
				}

			}
			redrawRTFields();
		}
		if (rowClicked >= rows() || colClicked >= columns() || rowClicked < 0 || colClicked < 0) { //clik mimio polya
			update();
			return;
		}


		QGraphicsView *view = views().first(); //current view
		update();
		view->repaint();
		return;
	}
	if (mode == TEMP_MODE) {
		qDebug() << "Temp MODE CLick";
		if (rowClicked >= rows() || colClicked >= columns() || rowClicked < 0 || colClicked < 0) { //clik mimio polya
			return;
		}
		if (tempSpinBox->isVisible())

		{
			clickCell = QPair<int, int>(rowClicked, colClicked);
			qDebug() << "SET F:" << clickCell.first << "SET SEC:" << clickCell.second;
			getFieldItem(rowClicked, colClicked)->temperature = tempSpinBox->value();
			update();
		}
		redrawRTFields();
		QGraphicsView *view = views().first();
		view->repaint();
		update();
		return;
	}

	bool left = false, right = false, up = false, down = false;
	int upD = fieldSize, downD = fieldSize, leftD = fieldSize, rightD = fieldSize;
	qreal delta_row = scenePos.y() - rowClicked * fieldSize;
	qreal delta_col = scenePos.x() - colClicked * fieldSize - 3;
	qDebug() << "MousePress event";
	if ((rowClicked == rows()) && (scenePos.x() > ((fieldSize * (columns()) / 2) - (wAddRow->size().height() + 3))) && (scenePos.x() < fieldSize * ((float)columns() / 2))) {
		addRow();
		return;
	}
	if ((rowClicked == rows())   && (scenePos.x() > fieldSize * ((float)columns() / 2))  && (scenePos.x() < fieldSize * ((float)columns() / 2) + wAddRow->size().height() + 3)) {
		remRow();
		return;
	}
	if (scenePos.x() > (fieldSize * ((float)columns()) + 7)  && scenePos.y() > ((fieldSize * (rows()) / 2) - (wAddRow->size().width() + 1))
		&& scenePos.x() < (fieldSize * ((float)columns()) + 7) + (fieldSize / 2) + 5 && scenePos.y() < ((fieldSize * (rows()) / 2) - (wAddRow->size().width() + 1)) + fieldSize) {
		addCol();
		return;
	}
	if (scenePos.x() > fieldSize * ((float)columns()) + 7 && scenePos.y() > fieldSize * (rows()) / 2 + 1 &&
		scenePos.x() < fieldSize * ((float)columns()) + 7 + (fieldSize / 2) + 5 && scenePos.y() < fieldSize * (rows()) / 2 + 1 + fieldSize) {
		remCol();
		return;

	}
	QGraphicsScene::mousePressEvent(mouseEvent);
	if (mouseEvent->isAccepted()) {
		return;
	}

	if ((rowClicked > rows() - 1) || (rowClicked < 0)) {
		mouseEvent->ignore();
		QGraphicsScene::mousePressEvent(mouseEvent);
		return;
	}

	if ((colClicked > columns() - 1) || (colClicked < 0)) {
		mouseEvent->ignore();
		QGraphicsScene::mousePressEvent(mouseEvent);
		return;
	}

	// qDebug()<<"ScenePos y "<<scenePos.y() <<" sceneposx"<<scenePos.x() << "rowClick:"<<rowClicked << " "<<" colClick:"<<colClicked <<
	//  " delta_row" <<delta_row << " delta_col" << delta_col;
	if (!NEDIT_MODE) {
		return;    //don't setup walls,and marks;
	}

	if (delta_row <= MAX_CLICK_DELTA) {
		up = true;
		upD = delta_row;
		qDebug("UP");
	}

	if (fieldSize - delta_row <= MAX_CLICK_DELTA) {
		down = true;
		downD = fieldSize - delta_row;
	}


	if (delta_col <= MAX_CLICK_DELTA) {
		left = true;
		leftD = delta_col;
	}

	if (fieldSize - delta_col <= MAX_CLICK_DELTA) {
		right = true;
		rightD = fieldSize - delta_col;
	}

	if (mouseEvent->modifiers() == Qt::ControlModifier) {
		markMode = false;
	} else {
		markMode = true;
	}

	//Углы клетки
	if ((left) && (up)) {
		if (upD < leftD) {
			left = false;
		} else  {
			up = false;
		}
	}

	if ((left) && (down)) {
		if (downD < leftD) {
			left = false;
		} else  {
			down = false;
		}
	}

	if ((right) && (up)) {
		if (upD < rightD) {
			right = false;
		} else  {
			up = false;
		}
	}

	if ((right) && (down)) {
		if (downD < rightD) {
			right = false;
		} else  {
			down = false;
		}
	}

	//Ставим стены
	if (up) {
		reverseUpWall(rowClicked, colClicked);
		qDebug("ReversUP");
	}
	if (down) {
		reverseDownWall(rowClicked, colClicked);
	}
	if (left) {
		reverseLeftWall(rowClicked, colClicked);
	}
	if (right) {
		reverseRightWall(rowClicked, colClicked);
	}

	if (!up && !down && !right && !left) {
		if (markMode) {
			reverseColor(rowClicked, colClicked); //Zakraska
			old_cell = QPair<int, int>(rowClicked, colClicked);
		} else { //Otmetit tochkoy
			reverseMark(rowClicked, colClicked);
			old_cell = QPair<int, int>(rowClicked, colClicked);
		}
	}
	update();
	QGraphicsScene::update();
	views().first()->update();
	views().first()->viewport()->update();
	wasEdit = true;
}

void RoboField::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	qDebug() << "FIELD RELEASE";
	QGraphicsScene::mouseReleaseEvent(mouseEvent);
	if (mouseEvent->isAccepted()) {
		qDebug() << "Field is ACC";
		return;
	}
	if (mode == 0) {
		pressed = true;
	}
}

void RoboField::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	// qDebug()<<"FIELD MOVE";
	if (mode == 0) {
		return;
	}
	if (mode != NEDIT_MODE) {
		return;
	}

	QGraphicsScene::mouseMoveEvent(mouseEvent);
	if (!pressed) {
		QPointF scenePos = mouseEvent->scenePos();
		int rowClicked = scenePos.y() / fieldSize; //clicked = mouse cur pos!!
		int colClicked = (scenePos.x() - 3) / fieldSize;

		bool left, right, up, down;
		left = false;
		right = false;
		up = false;
		down = false;
		int upD = fieldSize, downD = fieldSize, leftD = fieldSize, rightD = fieldSize;
		qreal delta_row = scenePos.y() - rowClicked * fieldSize;
		qreal delta_col = scenePos.x() - colClicked * fieldSize - 3;


		if ((rowClicked > rows() - 1) || (rowClicked < 0)) {
			mouseEvent->ignore();
			QGraphicsScene::mousePressEvent(mouseEvent);
			return;
		};

		if ((colClicked > columns() - 1) || (colClicked < 0)) {
			mouseEvent->ignore();
			QGraphicsScene::mousePressEvent(mouseEvent);
			return;
		};


		if (delta_row <= MAX_CLICK_DELTA) {
			up = true;
			upD = delta_row;
			qDebug("UP");
		};

		if (fieldSize - delta_row <= MAX_CLICK_DELTA) {
			down = true;
			downD = fieldSize - delta_row;
		};


		if (delta_col <= MAX_CLICK_DELTA) {
			left = true;
			leftD = delta_col;
		};

		if (fieldSize - delta_col <= MAX_CLICK_DELTA) {
			right = true;
			rightD = fieldSize - delta_col;
		};
		if (mouseEvent->modifiers() == Qt::ControlModifier) {
			markMode = false;
		} else {
			markMode = true;
		}

		//Углы клетки
		if ((left) && (up)) {
			if (upD < leftD) {
				left = false;
			} else  {
				up = false;
			}
		}
		if ((left) && (down)) {
			if (downD < leftD) {
				left = false;
			} else  {
				down = false;
			}
		}
		if ((right) && (up)) {
			if (upD < rightD) {
				right = false;
			} else  {
				up = false;
			}
		}
		if ((right) && (down)) {
			if (downD < rightD) {
				right = false;
			} else  {
				down = false;
			}
		}

		//Ставим стены
		if (up && markMode) {
			showUpWall(rowClicked, colClicked);
			qDebug("ShowUP");
		};
		if (down && markMode) {
			showDownWall(rowClicked, colClicked);
		}
		if (left && markMode) {
			showLeftWall(rowClicked, colClicked);
		}
		if (right && markMode) {
			showRightWall(rowClicked, colClicked);
		}
		if (!up && !down && !left && !right) {
			if ((mouseEvent->buttons() == Qt::LeftButton) && (old_cell != QPair<int, int>(rowClicked, colClicked) && (!robot->isMoving()))) {
				if (markMode) {

					reverseColor(rowClicked, colClicked); //Zakraska
					old_cell = QPair<int, int>(rowClicked, colClicked);
				} else { //Otmetit tochkoy
					reverseMark(rowClicked, colClicked);
					old_cell = QPair<int, int>(rowClicked, colClicked);
				}


			}
			// qDebug()<<"OldCell!newCell"<<  (old_cell!=QPair<int,int>(rowClicked, colClicked));
			showWall->setVisible(false);
		}

	}
}

void RoboField::keyPressEvent(QKeyEvent *keyEvent)
{
	qDebug() << "KEY PRESSD" << keyEvent->text();
	if (clickCell == QPair<int, int>(-1, 1)) {
		return;
	}

	redrawRTFields();

	if (!(mode == NEDIT_MODE || mode == TEXT_MODE)) {
		return;
	}

	QString text = keyEvent->text();
	if ( text.isNull() || text.isEmpty() || text == "\n") {
		return;
	}

	int rowP = clickCell.first, colP = clickCell.second;
	int ulx = upLeftCorner(rowP, colP).x();
	int uly = upLeftCorner(rowP, colP).y();

	FieldItm* cell = getFieldItem(rowP, colP);
	if (cell == 0) {
		return;
	}

	if (!pressD) { //Pressed up symbol
		cell->upChar = keyEvent->text().at(0);
		cell->showUpChar(ulx, uly, fieldSize);
	} else {
		cell->downChar = keyEvent->text().at(0);
		cell->showDownChar(ulx, uly, fieldSize);
	}
	wasEdit = true;
}

void RoboField::showCursorUp(int row, int col)
{
	timer->start(500);

	keyCursor = new QGraphicsLineItem(upLeftCorner(row, col).x() + 4, upLeftCorner(row, col).y() + 4, upLeftCorner(row, col).x() + 4, upLeftCorner(row, col).y() + 14);
	keyCursor->setPen(QPen(QColor(Qt::white)));
	keyCursor->setZValue(210);
	this->addItem(keyCursor);
}

void RoboField::showCursorDown(int row, int col)
{
	timer->start(500);

	if (this->items().indexOf(keyCursor) > -1) {
		this->removeItem(keyCursor);
	}

	keyCursor = new QGraphicsLineItem(upLeftCorner(row, col).x() + 4, upLeftCorner(row, col).y() + 18, upLeftCorner(row, col).x() + 4, upLeftCorner(row, col).y() + 29);
	keyCursor->setPen(QPen(QColor(Qt::white)));
	keyCursor->setZValue(210);
	this->addItem(keyCursor);
}

void RoboField::timerTic()
{
	if (mode != TEXT_MODE) {
		if (keyCursor) {
			keyCursor->hide();
		}
		timer->stop();
	}
	qDebug() << "TIK!";
	if (!keyCursor) {
		timer->stop();
		return;
	}
	timer->start(500);
	if (keyCursor->isVisible()) {
		keyCursor->hide();
	} else {
		keyCursor->show();
	}
}

void RoboField::setFieldItems(QList<QList<FieldItm *> > FieldItems)
{

	Items = QList< QList<FieldItm *> >();
	for (int i = 0; i < FieldItems.size(); i++) {
		QList<FieldItm *> row;
		for (int j = 0; j < FieldItems[i].size(); j++) {
			row << NULL;
		}
		Items << row;
	}
}

void RoboField::setItem(FieldItm *item, uint str, uint stlb)
{
	if (Items[str][stlb]) {
		delete Items[str][stlb];
	}
	Items[str][stlb] = item;
}

RoboField *RoboField::Clone() const
{
	RoboField *clone = new RoboField(0, Actor);
	clone->setFieldItems(Items);
	clone->robo_x = robo_x;
	clone->robo_y = robo_y;
	clone->setModeFlag(mode);
	for (int i = 0; i < rows(); i++) {
		for (int j = 0; j < columns(); j++) {
			clone->setItem(getFieldItem(i, j)->Copy(), i, j);
			if (j > 0) {
				clone->getFieldItem(i, j)->setLeftsepItem(clone->getFieldItem(i, j - 1));
			}
			if (i > 0) {
				clone->getFieldItem(i, j)->setUpsepItem(clone->getFieldItem(i - 1, j));
			}
		}
	}

	return clone;
}

} // ActorRobot namespace
