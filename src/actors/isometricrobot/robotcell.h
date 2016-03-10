#ifndef ROBOTCELL_H
#define ROBOTCELL_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif


#define LEFT_WALL 	0x1
#define RIGHT_WALL 	0x2
#define DOWN_WALL 	0x4
#define UP_WALL   	0x8

namespace Robot25D {

class CellGraphicsItem;

struct RobotCell {
    inline RobotCell() {
        painted = pointed = flag = false;
        wallUp = wallDown = wallLeft = wallRight = false;
        cellItem = NULL;
        pointItem = NULL;
        wallUpItem = wallDownItem = wallLeftItem = wallRightItem = NULL;
        paintState = 0;
    }
    bool painted;
    bool wallUp;
    bool wallDown;
    bool wallLeft;
    bool wallRight;
    bool pointed;
    bool flag;
    quint8 paintState;
    qreal baseZOrder;
    CellGraphicsItem *cellItem;

    QGraphicsItem *pointItem;

    QGraphicsItem *wallUpItem;
    QGraphicsItem *wallDownItem;
    QGraphicsItem *wallLeftItem;
    QGraphicsItem *wallRightItem;
};

}

#endif // ROBOTCELL_H
