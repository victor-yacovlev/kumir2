#include "srobot.h"
#include <QGraphicsSceneEvent>
#include <QPainter>
#include <QMimeData>
#include <QDebug>

namespace ActorRobot {

SimpleRobot::SimpleRobot(QGraphicsItem *parent): QGraphicsObject(parent)
{
	Q_UNUSED(parent);
	setAcceptDrops(true);

	static const int points[] = {  14 + 2, 6 + 3, 22 + 3, 14 + 2, 14 + 3, 22 + 3, 6 + 3, 14 + 3 };
	QPolygon polygon;
	polygon.setPoints(4, points);
	QPolygonF polygonf = QPolygonF(polygon);
	Robot.setPolygon(polygonf);
	Robot.setZValue(100);
	setZValue(100);
	crash = NO_CRASH;
	moving = false;
}

void SimpleRobot::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
	event->setAccepted(event->mimeData()->hasFormat("text/plain"));
	qDebug("Robo dragEnter");
}

void SimpleRobot::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
	event->accept();
	qDebug("Robo dragMove");
}

void SimpleRobot::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	moving = true;
	qDebug() << "Robo mousePress" << event->pos();
}

void SimpleRobot::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	qDebug("Mouse move srobot");
	setPos(event->scenePos());

}

void SimpleRobot::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	qDebug() << "Mouse release SROBOT" << event->pos();
}

QRectF SimpleRobot::boundingRect() const
{
	return Robot.boundingRect();
}


void SimpleRobot::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	// Body
	painter->setBrush(QColor("lightgray"));
	painter->setPen(QPen("black"));

	static const int points[] = {  14 + 3, 6 + 3, 22 + 3, 14 + 3, 14 + 3, 22 + 3, 6 + 3, 14 + 3 };
	QVector<QPointF> up_crash, down_crash, left_crash, right_crash ;
	up_crash << QPointF(14 + 3, 6 + 3) << QPointF(13, 14) <<  QPointF(19 + 3, 11 + 3);
	down_crash << QPointF(14 + 3, 22 + 3) << QPointF(11 + 3, 18 + 3) <<  QPointF(18 + 3, 18 + 3);
	left_crash << QPointF(7 + 3, 14 + 3) << QPointF(11 + 3, 10 + 3) <<  QPointF(11 + 3, 18 + 3);
	right_crash << QPointF(22 + 3, 14 + 3) << QPointF(18 + 3, 10 + 3) <<  QPointF(18 + 3, 18 + 3);
	QPolygon polygon;
	polygon.setPoints(4, points);
	QPolygonF polygonf = QPolygonF(polygon);

	painter->drawPolygon(polygonf);
	if (crash != NO_CRASH) {
		qDebug("crash!");
		painter->setPen(Qt::NoPen);
		painter->setBrush(QColor("red"));
		if (crash == UP_CRASH) {
			painter->drawPolygon(QPolygonF(up_crash));
		}
		if (crash == DOWN_CRASH) {
			painter->drawPolygon(QPolygonF(down_crash));
		}
		if (crash == LEFT_CRASH) {
			painter->drawPolygon(QPolygonF(left_crash));
		}
		if (crash == RIGHT_CRASH) {
			painter->drawPolygon(QPolygonF(right_crash));
		}
	}
}

void SimpleRobot::setCrash(uint dir)
{
	crash = dir;
	update();
}

} // ActorRobot namespace

