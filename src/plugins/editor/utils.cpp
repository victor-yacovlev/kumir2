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

bool Utils::temporaryLayoutSwitch = false;

QString Utils::textByKey(Qt::Key key, const QString & keyText, bool shiftPressed)
{
    Q_UNUSED(key);
    Q_UNUSED(shiftPressed);
    QString result = keyText;
    if (temporaryLayoutSwitch && key==Qt::Key_1) {
        result = "|";
    }
    else if (temporaryLayoutSwitch && key==Qt::Key_Equal) {
        result = ":=";
    }
    else if (temporaryLayoutSwitch && isRussianLayout()) {
        if (keyText.toLower()==QString::fromUtf8("й"))
            result = "q";
        else if (keyText.toLower()==QString::fromUtf8("ц"))
            result = "w";
        else if (keyText.toLower()==QString::fromUtf8("у"))
            result = "e";
        else if (keyText.toLower()==QString::fromUtf8("к"))
            result = "r";
        else if (keyText.toLower()==QString::fromUtf8("е"))
            result = "t";
        else if (keyText.toLower()==QString::fromUtf8("н"))
            result = "y";
        else if (keyText.toLower()==QString::fromUtf8("г"))
            result = "u";
        else if (keyText.toLower()==QString::fromUtf8("ш"))
            result = "i";
        else if (keyText.toLower()==QString::fromUtf8("щ"))
            result = "o";
        else if (keyText.toLower()==QString::fromUtf8("з"))
            result = "p";
        else if (keyText.toLower()==QString::fromUtf8("х"))
            result = shiftPressed? "{" : "[";
        else if (keyText.toLower()==QString::fromUtf8("ъ"))
            result = shiftPressed? "}" : "]";

        else if (keyText.toLower()==QString::fromUtf8("ф"))
            result = "a";
        else if (keyText.toLower()==QString::fromUtf8("ы"))
            result = "s";
        else if (keyText.toLower()==QString::fromUtf8("в"))
            result = "d";
        else if (keyText.toLower()==QString::fromUtf8("а"))
            result = "f";
        else if (keyText.toLower()==QString::fromUtf8("п"))
            result = "g";
        else if (keyText.toLower()==QString::fromUtf8("р"))
            result = "h";
        else if (keyText.toLower()==QString::fromUtf8("о"))
            result = "j";
        else if (keyText.toLower()==QString::fromUtf8("л"))
            result = "k";
        else if (keyText.toLower()==QString::fromUtf8("д"))
            result = "l";
        else if (keyText.toLower()==QString::fromUtf8("ж"))
            result = shiftPressed? ":" : ";";
        else if (keyText.toLower()==QString::fromUtf8("э"))
            result = shiftPressed? "\"" : "'";

        else if (keyText.toLower()==QString::fromUtf8("я"))
            result = "z";
        else if (keyText.toLower()==QString::fromUtf8("ч"))
            result = "x";
        else if (keyText.toLower()==QString::fromUtf8("с"))
            result = "c";
        else if (keyText.toLower()==QString::fromUtf8("м"))
            result = "v";
        else if (keyText.toLower()==QString::fromUtf8("и"))
            result = "b";
        else if (keyText.toLower()==QString::fromUtf8("т"))
            result = "n";
        else if (keyText.toLower()==QString::fromUtf8("ь"))
            result = "m";
        else if (keyText.toLower()==QString::fromUtf8("б"))
            result = shiftPressed? "<" : ",";
        else if (keyText.toLower()==QString::fromUtf8("ю"))
            result = shiftPressed? ">" : ".";
        else if (keyText.toLower()==QString::fromUtf8("ё"))
            result = shiftPressed? "~" : "`";
        else if (keyText=="." || keyText==",")
            result = shiftPressed? "?" : "/";

        if ( (shiftPressed && !isCapsLock()) || (!shiftPressed && isCapsLock()) )
            result = result.toUpper();
    }
    else if (temporaryLayoutSwitch && !isRussianLayout()) {
        if (key==Qt::Key_Q)
            result = QString::fromUtf8("й");
        else if (key==Qt::Key_W)
            result = QString::fromUtf8("ц");
        else if (key==Qt::Key_E)
            result = QString::fromUtf8("у");
        else if (key==Qt::Key_R)
            result = QString::fromUtf8("к");
        else if (key==Qt::Key_T)
            result = QString::fromUtf8("е");
        else if (key==Qt::Key_Y)
            result = QString::fromUtf8("н");
        else if (key==Qt::Key_U)
            result = QString::fromUtf8("г");
        else if (key==Qt::Key_I)
            result = QString::fromUtf8("ш");
        else if (key==Qt::Key_O)
            result = QString::fromUtf8("щ");
        else if (key==Qt::Key_P)
            result = QString::fromUtf8("з");
        else if (key==Qt::Key_BraceLeft || key==Qt::Key_BracketLeft)
            result = QString::fromUtf8("х");
        else if (key==Qt::Key_BraceRight || key==Qt::Key_BracketRight)
            result = QString::fromUtf8("ъ");
        else if (key==Qt::Key_A)
            result = QString::fromUtf8("ф");
        else if (key==Qt::Key_S)
            result = QString::fromUtf8("ы");
        else if (key==Qt::Key_D)
            result = QString::fromUtf8("в");
        else if (key==Qt::Key_F)
            result = QString::fromUtf8("а");
        else if (key==Qt::Key_G)
            result = QString::fromUtf8("п");
        else if (key==Qt::Key_H)
            result = QString::fromUtf8("р");
        else if (key==Qt::Key_J)
            result = QString::fromUtf8("о");
        else if (key==Qt::Key_K)
            result = QString::fromUtf8("л");
        else if (key==Qt::Key_L)
            result = QString::fromUtf8("д");
        else if (key==Qt::Key_Semicolon || key==Qt::Key_Colon)
            result = QString::fromUtf8("ж");
        else if (key==Qt::Key_QuoteDbl || key==Qt::Key_Apostrophe)
            result = QString::fromUtf8("э");
        else if (key==Qt::Key_Z)
            result = QString::fromUtf8("я");
        else if (key==Qt::Key_X)
            result = QString::fromUtf8("ч");
        else if (key==Qt::Key_C)
            result = QString::fromUtf8("с");
        else if (key==Qt::Key_V)
            result = QString::fromUtf8("м");
        else if (key==Qt::Key_B)
            result = QString::fromUtf8("и");
        else if (key==Qt::Key_N)
            result = QString::fromUtf8("т");
        else if (key==Qt::Key_M)
            result = QString::fromUtf8("ь");
        else if (key==Qt::Key_Comma || key==Qt::Key_Less)
            result = QString::fromUtf8("б");
        else if (key==Qt::Key_Period || key==Qt::Key_Greater)
            result = QString::fromUtf8("ю");
        else if (key==Qt::Key_QuoteLeft || key==Qt::Key_AsciiTilde)
            result = QString::fromUtf8("ё");
        else if (key==Qt::Key_Slash || key==Qt::Key_Question)
            result = shiftPressed? "," : ".";

        if ( (shiftPressed && !isCapsLock()) || (!shiftPressed && isCapsLock()) )
            result = result.toUpper();

    }
    return result;
}

QChar Utils::latinKey(const QChar &cyrillicKey)
{
    if (cyrillicKey.toLower()==QString::fromUtf8("й")[0]) {
        return 'q';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("ц")[0]) {
        return 'w';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("у")[0]) {
        return 'e';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("к")[0]) {
        return 'r';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("е")[0]) {
        return 't';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("н")[0]) {
        return 'y';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("г")[0]) {
        return 'u';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("ш")[0]) {
        return 'i';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("щ")[0]) {
        return 'o';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("з")[0]) {
        return 'p';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("х")[0]) {
        return '[';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("ъ")[0]) {
        return ']';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("ф")[0]) {
        return 'a';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("ы")[0]) {
        return 's';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("в")[0]) {
        return 'd';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("а")[0]) {
        return 'f';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("п")[0]) {
        return 'g';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("р")[0]) {
        return 'h';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("о")[0]) {
        return 'j';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("л")[0]) {
        return 'k';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("д")[0]) {
        return 'l';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("ж")[0]) {
        return ';';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("э")[0]) {
        return '\'';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("я")[0]) {
        return 'z';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("ч")[0]) {
        return 'x';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("с")[0]) {
        return 'c';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("м")[0]) {
        return 'v';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("и")[0]) {
        return 'b';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("т")[0]) {
        return 'n';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("ь")[0]) {
        return 'm';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("б")[0]) {
        return ',';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("ю")[0]) {
        return '.';
    }
    else if (cyrillicKey.toLower()==QString::fromUtf8("ё")[0]) {
        return '`';
    }
    else {
        return cyrillicKey.toLower();
    }
}

} // namespace Editor
