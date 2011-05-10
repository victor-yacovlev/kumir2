#ifndef EDITORPLANE_H
#define EDITORPLANE_H

#include <QtGui>

#include "interfaces/lexemtype.h"

namespace Editor {

class EditorPlane : public QWidget
{
    Q_OBJECT
public:
    explicit EditorPlane(class TextDocument * doc
                         , class TextCursor * cursor
                         , class Clipboard * clipboard
                         , QWidget *parent = 0);
    int widthInChars() const;
    int charWidth() const;
    int lineHeight() const;
    QRect cursorRect() const;
protected:
    void paintEvent(QPaintEvent *);
    void paintBackground(QPainter *p, const QRect &rect);
    void paintCursor(QPainter *p, const QRect &rect);
    void paintText(QPainter *p, const QRect &rect);
    void paintSelection(QPainter *p, const QRect &rect);
    void keyPressEvent(QKeyEvent *);
    void setProperFormat(QPainter * p, Shared::LexemType type, const QChar &c);
    QRegion selectionRegion() const;
protected slots:
    void updateCursor();
    void updateText(int fromLine, int toLine);

private:
    int i_marginWidth;
    int i_timerId;
    class TextDocument * m_document;
    class TextCursor * m_cursor;
    class Clipboard * m_clipboard;


};

} // namespace Editor

#endif // EDITORPLANE_H
