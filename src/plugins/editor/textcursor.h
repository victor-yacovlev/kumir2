#ifndef TEXTCURSOR_H
#define TEXTCURSOR_H


#include "interfaces/analizerinterface.h"
#include "keycommand.h"
#include "macro.h"
#include "extensionsystem/settings.h"

#include <memory>
#include <QtGui>


using namespace Shared;

namespace Editor {

class TextCursor : public QObject
{
    Q_OBJECT
public:
    enum EditMode { EM_Insert, EM_Overwrite };
    enum MoveMode { MM_Move, MM_Select, MM_RectSelect };
    enum ViewMode { VM_Blinking, VM_Hidden, VM_Visible };
    explicit TextCursor(class Editor * editor);

    ~TextCursor();
    inline uint row() const { return row_; }
    inline uint column() const { return column_; }
    inline void setRow(uint v) { row_ = v; emit updateRequest(); }
    inline void setColumn(uint v) { column_ = v; emit updateRequest(); }
    inline void setTeacherMode(bool v) { teacherModeFlag_ = v; }
    void moveTo(int row, int col);
    void selectRangeBlock(int fromRow, int fromCol, int toRow, int toCol);
    void selectRangeText(int fromRow, int fromCol, int toRow, int toCol);
    inline EditMode mode() const { return editMode_; }
    inline void setMode(EditMode m) { editMode_ = m; emit updateRequest();}
    inline bool isEnabled() const { return enabledFlag_; }
    inline bool isVisible() const { return enabledFlag_ && visibleFlag_; }
    void setViewMode(ViewMode mode);
    void setEnabled(bool v);
    bool hasSelection() const;
    inline bool hardIndents() const { return hardIndents_; }
    inline uint indentSize() const { return indentSize_; }
    inline bool hasRectSelection() const { return selectionRect_.x()!=-1 && selectionRect_.y()!=-1; }
    inline QRect selectionRect() const { return selectionRect_; }
    void selectionBounds(int &fromRow, int &fromCol, int &toRow, int &toCol) const;
    QStringList rectSelectionText() const;
    void removeRectSelection();
    void insertText(const QString &text);
    void insertImport(const QString &importableName);
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

    bool isFreeCursorMovement() const;
    bool modifiesProtectedLiines() const;

public slots:
    void toggleComment();
    void undo();
    void redo();
    void handleUndoChanged(bool v);
    void handleRedoChanged(bool v);
    void startRecordMacro();
    Macro* endRecordMacro();


signals:
    void positionChanged(int row, int col);
    void updateRequest();
    void updateRequest(int fromLine, int toLine);
    void undoAvailable(bool);
    void redoAvailable(bool);
    void signalizeNotEditable();


protected:

    class Editor * editor_;

    int justifyLeft(const QString & text) const;    

    void findLexemBound(uint & row,  uint & column, const qint8 dir) const;

    void timerEvent(QTimerEvent *e);
    void emitPositionChanged();
    EditMode editMode_;
    ViewMode viewMode_;
    int blinkTimerId_;
    bool enabledFlag_;
    bool visibleFlag_;
    uint row_;
    uint column_;
    int keptColumn_;
    bool teacherModeFlag_;

    QRect selectionRect_;
    Macro * recordingMacro_;
    unsigned int indentSize_;
    bool hardIndents_;
    bool denyAutoCloseBraces_;

};

} // namespace Editor

#endif // TEXTCURSOR_H
