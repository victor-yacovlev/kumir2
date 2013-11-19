#ifndef GRAPHICSIMAGEITEM_H
#define GRAPHICSIMAGEITEM_H

#include <QtCore>
#include <QtGui>

namespace Robot25D {

class GraphicsImageItem : public QGraphicsItem
{
public:
    GraphicsImageItem();
    QImage image() const;
    void setImage(const QImage &image);
    QRectF boundingRect() const;
    ~GraphicsImageItem();
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    QImage m_image;
    QMutex *mutex_image;
};

}

#endif // GRAPHICSIMAGEITEM_H
