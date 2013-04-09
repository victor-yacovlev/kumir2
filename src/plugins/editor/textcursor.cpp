#include "textcursor.h"
#include "textdocument.h"
#include "clipboard.h"
#include "editcommands.h"
#include "extensionsystem/pluginmanager.h"
#include "settingspage.h"

namespace Editor {

TextCursor::TextCursor(TextDocument * document, Clipboard * clipboard, AnalizerInterface * analizer)
    : QObject(0)
    , document_(document)
    , clipboard_(clipboard)
    , analizer_(analizer)
    , editMode_(EM_Insert)
    , viewMode_(VM_Hidden)
    , blinkTimerId_(-1)
    , enabledFlag_(true)
    , visibleFlag_(false)
    , row_(0)
    , column_(0)

{
    teacherModeFlag_ = false;
    blinkTimerId_ = startTimer(QApplication::cursorFlashTime()/2);
    emitPositionChanged();

    selectionRect_ = QRect(-1,-1,0,0);
}

void TextCursor::setViewMode(ViewMode mode)
{
    viewMode_ = mode;
    if (mode==VM_Hidden)
        visibleFlag_ = false;
    else if (mode==VM_Visible)
        visibleFlag_ = true;
    emit updateRequest();
}



void TextCursor::emitPositionChanged()
{

    emit positionChanged(row_, column_);

}

void TextCursor::timerEvent(QTimerEvent *e)
{
    if (viewMode_==VM_Blinking) {
        visibleFlag_ = !visibleFlag_;
    }
    else if (viewMode_==VM_Hidden) {
        visibleFlag_ = false;
    }
    else if (viewMode_==VM_Visible) {
        visibleFlag_ = true;
    }
    emit updateRequest();
    e->accept();

}

TextCursor::~TextCursor()
{
    if (blinkTimerId_!=-1)
        killTimer(blinkTimerId_);
}

void TextCursor::toggleComment()
{
    if (!enabledFlag_)
        return;
    if (analizer_==0)
        return;
    if (hasSelection()) {
        int start = -1;
        int end = -1;
        for (int i=0; i<document_->linesCount(); i++) {
            if (document_->lineEndSelectedAt(i) || document_->selectionMaskAt(i).contains(true)) {
                if (start==-1) {
                    start = i;
                }
                end = i;
            }
        }
        document_->undoStack()->push(new ToggleCommentCommand(
                                          document_,
                                          start,
                                          end,
                                          this,
                                          analizer_
                                          ));
    }
    else if (hasRectSelection()) {
        int start = selectionRect_.top();
        int end = selectionRect_.bottom();
        document_->undoStack()->push(new ToggleCommentCommand(
                                          document_,
                                          start,
                                          end,
                                          this,
                                          analizer_
                                          ));
    }
    else {
        if (row_ < document_->linesCount()) {
            document_->undoStack()->push(new ToggleCommentCommand(
                                              document_,
                                              row_,
                                              row_,
                                              this,
                                              analizer_
                                              ));
        }
    }
}

void TextCursor::evaluateCommand(const KeyCommand &command)
{
    int prevRow = row_;
    int prevLines = document_->linesCount();
    bool clearCurrentLineError = false;
    switch (command.type) {
    case KeyCommand::MoveToNextChar:
        movePosition(QTextCursor::NextCell, TextCursor::MM_Move);
        break;
    case KeyCommand::MoveToNextLexem:
        movePosition(QTextCursor::NextWord, TextCursor::MM_Move);
        break;
    case KeyCommand::SelectNextLexem:
        movePosition(QTextCursor::NextWord, TextCursor::MM_Select);
        break;
    case KeyCommand::MoveToPreviousLexem:
        movePosition(QTextCursor::PreviousWord, TextCursor::MM_Move);
        break;
    case KeyCommand::SelectPreviousLexem:
        movePosition(QTextCursor::PreviousWord, TextCursor::MM_Select);
        break;
    case KeyCommand::SelectNextChar:
        movePosition(QTextCursor::NextCell, TextCursor::MM_Select);
        break;
    case KeyCommand::SelectNextColumn:
        movePosition(QTextCursor::NextCell, TextCursor::MM_RectSelect);
        break;
    case KeyCommand::MoveToPreviousChar:
        movePosition(QTextCursor::PreviousCell, TextCursor::MM_Move);
        break;
    case KeyCommand::SelectPreviousChar:
        movePosition(QTextCursor::PreviousCell, TextCursor::MM_Select);
        break;
    case KeyCommand::SelectPreviousColumn:
        movePosition(QTextCursor::PreviousCell, TextCursor::MM_RectSelect);
        break;
    case KeyCommand::MoveToNextLine:
        movePosition(QTextCursor::NextRow, TextCursor::MM_Move);
        break;
    case KeyCommand::SelectNextLine:
        movePosition(QTextCursor::NextRow, TextCursor::MM_Select);
        break;
    case KeyCommand::SelectNextRow:
        movePosition(QTextCursor::NextRow, TextCursor::MM_RectSelect);
        break;
    case KeyCommand::MoveToPreviousLine:
        movePosition(QTextCursor::PreviousRow, TextCursor::MM_Move);
        break;
    case KeyCommand::SelectPreviousLine:
        movePosition(QTextCursor::PreviousRow, TextCursor::MM_Select);
        break;
    case KeyCommand::SelectPreviousRow:
        movePosition(QTextCursor::PreviousRow, TextCursor::MM_RectSelect);
        break;
    case KeyCommand::MoveToStartOfLine:
        movePosition(QTextCursor::StartOfBlock, TextCursor::MM_Move);
        break;
    case KeyCommand::SelectStartOfLine:
        movePosition(QTextCursor::StartOfBlock, TextCursor::MM_Select);
        break;
    case KeyCommand::MoveToEndOfLine:
        movePosition(QTextCursor::EndOfBlock, TextCursor::MM_Move);
        break;
    case KeyCommand::SelectEndOfLine:
        movePosition(QTextCursor::EndOfBlock, TextCursor::MM_Select);
        break;
    case KeyCommand::MoveToPreviousPage:
        movePosition(QTextCursor::PreviousRow, TextCursor::MM_Move, 10);
        break;
    case KeyCommand::SelectPreviousPage:
        movePosition(QTextCursor::PreviousRow, TextCursor::MM_Select, 10);
        break;
    case KeyCommand::MoveToNextPage:
        movePosition(QTextCursor::NextRow, TextCursor::MM_Move, 10);
        break;
    case KeyCommand::SelectNextPage:
        movePosition(QTextCursor::NextRow, TextCursor::MM_Select, 10);
        break;
    case KeyCommand::MoveToStartOfDocument:
        movePosition(QTextCursor::Start, TextCursor::MM_Move);
        break;
    case KeyCommand::SelectStartOfDocument:
        movePosition(QTextCursor::Start, TextCursor::MM_Select);
        break;
    case KeyCommand::MoveToEndOfDocument:
        movePosition(QTextCursor::End, TextCursor::MM_Move);
        break;
    case KeyCommand::SelectEndOfDocument:
        movePosition(QTextCursor::End, TextCursor::MM_Select);
        break;
    case KeyCommand::SelectAll:
        movePosition(QTextCursor::Start, TextCursor::MM_Move);
        movePosition(QTextCursor::End, TextCursor::MM_Select);
        break;
    case KeyCommand::Copy:
        if (hasSelection()) {
            ClipboardData data;
            data.text = selectedText();
            data.type = ClipboardData::Text;
            data.html = document_->toHtml(-1, -1);
            clipboard_->push(data);
        }
        else if (hasRectSelection()) {
            ClipboardData data;
            data.text = selectedText();
            data.type = ClipboardData::Block;
            data.block = rectSelectionText();
            data.html = document_->toHtml(selectionRect_.top(), selectionRect_.bottom());
            clipboard_->push(data);
        }
        break;

    case KeyCommand::InsertText:
        insertText(command.text);
        clearCurrentLineError = !command.text.contains("\n");
        break;
    case KeyCommand::Backspace:
        clearCurrentLineError = true;
        removePreviousChar();
        break;
    case KeyCommand::Delete:
        clearCurrentLineError = column_>=2*document_->indentAt(row_) || hasSelection() || hasRectSelection();
        removeCurrentChar();
        break;
    case KeyCommand::RemoveLine:
        removeCurrentLine();
        break;
    case KeyCommand::RemoveTail:
        clearCurrentLineError = true;
        removeLineTail();
        break;
    case KeyCommand::Cut:
        evaluateCommand(KeyCommand::Copy);
        if (hasSelection()) {
            clearCurrentLineError = !selectedText().contains("\n");
            removeSelectedText();
        }
        else if (hasRectSelection()) {
            removeRectSelection();
        }
        break;
    case KeyCommand::Paste:
        if (clipboard_->hasContent()) {
            ClipboardData data = clipboard_->content();
            if (data.type == ClipboardData::Block) {
                insertBlock(data.block);
            }
            else if (data.type == ClipboardData::Text) {
                insertText(data.text);
                clearCurrentLineError = !data.text.contains("\n");
            }
        }
        break;
    case KeyCommand::ToggleComment:
        toggleComment();
        clearCurrentLineError = true;
        break;
    default:
        break;
    }
    if (clearCurrentLineError && row_<document_->linesCount()) {
        document_->setErrorsAt(row_, QStringList());
    }

    if (prevRow!=row_ || prevLines != document_->linesCount()) {
        document_->flushTransaction();
        if (analizer_
                && command.type==KeyCommand::InsertText
                && command.text=="\n") {
            // Try to complete closing bracket
            Shared::TextAppend append = analizer_->closingBracketSuggestion(
                        document_->documentId,
                        prevRow);
                // Ask for a compiler
            if (append.first.length()>0) {
                // If found suggesstion
                const QStringList extraLines = append.first.split('\n', QString::KeepEmptyParts);
                for (int i=0; i<extraLines.size(); i++) {
                    // insert closing bracket text lines
                    QString line = extraLines[i];
                    evaluateCommand(KeyCommand::MoveToStartOfLine);
                    if (i<extraLines.size()-1)
                        line += "\n";
                    evaluateCommand(line);
                }
                for (quint32 i=0; i<append.second; i++)
                    // move cursor back required times
                    evaluateCommand(KeyCommand::MoveToPreviousLine);
                evaluateCommand(KeyCommand::MoveToEndOfLine);
                if (document_->textAt(row_).trimmed().length()>0) {
                    evaluateCommand(" ");
                }
            }
        }
    }
    if (ExtensionSystem::PluginManager::instance()->currentGlobalState()==ExtensionSystem::GS_Observe
            && command.type & KeyCommand::CommandModifiesTextMask
            )
        ExtensionSystem::PluginManager::instance()->switchGlobalState(ExtensionSystem::GS_Unlocked);
}

void TextCursor::moveTo(int row, int col)
{
    visibleFlag_ = false;
    updateRequest();
    row_ = qMax(0, row);
    column_ = qMax(0, col);
    if (viewMode_!=VM_Hidden) {
        visibleFlag_ = true;
        updateRequest();
    }
    emitPositionChanged();
}

void TextCursor::selectRangeText(int fromRow, int fromCol, int toRow, int toCol)
{
    visibleFlag_ = false;
    updateRequest();

    int endX = toCol;
    int endY = toRow;

    removeSelection();
    selectionRect_ = QRect(-1,-1,0,0);

    bool leftToRight = true;

    if (toRow<fromRow) {
        leftToRight = false;
        int tmp = toRow;
        toRow = fromRow;
        fromRow = tmp;
    }
    else if (toRow==fromRow) {
        leftToRight = toCol > fromCol;
    }

    for (int i=fromRow+1; i<toRow; i++) {
        if (i<document_->linesCount()) {
            const QString text = document_->textAt(i);
            for (int j=0; j<text.size(); j++) {
                document_->setSelected(i,j,true);
            }
            document_->setEndOfLineSelected(i, true);
        }
    }
    if (leftToRight) {
        if (fromRow<document_->linesCount()) {
            const QString text = document_->textAt(fromRow);
            QList<bool> sm = document_->selectionMaskAt(fromRow);
            int indent = document_->indentAt(fromRow)*2;
            int start = fromCol - indent;
            int end = (fromRow==toRow)? toCol-indent : text.size();
            start = qMax(0,start);
            start = qMin(start, sm.size());
            end = qMax(0,end);
            end = qMin(end, sm.size());
            for (int j=start; j<end; j++) {
                document_->setSelected(fromRow, j, true);
            }
            document_->setEndOfLineSelected(fromRow, fromRow!=toRow);
        }
        if (toRow<document_->linesCount()) {
            QList<bool> sm = document_->selectionMaskAt(toRow);
            int indent = document_->indentAt(toRow)*2;
            int start = (fromRow==toRow)? toCol-indent : 0;
            int end = toCol - indent;
            start = qMax(0,start);
            start = qMin(start, sm.size());
            end = qMax(0,end);
            end = qMin(end, sm.size());
            for (int j=start; j<end; j++) {
                document_->setSelected(toRow, j, true);
            }
        }
    }
    else {
        if (fromRow<document_->linesCount()) {
            const QString text = document_->textAt(fromRow);
            QList<bool> sm = document_->selectionMaskAt(fromRow);
            int indent = document_->indentAt(fromRow)*2;
            int start = toCol - indent;
            int end = (fromRow==toRow)? fromCol-indent : text.size();
            start = qMax(0,start);
            start = qMin(start, sm.size());
            end = qMax(0,end);
            end = qMin(end, sm.size());
            for (int j=start; j<end; j++) {
                document_->setSelected(fromRow, j, true);
            }
            document_->setEndOfLineSelected(fromRow, fromRow!=toRow);
        }
        if (toRow<document_->linesCount()) {
            QList<bool> sm = document_->selectionMaskAt(toRow);
            int indent = document_->indentAt(toRow)*2;
            int start = (fromRow==toRow)? fromCol-indent : 0;
            int end = fromCol - indent;
            start = qMax(0,start);
            start = qMin(start, sm.size());
            end = qMax(0,end);
            end = qMin(end, sm.size());
            for (int j=start; j<end; j++) {
                document_->setSelected(toRow, j, true);
            }
        }
    }
    row_ = endY;
    column_ = endX;
    if (viewMode_!=VM_Hidden) {
        visibleFlag_ = true;
    }
    updateRequest();
    emitPositionChanged();
}

void TextCursor::selectRangeBlock(int fromRow, int fromCol, int toRow, int toCol)
{
    visibleFlag_ = false;
    updateRequest();

    removeSelection();
    selectionRect_ = QRect(qMin(fromCol, toCol),
                           qMin(fromRow, toRow),
                           abs(fromCol-toCol)+1,
                           abs(fromRow-toRow)+1
                           );

    row_ = toRow;
    column_ = toCol;

    if (viewMode_!=VM_Hidden) {
        visibleFlag_ = true;
    }
    updateRequest();
    emitPositionChanged();
}

void TextCursor::movePosition(QTextCursor::MoveOperation op, MoveMode m, int n)
{
    visibleFlag_ = false;
    updateRequest();
    bool wasRectSelection = hasRectSelection();
    if (m==MM_Move) {
        removeSelection();
        selectionRect_ = QRect(-1, -1, 0, 0);
    }
    else if (m==MM_Select) {
        selectionRect_ = QRect(-1, -1, 0, 0);
    }
    else if (m==MM_RectSelect) {
        removeSelection();
        if (!selectionRect_.isValid()) {
            selectionRect_ = QRect(column_, row_, 0, 0);
        }
    }
    for (int i=0; i<n; i++) {
        if (op==QTextCursor::NextCell) {
            if (m==MM_Move)
                column_ ++;
            else if (m==MM_Select) {
                int indent = 2 * document_->indentAt(row_);
                if (row_<document_->linesCount()) {
                    int textPos = column_-indent;
                    if (textPos<0) {
                        column_ = indent;
                        textPos = 0;
                    }
                    if (textPos<document_->textAt(row_).length()) {
                        document_->setSelected(row_, textPos, !document_->selectionMaskAt(row_)[textPos]);
                        column_++;
                    }
                    else {
                        if (row_<document_->linesCount()) {
                            document_->setEndOfLineSelected(row_, !document_->lineEndSelectedAt(row_));
                        }
                        row_++;
                        column_ = document_->indentAt(row_)*2;
                    }
                }
            }
            else if (m==MM_RectSelect) {
                column_++;
                if (column_>selectionRect_.right())
                    selectionRect_.setRight(column_);
                else {
                    selectionRect_.setLeft(column_);
                }
                selectionRect_.setHeight(qMax(1, selectionRect_.height()));
            }

        }
        else if (op==QTextCursor::PreviousCell) {
            if (m==MM_Move) {
                if (column_==0 && row_==0) {}
                else if (column_==0 && row_>0) {
                    row_ --;
                    int indent = document_->indentAt(row_) * 2;
                    int textPos = 0;
                    if (row_<document_->linesCount())
                        textPos = document_->textAt(row_).length();
                    column_ = indent + textPos;

                }
                else
                    column_--;
            }
            else if (m==MM_Select) {
                int indent = document_->indentAt(row_)*2;
                if (column_<=indent && row_==0) {}
                else if (row_>=document_->linesCount()) {
                    row_ = document_->linesCount()-1;
                    column_ = document_->indentAt(row_) * 2
                            + document_->textAt(row_).length();
                    document_->setEndOfLineSelected(row_, !document_->lineEndSelectedAt(row_));
                }
                else if (row_<document_->linesCount()
                         && column_>indent+document_->textAt(row_).length())
                {
                    column_ = document_->indentAt(row_) * 2
                            + document_->textAt(row_).length()-1;
                    document_->setSelected(row_, document_->selectionMaskAt(row_).size()-1,
                                            !document_->selectionMaskAt(row_).last());
                }
                else if (column_<=indent && row_>0) {
                    document_->setEndOfLineSelected(row_-1, !document_->lineEndSelectedAt(row_-1));
                    row_ --;
                    column_ = document_->textAt(row_).length() +
                            document_->indentAt(row_) * 2;
                }
                else {
                    int textPos = column_ - document_->indentAt(row_)*2;
                    if (textPos>0) {
                        document_->setSelected(row_, textPos-1, !document_->selectionMaskAt(row_)[textPos-1]);
                        column_ --;
                    }
                }
            }
            else if (m==MM_RectSelect) {
                if (column_>0) {
                    column_--;
                    if (column_<selectionRect_.left()) {
                        selectionRect_.setLeft(column_);
                    }
                    else {
                        selectionRect_.setRight(column_);
                    }
                    selectionRect_.setHeight(qMax(1, selectionRect_.height()));
                }
            }
        }
        else if (op==QTextCursor::NextRow) {
            if (m==MM_Move)
                row_++;
            else if (m==MM_Select) {
                if (row_<document_->linesCount()) {
                    document_->setEndOfLineSelected(row_, !document_->lineEndSelectedAt(row_));
                    int textPos = column_ - document_->indentAt(row_) * 2;
                    if (textPos<0)
                        textPos = 0;
                    for (int i=textPos; i<document_->textAt(row_).length(); i++) {
                        document_->setSelected(row_, i, !document_->selectionMaskAt(row_)[i]);
                    }

                    row_++;
                    textPos = column_ - document_->indentAt(row_) * 2;
                    if (textPos<0)
                        textPos = 0;
                    if (row_>=document_->linesCount())
                        textPos = 0;
                    else if (textPos>document_->textAt(row_).length())
                        textPos = document_->textAt(row_).length();
                    for (int i=0; i<textPos; i++) {
                        document_->setSelected(row_, i, !document_->selectionMaskAt(row_)[i]);
                    }
                }
            }
            else if (m==MM_RectSelect) {
                row_++;
                if (row_>selectionRect_.bottom())
                    selectionRect_.setBottom(row_);
                else {
                    selectionRect_.setTop(row_);
                }
                selectionRect_.setWidth(qMax(1, selectionRect_.width()));
            }
        }
        else if (op==QTextCursor::PreviousRow) {
            if (row_==0) {}
            else {
                if (m==MM_Move)
                    row_--;
                else if (m==MM_Select) {
                    if (row_==0) {
                        int textPos = column_ - document_->indentAt(row_) * 2;
                        for (int i=0; i<qMin(textPos, document_->textAt(row_).length()); i++) {
                            document_->setSelected(row_, i, !document_->selectionMaskAt(row_)[i]);
                        }
                        column_ = document_->indentAt(0)*2;
                    }
                    else {
                        int textPos = column_ - document_->indentAt(row_) * 2;
                        if (row_<document_->linesCount()) {
                            for (int i=0; i<qMin(textPos, document_->textAt(row_).length()); i++) {
                                document_->setSelected(row_, i, !document_->selectionMaskAt(row_)[i]);
                            }
                            row_ --;
                        }
                        else {
                            row_ = document_->linesCount()-1;
                        }
                        textPos = column_ - document_->indentAt(row_) * 2;
                        if (textPos<0)
                            textPos = 0;
                        for (int i=textPos; i<document_->textAt(row_).length(); i++) {
                            document_->setSelected(row_, i, !document_->selectionMaskAt(row_)[i]);
                        }
                        document_->setEndOfLineSelected(row_, !document_->lineEndSelectedAt(row_));
                    }
                }
                else if (m==MM_RectSelect) {
                    row_--;
                    if (row_<selectionRect_.top())
                        selectionRect_.setTop(row_);
                    else {
                        selectionRect_.setBottom(row_);
                    }
                    selectionRect_.setWidth(qMax(1, selectionRect_.width()));
                }
            }
        }
        else if (op==QTextCursor::StartOfBlock) {
            if (m==MM_Move) {
                if (row_>=document_->linesCount())
                    column_ = 0;
                else {
                    int indent = document_->indentAt(row_) * 2;
                    const QString textLine = document_->textAt(row_);
                    int nonSpacePos = 0;
                    for (; nonSpacePos<textLine.length(); nonSpacePos++) {
                        if (!textLine[nonSpacePos].isSpace())
                            break;
                    }
                    if (nonSpacePos==textLine.length())
                        nonSpacePos = 0;
                    int beforeTextPos = indent + nonSpacePos;
                    if (column_!=beforeTextPos)
                        column_ = beforeTextPos;
                    else
                        column_ = indent;
                }
            }
            else if (m==MM_Select) {
                if (row_<document_->linesCount()) {
                    int textPos = column_ - document_->indentAt(row_);
                    textPos = qMax(0, textPos);
                    textPos = qMin(document_->textAt(row_).length(), textPos);
                    for (int i=0; i<textPos; i++) {
                        document_->setSelected(row_, i, !document_->selectionMaskAt(row_)[i]);
                    }
                    column_ = document_->indentAt(row_) * 2;
                }
                else {
                    column_ = 0;
                }
            }
        }
        else if (op==QTextCursor::EndOfBlock) {
            if (m==MM_Move) {
                if (row_>=document_->linesCount()) { column_ = 0; }
                else {
                    const QString textLine = document_->textAt(row_);
                    int indent = document_->indentAt(row_) * 2;
                    int nonSpacePos = textLine.length();
                    for (int i=textLine.length()-1; i>=0; i--) {
                        if (textLine[i].isSpace())
                            nonSpacePos--;
                        else
                            break;
                    }
                    column_ = indent + nonSpacePos;
//                    i_column = m_document->indentAt(i_row) * 2 +
//                            m_document->textAt(i_row).length();
                }
            }
            else if (m==MM_Select) {
                if (row_<document_->linesCount()) {
                    int textPos = column_ - document_->indentAt(row_) * 2;
                    textPos = qMax(0, textPos);
                    for (int i=textPos; i<document_->textAt(row_).length(); i++) {
                        document_->setSelected(row_, i, !document_->selectionMaskAt(row_)[i]);
                    }
                    column_ = document_->indentAt(row_) * 2 +
                            document_->textAt(row_).length();
                }
            }
        }
        else if (op==QTextCursor::Start) {
            if (m==MM_Move) {
                row_ = 0;
                column_ = 0;
            }
            else if (m==MM_Select) {
                if (row_<document_->linesCount()) {
                    int textPos = column_ - document_->indentAt(row_) * 2;
                    textPos = qMax(0, textPos);
                    textPos = qMin(document_->textAt(row_).length(), textPos);
                    for (int i=0; i<textPos; i++) {
                        document_->setSelected(row_, i, !document_->selectionMaskAt(row_)[i]);
                    }
                    row_--;
                }
                else {
                    row_ = document_->linesCount()-1;
                }
                for ( ; row_>=0; row_-- ) {
                    for (int i=0; i<document_->selectionMaskAt(row_).size(); i++) {
                        document_->setSelected(row_, i, !document_->selectionMaskAt(row_)[i]);
                    }
                    document_->setEndOfLineSelected(row_, !document_->lineEndSelectedAt(row_));
                }
                column_ = 0;
                row_ = 0;
            }
        }
        else if (op==QTextCursor::End) {
            if (m==MM_Move) {
                row_ = document_->linesCount()-1;
                column_ = document_->indentAt(row_)*2 +
                        document_->textAt(document_->linesCount()-1).length();
            }
            else if (m==MM_Select) {
                if (row_>=document_->linesCount()) {
                    row_ = document_->linesCount()-1;
                    if (row_!=-1) {
                        column_ = document_->indentAt(row_)*2 +
                                document_->textAt(document_->linesCount()-1).length();
                    }
                    else {
                        row_ = 0;
                        column_ = 0;
                    }
                }
                else {
                    int textPos = column_ - document_->indentAt(row_);
                    textPos = qMax(0, textPos);
                    for (int i=textPos; i<document_->textAt(row_).length(); i++) {
                        document_->setSelected(row_, i, !document_->selectionMaskAt(row_)[i]);
                    }
                    document_->setEndOfLineSelected(row_, !document_->lineEndSelectedAt(row_));
                    row_ ++;
                    for ( ; row_<document_->linesCount(); row_++ ) {
                        for (int i=0; i<document_->selectionMaskAt(row_).size(); i++) {
                            document_->setSelected(row_, i, !document_->selectionMaskAt(row_)[i]);
                        }
                        if (row_<document_->linesCount()-1) {
                            document_->setEndOfLineSelected(row_, !document_->lineEndSelectedAt(row_));
                        }
                    }
                    row_ = document_->linesCount()-1;
                    column_ = document_->indentAt(row_)*2 +
                            document_->textAt(document_->linesCount()-1).length();
                }
            }
        } // end End
        else if (op==QTextCursor::PreviousWord) {
            uint toRow, toColumn;
            findLexemBound(toRow, toColumn, -1);
            if (m==MM_Move) {
                row_ = toRow; column_ = toColumn;
            }
            else if (m==MM_Select && (row_>0 || column_>0) ) {
                if (row_>=document_->linesCount()) {
                    row_ = document_->linesCount() - 1;
                    column_ = 2*document_->indentAt(row_) +
                            document_->at(row_).text.length()+1;
                }
                column_ -= 1;
                    // Selection starts from left of current column

                forever {
                    int stopAt = row_==toRow? toColumn : 2*document_->indentAt(row_);
                    forever {
                        int linePos = column_ - 2*document_->indentAt(row_);
                        if (linePos>=0 && linePos<document_->at(row_).text.length())
                            document_->at(row_).selected[linePos] = !document_->at(row_).selected[linePos];
                        if (column_>stopAt)
                            column_ -= 1;
                        else
                            break;
                    }
                    if (row_==toRow)
                        break;
                    row_ -= 1;
                    document_->at(row_).lineEndSelected = !document_->at(row_).lineEndSelected;
                    column_ = 2*document_->indentAt(row_) +
                            document_->at(row_).text.length();
                }
            }
        } // end PreviousWord
        else if (op==QTextCursor::NextWord) {
            uint toRow, toColumn;
            findLexemBound(toRow, toColumn, +1);
            if (m==MM_Move) {
                row_ = toRow; column_ = toColumn;
            }
            else if (m==MM_Select) {
                forever {
                    forever {
                        if (row_==toRow && column_==toColumn)
                            break;
                        int linePos = column_ - 2*document_->indentAt(row_);
                        if (linePos>=0 && linePos<document_->at(row_).text.length())
                            document_->at(row_).selected[linePos] = !document_->at(row_).selected[linePos];
                        if (linePos<document_->at(row_).text.length())
                            column_ += 1;
                        else
                            break;
                    }
                    if (row_==toRow)
                        break;
                    document_->at(row_).lineEndSelected = !document_->at(row_).lineEndSelected;
                    row_ += 1;
                    column_ = 0;
                }
            }
        } // end NextWord
    }
    visibleFlag_ = true;
    emit updateRequest();
    if (m==MM_Select || m==MM_RectSelect || wasRectSelection) {
        emit updateRequest(-1, -1);
    }
    emitPositionChanged();
    Q_ASSERT(row_>=0);
    Q_ASSERT(column_>=0);
}

void TextCursor::findLexemBound(uint &row, uint &column, const qint8 dir) const
{
    row = row_; column = column_;

    if (dir>0 && row_>=document_->linesCount())
        return;
    if (dir<0 && row_==0 && row_<document_->linesCount() && column_<=2*document_->indentAt(0))
        return;

    int linePos = column - 2*document_->indentAt(row);

    if (dir<0) {
        // search backward
        int realTextRow = qMin(document_->linesCount()-1, row);
        int realTextEndPos = document_->at(realTextRow).text.length();
        while (realTextEndPos>0) {
            // find last non space position in text
            if (document_->at(realTextRow).text.at(realTextEndPos-1).isSpace())
                realTextEndPos -= 1;
            else
                break;
        }
        if (linePos>realTextEndPos || row>realTextRow) {
            // try to act like 'End' key
            row = realTextRow;
            column = realTextEndPos + 2*document_->indentAt(row);
            return;
        }

        forever {
            linePos = column - 2*document_->indentAt(row);
            linePos = qMax(0, qMin(linePos, document_->at(row).text.length()));
                // Normalization of linePos

            if (linePos==0 && row==0) {
                // found start of document
                return;
            }
            else if (linePos==0 && row>0) {
                // jump to end of previous line
                row -= 1;
                linePos = document_->at(row).text.length();
                while (linePos>0 && document_->at(row).text.at(linePos-1).isSpace()) {
                    linePos -= 1;
                }
                column = linePos + 2*document_->indentAt(row);
                return;
            }
            else {
                // jump to start of lexem (or word in text mode)
                if (analizer_) {
                    // jump to start of lexem in this case
                    LexemType curLexemType, prevPosLexemType;
                    curLexemType = document_->at(row).highlight.at(linePos-1);
                    while (linePos>0 && curLexemType==LxTypeEmpty) {
                        // ignore whitespaces
                        linePos -= 1;
                        curLexemType = document_->at(row).highlight.at(linePos-1);
                    }
                    while (linePos>0) {
                        // linePos==0 is : line bound is a delimeter too
                        prevPosLexemType = document_->at(row).highlight.at(linePos-1);
                        if (prevPosLexemType!=curLexemType)
                            // found a lexem of previous type
                            break;
                        linePos -= 1;
                    }
                }
                else {
                    // jump to last non-space position at left
                    if (linePos>0
                            &&
                            linePos<document_->at(row).text.length()
                            && document_->at(row).text.at(linePos-1).isSpace()
                            )
                    {
                        // if already at begin of word, then go left
                        // to find previous word
                        linePos -= 1;

                    }
                    while (linePos>=0) {
                        // skip trailing spaces first
                        if (linePos>=document_->at(row).text.length() || document_->at(row).text.at(linePos).isSpace())
                            linePos -= 1;
                        else break;
                    }
                    while (linePos>0) {
                        // do actual search
                        bool prevIsSpace = document_->at(row).text.at(linePos-1).isSpace();
                        if (prevIsSpace)
                            // found start of word
                            break;
                        linePos -= 1;
                    }
                }
                column = linePos + 2*document_->indentAt(row);
                return;
            }
        }
    }
    else if (dir>0) {
        // search forward
        forever {
            linePos = column - 2*document_->indentAt(row);
            linePos = qMax(linePos, 0);
                // skip indent pos

            // find text line length excluding trailing spaces
            const QString & lineText = document_->at(row).text;
            int length = lineText.length();
            int trailingSpaces = 0;
            for (int k=length-1; k>=0; k--) {
                if (lineText[k].isSpace())
                    trailingSpaces += 1;
                else
                    break;
            }
            length -= trailingSpaces;
            if (row==document_->linesCount()-1 && linePos>=length) {
                // found end of document
                return;
            }
            else if (linePos>=length && row+1<document_->linesCount()) {
                // jump to start of next line
                row += 1;
                linePos = 0;
                while (linePos<lineText.length() && lineText.at(linePos).isSpace()) {
                    linePos += 1;
                }
                column = linePos + 2*document_->indentAt(row);
                return;
            }
            else {
                if (analizer_) {
                    // jump to end of lexem in this case
                    LexemType curLexemType, nextPosLexemType;
                    curLexemType = document_->at(row).highlight.at(linePos);
                    while (linePos<length && curLexemType==LxTypeEmpty) {
                        // ignore whitespaces
                        curLexemType = document_->at(row).highlight.at(linePos);
                        linePos += 1;
                    }
                    while (linePos<length) {
                        linePos += 1;
                        if (linePos==length)
                            // found end of line (except trailing spaces)
                            break;
                        nextPosLexemType = document_->at(row).highlight.at(linePos);
                        if (nextPosLexemType!=curLexemType && nextPosLexemType!=LxTypeEmpty)
                            // found a lexem of next type;
                            break;
                    }
                }
                else {
                    // jump to start of next word, i.e.
                    // first non-space symbol after any spaces
                    while (linePos<length
                           && !document_->at(row).text.at(linePos).isSpace())
                    {
                        // at first - go to end of current word
                        linePos += 1;
                    }
                    while (linePos<length
                           && document_->at(row).text.at(linePos).isSpace()
                           )
                    {
                        // skip spaces
                        linePos += 1;
                    }
                }
                column = linePos + 2*document_->indentAt(row);
                return;
            }
        }
    }
}

void TextCursor::insertBlock(const QStringList &block)
{
    if (!enabledFlag_)
        return;
    if (modifiesProtectedLiines())
        return;
    if (hasSelection())
        removeSelectedText();
    if (hasRectSelection())
        removeSelectedBlock();

    document_->undoStack()->push(new InsertBlockCommand(document_, this, analizer_, row_, column_, block));

    emit updateRequest(-1, -1);

    emitPositionChanged();
}

void TextCursor::removeRectSelection()
{
    selectionRect_ = QRect(-1, -1, 0, 0);
    emit updateRequest();
}

bool TextCursor::modifiesProtectedLiines() const
{
    if (teacherModeFlag_)
        return false;
    if (hasRectSelection()) {
        uint lineStart = selectionRect_.top();
        uint lineEnd = selectionRect_.bottom();

        lineStart = qMin(
                    qMax(0u, lineStart),
                    document_->linesCount()-1
                    );

        lineEnd = qMin(
                    qMax(1u, lineEnd),
                    document_->linesCount()
                    );

        for (int i=lineStart; i<lineEnd; i++) {
            if (document_->isProtected(i)) {
                return true;
            }
        }
    }
    for (int i=0; i<document_->linesCount(); i++) {
        if (document_->isProtected(i)) {
            if (document_->lineEndSelectedAt(i))
                return true;
            if (row_==i)
                return true;
            if (document_->selectionMaskAt(i).contains(true))
                return true;
        }
    }
    return false;
}

void TextCursor::insertText(const QString &text)
{
    if (!enabledFlag_) {
        emit signalizeNotEditable();
        return;
    }

    if (modifiesProtectedLiines())
        return;

    bool sel = hasSelection();
    bool bsel = hasRectSelection();

    if (sel) {
        document_->undoStack()->beginMacro("replaceSelectedText");
        removeSelectedText();
    }
    if (bsel) {
        document_->undoStack()->beginMacro("replaceSelectedBlock");
        removeSelectedBlock();
    }



    int fromLineUpdate = row_;

    const int indent = document_->indentAt(row_);

    // move cursor from protected part
    column_ = qMax(column_, indent * 2u);

    if (document_->m_settings->value(SettingsPage::KeyForcePressTextToLeft, SettingsPage::DefaultForcePressTextToLeft).toBool())
        column_ = justifyLeft(text);

    int textPos = column_ - indent * 2;
    document_->undoStack()->push(new InsertCommand(document_, this, analizer_, row_, textPos, text));

    int toLineUpdate = -1;

    emit updateRequest(fromLineUpdate, toLineUpdate);

    if (sel || bsel)
        document_->undoStack()->endMacro();

    emitPositionChanged();
}

int TextCursor::justifyLeft(const QString &text) const
{

    if (!analizer_ || text.trimmed().isEmpty())
        return column_;

    // Emulate text change and get line property

    const int indent = document_->indentAt(row_) * 2;
    int col = column_;
    int textPos = col - indent;
    if (textPos<=0) {
        return column_; // Nothing to do
    }
    QString s;
    if (document_->linesCount()>row_) {
        s = document_->textAt(row_);
    }
    s.insert(textPos, text);
    LineProp lp = analizer_->lineProp(document_->documentId, s);
    if (lp[textPos]==LxTypeComment || lp[textPos]==LxConstLiteral) {
        // Do not justify comments and literal constants
        return column_;
    }
    else {
        if (textPos>0 && s[textPos-1].isSpace()) {
            // If typing new word -- press text to left
            while (textPos>0 && s[textPos-1].isSpace()) {
                textPos --;
            }
            if (textPos>0 && !text[0].isPunct())
                textPos++; // Do not concatenate to previous word if exists
        }
    }
    return indent + textPos;
}

void TextCursor::removePreviousChar()
{
    // TODO Undo-redo stack!
    if (!enabledFlag_)
        return;
    if (hasSelection()) {
        removeSelectedText();
        emitPositionChanged();
        return;
    }
    if (hasRectSelection()) {
        removeSelectedBlock();
        emitPositionChanged();
        return;
    }
    removeSelection();
    visibleFlag_ = false;
    emit updateRequest();
    int fromLineUpdate = -1;
    int toLineUpdate = -1;

    const int indent = document_->indentAt(row_);
    int textPos = column_ - indent * 2;
    if (textPos>0) {
        // remove previous char in current line
        if ( row_<document_->linesCount() &&
             textPos <= document_->textAt(row_).length())
        {
            document_->undoStack()->push(new RemoveCommand(document_,
                                                            this,
                                                            analizer_,
                                                            row_,
                                                            textPos-1,
                                                            1,
                                                            false,
                                                            row_,
                                                            column_-1
                                                            ));
        }
        else if (row_<document_->linesCount() && textPos>document_->textAt(row_).length()) {
            movePosition(QTextCursor::PreviousCell, TextCursor::MM_Move);
        }
        else {
            row_ --;
            column_ = 2 * document_->indentAt(row_);
        }
    }
    else {
        // remove current line and set cursor to end of previous line
        if (row_>0) {
            if (row_<document_->linesCount()) {
                document_->undoStack()->push(new RemoveCommand(document_,
                                                                this,
                                                                analizer_,
                                                                row_-1,
                                                                document_->textAt(row_-1).length(),
                                                                1,
                                                                false,
                                                                row_-1,
                                                                document_->textAt(row_-1).length() + 2*document_->indentAt(row_-1)
                                                                ));
            }
        }
    }

    visibleFlag_ = true;
    emit updateRequest();
    emit updateRequest(fromLineUpdate, toLineUpdate);

    emitPositionChanged();
}

void TextCursor::removeCurrentLine()
{
    if (!enabledFlag_)
        return;
    if (hasSelection()) {
        removeSelectedText();
        emitPositionChanged();
        return;
    }

    if (row_<document_->linesCount()) {
        int column = column_;
        movePosition(QTextCursor::EndOfBlock, MM_Move, 1);
        document_->undoStack()->push(new RemoveCommand(document_, this, analizer_, row_, 0, document_->textAt(row_).length()+1, true, row_, column_));
        column_ = column;
        emit updateRequest(-1, -1);
        emit updateRequest();
    }

    emitPositionChanged();
}

void TextCursor::removeLineTail()
{
    if (!enabledFlag_)
        return;
    if (hasSelection()) {
        removeSelection();
    }

    if (row_<document_->linesCount()) {

        int textPos = column_ - document_->indentAt(row_)*2;
        if (textPos<document_->textAt(row_).length()) {
            document_->undoStack()->push(new RemoveCommand(document_, this, analizer_, row_, textPos, document_->textAt(row_).length()-textPos, true, row_, column_));
            emit updateRequest(-1, -1);
            emit updateRequest();
        }
    }

    emitPositionChanged();
}

void TextCursor::removeCurrentChar()
{
    // TODO Undo-redo stack!
    if (!enabledFlag_)
        return;
    if (hasSelection()) {
        removeSelectedText();
        emitPositionChanged();
        return;
    }
    if (hasRectSelection()) {
        removeSelectedBlock();
        emitPositionChanged();
        return;
    }
    visibleFlag_ = false;
    emit updateRequest();

    int fromLineUpdate = -1;
    int toLineUpdate = -1;

    const int indent = document_->indentAt(row_);
    int textPos = column_ - indent * 2;
    if (row_>=document_->linesCount())
        return;
    if (textPos>=document_->textAt(row_).length() && row_>=document_->linesCount()-1)
        return;
    if (textPos<0) {
        column_ = 2*document_->indentAt(row_);
        return;
    }
    document_->undoStack()->push(new RemoveCommand(document_, this, analizer_, row_, textPos, 1, true, row_, column_));

    visibleFlag_ = true;
    emit updateRequest();
    emit updateRequest(fromLineUpdate, toLineUpdate);

    emitPositionChanged();
}


void TextCursor::removeSelectedText()
{
    if (!enabledFlag_)
        return;

    if (!hasSelection())
        return;

    if (modifiesProtectedLiines())
        return;

    // Find where to place cursor after deletion

    int cursorStartLine = -1;
    int cursorTextPos = -1;

    for (int i=0; i<document_->linesCount(); i++) {

        if (document_->lineEndSelectedAt(i) && cursorStartLine==-1)
            cursorStartLine = i;
        QList<bool> sm = document_->selectionMaskAt(i);
        for (int j=0; j<sm.size(); j++) {
            if (sm[j] && cursorTextPos==-1)
                cursorTextPos = j;
            if (sm[j] && cursorStartLine==-1)
                cursorStartLine = i;
        }
        if (cursorStartLine!=-1) {
            if (cursorTextPos==-1) {
                cursorTextPos = document_->textAt(i).length();
            }
            break;
        }
    }

    // Remove selected text inside lines

    int selectionLineStart = -1;
    int selectionLineEnd = -1;

    int lineStart = -1;
    int posStart = -1;
    int count = 0;

    for (int i=0; i<document_->linesCount(); i++) {
        int start = -1;
        int end = -1;
        QList<bool> sm = document_->selectionMaskAt(i);
        for (int j=0; j<sm.size(); j++) {
            bool v = sm[j];
            if (v) {
                if (selectionLineStart==-1) {
                    selectionLineStart = i;
                    lineStart = i;
                    posStart = j;
                }
                selectionLineEnd = qMax(selectionLineEnd, i);
                if (start==-1) {
                    start = j;
                }
                end = qMax(j, end);
                count ++;
            }
        }
        if (document_->lineEndSelectedAt(i)) {
            if (selectionLineStart==-1) {
                selectionLineStart = i;
                lineStart = i;
                posStart = document_->textAt(i).length();
            }
            count ++;
            selectionLineEnd = qMax(i+1, selectionLineEnd);
        }
    }


    if (count>0) {
        document_->undoStack()->push(new RemoveCommand(document_,
                                                        this,
                                                        analizer_,
                                                        lineStart,
                                                        posStart,
                                                        count,
                                                        true,
                                                        cursorStartLine,
                                                        document_->indentAt(row_)*2 + cursorTextPos
                                                        ));
    }

    // Move cursor

    removeSelection();

    emit updateRequest(-1, -1);
    emit updateRequest();

}

void TextCursor::removeSelectedBlock()
{
    if (!enabledFlag_)
        return;

    if (!hasRectSelection())
        return;

    int lineStart = selectionRect_.top();
    int startPos = selectionRect_.left();

    document_->undoStack()->push(new RemoveBlockCommand(document_, this, analizer_, selectionRect_));

    row_ = lineStart;
    column_ = startPos;
    selectionRect_ = QRect(-1, -1, 0, 0);
    emit updateRequest(-1, -1);
    emit updateRequest();

    emitPositionChanged();
}

bool TextCursor::hasSelection() const
{
    for (int i=0; i<document_->linesCount(); i++) {
        if (document_->lineEndSelectedAt(i))
            return true;
        if (document_->selectionMaskAt(i).contains(true))
            return true;
    }
    return false;
}


QString TextCursor::selectedText() const
{
    QString result;
    if (hasSelection()) {
        for (int i=0; i<document_->linesCount(); i++) {
            const QList<bool> sm = document_->selectionMaskAt(i);
            const QString text = document_->textAt(i);
//            Q_ASSERT(text.length()==sm.size());
            for (int j=0; j<qMin(text.length(), sm.size()); j++) {
                if (sm[j])
                    result += text[j];
            }
            if (document_->lineEndSelectedAt(i) && i<document_->linesCount()-1)
                result += "\n";
        }
    }
    else if (hasRectSelection()) {
        uint startLine = selectionRect_.top();
        uint endLine = qMin(document_->linesCount(), selectionRect_.bottom()+1u);
        uint startPos = selectionRect_.left();
        uint endPos = selectionRect_.right();
        for (uint i=startLine; i<endLine; i++) {
            uint indent = document_->indentAt(i);
            uint textStart = qMax(0, int(startPos) - int(indent) * 2);
            uint textEnd = qMax(0, int(endPos) - int(indent) * 2);
            textStart = qMin(textStart, uint(document_->textAt(i).length()));
            textEnd = qMin(textEnd, uint(document_->textAt(i).length()));
            result += document_->textAt(i).mid(textStart, textEnd-textStart);
            if (i<endLine-1) {
                result += "\n";
            }
        }
    }
    return result;
}

QStringList TextCursor::rectSelectionText() const
{
    QStringList result;
    if (hasRectSelection()) {
        uint startLine = selectionRect_.top();
        uint endLine = qMin(
                    document_->linesCount(),
                    selectionRect_.bottom()+1u);
        int startPos = selectionRect_.left();
        int endPos = selectionRect_.right()+1;
        for (int i=startLine; i<endLine; i++) {
            int indent = document_->indentAt(i);
            int textStart = startPos - indent * 2;
            int textEnd = endPos - indent * 2;
            textStart = qMin(textStart, document_->textAt(i).length());
            textEnd = qMin(textEnd, document_->textAt(i).length());
            textStart = qMax(0, textStart);
            textEnd = qMax(0, textEnd);
            QString text = document_->textAt(i).mid(textStart, textEnd-textStart);
            int w = selectionRect_.width();
            result << text.leftJustified(w, ' ');
        }
    }
    return result;
}

void TextCursor::selectionBounds(int &fromRow, int &fromCol, int &toRow, int &toCol) const
{
    fromRow = fromCol = toRow = toCol = -1;
    if (!hasSelection())
        return;
    for (int i=0; i<document_->linesCount(); i++) {
        if (document_->selectionMaskAt(i).contains(true) || document_->lineEndSelectedAt(i)) {
            if (fromRow==-1)
                fromRow = i;
        }
        if (!document_->lineEndSelectedAt(i)) {
            if (fromRow!=-1) {
                toRow = i;
                break;
            }
        }
    }
    const QList<bool> first = document_->selectionMaskAt(fromRow);
    fromCol = first.indexOf(true);
    if (fromCol==-1)
        fromCol = first.size();
    fromCol += 2 * document_->indentAt(fromRow);

    if (toRow!=-1) {
        const QList<bool> last = document_->selectionMaskAt(toRow);
        toCol = last.lastIndexOf(true);
        if (toCol==-1)
            toCol=0;
        toCol += 2 * document_->indentAt(toRow);
    }
    else {
        toRow = document_->linesCount();
        toCol = 0;
    }
}

void TextCursor::removeSelection()
{
    document_->removeSelection();
    emit updateRequest(-1, -1);
    emitPositionChanged();

}

void TextCursor::undo()
{
//    int prevRow = i_row;
//    int prevLines = m_document->linesCount();
    if (enabledFlag_) {
        document_->undoStack()->undo();
    }
    int row = row_;
    int column = column_;
//    if (prevRow!=i_row || prevLines!=m_document->linesCount()) {
        document_->forceCompleteRecompilation();
//    }
    row_ = row;
    column_ = column;
        // restore cursor position due it may be changed
        // after compilation request

    emit undoAvailable(enabledFlag_ && document_->undoStack()->canUndo());
    emit redoAvailable(enabledFlag_ && document_->undoStack()->canRedo());
}

void TextCursor::redo()
{
//    int prevRow = i_row;
//    int prevLines = m_document->linesCount();
    if (enabledFlag_) {
        document_->undoStack()->redo();
    }
//    if (prevRow!=i_row || prevLines!=m_document->linesCount()) {
        document_->forceCompleteRecompilation();
//    }
    emit undoAvailable(enabledFlag_ && document_->undoStack()->canUndo());
    emit redoAvailable(enabledFlag_ && document_->undoStack()->canRedo());
}

void TextCursor::handleRedoChanged(bool v)
{
    emit redoAvailable(enabledFlag_ && v);
}

void TextCursor::handleUndoChanged(bool v)
{
    emit undoAvailable(enabledFlag_ && v);
}

void TextCursor::setEnabled(bool v)
{
     enabledFlag_ = v;
     emit updateRequest();
     emit undoAvailable(enabledFlag_ && document_->undoStack()->canUndo());
     emit redoAvailable(enabledFlag_ && document_->undoStack()->canRedo());
}

} // namespace Editor
