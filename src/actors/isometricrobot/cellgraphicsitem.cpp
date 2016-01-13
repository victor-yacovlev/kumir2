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
                                       , b_editable(editable)
                                       , i_cellX(cellX)
                                       , i_cellY(cellY)
                                       , m_view(view)
                                       , b_hoveredCell(false)
                                       , b_pressed(false)
{
    if (i_cellX<0 || i_cellY<0)
        b_editable = false;
    if (i_cellX>=m_view->m_field[0].size())
        b_editable = false;
    if (i_cellY>=m_view->m_field.size())
        b_editable = false;
    setAcceptHoverEvents(b_editable);

}



void CellGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsPolygonItem::paint(painter, option, widget);
    if (b_hoveredCell) {
        painter->save();
        painter->setBrush(QColor(0,0,0,128));
        painter->drawPolygon(polygon());
        painter->restore();
    }
//    if (i_hoveredWall) {
//        painter->save();
//        painter->setBrush(QColor(0,0,0,128));
//        if (i_hoveredWall==DOWN_WALL) {
//            painter->drawPolygon(p_south);
//        }
//        else if (i_hoveredWall==RIGHT_WALL) {
//            painter->drawPolygon(p_east);
//        }
//        else if (i_hoveredWall==UP_WALL) {
//            painter->drawPolygon(p_north);
//        }
//        else if (i_hoveredWall==LEFT_WALL) {
//            painter->drawPolygon(p_west);
//        }
//        painter->restore();
//    }
}

int CellGraphicsItem::whichWall(const QPointF &p) const
{
    qDebug() << "Which wall at " << p << " ?";
    Qt::FillRule rule = Qt::WindingFill;
    int result = 0;
    if (p_south.containsPoint(p, rule)) {
        result = DOWN_WALL;
    }
    else if (p_east.containsPoint(p, rule)) {
        result = RIGHT_WALL;
    }
    else if (p_north.containsPoint(p, rule)) {
        result = UP_WALL;
    }
    else if (p_west.containsPoint(p, rule)) {
        result = LEFT_WALL;
    }
    qDebug() << "Result is : " << result;
    return result;
}


}
