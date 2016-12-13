#include "cellgraphicsitem.h"
#include "robotview.h"
#include "robotcell.h"
#include "robotitem.h"
#include "util.h"

#define NO_WALL_OPACITY 0.5
#define WALL_OPACITY 0.8

namespace Robot25D {

CellGraphicsItem::CellGraphicsItem(const QPolygonF &poly,
                                   const QPolygonF &polySouth,
                                   const QPolygonF &polyEast,
                                   const QPolygonF &polyNorth,
                                   const QPolygonF &polyWest,
                                   bool editable,
                                   int cellX,
                                   int cellY,
                                   RobotView *view,
                                   QGraphicsItem *parent)
                                       : QGraphicsPolygonItem(poly, parent)
                                       , p_south(polySouth)
                                       , p_east(polyEast)
                                       , p_north(polyNorth)
                                       , p_west(polyWest)
                                       , i_cellX(cellX)
                                       , i_cellY(cellY)
                                       , m_view(view)
{


}



void CellGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsPolygonItem::paint(painter, option, widget);

}


}
