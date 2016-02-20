#ifndef ROBOTVIEW_H
#define ROBOTVIEW_H

#include <QtCore>
#include <QtGui>

#include "robotcell.h"
#include "roboterrors.h"
#include "robotitem.h"
#include "sch_environment.h"

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
    Q_OBJECT;
    Q_PROPERTY(quint16 unpaintedPoints READ unpaintedPoints);
public:
    enum EditMode {
        NoEdit,
        Walls,
        Robot,
        Paint,
        Points
    };
    RobotView(const QDir & imagesDir, bool withConrols, bool withBackground,
              bool teacherMode,
              const QSize &minSize, QGraphicsItem *parent=NULL);
    inline void waitForAnimated() { m_robotItem->waitForAnimated(); }
    inline void setTeacherMode(bool v) { b_teacherMode =v; }
    quint16 unpaintedPoints() const;
    void prepareToDispose();
    void finishEvaluation();
    inline qreal baseZOrder(int x, int y) { return m_field[y][x].baseZOrder; }
    inline int robotSpeed() const { return m_robotItem->speed(); }
    QString lastError(QLocale::Language language) const;

    ~RobotView();
public slots:
        void evaluateCommand(const QString &englishName);
    void setLoadingMode(bool v);
    void setLoadingState(qreal v);
    inline void setRobotSpeed(int msec) { m_robotItem->setSpeed(msec); }

protected:
    void init();
    QRectF createEmptyCell(int x, int y, bool painted, bool pointed, bool isBorder);
    void updateCell(int x, int y, bool painted);
    QGraphicsItem* createHorizontalWall(int x, int y, qreal zOrder);
    QGraphicsItem* createVerticalWall(int x, int y, qreal zOrder);
    QGraphicsItem* createFlagItem(int x, int y, qreal zOrder);
    void createField();
    void createRobot(int x, int y, RobotItem::Direction direction);


    static QPen wallPen();
    static QBrush wallBrush();
    static qreal m_zMax;
    static qreal m_sceneRotationAngle;
    static qreal m_sceneSlopeAngle;
    static qreal m_cellSize;
    static QColor m_unpaintedColor;
    static QColor m_paintedColor;
    static qreal m_cellBorderSize;
    static qreal m_wallWidth;
    static qreal m_wallHeight;
    static QColor m_wallColor;


    QVector< QVector<RobotCell> > m_field;
    QVector< QVector<RobotCell> > m_originalField;

    QList<QGraphicsItem*> l_allItems;
    QPointF p_offset;

    RobotItem *m_robotItem;
    Robot25D::RuntimeError e_lastError;

    bool b_teacherMode;

    QImage px_background;
    QImage px_backgroundScaled;


    bool b_withControls;

    Point2Di m_originalRobotPosition;
    RobotItem::Direction m_originalRobotDirection;

    QList<QBrush> lbr_grass;

//    QGraphicsScene *m_scene;

    bool b_incrSpeedPressed;
    bool b_decrSpeedPressed;
    bool b_mousePressed;


    bool b_loadingMode;
    qreal r_loadingState;
    QDir imagesDir_;





//    virtual void mouseMoveEvent(QMouseEvent *event);
//    virtual void mousePressEvent(QMouseEvent *event);
//    virtual void mouseReleaseEvent(QMouseEvent *event);
//    virtual void resizeEvent(QResizeEvent *event);
//    virtual void paintEvent(QPaintEvent *event);

//    void setProperMouseCursor();


    QPoint m_lastMousePoint;

protected slots:
    void moveRobot(int x, int y);
    void handleRobotEvaluationFinised();

    /* Robot actor methods */
public slots:
    inline void setAnimated(bool v) { m_robotItem->setAnimated(v); }
    inline bool isAnimated() const { return m_robotItem->animated(); }
    bool goForward();
    void turnLeft();
    void turnRight();
    void doPaint();
    bool isWall();
    bool isPainted();
    bool isPainted(int x, int y) const;
    bool isPointed(int x, int y) const;
    bool isFlagged(int x, int y) const;
    int sizeX() const;
    int sizeY() const;
    int positionX() const;
    int positionY() const;
    void reset();

    bool loadEnvironment(const Schema::Environment &env);

    Schema::Environment environment() const;

signals:
    void sync();



};
}
#endif // ROBOTVIEW_H
