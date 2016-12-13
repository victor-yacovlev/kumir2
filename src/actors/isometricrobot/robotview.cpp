#include "robotview.h"
#include "util.h"
#include "cellgraphicsitem.h"
#include "sch_environment.h"

#define DEBUG_Z_ORDER false

#define MIN_ROBOT_DELAY 1000
#define MAX_ROBOT_DELAY 200
#define ROBOT_DELAY_STEP 200;




namespace Robot25D {

qreal RobotView::CellSize = 45.0;
qreal RobotView::CellBorderSize = 1.0;
QColor RobotView::UnpaintedColor = QColor("lightgreen");
QColor RobotView::PaintedColor = QColor("gray");
qreal RobotView::WallWidth = 10;
qreal RobotView::WallHeight = 10.0;



RobotView::RobotView(RobotModel * model, const QDir & imagesDir, QGraphicsItem *parent)
    : QObject(0)
    , QGraphicsRectItem(parent)
    , _robotItem(0)

{
    _model = model;
    connect(_model, SIGNAL(robotCrashed()), this, SIGNAL(sync()));
    connect(_model, SIGNAL(fieldChanged()), this, SLOT(handleModelFieldChanged()));
    setPen(Qt::NoPen);
    _imagesDir = imagesDir;
    _robotItem = NULL;
    const QString imagesRoot = imagesDir.absolutePath();
    for (int i=0; i<8; i++) {
        QPixmap px(imagesRoot+"/grass_"+QString::number(i)+".png");
        QBrush br(px);
        _grass << br;
    }
}

RobotView::~RobotView()
{
    if (_robotItem) {
        delete _robotItem;
    }
}

void RobotView::handleModelFieldChanged()
{
    createField();
    createRobot();
}


void RobotView::createField()
{    
    for (int i=0; i<_allItems.size(); i++) {
        if (scene()) {
            scene()->removeItem(_allItems[i]);
            delete _allItems[i];
        }
    }
    _allItems.clear();
    if (_model->field().empty())
        return;
    QPointF sceneTopLeft, sceneBottomRight;
    QRectF rect;
    for (int i=0; i<_model->field().size(); i++) {
        rect = createEmptyCell(-1,i,false,false,true);
        if (rect.topLeft().x()<sceneTopLeft.x())
            sceneTopLeft.setX(rect.topLeft().x());
        if (rect.topLeft().y()<sceneTopLeft.y())
            sceneTopLeft.setY(rect.topLeft().y());
        rect = createEmptyCell(_model->field()[0].size(),i,false,false,true);
        if (rect.bottomRight().x()>sceneBottomRight.x())
            sceneBottomRight.setX(rect.bottomRight().x());
        if (rect.bottomRight().y()>sceneBottomRight.y())
            sceneBottomRight.setY(rect.bottomRight().y());
        for (int j=0; j<_model->field()[i].size(); j++) {
            createEmptyCell(j,i,_model->field()[i][j].painted,_model->field()[i][j].pointed,false);
        }
    }
    for (int y=0; y<_model->field().size(); y++) {
        for (int x=0; x<_model->field()[y].size(); x++) {
            QGraphicsItem *wh = createHorizontalWall(x,y,_model->field()[y][x].baseZOrder-0.1);
            QGraphicsItem *wv = createVerticalWall(x,y,_model->field()[y][x].baseZOrder-0.1);
            _allItems << wh << wv;
            _model->field()[y][x].wallLeftItem = wv;
            _model->field()[y][x].wallUpItem = wh;
            if (y>0)
                _model->field()[y-1][x].wallDownItem = wh;
            if (x>0)
                _model->field()[y][x-1].wallRightItem = wv;
            wv->setVisible(_model->field()[y][x].wallLeft);
            wh->setVisible(_model->field()[y][x].wallUp);
        }
    }


    for (int x=0; x<_model->field().last().size(); x++) {
        QGraphicsItem *w = createHorizontalWall(x, _model->field().size(), _model->field().last()[x].baseZOrder+0.001);
        _allItems << w;
        _model->field().last()[x].wallDown = true;
        _model->field().last()[x].wallDownItem = w;

    }

    for (int y=0; y<_model->field().size(); y++) {
        QGraphicsItem *w = createVerticalWall(_model->field()[y].size(), y, _model->field()[y].last().baseZOrder+0.001);
        _allItems << w;
        _model->field()[y].last().wallRight = true;
        _model->field()[y].last().wallRightItem = w;
    }

    for (int y=0; y<_model->field().size(); ++y) {
        for (int x=0; x<_model->field()[y].size(); ++x) {
            if (_model->field()[y][x].flag) {
                QGraphicsItem *flagItem = createFlagItem(x, y, _model->field()[y][x].baseZOrder);
                _allItems << flagItem;
            }
        }
    }

    for (int y=0; y<_model->field().size(); y++) {
        for (int x=0; x<_model->field()[y].size(); x++) {
            RobotCell cell = _model->field()[y][x];
            Q_CHECK_PTR(cell.wallDownItem);
            Q_CHECK_PTR(cell.wallLeftItem);
            Q_CHECK_PTR(cell.wallRightItem);
            Q_CHECK_PTR(cell.wallUpItem);
        }
    }

    for (int i=0; i<_model->field()[0].size(); i++) {
        createEmptyCell(i,-1,false,false,true);
        createEmptyCell(i,_model->field().size(),false,false,true);
    }
    createEmptyCell(-1,-1,false,false,true);
    createEmptyCell(_model->field()[0].size(),-1,false,false,true);
    createEmptyCell(_model->field()[0].size(),_model->field().size(),false,false,true);
    createEmptyCell(-1,_model->field().size(),false,false,true);

    qreal sceneX = sceneTopLeft.x();
    qreal sceneY = sceneTopLeft.y();
    qreal sceneWidth = fabs(sceneBottomRight.x()-sceneTopLeft.x());
    qreal sceneHeight = fabs(sceneBottomRight.y()-sceneTopLeft.y());


    _offset = QPointF(-sceneX, -sceneY);
    for (int i=0; i<_allItems.size(); i++) {
        _allItems[i]->moveBy(_offset.x(), _offset.y());
    }
    setRect(0, 0, sceneWidth, sceneHeight);


}

QGraphicsItem* RobotView::createHorizontalWall(int x, int y, qreal zOrder)
{
    QVector<QPointF> points;
    QPolygonF polygon;
    QGraphicsPolygonItem *item = NULL;

    qreal x1_isometric;
    qreal y1_isometric;
    qreal z1_isometric;

    qreal x2_isometric;
    qreal y2_isometric;
    qreal z2_isometric;

    qreal x3_isometric;
    qreal y3_isometric;
    qreal z3_isometric;

    qreal x4_isometric;
    qreal y4_isometric;
    qreal z4_isometric;

    /* В изометрическом изображении стена имеет только 3 видимые грани:
                 две боковые и верхнюю */

    QGraphicsItemGroup *group = new QGraphicsItemGroup();

    /* передняя грань (обход координат по часовой стрелке от левого нижнего угла)*/
    x1_isometric = x*CellSize;// - m_wallWidth/2;
    y1_isometric = y*CellSize + WallWidth/2;
    z1_isometric = 0.0;

    x2_isometric = x*CellSize;// - m_wallWidth/2;
    y2_isometric = y*CellSize + WallWidth/2;
    z2_isometric = WallHeight;

    x3_isometric = (x+1)*CellSize;// + m_wallWidth/2;
    y3_isometric = y*CellSize + WallWidth/2;
    z3_isometric = WallHeight;

    x4_isometric = (x+1)*CellSize;// + m_wallWidth/2;
    y4_isometric = y*CellSize + WallWidth/2;
    z4_isometric = 0.0;

    points.clear();
    points << mapToIsometricCoordinates(x1_isometric,y1_isometric,z1_isometric);
    points << mapToIsometricCoordinates(x2_isometric,y2_isometric,z2_isometric);
    points << mapToIsometricCoordinates(x3_isometric,y3_isometric,z3_isometric);
    points << mapToIsometricCoordinates(x4_isometric,y4_isometric,z4_isometric);
    polygon = QPolygonF(points);
    item = new QGraphicsPolygonItem(polygon);
    item->setPen(wallPen());
    item->setBrush(wallBrush());
    item->setZValue(zOrder);
//    m_scene->addItem(item);
    group->addToGroup(item);

    /* боковая грань (обход координат по часовой стрелке от левого нижнего угла)*/

    x1_isometric = (x+1)*CellSize;// + m_wallWidth/2;
    y1_isometric = y*CellSize + WallWidth/2;
    z1_isometric = 0.0;

    x2_isometric = (x+1)*CellSize;// + m_wallWidth/2;
    y2_isometric = y*CellSize + WallWidth/2;
    z2_isometric = WallHeight;

    x3_isometric = (x+1)*CellSize;// + m_wallWidth/2;
    y3_isometric = y*CellSize - WallWidth/2;
    z3_isometric = WallHeight;

    x4_isometric = (x+1)*CellSize;// + m_wallWidth/2;
    y4_isometric = y*CellSize - WallWidth/2;
    z4_isometric = 0.0;

    points.clear();
    points << mapToIsometricCoordinates(x1_isometric,y1_isometric,z1_isometric);
    points << mapToIsometricCoordinates(x2_isometric,y2_isometric,z2_isometric);
    points << mapToIsometricCoordinates(x3_isometric,y3_isometric,z3_isometric);
    points << mapToIsometricCoordinates(x4_isometric,y4_isometric,z4_isometric);
    polygon = QPolygonF(points);
    item = new QGraphicsPolygonItem(polygon);
    item->setPen(wallPen());
    item->setBrush(wallBrush());
    item->setZValue(zOrder);
//    m_scene->addItem(item);
    group->addToGroup(item);

    /* верхняя грань (обход координат по часовой стрелке от левого нижнего угла)*/

    x1_isometric = x*CellSize;// - m_wallWidth/2;
    y1_isometric = y*CellSize + WallWidth/2;
    z1_isometric = WallHeight;

    x2_isometric = x*CellSize;// - m_wallWidth/2;
    y2_isometric = y*CellSize - WallWidth/2;
    z2_isometric = WallHeight;

    x3_isometric = (x+1)*CellSize;// + m_wallWidth/2;
    y3_isometric = y*CellSize - WallWidth/2;
    z3_isometric = WallHeight;

    x4_isometric = (x+1)*CellSize;// + m_wallWidth/2;
    y4_isometric = y*CellSize + WallWidth/2;
    z4_isometric = WallHeight;

    points.clear();
    points << mapToIsometricCoordinates(x1_isometric,y1_isometric,z1_isometric);
    points << mapToIsometricCoordinates(x2_isometric,y2_isometric,z2_isometric);
    points << mapToIsometricCoordinates(x3_isometric,y3_isometric,z3_isometric);
    points << mapToIsometricCoordinates(x4_isometric,y4_isometric,z4_isometric);
    polygon = QPolygonF(points);
    item = new QGraphicsPolygonItem(polygon);
    item->setPen(wallPen());
    item->setBrush(wallBrush());
    item->setZValue(zOrder);
//    m_scene->addItem(item);
    group->addToGroup(item);
//    m_scene->addItem(group);
    group->setParentItem(this);


    return group;

}

QGraphicsItem* RobotView::createVerticalWall(int x, int y, qreal zOrder)
{
    QVector<QPointF> points;
    QPolygonF polygon;
    QGraphicsPolygonItem *item = NULL;

    qreal x1_isometric;
    qreal y1_isometric;
    qreal z1_isometric;

    qreal x2_isometric;
    qreal y2_isometric;
    qreal z2_isometric;

    qreal x3_isometric;
    qreal y3_isometric;
    qreal z3_isometric;

    qreal x4_isometric;
    qreal y4_isometric;
    qreal z4_isometric;

    /* В изометрическом изображении стена имеет только 3 видимые грани:
                 две боковые и верхнюю */

    QGraphicsItemGroup *group = new QGraphicsItemGroup();

    /* передняя грань (обход координат по часовой стрелке от левого нижнего угла)*/
    x1_isometric = x*CellSize - WallWidth/2;
    y1_isometric = (y+1)*CellSize;// - m_wallWidth/2;
    z1_isometric = 0.0;

    x2_isometric = x*CellSize - WallWidth/2;
    y2_isometric = (y+1)*CellSize;// - m_wallWidth/2;
    z2_isometric = WallHeight;

    x3_isometric = x*CellSize + WallWidth/2;
    y3_isometric = (y+1)*CellSize;// - m_wallWidth/2;
    z3_isometric = WallHeight;

    x4_isometric = x*CellSize + WallWidth/2;
    y4_isometric = (y+1)*CellSize;// - m_wallWidth/2;
    z4_isometric = 0.0;

    points.clear();
    points << mapToIsometricCoordinates(x1_isometric,y1_isometric,z1_isometric);
    points << mapToIsometricCoordinates(x2_isometric,y2_isometric,z2_isometric);
    points << mapToIsometricCoordinates(x3_isometric,y3_isometric,z3_isometric);
    points << mapToIsometricCoordinates(x4_isometric,y4_isometric,z4_isometric);
    polygon = QPolygonF(points);
    item = new QGraphicsPolygonItem(polygon);
    item->setPen(wallPen());
    item->setBrush(wallBrush());
    item->setZValue(zOrder);
//    m_scene->addItem(item);
    group->addToGroup(item);

    /* боковая грань (обход координат по часовой стрелке от левого нижнего угла)*/

    x1_isometric = x*CellSize + WallWidth/2;
    y1_isometric = (y+1)*CellSize;// - m_wallWidth/2;
    z1_isometric = 0.0;

    x2_isometric = x*CellSize + WallWidth/2;
    y2_isometric = (y+1)*CellSize;// - m_wallWidth/2;
    z2_isometric = WallHeight;

    x3_isometric = x*CellSize + WallWidth/2;
    y3_isometric = (y)*CellSize;// - m_wallWidth/2;
    z3_isometric = WallHeight;

    x4_isometric = x*CellSize + WallWidth/2;
    y4_isometric = (y)*CellSize;// - m_wallWidth/2;
    z4_isometric = 0.0;

    points.clear();
    points << mapToIsometricCoordinates(x1_isometric,y1_isometric,z1_isometric);
    points << mapToIsometricCoordinates(x2_isometric,y2_isometric,z2_isometric);
    points << mapToIsometricCoordinates(x3_isometric,y3_isometric,z3_isometric);
    points << mapToIsometricCoordinates(x4_isometric,y4_isometric,z4_isometric);
    polygon = QPolygonF(points);
    item = new QGraphicsPolygonItem(polygon);
    item->setPen(wallPen());
    item->setBrush(wallBrush());
    item->setZValue(zOrder);
//    m_scene->addItem(item);
    group->addToGroup(item);

    /* верхняя грань (обход координат по часовой стрелке от левого нижнего угла)*/

    x1_isometric = x*CellSize - WallWidth/2;
    y1_isometric = (y+1)*CellSize;// - m_wallWidth/2;
    z1_isometric = WallHeight;

    x2_isometric = x*CellSize - WallWidth/2;
    y2_isometric = (y)*CellSize;// - m_wallWidth/2;
    z2_isometric = WallHeight;

    x3_isometric = x*CellSize + WallWidth/2;
    y3_isometric = (y)*CellSize;// - m_wallWidth/2;
    z3_isometric = WallHeight;

    x4_isometric = x*CellSize + WallWidth/2;
    y4_isometric = (y+1)*CellSize;// - m_wallWidth/2;
    z4_isometric = WallHeight;

    points.clear();
    points << mapToIsometricCoordinates(x1_isometric,y1_isometric,z1_isometric);
    points << mapToIsometricCoordinates(x2_isometric,y2_isometric,z2_isometric);
    points << mapToIsometricCoordinates(x3_isometric,y3_isometric,z3_isometric);
    points << mapToIsometricCoordinates(x4_isometric,y4_isometric,z4_isometric);
    polygon = QPolygonF(points);
    item = new QGraphicsPolygonItem(polygon);
    item->setPen(wallPen());
    item->setBrush(wallBrush());
    item->setZValue(zOrder);
//    m_scene->addItem(item);
    group->addToGroup(item);
//    m_scene->addItem(group);
    group->setParentItem(this);

    return group;
}

QGraphicsItem *RobotView::createFlagItem(int x, int y, qreal zOrder)
{
    QGraphicsItemGroup *group = new QGraphicsItemGroup();
    const QPointF base = mapToIsometricCoordinates(x * CellSize + 7, y * CellSize + 9, 0.0);
    static const qreal StickHeight = 80;
    static const qreal StickWidth = 4;
    static const qreal FlagSize = 12;
    static const QColor FlagColor = QColor(Qt::yellow);
    QGraphicsRectItem * stick = new QGraphicsRectItem(QRectF(base.x(), base.y()-StickHeight, StickWidth, StickHeight), group);
    stick->setPen(Qt::NoPen);
    stick->setBrush(QColor(Qt::black));

    group->setParentItem(this);
    QPolygonF flagPolygon;
    flagPolygon.append(base + QPointF(StickWidth/2 + 0, -StickHeight+FlagSize));
    flagPolygon.append(base + QPointF(StickWidth/2 + 2*FlagSize, -StickHeight+0.5*FlagSize));
    flagPolygon.append(base + QPointF(StickWidth/2 + 0, -StickHeight));
    QGraphicsPolygonItem * flag = new QGraphicsPolygonItem(flagPolygon, group);
    flag->setPen(QPen(QColor(Qt::black), 1));
    flag->setBrush(FlagColor);

    group->setZValue(zOrder);
    return group;
}

QPen RobotView::wallPen()
{
    return QPen(QColor("black"),CellBorderSize);
}

QBrush RobotView::wallBrush()
{
    return QBrush(QColor("sandybrown"), Qt::Dense4Pattern);
}

QRectF RobotView::createEmptyCell(int x, int y,
                                  bool painted, bool pointed,
                                  bool isBorder)
{

    qreal x1_isometric = CellSize*x;
    qreal y1_isometric = CellSize*y;
    qreal z1_isometric = 0.0;

    qreal x2_isometric = CellSize*x;
    qreal y2_isometric = CellSize*y;
    qreal z2_isometric = 0.0;

    qreal x3_isometric = CellSize*x;
    qreal y3_isometric = CellSize*y;
    qreal z3_isometric = 0.0;

    qreal x4_isometric = CellSize*x;
    qreal y4_isometric = CellSize*y;
    qreal z4_isometric = 0.0;

    if (isBorder) {
        if (y==_model->field().size()) {
            y3_isometric += CellSize/2;
            y4_isometric += CellSize/2;
        }
        else if (y==-1) {
            y1_isometric += CellSize/2;
            y2_isometric += CellSize/2;
            y3_isometric += CellSize;
            y4_isometric += CellSize;
        }
        else {
            y3_isometric += CellSize;
            y4_isometric += CellSize;
        }
        if (x==_model->field()[0].size()) {
            x2_isometric += CellSize/2;
            x3_isometric += CellSize/2;
        }
        else if (x==-1) {
            x1_isometric += CellSize/2;
            x4_isometric += CellSize/2;
            x2_isometric += CellSize;
            x3_isometric += CellSize;
        }
        else {
            x2_isometric += CellSize;
            x3_isometric += CellSize;
        }

    }
    else {
        x2_isometric += CellSize;
        x3_isometric += CellSize;
        y3_isometric += CellSize;
        y4_isometric += CellSize;
    }

    QVector<QPointF> points;
    QVector<QPointF> south;
    QVector<QPointF> east;
    QVector<QPointF> north;
    QVector<QPointF> west;

    points << mapToIsometricCoordinates(x1_isometric, y1_isometric, z1_isometric);
    points << mapToIsometricCoordinates(x2_isometric, y2_isometric, z2_isometric);
    points << mapToIsometricCoordinates(x3_isometric, y3_isometric, z3_isometric);
    points << mapToIsometricCoordinates(x4_isometric, y4_isometric, z4_isometric);

    if (y>0) {
        north << mapToIsometricCoordinates(x1_isometric, y1_isometric-CellSize/4, z1_isometric);
        north << mapToIsometricCoordinates(x2_isometric, y2_isometric-CellSize/4, z2_isometric);
        north << mapToIsometricCoordinates(x3_isometric, y3_isometric-CellSize*3/4, z3_isometric);
        north << mapToIsometricCoordinates(x4_isometric, y4_isometric-CellSize*3/4, z4_isometric);
    }

    if (y<_model->field().size()-1) {
        south << mapToIsometricCoordinates(x1_isometric, y1_isometric+CellSize*3/4, z1_isometric);
        south << mapToIsometricCoordinates(x2_isometric, y2_isometric+CellSize*3/4, z2_isometric);
        south << mapToIsometricCoordinates(x3_isometric, y3_isometric+CellSize*5/4, z3_isometric);
        south << mapToIsometricCoordinates(x4_isometric, y4_isometric+CellSize*5/4, z4_isometric);
    }

    west << mapToIsometricCoordinates(x1_isometric-CellSize/4, y1_isometric, z1_isometric);
    west << mapToIsometricCoordinates(x1_isometric+CellSize/4, y2_isometric, z2_isometric);
    west << mapToIsometricCoordinates(x1_isometric+CellSize/4, y3_isometric, z3_isometric);
    west << mapToIsometricCoordinates(x1_isometric-CellSize/4, y4_isometric, z4_isometric);

    east << mapToIsometricCoordinates(x2_isometric-CellSize/4, y1_isometric, z1_isometric);
    east << mapToIsometricCoordinates(x2_isometric+CellSize/4, y2_isometric, z2_isometric);
    east << mapToIsometricCoordinates(x2_isometric+CellSize/4, y3_isometric, z3_isometric);
    east << mapToIsometricCoordinates(x2_isometric-CellSize/4, y4_isometric, z4_isometric);



    QPolygonF polygon(points);
    QPolygonF ps(south);
    QPolygonF pe(east);
    QPolygonF pn(north);
    QPolygonF pw(west);

    int cellX=-1, cellY=-1;
    if (x!=-1 && y!=-1) {
        cellX = x; cellY = y;
    }
    if (y>=_model->field().size() || x>=_model->field()[0].size()) {
        cellX = -1; cellY = -1;
    }

    CellGraphicsItem *result = new CellGraphicsItem(polygon, ps, pe, pn, pw,
                                                    false,
                                                    cellX, cellY,
                                                    this);

    _allItems << result;
    result->setParentItem(this);
    result->setZValue(-1000);
    if (y>=0&&y<_model->field().size()&&x>=0&&x<_model->field()[0].size()) {
        _model->field()[y][x].cellItem = result;
        updateCell(x,y,painted);
    }
    else {
        result->setPen(QPen(QColor("black"),CellBorderSize));
        result->setBrush(_grass[0]);
    }

    if (DEBUG_Z_ORDER) {
        QPointF textPoint = mapToIsometricCoordinates(CellSize*x-CellSize/2+20,CellSize*y+CellSize/2+2,0);
        QGraphicsTextItem *text = new QGraphicsTextItem(QString::number(_model->field()[y][x].baseZOrder,'f',1));
        text->setFont(QFont("sans-serif",6));
        text->setZValue(-900);
        text->setPos(textPoint);
//        m_scene->addItem(text);
        text->setParentItem(this);
    }

    //    if (pointed) {
    if (x>=0 && y>=0 && x<_model->field()[0].size() && y<_model->field().size() )
    {

        QVector<QPointF> points(4);

        qreal OFFSET = 15;

        points[0] = mapToIsometricCoordinates(CellSize*x+OFFSET, CellSize*y+OFFSET, 0);
        points[1] = mapToIsometricCoordinates(CellSize*(x+1)-OFFSET, CellSize*y+OFFSET, 0);
        points[2] = mapToIsometricCoordinates(CellSize*(x+1)-OFFSET, CellSize*(y+1)-OFFSET, 0);
        points[3] = mapToIsometricCoordinates(CellSize*x+OFFSET, CellSize*(y+1)-OFFSET, 0);



        QAbstractGraphicsShapeItem *item = new QGraphicsPolygonItem(QPolygonF(points));

        item->setBrush(QColor(255,255,255,100));
        item->setPen(QPen(QColor("black"), 2));
        item->setZValue(_model->field()[y][x].baseZOrder);
//        scene()->addItem(item);
        item->setParentItem(this);
        _allItems << item;
        _model->field()[y][x].pointItem = item;
        item->setVisible(pointed);
    }

    return result->boundingRect();

}

void RobotView::updateCell(int x, int y, bool painted)
{
    _model->updateCell(x, y, painted);
    _model->field()[y][x].paintState = painted? _grass.size()-1 : 0;
    QGraphicsPolygonItem *item = _model->field()[y][x].cellItem;
    item->setPen(QPen(QColor("black"),CellBorderSize));
    item->setBrush(painted? (_grass.last()) : (_grass.first()));
    item->update();
}

void RobotView::createRobot()
{
    if (_model->field().isEmpty())
        return;
    if (_robotItem) {
        _robotItem->disconnect();
        _robotItem->prepareForDelete();
        _robotItem->deleteLater();
    }
    _robotItem = new RobotItem(_model, _imagesDir, this);
    connect(_robotItem, SIGNAL(evaluationFinished()), this, SLOT(handleRobotEvaluationFinised()));
    _robotItem->setAnimated(false);
}



void RobotView::finishEvaluation()
{
    for (int y=0; y<_model->field().size(); y++) {
        for (int x=0; x<_model->field()[y].size(); x++) {
            updateCell(x, y, _model->field()[y][x].painted);
        }
    }
    _robotItem->setAnimated(true);
}


void RobotView::prepareToDispose()
{
    setAnimated(false);
}

void RobotView::reset()
{
    _robotItem->reset();
    _robotItem->setAnimated(false);
}


void RobotView::handleRobotEvaluationFinised()
{
    emit sync();
}

void RobotView::setAnimated(bool v)
{
    if (!_robotItem)
        return;
    _robotItem->setAnimated(v);
}

bool RobotView::isAnimated() const
{
    if (_robotItem)
        return false;
    return _robotItem->isAnimated();
}

}
