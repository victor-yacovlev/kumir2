#include "rfield.h"
#include "cfield.h"
#include "robotview.h"
#include "robotmodule.h"

namespace ActorRobot {

RobotView::RobotView(RoboField *roboField)
{
	setScene(roboField);
	pressed = false;
	inDock = true;
	firstShow = true;
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setMouseTracking(true);
	setCursor(Qt::OpenHandCursor);
	robotField = roboField;
	textEditBtn = new QToolButton(this);
	textEditBtn->hide();
	textEditBtn->setCheckable(true);
	textEditBtn->setIcon(QIcon(RobotModule::self->myResourcesDir()
		.absoluteFilePath("text.png")));

	radEditBtn = new QToolButton(this);
	radEditBtn->setIcon(QIcon(RobotModule::self->myResourcesDir()
		.absoluteFilePath("btn_radiation.png")));

	radEditBtn->hide();
	radEditBtn->setCheckable(true);
	radEditBtn->move(textEditBtn->height(), 0);
	tmpEditBtn = new QToolButton(this);
	tmpEditBtn->hide();
	tmpEditBtn->setCheckable(true);
	tmpEditBtn->setIcon(QIcon(RobotModule::self->myResourcesDir()
		.absoluteFilePath("btn_temperature.png")));
	tmpEditBtn->move(textEditBtn->height() * 2 + 2, 0);

	connect(textEditBtn, SIGNAL(toggled(bool)), this, SLOT(changeEditMode(bool)));
	connect(radEditBtn, SIGNAL(toggled(bool)), this, SLOT(changeEditMode(bool)));
	connect(tmpEditBtn, SIGNAL(toggled(bool)), this, SLOT(changeEditMode(bool)));
	setMinimumWidth(30);
	c_scale = 1;
	CurCellSize = FIELD_SIZE_SMALL;
}

void RobotView::showButtons(bool flag)
{
	textEditBtn->setVisible(flag);
	radEditBtn->setVisible(flag);
	tmpEditBtn->setVisible(flag);
}

void RobotView::mousePressEvent(QMouseEvent *event)
{
	if (robotField->isEditMode()) {
		qDebug() << "Edit mode;";
		QGraphicsView::mousePressEvent(event);
		update();
		repaint();
		qApp->processEvents();
		return;
	}

	if (
		robotField->sceneRect().height() * c_scale > this->height() ||
		robotField->sceneRect().width() * c_scale > this->width()
	) { //field > view
		pressed = true;
		setCursor(Qt::ClosedHandCursor);
	}
	pressX = event->pos().x();
	pressY = event->pos().y();

}

void RobotView::mouseReleaseEvent(QMouseEvent *event)
{
	repaint();
	update();
	if (robotField->isEditMode()) {
		robotField->finishMove(this->mapToScene(event->pos()));
		qDebug() << "Mouse Release EV (RobotView)";
		return;
	}
	pressed = false;
	setCursor(Qt::OpenHandCursor);
}

void RobotView::mouseMoveEvent(QMouseEvent *event)
{
	if (robotField->isEditMode()) {
		setCursor(Qt::ArrowCursor);
		QGraphicsView::mouseMoveEvent(event);
		return;
	}

	setCursor(Qt::ArrowCursor);
	if (robotField->sceneRect().height() > this->height()  || robotField->sceneRect().width() > this->width()) { //field size more then view size
		setCursor(Qt::OpenHandCursor);
	}

	if (!pressed)
		return;

	setCursor(Qt::ClosedHandCursor);
	QPointF center = mapToScene(viewport()->rect().center());
	qDebug() << "==========Move==========";
	qDebug() << "Old center:" << center;
	qreal diffX = pressX - event->pos().x();
	qreal diffY = pressY - event->pos().y();

	if (diffX == 0 && diffY == 0)
		return;

	center.setX(center.x() + diffX);
	center.setY(center.y() + diffY);
	qDebug() << "New center:" << center << " DiffX" << diffX;

	if (diffX < width() && diffY < height()) {
		centerOn(center);
	}
}

void RobotView::wheelEvent(QWheelEvent *event)
{
	float numDegrees = event->delta() / 8;
	qDebug() << "wheel: " << numDegrees;
	qDebug() << "Scale: " << c_scale;

	double n_scale = c_scale;

	if (numDegrees > 0) {
		n_scale *= 1.25;
	} else {
		n_scale *= 0.8;
	}

	if (0.1 <= n_scale && n_scale <= 10 && n_scale != c_scale) {
		double r = n_scale / c_scale;
		this->scale(r, r);
		c_scale = n_scale;
	}
}

void RobotView::showEvent(QShowEvent *event)
{
	Q_UNUSED(event);
	if (firstShow)
		setWindowSize(size());
	firstShow = false;
}

void RobotView:: FindRobot()
{
	centerOn(robotField->roboPosF());
}

void RobotView::setWindowSize(const QSize newGeometry)
{
	QSize oldSize = this->size();

	if (oldSize.width() == 0) {
		oldSize.setWidth(1);
	}

	if (oldSize.height() == 0) {
		oldSize.setHeight(1);
	}

	if (inDock) {
		qDebug() << "IN DOCK";
		scale(1 / c_scale, 1 / c_scale);

		qreal w = sceneRect().width();
		qreal h = sceneRect().height();
		qreal maxDisp = qMax(w / oldSize.width(), h / oldSize.height());

		scale(1 / maxDisp, 1 / maxDisp);
		c_scale = 1 / maxDisp;
		maxDisp = qMax(
			w * (1 / maxDisp) / oldSize.width(),
			h * (1 / maxDisp) / oldSize.height()
		);
		while (maxDisp > 1.01 || maxDisp < 0.99) {
			scale(1 / maxDisp, 1 / maxDisp);
			c_scale = c_scale / maxDisp;
			maxDisp = qMax(
				w * (1 / maxDisp) / oldSize.width(),
				h * (1 / maxDisp) / oldSize.height()
			);
		}
		centerOn(sceneRect().width() / 2, sceneRect().height());
		return;
	}

	emit  resizeRequest(newGeometry);
	if (newGeometry != oldSize) {
		centerOn(
			newGeometry.width() / 2 - (CurCellSize / 2),
			newGeometry.height() / 2 - (CurCellSize / 2)
		);
		qDebug() << "CenterON:" << newGeometry.width() / 2 - CurCellSize / 2 << newGeometry.width() / 2 - CurCellSize / 2;
		this->scale(1 / c_scale, 1 / c_scale);
		c_scale = 1;
	}
}

QSize RobotView::sizeHint() const
{
	return QSize(
		robotField->columns() * CurCellSize + CurCellSize,
		robotField->rows()    * CurCellSize + CurCellSize
	);
}

void RobotView::handleDocked()
{
	qDebug() << "Robot is visible in dock. Size = " << size();
	setWindowSize(size());
}

void RobotView::reloadSett(ExtensionSystem::SettingsPtr settings)
{
	CurCellSize = settings->value("Robot/CellSize", FIELD_SIZE_SMALL).toInt();

}

void RobotView::setDock(bool docked)
{
	qDebug() << "RobotView::setDock(" << docked << ")";
	inDock = docked;
	if (inDock)
		setWindowSize(size());
}

void RobotView::changeEditMode(bool state)
{
	QToolButton *clicked = qobject_cast<QToolButton *>(sender());

	if (radEditBtn != clicked && radEditBtn->isChecked()) {
		radEditBtn->setChecked(false);
	}

	if (textEditBtn != clicked && textEditBtn->isChecked()) {
		textEditBtn->setChecked(false);
	}

	if (tmpEditBtn != clicked && tmpEditBtn->isChecked()) {
		tmpEditBtn->setChecked(false);
	}

	if (clicked->isChecked() != state) {
		clicked->setChecked(state);
	}

	if (!textEditBtn->isChecked() && !radEditBtn->isChecked()) {
		robotField->setMode(NEDIT_MODE);
	}

	if (textEditBtn->isChecked()) {
		robotField->setMode(TEXT_MODE);
		update();
	}

	if (radEditBtn->isChecked()) {
		robotField->setMode(RAD_MODE);
		repaint();
		update();
	}

	if (tmpEditBtn->isChecked()) {
		robotField->setMode(TEMP_MODE);
		repaint();
		update();
	}

	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

} // namespace ActorRobot
