#include "utils.h"

#include <QtCore>
#include <QtGui>

#ifdef Q_OS_WIN32
#   include <windows.h>
#endif

#ifdef Q_WS_X11
#   include <X11/XKBlib.h>
#   include <QX11Info>
#endif


namespace Editor {

bool Utils::isRussianLayout()
{
    bool result = false;
#ifdef Q_WS_X11
    QString keybLang = QApplication::keyboardInputLocale().name();
    result = keybLang.contains("ru");
#endif
#ifdef Q_OS_WIN32
    HKL l = GetKeyboardLayout(0);
    result = unsigned(l)==0x4190419;
#endif
    return result;
}

bool Utils::isCapsLock()
{
    bool result = false;
#ifdef Q_WS_X11
    Display * d = QX11Info::display();
    if (d) {
        unsigned n;
        XkbGetIndicatorState(d, XkbUseCoreKbd, &n);
        result = (n & 0x01) == 1;
    }
#endif
#ifdef Q_OS_WIN32
    result = GetKeyState(VK_CAPITAL)==1;
#endif
    return result;
}


} // namespace Editor
