#ifndef SROBOT_H
#define SROBOT_H

#include <QGraphicsObject>

namespace ActorRobot {

enum CrashDir {
	NO_CRASH,
	UP_CRASH,
	DOWN_CRASH,
	LEFT_CRASH,
	RIGHT_CRASH
};

class SimpleRobot: public QGraphicsObject
{
	Q_OBJECT;

public:
	SimpleRobot(QGraphicsItem *parent = 0);
	QRectF boundingRect() const;

	void paint(
		QPainter *painter,
		const QStyleOptionGraphicsItem *option,
		QWidget *widget
	);

	void setCrash(uint dir);
	void move(QPoint point);

	bool isMoving() const { return moving; }
	void setMoving(bool flag) { moving = flag; }

signals:
	void moved(QPointF point);

protected:
	void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
	void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
	QGraphicsPolygonItem Robot;
	bool moving;
	uint crash;
};

} // ActorRobot namespace

#endif // SROBOT_H
