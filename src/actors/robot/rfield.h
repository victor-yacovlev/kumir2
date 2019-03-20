#ifndef RFIELD_H
#define RFIELD_H

#include <QGraphicsWidget>
#include <QGraphicsScene>
#include <QPen>
#include <QIcon>
class QToolButton;
class QSpinBox;
class QDoubleSpinBox;

#include <kumir2-libs/extensionsystem/settings.h>

namespace ActorRobot {

class EditLine;
class RobotModule;
class SimpleRobot;

class FieldItm: public QGraphicsWidget
{
	Q_OBJECT

public:
	FieldItm(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
	~FieldItm();

	FieldItm *Copy();
	void cleanSelf();

	void setUpsepItem(FieldItm *ItmUp);
	void setDownsepItem(FieldItm *ItmDown);
	void setLeftsepItem(FieldItm *ItmLeft);
	void setRightsepItem(FieldItm *ItmRight);

	void removeDownsepItem();
	void removeLeftsepItem();
	void removeRightsepItem();

	void setWalls(int wallByte);

	int wallByte();
	void showCharFld(qreal upLeftCornerX, qreal upLeftCornerY, int field_size);
	void hideCharFld();

	void showCharMark(qreal upLeftCornerX, qreal upLeftCornerY, int size);
	void showUpChar(qreal upLeftCornerX, qreal upLeftCornerY, int size);
	void showDownChar(qreal upLeftCornerX, qreal upLeftCornerY, int size);
	void showRTItm(qreal upLeftCornerX, qreal upLeftCornerY, int size, int mode);
	void hideRTItm();

	void showMark(qreal upLeftCornerX, qreal upLeftCornerY, int size);
	void setColorRect(QGraphicsRectItem *Rect, QColor color);

	void setUpLine(QGraphicsLineItem *Line, const QPen &pen);
	void setDownLine(QGraphicsLineItem *Line, const QPen &pen);
	void setLeftLine(QGraphicsLineItem *Line, const QPen &pen);
	void setRightLine(QGraphicsLineItem *Line, const QPen &pen);

	bool hasUpWall();
	bool hasDownWall();
	bool hasLeftWall();
	bool hasRightWall();

	bool canUp();
	bool canDown();
	bool canLeft();
	bool canRight();

	bool hasUpSep();
	bool hasDownSep();
	bool hasLeftSep();
	bool hasRightSep();

	QGraphicsLineItem *UpWallLine() const { return upWallLine; }
	QGraphicsLineItem *DownWallLine() const { return downWallLine; }
	QGraphicsLineItem *LeftWallLine() const { return leftWallLine; }
	QGraphicsLineItem *RightWallLine() const { return rightWallLine; }

	bool isColored() const { return IsColored; }

	void setUpWall(QGraphicsLineItem *Line, const QPen &pen);
	void setDownWall(QGraphicsLineItem *Line, const QPen &pen);
	void setLeftWall(QGraphicsLineItem *Line, const QPen &pen);
	void setRightWall(QGraphicsLineItem *Line, const QPen &pen);

	void removeUpWall();
	void removeDownWall();
	void removeLeftWall();
	void removeRightWall();

	void removeColor();
	void removeMark();
	void removeUpChar();
	void removeDownChar();

	bool emptyCell();
	void setScene(QGraphicsScene *scene);
	void wbWalls();
	void colorWalls();
	void setTextColor();

public:
	bool IsColored;
	float radiation;
	float temperature;
	QChar upChar;
	QChar downChar;
	bool mark;
	bool upWall;
	bool downWall;
	bool rightWall;
	bool leftWall;

private:
	ExtensionSystem::SettingsPtr sett;

	QFont font;
	QPen wallPen;
	QColor TextColor;
	QFont charFnt;

	QGraphicsScene *Scene;
	QGraphicsLineItem *upWallLine;
	QGraphicsLineItem *downWallLine;
	QGraphicsLineItem *leftWallLine;
	QGraphicsLineItem *rightWallLine;
	QGraphicsRectItem *ColorRect;

	QGraphicsTextItem *upCharItm;
	QGraphicsTextItem *downCharItm;
	QGraphicsTextItem *markItm;
	EditLine *radItm;
	EditLine *tempItm;
	QGraphicsRectItem *upCharFld;
	QGraphicsRectItem *downCharFld;

	FieldItm *sepItmUp;
	FieldItm *sepItmDown;
	FieldItm *sepItmLeft;
	FieldItm *sepItmRight;
};

class RoboField: public QGraphicsScene
{
	Q_OBJECT

public:
	RoboField(QWidget *parent, RobotModule *actor);
	RoboField *Clone() const;
	~RoboField();

public:
	void drawField(uint cellSize);
	void destroyField();
	void setTextEditMode(bool flag);
	void redrawEditFields();
	void redrawRTFields();
	void destroyRobot();

	int rows() const { return Items.count(); }
	int columns() const { return Items.isEmpty() ? 0 : Items.last().size(); }

	void drawNet();
	qreal m_height() const;
	qreal m_width() const;
	void setItem(FieldItm *item, uint str, uint stlb);
	void destroyNet();
	void destroyScene();
	void createField(int shirina, int visota);
	FieldItm *getFieldItem(int str, int stlb) const;
	void debug() const;
	void reloadSettings();
	int loadFromFile(const QString &fileName);
	int loadFromDataStream(QIODevice *l_File);
	int saveToFile(QString fileName);
	void createRobot();
//	void UpdateColors();
	void showCursorUp(int row, int col);
	void showCursorDown(int row, int col);

	/**
	 * Удаляет / ставит стену
	 * @param row ряд
	 * @param col столбец
	 *
	 */
	void reverseUpWall(int row, int col);
	void reverseDownWall(int row, int col);
	void reverseLeftWall(int row, int col);
	void reverseRightWall(int row, int col);

	/**
	 * Удаляет / ставит стену
	 * @param row ряд
	 * @param col столбец
	 *
	 */
	void showUpWall(int row, int col);
	void showDownWall(int row, int col);
	void showLeftWall(int row, int col);
	void showRightWall(int row, int col);

	void reverseColor(int row, int col);
	void reverseColorCurrent();

	void reverseMark(int row, int col);

	bool isEditMode() const { return mode > 0; }

	QList<QList<FieldItm * > > FieldItems() { return Items; }
	void setFieldItems(QList<QList<FieldItm * > > FieldItems);

	QPointF roboPosF() const;
	void setCrash(uint dir);
	void finishMove(QPointF pos);

	bool stepUp();
	bool stepDown();
	bool stepLeft();
	bool stepRight();

	void editField();

	void setMode(int Mode); //swich mode (show/hide controls etc.)
	void setModeFlag(int Mode) { mode = Mode; }

	FieldItm *currentCell() const
	{
		return getFieldItem(robo_y, robo_x);
	}

	FieldItm *cellAt(int x, int y) const
	{
		return getFieldItem(x, y);
	}

	void setMarkMode(bool isColor) { markMode = isColor; }

	bool WasEdit() const { return wasEdit; }
	void setRoboPos(int roboX, int roboY);

	int robotX() const { return robo_x; }
	int robotY() const { return robo_y; }


	void dropWasEdit() { wasEdit = false; }
#if 0
	void wbMode();
	void colorMode();
#endif
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

public slots:
	void roboMoved(QPointF pos);
//	void cellDialogOk();
	void timerTic();
	void addRow();
	void remRow();
	void remCol();
	void addCol();

signals:
	void MousePress(qreal x, qreal y, bool Flag);

private:
	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void keyPressEvent(QKeyEvent *keyEvent);
	void showButtons(bool yes);
	void createResizeButtons();

	SimpleRobot *robot;
	QTimer *timer;
	QList<QList<FieldItm * > > Items;
	QList<QGraphicsLineItem *> setka;
	bool markMode, wasEdit;
	QColor LineColor, EditLineColor, NormalColor, EditColor;
	QColor WallColor, FillColor, TextColor;
	uint fieldSize;
	uint robo_x, robo_y;
	QWidget *Parent;
	QPen BortLine, WallLine, ShowLine;
	QIcon PlusIcon, MinusIcon;
	int LetterShift, MarkShift, MarkShiftLeft;
	int BortWidth, WallWidth, GridWidth;
	bool WasEditFlag;

	//TOOLS
	QPoint upLeftCorner(int str, int stlb);
	bool pressed;
	ExtensionSystem::SettingsPtr sett;
	QRectF  oldRect;
	qreal perssX, pressY;
	QGraphicsLineItem *showWall, *keyCursor;
	QPair<int, int> old_cell, clickCell;
	bool pressD;
	QDoubleSpinBox *radSpinBox;
	QSpinBox *tempSpinBox;
	QGraphicsProxyWidget *wAddRow, *wAddCol, *wRemCol, *wRemRow;
	QToolButton *btnAddRow, *btnAddCol, *btnRemCol, *btnRemRow;
	int mode;
	RobotModule *Actor;
};

} // ActorRobot namespace

#endif // RFIELD_H

