#include "graphicsimageitem.h"


namespace Robot25D {
GraphicsImageItem::GraphicsImageItem()
{
    _mutex_image = new QMutex;
}

QRectF GraphicsImageItem::boundingRect() const
{
    _mutex_image->lock();
    QRectF result;
    if (!_image.isNull())
        result = QRectF(QPointF(0.0, 0.0), _image.size()).adjusted(-0.5, -0.5, 0.5, 0.5);
    _mutex_image->unlock();
    return result;
}

void GraphicsImageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    _mutex_image->lock();
//    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
//    bool antialiasing = scene()->views()[0]->renderHints().testFlag(QPainter::Antialiasing);
    painter->drawImage(0,0,_image);
//    painter->setPen(QColor("blue"));
//    painter->drawRect(0,0,m_image.width(),m_image.height());
    _mutex_image->unlock();
}

QImage GraphicsImageItem::image() const
{
    _mutex_image->lock();
    QImage result = _image;
    _mutex_image->unlock();
    return result;
}

void GraphicsImageItem::setImage(const QImage &image)
{
    _mutex_image->lock();
    _image = image;
    _mutex_image->unlock();
//    update();
}

GraphicsImageItem::~GraphicsImageItem()
{
    delete _mutex_image;
}

}
