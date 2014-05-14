#include "scrollarea.h"

#include <QWheelEvent>
#include <QScrollBar>

namespace ActorPainter {

ScrollArea::ScrollArea(QWidget *parent) :
    QScrollArea(parent)
{
}

void ScrollArea::wheelEvent(QWheelEvent *evt)
{
    const int delta = evt->delta();
    const int degrees = delta / 8;
    const int steps = -1 * degrees / 15;
    if (! (evt->modifiers() & Qt::ControlModifier) ) {
        // Scroll contents
        QAbstractSlider* slider = evt->orientation() == Qt::Vertical
                ? verticalScrollBar()
                : horizontalScrollBar();
        if (evt->orientation() == Qt::Vertical && evt->modifiers() & Qt::ShiftModifier) {
            slider = horizontalScrollBar();
        }
        if (slider->isVisible()) {
            int value = slider->value() + steps * slider->singleStep();
            value = qMin(value, slider->maximum());
            slider->setValue(value);
            evt->accept();
        }
        else {
            evt->ignore();
        }
    }
    else {
        // Zoom
        emit changeZoomRequest(steps);
        evt->accept();
    }
}

} // namespace ActorPainter
