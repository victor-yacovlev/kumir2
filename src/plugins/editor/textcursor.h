#ifndef TEXTCURSOR_H
#define TEXTCURSOR_H

#include <QtGui>

#include "interfaces/analizerinterface.h"
#include "keycommand.h"

using namespace Shared;

namespace Editor {

class TextCursor : public QObject
{
    Q_OBJECT
public:
    enum EditMode { EM_Insert, EM_Overwrite };
    enum MoveMode { MM_Move, MM_Select, MM_RectSelect };
    enum ViewMode { VM_Blinking, VM_Hidden, VM_Visible };
    explicit TextCursor(class TextDocument * document, class Clipboard * clipboard, class AnalizerInterface * analizer);

    ~TextCursor();
    inline int row() const { return i_row; }
    inline int column() const { return i_column; }
    inline void setRow(int v) { i_row = v; emit updateRequest(); }
    inline void setColumn(int v) { i_column = v; emit updateRequest(); }
    inline void setTeacherMode(bool v) { b_teacher = v; }
    void moveTo(int row, int col);
    void selectRangeBlock(int fromRow, int fromCol, int toRow, int toCol);
    void selectRangeText(int fromRow, int fromCol, int toRow, int toCol);
    inline EditMode mode() const { return e_mode; }
    inline void setMode(EditMode m) { e_mode = m; emit updateRequest();}
    inline bool isEnabled() const { return b_enabled; }
    inline bool isVisible() const { return b_enabled && b_visible; }
    void setViewMode(ViewMode mode);
    void setEnabled(bool v);
    bool hasSelection() const;
    inline bool hasRectSelection() const { return rect_selection.x()!=-1 && rect_selection.y()!=-1; }
    inline QRect selectionRect() const { return rect_selection; }
    void selectionBounds(int &fromRow, int &fromCol, int &toRow, int &toCol) const;
    QStringList rectSelectionText() const;
    void removeRectSelection();
    void insertText(const QString &text);
    void insertBlock(const QStringList & block);
    void removePreviousChar();
    void removeCurrentChar();
    void removeSelection();
    void removeSelectedText();
    void removeCurrentLine();
    void removeLineTail();
    void movePosition(QTextCursor::MoveOperation, MoveMode, int n=1);

    QString selectedText() const;
    void removeSelectedBlock();
    void evaluateCommand(const KeyCommand & command);
public slots:
    void toggleComment();
    void undo();
    void redo();
    void handleUndoChanged(bool v);
    void handleRedoChanged(bool v);


signals:
    void positionChanged(int row, int col);
    void updateRequest();
    void updateRequest(int fromLine, int toLine);
    void undoAvailable(bool);
    void redoAvailable(bool);
    void signalizeNotEditable();


protected:

    int justifyLeft(const QString & text) const;
    bool modifiesProtectedLiines() const;

    void findLexemBound(int & row,  int & column, const qint8 dir) const;

    void timerEvent(QTimerEvent *e);
    void emitPositionChanged();
    class TextDocument * m_document;
    class Clipboard * m_clipboard;
    class AnalizerInterface * m_analizer;
    EditMode e_mode;
    ViewMode e_viewMode;
    int i_timerId;
    bool b_enabled;
    bool b_visible;
    int i_row;
    int i_column;
    bool b_teacher;

    QRect rect_selection;

};

} // namespace Editor

#endif // TEXTCURSOR_H
