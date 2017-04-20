#ifndef CYRILLIC_MENU_H
#define CYRILLIC_MENU_H
#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#ifdef WIDGETS_LIBRARY
#define WIDGETS_EXPORT Q_DECL_EXPORT
#else
#define WIDGETS_EXPORT Q_DECL_IMPORT
#endif

namespace Widgets {

class WIDGETS_EXPORT CyrillicMenu:
        public QMenu
{
    Q_OBJECT
public:
    CyrillicMenu(QWidget * parent);
    CyrillicMenu(const QString & title, QWidget * parent);
    void updateUbuntuShortcutTexts();
protected:
    void paintEvent(QPaintEvent *event);
private:
    static QString shortcutToText(const QKeySequence & shortcut);
};


}

#endif
