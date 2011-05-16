#ifndef TEXTCURSOR_H
#define TEXTCURSOR_H

#include <QtGui>

namespace Editor {

class TextCursor : public QObject
{
    Q_OBJECT
public:
    enum EditMode { EM_Insert, EM_Overwrite };
    enum MoveMode { MM_Move, MM_Select, MM_RectSelect };
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
    inline bool hasRectSelection() const { return rect_selection.x()!=-1 && rect_selection.y()!=-1; }
    inline QRect selectionRect() const { return rect_selection; }
    QStringList rectSelectionText() const;
    void clearSelectedBlock();
    void insertText(const QString &text);
    void insertBlock(const QStringList & block);
    void removePreviousChar();
    void removeCurrentChar();
    void removeSelection();
    void removeSelectedText();
    void removeCurrentLine();
    void removeLineTail();
    void movePosition(QTextCursor::MoveOperation, MoveMode, int n=1);
    void clearUndoRedoStacks();
    bool isModified() const;
    QString selectedText() const;


signals:
    void positionChanged(int row, int col);
    void lineAndTextChanged(const QList<int> &, const QList<int> & );
    void updateRequest();
    void updateRequest(int fromLine, int toLine);


protected:

    void removeSelectedBlock();
    void timerEvent(QTimerEvent *e);
    void emitPositionChanged();
    class TextDocument * m_document;
    QList <int> l_removedLines;
    QList <int> l_newLines;
    EditMode e_mode;
    int i_timerId;
    bool b_enabled;
    bool b_visible;
    int i_row;
    int i_column;
    int i_prevRow;
    int i_prevCol;
    QRect rect_selection;

};

} // namespace Editor

#endif // TEXTCURSOR_H
