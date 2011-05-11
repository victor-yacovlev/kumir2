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
    bool hasSelection() const;
    void insertText(const QString &text);
    void removePreviousChar();
    void removeCurrentChar();
    void removeSelection();
    void removeSelectedText();
    void removeCurrentLine();
    void removeLineTail();
    void movePosition(QTextCursor::MoveOperation, QTextCursor::MoveMode, int n=1);
    void clearUndoRedoStacks();
    bool isModified() const;
    QString selectedText() const;


signals:
    void positionChanged(int row, int col);
    void updateRequest();
    void updateRequest(int fromLine, int toLine);

protected:
    void timerEvent(QTimerEvent *e);
    void emitPositionChanged();
    class TextDocument * m_document;
    EditMode e_mode;
    int i_timerId;
    bool b_enabled;
    bool b_visible;
    int i_row;
    int i_column;
    int i_prevRow;
    int i_prevCol;

};

} // namespace Editor

#endif // TEXTCURSOR_H
