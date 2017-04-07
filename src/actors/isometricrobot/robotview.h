#ifndef ROBOTVIEW_H
#define ROBOTVIEW_H

#include <QtCore>
#include <QtGui>

#include "robotcell.h"
#include "roboterrors.h"
#include "robotitem.h"
#include "sch_environment.h"
#include "robotmodel.h"

namespace Desktop {
class NewEnvironmentDialog;

}

namespace Robot25D {

class Plugin;

class RobotView :
        public QObject,
        public QGraphicsRectItem
{
    friend class RobotItem;
    friend class CellGraphicsItem;
    Q_OBJECT
public:
    enum EditMode {
        NoEdit,
        Walls,
        Robot,
        Paint,
        Points
    };
    RobotView(RobotModel * model, const QDir & imagesDir, QGraphicsItem *parent=NULL);
    void waitForAnimated();
    void prepareToDispose();
    void finishEvaluation();
    inline qreal baseZOrder(int x, int y) { return _model->cellAt(x, y).baseZOrder; }
    inline int robotSpeed() const { return _robotItem->speed(); }

    ~RobotView();
public slots:
    inline void setRobotSpeed(int msec) { _robotItem->setSpeed(msec); }

protected slots:
    void handleModelFieldChanged();

protected:
    void init();
    QRectF createEmptyCell(int x, int y, bool painted, bool pointed, bool isBorder);
    void updateCell(int x, int y, bool painted);
    QGraphicsItem* createHorizontalWall(int x, int y, qreal zOrder);
    QGraphicsItem* createVerticalWall(int x, int y, qreal zOrder);
    QGraphicsItem* createFlagItem(int x, int y, qreal zOrder);
    void createField();
    void createRobot();


    static QPen wallPen();
    static QBrush wallBrush();

    static qreal ZMax;
    static qreal SceneRotationAngle;
    static qreal SceneSlopeAngle;
    static qreal CellSize;
    static QColor UnpaintedColor;
    static QColor PaintedColor;
    static qreal CellBorderSize;
    static qreal WallWidth;
    static qreal WallHeight;
    static QColor WallColor;

protected slots:
    void handleRobotEvaluationFinised();

public slots:
    void setAnimated(bool v);
    bool isAnimated() const;
    void reset();

signals:
    void sync();

private /* fields */:
    RobotModel * _model;
    QList<QGraphicsItem*> _allItems;
    QPointF _offset;
    RobotItem *_robotItem;
    QImage _background;
    QImage _backgroundScaled;
    QList<QBrush> _grass;
    QDir _imagesDir;

};
}
#endif // ROBOTVIEW_H
