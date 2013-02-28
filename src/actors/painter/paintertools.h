#ifndef PAINTERTOOLS_H
#define PAINTERTOOLS_H

#include <QtCore>
#include <QtGui>

namespace ActorPainter {

    extern QColor parseColor(const QString &css);
    extern QString CSS_RGB_toString(const QString &cssRgb);

}

#endif // PAINTERTOOLS_H
