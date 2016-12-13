#ifndef GRAPHICSIMAGEITEM_H
#define GRAPHICSIMAGEITEM_H

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

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
    QImage _image;
    QMutex *_mutex_image;
};

}

#endif // GRAPHICSIMAGEITEM_H
