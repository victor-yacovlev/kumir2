#ifndef ACTORPAINTER_SCROLLAREA_H
#define ACTORPAINTER_SCROLLAREA_H

#include <QScrollArea>

namespace ActorPainter {

class ScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit ScrollArea(QWidget *parent = 0);

signals:
    void changeZoomRequest(int steps);

public slots:

private /* methods */:
    void wheelEvent(QWheelEvent *evt);

};

} // namespace ActorPainter

#endif // ACTORPAINTER_SCROLLAREA_H
