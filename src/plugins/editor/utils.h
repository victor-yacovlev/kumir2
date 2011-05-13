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
    static bool temporaryLayoutSwitch;
    static QString textByKey(Qt::Key key, const QString & keyText, bool shiftPressed);
};

} // namespace Editor

#endif // UTILS_H
