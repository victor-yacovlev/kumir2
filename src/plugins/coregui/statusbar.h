#ifndef COREGUI_STATUSBAR_H
#define COREGUI_STATUSBAR_H

#include "extensionsystem/kplugin.h"
#include "mainwindow.h"

#include <QStatusBar>
#include <QToolButton>
#include <QList>

namespace CoreGUI {

class StatusBar : public QStatusBar
{
    Q_OBJECT
public:

    enum MessageRole { Normal, Error };

    explicit StatusBar(QWidget *parent = 0);    
    QSize minimumSizeHint() const;
    void addButtonToLeft(QToolButton * btn);
signals:
    
public slots:
    void setState(ExtensionSystem::GlobalState state);
    void setMessage(const QString & message, MessageRole role = Normal);
    void unsetMessage();
    void setStepsDoneCounter(uint value);
    void setErrorsCounter(uint value);
    void setEditorCursorPosition(uint row, uint column);
    void setEditorKeyboardLayout(QLocale::Language lang, bool capslock, bool shift, bool alt);
    void setRecordIndicator(bool on);
    void setDocumentType(MainWindow::DocumentType type);

private:
    int fontHeight() const;
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void paintModeItem(QPainter &p, int x);
    void paintCounterItem(QPainter &p, int x);
    void paintMessageItem(QPainter &p, int x);
    void paintCursorItem(QPainter &p, int x);
    void paintKeyboardItem(QPainter &p, int x);
    void paintItemRect(QPainter &p, const QSize & sz, int x);
    static QImage makeIndicatorIcon(const QString & name, const QColor & color);
    QColor normalColor() const;
    QColor alternateColor() const;
    QFont statusBarFont() const;
    QFontMetrics statusBarFontMetrics() const;
    QSize keyboardLayoutItemSize() const;
    QSize cursorPositionItemSize() const;
    QSize modeItemSize() const;
    QSize counterItemSize() const;
    QSize messageItemSize() const;
    int messageWidth_;
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
    bool editorRecord_;
    QList<QToolButton*> toolButtons_;
    MainWindow::DocumentType documentType_;
};

} // namespace CoreGUI

#endif // COREGUI_STATUSBAR_H
