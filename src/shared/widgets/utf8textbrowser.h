#ifndef WIDGETS_UTF8TEXTBROWSER_H
#define WIDGETS_UTF8TEXTBROWSER_H

#include <QTextBrowser>

namespace Widgets {

class Utf8TextBrowser : public QTextBrowser
{
public:
    inline Utf8TextBrowser(QWidget* parent = 0) : QTextBrowser(parent) {}
    virtual QVariant loadResource(int type, const QUrl &name);
};

} // namespace Widgets

#endif // WIDGETS_UTF8TEXTBROWSER_H
