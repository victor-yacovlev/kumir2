/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents.
You should change it corresponding to functionality.
*/

// Self include
#include "widgets/draw_turtle_field.h"
#include "drawmodule.h"

// Kumir includes
#include "extensionsystem/kplugin.h"
#include "extensionsystem/pluginmanager.h"


// Qt includes
#include <QtCore>
#include <QtGui>


namespace ActorDraw {
#define NET_RESERVE 15
#define KUM_MULTI 50
    
    ExtensionSystem::SettingsPtr DrawModule::DrawSettings()
    {
        ExtensionSystem::PluginManager * pluginManager = ExtensionSystem::PluginManager::instance();
        ExtensionSystem::KPlugin * plugin = pluginManager->loadedPlugins("ActorDraw")[0];
        return pluginManager->settingsByObject(plugin);
    }//Get  settings
    
    static const qreal MAX_ZOOM = 1000000;
    
    
        
    
    
    
    
    QRectF DrawScene::getRect() //User lines bounding rect
    {
        QRectF boundRect=QRectF(QPointF(-1,5),QPointF(4,-1));//default rect
  
        for(int i=0;i<lines.count();i++)
        {
            qreal deb=0;
            if(lines.at(i)->line().dx()>=0)
            {
                if(lines.at(i)->line().p1().x()<boundRect.left() || (i==0))
                {
                    deb=lines.at(i)->line().p1().x();
                    boundRect.setLeft(lines.at(i)->line().p1().x()); 
                    
                }
                if(lines.at(i)->line().p2().x()>boundRect.right() || (i==0))
                {
                    boundRect.setRight(lines.at(i)->line().p2().x()); 
                    
                }
         
            }
            if(lines.at(i)->line().dx()<0)
            {
                if(lines.at(i)->line().p2().x()<boundRect.left() || (i==0))
                {
                    boundRect.setLeft(lines.at(i)->line().p2().x()); 
                } 
                if(lines.at(i)->line().p1().x()>boundRect.right() || (i==0))
                {
                    boundRect.setRight(lines.at(i)->line().p1().x()); 
                    
                }
             
            }
            if(lines.at(i)->line().dy()>=0)
            {
                qDebug()<<"Bott"<<boundRect.bottom()<<"Top:"<<boundRect.top()<<"line p1y:"<<lines.at(i)->line().p1().y()<<"line p2y:"<<lines.at(i)->line().p2().y();
                if(-lines.at(i)->line().p1().y()>boundRect.bottom() || (i==0))
                {
                    deb=-lines.at(i)->line().p1().y();
                    boundRect.setBottom(-lines.at(i)->line().p1().y()); 
                }
                if(lines.at(i)->line().p2().x()>boundRect.top() || (i==0))
                {
                    boundRect.setTop(-lines.at(i)->line().p2().y()); 
                    
                }
            }
            if(lines.at(i)->line().dy()<0)
            {
                qDebug()<<"Bott"<<boundRect.bottom()<<"Top:"<<boundRect.top()<<"line p1y:"<<lines.at(i)->line().p1().y()<<"line p2y:"<<lines.at(i)->line().p2().y();
                if(lines.at(i)->line().p2().y()<boundRect.top())
                {
                    boundRect.setTop(-lines.at(i)->line().p2().y()); 
                } 
                if(lines.at(i)->line().p1().y()>boundRect.bottom() || (i==0))
                {
                    boundRect.setBottom(lines.at(i)->line().p1().y()); 
                    
                }
            }
        }
        
        //if(boundRect.width()!=boundRect.height())
       // {
       //     qreal size=qMax(boundRect.width(),boundRect.width());
       //     boundRect.setWidth(size);
       //     boundRect.setHeight(size);
            
      //  }
        
        return(boundRect);
    };
    
    bool DrawScene::isUserLine(QGraphicsItem* obj)
    {
        for(int i=0;i<lines.count();i++)
        {
            if(lines.at(i)==obj)return true;
        }
        return false;
    };
    
    bool DrawScene::isLineAt(const QPointF &pos,qreal radius)
    {
        QGraphicsEllipseItem * testCirc=addEllipse(QRectF(pos.x()-radius,pos.y()-radius,radius*2,radius*2));
        QList<QGraphicsItem *> colItems=collidingItems(testCirc);
        delete testCirc;
     
        for (int i=0;i<colItems.count();i++)
            {
             
             if(isUserLine(colItems.at(i)))
                return true;
                   
            }
        return false;
        
    };
    
    qreal DrawScene::drawText(const QString &Text,qreal widthChar,QPointF from,QColor color)
    {
        QFont font ( "Courier", 12);
        font.setPointSizeF(KUM_MULTI);
        QFontMetricsF fontMetric(font);
        qreal bs=fontMetric.boundingRect(Text).width();
        qreal ch=bs/Text.length();
        qreal psizeF=widthChar/bs;
       
        
   
        //  font.setPointSizeF(psizeF*inc);
          bs=fontMetric.boundingRect("OOOXX").width()/5;
        
       
        fontMetric=QFontMetricsF(font);
        qDebug()<<"Char Size:"<<fontMetric.boundingRect("OOOXX").width()/5000;
        
        
        texts.append(addSimpleText(Text,font));
//        texts.last()->scale(0.001,0.001);
        texts.last()->setScale(psizeF);
        texts.last()->setPos(from.x(), from.y()-(fontMetric.boundingRect(Text).height()*psizeF));
        texts.last()->setPen(QPen(color));
        texts.last()->setZValue(90);
        return widthChar;
    };
    void DrawScene::DestroyNet()
    {
      //  qDebug()<<Netlines.count();
        for ( int i = 0; i < Netlines.count(); i++)
        {
       
           delete Netlines[i];
        }
        Netlines.clear();
        
    }
    void DrawScene::drawOnlyAxis(double startx ,double endx,double starty,double endy)
    {
        
        QPen axisPen=QPen(DRAW->axisColor());
        Netlines.append(addLine(startx-qAbs(startx-endx),0  , endx+qAbs(startx-endx), 0 ));
        Netlines.last()->setPen(axisPen);
        Netlines.last()->setZValue(1);
        
        Netlines.append(addLine(0,starty-qAbs(starty-endy),0 , endy+qAbs(startx-endx) ));
        Netlines.last()->setPen(axisPen);
        Netlines.last()->setZValue(1);

    }
    
    
    
    void DrawScene::drawNet(double startx ,double endx,double starty,double endy,QColor color,double stepX,double stepY,bool net)
    {
        
        QPointF pos,tail;    
        QColor AxisColor=DRAW->axisColor();
        
        DestroyNet();
        if(!net)
        {
            drawOnlyAxis(startx,endx,starty,endy);
            return;
        }
        int lines=qRound(startx/stepX);
        startx=lines*stepX;
        double fx1=startx-NET_RESERVE*stepX,fx2,fy1,fy2; 

       // return;
        while (fx1 < endx+NET_RESERVE*stepX)
		{
			fx1 = fx1 + stepX;
			fx2 = fx1;
			fy1 = starty-NET_RESERVE*stepX;
			fy2 = endy+NET_RESERVE*stepX;
            
			Netlines.append(addLine(fx1, fy1 , fx2, fy2 ));
			Netlines.last()->setZValue(0.5);
			Netlines.last()->setPen(QPen(color));
            if(fx1>0-1/DRAW->zoom() && fx1<0+1/DRAW->zoom())
            {
                QPen axisPen=QPen(AxisColor);
               // axisPen.setWidth(3/DRAW->zoom());
                Netlines.last()->setPen(axisPen); 
                Netlines.last()->setZValue(1);
                Netlines.append(addLine(fx1+1/DRAW->zoom(), fy1 , fx2+1/DRAW->zoom(), fy2 ));
                Netlines.last()->setZValue(1);
                Netlines.last()->setPen(axisPen); 
            }
		}
      //  Netlines.append(addLine(-1, -1 , 1, 1 ));
       // Netlines.append(addLine(1, -1 , -1, 1 ));
        lines=starty/stepY;
        starty=lines*stepY;
        fy1 = starty-NET_RESERVE*stepY;
        
		while (fy1 < endy+NET_RESERVE*stepY)
		{
			fy1 = fy1 + stepY;
			fy2 = fy1;
			fx1 = startx-NET_RESERVE*stepY;
			fx2 = endx+NET_RESERVE*stepY;
            
            Netlines.append(addLine(fx1, fy1 , fx2, fy2 ));
			Netlines.last()->setZValue(0.5);
			Netlines.last()->setPen(QPen(color));
            if(fy1>0-1/DRAW->zoom() && fy1<0+1/DRAW->zoom())
            {
                QPen axisPen=QPen(AxisColor);
               // axisPen.setWidth(6/(DRAW->zoom()*2));
               // qDebug()<<"Width"<<6/(DRAW->zoom()*2);
                Netlines.last()->setPen(axisPen);
                Netlines.last()->setZValue(1);
                Netlines.append(addLine(fx1, fy1+1/DRAW->zoom() , fx2, fy2+1/DRAW->zoom() ));
                Netlines.last()->setZValue(1);
                Netlines.last()->setPen(axisPen);  
            }
            
		}
    }
    
int   DrawScene::loadFromFile(const QString& p_FileName)
    {
        QFileInfo fi(p_FileName);
        QString name = fi.fileName();                        
        QString Title = QString::fromUtf8("Чертежник - ") + name;
        double CurX,CurY;
      //  MV->setWindowTitle ( Title);
        qreal CurrentScale;
        
        QString tmp = "";
        char ctmp[200];
        QString l_String;
        QFile l_File(p_FileName);
        QColor CurColor;
        
        
        int NStrok;
        NStrok = 0;
        
        //	long l_Err;
        //int CurX,CurY;
        //	int SizeX, SizeY;
        qreal x1,y1,x2,y2;
        if  (!l_File.open(QIODevice::ReadOnly))
        {
            QMessageBox::information( NULL, "", QString::fromUtf8("Ошибка открытия файла"), 0,0,0);
            return 1;
        }
        
        //l_String = l_File.readLine();
        //QMessageBox::information( 0, "", tmp, 0,0,0);
        QByteArray ttt;
        ttt = l_File.readLine();
        l_String = QString::fromUtf8(ttt);
        
        //QMessageBox::information( MV, "",l_String , 0,0,0);
        if (l_String.isNull())
        {
            l_File.close();
            QMessageBox::information( NULL, "", QString::fromUtf8("Ошибка чтения строки"), 0,0,0);
            return 1;
        }
        l_String = l_String.simplified();
        QStringList l_List = l_String.split(' ');
        
        
        if (!(l_String == "%!PS-Adobe-1.0 EPSF-1.0"))
        {
            l_File.close();
            QMessageBox::information(NULL, "",QString::fromUtf8("Это не PS - файл") , 0,0,0);
            
            return 1;
        }
        
        // Вторая строка %%Creator: Cherteznik
        //	l_String = l_File.readLine();
        ttt = l_File.readLine();
        l_String = QString::fromUtf8(ttt);
        
        NStrok++;
        
        l_String = l_String.simplified();
        l_List = l_String.split(' ');
        if(!(l_List[1] == "Cherteznik"))
            //if (!(l_String == "%Creator: Cherteznik"))
        {
            
            QMessageBox::information(NULL, "",QString::fromUtf8("Это не файл чережника") , 0,0,0);
            
            l_File.close();
            return 1;
        }
        
        
        for (int i =0; i<15;i++)
        {
            //	l_String = l_File.readLine();
            ttt = l_File.readLine();
            l_String = QString::fromUtf8(ttt);
            NStrok++;
        }
        
        // koordinaty vektorov
       // CurZ = 1.;
        while (!l_File.atEnd())
        {
            //считываем цвет
            
            //	l_String = l_File.readLine();
            ttt = l_File.readLine();
            l_String = QString::fromUtf8(ttt);
            NStrok++;
            
            
            if (l_String.isNull())
            {
                QMessageBox::information( NULL, "",QString::fromUtf8("Ошибка чтения строки") , 0,0,0);
                l_File.close();
                return 1;
            }
            
            l_String = l_String.simplified();
            l_List = l_String.split(' ');
            if (l_List.count() == 0)continue;
            
            
            if(l_List[l_List.count() - 1] == "stroke")break;
            
            if(l_List[l_List.count() - 1] == "setfont")
            {
                CurrentScale = x1 = l_List[2].toFloat();
                continue;
            }
            
            if(l_List[l_List.count() - 1] == "show")
            {
                tmp = "";
                int NWord = l_List.count()-1;
                for (int j = 0; j < NWord; j++)
                {
                    tmp += l_List[j]+" ";
                }
                //		QMessageBox::information( 0, "",  QString::fromUtf8(tmp), 0,0,0);
                int l = tmp.length();
                tmp.chop(2);
                tmp = tmp.right(l-3);
                drawText(tmp,CurrentScale,QPoint(0,0),CurColor);
                continue;
            }
            
            if (l_List[l_List.count() - 1] == "setrgbcolor")
            {
                CurColor.setRed( l_List[0].toInt());
                CurColor.setGreen(l_List[1].toInt());
                CurColor.setBlue ( l_List[2].toInt());
                continue;
            }
            
            
            
            
            if(l_List[l_List.count() - 1] == "moveto")
            {
                x1 = l_List[0].toFloat();
                y1 = -l_List[1].toFloat();
                CurX = x1;
                CurY = -y1;
                continue;
            }
            
            if(l_List[l_List.count() - 1] == "lineto")
            {
                x2 = l_List[0].toFloat();
                y2 = -l_List[1].toFloat();
                lines.append(addLine(x1, y1 , x2, y2 ));
                lines.last()->setZValue(10);
                lines.last()->setPen(QPen(CurColor));
               // CurZ += 0.01;
                CurX = x2;
                CurY = -y2;
                continue;
            }
            
            sprintf(ctmp,"%4i",NStrok);
            tmp = QString::fromUtf8("Ошибка в строке ");
            tmp.append(QString(ctmp));
            
            break;
        }
        
        l_File.close();
        
        
        

        return 0;
        

    }
int DrawScene::saveToFile(const QString& p_FileName)
    {
      	QFile l_File(p_FileName);
        QChar Bukva;
        char ctmp[200];
        if  (!l_File.open(QIODevice::WriteOnly))
        {
            return 1;
        }
        
        //QString ttt = QString::fromUtf8("Чертежник - Начало");
        
        //l_File.write( ttt.toUtf8());
        l_File.write( "%!PS-Adobe-1.0 EPSF-1.0\n");
        QString l_String;
        l_File.write( "%%Creator: Cherteznik\n");
        l_File.write("%%Pages: 1\n");
        l_File.write("%%Orientation: Portrait\n");
        
        
        // maximum, minimum
        
        qreal MinX,MaxX,MinY,MaxY,VecX1,VecX2,VecY1,VecY2;
        
        QLineF TmpLine;
        MinX = 1000000;
        MinY = 1000000;
        
        MaxX = -1000000;
        MaxY = -1000000;
        
        
        for (int i = 0; i <lines.count(); i++)
        {
            
            TmpLine = lines[i]->line();
            VecX1 = TmpLine.x1();
            VecY1 = -TmpLine.y1();
            VecX2 = TmpLine.x2();
            VecY2 = -TmpLine.y2();
            if (VecX1 < MinX)MinX = VecX1;
            if (VecY1 < MinY)MinY = VecY1;
            if (VecX1 > MaxX)MaxX = VecX1;
            if (VecY1 > MaxY)MaxY = VecY1;
            
            if (VecX2 < MinX)MinX = VecX2;
            if (VecY2 < MinY)MinY = VecY2;
            if (VecX2 > MaxX)MaxX = VecX2;
            if (VecY2 > MaxY)MaxY = VecY2;
            
        }
        double Scale;
        
        if (MaxX - MinX > MaxY - MinY)
        {
            Scale = (596-10)/(MaxX-MinX);
        }
        else
        {
            Scale = (842-10)/(MaxY-MinY);
        }
        Scale = Scale*0.9;
        
        //	QString tmp1 = QString(ctmp)+" scale\n";
        
        
        
        l_File.write("%%BoundingBox: 0 0 596 842\n");
        l_File.write("%%HiResBoundingBox: 0 0 596 842\n");
        l_File.write( "%%EndComments\n");
        l_File.write( "%%Page: 1 1\n");
        sprintf(ctmp,"%f %f translate\n",-MinX,-MinY);
        l_File.write(ctmp);
        sprintf(ctmp,"%f %f scale\n",Scale,Scale);
        l_File.write(ctmp);
        l_File.write("gsave [1 0 0 1 0 0] concat\n");
        l_File.write("0 0 0 setrgbcolor\n");
        l_File.write("[] 0 setdash\n");
        l_File.write("1 setlinewidth\n");
        l_File.write("0 setlinejoin\n");
        l_File.write("0 setlinecap\n");
        l_File.write("newpath\n");
        //QColor TmpColor;
        QPen TmpPen;
        QColor TmpColor;
        for (int i = 0; i <lines.count(); i++)
        {
            
            TmpLine = lines[i]->line();
            TmpPen = lines[i]->pen();
            TmpColor = TmpPen.color();
            sprintf(ctmp,"%i %i %i setrgbcolor\n", TmpColor.red(),  TmpColor.green(), TmpColor.blue());
            l_File.write(ctmp);
            
            VecX1 = TmpLine.x1();
            VecY1 = -TmpLine.y1();
            VecX2 = TmpLine.x2();
            VecY2 = -TmpLine.y2();
            
            sprintf(ctmp,"%f %f moveto\n", VecX1, VecY1);
            l_File.write(ctmp);
            
            sprintf(ctmp,"%f %f lineto\n", VecX2, VecY2);
            
            l_File.write(ctmp);
            
            
        }
        
        //77777777777777777777777777777777777
        QString TmpText;
        QByteArray ccc;
        qreal tmpX,tmpY,FontSize;
        for (int i = 0; i<texts.count(); i++)
        {
            FontSize = texts[i]->font().pointSizeF();
            sprintf(ctmp,"/Curier findfont %f scalefont setfont\n",FontSize);
            l_File.write(ctmp);
            
            tmpX = texts[i]->pos().x();
            tmpY = texts[i]->pos().x();
            sprintf(ctmp,"%f %f moveto\n", tmpX, tmpY);
            l_File.write(ctmp);
            
            
            //TmpPen = texts[i]->pen();
            //TColor = TmpPen.color();
            sprintf(ctmp,"%i %i %i setrgbcolor\n", texts[i]->pen().color().red(), texts[i]->pen().color().green(), texts[i]->pen().color().blue());
            l_File.write(ctmp);
            
            TmpText = "("+texts[i]->text()+") show\n";
            ccc = TmpText.toUtf8();
            l_File.write(ccc);
        }
        
       
        
        l_File.write("stroke\n");
        l_File.write("grestore\n");
        l_File.write( "showpage\n");
        l_File.close();
        return 0;
        
        
    };    
void DrawView::resizeEvent ( QResizeEvent * event )
    {
        if(firstResize)
        {
                  qDebug()<<"FirstresizeEvent";
           QPointF maxRight=mapToScene(geometry().bottomRight());
            
            centerOn(maxRight.x()/2-maxRight.x()/5,-maxRight.y()/2+maxRight.y()/4);
        }
        firstResize=false;
        qDebug()<<"resizeEvent";
       // setViewportUpdateMode (QGraphicsView::NoViewportUpdate);
       if(!pressed)  DRAW->drawNet();
        update();
       // setViewportUpdateMode (QGraphicsView::SmartViewportUpdate);
    };
  
    
    void DrawView::mousePressEvent ( QMouseEvent * event )
    {
        pressed=true;
        press_pos=event->pos();
        qDebug()<<"Mouse press"<<mapToScene(press_pos);
    
    };
    void DrawView::mouseReleaseEvent ( QMouseEvent * event )
    {
        pressed=false;
        DRAW->drawNet();
    };
    void DrawView::mouseMoveEvent ( QMouseEvent * event )
    {
        if(pressed)
        {
            setViewportUpdateMode (QGraphicsView::SmartViewportUpdate);
            QPointF delta=mapToScene(press_pos)-mapToScene(event->pos());
            //if(qAbs(delta.x())>100)press_pos.setX(event->pos().x());
            //if(qAbs(delta.y())>100)press_pos.setY(event->pos().y());
            QPointF center = mapToScene(viewport()->rect().center());
            //QPointF center = sceneRect().center();
             qDebug()<<"CenterOn"<<center;
           // center.setX(center.x()+(mapToScene(press_pos).x()-mapToScene(event->pos()).x()));
           // center.setY(center.y()+(mapToScene(press_pos).y()-mapToScene(event->pos()).y()));
         //   scrollContentsBy(press_pos.x()-event->pos().x(), 10);
            verticalScrollBar()->setValue(verticalScrollBar()->value()+(press_pos.y()-event->pos().y()));
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() +(press_pos.x()-event->pos().x()));
            // centerOn(center);
          
           qDebug()<<"CenterOn"<<center<<" realCenter"<<mapToScene(viewport()->rect().center());
            qDebug()<<"SCENERECTCENTER"<<sceneRect().center().x();
            qDebug()<<"DELTA"<<delta<<" xd"<<press_pos.x()-event->pos().x()<<" yd"<<mapToScene(press_pos).y()-mapToScene(event->pos()).y();
            press_pos=event->pos();
            qDebug()<<"Ppos"<<press_pos;
            update();
        }
    }; 
    void DrawView::setZoom(double zoom)
    {
        if(zoom>MAX_ZOOM)
        {return;}
        if(zoom<1/MAX_ZOOM)
        {return;}
       this->scale(zoom/c_scale,zoom/c_scale);
        
        c_scale=zoom;
    };
 
    void DrawView::setNet()
    {
        if(DRAW->isAutoNet())
        {
            net=true;
             smallNetLabel->hide();
            double pixel_per_cell=DRAW->NetStepX()/(1/c_scale);
            qreal stepX=DRAW->NetStepX();
            qreal stepY=DRAW->NetStepY();
            
            
            if(stepX>5)stepX=(int)(stepX-0.5);
            if(stepY>5)stepY=(int)(stepY-0.5);
            
            if(pixel_per_cell>100)
            {
                stepX=DRAW->NetStepX()*0.5;
                stepY=DRAW->NetStepY()*0.5;
                if(stepX>5)stepX=(int)(stepX-0.5);
                if(stepY>5)stepY=(int)(stepY-0.5);
                DRAW->setNetStepX(stepX);
                DRAW->setNetStepY(stepY);
                
               // DRAW->scalePen(0.5);
                
            }
            if(pixel_per_cell<15)
            {
                stepX=DRAW->NetStepX()*2;
                 stepY=DRAW->NetStepX()*2;
                //if(stepX>5)stepX=(int)(stepX-0.5);
                //if(stepY>5)stepY=(int)(stepY-0.5);
                DRAW->setNetStepX(stepX);
                DRAW->setNetStepY(stepY);
                DRAW->drawNet();
                //DRAW->scalePen(1.2);
               
            }
            DRAW->setNetStepX(stepX);
            DRAW->setNetStepY(stepY);
            lastStep=stepX;
            qDebug()<<"c_scale"<<c_scale<<"NetStep"<<DRAW->NetStepX()<<"PPC"<<pixel_per_cell;
             update();
        }
        else
        {
           double pixel_per_cell=DRAW->NetStepX()/(1/c_scale);
            if(!net)pixel_per_cell=lastStep/(1/c_scale);
            if(pixel_per_cell<15) //Net step too short
            {
             net=false;
                smallNetLabel->show();
               // lastStep=DRAW->NetStepX();
            }else
            {
                if(pixel_per_cell>15 && !net && pixel_per_cell<this->width()*2)
                {
                 net=true;
                    smallNetLabel->hide();
                    DRAW->setNetStepX(lastStep);
                    DRAW->setNetStepY(lastStep);
                }
                if(pixel_per_cell>this->width()*2)
                {
                   net=false; 
                }
                  
            }
            
                
        }
    };
    
    void	DrawView::wheelEvent ( QWheelEvent * event )
    {
        float numDegrees = event->delta() / 8;
        qDebug()<<"whell:"<<numDegrees;
        //        c_scale=c_scale*0.8;
        qDebug()<<"Scale"<<c_scale;
        
        
        //setRenderHint(QPainter::Antialiasing);
        if(numDegrees>0)
        {
            
 
            setZoom(zoom()*1.189207);
            setNet();
            DRAW->scalePen(DRAW->Pen()->scale()*(1/1.189207));
              DRAW->drawNet();
        }
        else
        {
            if(c_scale<3e-05)return;
            setZoom(zoom()*(1/1.189207));
            setNet();
            DRAW->scalePen(DRAW->Pen()->scale()*((1.189207)));
            DRAW->drawNet();
            
        }
        
    }    

DrawModule::DrawModule(ExtensionSystem::KPlugin * parent)
    : DrawModuleBase(parent)
{         
    CurView = 0;
}

void DrawModule::createGui()
{
    CurView=new DrawView();
    netStepX=1;
    netStepY=1;
    autoNet=true;
    netColor=QColor("gray");
    penIsDrawing=false;
    CurScene=new DrawScene(CurView);
    navigator=new DrawNavigator(CurView);
    showToolsBut=new QToolButton(CurView);
    showToolsBut->move(20,20);
    showToolsBut->setCheckable(true);


    connect(showToolsBut,SIGNAL(toggled (bool)),this,SLOT(showNavigator(bool)));

    connect(m_actionDrawSaveDrawing,SIGNAL(triggered()),this,SLOT(saveFile()));
    connect(m_actionDrawLoadDrawing,SIGNAL(triggered()),this,SLOT(openFile()));
    connect(navigator,SIGNAL(redrawNet()),this,SLOT(drawNet()));
    connect(navigator,SIGNAL(autoNetChange(bool)),this,SLOT(autoNetChange(bool)));
     connect(navigator,SIGNAL(netStepChange(double)),this,SLOT(netStepChange(double)));
    
   // navigator->setDraw(this);
    connect(navigator->zoomUp,SIGNAL(pressed()),this,SLOT(zoomIn()));
    connect(navigator->zoomDown,SIGNAL(pressed()),this,SLOT(zoomOut()));
    connect(navigator->zoomNormal,SIGNAL(pressed()),this,SLOT(zoomNorm()));
    connect(navigator->zoomFullDraw,SIGNAL(pressed()),this,SLOT(zoomFullDraw()));
    
    
    navigator->setParent(CurView);
    navigator->setFixedSize(QSize(120,175));
    
    navigator->move(20,showToolsBut->pos().y()+showToolsBut->height());
    navigator->hide();

    CurScene->setDraw(this);
    CurView->setScene(CurScene);
    penColor.r = penColor.g = penColor.b = 0;
    penColor.a = 255;
    CurView->setDraw(this);
    CurView->centerOn(5,-5);
    CurView->setViewportUpdateMode (QGraphicsView::NoViewportUpdate);//For better perfomance; Manual Update;
    drawNet();
    CreatePen();
    CurView->setZoom(50);
    CurView->setNet();
    netStepX=1;
    netStepY=1;



    CurView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    CurView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QBrush curBackground=QBrush(QColor(DrawSettings()->value("Draw/BackColor","lightgreen").toString()));

    CurScene->setBackgroundBrush (curBackground);
    Color Black;
       Black.r=0;Black.g=0;Black.b=0;Black.a=255;
    
    penColor=Black;
}

QString DrawModule::initialize(const QStringList &configurationParameters, const ExtensionSystem::CommandLine &)
{
    if (!configurationParameters.contains("tablesOnly")) {
        createGui();
       
    }
    return "";
}
    
void  DrawModule::openFile()
    {
        QString	File=QFileDialog::getOpenFileName(mainWidget(), QString::fromUtf8 ("Открыть файл"), curDir.path(), "(*.ps)");
        
        
        
        QFileInfo info(File);
        QDir dir=info.absoluteDir();
        
        
        if ( File.isEmpty())return;
        // CurrentFileName = RobotFile;
        
        if( CurScene->loadFromFile(File)!=0)//Get troubles when loading env.
        {
            QMessageBox::information(CurView, "", QString::fromUtf8("Ошибка открытия файла! ")+File, 0,0,0); 
            return;
        }
        
    
        
;

    };
void  DrawModule::saveFile()
    {
        QString	File=QFileDialog::getSaveFileName(CurView, QString::fromUtf8 ("Сохранить файл"),curDir.path(), "(*.ps)");
        
        
        //QString	RobotFile=dialog.selectedFiles().first();
        QFileInfo info(File);
        QDir dir=info.absoluteDir();
        curDir=dir.path();
        if (File.contains("*") || File.contains("?"))
        {
            QMessageBox::information( 0, "", QString::fromUtf8("Недопустимый символ в имени файла!"), 0,0,0);
            return;
        }
        //QString	RobotFile =  QFileDialog::getSaveFileName(MV,QString::fromUtf8 ("Сохранить в файл"),"/home", "(*.fil)");
        //if ( RobotFile.isEmpty())return;
        
        if(File.right(4)!=".ps")File+=".ps";
        //CurrentFileName = RobotFile;
        
        CurScene->saveToFile(File);
    };    

void DrawModule::showNavigator(bool state)
    {
        navigator->setVisible(state);
    };
    
/* public static */ QList<ExtensionSystem::CommandLineParameter> DrawModule::acceptableCommandLineParameters()
{
    // See "src/shared/extensionsystem/commandlineparameter.h" for constructor details
    return QList<ExtensionSystem::CommandLineParameter>();
}

/* public slot */ void DrawModule::changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current)
{
    // Called when changed kumir state. The states are defined as enum ExtensionSystem::GlobalState:
    /*
    namespace ExtensionSystem {
        enum GlobalState {
            GS_Unlocked, // Edit mode
            GS_Observe, // Observe mode
            GS_Running, // Running mode
            GS_Input,  // User input required
            GS_Pause  // Running paused
        };
    }
    */
    // TODO implement me
    using namespace ExtensionSystem;  // not to write "ExtensionSystem::" each time in this method scope
    Q_UNUSED(old);  // Remove this line on implementation
    Q_UNUSED(current);  // Remove this line on implementation
  
    CurView->forceRedraw();
    CurScene->update(CurScene->sceneRect());
    CurView->repaint();
    CurView->viewport()->update();
}

/* public slot */ void DrawModule::loadActorData(QIODevice * source)
{
    // Set actor specific data (like environment)
    // The source should be ready-to-read QIODevice like QBuffer or QFile
    Q_UNUSED(source);  // By default do nothing

}

/* public */ QWidget* DrawModule::mainWidget() const
{

    return  CurView;

}

/* public */ QWidget* DrawModule::pultWidget() const
{
 //No pult for Draw module.
    return nullptr;
}

/* public slot */ void DrawModule::reloadSettings(ExtensionSystem::SettingsPtr settings, const QStringList & keys)
{
    // Updates setting on module load, workspace change or appliyng settings dialog.
    // If @param keys is empty -- should reload all settings, otherwise load only setting specified by @param keys
    // TODO implement me
    QBrush curBackground=QBrush(QColor(settings->value("BackColor","lightgreen").toString()));
    
    CurScene->setBackgroundBrush (curBackground);
    netColor=QColor(settings->value("LineColor","gray").toString());
    drawNet();
    Q_UNUSED(keys);
}

/* public slot */ void DrawModule::reset()
{
    // Resets module to initial state before program execution
    // TODO implement me
    mPen->setBrush(QBrush(QColor("white")));
    penIsDrawing=false;
    mPen->setPos(0,0);
    CurScene->reset();
    Color Black;
    Black.r=0;Black.g=0;Black.b=0;Black.a=255;
    
    penColor=Black;
    QList<QRectF> rect;
    rect.append(CurView->sceneRect());
    CurView->updateScene(rect);
    CurView->show();
    CurView->forceRedraw();
    CurView->setZoom(CurView->zoom()*2);
    CurView->setZoom(CurView->zoom()*0.5);
}

/* public slot */ void DrawModule::setAnimationEnabled(bool enabled)
{
    // Sets GUI animation flag on run
    // NOTE this method just setups a flag and might be called anytime, even module not needed
    // TODO implement me
    Q_UNUSED(enabled);  // Remove this line on implementation
}

/* public slot */ void DrawModule::runSetupPen()
{
    /* алг опустить перо */
    // TODO implement me
     mPen->setBrush(QBrush(QColor(penColor.r, penColor.g, penColor.b, penColor.a)));
    penIsDrawing=true;
}

/* public slot */ void DrawModule::runReleasePen()
{
    /* алг поднять перо */
    // TODO implement me

    mutex.lock();

    mPen->setBrush(QBrush(QColor("white")));

    penIsDrawing=false;
    mutex.unlock();
    
}

/* public slot */ void DrawModule::runSetPenColor(const Color& color)
{
    /* алг установить цвет(цвет color) */
   
    penColor=color;
    mPen->setBrush(QBrush(QColor(penColor.r, penColor.g, penColor.b, penColor.a)));
    qDebug()
            << "DrawModule::runSetPenColor( { cssValue = \""
            << QColor(color.r, color.g, color.b, color.a).name()
            << "\" } )"<<color.a;
}

/* public slot */ void DrawModule::runMoveTo(const qreal x, const qreal y)
{
    mutex.lock();
    QPointF start=mPen->pos();
    mPen->setPos(x, -y);
    if(penIsDrawing)
    {
        CurScene->addDrawLine(QLineF(start,mPen->pos()), QColor(penColor.r, penColor.g, penColor.b, penColor.a));
    }
    CurView->resetCachedContent();
    CurView->update();
    
    mutex.unlock();
}

/* public slot */ void DrawModule::runMoveBy(const qreal dX, const qreal dY)
{
    /* алг сместиться на вектор(вещ dX, вещ dY) */
    // TODO implement me
    mutex.lock();
    QPointF start=mPen->pos();
    mPen->moveBy(dX, -dY);
    if(penIsDrawing)
        {
            CurScene->addDrawLine(QLineF(start,mPen->pos()), QColor(penColor.r, penColor.g, penColor.b, penColor.a));
        }
    CurView->update();
    mutex.unlock();
}

/* public slot */ void DrawModule::runAddCaption(const qreal width, const QString& text)
{
    /* алг надпись(вещ width, лит text) */
   
    if (width<=0)
    {
        setError(trUtf8("Отрицательная или нулевая ширина надписи!"));
        return;
    }
    qreal offset=CurScene->drawText(text, width, mPen->pos(),QColor(penColor.r, penColor.g, penColor.b, penColor.a));
    mPen->moveBy(offset, 0);
     qDebug()<<"TExt ofset"<<offset;  
}
    
    
bool DrawModule::runIsLineAtCircle(const qreal x, const qreal y, const qreal radius)
    {
        return CurScene->isLineAt(QPointF(x,-y), radius);
    };    
    
void DrawModule::drawNet()
    {
        mutex.lock();
       
        QPointF start_d=CurView->mapToScene(CurView->geometry().topLeft());
        QPointF end_d=CurView->mapToScene(CurView->geometry().bottomRight());
       // QPointF center=CurView->mapToScene(CurView->geometry().center());
       // qDebug()<<"StartDeb"<<start_d<<end_d;
        // QPointF end=CurView->mapToScene(CurView->viewport()->rect().bottomRight().x(),CurView->viewport()->rect().bottomRight().y());

        CurView->setSceneRect(QRectF(QPointF(start_d.x()-(CurView->geometry().width())*(1/zoom()),start_d.y()-(CurView->geometry().height()*2)*(1/zoom())),
                                     QPointF(end_d.x()+2000*(1/zoom()),end_d.y()+2000*(1/zoom()))));
        QPointF start=CurView->sceneRect().topLeft();
        QPointF end=CurView->sceneRect().bottomRight();
        CurScene->drawNet(start.x(),end.x(),start.y(),end.y(), netColor,netStepX,NetStepY(),CurView->isNet());
     
       // CurView->centerOn(center);
        mutex.unlock();
        qDebug()<<"NETSEPX"<<NetStepX();
        navigator->reDraw(zoom(),netStepY,NetStepX());
        CurView->update();
       
    };

    
    void DrawModule::zoomIn()
    {
        CurView->setZoom(CurView->zoom()*2);
        scalePen(Pen()->scale()*(0.5));
        CurView->setNet();
        drawNet();
    };
    void DrawModule::zoomOut()
    {
        CurView->setZoom(CurView->zoom()*0.5);
        scalePen(Pen()->scale()*(2));
        CurView->setNet();
        drawNet(); 
    };
    void DrawModule::zoomNorm()
    {
       
   
        CurView->setZoom(50);
        CurView->setNet();
        mPen->setScale(0.05);
        setNetStepX(1);
        setNetStepY(1);
       
        CurView->centerOn(3,-3);
        CurView->setNet();
        drawNet();
        navigator->updateSelf(1,1);
    };
    
     void DrawModule::zoomFullDraw()
    {
        
        QPointF start_d=CurView->mapToScene(CurView->geometry().topLeft());
        QPointF end_d=CurView->mapToScene(CurView->geometry().bottomRight());
        qreal width=end_d.x()-start_d.x();
        qreal heigth=end_d.y()-start_d.y();
        qreal size=qMax(width,heigth);
        QRectF sceneInfoRect=CurScene->getRect();//Get user lines bounding rect from scene
   
        sceneInfoRect.setY(-sceneInfoRect.y());//Convert to Kumir Coordinates
 
        qDebug()<<"SceneInfoRect:"<<sceneInfoRect<<"L"<<sceneInfoRect.left()<<"R"<<sceneInfoRect.right()<<"t"<<sceneInfoRect.top()<<"B"<<sceneInfoRect.bottom()<<"MaxSize"<<qMax(sceneInfoRect.height(),sceneInfoRect.width());
 
      //  CurView->setSceneRect(QRectF(QPointF(sceneInfoRect.x()-(CurView->geometry().width())*(1/zoom()),sceneInfoRect.y()-(CurView->geometry().height()*2)*(1/zoom())),
       //                              QPointF(sceneInfoRect.x()+1000*(1/zoom()),sceneInfoRect.y()+1000*(1/zoom()))));
        
        QPointF cnt=sceneInfoRect.center();
        cnt.setY(-cnt.y());
        qreal width2=sceneInfoRect.width();
        qreal size2=qMax(sceneInfoRect.height(),sceneInfoRect.width());
        qreal oldZoom=CurView->zoom();
        qreal newZoom=(CurView->zoom()*(size/size2))*0.43;//Some magic
         qDebug()<<"NZ"<<newZoom;
        CurView->setZoom(newZoom/2);
               sceneInfoRect.moveCenter(cnt);//convert to scene coord
        CurView->setSceneRect(sceneInfoRect);
        qDebug()<<"PenScale"<<Pen()->scale()<<"ZoomMP"<<oldZoom/newZoom;
     

               // CurView->centerOn(0,0);
        start_d=CurView->sceneRect().topLeft();
        end_d=CurView->sceneRect().bottomRight();
        QRectF viewRect(start_d,end_d);
       
     
             cnt=sceneInfoRect.center();
            
            qDebug()<<"realCenter2 correction"<<viewRect.center();
            qDebug()<<"Need Center"<<sceneInfoRect.center();
            qDebug()<<"Dist"<<(QVector2D(sceneInfoRect.center())-QVector2D(viewRect.center())).length();
            QVector2D offScene(QVector2D(CurView->mapFromScene(sceneInfoRect.center()))-QVector2D(CurView->mapFromScene(viewRect.center())));
            qDebug()<<"CurCenter"<<CurView->sceneRect().center()<<"Y off scene offset"<<offScene.toPoint().y()<<"Scbar"<<CurView->verticalScrollBar()->value();
           // CurView->verticalScrollBar()->setValue(CurView->verticalScrollBar()->value()+offScene.toPoint().y());
          // CurView->horizontalScrollBar()->setValue(CurView->horizontalScrollBar()->value());
        
 
        CurView->centerOn(sceneInfoRect.center());
        CurView->setZoom(newZoom);
        CurView->setNet();
           drawNet();
        CurView->centerOn(sceneInfoRect.center());
            CurView->update();
            CurView->forceRedraw();
        start_d=CurView->sceneRect().topLeft();
        end_d=CurView->sceneRect().bottomRight();
            viewRect=QRectF(start_d,end_d);
         

      // }
        scalePen(Pen()->scale()*(oldZoom/newZoom));
        navigator->updateSelf(NetStepX(),NetStepY());
        CurView->update();
        CurView->forceRedraw();
    };
    void DrawModule::CreatePen(void)
    {
        
        
        static const int points[] = {  0,0, 6,-3, 18,-15, 15,-18, 3,-6 };
        QPolygon polygon;
        polygon.setPoints(5, points);
        QPolygonF polygonf = QPolygonF(polygon);
        
        mPen = new QGraphicsPolygonItem ( );
        mPen->setPolygon(polygonf);
        mPen->setZValue(100);
        mPen->setBrush(QBrush(QColor("black")));
        CurScene->addItem(mPen);
//        mPen->scale(0.5,0.5);
//        mPen->scale(0.5,0.5);
//        mPen->scale(0.5,0.5);
//        mPen->scale(0.5,0.5);
//        mPen->scale(0.5,0.5);
        mPen->setScale((0.05)*mPen->scale());
        mPen->setZValue(100);
        
        
    }
    
    
    void DrawModule:: autoNetChange(bool value)
    {
        setAutoNet(value);
        getCurView()->setNet();
        drawNet();
    }
    void DrawModule::netStepChange(double value)
    {
        double oldValue=NetStepY();
        setNetStepY(value);
        setNetStepX(value);
        if(oldValue!=value && value>navigator->netStepYS->minimum() )drawNet();
    }

} // namespace ActorDraw
