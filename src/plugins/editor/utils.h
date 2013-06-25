#ifndef UTILS_H
#define UTILS_H

#include <QtCore>
#include <QtGui>

namespace Editor {

class Utils
{
public:
    static bool isRussianLayout();
    static bool isCapsLock();
    static bool altKeyPressed;
    static bool shiftKeyPressed;
    static QString textByKey(Qt::Key key,
                             const QString & keyText,
                             bool shiftPressed,
                             bool teacherMode
                             );
    static QChar latinKey(const QChar & cyrillicKey);
};

} // namespace Editor

#endif // UTILS_H
