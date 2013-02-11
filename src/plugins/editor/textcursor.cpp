#include "textcursor.h"
#include "textdocument.h"
#include "clipboard.h"
#include "editcommands.h"
#include "extensionsystem/pluginmanager.h"
#include "settingspage.h"

namespace Editor {

TextCursor::TextCursor(TextDocument * document, Clipboard * clipboard, AnalizerInterface * analizer)
    : QObject(0)
    , m_document(document)
    , m_clipboard(clipboard)
    , m_analizer(analizer)
    , e_mode(EM_Insert)
    , e_viewMode(VM_Hidden)
    , i_timerId(-1)
    , b_enabled(true)
    , b_visible(false)
    , i_row(0)
    , i_column(0)

{
    b_teacher = false;
    i_timerId = startTimer(QApplication::cursorFlashTime()/2);
    emitPositionChanged();

    rect_selection = QRect(-1,-1,0,0);
}

void TextCursor::setViewMode(ViewMode mode)
{
    e_viewMode = mode;
    if (mode==VM_Hidden)
        b_visible = false;
    else if (mode==VM_Visible)
        b_visible = true;
    emit updateRequest();
}



void TextCursor::emitPositionChanged()
{

    emit positionChanged(i_row, i_column);

}

void TextCursor::timerEvent(QTimerEvent *e)
{
    if (e_viewMode==VM_Blinking) {
        b_visible = !b_visible;
    }
    else if (e_viewMode==VM_Hidden) {
        b_visible = false;
    }
    else if (e_viewMode==VM_Visible) {
        b_visible = true;
    }
    emit updateRequest();
    e->accept();

}

TextCursor::~TextCursor()
{
    if (i_timerId!=-1)
        killTimer(i_timerId);
}

void TextCursor::toggleComment()
{
    if (!b_enabled)
        return;
    if (m_analizer==0)
        return;
    if (hasSelection()) {
        int start = -1;
        int end = -1;
        for (int i=0; i<m_document->linesCount(); i++) {
            if (m_document->lineEndSelectedAt(i) || m_document->selectionMaskAt(i).contains(true)) {
                if (start==-1) {
                    start = i;
                }
                end = i;
            }
        }
        m_document->undoStack()->push(new ToggleCommentCommand(
                                          m_document,
                                          start,
                                          end,
                                          this,
                                          m_analizer
                                          ));
    }
    else if (hasRectSelection()) {
        int start = rect_selection.top();
        int end = rect_selection.bottom();
        m_document->undoStack()->push(new ToggleCommentCommand(
                                          m_document,
                                          start,
                                          end,
                                          this,
                                          m_analizer
                                          ));
    }
    else {
        if (i_row < m_document->linesCount()) {
            m_document->undoStack()->push(new ToggleCommentCommand(
                                              m_document,
                                              i_row,
                                              i_row,
                                              this,
                                              m_analizer
                                              ));
        }
    }
}

void TextCursor::evaluateCommand(const KeyCommand &command)
{
    int prevRow = i_row;
    int prevLines = m_document->linesCount();
    bool clearCurrentLineError = false;
    switch (command.type) {
    case KeyCommand::MoveToNextChar:
        movePosition(QTextCursor::NextCell, TextCursor::MM_Move);
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
            data.html = m_document->toHtml(-1, -1);
            m_clipboard->push(data);
        }
        else if (hasRectSelection()) {
            ClipboardData data;
            data.text = selectedText();
            data.type = ClipboardData::Block;
            data.block = rectSelectionText();
            data.html = m_document->toHtml(rect_selection.top(), rect_selection.bottom());
            m_clipboard->push(data);
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
        clearCurrentLineError = true;
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
        if (m_clipboard->hasContent()) {
            ClipboardData data = m_clipboard->content();
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
    if (clearCurrentLineError && i_row<m_document->linesCount()) {
        m_document->setErrorsAt(i_row, QStringList());
    }

    if (prevRow!=i_row || prevLines != m_document->linesCount()) {
        m_document->flushTransaction();
        if (m_analizer
                && command.type==KeyCommand::InsertText
                && command.text=="\n") {
            // Try to complete closing bracket
            Shared::TextAppend append = m_analizer->closingBracketSuggestion(
                        m_document->documentId,
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
                if (m_document->textAt(i_row).trimmed().length()>0) {
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
    b_visible = false;
    updateRequest();
    i_row = qMax(0, row);
    i_column = qMax(0, col);
    if (e_viewMode!=VM_Hidden) {
        b_visible = true;
        updateRequest();
    }
    emitPositionChanged();
}

void TextCursor::selectRangeText(int fromRow, int fromCol, int toRow, int toCol)
{
    b_visible = false;
    updateRequest();

    int endX = toCol;
    int endY = toRow;

    removeSelection();
    rect_selection = QRect(-1,-1,0,0);

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
        if (i<m_document->linesCount()) {
            const QString text = m_document->textAt(i);
            for (int j=0; j<text.size(); j++) {
                m_document->setSelected(i,j,true);
            }
            m_document->setEndOfLineSelected(i, true);
        }
    }
    if (leftToRight) {
        if (fromRow<m_document->linesCount()) {
            const QString text = m_document->textAt(fromRow);
            QList<bool> sm = m_document->selectionMaskAt(fromRow);
            int indent = m_document->indentAt(fromRow)*2;
            int start = fromCol - indent;
            int end = (fromRow==toRow)? toCol-indent : text.size();
            start = qMax(0,start);
            start = qMin(start, sm.size());
            end = qMax(0,end);
            end = qMin(end, sm.size());
            for (int j=start; j<end; j++) {
                m_document->setSelected(fromRow, j, true);
            }
            m_document->setEndOfLineSelected(fromRow, fromRow!=toRow);
        }
        if (toRow<m_document->linesCount()) {
            QList<bool> sm = m_document->selectionMaskAt(toRow);
            int indent = m_document->indentAt(toRow)*2;
            int start = (fromRow==toRow)? toCol-indent : 0;
            int end = toCol - indent;
            start = qMax(0,start);
            start = qMin(start, sm.size());
            end = qMax(0,end);
            end = qMin(end, sm.size());
            for (int j=start; j<end; j++) {
                m_document->setSelected(toRow, j, true);
            }
        }
    }
    else {
        if (fromRow<m_document->linesCount()) {
            const QString text = m_document->textAt(fromRow);
            QList<bool> sm = m_document->selectionMaskAt(fromRow);
            int indent = m_document->indentAt(fromRow)*2;
            int start = toCol - indent;
            int end = (fromRow==toRow)? fromCol-indent : text.size();
            start = qMax(0,start);
            start = qMin(start, sm.size());
            end = qMax(0,end);
            end = qMin(end, sm.size());
            for (int j=start; j<end; j++) {
                m_document->setSelected(fromRow, j, true);
            }
            m_document->setEndOfLineSelected(fromRow, fromRow!=toRow);
        }
        if (toRow<m_document->linesCount()) {
            QList<bool> sm = m_document->selectionMaskAt(toRow);
            int indent = m_document->indentAt(toRow)*2;
            int start = (fromRow==toRow)? fromCol-indent : 0;
            int end = fromCol - indent;
            start = qMax(0,start);
            start = qMin(start, sm.size());
            end = qMax(0,end);
            end = qMin(end, sm.size());
            for (int j=start; j<end; j++) {
                m_document->setSelected(toRow, j, true);
            }
        }
    }
    i_row = endY;
    i_column = endX;
    if (e_viewMode!=VM_Hidden) {
        b_visible = true;
    }
    updateRequest();
    emitPositionChanged();
}

void TextCursor::selectRangeBlock(int fromRow, int fromCol, int toRow, int toCol)
{
    b_visible = false;
    updateRequest();

    removeSelection();
    rect_selection = QRect(qMin(fromCol, toCol),
                           qMin(fromRow, toRow),
                           abs(fromCol-toCol)+1,
                           abs(fromRow-toRow)+1
                           );

    i_row = toRow;
    i_column = toCol;

    if (e_viewMode!=VM_Hidden) {
        b_visible = true;
    }
    updateRequest();
    emitPositionChanged();
}

void TextCursor::movePosition(QTextCursor::MoveOperation op, MoveMode m, int n)
{
    b_visible = false;
    updateRequest();
    bool wasRectSelection = hasRectSelection();
    if (m==MM_Move) {
        removeSelection();
        rect_selection = QRect(-1, -1, 0, 0);
    }
    else if (m==MM_Select) {
        rect_selection = QRect(-1, -1, 0, 0);
    }
    else if (m==MM_RectSelect) {
        removeSelection();
        if (!rect_selection.isValid()) {
            rect_selection = QRect(i_column, i_row, 0, 0);
        }
    }
    for (int i=0; i<n; i++) {
        if (op==QTextCursor::NextCell) {
            if (m==MM_Move)
                i_column ++;
            else if (m==MM_Select) {
                int indent = 2 * m_document->indentAt(i_row);
                if (i_row<m_document->linesCount()) {
                    int textPos = i_column-indent;
                    if (textPos<0) {
                        i_column = indent;
                        textPos = 0;
                    }
                    if (textPos<m_document->textAt(i_row).length()) {
                        m_document->setSelected(i_row, textPos, !m_document->selectionMaskAt(i_row)[textPos]);
                        i_column++;
                    }
                    else {
                        if (i_row<m_document->linesCount()) {
                            m_document->setEndOfLineSelected(i_row, !m_document->lineEndSelectedAt(i_row));
                        }
                        i_row++;
                        i_column = m_document->indentAt(i_row)*2;
                    }
                }
            }
            else if (m==MM_RectSelect) {
                i_column++;
                if (i_column>rect_selection.right())
                    rect_selection.setRight(i_column);
                else {
                    rect_selection.setLeft(i_column);
                }
                rect_selection.setHeight(qMax(1, rect_selection.height()));
            }

        }
        else if (op==QTextCursor::PreviousCell) {
            if (m==MM_Move) {
                if (i_column==0 && i_row==0) {}
                else if (i_column==0 && i_row>0) {
                    i_row --;
                    int indent = m_document->indentAt(i_row) * 2;
                    int textPos = 0;
                    if (i_row<m_document->linesCount())
                        textPos = m_document->textAt(i_row).length();
                    i_column = indent + textPos;

                }
                else
                    i_column--;
            }
            else if (m==MM_Select) {
                int indent = m_document->indentAt(i_row)*2;
                if (i_column<=indent && i_row==0) {}
                else if (i_row>=m_document->linesCount()) {
                    i_row = m_document->linesCount()-1;
                    i_column = m_document->indentAt(i_row) * 2
                            + m_document->textAt(i_row).length();
                    m_document->setEndOfLineSelected(i_row, !m_document->lineEndSelectedAt(i_row));
                }
                else if (i_row<m_document->linesCount()
                         && i_column>indent+m_document->textAt(i_row).length())
                {
                    i_column = m_document->indentAt(i_row) * 2
                            + m_document->textAt(i_row).length()-1;
                    m_document->setSelected(i_row, m_document->selectionMaskAt(i_row).size()-1,
                                            !m_document->selectionMaskAt(i_row).last());
                }
                else if (i_column<=indent && i_row>0) {
                    m_document->setEndOfLineSelected(i_row-1, !m_document->lineEndSelectedAt(i_row-1));
                    i_row --;
                    i_column = m_document->textAt(i_row).length() +
                            m_document->indentAt(i_row) * 2;
                }
                else {
                    int textPos = i_column - m_document->indentAt(i_row)*2;
                    if (textPos>0) {
                        m_document->setSelected(i_row, textPos-1, !m_document->selectionMaskAt(i_row)[textPos-1]);
                        i_column --;
                    }
                }
            }
            else if (m==MM_RectSelect) {
                if (i_column>0) {
                    i_column--;
                    if (i_column<rect_selection.left()) {
                        rect_selection.setLeft(i_column);
                    }
                    else {
                        rect_selection.setRight(i_column);
                    }
                    rect_selection.setHeight(qMax(1, rect_selection.height()));
                }
            }
        }
        else if (op==QTextCursor::NextRow) {
            if (m==MM_Move)
                i_row++;
            else if (m==MM_Select) {
                if (i_row<m_document->linesCount()) {
                    m_document->setEndOfLineSelected(i_row, !m_document->lineEndSelectedAt(i_row));
                    int textPos = i_column - m_document->indentAt(i_row) * 2;
                    if (textPos<0)
                        textPos = 0;
                    for (int i=textPos; i<m_document->textAt(i_row).length(); i++) {
                        m_document->setSelected(i_row, i, !m_document->selectionMaskAt(i_row)[i]);
                    }

                    i_row++;
                    textPos = i_column - m_document->indentAt(i_row) * 2;
                    if (textPos<0)
                        textPos = 0;
                    if (i_row>=m_document->linesCount())
                        textPos = 0;
                    else if (textPos>m_document->textAt(i_row).length())
                        textPos = m_document->textAt(i_row).length();
                    for (int i=0; i<textPos; i++) {
                        m_document->setSelected(i_row, i, !m_document->selectionMaskAt(i_row)[i]);
                    }
                }
            }
            else if (m==MM_RectSelect) {
                i_row++;
                if (i_row>rect_selection.bottom())
                    rect_selection.setBottom(i_row);
                else {
                    rect_selection.setTop(i_row);
                }
                rect_selection.setWidth(qMax(1, rect_selection.width()));
            }
        }
        else if (op==QTextCursor::PreviousRow) {
            if (i_row==0) {}
            else {
                if (m==MM_Move)
                    i_row--;
                else if (m==MM_Select) {
                    if (i_row==0) {
                        int textPos = i_column - m_document->indentAt(i_row) * 2;
                        for (int i=0; i<qMin(textPos, m_document->textAt(i_row).length()); i++) {
                            m_document->setSelected(i_row, i, !m_document->selectionMaskAt(i_row)[i]);
                        }
                        i_column = m_document->indentAt(0)*2;
                    }
                    else {
                        int textPos = i_column - m_document->indentAt(i_row) * 2;
                        if (i_row<m_document->linesCount()) {
                            for (int i=0; i<qMin(textPos, m_document->textAt(i_row).length()); i++) {
                                m_document->setSelected(i_row, i, !m_document->selectionMaskAt(i_row)[i]);
                            }
                            i_row --;
                        }
                        else {
                            i_row = m_document->linesCount()-1;
                        }
                        textPos = i_column - m_document->indentAt(i_row) * 2;
                        if (textPos<0)
                            textPos = 0;
                        for (int i=textPos; i<m_document->textAt(i_row).length(); i++) {
                            m_document->setSelected(i_row, i, !m_document->selectionMaskAt(i_row)[i]);
                        }
                        m_document->setEndOfLineSelected(i_row, !m_document->lineEndSelectedAt(i_row));
                    }
                }
                else if (m==MM_RectSelect) {
                    i_row--;
                    if (i_row<rect_selection.top())
                        rect_selection.setTop(i_row);
                    else {
                        rect_selection.setBottom(i_row);
                    }
                    rect_selection.setWidth(qMax(1, rect_selection.width()));
                }
            }
        }
        else if (op==QTextCursor::StartOfBlock) {
            if (m==MM_Move) {
                if (i_row>=m_document->linesCount())
                    i_column = 0;
                else {
                    int indent = m_document->indentAt(i_row) * 2;
                    const QString textLine = m_document->textAt(i_row);
                    int nonSpacePos = 0;
                    for (; nonSpacePos<textLine.length(); nonSpacePos++) {
                        if (!textLine[nonSpacePos].isSpace())
                            break;
                    }
                    if (nonSpacePos==textLine.length())
                        nonSpacePos = 0;
                    int beforeTextPos = indent + nonSpacePos;
                    if (i_column!=beforeTextPos)
                        i_column = beforeTextPos;
                    else
                        i_column = indent;
                }
            }
            else if (m==MM_Select) {
                if (i_row<m_document->linesCount()) {
                    int textPos = i_column - m_document->indentAt(i_row);
                    textPos = qMax(0, textPos);
                    textPos = qMin(m_document->textAt(i_row).length(), textPos);
                    for (int i=0; i<textPos; i++) {
                        m_document->setSelected(i_row, i, !m_document->selectionMaskAt(i_row)[i]);
                    }
                    i_column = m_document->indentAt(i_row) * 2;
                }
                else {
                    i_column = 0;
                }
            }
        }
        else if (op==QTextCursor::EndOfBlock) {
            if (m==MM_Move) {
                if (i_row>=m_document->linesCount()) { i_column = 0; }
                else {
                    const QString textLine = m_document->textAt(i_row);
                    int indent = m_document->indentAt(i_row) * 2;
                    int nonSpacePos = textLine.length();
                    for (int i=textLine.length()-1; i>=0; i--) {
                        if (textLine[i].isSpace())
                            nonSpacePos--;
                        else
                            break;
                    }
                    i_column = indent + nonSpacePos;
//                    i_column = m_document->indentAt(i_row) * 2 +
//                            m_document->textAt(i_row).length();
                }
            }
            else if (m==MM_Select) {
                if (i_row<m_document->linesCount()) {
                    int textPos = i_column - m_document->indentAt(i_row) * 2;
                    textPos = qMax(0, textPos);
                    for (int i=textPos; i<m_document->textAt(i_row).length(); i++) {
                        m_document->setSelected(i_row, i, !m_document->selectionMaskAt(i_row)[i]);
                    }
                    i_column = m_document->indentAt(i_row) * 2 +
                            m_document->textAt(i_row).length();
                }
            }
        }
        else if (op==QTextCursor::Start) {
            if (m==MM_Move) {
                i_row = 0;
                i_column = 0;
            }
            else if (m==MM_Select) {
                if (i_row<m_document->linesCount()) {
                    int textPos = i_column - m_document->indentAt(i_row) * 2;
                    textPos = qMax(0, textPos);
                    textPos = qMin(m_document->textAt(i_row).length(), textPos);
                    for (int i=0; i<textPos; i++) {
                        m_document->setSelected(i_row, i, !m_document->selectionMaskAt(i_row)[i]);
                    }
                    i_row--;
                }
                else {
                    i_row = m_document->linesCount()-1;
                }
                for ( ; i_row>=0; i_row-- ) {
                    for (int i=0; i<m_document->selectionMaskAt(i_row).size(); i++) {
                        m_document->setSelected(i_row, i, !m_document->selectionMaskAt(i_row)[i]);
                    }
                    m_document->setEndOfLineSelected(i_row, !m_document->lineEndSelectedAt(i_row));
                }
                i_column = 0;
                i_row = 0;
            }
        }
        else if (op==QTextCursor::End) {
            if (m==MM_Move) {
                i_row = m_document->linesCount()-1;
                i_column = m_document->indentAt(i_row)*2 +
                        m_document->textAt(m_document->linesCount()-1).length();
            }
            else if (m==MM_Select) {
                if (i_row>=m_document->linesCount()) {
                    i_row = m_document->linesCount()-1;
                    i_column = m_document->indentAt(i_row)*2 +
                            m_document->textAt(m_document->linesCount()-1).length();
                }
                else {
                    int textPos = i_column - m_document->indentAt(i_row);
                    textPos = qMax(0, textPos);
                    for (int i=textPos; i<m_document->textAt(i_row).length(); i++) {
                        m_document->setSelected(i_row, i, !m_document->selectionMaskAt(i_row)[i]);
                    }
                    m_document->setEndOfLineSelected(i_row, !m_document->lineEndSelectedAt(i_row));
                    i_row ++;
                    for ( ; i_row<m_document->linesCount(); i_row++ ) {
                        for (int i=0; i<m_document->selectionMaskAt(i_row).size(); i++) {
                            m_document->setSelected(i_row, i, !m_document->selectionMaskAt(i_row)[i]);
                        }
                        if (i_row<m_document->linesCount()-1) {
                            m_document->setEndOfLineSelected(i_row, !m_document->lineEndSelectedAt(i_row));
                        }
                    }
                    i_row = m_document->linesCount()-1;
                    i_column = m_document->indentAt(i_row)*2 +
                            m_document->textAt(m_document->linesCount()-1).length();
                }
            }
        }
    }
    b_visible = true;
    emit updateRequest();
    if (m==MM_Select || m==MM_RectSelect || wasRectSelection) {
        emit updateRequest(-1, -1);
    }
    emitPositionChanged();
    Q_ASSERT(i_row>=0);
    Q_ASSERT(i_column>=0);
}

void TextCursor::insertBlock(const QStringList &block)
{
    if (!b_enabled)
        return;
    if (modifiesProtectedLiines())
        return;
    if (hasSelection())
        removeSelectedText();
    if (hasRectSelection())
        removeSelectedBlock();

    m_document->undoStack()->push(new InsertBlockCommand(m_document, this, m_analizer, i_row, i_column, block));

    emit updateRequest(-1, -1);

    emitPositionChanged();
}

void TextCursor::removeRectSelection()
{
    rect_selection = QRect(-1, -1, 0, 0);
    emit updateRequest();
}

bool TextCursor::modifiesProtectedLiines() const
{
    if (b_teacher)
        return false;
    if (hasRectSelection()) {
        int lineStart = rect_selection.top();
        int lineEnd = rect_selection.bottom();
        lineStart = qMin(qMax(0, lineStart), m_document->linesCount()-1);
        lineEnd = qMin(qMax(1, lineEnd), m_document->linesCount());
        for (int i=lineStart; i<lineEnd; i++) {
            if (m_document->isProtected(i)) {
                return true;
            }
        }
    }
    for (int i=0; i<m_document->linesCount(); i++) {
        if (m_document->isProtected(i)) {
            if (m_document->lineEndSelectedAt(i))
                return true;
            if (i_row==i)
                return true;
            if (m_document->selectionMaskAt(i).contains(true))
                return true;
        }
    }
    return false;
}

void TextCursor::insertText(const QString &text)
{
    if (!b_enabled) {
        emit signalizeNotEditable();
        return;
    }

    if (modifiesProtectedLiines())
        return;

    bool sel = hasSelection();
    bool bsel = hasRectSelection();

    if (sel) {
        m_document->undoStack()->beginMacro("replaceSelectedText");
        removeSelectedText();
    }
    if (bsel) {
        m_document->undoStack()->beginMacro("replaceSelectedBlock");
        removeSelectedBlock();
    }



    int fromLineUpdate = i_row;

    const int indent = m_document->indentAt(i_row);

    // move cursor from protected part
    i_column = qMax(i_column, indent * 2);

    if (m_document->m_settings->value(SettingsPage::KeyForcePressTextToLeft, SettingsPage::DefaultForcePressTextToLeft).toBool())
        i_column = justifyLeft(text);

    int textPos = i_column - indent * 2;
    m_document->undoStack()->push(new InsertCommand(m_document, this, m_analizer, i_row, textPos, text));

    int toLineUpdate = -1;

    emit updateRequest(fromLineUpdate, toLineUpdate);

    if (sel || bsel)
        m_document->undoStack()->endMacro();

    emitPositionChanged();
}

int TextCursor::justifyLeft(const QString &text) const
{

    if (!m_analizer || text.trimmed().isEmpty())
        return i_column;

    // Emulate text change and get line property

    const int indent = m_document->indentAt(i_row) * 2;
    int col = i_column;
    int textPos = col - indent;
    if (textPos<=0) {
        return i_column; // Nothing to do
    }
    QString s;
    if (m_document->linesCount()>i_row) {
        s = m_document->textAt(i_row);
    }
    s.insert(textPos, text);
    LineProp lp = m_analizer->lineProp(m_document->documentId, s);
    if (lp[textPos]==LxTypeComment || lp[textPos]==LxConstLiteral) {
        // Do not justify comments and literal constants
        return i_column;
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
    if (!b_enabled)
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
    b_visible = false;
    emit updateRequest();
    int fromLineUpdate = -1;
    int toLineUpdate = -1;

    const int indent = m_document->indentAt(i_row);
    int textPos = i_column - indent * 2;
    if (textPos>0) {
        // remove previous char in current line
        if ( i_row<m_document->linesCount() &&
             textPos <= m_document->textAt(i_row).length())
        {
            m_document->undoStack()->push(new RemoveCommand(m_document, this, m_analizer, i_row, textPos-1, 1, false));
        }
        else if (i_row<m_document->linesCount() && textPos>m_document->textAt(i_row).length()) {
            movePosition(QTextCursor::PreviousCell, TextCursor::MM_Move);
        }
        else {
            i_row --;
            i_column = 2 * m_document->indentAt(i_row);
        }
    }
    else {
        // remove current line and set cursor to end of previous line
        if (i_row>0) {
            if (i_row<m_document->linesCount()) {
                m_document->undoStack()->push(new RemoveCommand(m_document, this, m_analizer, i_row-1, m_document->textAt(i_row-1).length(), 1, false));
            }
        }
    }

    b_visible = true;
    emit updateRequest();
    emit updateRequest(fromLineUpdate, toLineUpdate);

    emitPositionChanged();
}

void TextCursor::removeCurrentLine()
{
    if (!b_enabled)
        return;
    if (hasSelection()) {
        removeSelectedText();
        emitPositionChanged();
        return;
    }

    if (i_row<m_document->linesCount()) {
        int column = i_column;
        movePosition(QTextCursor::EndOfBlock, MM_Move, 1);
        m_document->undoStack()->push(new RemoveCommand(m_document, this, m_analizer, i_row, 0, m_document->textAt(i_row).length()+1, true));
        i_column = column;
        emit updateRequest(-1, -1);
        emit updateRequest();
    }

    emitPositionChanged();
}

void TextCursor::removeLineTail()
{
    if (!b_enabled)
        return;
    if (hasSelection()) {
        removeSelectedText();
        emitPositionChanged();
        return;
    }

    if (i_row<m_document->linesCount()) {

        int textPos = i_column - m_document->indentAt(i_row)*2;
        if (textPos<m_document->textAt(i_row).length()) {
            m_document->undoStack()->push(new RemoveCommand(m_document, this, m_analizer, i_row, textPos, m_document->textAt(i_row).length()-textPos, true));
            emit updateRequest(-1, -1);
            emit updateRequest();
        }
    }

    emitPositionChanged();
}

void TextCursor::removeCurrentChar()
{
    // TODO Undo-redo stack!
    if (!b_enabled)
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
    b_visible = false;
    emit updateRequest();

    int fromLineUpdate = -1;
    int toLineUpdate = -1;

    const int indent = m_document->indentAt(i_row);
    int textPos = i_column - indent * 2;
    if (i_row>=m_document->linesCount())
        return;
    if (textPos>=m_document->textAt(i_row).length() && i_row>=m_document->linesCount()-1)
        return;
    m_document->undoStack()->push(new RemoveCommand(m_document, this, m_analizer, i_row, textPos, 1, true));

    b_visible = true;
    emit updateRequest();
    emit updateRequest(fromLineUpdate, toLineUpdate);

    emitPositionChanged();
}


void TextCursor::removeSelectedText()
{
    if (!b_enabled)
        return;

    if (!hasSelection())
        return;

    if (modifiesProtectedLiines())
        return;

    // Find where to place cursor after deletion

    int cursorStartLine = -1;
    int cursorTextPos = -1;

    for (int i=0; i<m_document->linesCount(); i++) {

        if (m_document->lineEndSelectedAt(i) && cursorStartLine==-1)
            cursorStartLine = i;
        QList<bool> sm = m_document->selectionMaskAt(i);
        for (int j=0; j<sm.size(); j++) {
            if (sm[j] && cursorTextPos==-1)
                cursorTextPos = j;
            if (sm[j] && cursorStartLine==-1)
                cursorStartLine = i;
        }
        if (cursorStartLine!=-1) {
            if (cursorTextPos==-1) {
                cursorTextPos = m_document->textAt(i).length();
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

    for (int i=0; i<m_document->linesCount(); i++) {
        int start = -1;
        int end = -1;
        QList<bool> sm = m_document->selectionMaskAt(i);
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
        if (m_document->lineEndSelectedAt(i)) {
            if (selectionLineStart==-1) {
                selectionLineStart = i;
                lineStart = i;
                posStart = m_document->textAt(i).length();
            }
            count ++;
            selectionLineEnd = qMax(i+1, selectionLineEnd);
        }
    }


    if (count>0) {
        m_document->undoStack()->push(new RemoveCommand(m_document, this, m_analizer, lineStart, posStart, count, true));
    }

    // Move cursor

    i_row = cursorStartLine;
    i_column = m_document->indentAt(i_row)*2 + cursorTextPos;

    removeSelection();

    emit updateRequest(-1, -1);
    emit updateRequest();

}

void TextCursor::removeSelectedBlock()
{
    if (!b_enabled)
        return;

    if (!hasRectSelection())
        return;

    int lineStart = rect_selection.top();
    int startPos = rect_selection.left();

    m_document->undoStack()->push(new RemoveBlockCommand(m_document, this, m_analizer, rect_selection));

    i_row = lineStart;
    i_column = startPos;
    rect_selection = QRect(-1, -1, 0, 0);
    emit updateRequest(-1, -1);
    emit updateRequest();

    emitPositionChanged();
}

bool TextCursor::hasSelection() const
{
    for (int i=0; i<m_document->linesCount(); i++) {
        if (m_document->lineEndSelectedAt(i))
            return true;
        if (m_document->selectionMaskAt(i).contains(true))
            return true;
    }
    return false;
}


QString TextCursor::selectedText() const
{
    QString result;
    if (hasSelection()) {
        for (int i=0; i<m_document->linesCount(); i++) {
            const QList<bool> sm = m_document->selectionMaskAt(i);
            const QString text = m_document->textAt(i);
//            Q_ASSERT(text.length()==sm.size());
            for (int j=0; j<qMin(text.length(), sm.size()); j++) {
                if (sm[j])
                    result += text[j];
            }
            if (m_document->lineEndSelectedAt(i) && i<m_document->linesCount()-1)
                result += "\n";
        }
    }
    else if (hasRectSelection()) {
        int startLine = rect_selection.top();
        int endLine = qMin(m_document->linesCount(), rect_selection.bottom()+1);
        int startPos = rect_selection.left();
        int endPos = rect_selection.right();
        for (int i=startLine; i<endLine; i++) {
            int indent = m_document->indentAt(i);
            int textStart = startPos - indent * 2;
            int textEnd = endPos - indent * 2;
            textStart = qMin(textStart, m_document->textAt(i).length());
            textEnd = qMin(textEnd, m_document->textAt(i).length());
            result += m_document->textAt(i).mid(textStart, textEnd-textStart);
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
        int startLine = rect_selection.top();
        int endLine = qMin(m_document->linesCount(), rect_selection.bottom()+1);
        int startPos = rect_selection.left();
        int endPos = rect_selection.right()+1;
        for (int i=startLine; i<endLine; i++) {
            int indent = m_document->indentAt(i);
            int textStart = startPos - indent * 2;
            int textEnd = endPos - indent * 2;
            textStart = qMin(textStart, m_document->textAt(i).length());
            textEnd = qMin(textEnd, m_document->textAt(i).length());
            textStart = qMax(0, textStart);
            textEnd = qMax(0, textEnd);
            QString text = m_document->textAt(i).mid(textStart, textEnd-textStart);
            int w = rect_selection.width();
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
    for (int i=0; i<m_document->linesCount(); i++) {
        if (m_document->selectionMaskAt(i).contains(true) || m_document->lineEndSelectedAt(i)) {
            if (fromRow==-1)
                fromRow = i;
        }
        if (!m_document->lineEndSelectedAt(i)) {
            if (fromRow!=-1) {
                toRow = i;
                break;
            }
        }
    }
    const QList<bool> first = m_document->selectionMaskAt(fromRow);
    fromCol = first.indexOf(true);
    if (fromCol==-1)
        fromCol = first.size();
    fromCol += 2 * m_document->indentAt(fromRow);

    if (toRow!=-1) {
        const QList<bool> last = m_document->selectionMaskAt(toRow);
        toCol = last.lastIndexOf(true);
        if (toCol==-1)
            toCol=0;
        toCol += 2 * m_document->indentAt(toRow);
    }
    else {
        toRow = m_document->linesCount();
        toCol = 0;
    }
}

void TextCursor::removeSelection()
{
    m_document->removeSelection();
    emit updateRequest(-1, -1);
    emitPositionChanged();

}

void TextCursor::undo()
{
//    int prevRow = i_row;
//    int prevLines = m_document->linesCount();
    if (b_enabled) {
        m_document->undoStack()->undo();
    }
//    if (prevRow!=i_row || prevLines!=m_document->linesCount()) {
        m_document->forceCompleteRecompilation();
//    }
    emit undoAvailable(b_enabled && m_document->undoStack()->canUndo());
    emit redoAvailable(b_enabled && m_document->undoStack()->canRedo());
}

void TextCursor::redo()
{
//    int prevRow = i_row;
//    int prevLines = m_document->linesCount();
    if (b_enabled) {
        m_document->undoStack()->redo();
    }
//    if (prevRow!=i_row || prevLines!=m_document->linesCount()) {
        m_document->forceCompleteRecompilation();
//    }
    emit undoAvailable(b_enabled && m_document->undoStack()->canUndo());
    emit redoAvailable(b_enabled && m_document->undoStack()->canRedo());
}

void TextCursor::handleRedoChanged(bool v)
{
    emit redoAvailable(b_enabled && v);
}

void TextCursor::handleUndoChanged(bool v)
{
    emit undoAvailable(b_enabled && v);
}

void TextCursor::setEnabled(bool v)
{
     b_enabled = v;
     emit updateRequest();
     emit undoAvailable(b_enabled && m_document->undoStack()->canUndo());
     emit redoAvailable(b_enabled && m_document->undoStack()->canRedo());
}

} // namespace Editor
