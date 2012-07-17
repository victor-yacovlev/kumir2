/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#ifndef ROBOTMODULE_H
#define ROBOTMODULE_H

#include <QtCore>
#include <QtGui>
#include "extensionsystem/kplugin.h"
#include "robotmodulebase.h"

namespace ActorRobot {

    
    
#define LEFT_WALL 	0x1
#define RIGHT_WALL 	0x2
#define DOWN_WALL 	0x4
#define UP_WALL   	0x8
#define MAX_CLICK_DELTA 7
    
#define NO_CRASH 0
#define UP_CRASH 1
#define DOWN_CRASH 2
#define LEFT_CRASH 3
#define RIGHT_CRASH 4

    //Robot
    //#define FIELD_SIZE 20
#define FIELD_SIZE_SMALL 30
#define BUTTON_SIZE 20
    
#define NOT_LEFT_WALL   14
#define NOT_RIGHT_WALL   13
#define NOT_DOWN_WALL   11
#define NOT_UP_WALL   7
#define DEFAULT_X 16
#define DEFAULT_Y 9
#define BORT 5
#define MAX_SIZE 256
#define MIN_SIZE 3
    // NEW V.K. 05-10
#define MIN_TEMP -273 
#define DPANEL_HIGTH 0
    //#define PANEL_HIGTH 0
#define BTNXSDVIG 0
#define SEE_MODE 0
#define RUN_MODE 1
#define EDIT_MODE 2
#define ANALYZE_MODE 3
#define PAUSE_MODE 4
#define DEFAULT_SIZEX 400
#define DEFAULT_SIZEY 400    
    
   
    
    class SimpleRobot:
    public QGraphicsObject
    {
        Q_OBJECT;
        
    public:
        SimpleRobot(QGraphicsItem *parent = 0);
        QRectF boundingRect() const;
        
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget);
        QGraphicsPolygonItem* RoboItem();
        void setCrash(uint dirct);
        void move(QPoint point);
        //void show();
    signals:
        void moved(QPointF point);
    protected:
        void dragMoveEvent ( QGraphicsSceneDragDropEvent * event );
        void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
        void mousePressEvent ( QGraphicsSceneMouseEvent * event );
        void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
        void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    private:
        
        QGraphicsPolygonItem *Robot;
        bool ready;
        uint crash;
        
    };    

    
    
    class FieldItm: public QWidget
    {
        Q_OBJECT
    public:
        
        FieldItm(QWidget *parent = 0,QGraphicsScene* scene=0);
        
        
        ~FieldItm();
        FieldItm* Copy();
        void cleanSelf();
        
        
        void setLeftsepItem(FieldItm* ItmLeft);
        
        void setRightsepItem(FieldItm* ItmRight);
        
        void setUpsepItem(FieldItm* ItmUp);
        
        void setDownsepItem(FieldItm* ItmDown);
        
        void setWalls(int wallByte);
        
        int wallByte();
        
        void setUpLine(QGraphicsLineItem* Line,QPen pen);
        
        void showCharMark(qreal upLeftCornerX,qreal upLeftCornerY,int size);
        void showUpChar(qreal upLeftCornerX,qreal upLeftCornerY,int size);
        void showDownChar(qreal upLeftCornerX,qreal upLeftCornerY,int size);
        
        void showMark(qreal upLeftCornerX,qreal upLeftCornerY,int size);
        
        void setColorRect(QGraphicsRectItem* Rect);
        void setDownLine(QGraphicsLineItem* Line,QPen pen);
        void setLeftLine(QGraphicsLineItem* Line,QPen pen);
        void setRightLine(QGraphicsLineItem* Line,QPen pen);
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
        QGraphicsLineItem* UpWallLine();
        QGraphicsLineItem* DownWallLine();
        QGraphicsLineItem* LeftWallLine();
        QGraphicsLineItem* RightWallLine();
        bool isColored();
        
        void removeUpWall();
        
        void setUpWall(QGraphicsLineItem* Line,QPen pen);
        void setDownWall(QGraphicsLineItem* Line,QPen pen);
        void removeDownWall();
        
        void removeLeftWall();
        void removeRightWall();
        
        
        
        void setLeftWall(QGraphicsLineItem* Line,QPen pen);
        void setRightWall(QGraphicsLineItem* Line,QPen pen);
        
        void removeColor();
        
        void removeMark();
        
        void removeUpChar();
        
        void removeDownChar();
        bool emptyCell();
        void setScene(QGraphicsScene* scene);
        void wbWalls();
        void colorWalls();
        void setTextColor()
        {
            TextColor=QColor(sett->value("Robot/TextColor","#FFFFFF").toString());
        };
        
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
        
        QFont font;
        
        
        QPen wallPen;
        QGraphicsScene* Scene;
        QGraphicsLineItem* upWallLine;
        QGraphicsLineItem* downWallLine;
        QGraphicsLineItem* leftWallLine;
        QGraphicsLineItem* rightWallLine;
        QGraphicsRectItem * ColorRect;
        
        QGraphicsTextItem * upCharItm;
        QGraphicsTextItem * downCharItm;
        QGraphicsTextItem * markItm;
        QColor TextColor;
        
        FieldItm* sepItmUp;
        FieldItm* sepItmDown;
        FieldItm* sepItmLeft;
        FieldItm* sepItmRight;
        QSettings* sett;
    };
    
    class RoboField: public QGraphicsScene
    {
        Q_OBJECT
    public:
        RoboField(QWidget *parent);
        RoboField* Clone();
        ~RoboField();
        
    public:
        void drawField(uint cellSize);//TODO Document
        void destroyField();
        
        void destroyRobot();
        
        /**
         * Количество строк поля робота
         * @return количество строк
         */
        inline int rows() const { return Items.count(); }
        /**
         * Количество столбцов поля робота
         * @return количество столбцов
         */
        inline int columns() const { return Items.isEmpty()? 0 : Items.last().size();  }
        void drawNet();
        void setItem(FieldItm* item,uint str,uint stlb);
        void destroyNet();
        void destroyScene();
        void createField(int shirina,int visota);
        FieldItm* getFieldItem(int str,int stlb);
        void debug();
        
        int loadFromFile(QString fileName);
        int saveToFile(QString fileName);
        void createRobot();
        void UpdateColors();
        
        /**
         * Удаляет / ставит верхнюю стену
         * @param row Ряд
         * @param col столбец
         *
         */
        void reverseUpWall(int row,int col);
        
        /**
         * Удаляет / ставит нижнюю стену
         * @param row Ряд
         * @param col столбец
         *
         */
        void reverseDownWall(int row,int col);
        void reverseLeftWall(int row,int col);
        void reverseRightWall(int row,int col);
        
        void reverseColor(int row,int col);
        void reverseColorCurrent();
        
        void reverseMark(int row,int col);
        
        
        
        inline QList<QList<FieldItm * > > FieldItems() { return Items; }
        void setFieldItems(QList<QList<FieldItm * > > FieldItems);
        
        
        bool stepUp();
        bool stepDown();
        bool stepLeft();
        bool stepRight();
        void editField();
        void setEditMode(bool EditMode) { editMode=EditMode;
            LineColor = QColor(sett->value("Robot/LineColor","#C8C800").toString());
            WallColor=QColor(sett->value("Robot/WallColor","#C8C800").toString());
            EditColor=QColor(sett->value("Robot/EditColor","#00008C").toString());
            NormalColor=QColor(sett->value("Robot/NormalColor","#289628").toString());}
        void showCellDialog(FieldItm * cellClicked);
        inline FieldItm * currentCell() { return getFieldItem(robo_y,robo_x); }
        inline FieldItm * cellAt(int x,int y) { return getFieldItem(x,y); }
        
        inline void setMarkMode(bool isColor) { markMode=isColor; }
        
        inline bool WasEdit() const { return wasEdit; }
        
        void setRoboPos(int roboX,int roboY);
        inline int robotX() const { return robo_x; }
        inline int robotY() const { return robo_y; }
        
        
        inline void dropWasEdit() { wasEdit=false; }
        SimpleRobot * robot;
        void wbMode();
        void colorMode();
        public
        
    slots:
        void roboMoved(QPointF pos);
        void cellDialogOk();
    signals:
        void MousePress(qreal x,qreal y, bool Flag);
       // void was_edit();
        
        
        
    private:
        
        void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
        QList<QList<FieldItm * > > Items;
        QList<QGraphicsLineItem*> setka;
        //QGraphicsView * scena;
        bool editMode,markMode,wasEdit;
        QColor LineColor,WallColor,EditColor,NormalColor,Color;
        uint fieldSize;
        uint robo_x,robo_y;
        QWidget *Parent;
        QPen BortLine,StLine;
       // CellDialog* cellDialog;
        bool WasEditFlag;
        //TOOLS
        QPoint upLeftCorner(int str,int stlb);
       // RobotModule *m_robot;
        QSettings* sett;
    };
  
    
    
    class RobotModule
	: public RobotModuleBase
    {
        Q_OBJECT
    public:
        // Constructor
        RobotModule(ExtensionSystem::KPlugin * parent);
        public slots:
        // Reset actor state before program starts
        void reset();
        // Set animation flag
        void setAnimationEnabled(bool enabled);
        // Actor methods
        void runGoUp();
        void runGoDown();
        void runGoLeft();
        void runGoRight();
        void runDoPaint();
        bool runIsWallAtTop();
        bool runIsWallAtBottom();
        bool runIsWallAtLeft();
        bool runIsWallAtRight();
        
    public:
        // GUI access methods
        QWidget* mainWidget() const;
        QWidget* pultWidget() const;
        static QSettings * robotSettings();
        
    private:
        QWidget* m_mainWidget;
        QWidget* m_pultWidget;
        RoboField* field;
        RoboField * startField;
        bool animation;
        QGraphicsView * view;
    }; // RobotModule

} // ActorRobot

#endif // ROBOTMODULE_H
