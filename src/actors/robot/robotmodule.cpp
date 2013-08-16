/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#include <QtCore>
#include <QtGui>
#include <QtDeclarative>
#include <qmessagebox.h>
#include <algorithm>
#include "robotmodule.h"
#include "extensionsystem/pluginmanager.h"

namespace ActorRobot {


    ExtensionSystem::SettingsPtr RobotModule::robotSettings()
    {
        ExtensionSystem::PluginManager * pluginManager = ExtensionSystem::PluginManager::instance();
        ExtensionSystem::KPlugin * plugin = pluginManager->loadedPlugins("ActorRobot")[0];
        return pluginManager->settingsByObject(plugin);
    }

    FieldItm::FieldItm(QWidget *parent, QGraphicsScene *scene)
    {
        sett=RobotModule::robotSettings();
        Q_UNUSED(parent);
        upWallLine = NULL;
        downWallLine = NULL;
        leftWallLine = NULL;
        rightWallLine = NULL;
        sepItmUp=NULL;
        sepItmDown=NULL;
        sepItmLeft=NULL;
        sepItmRight=NULL;
        Scene=scene;
        upWall=false;
        downWall=false;
        leftWall=false;
        rightWall=false;
        IsColored=false;
        mark=false;
        ColorRect=NULL;
        upChar=' ';
        downChar=' ';
        upCharItm=NULL;
        downCharItm=NULL;
        markItm=NULL;
        radItm=NULL;
        tempItm=NULL;
        
        font.setPixelSize(9);
        font.setPointSize(9);
        font.setStyle(QFont::StyleNormal);
        font.setBold(true);
        font.setStyleHint(QFont::SansSerif);
        radiation=0;
        temperature=0;
        font.setWeight(2);
        TextColor=QColor(sett->value("Robot/TextColor","#FFFFFF").toString());
        upCharFld=NULL;
        downCharFld=NULL;
        
    }
    
    FieldItm::~FieldItm()
    {
        cleanSelf();
    }
    
    void FieldItm::cleanSelf()
    {
        if(upWallLine)
        {
            if(Scene)
                Scene->removeItem(upWallLine);
            if(upWallLine && upWallLine->scene()==Scene)delete upWallLine;
            upWallLine=NULL;
        }
        if(downWallLine)
        {
            if(Scene)
                if(downWallLine && downWallLine->scene()==Scene)        Scene->removeItem(downWallLine);
            delete downWallLine;
            downWallLine=NULL;
        }
        if(leftWallLine)
        {
            if(Scene)
                Scene->removeItem(leftWallLine);
            if(leftWallLine && leftWallLine->scene()==Scene)delete leftWallLine;
            leftWallLine=NULL;
        }
        if(rightWallLine)
        {
            if(Scene)
                Scene->removeItem(rightWallLine);
            if(rightWallLine && rightWallLine->scene()==Scene) delete rightWallLine;
            rightWallLine=NULL;
        }
        if(ColorRect)
        {
            if(Scene)
                Scene->removeItem(ColorRect);
            delete ColorRect;
            ColorRect=NULL;
        }
        if(upCharItm)
        {
            if(Scene)
                Scene->removeItem(upCharItm);
            delete upCharItm;
            upCharItm=NULL;
        }
        if(downCharItm)
        {
            if(Scene)
                Scene->removeItem(downCharItm);
            delete downCharItm;
            downCharItm=NULL;
        }
        if(markItm)
        {
            if(Scene)
                Scene->removeItem(markItm);
            delete markItm;
            markItm=NULL;
        }
        if(upCharFld)
        {
            if(Scene)Scene->removeItem(upCharFld);
            upCharFld=NULL;
        }
        if(downCharFld)
        {
            if(Scene)Scene->removeItem(downCharFld);
            downCharFld=NULL;
        }
        if(radItm)
        {
            if(Scene)Scene->removeItem(radItm);
            radItm=NULL;
        }
        if(tempItm)
        {
            if(Scene)Scene->removeItem(tempItm);
            tempItm=NULL;
        }
    }
    
    
    void FieldItm::setTextColor()
    {
        sett=RobotModule::robotSettings();
        TextColor=QColor(sett->value("Robot/TextColor","#FFFFFF").toString());
        
        
        upCharItm->setDefaultTextColor(TextColor);
        downCharItm->setDefaultTextColor(TextColor);

        
    };
    
    void FieldItm::setLeftsepItem(FieldItm *ItmLeft)
    {
        sepItmLeft=ItmLeft;
        ItmLeft->setRightsepItem(this);	//Obratnaya ssilka u soseda
    }
    
    void FieldItm::setRightsepItem(FieldItm *ItmRight)
    {
        sepItmRight=ItmRight;
    }
    
    void FieldItm::setUpsepItem(FieldItm *ItmUp)
    {
        sepItmUp=ItmUp;
        ItmUp->setDownsepItem(this);
    }
    
    void FieldItm::setDownsepItem(FieldItm *ItmDown)
    {
        sepItmDown=ItmDown;
    }
    
    void FieldItm::setWalls(int wallByte)
    {
        if((wallByte & UP_WALL) == UP_WALL)upWall=true;else upWall=false;
        if((wallByte & DOWN_WALL) == DOWN_WALL)downWall=true; else
            downWall=false;
        if((wallByte & LEFT_WALL) == LEFT_WALL)leftWall=true; else leftWall=false;
        if((wallByte & RIGHT_WALL) == RIGHT_WALL)rightWall=true; else rightWall=false;
    }
    
    int FieldItm::wallByte()
    {
        int toret=0;
        if(upWall)toret+=UP_WALL;
        if(downWall)toret+=DOWN_WALL;
        if(leftWall)toret+=LEFT_WALL;
        if(rightWall)toret+=RIGHT_WALL;
        return toret;
    }
    
    void FieldItm::setUpLine(QGraphicsLineItem *Line, QPen pen)
    {
        upWallLine=Line;
        upWallLine->setPen(pen);
        Scene->addItem(upWallLine);
        upWallLine->setZValue(1);
    }
    
    void FieldItm::showCharMark(qreal upLeftCornerX, qreal upLeftCornerY, int size)
    {
        showUpChar(upLeftCornerX,upLeftCornerY,size);
        showDownChar(upLeftCornerX,upLeftCornerY,size);
        showMark(upLeftCornerX,upLeftCornerY,size);
    }
    void FieldItm::showRTItm(qreal upLeftCornerX, qreal upLeftCornerY, int size,int mode)
    {
       
        if (radItm) {
            if (Scene)
                Scene->removeItem(radItm);
            delete radItm;
            radItm = NULL;
        }
        
        
        if (tempItm) {
            if (Scene)
                Scene->removeItem(tempItm);
            delete tempItm;
            tempItm = NULL;
        }
        
     
      // radItm=Scene->addRect(upLeftCornerX+1, upLeftCornerY+1, size*(radiation/99),size/5 );
        radItm=new EditLine();
        radItm->setRad();
        radItm->moveBy(upLeftCornerX+1, upLeftCornerY+1);
        radItm->setScale(0.25);
        radItm->setValue(radiation);
        // radItm=Scene->addItem(upLeftCornerX+1, upLeftCornerY+1, size*(radiation/99),size/5 );
       Scene->addItem(radItm);
      // radItm->setBrush(QBrush(Qt::yellow));
     //  radItm->setPos(upLeftCornerX+1,upLeftCornerY+1);
        radItm->setZValue(100);
        if(mode==RAD_MODE)radItm->show();
        else radItm->hide();
        
        
        
        tempItm=new EditLine();
        tempItm->moveBy(upLeftCornerX+1, upLeftCornerY+1);
        tempItm->setTemp();
        tempItm->setValue(temperature);
        Scene->addItem(tempItm);
         tempItm->setScale(0.25);
        tempItm->setZValue(100);
        if(mode==TEMP_MODE)tempItm->show();
        else tempItm->hide();
        //tempItm=Scene->addText("T: "+QString::number(temperature),font);
        //tempItm->setDefaultTextColor(TextColor);
        //tempItm->setPos(upLeftCornerX+1,upLeftCornerY+size-16);
        //tempItm->setZValue(1);
        

    }
    void FieldItm::hideRTItm()
    {
        if (radItm) {radItm->hide();};
        if(tempItm){tempItm->hide();};
    }
    void FieldItm::showCharFld(qreal upLeftCornerX,qreal upLeftCornerY,int field_size)
    {
        
         if(upCharFld) Scene->removeItem(upCharFld);
        upCharFld=new QGraphicsRectItem(upLeftCornerX+4,upLeftCornerY+4,7,11);
        upCharFld->setPen(QPen(Qt::NoPen));
        upCharFld->setBrush(QBrush(QColor(60,115,175)));
        Scene->addItem(upCharFld);
        upCharFld->setZValue(0.2);
        
         if(downCharFld) Scene->removeItem(downCharFld);
        downCharFld=new QGraphicsRectItem(upLeftCornerX+4,field_size+upLeftCornerY-14,7,11);
        downCharFld->setPen(QPen(Qt::NoPen));
        downCharFld->setBrush(QBrush(QColor(60,115,175)));
        Scene->addItem(downCharFld);
        downCharFld->setZValue(0.2);
    };
    void FieldItm::hideCharFld()
    {
       
       if(upCharFld!=NULL) Scene->removeItem(upCharFld);
        if(!Scene)
        {
        qDebug()<<"Scene is NULL!";
            return;
        }
       if(downCharFld)Scene->removeItem(downCharFld);
    };
    void FieldItm::showUpChar(qreal upLeftCornerX, qreal upLeftCornerY, int size)
    {
        Q_UNUSED(size);
        if (upCharItm) {
            if (Scene)
                Scene->removeItem(upCharItm);
            delete upCharItm;
            upCharItm = NULL;
        }
        if (upChar.isPrint() && upChar!=' ') {
            upCharItm=Scene->addText(upChar,font);
            upCharItm->setDefaultTextColor(TextColor);
            upCharItm->setPos(upLeftCornerX+1,upLeftCornerY+1);
            upCharItm->setZValue(1);
        }
    }
    
    void FieldItm::showDownChar(qreal upLeftCornerX, qreal upLeftCornerY, int size)
    {
        Q_UNUSED(size);
        if (downCharItm) {
            if (Scene) {
                Scene->removeItem(downCharItm);
            }
            delete downCharItm;
            downCharItm = NULL;
        }
        if (downChar.isPrint() && downChar!=' ') {
            downCharItm=Scene->addText(downChar,font);
            downCharItm->setDefaultTextColor(TextColor);
            downCharItm->setPos(upLeftCornerX+1,upLeftCornerY+size-16);
            downCharItm->setZValue(1);
        }
    }
    
    void FieldItm::showMark(qreal upLeftCornerX, qreal upLeftCornerY, int size)
    {
        if(mark)
        {
            if (markItm) {
                if (Scene) {
                    Scene->removeItem(markItm);
                }
                delete markItm;
                markItm = NULL;
            }
            markItm=Scene->addText(QChar(9787),font);
            markItm->setDefaultTextColor(TextColor);
            markItm->setPos(upLeftCornerX+size-(size/3)-3,upLeftCornerY-15+size);
            markItm->setZValue(1);
        }
    }
    
    void FieldItm::setColorRect(QGraphicsRectItem *Rect)
    {
        ColorRect = Rect;
        ColorRect->setPen(QPen("gray"));
        ColorRect->setBrush(QBrush(QColor("gray")));
        
        Scene->addItem(Rect);
        
        Rect->setZValue(0.2);
        IsColored=true;
    }
    
    void FieldItm::setDownLine(QGraphicsLineItem *Line, QPen pen)
    {
        downWallLine=Line;
        downWallLine->setPen(pen);
        Scene->addItem(downWallLine);
        downWallLine->setZValue(1);
    }
    
    void FieldItm::setLeftLine(QGraphicsLineItem *Line, QPen pen)
    {
        leftWallLine=Line;
        leftWallLine->setPen(pen);
        Scene->addItem(leftWallLine);
        leftWallLine->setZValue(1);
    }
    
    void FieldItm::setRightLine(QGraphicsLineItem *Line, QPen pen)
    {
        rightWallLine=Line;
        rightWallLine->setPen(pen);
        Scene->addItem(rightWallLine);
        
        rightWallLine->setZValue(1);
    }
    
    bool FieldItm::hasUpWall()
    {
        return upWall;
    }
    
    bool FieldItm::hasDownWall()
    {
        return downWall;
    }
    bool FieldItm::hasLeftWall()
    {
        return leftWall;
    }
    
    bool FieldItm::hasRightWall()
    {
        return rightWall;
    }
    
    bool FieldItm::canUp()
    {
        if(!hasUpSep())return false;
        return !(upWall || sepItmUp->hasDownWall());
    }
    
    bool FieldItm::canDown()
    {
        if(!hasDownSep())return false;
        return !(downWall || sepItmDown->hasUpWall());
    }
    
    bool FieldItm::canLeft()
    {
        if(!hasLeftSep())return false;
        return !(leftWall || sepItmLeft->hasRightWall());
    }
    
    bool FieldItm::canRight()
    {
        if(!hasRightSep())return false;
        return !(rightWall || sepItmRight->hasLeftWall());
    }
    
    bool FieldItm::hasUpSep()
    {
        if(sepItmUp)return true;return false;
    }
    
    bool FieldItm::hasDownSep()
    {
        if(sepItmDown)return true;return false;
    }
    
    bool FieldItm::hasLeftSep()
    {
        if(sepItmLeft)return true;return false;
    }
    
    bool FieldItm::hasRightSep()
    {
        if(sepItmRight)return true;return false;
    }
    
    QGraphicsLineItem* FieldItm::UpWallLine()
    {
        return upWallLine;
    }
    
    QGraphicsLineItem* FieldItm::DownWallLine()
    {
        return downWallLine;
    }
    
    QGraphicsLineItem* FieldItm::LeftWallLine()
    {
        return leftWallLine;
    }
    
    QGraphicsLineItem* FieldItm::RightWallLine()
    {
        return rightWallLine;
    }
    
    bool FieldItm::isColored()
    {
        return IsColored;
    }
    
    void FieldItm::removeUpWall()
    {
        if(upWallLine)
        {
            Scene->removeItem(upWallLine);
            delete upWallLine;
            upWallLine=NULL;
            qDebug("UwallRemoved");
        };
        upWall=false;
        if(hasUpSep())if(sepItmUp->hasDownWall())sepItmUp->removeDownWall();
    }
    
    void FieldItm::setUpWall(QGraphicsLineItem *Line, QPen pen)
    {
        upWall=true;
        setUpLine(Line,pen);
    }
    
    void FieldItm::setDownWall(QGraphicsLineItem *Line, QPen pen)
    {
        downWall=true;
        setDownLine(Line,pen);
    }
    
    void FieldItm::removeDownWall()
    {
        if(downWallLine)
        {
            Scene->removeItem(downWallLine);
            delete downWallLine;
            downWallLine=NULL;
            qDebug("DwallRemoved");
        };
        downWall=false;
        
        if(hasDownSep())if(sepItmDown->hasUpWall())sepItmDown->removeUpWall();
    }
    
    void FieldItm::removeLeftWall()
    {
        if(leftWallLine)
        {
            Scene->removeItem(leftWallLine);
            delete leftWallLine;
            leftWallLine=NULL;
            qDebug("LwallRemoved");
        };
        leftWall=false;
        
        if(hasLeftSep())if(sepItmLeft->hasRightWall())sepItmLeft->removeRightWall();
    }
    
    void FieldItm::removeRightWall()
    {
        if(rightWallLine)
        {
            QGraphicsScene* debug=rightWallLine->scene();
            Q_UNUSED(debug);
            rightWallLine->setVisible(false);
            Scene->removeItem(rightWallLine);
            delete rightWallLine;
            rightWallLine=NULL;
            qDebug("RwallRemoved");
        };
        rightWall=false;
        
        if(hasRightSep())if(sepItmRight->hasLeftWall())sepItmRight->removeLeftWall();
    }
    
    void FieldItm::setLeftWall(QGraphicsLineItem *Line, QPen pen)
    {
        leftWall=true;
        setLeftLine(Line,pen);
    }
    
    void FieldItm::setRightWall(QGraphicsLineItem *Line, QPen pen)
    {
        rightWall=true;
        setRightLine(Line,pen);
    }
    
    void FieldItm::removeColor()
    {
        
        if(ColorRect)
        {
            Scene->removeItem(ColorRect);
            delete ColorRect;
            ColorRect=NULL;
        };
        IsColored=false;
    }
    
    void FieldItm::removeMark()
    {
        if(markItm)
        {
            Scene->removeItem(markItm);
            delete markItm;
            markItm=NULL;
        };
        mark=false;
    }
    
    void FieldItm::removeUpChar()
    {
        if(upCharItm)
        {
            Scene->removeItem(upCharItm);
            delete upCharItm;
            upCharItm=NULL;
        };
        
        upChar=' ';
    }
    
    void FieldItm::removeDownChar()
    {
        if(downCharItm)
        {
            Scene->removeItem(downCharItm);
            delete downCharItm;
            downCharItm=NULL;
        };
        downChar=' ';
    }
    
    bool FieldItm::emptyCell()
    {
        return (!upWall && !downWall && !leftWall && !rightWall && !IsColored && !mark)
        &&(radiation==0)&&(temperature==0)&&(upChar==' ')&&(downChar==' ');
    }
    
    void FieldItm::setScene(QGraphicsScene *scene)
    {
        Scene=scene;
    }
    
    void FieldItm::wbWalls()
    {
        if(UpWallLine())
        {
            wallPen=UpWallLine()->pen();
            int w=3;
            if(!hasUpSep())w=4;
            UpWallLine()->setPen(QPen(QBrush(QColor("blue")),w));
        };
        if(DownWallLine())
        {
            wallPen=DownWallLine()->pen();
            int w=3;
            if(!hasDownSep())w=4;
            DownWallLine()->setPen(QPen(QBrush(QColor("blue")),w));
        };
        if(LeftWallLine())
        {
            wallPen=LeftWallLine()->pen();
            int w=3;
            if(!hasLeftSep())w=4;
            LeftWallLine()->setPen(QPen(QBrush(QColor("blue")),w));
        };
        if(RightWallLine())
        {
            wallPen=RightWallLine()->pen();
            int w=3;
            if(!hasRightSep())w=4;
            RightWallLine()->setPen(QPen(QBrush(QColor("blue")),w));
        };
        
        if(downCharItm)downCharItm->setDefaultTextColor("black");
        if(upCharItm)upCharItm->setDefaultTextColor("black");
        if(markItm)markItm->setDefaultTextColor("black");
    }
    
    void FieldItm::colorWalls()
    {
        if(UpWallLine())
        {
            if(!hasUpSep())wallPen.setWidth(4);else wallPen.setWidth(3);
            UpWallLine()->setPen(wallPen);
        };
        if(DownWallLine())
        {
            if(!hasDownSep())wallPen.setWidth(4);else wallPen.setWidth(3);
            DownWallLine()->setPen(wallPen);
        };
        if(LeftWallLine())
        {
            if(!hasLeftSep())wallPen.setWidth(4);else wallPen.setWidth(3);
            LeftWallLine()->setPen(wallPen);
        };
        if(RightWallLine())
        {
            if(!hasRightSep())wallPen.setWidth(4);else wallPen.setWidth(3);
            RightWallLine()->setPen(wallPen);
        };
        if(downCharItm)downCharItm->setDefaultTextColor("white");
        if(upCharItm) upCharItm->setDefaultTextColor("white");
        if(markItm)markItm->setDefaultTextColor("white");
    }
    
    
    
    
    FieldItm* FieldItm::Copy()
    {
        FieldItm* copy=new FieldItm();
        copy->leftWall=leftWall;
        copy->rightWall=rightWall;
        copy->upWall=upWall;
        copy->downWall=downWall;
        
        copy->upChar=upChar;
        copy->downChar=downChar;
        copy->mark=mark;
        copy->radiation=radiation;
        copy->temperature=temperature;
        copy->IsColored=IsColored;
        copy->upCharFld=upCharFld;
        return copy;
    };
    
    RoboField::~RoboField()
    {
        destroyField();
        destroyRobot();
        destroyNet();
        destroyScene();
        for (int i=0; i<Items.size(); i++) {
            for (int j=0; j<Items[i].size(); j++) {
                if (Items[i][j])
                    delete Items[i][j];
            }
        }
        Items.clear();
    }
    
    RoboField::RoboField(QWidget *parent)
	: QGraphicsScene(parent)
	
    
    {
        old_cell=QPair<int,int>(-1,-1);
        pressed=false;
        sett=RobotModule::robotSettings();
//        QString className = sett->metaObject()->className();
        Parent=parent;
        mode=0;
        LineColor = QColor(sett->value("LineColor","#C8C800").toString());
        WallColor=QColor(sett->value("WallColor","#C8C800").toString());
        EditColor=QColor(sett->value("EditColor","#00008C").toString());
        NormalColor=QColor(sett->value("NormalColor","#289628").toString());
        showLine=QPen(QColor(0,255,0,125));
        showLine.setWidth(3);
        timer=new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(timerTic()));
        fieldSize=30;
        this->setItemIndexMethod(NoIndex);
        robot=NULL;
        markMode=true;

        wasEdit=false;
        showWall=new QGraphicsLineItem(0,0,0,0);
        this->addItem(showWall);
      //  textEditMode=false;
       
        keyCursor=new QGraphicsLineItem(0,0,0,0);
        this->addItem(keyCursor); 
        keyCursor->hide();
        radSpinBox=new QDoubleSpinBox();
        radSpinBox->setRange(0, 99);
        tempSpinBox=new QSpinBox();
        tempSpinBox->setRange(0, 171);
        tempSpinBox->setValue(77);
        radSpinBox->setValue(55);
        
    };
  void RoboField::setMode( int Mode) 
    { 
        mode=Mode;
        sett=RobotModule::robotSettings();
        QGraphicsView * view=views().first();
       if(mode==NORMAL_MODE)
       {
              if(this->items().indexOf(keyCursor)>-1)this->removeItem(keyCursor);
       
           radSpinBox->hide();
           tempSpinBox->hide();
           redrawEditFields();
           redrawRTFields();  
           view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
           view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
       }
        if(mode==NEDIT_MODE)
        {
             if(this->items().indexOf(keyCursor)>-1)this->removeItem(keyCursor);
          radSpinBox->hide();
          tempSpinBox->hide();
          redrawEditFields();
          redrawRTFields();  
          view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
          view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);                                    
        }
        
        if(mode==RAD_MODE)
        {
               if(this->items().indexOf(keyCursor)>-1)this->removeItem(keyCursor);
          
            tempSpinBox->hide();
            radSpinBox->setParent(view);
            radSpinBox->move(100,2);
            
            radSpinBox->show();
            clickCell=QPair<int,int>(-1,-1);
          
            
            
            redrawEditFields();
            redrawRTFields();  
            
        }
        if(mode==TEMP_MODE)
        {
            if(this->items().indexOf(keyCursor)>-1)this->removeItem(keyCursor);
         
            radSpinBox->hide();
            tempSpinBox->setParent(view);
            tempSpinBox->move(100,2);
            
            tempSpinBox->show();
            clickCell=QPair<int,int>(-1,-1);
            
            
            
            redrawEditFields();
            redrawRTFields();  
            
        }
        if(mode==TEXT_MODE)
        {
            tempSpinBox->hide();
            radSpinBox->hide();
            redrawRTFields();
            setTextEditMode(true);
        }
        LineColor = QColor(sett->value("LineColor","#C8C800").toString());
        WallColor=QColor(sett->value("WallColor","#C8C800").toString());
        EditColor=QColor(sett->value("EditColor","#00008C").toString());
        NormalColor=QColor(sett->value("NormalColor","#289628").toString());
        
        view->repaint();
        
    }
    void RoboField::editField()
    {
       mode=NEDIT_MODE;
     //   connect(cellDialog->buttonBox,SIGNAL(accepted()),this,SLOT(cellDialogOk()));
    };
    /**
     * Создание пустого поля робота
     * @param x количество строк
     * @param y количество столбцов
     */
    void RoboField::createField(int x,int y)
    {
        destroyNet();
        destroyField();
        destroyRobot();
        //qDebug("Before items destroy");
        while (Items.count()>0)Items.removeFirst();
        //qDebug("After items destroy");
        for(int i=0;i<x;i++)
        {
            QList<FieldItm*> row;
            for(int j=0;j<y;j++)
            {
                
                row.append(new FieldItm(0,this));
                FieldItm* thisItm=row.last();
                if(j>0)thisItm->setLeftsepItem(row[j-1]);//Устанавливаем соседей
                if(i>0)thisItm->setUpsepItem(Items[i-1].at(j));//Устанавливаем соседей
            };
            Items.append(row);
        };
        wasEdit=true;
    };
    
    QPoint RoboField::upLeftCorner(int str,int stlb)
    {
        int ddx = BORT-2;
        return QPoint(stlb*fieldSize+ddx,str*fieldSize);
    };
    
    FieldItm* RoboField::getFieldItem(int str,int stlb)
    {
        if(rows()<str) {
            qDebug("RoboField:rows()<str");
            return NULL;
        };
        if(columns()<stlb) {
            qDebug("RoboField:columns()<str");
            return NULL;
        };
        
        if (str>=0 && stlb>=0 && str<Items.size() && stlb<Items[str].size())
            return Items[str].at(stlb);
        else
            return NULL;
        
    };
    void RoboField::drawField(uint FieldSize)
    {
        sett=RobotModule::robotSettings();
        keyCursor=new QGraphicsLineItem(0,0,0,0);
        this->addItem(keyCursor);
        keyCursor->hide();
        if(rows()<1||columns()<1)return;
        destroyNet();
        destroyField();
        // clear();
        //debug();
        showWall = new QGraphicsLineItem(0,0,0,0);
        if(!(mode>0))Color = NormalColor;//Normal Color
        else Color=EditColor;//Edit Color
        this->setBackgroundBrush (QBrush(Color));
        fieldSize=FieldSize;
        drawNet();
        
        BortLine = QPen(WallColor,4);
        StLine=QPen(LineColor,3);
        qDebug()<<"Rows"<<rows()<< "Cols:"<<columns();
        //if(rows()==2)return;
        for(int i=0;i<rows();i++) //Cikl po kletkam
        {
            QList<FieldItm*>* row=&Items[i];
            for(int j=0;j<columns();j++)
            {
               
                //Отрисовываем бордюры
                row->at(j)->setScene(this);
//                if(textEditMode)row->at(j)->showCharFld(upLeftCorner(i,j).x(),
//                                        upLeftCorner(i,j).y(),fieldSize);
//                else row->at(j)->hideCharFld();
                
                
                if(!row->at(j)->hasUpSep())
                    row->at(j)->setUpLine(
                                          new QGraphicsLineItem(upLeftCorner(i,j).x(),
                                                                upLeftCorner(i,j).y(),
                                                                upLeftCorner(i,j).x()+fieldSize,
                                                                upLeftCorner(i,j).y()),BortLine);
                
                if(!row->at(j)->hasDownSep())
                    row->at(j)->setDownLine(
                                            new QGraphicsLineItem(upLeftCorner(i,j).x(),
                                                                  upLeftCorner(i,j).y()+fieldSize,
                                                                  upLeftCorner(i,j).x()+fieldSize,
                                                                  upLeftCorner(i,j).y()+fieldSize),BortLine);
                
                
                if(!row->at(j)->hasLeftSep())
                {
                    row->at(j)->setLeftLine(
                                            new QGraphicsLineItem(upLeftCorner(i,j).x(),
                                                                  upLeftCorner(i,j).y(),
                                                                  upLeftCorner(i,j).x(),
                                                                  upLeftCorner(i,j).y()+fieldSize),BortLine);
                    //qDebug()<<"Line "<<row->at(j)->leftWallLine <<"Scene"<<row->at(j)->leftWallLine->scene();
                };
                if(!row->at(j)->hasRightSep())
                    row->at(j)->setRightLine(
                                             new QGraphicsLineItem(upLeftCorner(i,j).x()+fieldSize,
                                                                   upLeftCorner(i,j).y(),
                                                                   upLeftCorner(i,j).x()+fieldSize,
                                                                   upLeftCorner(i,j).y()+fieldSize),BortLine);
                if(row->at(j)->hasDownWall())
                {
                    row->at(j)->setDownLine(
                                            new QGraphicsLineItem(upLeftCorner(i,j).x(),
                                                                  upLeftCorner(i,j).y()+fieldSize,
                                                                  upLeftCorner(i,j).x()+fieldSize,
                                                                  upLeftCorner(i,j).y()+fieldSize),StLine);
                    
                };
                if(row->at(j)->hasUpWall())
                {
                    row->at(j)->setUpLine(
                                          new QGraphicsLineItem(upLeftCorner(i,j).x(),
                                                                upLeftCorner(i,j).y(),
                                                                upLeftCorner(i,j).x()+fieldSize,
                                                                upLeftCorner(i,j).y()),StLine);
                    
                };
                if(row->at(j)->hasLeftWall())
                {
                    row->at(j)->setLeftLine(
                                            new QGraphicsLineItem(upLeftCorner(i,j).x(),
                                                                  upLeftCorner(i,j).y(),
                                                                  upLeftCorner(i,j).x(),
                                                                  upLeftCorner(i,j).y()+fieldSize),StLine);
                    
                };
                if(row->at(j)->hasRightWall())
                {
                    row->at(j)->setRightLine(
                                             new QGraphicsLineItem(upLeftCorner(i,j).x()+fieldSize,
                                                                   upLeftCorner(i,j).y(),
                                                                   upLeftCorner(i,j).x()+fieldSize,
                                                                   upLeftCorner(i,j).y()+fieldSize),StLine);
                    
                };
                if(row->at(j)->isColored())
                {
                    row->at(j)->setColorRect(
                                             new QGraphicsRectItem(upLeftCorner(i,j).x(),
                                                                   upLeftCorner(i,j).y(),
                                                                   fieldSize,
                                                                   fieldSize));
                    
                };
                
                row->at(j)->showCharMark(upLeftCorner(i,j).x(),upLeftCorner(i,j).y(),fieldSize);
                
                
            };
        };
        destroyRobot();
        createRobot();
    }
    
    void RoboField::destroyField()
    {
        qDebug()<<"cols"<<columns();
        for(int i=0;i<columns();i++)
        {
            for(int j=0;j<rows();j++)  {
                FieldItm * itm=getFieldItem(j,i);
                if(itm)itm->cleanSelf();
            }
        };
        
        clear();
        setka.clear();
        robot=NULL;
        this->update();
    }
   void RoboField::setTextEditMode(bool flag)
    {
        clickCell=QPair<int,int>(-1,-1);
        if(flag)
        {
            keyCursor= new QGraphicsLineItem(0,0,0,0);
            this->addItem(keyCursor);
        }
       
       
       // destroyField();
       // drawField(FIELD_SIZE_SMALL);

        if(mode!=TEXT_MODE && !flag)
        {timer->stop();
        if(keyCursor)keyCursor->hide();
        };
        radSpinBox->hide(); 
        redrawEditFields();
        redrawRTFields();
    };

    
    void RoboField::destroyRobot()
    {
        if(robot)
        {
            removeItem(robot);
            delete robot;
            robot=NULL;
        }
    }
    
    void RoboField::destroyNet()
    {
        
        for(int i=0;i<setka.count();i++)
            this->removeItem(setka[i]);
        setka.clear();
    }
    void RoboField::redrawEditFields()
    {
        for(int i=0;i<rows();i++) //Cikl po kletkam
        {
            QList<FieldItm*>* row=&Items[i];
            for(int j=0;j<columns();j++)
            {
      
                if(mode==TEXT_MODE)row->at(j)->showCharFld(upLeftCorner(i,j).x(),
                                                        upLeftCorner(i,j).y(),FIELD_SIZE_SMALL);
                else row->at(j)->hideCharFld();
                
            };
        };
    };
    void RoboField::redrawRTFields()
    {
        for(int i=0;i<rows();i++) //Cikl po kletkam
        {
            QList<FieldItm*>* row=&Items[i];
            for(int j=0;j<columns();j++)
            {
                
                if(mode==RAD_MODE || mode==TEMP_MODE)row->at(j)->showRTItm(upLeftCorner(i,j).x(),
                                                        upLeftCorner(i,j).y(),FIELD_SIZE_SMALL,mode);
                else row->at(j)->hideRTItm();
                
            };
        };
    };
    void RoboField::destroyScene()
    {
        QList<QGraphicsItem*> items=this->items();
        while(items.count()>0)
        {
            this->removeItem(items.first());
            items=this->items();
        }
        clickCell=QPair<int,int>(-1,-1);
    }
    
    void RoboField::debug()
    {
        QList<QGraphicsItem*> items=this->items();
        for(int i=0;i<items.count();i++)
            qDebug()<<"Scene debug"<<items[i];
    }
    
    void RoboField::createRobot()
    {
        robot=new SimpleRobot();
        this->addItem(robot);
        robot->setPos(upLeftCorner(robo_y,robo_x).x(),upLeftCorner(robo_y,robo_x).y());
        connect(robot,SIGNAL(moved(QPointF)),this,SLOT(roboMoved(QPointF)));
    }
    
    void RoboField::reverseUpWall(int row, int col)
    {
        if(!getFieldItem(row,col)->hasUpSep()){
            return;qDebug("!UpSep");
        }
        if(!getFieldItem(row,col)->canUp())
        {
            getFieldItem(row,col)->removeUpWall();
            qDebug("removeUp");
        } else {
            getFieldItem(row,col)->setUpWall(new QGraphicsLineItem(upLeftCorner(row,col).x(),
                                                                   upLeftCorner(row,col).y(),
                                                                   upLeftCorner(row,col).x()+fieldSize,
                                                                   upLeftCorner(row,col).y()),StLine);
        }
        wasEdit=true;
    }
    
    void RoboField::reverseDownWall(int row, int col)
    {
        if(!getFieldItem(row,col)->hasDownSep()) {
            return;
        }
        if(!getFieldItem(row,col)->canDown())
        {
            getFieldItem(row,col)->removeDownWall();
        }
        else {
            getFieldItem(row,col)->setDownWall(new QGraphicsLineItem(upLeftCorner(row,col).x(),
                                                                     upLeftCorner(row,col).y()+fieldSize,
                                                                     upLeftCorner(row,col).x()+fieldSize,
                                                                     upLeftCorner(row,col).y()+fieldSize),StLine);
        }
        wasEdit=true;
    }
    
    void RoboField::reverseLeftWall(int row, int col)
    {
        if(!getFieldItem(row,col)->hasLeftSep())
            return;
        if(!getFieldItem(row,col)->canLeft())
        {
            getFieldItem(row,col)->removeLeftWall();
        }
        else {
            getFieldItem(row,col)->setLeftWall(new QGraphicsLineItem(upLeftCorner(row,col).x(),
                                                                     upLeftCorner(row,col).y(),
                                                                     upLeftCorner(row,col).x(),
                                                                     upLeftCorner(row,col).y()+fieldSize),StLine);
        }
        wasEdit=true;
    }
    
    void RoboField::reverseRightWall(int row, int col)
    {
        if(!getFieldItem(row,col)->hasRightSep())
            return;
        if(!getFieldItem(row,col)->canRight())
        {
            getFieldItem(row,col)->removeRightWall();
        }
        else {
            getFieldItem(row,col)->setRightWall(new QGraphicsLineItem(upLeftCorner(row,col).x()+fieldSize,
                                                                      upLeftCorner(row,col).y(),
                                                                      upLeftCorner(row,col).x()+fieldSize,
                                                                      upLeftCorner(row,col).y()+fieldSize),StLine);
        }
        wasEdit=true;
    }
    
    
    
    void RoboField::showUpWall(int row, int col)
    {
       
        this->removeItem(showWall);
        delete showWall;
        if(mode>NEDIT_MODE)
          {
              showWall=new QGraphicsLineItem(0,0,0,0);
              this->addItem(showWall);
              return;
              
          }
        showWall=new QGraphicsLineItem(upLeftCorner(row,col).x(),
                                                                   upLeftCorner(row,col).y(),
                                                                   upLeftCorner(row,col).x()+fieldSize,
                                                                   upLeftCorner(row,col).y());
        
        showWall->setPen(showLine);
        showWall->setZValue(200);

        this->addItem(showWall);
    qDebug()<<"Show up wall";
    }
    
    void RoboField::showDownWall(int row, int col)
    {
        this->removeItem(showWall);
        delete showWall;
        if(mode>NEDIT_MODE)
        {
            showWall=new QGraphicsLineItem(0,0,0,0);
            this->addItem(showWall);
            return;
        }
        showWall=new QGraphicsLineItem(upLeftCorner(row,col).x(),
                                                                     upLeftCorner(row,col).y()+fieldSize,
                                                                     upLeftCorner(row,col).x()+fieldSize,
                                                                     upLeftCorner(row,col).y()+fieldSize);
        
        showWall->setPen(showLine);
        showWall->setZValue(200);
        
        this->addItem(showWall);
       
    }
    
    void RoboField::showLeftWall(int row, int col)
    {
        this->removeItem(showWall);
       if(showWall) delete showWall;
        if(mode>NEDIT_MODE)
        {
            showWall=new QGraphicsLineItem(0,0,0,0);
            this->addItem(showWall);
            return;
        };
        showWall=new QGraphicsLineItem(upLeftCorner(row,col).x(),
                                                                     upLeftCorner(row,col).y(),
                                                                     upLeftCorner(row,col).x(),
                                                                     upLeftCorner(row,col).y()+fieldSize);
      
       
        showWall->setPen(showLine);
        showWall->setZValue(200);
        
        this->addItem(showWall);
    }
    
    void RoboField::showRightWall(int row, int col)
    {
        this->removeItem(showWall);
        delete showWall;
        
        if(mode>NEDIT_MODE)
        {
            showWall=new QGraphicsLineItem(0,0,0,0);
            this->addItem(showWall);
            return;
            
        };
        showWall=new QGraphicsLineItem(upLeftCorner(row,col).x()+fieldSize,
                                                                      upLeftCorner(row,col).y(),
                                                                      upLeftCorner(row,col).x()+fieldSize,
                                                                      upLeftCorner(row,col).y()+fieldSize);
        
        showWall->setPen(showLine);
        showWall->setZValue(200);
        
        this->addItem(showWall);
    }
    

    
    
    void RoboField::reverseColor(int row, int col)
    {
        if(getFieldItem(row,col)->isColored())
        {
            getFieldItem(row,col)->removeColor();
        }else
        {
            getFieldItem(row,col)->setColorRect(
                                                new QGraphicsRectItem(upLeftCorner(row,col).x(),
                                                                      upLeftCorner(row,col).y(),
                                                                      fieldSize,
                                                                      fieldSize));
        };
        wasEdit=true;
    }
    
    void RoboField::reverseColorCurrent()
    {
        reverseColor(robo_y,robo_x);
        wasEdit=true;
    }
    
    void RoboField::reverseMark(int row, int col)
    {
        if(getFieldItem(row,col)->mark)
        {
            getFieldItem(row,col)->removeMark();
        }
        else {
            getFieldItem(row,col)->mark=true;
            getFieldItem(row,col)->showCharMark(upLeftCorner(row,col).x(),upLeftCorner(row,col).y(),fieldSize);
        }
        wasEdit=true;
    }
    
    void RoboField::setRoboPos(int roboX, int roboY)
    {
        robo_x=roboX;
        robo_y=roboY;
    }
    

    
    void RoboField::roboMoved(QPointF pos)
    {
        int roboRow=pos.y() / FIELD_SIZE_SMALL;
        int roboCol=pos.x() / FIELD_SIZE_SMALL;
        if(roboRow>=rows())roboRow=rows()-1;
        if(roboRow<0)roboRow=0;
        if(roboCol>=columns())roboCol=columns()-1;
        if(roboCol<0)roboCol=0;
        robot->setPos(upLeftCorner(roboRow,roboCol).x(),upLeftCorner(roboRow,roboCol).y());
        robo_x=roboCol;robo_y=roboRow;
        wasEdit=true;
    };
    
    void RoboField::UpdateColors()
    {
        LineColor = QColor(sett->value("LineColor","#C8C800").toString());
        WallColor=QColor(sett->value("WallColor","#C8C800").toString());
        EditColor=QColor(sett->value("EditColor","#00008C").toString());
        NormalColor=QColor(sett->value("NormalColor","#289628").toString());
        
        destroyNet();
        destroyField();
        drawNet();
        drawField(FIELD_SIZE_SMALL);
    };
    
    void RoboField::drawNet()
    {
        QPen Pen,PenError;
        int ddx = 2+FIELD_SIZE_SMALL/2;
        qDebug()<<"Bort "<<BORT;
        int infin;
        
        
        
        Pen = QPen(LineColor,1);
        PenError = QPen(LineColor,1);
        infin = 5*BORT+10;
        
        for (int i = -1; i < columns(); i++) //Vertikalnie linii
        {
            setka.append(this->addLine(i * FIELD_SIZE_SMALL+ddx+FIELD_SIZE_SMALL/2+2 , -FIELD_SIZE_SMALL , i * FIELD_SIZE_SMALL+ddx+2+FIELD_SIZE_SMALL/2,(rows()+1) * FIELD_SIZE_SMALL,Pen ));
            setka.last()->setZValue(0.5);
        }
        
        for (int i = -1; i < rows(); i++)//Horizontalnie linii
        {
            setka.append(this->addLine(-FIELD_SIZE_SMALL, i * FIELD_SIZE_SMALL+ddx-1+FIELD_SIZE_SMALL/2 ,(columns()+1 )* FIELD_SIZE_SMALL, i * FIELD_SIZE_SMALL+ddx-1+FIELD_SIZE_SMALL/2,Pen));
            setka.last()->setZValue(0.5);
        }
       
    };
    qreal RoboField::m_height()
    {
        return FIELD_SIZE_SMALL*  rows(); 
    }
    qreal RoboField::m_width()
    {
        return FIELD_SIZE_SMALL*  rows(); 
    }
    /**
     * Загрузка обстановки из файла, отображение не производится.
     * @param fileName Имя файла
     * @return Код ошибки
     * 1 - Ошибка чтения файла
     * 2 - Пустой файл
     * 3 - Ошибка чтения размеров обстановки
     * 4 - Неверные размеры поля
     * 5 - Пустя строка
     * 6 - Неверное положение робота
     * 0< - Ошибка в строке
     * 10 - Ошибки основного прогона
     */
    int RoboField::loadFromDataStream(QIODevice * l_File)
    {
        
        //destroyField();
     
        
        
        QString tmp = "";
 
        
        

        
        
        
        
        int NStrok;
        NStrok = 0;
        QString l_String;
        //	long l_Err;
        int CurX,CurY;
        int SizeX, SizeY;
        destroyField();
        // Тестовый прогон
        
        if  (!l_File->open(QIODevice::ReadOnly))
        {
            
            return 1;
        }
        
        
        
        // 1 stroka - razmery polya
        tmp = l_File->readLine();
        //QMessageBox::information( 0, "", tmp, 0,0,0);
        
        
        if (tmp.isNull()||tmp.isEmpty())
        {
            l_File->close();
            
            return 2;
        }
        
        //QMessageBox::information( 0, "", tmp, 0,0,0);
        
        while (tmp.left(1) == ";" || tmp == "")
        {
            tmp = l_File->readLine();
            NStrok++;
            if (tmp.isNull())
            {
                return 1;
            }
        }
        tmp = tmp.simplified();
        QStringList l_List = tmp.split(' ');
        
        if (l_List.count() != 2)
        {
            l_File->close();
            
            
            return 3;
        }
        
        SizeX = (l_List[0]).toInt();
        SizeY = (l_List[1]).toInt();
        
        if ( (SizeX<= 0) || (SizeY <= 0) )
        {
            return 4;
        }
        //            field.destroyField();
        // field.createField(l_List[0].toInt(),l_List[1].toInt());
        
        // Вторая строка - положение робота
        
        tmp = l_File->readLine();
        
        
        
        if (tmp.isNull())
        {
            l_File->close();
            
            return 5;
        }
        
        
        
        while (tmp.left(1) == ";" || tmp == "")
        {
            tmp = l_File->readLine();
            NStrok++;
            if (tmp.isNull())
            {
               l_File->close();
                
                return 5;
            }
        }
        tmp = tmp.simplified();
        l_List = tmp.split(' ');
        
        // koordinaty robota
        // proverka
        if ((l_List[0]).toInt() < 0 || (l_List[1]).toInt() < 0)
        {
            
            
            l_File->close();return 6;
        }
        
        if ((l_List[0]).toInt() > SizeX || (l_List[1]).toInt() > SizeY )
        {
            
            l_File->close(); return 6;
        }
        
        
        //	m_DefaultSett = l_Sett;
        
        while (!l_File->atEnd())
        {
            //l_Err = l_File.readLine(l_String, 255);
            tmp = QString::fromUtf8(l_File->readLine());
            NStrok++;
            if (tmp.isNull())
            {
                
                return 5;
            }
            if (tmp.left(1) == ";" || tmp == "")
            {
                continue;
            }
            tmp = tmp.simplified();
            l_List = tmp.split(' ');
            if (l_List.count() == 0)continue;
            
            if (l_List.count() > 9 )
            {
                
                l_File->close();
                return -NStrok;
            }
            if(l_List.count()<6)
            {
                l_File->close();
                qDebug("N Lexem<6");
                return -NStrok;
            };
            bool ok;
            CurX = l_List[0].toInt(&ok);
            if(!ok)
            {
                l_File->close();
                qDebug("Bad cur X<6");
                return -NStrok;
            };
            
            CurY = l_List[1].toInt(&ok);
            
            if(!ok){
                l_File->close();
                qDebug("Bad curY <6");
                return -NStrok;
            };
            
            if (CurX < 0 || CurX > SizeX || CurY < 0 || CurY > SizeY)
            {
                
                l_File->close(); return -NStrok;
            }
            
            if (l_List[4].toFloat() < 0)
            {
                
                l_File->close(); return -NStrok;
            }
            
            
            if (l_List[5].toFloat() < MIN_TEMP)
            {
                
               l_File->close(); return -NStrok;
            }
            
            
            if (l_List.count() >= 7)
            {
                
                QString tmp1 = l_List[6];
                //dlina lexemy dolzna ravnyatsa 1
                if (!(tmp1.length() == 1))
                {
                    
                    l_File->close(); return -NStrok;
                }
                
                
            }
            
            
            if (l_List.count() >= 8)
            {
                
                QString tmp1 = l_List[7];
                //dlina lexemy dolzna ravnyatsa 1
                if (!(tmp1.length() == 1))
                {
                    l_File->close(); return -NStrok;
                }
                
                
            }
            
        }
        l_File->close();
        
        //реальный прогон
        //destroyField();
        
        if  (!l_File->open(QIODevice::ReadOnly))
        {
            
            return 10;
        }
        
        
        
        // 1 stroka - razmery polya
        tmp = l_File->readLine();
        
        if (tmp.isNull())
        {
            l_File->close();
            
            return 10;
        }
        
        
        while (tmp.left(1) == ";" || tmp == "")
        {
            tmp = QString::fromUtf8(l_File->readLine());
            NStrok++;
            if (tmp.isNull())
            {
                l_File->close();
                
                return 10;
            }
        }
        tmp = tmp.simplified();
        l_List = tmp.split(' ');
        
        if (l_List.count() != 2)
        {
            l_File->close();
            
            return -NStrok;
        }
        
        SizeX = (l_List[0]).toInt();
        SizeY = (l_List[1]).toInt();
        // 	 //NEW ROBO CODE
        createField(SizeY,SizeX);
        
        //END
        if ((l_List[0]).toInt() <= 0 || (l_List[1]).toInt() <= 0)
        {
            
            l_File->close();
            return - NStrok;
        }
        
        
        // Вторая строка - положение робота
        
        tmp = l_File->readLine();
        
        
        
        if (tmp.isNull())
        {
            l_File->close();
            
            return 10;
        }
        
        
        
        while (tmp.left(1) == ";" || tmp == "")
        {
            tmp = l_File->readLine();
            NStrok++;
            if (tmp.isNull())
            {
                l_File->close();
                
                return 10;
            }
        }
        tmp = tmp.simplified();
        l_List = tmp.split(' ');
        
        // koordinaty robota
        
        if ((l_List[0]).toInt() < 0 || (l_List[1]).toInt() < 0)
        {
            
           l_File->close();return - NStrok;
        }
        
        if ((l_List[0]).toInt() > SizeX || (l_List[1]).toInt() > SizeY )
        {
            
            l_File->close(); return - NStrok;
        }
        
        robo_x = (l_List[0]).toInt();
        robo_y = (l_List[1]).toInt();
        
        //InitialX = m_x;
        //InitialY = m_y;
        
        
        
        
        //	delete []m_FieldDubl;
        
        
        
        while (!l_File->atEnd())
        {
            tmp = QString::fromUtf8(l_File->readLine());
            NStrok++;
            if (tmp.isNull())
            {
                
                l_File->close();
                return 10;
            }
            if (tmp.left(1) == ";" || tmp == "")
            {
                continue;
            }
            tmp = tmp.simplified();
            l_List = tmp.split(' ');
            if (l_List.count() == 0)continue;
            
            if (l_List.count() > 9)
            {
                
                l_File->close();
                return -NStrok;
            }
            CurX = l_List[0].toInt();
            CurY = l_List[1].toInt();
            if (CurX < 0 || CurX > SizeX || CurY < 0 || CurY > SizeY)
            {
                
                l_File->close(); return -NStrok;
            }
            // TODO STENI
            if (getFieldItem(CurY, CurX))
                getFieldItem(CurY,CurX)->setWalls((l_List[2]).toInt());
            
            //		int ix = (l_List[0]).toInt();
            //		int iy = (l_List[1]).toInt();
            
            if (!((l_List[3]).toInt() == 0))
            {
                getFieldItem(CurY,CurX)->IsColored = true;
                // //QMessageBox::information( 0, "","test1" , 0,0,0);
            }
            else {
                if (getFieldItem(CurY,CurX))
                    getFieldItem(CurY,CurX)->IsColored = false;
            }
            qreal radiation = (l_List[4].replace(",",".")).toDouble();
            if (getFieldItem(CurY,CurX))
                getFieldItem(CurY,CurX)->radiation=radiation;
            
            if (l_List[4].toFloat() < 0)
            {
                
                l_File->close(); return -NStrok;
            }
            qreal temperature = (l_List[5].replace(",",".")).toDouble();
            if (getFieldItem(CurY,CurX))
                getFieldItem(CurY,CurX)->temperature=temperature;
            
            if (l_List[5].toFloat() < MIN_TEMP)
            {
                
                l_File->close(); return -NStrok;
            }
            
            
            
            if (l_List.count() >= 7)
            {
                
                QString tmp1 = l_List[6];
                //dlina lexemy dolzna ravnyatsa 1
                if (!(tmp1.length() == 1))
                {
                    
                    l_File->close(); return -NStrok;
                }
                //qDebug()<<QString::fromUtf8("Тест Up:")<<tmp1[0];
                if(tmp1[0]!='$') {
                    if (getFieldItem(CurY,CurX))
                        getFieldItem(CurY,CurX)->upChar = tmp1[0];
                }
                else {
                    if (getFieldItem(CurY,CurX))
                        getFieldItem(CurY,CurX)->upChar = ' ' ;
                }
            }
            else
            {
                if (getFieldItem(CurY,CurX))
                    getFieldItem(CurY,CurX)->upChar = ' ' ;
            }
            
            
            
            if (l_List.count() >= 8)
            {
                
                QString tmp1 = l_List[7];
                
                //dlina lexemy dolzna ravnyatsa 1
                if (!(tmp1.length() == 1))
                {
                    
                    l_File->close(); return -NStrok;
                }
                //qDebug()<<QString::fromUtf8("Тест Down:")<<tmp1[0];
                if(tmp1[0]!='$') {
                    if (getFieldItem(CurY,CurX))
                        getFieldItem(CurY,CurX)->downChar = tmp1[0];
                }
                else {
                    if (getFieldItem(CurY,CurX))
                        getFieldItem(CurY,CurX)->downChar = ' ' ;
                }
            }
            else
            {
                if (getFieldItem(CurY,CurX))
                    getFieldItem(CurY,CurX)->downChar = ' ' ;
            }
            
            
            
            if (l_List.count() >= 9)
            {
                
                QString tmp1 = l_List[8];
                //dlina lexemy dolzna ravnyatsa 1
                if (!(tmp1.length() == 1))
                {
                    
                    l_File->close(); return -NStrok;
                }
                if(tmp1[0]=='1') {
                    if (getFieldItem(CurY,CurX))
                        getFieldItem(CurY,CurX)->mark = true;
                }
                else {
                    if (getFieldItem(CurY,CurX))
                        getFieldItem(CurY,CurX)->mark = false ;
                }
            }
            else
            {
                if (getFieldItem(CurY,CurX))
                    getFieldItem(CurY,CurX)->mark = false ;
            }
            
            
            
            
            
            
        }
        
       l_File->close();
        
        
        
        
        wasEdit=false;
        
        
        //robot->setZValue(100);
        return(0);
        
        

    }
    
    /**
     * Загрузка обстановки из файла, отображение не производится.
     * @param fileName Имя файла
     * @return Код ошибки
     * 1 - Ошибка чтения файла
     * 2 - Пустой файл
     * 3 - Ошибка чтения размеров обстановки
     * 4 - Неверные размеры поля
     * 5 - Пустя строка
     * 6 - Неверное положение робота
     * 0< - Ошибка в строке
     * 10 - Ошибки основного прогона
     */
    int RoboField::loadFromFile(QString fileName)
    {
        
        //destroyField();
        
        QFileInfo fi(fileName);
        QString name = fi.fileName();
        
        QString Title = QString::fromUtf8("Робот - ") + name;
        
        
        QString tmp = "";
        QString ctmp;
        
        
        QFile l_File(fileName);
        
        
        
        
        int NStrok;
        NStrok = 0;
        QString l_String;
        //	long l_Err;
        int CurX,CurY;
        int SizeX, SizeY;
        destroyField();
        // Тестовый прогон
        
        if  (!l_File.open(QIODevice::ReadOnly))
        {
            
            return 1;
        }
        
        
        
        // 1 stroka - razmery polya
        tmp = l_File.readLine();
        //QMessageBox::information( 0, "", tmp, 0,0,0);
        
        
        if (tmp.isNull()||tmp.isEmpty())
        {
            l_File.close();
            
            return 2;
        }
        
        //QMessageBox::information( 0, "", tmp, 0,0,0);
        
        while (tmp.left(1) == ";" || tmp == "")
        {
            tmp = l_File.readLine();
            NStrok++;
            if (tmp.isNull())
            {
                return 1;
            }
        }
        tmp = tmp.simplified();
        QStringList l_List = tmp.split(' ');
        
        if (l_List.count() != 2)
        {
            l_File.close();
            
            
            return 3;
        }
        
        SizeX = (l_List[0]).toInt();
        SizeY = (l_List[1]).toInt();
        
        if ( (SizeX<= 0) || (SizeY <= 0) )
        {
            return 4;
        }
        //            field.destroyField();
        // field.createField(l_List[0].toInt(),l_List[1].toInt());
        
        // Вторая строка - положение робота
        
        tmp = l_File.readLine();
        
        
        
        if (tmp.isNull())
        {
            l_File.close();
            
            return 5;
        }
        
        
        
        while (tmp.left(1) == ";" || tmp == "")
        {
            tmp = l_File.readLine();
            NStrok++;
            if (tmp.isNull())
            {
                l_File.close();
                
                return 5;
            }
        }
        tmp = tmp.simplified();
        l_List = tmp.split(' ');
        
        // koordinaty robota
        // proverka
        if ((l_List[0]).toInt() < 0 || (l_List[1]).toInt() < 0)
        {
            
            
            l_File.close();return 6;
        }
        
        if ((l_List[0]).toInt() > SizeX || (l_List[1]).toInt() > SizeY )
        {
            
            l_File.close(); return 6;
        }
        
        
        //	m_DefaultSett = l_Sett;
        
        while (!l_File.atEnd())
        {
            //l_Err = l_File.readLine(l_String, 255);
            tmp = QString::fromUtf8(l_File.readLine());
            NStrok++;
            if (tmp.isNull())
            {
                
                return 5;
            }
            if (tmp.left(1) == ";" || tmp == "")
            {
                continue;
            }
            tmp = tmp.simplified();
            l_List = tmp.split(' ');
            if (l_List.count() == 0)continue;
            
            if (l_List.count() > 9 )
            {
                
                l_File.close();
                return -NStrok;
            }
            if(l_List.count()<6)
            {
                l_File.close();
                qDebug("N Lexem<6");
                return -NStrok;
            };
            bool ok;
            CurX = l_List[0].toInt(&ok);
            if(!ok)
            {
                l_File.close();
                qDebug("Bad cur X<6");
                return -NStrok;
            };
            
            CurY = l_List[1].toInt(&ok);
            
            if(!ok){
                l_File.close();
                qDebug("Bad curY <6");
                return -NStrok;
            };
            
            if (CurX < 0 || CurX > SizeX || CurY < 0 || CurY > SizeY)
            {
                
                l_File.close(); return -NStrok;
            }
            
            if (l_List[4].toFloat() < 0)
            {
                
                l_File.close(); return -NStrok;
            }
            
            
            if (l_List[5].toFloat() < MIN_TEMP)
            {
                
                l_File.close(); return -NStrok;
            }
            
            
            if (l_List.count() >= 7)
            {
                
                QString tmp1 = l_List[6];
                //dlina lexemy dolzna ravnyatsa 1
                if (!(tmp1.length() == 1))
                {
                    
                    l_File.close(); return -NStrok;
                }
                
                
            }
            
            
            if (l_List.count() >= 8)
            {
                
                QString tmp1 = l_List[7];
                //dlina lexemy dolzna ravnyatsa 1
                if (!(tmp1.length() == 1))
                {
                    l_File.close(); return -NStrok;
                }
                
                
            }
            
        }
        l_File.close();
        
        //реальный прогон
        //destroyField();
        
        if  (!l_File.open(QIODevice::ReadOnly))
        {
            
            return 10;
        }
        
        
        
        // 1 stroka - razmery polya
        tmp = l_File.readLine();
        
        if (tmp.isNull())
        {
            l_File.close();
            
            return 10;
        }
        
        
        while (tmp.left(1) == ";" || tmp == "")
        {
            tmp = QString::fromUtf8(l_File.readLine());
            NStrok++;
            if (tmp.isNull())
            {
                l_File.close();
                
                return 10;
            }
        }
        tmp = tmp.simplified();
        l_List = tmp.split(' ');
        
        if (l_List.count() != 2)
        {
            l_File.close();
            
            return -NStrok;
        }
        
        SizeX = (l_List[0]).toInt();
        SizeY = (l_List[1]).toInt();
        // 	 //NEW ROBO CODE
        createField(SizeY,SizeX);
        
        //END
        if ((l_List[0]).toInt() <= 0 || (l_List[1]).toInt() <= 0)
        {
            
            l_File.close();
            return - NStrok;
        }
        
        
        // Вторая строка - положение робота
        
        tmp = l_File.readLine();
        
        
        
        if (tmp.isNull())
        {
            l_File.close();
            
            return 10;
        }
        
        
        
        while (tmp.left(1) == ";" || tmp == "")
        {
            tmp = l_File.readLine();
            NStrok++;
            if (tmp.isNull())
            {
                l_File.close();
                
                return 10;
            }
        }
        tmp = tmp.simplified();
        l_List = tmp.split(' ');
        
        // koordinaty robota
        
        if ((l_List[0]).toInt() < 0 || (l_List[1]).toInt() < 0)
        {
            
            l_File.close();return - NStrok;
        }
        
        if ((l_List[0]).toInt() > SizeX || (l_List[1]).toInt() > SizeY )
        {
            
            l_File.close(); return - NStrok;
        }
        
        robo_x = (l_List[0]).toInt();
        robo_y = (l_List[1]).toInt();
        
        //InitialX = m_x;
        //InitialY = m_y;
        
        
        
        
        //	delete []m_FieldDubl;
        
        
        
        while (!l_File.atEnd())
        {
            tmp = QString::fromUtf8(l_File.readLine());
            NStrok++;
            if (tmp.isNull())
            {
                
                l_File.close();
                return 10;
            }
            if (tmp.left(1) == ";" || tmp == "")
            {
                continue;
            }
            tmp = tmp.simplified();
            l_List = tmp.split(' ');
            if (l_List.count() == 0)continue;
            
            if (l_List.count() > 9)
            {
                
                l_File.close();
                return -NStrok;
            }
            CurX = l_List[0].toInt();
            CurY = l_List[1].toInt();
            if (CurX < 0 || CurX > SizeX || CurY < 0 || CurY > SizeY)
            {
                
                l_File.close(); return -NStrok;
            }
            // TODO STENI
            if (getFieldItem(CurY, CurX))
                getFieldItem(CurY,CurX)->setWalls((l_List[2]).toInt());
            
            //		int ix = (l_List[0]).toInt();
            //		int iy = (l_List[1]).toInt();
            
            if (!((l_List[3]).toInt() == 0))
            {
                getFieldItem(CurY,CurX)->IsColored = true;
                // //QMessageBox::information( 0, "","test1" , 0,0,0);
            }
            else {
                if (getFieldItem(CurY,CurX))
                    getFieldItem(CurY,CurX)->IsColored = false;
            }
            qreal radiation = (l_List[4].replace(",",".")).toDouble();
            if (getFieldItem(CurY,CurX))
                getFieldItem(CurY,CurX)->radiation=radiation;
            
            if (l_List[4].toFloat() < 0)
            {
                
                l_File.close(); return -NStrok;
            }
            qreal temperature = (l_List[5].replace(",",".")).toDouble();
            if (getFieldItem(CurY,CurX))
                getFieldItem(CurY,CurX)->temperature=temperature;
            
            if (l_List[5].toFloat() < MIN_TEMP)
            {
                
                l_File.close(); return -NStrok;
            }
            
            
            
            if (l_List.count() >= 7)
            {
                
                QString tmp1 = l_List[6];
                //dlina lexemy dolzna ravnyatsa 1
                if (!(tmp1.length() == 1))
                {
                    
                    l_File.close(); return -NStrok;
                }
                //qDebug()<<QString::fromUtf8("Тест Up:")<<tmp1[0];
                if(tmp1[0]!='$') {
                    if (getFieldItem(CurY,CurX))
                        getFieldItem(CurY,CurX)->upChar = tmp1[0];
                }
                else {
                    if (getFieldItem(CurY,CurX))
                        getFieldItem(CurY,CurX)->upChar = ' ' ;
                }
            }
            else
            {
                if (getFieldItem(CurY,CurX))
                    getFieldItem(CurY,CurX)->upChar = ' ' ;
            }
            
            
            
            if (l_List.count() >= 8)
            {
                
                QString tmp1 = l_List[7];
                
                //dlina lexemy dolzna ravnyatsa 1
                if (!(tmp1.length() == 1))
                {
                    
                    l_File.close(); return -NStrok;
                }
                //qDebug()<<QString::fromUtf8("Тест Down:")<<tmp1[0];
                if(tmp1[0]!='$') {
                    if (getFieldItem(CurY,CurX))
                        getFieldItem(CurY,CurX)->downChar = tmp1[0];
                }
                else {
                    if (getFieldItem(CurY,CurX))
                        getFieldItem(CurY,CurX)->downChar = ' ' ;
                }
            }
            else
            {
                if (getFieldItem(CurY,CurX))
                    getFieldItem(CurY,CurX)->downChar = ' ' ;
            }
            
            
            
            if (l_List.count() >= 9)
            {
                
                QString tmp1 = l_List[8];
                //dlina lexemy dolzna ravnyatsa 1
                if (!(tmp1.length() == 1))
                {
                    
                    l_File.close(); return -NStrok;
                }
                if(tmp1[0]=='1') {
                    if (getFieldItem(CurY,CurX))
                        getFieldItem(CurY,CurX)->mark = true;
                }
                else {
                    if (getFieldItem(CurY,CurX))
                        getFieldItem(CurY,CurX)->mark = false ;
                }
            }
            else
            {
                if (getFieldItem(CurY,CurX))
                    getFieldItem(CurY,CurX)->mark = false ;
            }
            
            
            
            
            
            
        }
        
        l_File.close();
        
        
        
        qDebug() << "File " << fileName ;
        wasEdit=false;
        
        
        //robot->setZValue(100);
        return(0);
        
        
        
    };
    
    int RoboField::saveToFile(QString fileName)
    {
        QFileInfo fi(fileName);
        QString name = fi.fileName();
        
        qDebug()<<"NewRobot Save file: "<<fileName;
        //QString Title = QString::fromUtf8("Робот - ") + name;
        
        
        QChar Bukva;
        char ctmp[200];
        QFile l_File(fileName);
        if  (!l_File.open(QIODevice::WriteOnly))
        {
            return 1;
        }
        
        l_File.write("; Field Size: x, y\n");
        
        //QString l_String;
        
        sprintf(ctmp,"%i %i\n", columns(), rows());
        l_File.write(ctmp);
        l_File.write("; Robot position: x, y\n");
        sprintf(ctmp,"%i %i\n", robo_x, robo_y);
        l_File.write(ctmp );
        l_File.write("; A set of special Fields: x, y, Wall, Color, Radiation, Temperature, Symbol, Symbol1, Point\n");
        
        for (int i = 0; i < rows(); i++)
        {
            for (int j = 0; j < columns(); j++)
            {
                
                Bukva = getFieldItem(i,j)->upChar;
                if(Bukva==' ')Bukva='$';
                qDebug()<<QString::fromUtf8("Буква")<<Bukva;
                QChar cc;
                cc = Bukva;
                
                
                Bukva = getFieldItem(i,j)->downChar;
                if(Bukva==' '){Bukva='$';qDebug()<<QString::fromUtf8("Буквы нет");}
                else qDebug()<<QString::fromUtf8("Буква ")<<Bukva;
                
                QChar cc1 = Bukva;
                
                
                char cc2[2];
                cc2[0] = 0;
                cc2[1] = 0;
                
                if(getFieldItem(i,j)->mark)cc2[0] = '1';
                //char cc = 'g';
                if (!getFieldItem(i, j)->emptyCell())
                {
                    qDebug()<<" i:"<<i << " j:"<< j << " walls:"<<getFieldItem(i, j)->wallByte();
                    sprintf(ctmp,"%i %i %i %i %f %f ", j, i, getFieldItem(i, j)->wallByte(), getFieldItem(i, j)->IsColored, getFieldItem(i, j)->radiation, getFieldItem(i,j)->temperature);
                    QString toWrite=QString(ctmp)+cc+" "+cc1+" "+cc2+"\n";
                    l_File.write(toWrite.toUtf8());
                    qDebug()<<"ROBOT:WRITE 2 FILE";
                }
            }
        }
        l_File.write( "; End Of File\n");
        
        l_File.close();
        if (fi.exists())
        {
            //QMessageBox::information( 0, "", QString::fromUtf8("файл записан"), 0,0,0);
            
        }
        else
        {
            QMessageBox::information( 0, "", QString::fromUtf8("Файл не записан - попробуйте снова!"), 0,0,0);
            return -1;
        }
        
        //ToDubl();
        wasEdit=false;
        return 0;
        
    };
    
    bool RoboField::stepUp()
    {
//        if (m_robot && !property("fromPult").toBool())
//            m_robot->showHideWindow(true);
        
        setProperty("fromPult", false);
        
        if(getFieldItem(robo_y,robo_x)->canUp())
        {
            robot->setPos( QPointF(robot->pos().x(),
                                   robot->pos().y() - fieldSize));
            
            robo_y--;
            return true;
        }else return false;
    };
    
    bool RoboField::stepDown()
    {
//        if (m_robot && !property("fromPult").toBool())
//            m_robot->showHideWindow(true);
        
        setProperty("fromPult", false);
        
        if(getFieldItem(robo_y,robo_x)->canDown())
        {
            robot->moveBy(0,fieldSize);
            robo_y++;
            return true;
        }else return false;
    };
    
    bool RoboField::stepLeft()
    {
//        if (m_robot && !property("fromPult").toBool())
//            m_robot->showHideWindow(true);
        setProperty("fromPult", false);
        
        if(getFieldItem(robo_y,robo_x)->canLeft())
        {
            robot->setPos( QPointF(robot->pos().x()-fieldSize,
                                   robot->pos().y()));
            robo_x--;
            return true;
        }else return false;
    };
    
    bool RoboField::stepRight()
    {
//        if (m_robot && !property("fromPult").toBool())
//            m_robot->showHideWindow(true);
        setProperty("fromPult", false);
        
        if(getFieldItem(robo_y,robo_x)->canRight())
        {
            robot->moveBy(fieldSize,0);
            robo_x++;
            return true;
        }else return false;
    };
    
    
    /**
     * Обработка событий нажатий кнопок мыши, показ диалога редактирования *клетки
     * @param mouseEvent
     */
    void RoboField::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
    {
        qDebug()<<" Field Mouse event mode:"<<mode;
        
        if(mode==0)
        {
            pressed=true;
            oldRect=sceneRect();
            perssX=mouseEvent->pos().x();
            pressY=mouseEvent->pos().y();
              mouseEvent->accept();
            return;
        };
        
        
        //qDebug()<<" Edit mouse press event";
        QPointF scenePos=mouseEvent->scenePos();
        float rc=float(scenePos.y())/float(fieldSize);
        if(rc<0)rc=-1;
        float cc=float(scenePos.x()-3)/float(fieldSize);
        if(cc<0)cc=-1;
        int rowClicked=rc;
        int colClicked=cc;
       // qDebug()<<"RC:"<<rowClicked<<"CC:"<<colClicked<<" sc pos y:"<<scenePos.y()<<" scenePos.x()"<<scenePos.x()<<"";
        if(mode==TEXT_MODE)
        {
            pressD=false;
            if(keyCursor)this->removeItem(keyCursor);
            clickCell=QPair<int,int>(rowClicked,colClicked);
            if((scenePos.y()-rowClicked*fieldSize)>fieldSize/2)
            {
                pressD=true;
                showCursorDown(rowClicked,colClicked);
            }
             else showCursorUp(rowClicked,colClicked);
            return;
        }
        if(mode==RAD_MODE)//if radiation || temp edit mode
        {   if(rowClicked>rows() || colClicked>columns() ||rowClicked<0 || colClicked<0)//clik mimio polya
            {
            //radSpinBox->hide();
            return;
            }
            qDebug()<<"RAD MODE CLick";
            if(clickCell!=QPair<int,int>(rowClicked,colClicked))
             {
               if(radSpinBox->isVisible())
                   
               {    clickCell=QPair<int,int>(rowClicked,colClicked);
                   qDebug()<<"SET F:"<<clickCell.first<<"SET SEC:"<<clickCell.second;
                   getFieldItem(rowClicked,colClicked)->radiation=radSpinBox->value();
                
               }
                 redrawRTFields();  
                // radSpinBox->hide();
             }
            if(rowClicked>rows() || colClicked>columns() ||rowClicked<0 || colClicked<0)//clik mimio polya
            {
              //radSpinBox->hide();
             return;
            }
           
   
           QGraphicsView * view=views().first();  //current view
            //QPoint clickViewPos=view->mapFromScene(mouseEvent->scenePos().x(), mouseEvent->scenePos().y());
           // qDebug()<<"ROW:"<<rowClicked<<"COL:"<<colClicked;
            //radSpinBox->setValue(getFieldItem(rowClicked,colClicked)->radiation);//set radiation
            view->repaint();
        
            return;
        }
        if(mode==TEMP_MODE)
        {
            qDebug()<<"Temp MODE CLick";
            if(rowClicked>rows() || colClicked>columns() ||rowClicked<0 || colClicked<0)//clik mimio polya
            {
                //radSpinBox->hide();
                return;
            }
            if(tempSpinBox->isVisible())
                
            {    clickCell=QPair<int,int>(rowClicked,colClicked);
                qDebug()<<"SET F:"<<clickCell.first<<"SET SEC:"<<clickCell.second;
                getFieldItem(rowClicked,colClicked)->temperature=tempSpinBox->value();
                
            }
            redrawRTFields();  
             QGraphicsView * view=views().first();
            view->repaint();
            return;
        }
 
        
        bool left,right,up,down;
        left=false;right=false;up=false;down=false;
        int upD=fieldSize,downD=fieldSize,leftD=fieldSize,rightD=fieldSize;
        qreal delta_row=scenePos.y() - rowClicked * fieldSize;
        qreal delta_col=scenePos.x()- colClicked * fieldSize-3;
        QGraphicsScene::mousePressEvent(mouseEvent);
        if(mouseEvent->isAccepted ())return;
        
        
        
        if((rowClicked>rows()-1)||(rowClicked<0))
        {mouseEvent->ignore();QGraphicsScene::mousePressEvent(mouseEvent);return;};
        
        if((colClicked>columns()-1)||(colClicked<0))
        {mouseEvent->ignore();QGraphicsScene::mousePressEvent(mouseEvent);return;};
        
       // qDebug()<<"ScenePos y "<<scenePos.y() <<" sceneposx"<<scenePos.x() << "rowClick:"<<rowClicked << " "<<" colClick:"<<colClicked <<
      //  " delta_row" <<delta_row << " delta_col" << delta_col;
        if(!NEDIT_MODE)return; //don't setup walls,and marks;
        
        if(delta_row<=MAX_CLICK_DELTA){up=true;upD=delta_row;qDebug("UP");};
        
        if(fieldSize-delta_row<=MAX_CLICK_DELTA){down=true;downD=fieldSize-delta_row;};
        
        
        if(delta_col<=MAX_CLICK_DELTA){left=true;leftD=delta_col;};
        
        if(fieldSize-delta_col<=MAX_CLICK_DELTA){right=true;rightD=fieldSize-delta_col;};
        if(mouseEvent->modifiers ()==Qt::ControlModifier) { markMode=false; } else { markMode=true; }
        
        //Углы клетки
        if ((left)&&(up)) {
            if(upD<leftD) {
                left=false;
            }
            else  {
                up=false;
            }
        }
        if((left)&&(down)) {
            if(downD<leftD) {
                left=false;
            }
            else  {
                down=false;
            }
        }
        if((right)&&(up)) {
            if(upD<rightD) {
                right=false;
            }
            else  {
                up=false;
            }
        }
        if((right)&&(down)) {
            if(downD<rightD) {
                right=false;
            }
            else  {
                down=false;
            }
        }
        
        //Ставим стены
        if(up){reverseUpWall(rowClicked,colClicked);qDebug("ReversUP");};
        if(down)reverseDownWall(rowClicked,colClicked);
        if(left)reverseLeftWall(rowClicked,colClicked);
        if(right)reverseRightWall(rowClicked,colClicked);
        if(!up && !down && !right && !left)
        {
            if(markMode)
            {
                reverseColor(rowClicked,colClicked); //Zakraska
                old_cell=QPair<int,int>(rowClicked, colClicked); 
            }
            else //Otmetit tochkoy
            {
                reverseMark(rowClicked,colClicked);
                old_cell=QPair<int,int>(rowClicked, colClicked); 
            };
        };
//        if(wasEdit)emit was_edit();
    };
    void RoboField::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
    {
        qDebug()<<"FIELD RELEASE";
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        if(mouseEvent->isAccepted ())
        {
        qDebug()<<"Field is ACC";
           return; 
        }
        if(mode==0)pressed=true;
    };
    void RoboField::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
    {
       // qDebug()<<"FIELD MOVE";
        if(mode==0)return;
        if(mode!=NEDIT_MODE)return;
        
        QGraphicsScene::mouseMoveEvent(mouseEvent);
     if(!pressed)
     {
         QPointF scenePos=mouseEvent->scenePos();
         int rowClicked=scenePos.y()/fieldSize;  //clicked = mouse cur pos!!
         int colClicked=(scenePos.x()-3)/fieldSize;
         
         bool left,right,up,down;
         left=false;right=false;up=false;down=false;
         int upD=fieldSize,downD=fieldSize,leftD=fieldSize,rightD=fieldSize;
         qreal delta_row=scenePos.y() - rowClicked * fieldSize;
         qreal delta_col=scenePos.x()- colClicked * fieldSize-3;

         
         if((rowClicked>rows()-1)||(rowClicked<0))
         {mouseEvent->ignore();QGraphicsScene::mousePressEvent(mouseEvent);return;};
         
         if((colClicked>columns()-1)||(colClicked<0))
         {mouseEvent->ignore();QGraphicsScene::mousePressEvent(mouseEvent);return;};

      
         if(delta_row<=MAX_CLICK_DELTA){up=true;upD=delta_row;qDebug("UP");};
         
         if(fieldSize-delta_row<=MAX_CLICK_DELTA){down=true;downD=fieldSize-delta_row;};
         
         
         if(delta_col<=MAX_CLICK_DELTA){left=true;leftD=delta_col;};
         
         if(fieldSize-delta_col<=MAX_CLICK_DELTA){right=true;rightD=fieldSize-delta_col;};
         if(mouseEvent->modifiers ()==Qt::ControlModifier) { markMode=false; } else { markMode=true; }
         
         //Углы клетки
         if ((left)&&(up)) {
             if(upD<leftD) {
                 left=false;
             }
             else  {
                 up=false;
             }
         }
         if((left)&&(down)) {
             if(downD<leftD) {
                 left=false;
             }
             else  {
                 down=false;
             }
         }
         if((right)&&(up)) {
             if(upD<rightD) {
                 right=false;
             }
             else  {
                 up=false;
             }
         }
         if((right)&&(down)) {
             if(downD<rightD) {
                 right=false;
             }
             else  {
                 down=false;
             }
         }
         
         //Ставим стены
         if(up && markMode){showUpWall(rowClicked,colClicked);qDebug("ShowUP");};
         if(down && markMode)showDownWall(rowClicked,colClicked);
         if(left && markMode)showLeftWall(rowClicked,colClicked);
         if(right && markMode)showRightWall(rowClicked,colClicked);
         if(!up && !down && !left && !right)
         {
             if((mouseEvent->buttons()==Qt::LeftButton) && (old_cell!=QPair<int,int>(rowClicked, colClicked)&&(!robot->isMoving())))
             {
             if(markMode)
             {
                 
              reverseColor(rowClicked,colClicked); //Zakraska 
                 old_cell=QPair<int,int>(rowClicked, colClicked); 
             }
             else //Otmetit tochkoy
             {
                 reverseMark(rowClicked,colClicked);
             old_cell=QPair<int,int>(rowClicked, colClicked); 
             }
                 
                 
             }
            // qDebug()<<"OldCell!newCell"<<  (old_cell!=QPair<int,int>(rowClicked, colClicked));
         showWall->setVisible(false);
         }
          
         
     }
    };   
    void RoboField::keyPressEvent ( QKeyEvent * keyEvent )
    {
        if(clickCell==QPair<int,int>(-1,1))return;
   
        redrawRTFields();
        
        if(!(mode==NEDIT_MODE || mode==TEXT_MODE))return;
      
        qDebug()<<"KEY PRESSD"<<keyEvent->text ();
        int rowP=clickCell.first;
        int colP=clickCell.second;
        if(keyEvent->text ().isNull() || keyEvent->text ().isEmpty() || keyEvent->text()=="/n" )return;
        if(!pressD)//Pressed up symbol
        {
        getFieldItem(rowP,colP)->upChar=keyEvent->text ().at(0);
        getFieldItem(rowP,colP)->showUpChar
        (upLeftCorner(rowP,colP).x(),upLeftCorner(rowP,colP).y(),fieldSize);
        }else
        {
            getFieldItem(rowP,colP)->downChar=keyEvent->text ().at(0);
            getFieldItem(rowP,colP)->showDownChar
            (upLeftCorner(rowP,colP).x(),upLeftCorner(rowP,colP).y(),fieldSize);
        }
            wasEdit=true;
    };  

    void RoboField::setColorFromSett()
    {
         sett=RobotModule::robotSettings();
        LineColor = QColor(sett->value("LineColor","#C8C800").toString());
        WallColor=QColor(sett->value("WallColor","#C8C800").toString());
        EditColor=QColor(sett->value("EditColor","#00008C").toString());
        NormalColor=QColor(sett->value("NormalColor","#289628").toString());
        qDebug()<<"Normal color blue"<<NormalColor.blue ();
       if(mode==NORMAL_MODE) this->setBackgroundBrush (QBrush(NormalColor));
            else  this->setBackgroundBrush (QBrush(EditColor));
        
        for(int i=0;i<setka.count();i++)
        {
            setka.at(i)->setPen(QPen(LineColor));
        }
    
    }
    
    
    
    void RoboField::showCellDialog(FieldItm * cellClicked)
    {
        
//        qDebug() << "showCellDialog";
//        QPoint pos = views().first()->pos();
//        pos = views().first()->mapToGlobal(pos);
//        qDebug() << "Pos: " << pos;
//        cellDialog->move(pos);
//        
//        cellDialog->setRowCols(rows(),columns());
//        
//        
//        //       cellDialog->mark->setChecked(cellClicked->mark);
//        
//        
//        //       cellDialog->colored->setChecked(cellClicked->IsColored);
//        
//        QChar upChar,downChar;
//        if(cellClicked->upChar == ' ')upChar='_';else upChar=cellClicked->upChar;
//        if(cellClicked->downChar == ' ')downChar='_';else downChar=cellClicked->downChar;
//        
//        cellDialog->upChar->setText(upChar);
//        cellDialog->downChar->setText(downChar);
//        
//        cellDialog->RadSpinBox->setValue(cellClicked->radiation);
//        cellDialog->TempSpinBox->setValue(cellClicked->temperature);
//        
//        cellDialog->show();
    };
    
    void RoboField::cellDialogOk()
    {
//        int rowP = cellDialog->row_pos;
//        int colP = cellDialog->col_pos;
//        // qDebug()<<"CellDalog:color:"<<cellDialog->colored->isChecked();
//        //  if(cellDialog->colored->isChecked()!=getFieldItem(rowP,colP)->IsColored)
//        //{reverseColor(rowP,colP);wasEdit=true;};
//        
//        //  if(cellDialog->mark->isChecked()!=getFieldItem(rowP,colP)->mark){reverseMark(rowP,colP);wasEdit=true;};
//        if(cellDialog->upChar->text()!=getFieldItem(rowP,colP)->upChar)
//        {
//            
//            QString tmp=cellDialog->upChar->text();	    if((tmp.length()==0)||(tmp[0]!='_'))getFieldItem(rowP,colP)->removeUpChar();
//            if((tmp.length()==1)&&(tmp[0]!='_')){
//                getFieldItem(rowP,colP)->removeUpChar();
//                
//                
//                
//                getFieldItem(rowP,colP)->upChar=tmp[0];
//                getFieldItem(rowP,colP)->showUpChar
//                (upLeftCorner(rowP,colP).x(),upLeftCorner(rowP,colP).y(),fieldSize);wasEdit=true;};
//        };
//        if(cellDialog->downChar->text()!=getFieldItem(rowP,colP)->downChar)
//        {
//            QString tmp=cellDialog->downChar->text();
//            if((tmp.length()==0)||(tmp[0]!='_'))getFieldItem(rowP,colP)->removeDownChar();
//            if((tmp.length()==1)&&(tmp[0]!='_')){
//                
//                getFieldItem(rowP,colP)->removeDownChar();
//                
//                getFieldItem(rowP,colP)->downChar=tmp[0];
//                getFieldItem(rowP,colP)->showDownChar
//                (upLeftCorner(rowP,colP).x(),upLeftCorner(rowP,colP).y(),fieldSize);wasEdit=true;};
//        };
//        if((getFieldItem(rowP,colP)->radiation!=cellDialog->RadSpinBox->value())||
//           (getFieldItem(rowP,colP)->temperature!=cellDialog->TempSpinBox->value())) wasEdit=true;
//        getFieldItem(rowP,colP)->radiation=cellDialog->RadSpinBox->value();
//        getFieldItem(rowP,colP)->temperature=cellDialog->TempSpinBox->value();
//        /*
//         if(cellDialog->setRobot->isChecked())
//         {
//         robo_y=rowP;
//         robo_x=colP;
//         robot->setPos(upLeftCorner(robo_y,robo_x).x(),upLeftCorner(robo_y,robo_x).y());
//         wasEdit=true;
//         };*/
//        
//        if(wasEdit)emit was_edit();
    };
    void RoboField::showCursorUp(int row,int col)
    {
        timer->start(500);
        


        
   keyCursor=new QGraphicsLineItem(upLeftCorner(row,col).x()+4,upLeftCorner(row,col).y()+4,upLeftCorner(row,col).x()+4,upLeftCorner(row,col).y()+14);
        keyCursor->setPen(QPen(QColor(Qt::white)));
        keyCursor->setZValue(210);
        this->addItem(keyCursor); 
    };
    void RoboField::showCursorDown(int row,int col)
    {
        timer->start(500);
        
        if(this->items().indexOf(keyCursor)>-1)this->removeItem(keyCursor);
        
        
        keyCursor=new QGraphicsLineItem(upLeftCorner(row,col).x()+4,upLeftCorner(row,col).y()+18,upLeftCorner(row,col).x()+4,upLeftCorner(row,col).y()+29);
        keyCursor->setPen(QPen(QColor(Qt::white)));
        keyCursor->setZValue(210);
        this->addItem(keyCursor);   
    };
    void RoboField::timerTic()
    {
        if(mode!=TEXT_MODE){
         keyCursor->hide();
            timer->stop();  
        }
      qDebug()<<"TIK!"; 
        if(!keyCursor)return;
      timer->start(500);
        if(keyCursor->isVisible())keyCursor->hide();
        else keyCursor->show();
    };
    void RoboField::setFieldItems(QList<QList<FieldItm *> > FieldItems)
    {
        
        Items = QList< QList<FieldItm*> >();
        for (int i=0; i<FieldItems.size(); i++) {
            QList<FieldItm*> row;
            for (int j=0; j<FieldItems[i].size(); j++) {
                row << NULL;
            }
            Items << row;
        }
    }
    
    void RoboField::setItem(FieldItm *item, uint str, uint stlb)
    {
        if (Items[str][stlb]) {
            delete Items[str][stlb];
        }
        Items[str][stlb]=item;
    }
    
    RoboField* RoboField::Clone()
    {
        RoboField* clone=new RoboField(0);
        clone->setFieldItems(Items);
        clone->robo_x=robo_x;
        clone->robo_y=robo_y;
        clone->setModeFlag(mode);
        for(int i=0;i<rows();i++)
        {
            for(int j=0;j<columns();j++)
            {
                clone->setItem(getFieldItem(i,j)->Copy(),i,j);
                if(j>0)clone->getFieldItem(i,j)->setLeftsepItem(clone->getFieldItem(i,j-1));
                if(i>0)clone->getFieldItem(i,j)->setUpsepItem(clone->getFieldItem(i-1,j));
            };
        };
        
        return clone;
    };
    
    
    
    void RoboField::wbMode()
    {
        
        this->setBackgroundBrush (QBrush("white"));
        for(int i=0;i<rows();i++)
        {
            for(int j=0;j<columns();j++)
            {
                getFieldItem(i,j)->wbWalls();
            };
        };
    };
    void RoboField::colorMode()
    {
         sett=RobotModule::robotSettings();
        for(int i=0;i<rows();i++)
        {
            for(int j=0;j<columns();j++)
            {
                getFieldItem(i,j)->colorWalls();
                getFieldItem(i,j)->setTextColor();
            };
        };
        if(mode==0)Color = QColor(40,150,40);//Normal Color
        else Color=QColor(0,0,140);//Edit Color
        this->setBackgroundBrush (QBrush(Color));
    };
    
    
    //++++++++EditLine
    
    EditLine::EditLine(QGraphicsItem *parent)
    {
        Value=0;
        istemp=false;
        iconPath=QUrl::fromLocalFile(
                                     qApp->property("sharePath").toString()+
                                     "/actors/robot/btn_radiation.png"
                                     );
        rad=QImage(iconPath.toLocalFile ());
        
        rad.load(iconPath.toLocalFile ());
        QPainter pntr;
        pntr.begin(&radiation);
       // pntr.drawImage(2,2,rad.scaledToHeight(FIELD_SIZE_SMALL) );
     //   setRenderHints(QPainter::Antialiasing);
    }
    
    void EditLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                            QWidget *widget)
    {
        Q_UNUSED(option); Q_UNUSED(widget);
      //  painter->setRenderHints(QPainter::SmoothPixmapTransform);
        if(isRad())
        {
                        
          //  painter->drawPicture(0,0,radiation);
            painter->setPen(QPen(Qt::yellow));
           // painter->drawRect(2, FIELD_SIZE_SMALL+4, (FIELD_SIZE_SMALL-FIELD_SIZE_SMALL/4)*(Value*4/99),FIELD_SIZE_SMALL*4/6 );
            QFont sansFont("Helvetica [Cronyx]", 12);
            sansFont.setPixelSize(FIELD_SIZE_SMALL*1.2);
            painter->setFont(sansFont);
            painter->drawText(4, FIELD_SIZE_SMALL+4,FIELD_SIZE_SMALL*3,FIELD_SIZE_SMALL*1.5,0 ,QString::number(Value));
            
        }
        if(isTemp())
        {
            
            //  painter->drawPicture(0,0,radiation);
            painter->setPen(QPen(Qt::white));
            // painter->drawRect(2, FIELD_SIZE_SMALL+4, (FIELD_SIZE_SMALL-FIELD_SIZE_SMALL/4)*(Value*4/99),FIELD_SIZE_SMALL*4/6 );
            QFont sansFont("Helvetica [Cronyx]", 12);
            sansFont.setPixelSize(FIELD_SIZE_SMALL*1.2);
            painter->setFont(sansFont);
            painter->drawText(4, FIELD_SIZE_SMALL+4,FIELD_SIZE_SMALL*3,FIELD_SIZE_SMALL*1.5,0 ,QString::number(Value));
            
        }
    }
    QRectF EditLine::boundingRect() const
    {
        //     qreal adjust = 0.5;
        return QRectF(0,0,FIELD_SIZE_SMALL/4,FIELD_SIZE_SMALL/4);
    };
    //+++++++Simple Robot
    SimpleRobot::SimpleRobot(QGraphicsItem *parent )
    {
        Q_UNUSED(parent);
        setAcceptDrops(true);
        
        
        
        static const int points[] = {  14,6, 22,14, 14,22, 6,14 };
        QPolygon polygon;
        polygon.setPoints(4, points);
        QPolygonF polygonf = QPolygonF(polygon);
        Robot = new QGraphicsPolygonItem ( );
        Robot->setPolygon(polygonf);
        Robot->setZValue(100);
        setZValue(100);
        crash=NO_CRASH;
        moving=false;
        // ainter->drawPolygon(polygonf);
        //Robot->setPolygon(polygonf);
    };
    
    void SimpleRobot::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
    {
        event->setAccepted(event->mimeData()->hasFormat("text/plain"));
        qDebug("Robo Drag enter");
    }
    void SimpleRobot::dragMoveEvent ( QGraphicsSceneDragDropEvent * event )
    {
        event->accept();
        qDebug("Robo Drag");
    };
    
    void SimpleRobot::mousePressEvent ( QGraphicsSceneMouseEvent * event )
    {
       // event->accept();
        moving=true;
        qDebug()<<"Mouse press SROBOT2" << event->pos();
    };
    
    void SimpleRobot::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
    {
        qDebug("Mouse move srobot");
        setPos(event->scenePos());
        
    };
    void SimpleRobot::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
    {
        //setZValue(100);
         qDebug()<<"Mouse release SROBOT" << event->pos();
       // emit moved(event->scenePos());
        
    };
    QRectF SimpleRobot::boundingRect() const
    {
        //     qreal adjust = 0.5;
        return Robot->boundingRect();
    };
    
    void SimpleRobot::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                            QWidget *widget)
    {
        Q_UNUSED(option); Q_UNUSED(widget);
        // Body
        painter->setBrush(QColor("lightgray"));
        painter->setPen(QPen("black"));
        
        static const int points[] = {  14,6, 22,14, 14,22, 6,14 };
        QVector<QPointF> up_crash,down_crash,left_crash,right_crash ;
        up_crash << QPointF(14 , 6) << QPointF(10, 11) <<  QPointF(19 ,11);
        down_crash << QPointF(14 , 22) << QPointF(11, 18) <<  QPointF(18 ,18);
        left_crash << QPointF(7 , 14) << QPointF(11, 10) <<  QPointF(11 ,18);
        right_crash << QPointF(22 , 14) << QPointF(18, 10) <<  QPointF(18 ,18);
        QPolygon polygon;
        polygon.setPoints(4, points);
        QPolygonF polygonf = QPolygonF(polygon);
        
        painter->drawPolygon(polygonf);
        if(crash!=NO_CRASH)
        {
            qDebug("crash!");
            QPen noPen(Qt::NoPen);
            painter->setPen(noPen);
            painter->setBrush(QColor("red"));
            if(crash==UP_CRASH)painter->drawPolygon(QPolygonF(up_crash));
            if(crash==DOWN_CRASH)painter->drawPolygon(QPolygonF(down_crash));
            if(crash==LEFT_CRASH)painter->drawPolygon(QPolygonF(left_crash));
            if(crash==RIGHT_CRASH)painter->drawPolygon(QPolygonF(right_crash));
            
        };
    };
    
    QGraphicsPolygonItem* SimpleRobot::RoboItem()
    {
        return Robot;
    }
    
    void SimpleRobot::setCrash(uint dirct)
    {
        crash=dirct;
        update();
    }
    
    
    
   
    
RobotModule::RobotModule(ExtensionSystem::KPlugin * parent)
	: RobotModuleBase(parent)
{
	/* TODO 
	implement class Constructor
	*/
    animation=true;
    pressed=false;
    field=new RoboField(0);
    //field->editField();
    field->createField(10,15);
   
    field->setRoboPos(0,0);
      field->createRobot();
    view=new RobotView(field);
    m_mainWidget = view;
    const QUrl rcUrl = QUrl::fromLocalFile(
                qApp->property("sharePath").toString()+
                "/actors/robot/rc.qml"
                );
    m_pultWidget = new QDeclarativeView(rcUrl);
    m_pultWidget->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);
    QDeclarativeItem * pult = qobject_cast<QDeclarativeItem*>(m_pultWidget->rootObject());
    connect(pult, SIGNAL(goLeft()), this, SLOT(runGoLeft()));
    connect(pult, SIGNAL(goRight()), this, SLOT(runGoRight()));
    connect(pult, SIGNAL(goUp()), this, SLOT(runGoUp()));
    connect(pult, SIGNAL(goDown()), this, SLOT(runGoDown()));
    connect(pult, SIGNAL(doPaint()), this, SLOT(runDoPaint()));
    connect(pult, SIGNAL(checkWallLeft()), this, SLOT(runIsWallAtLeft()));
    connect(pult, SIGNAL(checkWallRight()), this, SLOT(runIsWallAtRight()));
    connect(pult, SIGNAL(checkWallTop()), this, SLOT(runIsWallAtTop()));
    connect(pult, SIGNAL(checkWallBottom()), this, SLOT(runIsWallAtBottom()));
    connect(pult, SIGNAL(checkFreeLeft()), this, SLOT(runIsFreeAtLeft()));
    connect(pult, SIGNAL(checkFreeRight()), this, SLOT(runIsFreeAtRight()));
    connect(pult, SIGNAL(checkFreeTop()), this, SLOT(runIsFreeAtTop()));
    connect(pult, SIGNAL(checkFreeBottom()), this, SLOT(runIsFreeAtBottom()));
    connect(this, SIGNAL(sendToPultLog(QVariant)), pult, SLOT(addToResultLog(QVariant)));
    startField=field->Clone();
    field->drawField(FIELD_SIZE_SMALL);
    field->dropWasEdit();
    connect(m_actionRobotLoadEnvironment,SIGNAL(triggered()) , this, SLOT(loadEnv()));
    connect(m_actionRobotRevertEnvironment,SIGNAL(triggered()) , this, SLOT(resetEnv()));
    connect(m_actionRobotSaveEnvironment,SIGNAL(triggered()) , this, SLOT(saveEnv()));
    connect(m_actionRobotEditEnvironment,SIGNAL(triggered()) , this, SLOT(editEnv())); 
    connect(m_actionRobotNewEnvironment,SIGNAL(triggered()) , this, SLOT(newEnv()));
    
    prepareNewWindow();
    rescentMenu=new QMenu();
    m_actionRobotLoadRescent->setMenu(rescentMenu);
    
} 


/* public slot */ void RobotModule::loadActorData(QIODevice * source)
{
    // Set actor specific data (like environment)
    // The source should be ready-to-read QIODevice like QBuffer or QFile
       qDebug()<<"Load env";
    if(field->loadFromDataStream(source)!=0)return ;
    startField=field->Clone();
    field->dropWasEdit();
    //   ajustWindowSize();//NEW ROBOT
    
        
    
    
    //  ToDubl();
    
    
    //  SizeX = m_Size_x * m_FieldSize;
    // SizeY = m_Size_y * m_FieldSize + MenuHigth;
    
    
    
    // WasEditFlag=false;
    //delete btnFind;
    //CreatebtnFind();
    

    reset();
    return;
}

    
void RobotModule::reset()
{
	/* TODO
	This method is called when actor resets its state before program starts.
	*/
    //delete field;
    qDebug()<<"Reset!!";

        
    field->destroyRobot();
    field->deleteLater();
    if(field->isEditMode())
    {
        field->setMode(NORMAL_MODE);
        view->showButtons(false);
        
      startField=field->Clone();  
    }
    field=startField->Clone();
    field->setRoboPos(startField->robotX(),startField->robotY());
    field->createRobot();
    field->drawField(FIELD_SIZE_SMALL);
    //delete m_mainWidget;
    view->setScene(field);
    field->drawField(FIELD_SIZE_SMALL);
    view->setField(field);
    
}

    void RobotModule::changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current){
    qDebug()<<"RobotModuleBase::changeGlobalState";
        if(current==ExtensionSystem::GS_Running)
          {
              m_actionRobotRevertEnvironment->setEnabled(false);
              m_actionRobotLoadEnvironment->setEnabled(false);
              m_actionRobotLoadRescent->setEnabled(false);
              m_actionRobotRevertEnvironment->setEnabled(false);
              m_actionRobotSaveEnvironment->setEnabled(false);
              m_actionRobotEditEnvironment->setEnabled(false);
              m_actionRobotNewEnvironment->setEnabled(false);
          }
        if(current==ExtensionSystem::GS_Unlocked || current==ExtensionSystem::GS_Observe)
        {
            m_actionRobotRevertEnvironment->setEnabled(true);
            m_actionRobotLoadEnvironment->setEnabled(true);
            m_actionRobotLoadRescent->setEnabled(true);
            m_actionRobotRevertEnvironment->setEnabled(true);
            m_actionRobotSaveEnvironment->setEnabled(true);
            m_actionRobotEditEnvironment->setEnabled(true);
            m_actionRobotNewEnvironment->setEnabled(true);
            view->FindRobot();
        }

    };    
    
void RobotModule::reloadSettings(ExtensionSystem::SettingsPtr settings)
{
    qDebug()<<"reload settings";
    field->setColorFromSett();
    createRescentMenu();
}

void RobotModule::setAnimationEnabled(bool enabled)
{
    animation=enabled;
	/* TODO
	Set internal flag for animation
	*/
}


QWidget* RobotModule::mainWidget() const
{
	/* TODO
	This method should return a pointer to main widget.
	NOTE: This metod should NOT create main widget -- just return!
	*/
    return m_mainWidget;
}

QWidget* RobotModule::pultWidget() const
{
	/* TODO
	This method should return a pointer to pult widget.
	NOTE: This metod should NOT create pult widget -- just return!
	*/
    return m_pultWidget;
}
QList<ExtensionSystem::CommandLineParameter>  RobotModule::acceptableCommandLineParameters()
{
    QList<ExtensionSystem::CommandLineParameter> params;
    params.append(ExtensionSystem::CommandLineParameter(true,'f',"field",tr("Robot field file name"),QVariant::String,false));
    return params;
    
}
QString RobotModule::initialize(const QStringList &configurationParameters, const ExtensionSystem::CommandLine & runtimeParameters)
    {
        if(runtimeParameters.value("field").isValid())
            LoadFromFile(runtimeParameters.value("field").toString());
        return "";
    }
void RobotModule::runGoUp()
{
	/* TODO implement me */
    qDebug() << "Robot up";
    QString status = "OK";
     if(!field->stepUp())
     {
     field->robot->setCrash(UP_CRASH);    
     setError(trUtf8("Робот разбился: сверху стена!"));
     status = trUtf8("Отказ");
     }
     if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
         emit sendToPultLog(status);
     }
    if(animation)msleep(250);
	return;
}


void RobotModule::runGoDown()
{
	/* TODO implement me */
    qDebug() << "Robot down";
    QString status = "OK";
     if(!field->stepDown())
     {setError(trUtf8("Робот разбился: снизу стена!"));
     field->robot->setCrash(DOWN_CRASH);
     status = trUtf8("Отказ");
     }
     if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
         emit sendToPultLog(status);
     }
    if(animation){
        view->update();
     msleep(250);   
    }
	return;
}


void RobotModule::runGoLeft()
{
	/* TODO implement me */
    qDebug() << "Robot left";
    QString status = "OK";
    if(!field->stepLeft()){
      field->robot->setCrash(LEFT_CRASH);  
    setError(trUtf8("Робот разбился: слева стена!"));
    status = trUtf8("Отказ");
    };
    if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
        emit sendToPultLog(status);
    }
    if(animation)
    {
        view->update();
        msleep(250);
    }
	return;
}


void RobotModule::runGoRight()
{
	/* TODO implement me */
    qDebug() << "Robot right";
    QString status = "OK";
    if(!field->stepRight()){
        field->robot->setCrash(RIGHT_CRASH);  
        status = trUtf8("Отказ");
    setError(trUtf8("Робот разбился: справа стена!"));}
    if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
        emit sendToPultLog(status);
    }
     if(animation)
     {
         view->update();
     msleep(250);
     }
	return;
}


void RobotModule::runDoPaint()
{
	if(!field->currentCell()->IsColored)field->reverseColorCurrent();
    QString status = "OK";
    if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
        emit sendToPultLog(status);
    }
    if(animation)
    {
        view->update();
        msleep(250);
    }
	return;
}


bool RobotModule::runIsWallAtTop()
{
    bool result = !field->currentCell()->canUp();
    QString status = result? trUtf8("да") : trUtf8("нет");
    if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
        emit sendToPultLog(status);
    }
    return result;
}


bool RobotModule::runIsWallAtBottom()
{
    bool result = !field->currentCell()->canDown();
    QString status = result? trUtf8("да") : trUtf8("нет");
    if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
        emit sendToPultLog(status);
    }
    return result;
}


bool RobotModule::runIsWallAtLeft()
{
    bool result = !field->currentCell()->canLeft();
    QString status = result? trUtf8("да") : trUtf8("нет");
    if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
        emit sendToPultLog(status);
    }
    return result;
}


bool RobotModule::runIsWallAtRight()
{
    bool result = !field->currentCell()->canRight();
    QString status = result? trUtf8("да") : trUtf8("нет");
    if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
        emit sendToPultLog(status);
    }
    return result;
}

bool RobotModule::runIsFreeAtTop()
{
    bool result = field->currentCell()->canUp();
    QString status = result? trUtf8("да") : trUtf8("нет");
    if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
        emit sendToPultLog(status);
    }
    return result;
}
    
    
bool RobotModule::runIsFreeAtBottom()
{
    bool result = field->currentCell()->canDown();
    QString status = result? trUtf8("да") : trUtf8("нет");
    if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
        emit sendToPultLog(status);
    }
    return result;
}
    
    
bool RobotModule::runIsFreeAtLeft()
{
    bool result = field->currentCell()->canLeft();
    QString status = result? trUtf8("да") : trUtf8("нет");
    if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
        emit sendToPultLog(status);
    }
    return result;
}
    
    
bool RobotModule::runIsFreeAtRight()
{
    bool result = field->currentCell()->canRight();
    QString status = result? trUtf8("да") : trUtf8("нет");
    if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
        emit sendToPultLog(status);
    }
    return result;
}

bool RobotModule::runIsColor()
    {
     bool result = field->currentCell()->isColored();
        QString status = result? trUtf8("да") : trUtf8("нет");
        if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
            emit sendToPultLog(status);
        }
        return result;
        
    };
    bool RobotModule::runIsClear()
    {
        bool result = !field->currentCell()->isColored();
        QString status = result? trUtf8("да") : trUtf8("нет");
        if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
            emit sendToPultLog(status);
        }
        return result;
    };
    double RobotModule::runRadiation(){    
        double result = field->currentCell()->radiation;
        QString status = result? trUtf8("да") : trUtf8("нет");
        if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
            emit sendToPultLog(status);
        }
        return result;
    };
    int RobotModule::runTemperature(){       
        int result = field->currentCell()->temperature;
        QString status = result? trUtf8("да") : trUtf8("нет");
        if (sender() && qobject_cast<QDeclarativeItem*>(sender())) {
            emit sendToPultLog(status);
        }
        return result;};	
    void RobotModule::runFieldSize(int& rows, int& cols)
    {
        rows=field->rows();
        cols=field->columns();
    };
    bool RobotModule::runMark(const int row, const int col)
    {
        return field->cellAt(row-1,col-1)->mark;
    };
    bool RobotModule::runColored(const int row, const int col)
    {
        return  field->cellAt(row-1,col-1)->isColored();   
    };
    void RobotModule::runRobotPos(int& row, int& col)
    {
        col=field->robotX()+1;
        row=field->robotY()+1;
    };
    
    QChar RobotModule::runUpChar(const int row, const int col)
    {
        if(row-1>field->rows() ||col-1>field->columns())
        {
           
            setError(trUtf8("Нет какой клетки!"));
            
            return ' ';
        }
        qDebug()<<field->cellAt(row-1,col-1)->upChar;
        return field->cellAt(row-1,col-1)->upChar;  
    };
    QChar RobotModule::runDownChar(const int row, const int col)
    {qDebug()<<field->cellAt(col-1,row-1)->downChar;
        return field->cellAt(col-1,row-1)->downChar;
    };   
    
 int RobotModule::LoadFromFile(QString p_FileName)
    {
        if(field->loadFromFile(p_FileName)!=0)return 1;
        startField=field->Clone();
        field->dropWasEdit();
     //   ajustWindowSize();//NEW ROBOT
        
        QFileInfo fi(p_FileName);
        QString name = fi.fileName();
        
        QString Title = trUtf8("Robot") + " - " + name;

        m_mainWidget->setWindowTitle(Title);

        
        
      //  ToDubl();
        
        
      //  SizeX = m_Size_x * m_FieldSize;
       // SizeY = m_Size_y * m_FieldSize + MenuHigth;
        
        
        
       // WasEditFlag=false;
        //delete btnFind;
        //CreatebtnFind();
        
        RobotModule::robotSettings()->setValue("Robot/StartField/File",p_FileName);
        field->drawField(FIELD_SIZE_SMALL);
       // if(CurrentRobotMode==ANALYZE_MODE)SetRobotMode(SEE_MODE);
        qDebug() << "File " << p_FileName ;
        setWindowSize();
        return(0);
    }
void RobotModule::editEnv()
    {
        if(field->isEditMode())return;
        startField->setModeFlag(NEDIT_MODE);
        
        
        view->showButtons(true);
         
        reset();
        field->setMode(NEDIT_MODE);
        startField->setModeFlag(NORMAL_MODE);
        
    };    
void RobotModule::loadEnv()
    {
        if(field->WasEdit())
        {
            if(QMessageBox::question(mainWidget(), "", QString::fromUtf8("Сохранить изменения?"), QMessageBox::Yes,QMessageBox::No,0) == QMessageBox::Yes)
            {
               saveEnv();
                
            }
            
        }
        
        
        
        //
        
        QString	RobotFile=QFileDialog::getOpenFileName(mainWidget(), QString::fromUtf8 ("Открыть файл"), curDir, "(*.fil)");
        
        
        
        QFileInfo info(RobotFile);
        QDir dir=info.absoluteDir();
        
        
        if ( RobotFile.isEmpty())return;
       // CurrentFileName = RobotFile;
       
        if( LoadFromFile(RobotFile)!=0)QMessageBox::information( mainWidget(), "", QString::fromUtf8("Ошибка открытия файла! ")+RobotFile, 0,0,0); 
            else updateLastFiles(RobotFile);
        
    }
    void RobotModule::resetEnv()
    {
        reset();
    }
    void RobotModule::prepareNewWindow()
    {
    NewWindow = new QDialog(mainWidget());
	QGridLayout *nwl = new QGridLayout;
	NewWindow->setLayout(nwl);
	NewWindow->setWindowModality(Qt::WindowModal);
	//NewWindow->setPalette (BackPall);
	NewWindow->setWindowTitle ( QString::fromUtf8("Новая обстановка"));
	QLabel *lKol = new QLabel(QString::fromUtf8("Количество:"),NewWindow,0);
	nwl->addWidget(lKol, 0, 0, 1, 2, Qt::AlignCenter);
    
	QLabel *lXSize = new QLabel(QString::fromUtf8("Столбцов"),NewWindow,0);
	nwl->addWidget(lXSize, 1, 1, 1, 1, Qt::AlignCenter);
    
    
	QLabel *lYSize = new QLabel(QString::fromUtf8("Строк"),NewWindow,0);
	nwl->addWidget(lYSize, 1, 0, 1, 1, Qt::AlignCenter);
    
	NewWindow->setFixedSize(250,150);
	eXSizeEdit = new QSpinBox(NewWindow);
	eXSizeEdit->setRange ( 1, MAX_COLUMNS );
	//eTemp->show();
	eYSizeEdit = new QSpinBox(NewWindow);
    
    
	eYSizeEdit->setRange ( 1, MAX_ROWS );
	nwl->addWidget(eYSizeEdit, 2, 0, 1, 1, Qt::AlignCenter);
	nwl->addWidget(eXSizeEdit, 2, 1, 1, 1, Qt::AlignCenter);
    
    
	QDialogButtonBox *buttonBox = new QDialogButtonBox(NewWindow);
    
	btnOK1 = buttonBox->addButton(QDialogButtonBox::Ok);
	btnCancel1 = buttonBox->addButton(QDialogButtonBox::Cancel);
   
    nwl->addWidget(buttonBox, 3, 0, 1, 2, Qt::AlignRight);  
    connect ( btnCancel1, SIGNAL(clicked()), NewWindow, SLOT(close()));
    connect ( btnOK1, SIGNAL(clicked()), this, SLOT(createNewField()) );
    }
    void RobotModule::createNewField()
    {
        field->destroyField();
        field->destroyRobot();
        field->createField(eYSizeEdit->value(),eXSizeEdit->value());
        field->setRoboPos(0,0);
        field->createRobot();
        startField=field->Clone();
        field->drawField(FIELD_SIZE_SMALL);
        
        mainWidget()->setWindowTitle(QString::fromUtf8("Робот - нет файла") );

        view->centerOn(field->m_height()/2,field->m_width()/2);
        NewWindow->close();
        editEnv();
    };
    void RobotModule::newEnv()
    {
        if(field->WasEdit())
        {
            QMessageBox::StandardButton r;
            
            QMessageBox messageBox(
                                   QMessageBox::Question,
                                   tr("New field"),
                                   tr("Save current field?"),
                                   QMessageBox::NoButton,mainWidget()
                                   );
            
            QPushButton * btnSave =
            messageBox.addButton(tr("Save"), QMessageBox::AcceptRole);
            QPushButton * btnDiscard =
            messageBox.addButton(tr("Don't save"), QMessageBox::DestructiveRole);
            QPushButton * btnCancel =
            messageBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
            
            messageBox.setDefaultButton(btnSave);
            messageBox.exec();
            if (messageBox.clickedButton()==btnSave) {
                r = QMessageBox::Save;
            }
            if (messageBox.clickedButton()==btnDiscard) {
                r = QMessageBox::Discard;
            }
            if (messageBox.clickedButton()==btnCancel) {
                r = QMessageBox::Cancel;
            }
            if (r==QMessageBox::Save) {saveEnv();};
            if (r==QMessageBox::Cancel) {return;};
            
        }
        
        NewWindow->show();
    }
void RobotModule::saveEnv()
    {
        QString	RobotFile=QFileDialog::getSaveFileName(mainWidget(), QString::fromUtf8 ("Сохранить файл"), curDir, "(*.fil)");
        
        
        //QString	RobotFile=dialog.selectedFiles().first();
        QFileInfo info(RobotFile);
        QDir dir=info.absoluteDir();
        curDir=dir.path();
        if (RobotFile.contains("*") || RobotFile.contains("?"))
        {
            QMessageBox::information( 0, "", QString::fromUtf8("Недопустимый символ в имени файла!"), 0,0,0);
            return;
        }
        //QString	RobotFile =  QFileDialog::getSaveFileName(MV,QString::fromUtf8 ("Сохранить в файл"),"/home", "(*.fil)");
        //if ( RobotFile.isEmpty())return;
        
        if(RobotFile.right(4)!=".fil")RobotFile+=".fil";
        //CurrentFileName = RobotFile;
        
        SaveToFile(RobotFile);
         RobotModule::robotSettings()->setValue("Robot/StartField/File",RobotFile);
        updateLastFiles(RobotFile);

    }
    
    

  
int RobotModule::SaveToFile(QString p_FileName)
    {
        

        if(field->saveToFile(p_FileName)!=0){QMessageBox::warning(0, tr("Robot"), QString::fromUtf8("Не удалось сохранить файл"));return 1;};
        
     
        return 0;
        
    }    
    

    
 void RobotModule::createRescentMenu()
    {
        rescentMenu->clear();
        QStringList lastFiles= mySettings()->value("Robot/LastFiles").toString().split(";");
        qDebug()<<lastFiles;
        if(lastFiles.count()==0)rescentMenu->setEnabled(false);else  rescentMenu->setEnabled(true);


        for(int i=0;i<lastFiles.count();i++) {
            if(lastFiles[i]=="")continue;
            QAction *action = rescentMenu->addAction("&"+QString::number(i+1)+" "+lastFiles[i],this,SLOT(openRecent()));
            Q_UNUSED(action);
            }
    };
void RobotModule::setWindowSize()
    {
        QRect baseFieldSize; //fieldSize in pixel. Zoom 1:1
        baseFieldSize.setHeight(field->rows()*mySettings()->value("Robot/CellSize").toInt());
        baseFieldSize.setWidth(field->columns()*mySettings()->value("Robot/CellSize").toInt());
        if(baseFieldSize.height()<view->height())
        {
            view->setGeometry(view->x(), view->y(), view->width(), baseFieldSize.height());
           // mainWidget()->setGeometry(view->x(), view->y(), view->width(), baseFieldSize.height());
        }
        if(baseFieldSize.width()<view->width())
            view->setGeometry(view->x(), view->y(), baseFieldSize.width(), view->height()); 
    }   
  void RobotModule::setDock(bool docked)
    {
        
    };
 void RobotModule::openRecent()
    {
        
        if(field->WasEdit())
        {
            QMessageBox::StandardButton r;
            
            QMessageBox messageBox(
                                   QMessageBox::Question,
                                   tr("New field"),
                                   tr("Save current field?"),
                                   QMessageBox::NoButton,mainWidget()
                                   );
            
            QPushButton * btnSave =
            messageBox.addButton(tr("Save"), QMessageBox::AcceptRole);
            QPushButton * btnDiscard =
            messageBox.addButton(tr("Don't save"), QMessageBox::DestructiveRole);
            QPushButton * btnCancel =
            messageBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
            
            messageBox.setDefaultButton(btnSave);
            messageBox.exec();
            if (messageBox.clickedButton()==btnSave) {
                r = QMessageBox::Save;
            }
            if (messageBox.clickedButton()==btnDiscard) {
                r = QMessageBox::Discard;
            }
            if (messageBox.clickedButton()==btnCancel) {
                r = QMessageBox::Cancel;
            }
            if (r==QMessageBox::Save) {saveEnv();};
            if (r==QMessageBox::Cancel) {return;};
            
        }

        QAction *s = qobject_cast<QAction*>(sender());
        QString txt = s->text();
        txt.remove(0,1);
        QStringList words = txt.split(" ");
        if(words.count()<2)return;
        QString RobotFile=words[1];
        if( LoadFromFile(RobotFile)!=0)QMessageBox::information( mainWidget(), "", QString::fromUtf8("Ошибка открытия файла! ")+RobotFile, 0,0,0);
        
    };
 void RobotModule::updateLastFiles(const QString newFile )
    {
     QStringList lastFiles= RobotModule::robotSettings()->value("Robot/LastFiles").toString().split(";");
        if(lastFiles.indexOf(newFile)<0)lastFiles.prepend(newFile);
        int max_fid=std::min(lastFiles.count(),11);
        QString sett="";
        for(int i=0;i<max_fid;i++)
        {
            sett+= lastFiles.at(i)+";";
        }
        RobotModule::robotSettings()->setValue("Robot/LastFiles",sett);  
        createRescentMenu();
    }

    RobotView::RobotView(RoboField * roboField)
    {
        setScene(roboField);
        pressed=false;
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setMouseTracking(true);
        setCursor(Qt::OpenHandCursor);
        robotField=roboField;
        textEditBtn=new QToolButton(this);
        textEditBtn->hide();
        textEditBtn->setCheckable ( true );
        textEditBtn->setIcon(QIcon(qApp->property("sharePath").toString()+
                                  "/actors/robot/text.png"));
        radEditBtn=new QToolButton(this);
        radEditBtn->setIcon(QIcon(qApp->property("sharePath").toString()+
                                  "/actors/robot/btn_radiation.png"));

        radEditBtn->hide();
        radEditBtn->setCheckable ( true );
        radEditBtn->move(textEditBtn->height(),0);
        tmpEditBtn=new QToolButton(this);
        tmpEditBtn->hide();
        tmpEditBtn->setCheckable ( true );
        tmpEditBtn->setIcon(QIcon(qApp->property("sharePath").toString()+
                                   "/actors/robot/btn_temperature.png"));
        tmpEditBtn->move(textEditBtn->height()*2+2,0);
        
        connect(textEditBtn,SIGNAL(toggled(bool)),this,SLOT(changeEditMode(bool)));
        connect(radEditBtn,SIGNAL(toggled(bool)),this,SLOT(changeEditMode(bool)));
        connect(tmpEditBtn,SIGNAL(toggled(bool)),this,SLOT(changeEditMode(bool)));
        setMinimumWidth ( 30 );
        c_scale=1;
      //  setRenderHint(QPainter::Antialiasing);
    };
    
    void RobotView::showButtons(bool flag)
    {
        textEditBtn->setVisible(flag);
        radEditBtn->setVisible(flag);
        tmpEditBtn->setVisible(flag);
    };
    void RobotView::mousePressEvent ( QMouseEvent * event )
    {
        if(robotField->isEditMode())
        {
         qDebug()<<"Edit mode;";
           QGraphicsView::mousePressEvent(event);   
            return;
        }
        if(robotField->sceneRect().height()*c_scale> this->height()  &&robotField->sceneRect().width()*c_scale> this->width())//field > view
        {
        pressed=true;
        setCursor(Qt::ClosedHandCursor);   
        };
        pressX=event->pos().x();
        pressY=event->pos().y();
         
    };
    
   void RobotView::mouseReleaseEvent ( QMouseEvent * event )
    {
        if(robotField->isEditMode())
            
        { //event->ignore ();
           // robotField->mouseReleaseEvent(event);
            if(robotField->robot->isMoving())
            {
               robotField->roboMoved(this->mapToScene(event->pos()));
                robotField->robot->setMoving(false);
            };
            qDebug()<<"Mouse Release EV (RobotView)";
            return;   
        }
        pressed=false;
        setCursor(Qt::OpenHandCursor);
    };
    
void RobotView::mouseMoveEvent ( QMouseEvent * event )
    { 
        if(robotField->isEditMode())
        {
        setCursor(Qt::ArrowCursor);    
        QGraphicsView::mouseMoveEvent(event);
        return;
            
        }
        setCursor(Qt::ArrowCursor);
       if(robotField->sceneRect().height()> this->height()  &&robotField->sceneRect().width()> this->width())//field size more then view size
          {
        setCursor(Qt::OpenHandCursor);
           }
        if(!pressed)return;
        setCursor(Qt::ClosedHandCursor);
        QPointF center = mapToScene(viewport()->rect().center());
        qDebug()<<"==========Move==========";
        qDebug()<<"Old center:"<<center;
        qreal diffX=pressX-event->pos().x();
        qreal diffY=pressY-event->pos().y();
        if(diffX==0 && diffY==0)return;
        center.setX(center.x()+diffX);
        center.setY(center.y()+diffY);
        qDebug()<<"New center:"<<center<<" DiffX"<<diffX;
 
        if(diffX< width() && diffY<height())centerOn(center);
       // qDebug()<<mapFromScene (robotField->sceneRect())<<robotField->sceneRect().height();
        
    };
void	RobotView::wheelEvent ( QWheelEvent * event )
    {
        float numDegrees = event->delta() / 8;
        qDebug()<<"whell:"<<numDegrees;
//        c_scale=c_scale*0.8;
        qDebug()<<"Scale"<<c_scale;
      //setRenderHint(QPainter::Antialiasing);
        if(numDegrees>0)
        { if(c_scale<3 && c_scale>0.003)this->scale(1.2,1.2);
          c_scale=c_scale*1.2;
        }
        else{ 
          if(c_scale<3 && c_scale>0.003)this->scale(0.8,0.8);
          c_scale=c_scale*0.8;
        }
        
    }
    
 void RobotView:: FindRobot()
    {
        centerOn(robotField->roboPosF());
    };
void RobotView::changeEditMode(bool state)
    {
        QToolButton *clicked = qobject_cast<QToolButton*>(sender());
        if(radEditBtn!=clicked && radEditBtn->isChecked())radEditBtn->setChecked(false); 
        if(textEditBtn!=clicked && textEditBtn->isChecked())textEditBtn->setChecked(false);
        if(tmpEditBtn!=clicked && tmpEditBtn->isChecked())tmpEditBtn->setChecked(false); 
        if(clicked->isChecked()!=state)clicked->setChecked(state);
       if(!textEditBtn->isChecked () && !radEditBtn->isChecked ())robotField->setMode(NEDIT_MODE);
        if(textEditBtn->isChecked ())
        {
          
            robotField->setMode(TEXT_MODE); 
        }
        if(radEditBtn->isChecked ())
        {
                    robotField->setMode(RAD_MODE); 
        }; 
        if(tmpEditBtn->isChecked ())
        {
            robotField->setMode(TEMP_MODE); 
        }; 
        
    };
} // $namespace
