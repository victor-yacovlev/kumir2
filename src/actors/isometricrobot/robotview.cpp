#include "robotview.h"
#include "util.h"
#include "cellgraphicsitem.h"
#include "sch_environment.h"

#define DEBUG_Z_ORDER false

#define MIN_ROBOT_DELAY 1000
#define MAX_ROBOT_DELAY 200
#define ROBOT_DELAY_STEP 200;




namespace Robot25D {

qreal RobotView::m_cellSize = 45.0;
qreal RobotView::m_cellBorderSize = 1.0;
QColor RobotView::m_unpaintedColor = QColor("lightgreen");
QColor RobotView::m_paintedColor = QColor("gray");
qreal RobotView::m_wallWidth = 10;
qreal RobotView::m_wallHeight = 10.0;



RobotView::RobotView(const QDir & imagesDir, bool with_controls, bool with_bachground,
                     bool teacherMode,
                     const QSize &minSize, QGraphicsItem *parent)
    : QObject(0)
    , QGraphicsRectItem(parent)
    , m_robotItem(0)

{
    Q_UNUSED(with_bachground);
    Q_UNUSED(minSize);
    setPen(Qt::NoPen);
    imagesDir_ = imagesDir;

    b_loadingMode = false;
    r_loadingState = 0.0;
    b_teacherMode = teacherMode;
    e_lastError = Robot25D::NoRuntimeError;
    b_withControls = with_controls;
    b_mousePressed = false;
//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setMinimumSize(minSize);
//    m_scene = new QGraphicsScene(this);
//    setFrameShape(QFrame::NoFrame);
//    setRenderHint(QPainter::Antialiasing, true);
//    setScene(m_scene)
    m_robotItem = NULL;
    const QString imagesRoot = imagesDir.absolutePath();
//#ifdef Q_OS_WINCE
//    with_bachground = false;
//#endif
//    if (with_bachground) {
//        //        px_background = QImage(imagesRoot+"/saturn.png");
//        //        setBackgroundBrush(palette().window());
////        QPalette pal = palette();
////        pal.setBrush(QPalette::Base, QPixmap(imagesRoot+"/grass_0.png"));
////        setPalette(pal);
//                setBackgroundBrush(QPixmap(imagesRoot+"/grass_0.png"));
//    }
//    else {
//        setBackgroundBrush(QColor("black"));
//    }
    //    m_robotAnimator = new RobotAnimator(this);
    //    QPixmap grass(imagesRoot+"/grass.png");
    //    QPixmap pgrass(imagesRoot+"/grass_painted.png");
#ifndef Q_OS_WINCE
    for (int i=0; i<8; i++) {
        QPixmap px(imagesRoot+"/grass_"+QString::number(i)+".png");
        QBrush br(px);
        lbr_grass << br;
    }
#else
    for (int i=0; i<8; i++) {
        QColor c(0, i*128/8, (8-i)*192/8);
        QBrush br(c);
        lbr_grass << br;
    }
#endif
}

RobotView::~RobotView()
{
    if (m_robotItem) {
        delete m_robotItem;
    }
}


void RobotView::evaluateCommand(const QString &russianName)
{
    if (russianName.toLower().simplified()=="turn left") {

    }
    else if (russianName.toLower().simplified()=="turn right") {

    }
    else if (russianName.toLower().simplified()=="go forward") {

    }
    else if (russianName.toLower().simplified()=="paint") {

    }
}


void RobotView::setLoadingMode(bool v)
{
    b_loadingMode = v;
    update();
}

void RobotView::setLoadingState(qreal v)
{
    r_loadingState = v;
    update();
}

void RobotView::createField()
{    
    for (int i=0; i<l_allItems.size(); i++) {
        if (scene()) {
            scene()->removeItem(l_allItems[i]);
            delete l_allItems[i];
        }
    }
    l_allItems.clear();
    if (m_field.isEmpty())
        return;
    QPointF sceneTopLeft, sceneBottomRight;
    QRectF rect;
    for (int i=0; i<m_field.size(); i++) {
        rect = createEmptyCell(-1,i,false,false,true);
        if (rect.topLeft().x()<sceneTopLeft.x())
            sceneTopLeft.setX(rect.topLeft().x());
        if (rect.topLeft().y()<sceneTopLeft.y())
            sceneTopLeft.setY(rect.topLeft().y());
        rect = createEmptyCell(m_field[0].size(),i,false,false,true);
        if (rect.bottomRight().x()>sceneBottomRight.x())
            sceneBottomRight.setX(rect.bottomRight().x());
        if (rect.bottomRight().y()>sceneBottomRight.y())
            sceneBottomRight.setY(rect.bottomRight().y());
        for (int j=0; j<m_field[i].size(); j++) {
            createEmptyCell(j,i,m_field[i][j].painted,m_field[i][j].pointed,false);
            //            if (m_field[i][j].wallUp) {
            //                createHorizontalWall(j,i,m_field[i][j].baseZOrder-0.1);
            //            }
            //            if (m_field[i][j].wallDown) {
            //                createHorizontalWall(j,i+1,m_field[i][j].baseZOrder+0.001);
            //            }
            //            if (m_field[i][j].wallLeft) {
            //                createVerticalWall(j,i,m_field[i][j].baseZOrder-0.1);
            //            }
            //            if (m_field[i][j].wallRight) {
            //                createVerticalWall(j+1,i,m_field[i][j].baseZOrder+0.001);
            //            }
        }
    }
    for (int y=0; y<m_field.size(); y++) {
        for (int x=0; x<m_field[y].size(); x++) {
            QGraphicsItem *wh = createHorizontalWall(x,y,m_field[y][x].baseZOrder-0.1);
            QGraphicsItem *wv = createVerticalWall(x,y,m_field[y][x].baseZOrder-0.1);
            l_allItems << wh << wv;
            m_field[y][x].wallLeftItem = wv;
            m_field[y][x].wallUpItem = wh;
            if (y>0)
                m_field[y-1][x].wallDownItem = wh;
            if (x>0)
                m_field[y][x-1].wallRightItem = wv;
            wv->setVisible(m_field[y][x].wallLeft);
            wh->setVisible(m_field[y][x].wallUp);
        }
    }


    for (int x=0; x<m_field.last().size(); x++) {
        QGraphicsItem *w = createHorizontalWall(x, m_field.size(), m_field.last()[x].baseZOrder+0.001);
        l_allItems << w;
        m_field.last()[x].wallDown = true;
        m_field.last()[x].wallDownItem = w;

    }

    for (int y=0; y<m_field.size(); y++) {
        QGraphicsItem *w = createVerticalWall(m_field[y].size(), y, m_field[y].last().baseZOrder+0.001);
        l_allItems << w;
        m_field[y].last().wallRight = true;
        m_field[y].last().wallRightItem = w;
    }

    for (int y=0; y<m_field.size(); y++) {
        for (int x=0; x<m_field[y].size(); x++) {
            RobotCell cell = m_field[y][x];
            Q_CHECK_PTR(cell.wallDownItem);
            Q_CHECK_PTR(cell.wallLeftItem);
            Q_CHECK_PTR(cell.wallRightItem);
            Q_CHECK_PTR(cell.wallUpItem);
        }
    }

    for (int i=0; i<m_field[0].size(); i++) {
        createEmptyCell(i,-1,false,false,true);
        createEmptyCell(i,m_field.size(),false,false,true);
    }
    createEmptyCell(-1,-1,false,false,true);
    createEmptyCell(m_field[0].size(),-1,false,false,true);
    createEmptyCell(m_field[0].size(),m_field.size(),false,false,true);
    createEmptyCell(-1,m_field.size(),false,false,true);

    qreal sceneX = sceneTopLeft.x();
    qreal sceneY = sceneTopLeft.y();
    qreal sceneWidth = fabs(sceneBottomRight.x()-sceneTopLeft.x());
    qreal sceneHeight = fabs(sceneBottomRight.y()-sceneTopLeft.y());


    //    m_scene->addRect(sceneX+2, sceneY+2,
    //                     sceneWidth-4, sceneHeight-4,
    //                     QPen(QColor("white"),4), Qt::NoBrush);

//    setSceneRect(sceneX, sceneY, sceneWidth, sceneHeight);

    p_offset = QPointF(-sceneX, -sceneY);
    for (int i=0; i<l_allItems.size(); i++) {
        l_allItems[i]->moveBy(p_offset.x(), p_offset.y());
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
    x1_isometric = x*m_cellSize;// - m_wallWidth/2;
    y1_isometric = y*m_cellSize + m_wallWidth/2;
    z1_isometric = 0.0;

    x2_isometric = x*m_cellSize;// - m_wallWidth/2;
    y2_isometric = y*m_cellSize + m_wallWidth/2;
    z2_isometric = m_wallHeight;

    x3_isometric = (x+1)*m_cellSize;// + m_wallWidth/2;
    y3_isometric = y*m_cellSize + m_wallWidth/2;
    z3_isometric = m_wallHeight;

    x4_isometric = (x+1)*m_cellSize;// + m_wallWidth/2;
    y4_isometric = y*m_cellSize + m_wallWidth/2;
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

    x1_isometric = (x+1)*m_cellSize;// + m_wallWidth/2;
    y1_isometric = y*m_cellSize + m_wallWidth/2;
    z1_isometric = 0.0;

    x2_isometric = (x+1)*m_cellSize;// + m_wallWidth/2;
    y2_isometric = y*m_cellSize + m_wallWidth/2;
    z2_isometric = m_wallHeight;

    x3_isometric = (x+1)*m_cellSize;// + m_wallWidth/2;
    y3_isometric = y*m_cellSize - m_wallWidth/2;
    z3_isometric = m_wallHeight;

    x4_isometric = (x+1)*m_cellSize;// + m_wallWidth/2;
    y4_isometric = y*m_cellSize - m_wallWidth/2;
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

    x1_isometric = x*m_cellSize;// - m_wallWidth/2;
    y1_isometric = y*m_cellSize + m_wallWidth/2;
    z1_isometric = m_wallHeight;

    x2_isometric = x*m_cellSize;// - m_wallWidth/2;
    y2_isometric = y*m_cellSize - m_wallWidth/2;
    z2_isometric = m_wallHeight;

    x3_isometric = (x+1)*m_cellSize;// + m_wallWidth/2;
    y3_isometric = y*m_cellSize - m_wallWidth/2;
    z3_isometric = m_wallHeight;

    x4_isometric = (x+1)*m_cellSize;// + m_wallWidth/2;
    y4_isometric = y*m_cellSize + m_wallWidth/2;
    z4_isometric = m_wallHeight;

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
    x1_isometric = x*m_cellSize - m_wallWidth/2;
    y1_isometric = (y+1)*m_cellSize;// - m_wallWidth/2;
    z1_isometric = 0.0;

    x2_isometric = x*m_cellSize - m_wallWidth/2;
    y2_isometric = (y+1)*m_cellSize;// - m_wallWidth/2;
    z2_isometric = m_wallHeight;

    x3_isometric = x*m_cellSize + m_wallWidth/2;
    y3_isometric = (y+1)*m_cellSize;// - m_wallWidth/2;
    z3_isometric = m_wallHeight;

    x4_isometric = x*m_cellSize + m_wallWidth/2;
    y4_isometric = (y+1)*m_cellSize;// - m_wallWidth/2;
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

    x1_isometric = x*m_cellSize + m_wallWidth/2;
    y1_isometric = (y+1)*m_cellSize;// - m_wallWidth/2;
    z1_isometric = 0.0;

    x2_isometric = x*m_cellSize + m_wallWidth/2;
    y2_isometric = (y+1)*m_cellSize;// - m_wallWidth/2;
    z2_isometric = m_wallHeight;

    x3_isometric = x*m_cellSize + m_wallWidth/2;
    y3_isometric = (y)*m_cellSize;// - m_wallWidth/2;
    z3_isometric = m_wallHeight;

    x4_isometric = x*m_cellSize + m_wallWidth/2;
    y4_isometric = (y)*m_cellSize;// - m_wallWidth/2;
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

    x1_isometric = x*m_cellSize - m_wallWidth/2;
    y1_isometric = (y+1)*m_cellSize;// - m_wallWidth/2;
    z1_isometric = m_wallHeight;

    x2_isometric = x*m_cellSize - m_wallWidth/2;
    y2_isometric = (y)*m_cellSize;// - m_wallWidth/2;
    z2_isometric = m_wallHeight;

    x3_isometric = x*m_cellSize + m_wallWidth/2;
    y3_isometric = (y)*m_cellSize;// - m_wallWidth/2;
    z3_isometric = m_wallHeight;

    x4_isometric = x*m_cellSize + m_wallWidth/2;
    y4_isometric = (y+1)*m_cellSize;// - m_wallWidth/2;
    z4_isometric = m_wallHeight;

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

QPen RobotView::wallPen()
{
    return QPen(QColor("black"),m_cellBorderSize);
}

QBrush RobotView::wallBrush()
{
    return QBrush(QColor("sandybrown"), Qt::Dense4Pattern);
}

QRectF RobotView::createEmptyCell(int x, int y,
                                  bool painted, bool pointed,
                                  bool isBorder)
{

    qreal x1_isometric = m_cellSize*x;
    qreal y1_isometric = m_cellSize*y;
    qreal z1_isometric = 0.0;

    qreal x2_isometric = m_cellSize*x;
    qreal y2_isometric = m_cellSize*y;
    qreal z2_isometric = 0.0;

    qreal x3_isometric = m_cellSize*x;
    qreal y3_isometric = m_cellSize*y;
    qreal z3_isometric = 0.0;

    qreal x4_isometric = m_cellSize*x;
    qreal y4_isometric = m_cellSize*y;
    qreal z4_isometric = 0.0;

    if (isBorder) {
        if (y==m_field.size()) {
            y3_isometric += m_cellSize/2;
            y4_isometric += m_cellSize/2;
        }
        else if (y==-1) {
            y1_isometric += m_cellSize/2;
            y2_isometric += m_cellSize/2;
            y3_isometric += m_cellSize;
            y4_isometric += m_cellSize;
        }
        else {
            y3_isometric += m_cellSize;
            y4_isometric += m_cellSize;
        }
        if (x==m_field[0].size()) {
            x2_isometric += m_cellSize/2;
            x3_isometric += m_cellSize/2;
        }
        else if (x==-1) {
            x1_isometric += m_cellSize/2;
            x4_isometric += m_cellSize/2;
            x2_isometric += m_cellSize;
            x3_isometric += m_cellSize;
        }
        else {
            x2_isometric += m_cellSize;
            x3_isometric += m_cellSize;
        }

    }
    else {
        x2_isometric += m_cellSize;
        x3_isometric += m_cellSize;
        y3_isometric += m_cellSize;
        y4_isometric += m_cellSize;
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
        north << mapToIsometricCoordinates(x1_isometric, y1_isometric-m_cellSize/4, z1_isometric);
        north << mapToIsometricCoordinates(x2_isometric, y2_isometric-m_cellSize/4, z2_isometric);
        north << mapToIsometricCoordinates(x3_isometric, y3_isometric-m_cellSize*3/4, z3_isometric);
        north << mapToIsometricCoordinates(x4_isometric, y4_isometric-m_cellSize*3/4, z4_isometric);
    }

    if (y<m_field.size()-1) {
        south << mapToIsometricCoordinates(x1_isometric, y1_isometric+m_cellSize*3/4, z1_isometric);
        south << mapToIsometricCoordinates(x2_isometric, y2_isometric+m_cellSize*3/4, z2_isometric);
        south << mapToIsometricCoordinates(x3_isometric, y3_isometric+m_cellSize*5/4, z3_isometric);
        south << mapToIsometricCoordinates(x4_isometric, y4_isometric+m_cellSize*5/4, z4_isometric);
    }

    west << mapToIsometricCoordinates(x1_isometric-m_cellSize/4, y1_isometric, z1_isometric);
    west << mapToIsometricCoordinates(x1_isometric+m_cellSize/4, y2_isometric, z2_isometric);
    west << mapToIsometricCoordinates(x1_isometric+m_cellSize/4, y3_isometric, z3_isometric);
    west << mapToIsometricCoordinates(x1_isometric-m_cellSize/4, y4_isometric, z4_isometric);

    east << mapToIsometricCoordinates(x2_isometric-m_cellSize/4, y1_isometric, z1_isometric);
    east << mapToIsometricCoordinates(x2_isometric+m_cellSize/4, y2_isometric, z2_isometric);
    east << mapToIsometricCoordinates(x2_isometric+m_cellSize/4, y3_isometric, z3_isometric);
    east << mapToIsometricCoordinates(x2_isometric-m_cellSize/4, y4_isometric, z4_isometric);



    QPolygonF polygon(points);
    QPolygonF ps(south);
    QPolygonF pe(east);
    QPolygonF pn(north);
    QPolygonF pw(west);

    int cellX=-1, cellY=-1;
    if (x!=-1 && y!=-1) {
        cellX = x; cellY = y;
    }
    if (y>=m_field.size() || x>=m_field[0].size()) {
        cellX = -1; cellY = -1;
    }

    CellGraphicsItem *result = new CellGraphicsItem(polygon, ps, pe, pn, pw,
                                                    b_teacherMode,
                                                    cellX, cellY,
                                                    this);

//    m_scene->addItem(result);
    l_allItems << result;
    result->setParentItem(this);
    result->setZValue(-1000);
    if (y>=0&&y<m_field.size()&&x>=0&&x<m_field[0].size()) {
        m_field[y][x].cellItem = result;
        updateCell(x,y,painted);
    }
    else {
        result->setPen(QPen(QColor("black"),m_cellBorderSize));
        result->setBrush(lbr_grass[0]);
    }

    if (DEBUG_Z_ORDER) {
        QPointF textPoint = mapToIsometricCoordinates(m_cellSize*x-m_cellSize/2+20,m_cellSize*y+m_cellSize/2+2,0);
        QGraphicsTextItem *text = new QGraphicsTextItem(QString::number(m_field[y][x].baseZOrder,'f',1));
        text->setFont(QFont("sans-serif",6));
        text->setZValue(-900);
        text->setPos(textPoint);
//        m_scene->addItem(text);
        text->setParentItem(this);
    }

    //    if (pointed) {
    if (x>=0 && y>=0 && x<m_field[0].size() && y<m_field.size() )
    {

        QVector<QPointF> points(4);

        qreal OFFSET = 15;

        points[0] = mapToIsometricCoordinates(m_cellSize*x+OFFSET, m_cellSize*y+OFFSET, 0);
        points[1] = mapToIsometricCoordinates(m_cellSize*(x+1)-OFFSET, m_cellSize*y+OFFSET, 0);
        points[2] = mapToIsometricCoordinates(m_cellSize*(x+1)-OFFSET, m_cellSize*(y+1)-OFFSET, 0);
        points[3] = mapToIsometricCoordinates(m_cellSize*x+OFFSET, m_cellSize*(y+1)-OFFSET, 0);



        QAbstractGraphicsShapeItem *item = new QGraphicsPolygonItem(QPolygonF(points));

        item->setBrush(QColor(255,255,255,100));
        item->setPen(QPen(QColor("black"), 2));
        item->setZValue(m_field[y][x].baseZOrder);
//        scene()->addItem(item);
        item->setParentItem(this);
        l_allItems << item;
        m_field[y][x].pointItem = item;
        item->setVisible(pointed);
    }


    //    }
    //    else {
    //        //        m_field[y][x].pointItem = NULL;
    //    }

    return result->boundingRect();

}

void RobotView::updateCell(int x, int y, bool painted)
{
    m_field[y][x].painted = painted;
    m_field[y][x].paintState = painted? lbr_grass.size()-1 : 0;
    QGraphicsPolygonItem *item = m_field[y][x].cellItem;
    item->setPen(QPen(QColor("black"),m_cellBorderSize));
    item->setBrush(painted? (lbr_grass.last()) : (lbr_grass.first()));
    item->update();
}

void RobotView::createRobot(int x, int y, RobotItem::Direction direction)
{
    if (m_field.isEmpty())
        return;
    if (m_robotItem) {
        m_robotItem->disconnect();
        m_robotItem->prepareForDelete();
        m_robotItem->deleteLater();
    }
    m_robotItem = new RobotItem(imagesDir_, this);
    connect(m_robotItem, SIGNAL(evaluationFinished()), this, SLOT(handleRobotEvaluationFinised()));
    m_robotItem->setAnimated(false);
    m_robotItem->setDirection(direction);
    Point2Di p;
    p.x = x;
    p.y = y;
    m_robotItem->setScenePosition(p);
    //    m_robotX = x;
    //    m_robotY = y;
    //    m_originalRobotX = x;
    //    m_originalRobotY = y;
    //    m_originalRobotDirection = direction;
    //    m_robotItem->setPos(calculateRobotPosition(x,y));
    //    m_robotItem->setZValue(m_field[y][x].baseZOrder);
    //    m_robotItem->setDirection(direction);
    //    m_robotAnimator->setRobotItem(m_robotItem, m_robotDirection);
}



void RobotView::moveRobot(int x, int y)
{
    Point2Di p;
    p.x = x;
    p.y = y;
    m_robotItem->setScenePosition(p);
    //    m_robotX = x;
    //    m_robotY = y;
    //    QPointF targetPoint = calculateRobotPosition(x,y);
    //    m_robotItem->setZValue(m_field[y][x].baseZOrder);
    ////    m_robotAnimator->setSourceZOrder(m_robotItem->zValue());
    ////    m_robotAnimator->setTargetZOrder(m_field[y][x].baseZOrder);
    ////    m_robotAnimator->moveTo(targetPoint);
    //    Point3Dr coord;
    //    coord.x = targetPoint.x();
    //    coord.y = targetPoint.y();
    //    coord.z = m_field[y][x].baseZOrder;
    //    m_robotItem->moveTo(coord);
}

void RobotView::finishEvaluation()
{
    for (int y=0; y<m_field.size(); y++) {
        for (int x=0; x<m_field[y].size(); x++) {
            updateCell(x, y, m_field[y][x].painted);
        }
    }
    m_robotItem->setAnimated(true);
    m_robotItem->setScenePosition(m_robotItem->scenePosition());
    m_robotItem->setDirection(m_robotItem->direction());
    //    m_robotItem->setPos(calculateRobotPosition(m_robotX, m_robotY));
    //    m_robotItem->setZValue(m_field[m_robotY][m_robotX].baseZOrder);
    //    m_robotAnimator->setRobotItem(m_robotItem, m_robotDirection);
    //    if (m_broken)
    //        m_robotAnimator->showRobotBroken(m_robotDirection);
}


void RobotView::prepareToDispose()
{
    setAnimated(false);
}


// TODO : Implement all these methods

bool RobotView::goForward()
{
    qint16 nextX = m_robotItem->scenePosition().x;
    qint16 nextY = m_robotItem->scenePosition().y;
    if (m_robotItem->direction()==RobotItem::North)
        nextY--;
    else if (m_robotItem->direction()==RobotItem::South)
        nextY++;
    else if (m_robotItem->direction()==RobotItem::West)
        nextX--;
    else
        nextX++;
    bool wall = false;
    if (nextX<0 || nextX>=m_field[0].size() || nextY<0 || nextY>=m_field.size())
        wall = true;
    if (m_robotItem->direction()==RobotItem::North)
        wall = wall || m_field[m_robotItem->scenePosition().y][m_robotItem->scenePosition().x].wallUp;
    if (m_robotItem->direction()==RobotItem::South)
        wall = wall || m_field[m_robotItem->scenePosition().y][m_robotItem->scenePosition().x].wallDown;
    if (m_robotItem->direction()==RobotItem::West)
        wall = wall || m_field[m_robotItem->scenePosition().y][m_robotItem->scenePosition().x].wallLeft;
    if (m_robotItem->direction()==RobotItem::East)
        wall = wall || m_field[m_robotItem->scenePosition().y][m_robotItem->scenePosition().x].wallRight;
    if (wall) {
        m_robotItem->setBroken(true);
        e_lastError = Robot25D::WallCollisionError;
        emit sync();
        return false;
    }
    else {
        e_lastError = Robot25D::NoRuntimeError;
        Point2Di p;
        p.x = nextX;
        p.y = nextY;
        m_robotItem->moveTo(p);
        return true;
    }
}

void RobotView::turnLeft()
{
    e_lastError = Robot25D::NoRuntimeError;
    m_robotItem->turnLeft();
}

void RobotView::turnRight()
{
    e_lastError = Robot25D::NoRuntimeError;
    m_robotItem->turnRight();
}

void RobotView::doPaint()
{
    e_lastError = Robot25D::NoRuntimeError;
    m_robotItem->doPaint();
}

bool RobotView::isWall()
{
    bool v = false;
    Point2Di rp = m_robotItem->scenePosition();
    RobotCell cell = m_field[rp.y][rp.x];
    if (m_robotItem->direction()==RobotItem::North)
        v = cell.wallUp;
    if (m_robotItem->direction()==RobotItem::South)
        v = cell.wallDown;
    if (m_robotItem->direction()==RobotItem::East)
        v = cell.wallRight;
    if (m_robotItem->direction()==RobotItem::West)
        v = cell.wallLeft;
    return v;
}

bool RobotView::isPainted()
{
    Point2Di rp = m_robotItem->scenePosition();
    RobotCell cell = m_field[rp.y][rp.x];
    bool v = cell.painted;
    return v;
}

bool RobotView::isPainted(int x, int y) const
{
    if (y>m_field.size())
        return false;
    if (x>m_field[y].size())
        return false;
    RobotCell cell = m_field[y][x];
    return cell.painted;
}

bool RobotView::isPointed(int x, int y) const
{
    if (y>m_field.size())
        return false;
    if (x>m_field[y].size())
        return false;
    RobotCell cell = m_field[y][x];
    return cell.pointed;
}

int RobotView::sizeX() const
{
    if (m_field.size()==0)
        return 0;
    return m_field[0].size();
}

int RobotView::sizeY() const
{
    return m_field.size();
}

int RobotView::positionX() const
{
    Point2Di rp = m_robotItem->scenePosition();
    return rp.x;
}

int RobotView::positionY() const
{
    Point2Di rp = m_robotItem->scenePosition();
    return rp.y;
}

void RobotView::reset()
{
    for (int i=0;i<m_originalField.size(); i++)
    {
        for (int j=0;j<m_originalField[i].size(); j++) {
            updateCell(j,i,m_originalField[i][j].painted);
        }
    }
    m_robotItem->setAnimated(false);
    m_robotItem->setScenePosition(m_originalRobotPosition);
    m_robotItem->setDirection(m_originalRobotDirection);
    m_robotItem->setBroken(false);

}

bool RobotView::loadEnvironment(const Schema::Environment &env)
{
    m_field = QVector< QVector<RobotCell> > (env.size.height(), QVector<RobotCell>(env.size.width()) );
    // Initialize empty field
    for (int y=0; y<env.size.height(); y++) {
        for (int x=0; x<env.size.width(); x++) {
            QPoint coord(x,y);
            m_field[y][x].painted = env.painted.contains(coord);
            m_field[y][x].pointed = env.pointed.contains(coord);
            m_field[y][x].wallLeft = x==0;
            m_field[y][x].wallRight = x==env.size.width()-1;
            m_field[y][x].wallUp = y==0;
            m_field[y][x].wallDown = y==env.size.height()-1;
            m_field[y][x].cellItem = NULL;
        }
    }
    // Create vertical walls
    for (int y=0; y<env.size.height(); y++) {
        for (int x=1; x<env.size.width(); x++) {
            QPoint coord1(x-1, y);
            QPoint coord2(x,   y);
            QPair<QPoint,QPoint> variant1(coord1, coord2);
            QPair<QPoint,QPoint> variant2(coord2, coord1);
            bool wall = env.walls.contains(variant1) || env.walls.contains(variant2);
            m_field[y][x-1].wallRight = wall;
            m_field[y][x  ].wallLeft  = wall;
        }
    }
    // Create horizontal walls
    for (int x=0; x<env.size.width(); x++) {
        for (int y=1; y<env.size.height(); y++) {
            QPoint coord1(x, y-1);
            QPoint coord2(x, y  );
            QPair<QPoint,QPoint> variant1(coord1, coord2);
            QPair<QPoint,QPoint> variant2(coord2, coord1);
            bool wall = env.walls.contains(variant1) || env.walls.contains(variant2);
            m_field[y-1][x].wallDown = wall;
            m_field[y  ][x].wallUp   = wall;
        }
    }
    RobotItem::Direction dir = RobotItem::South;
    if (env.direction==Schema::Environment::North)
        dir = RobotItem::North;
    else if (env.direction==Schema::Environment::West)
        dir = RobotItem::West;
    else if (env.direction==Schema::Environment::East)
        dir = RobotItem::East;

    // Actual creation
    for (int i=0; i<m_field.size(); i++ ) {
        for (int j=0; j<m_field[0].size(); j++ ) {
            m_field[i][j].baseZOrder = 10.0*i+10.0*j;
        }
    }
    createField();
    createRobot(env.position.x(), env.position.y(), dir);
    m_originalField = m_field;
    if (!m_field.isEmpty()) {
        m_originalRobotDirection = m_robotItem->direction();
        m_originalRobotPosition = m_robotItem->scenePosition();
    }
    return true;
}

Schema::Environment RobotView::environment() const
{
    Schema::Environment env;
    env.size = QSize(m_field[0].size(), m_field.size());
    if (m_robotItem->direction()==RobotItem::North)
        env.direction = Schema::Environment::North;
    else if (m_robotItem->direction()==RobotItem::West)
        env.direction = Schema::Environment::West;
    else if (m_robotItem->direction()==RobotItem::East)
        env.direction = Schema::Environment::East;
    else
        env.direction = Schema::Environment::South;
    env.position = QPoint(m_robotItem->scenePosition().x, m_robotItem->scenePosition().y);

    for (int y=0; y<m_field.size(); y++) {
        for (int x=0; x<m_field[y].size(); x++) {
            if (m_field[y][x].painted) {
                env.painted.insert(QPoint(x,y));
            }
            if (m_field[y][x].pointed) {
                env.pointed.insert(QPoint(x,y));
            }
        }
    }
    // Save vertical walls
    for (int y=0; y<env.size.height(); y++) {
        for (int x=1; x<env.size.width(); x++) {
            if (m_field[y][x].wallLeft) {
                env.walls.insert(QPair<QPoint,QPoint>(QPoint(x-1, y),QPoint(x, y)));
            }
        }
    }
    // Save horizontal walls
    for (int x=0; x<env.size.width(); x++) {
        for (int y=1; y<env.size.height(); y++) {
            if (m_field[y][x].wallUp) {
                env.walls.insert(QPair<QPoint,QPoint>(QPoint(x, y-1),QPoint(x, y)));
            }
        }
    }
    return env;
}

quint16 RobotView::unpaintedPoints() const
{
    quint16 result = 0;
    for (int y=0; y<m_field.size(); y++) {
        for (int x=0; x<m_field[y].size(); x++) {
            if (m_field[y][x].pointed && !m_field[y][x].painted)
                result ++;
        }
    }
    return result;
}

QString RobotView::lastError(QLocale::Language language) const
{
    if (e_lastError==Robot25D::WallCollisionError) {
        if (language==QLocale::Russian)
            return QString::fromUtf8("Робот ударился об стену и разбился");
        else
            return "Wall collision caused to robot damage";
    }
    return "";
}





void RobotView::handleRobotEvaluationFinised()
{
//    qDebug() << __FUNCTION__;
    emit sync();
}

}
