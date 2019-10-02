#include "editline.h"
#include "robotmodule.h"
#include "robotview.h"
#include <QPainter>
#include <QPen>
#include <QUrl>

namespace ActorRobot {

EditLine::EditLine(QGraphicsItem *parent, bool _istemp) :
	value(0),
	istemp(_istemp),
	resDir(RobotModule::self->myResourcesDir())
{
	Q_UNUSED(parent);
	if (istemp)
		setTemp();
	else
		setRad();

	QPainter pntr;
	pntr.begin(&radiation);
	pntr.end();
}

void EditLine::paint(
	QPainter *painter,
	const QStyleOptionGraphicsItem *option,
	QWidget *widget
) {
	Q_UNUSED(option);
	Q_UNUSED(widget);

	if (istemp) {
		painter->setPen(QPen(Qt::white));
	} else {
		painter->setPen(QPen(Qt::yellow));
	}

	QFont sansFont("Helvetica [Cronyx]", 12);
	sansFont.setPixelSize(FIELD_SIZE_SMALL * 1.2);
	painter->setFont(sansFont);
	painter->drawText(
		4, FIELD_SIZE_SMALL + 4,
		FIELD_SIZE_SMALL * 3, FIELD_SIZE_SMALL * 1.5,
		0, QString::number(value)
	);
}

QRectF EditLine::boundingRect() const
{
	return QRectF(0, 0, FIELD_SIZE_SMALL / 4, FIELD_SIZE_SMALL / 4);
}

void EditLine::setRad()
{
	istemp = false;
	QUrl iconPath = QUrl::fromLocalFile(resDir.absoluteFilePath("btn_radiation.png"));
	rad = QImage(iconPath.toLocalFile());
}

void EditLine::setTemp()
{
	istemp = true;
	QUrl iconPath = QUrl::fromLocalFile(resDir.absoluteFilePath("temp.png"));
	rad = QImage(iconPath.toLocalFile());
}

} // ActorRobot namespace
