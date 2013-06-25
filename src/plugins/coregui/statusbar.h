#ifndef COREGUI_STATUSBAR_H
#define COREGUI_STATUSBAR_H

#include "extensionsystem/kplugin.h"

#include <QStatusBar>

namespace CoreGUI {

class StatusBar : public QStatusBar
{
    Q_OBJECT
public:

    enum MessageRole { Normal, Error };

    explicit StatusBar(QWidget *parent = 0);    
    QSize minimumSizeHint() const;
signals:
    
public slots:
    void setState(ExtensionSystem::GlobalState state);
    void setMessage(const QString & message, MessageRole role = Normal);
    void unsetMessage();
    void setStepsDoneCounter(uint value);
    void setErrorsCounter(uint value);
    void setEditorCursorPosition(uint row, uint column);
    void setEditorKeyboardLayout(QLocale::Language lang, bool capslock, bool shift, bool alt);

private:
    void paintEvent(QPaintEvent *event);
    void paintModeItem(QPainter &p, int x);
    void paintCounterItem(QPainter &p, int x);
    void paintMessageItem(QPainter &p, int x);
    void paintCursorItem(QPainter &p, int x);
    void paintKeyboardItem(QPainter &p, int x);
    void paintItemRect(QPainter &p, const QSize & sz, int x);
    static QImage makeIndicatorIcon(const QString & name, const QColor & color);
    QColor normalColor() const;
    QColor errorColor() const;
    QFont statusBarFont() const;
    QFontMetrics statusBarFontMetrics() const;
    QSize keyboardLayoutItemSize() const;
    QSize cursorPositionItemSize() const;
    QSize modeItemSize() const;
    QSize counterItemSize() const;
    QSize messageItemSize() const;
    ExtensionSystem::GlobalState state_;
    uint errorsCount_;
    uint stepsDone_;
    QString message_;
    MessageRole messageRole_;
    uint editorRow_;
    uint editorColumn_;
    QLocale::Language keyboardLayout_;
    bool keyboardCaps_;
    bool keyboardShift_;
    bool keyboardAlt_;
};

} // namespace CoreGUI

#endif // COREGUI_STATUSBAR_H
