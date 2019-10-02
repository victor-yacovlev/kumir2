#ifndef EDITLINE_H
#define EDITLINE_H

#include <QGraphicsObject>
#include <QDir>
#include <QImage>
#include <QPicture>

namespace ActorRobot {

class EditLine: public QGraphicsObject
{
	Q_OBJECT;
public:
	EditLine(QGraphicsItem *parent = 0, bool _istemp = false);
	QRectF boundingRect() const;

	void setValue(float _value) { value = _value; }

	void paint(
		QPainter *painter,
		const QStyleOptionGraphicsItem *option,
		QWidget *widget
	);

private:
	float value;
	bool istemp;

	QDir resDir;
	QImage rad;
	QPicture radiation;

	void setRad();
	void setTemp();
};

} // ActorRobot namespace

#endif // EDITLINE_H
