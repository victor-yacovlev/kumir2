#ifndef ROBOTVIEW_H
#define ROBOTVIEW_H

#include <QGraphicsView>
class QToolButton;

#include <kumir2-libs/extensionsystem/settings.h>

namespace ActorRobot
{

static const int FIELD_SIZE_SMALL = 33;

class RoboField;

enum RMode {
	NORMAL_MODE,
	NEDIT_MODE,
	TEMP_MODE,
	RAD_MODE,
	TEXT_MODE
};


class RobotView: public QGraphicsView
{
	Q_OBJECT

public:
	RobotView(RoboField *roboField);
	void  FindRobot();
	void showButtons(bool flag);
	QSize  sizeHint() const;

	void setField(RoboField *field)
	{
		robotField = field;
	}
	void setWindowSize(const QSize newGeometry);

public slots:
	void handleDocked();
	void changeEditMode(bool state);
	void setDock(bool);
	void reloadSett(ExtensionSystem::SettingsPtr settings);

signals:
	void resizeRequest(const QSize &newGeometry);

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	void showEvent(QShowEvent *event);

private:
	bool pressed, inDock, firstShow;
	int pressX, pressY;
	RoboField *robotField;
	QToolButton *textEditBtn;
	QToolButton *radEditBtn;
	QToolButton *tmpEditBtn;
	double c_scale;
	int CurCellSize;
};

} // ActorRobot

#endif // ROBOTVIEW_H
