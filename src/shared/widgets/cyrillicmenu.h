#ifndef CYRILLIC_MENU_H
#define CYRILLIC_MENU_H
#include <QtCore>
#include <QtGui>

#ifdef WIDGETS_LIBRARY
#define WIDGETS_EXPORT Q_DECL_EXPORT
#else
#define WIDGETS_EXPORT Q_DECL_IMPORT
#endif

namespace Widgets {

class CyrillicMenu:
        public QMenu
{
    Q_OBJECT
public:
    CyrillicMenu(QWidget * parent);
    CyrillicMenu(const QString & title, QWidget * parent);
protected:
    void paintEvent(QPaintEvent *event);
private:
    static QString shortcutToText(const QKeySequence & shortcut);
};


}

#endif
