#ifndef PAINTERTOOLS_H
#define PAINTERTOOLS_H

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "../_colorer/typecolor.h"

namespace ActorPainter {

    extern QColor parseColor(const QString &css);
    extern QString CSS_RGB_toString(const QString &cssRgb);
    extern QString findColorName(const Actor_Colorer::Color & color);
    extern const QStringList & standardRussianColorNames();

}

#endif // PAINTERTOOLS_H
