#ifndef TEXTCURSOR_H
#define TEXTCURSOR_H

#include <QtGui>

namespace Editor {

class TextCursor : public QObject
{
    Q_OBJECT
public:
    enum EditMode { EM_Insert, EM_Overwrite };
    explicit TextCursor(class TextDocument * document);
    ~TextCursor();
    inline int row() const { return i_row; }
    inline int column() const { return i_column; }
    inline EditMode mode() const { return e_mode; }
    inline void setMode(EditMode m) { e_mode = m; emit updateRequest();}
    inline bool isEnabled() const { return b_enabled; }
    inline bool isVisible() const { return b_enabled && b_visible; }
    inline void setEnabled(bool v) { b_enabled = v; emit updateRequest();}
    inline bool hasSelection() const { return p_selectionStart.x()!=-1 && p_selectionEnd.x()!=-1 && p_selectionStart.y()!=-1 && p_selectionEnd.y()!=-1; }
    inline QPoint selectionStart() const { return p_selectionStart; }
    inline QPoint selectionEnd() const { return p_selectionEnd; }
    bool insertText(const QString &text);
    bool removePreviousChar();
    bool removeCurrentChar();
    bool movePosition(QTextCursor::MoveOperation, QTextCursor::MoveMode, int n=1);
    void clearUndoRedoStacks();
    bool isModified() const;
    QString selectedText() const;


signals:
    void updateRequest();
    void updateRequest(int fromLine, int toLine);

protected:
    void timerEvent(QTimerEvent *e);
    class TextDocument * m_document;
    EditMode e_mode;
    int i_timerId;
    bool b_enabled;
    bool b_visible;
    int i_row;
    int i_column;
    QPoint p_selectionStart;
    QPoint p_selectionEnd;

};

} // namespace Editor

#endif // TEXTCURSOR_H
