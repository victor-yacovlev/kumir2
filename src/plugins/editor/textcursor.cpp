#include "textcursor.h"
#include "textdocument.h"

namespace Editor {

TextCursor::TextCursor(TextDocument * document)
    : QObject(0)
    , m_document(document)
    , e_mode(EM_Insert)
    , i_timerId(-1)
    , b_enabled(true)
    , b_visible(true)
    , i_row(0)
    , i_column(0)
    , i_prevRow(-1)
    , i_prevCol(-1)
{
    i_timerId = startTimer(QApplication::cursorFlashTime()/2);
    emitPositionChanged();
}

void TextCursor::emitPositionChanged()
{
    bool compileRequest = l_removedLines.size()>1 || l_newLines.size()>1;
    if (i_prevRow!=i_row || i_prevCol!=i_column) {
        if (i_prevRow!=i_row && i_prevRow!=-1) {
            if (l_removedLines.size()>0 || l_newLines.size()>0) {
                compileRequest = true;
            }
        }
        i_prevRow = i_row;
        i_prevCol = i_column;
        emit positionChanged(i_row, i_column);
    }
    if (compileRequest) {
        emit lineAndTextChanged(l_removedLines, l_newLines);
        l_removedLines.clear();
        l_newLines.clear();
    }
}

void TextCursor::timerEvent(QTimerEvent *e)
{
    b_visible = !b_visible;
    emit updateRequest();
    e->accept();
}

TextCursor::~TextCursor()
{
    if (i_timerId!=-1)
        killTimer(i_timerId);
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
                if (i_row<m_document->size()) {
                    int textPos = i_column-indent;
                    if (textPos<0) {
                        i_column = indent;
                        textPos = 0;
                    }
                    if (textPos<m_document->at(i_row).text.length()) {
                        (*m_document)[i_row].selected[textPos] = ! (*m_document)[i_row].selected[textPos];
                        i_column++;
                    }
                    else {
                        if (i_row<m_document->size()) {
                            (*m_document)[i_row].lineEndSelected = ! (*m_document)[i_row].lineEndSelected;
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
                    if (i_row<m_document->size())
                        textPos = m_document->at(i_row).text.length();
                    i_column = indent + textPos;

                }
                else
                    i_column--;
            }
            else if (m==MM_Select) {
                int indent = m_document->indentAt(i_row)*2;
                if (i_column<=indent && i_row==0) {}
                else if (i_row>=m_document->size()) {
                    i_row = m_document->size()-1;
                    i_column = m_document->indentAt(i_row) * 2
                            + m_document->at(i_row).text.length();
                    (*m_document)[i_row].lineEndSelected = ! (*m_document)[i_row].lineEndSelected;
                }
                else if (i_row<m_document->size()
                         && i_column>indent+m_document->at(i_row).text.length())
                {
                    i_column = m_document->indentAt(i_row) * 2
                            + m_document->at(i_row).text.length()-1;
                    (*m_document)[i_row].selected.last() = ! (*m_document)[i_row].selected.last();
                }
                else if (i_column<=indent && i_row>0) {
                    (*m_document)[i_row-1].lineEndSelected = ! (*m_document)[i_row-1].lineEndSelected;
                    i_row --;
                    i_column = m_document->at(i_row).text.length() +
                            m_document->indentAt(i_row) * 2;
                }
                else {
                    int textPos = i_column - m_document->indentAt(i_row)*2;
                    if (textPos>0) {
                        (*m_document)[i_row].selected[textPos-1] = ! (*m_document)[i_row].selected[textPos-1];
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
                if (i_row<m_document->size()) {
                    (*m_document)[i_row].lineEndSelected = !(*m_document)[i_row].lineEndSelected ;
                    int textPos = i_column - m_document->indentAt(i_row);
                    if (textPos<0)
                        textPos = 0;
                    for (int i=textPos; i<m_document->at(i_row).text.length(); i++) {
                        (*m_document)[i_row].selected[i] = !(*m_document)[i_row].selected[i];
                    }

                    i_row++;
                    textPos = i_column - m_document->indentAt(i_row);
                    if (textPos<0)
                        textPos = 0;
                    if (i_row>=m_document->size())
                        textPos = 0;
                    else if (textPos>m_document->at(i_row).text.length())
                        textPos = m_document->at(i_row).text.length();
                    for (int i=0; i<textPos; i++) {
                        (*m_document)[i_row].selected[i] = !(*m_document)[i_row].selected[i];
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
                        int textPos = i_column - m_document->indentAt(i_row);
                        for (int i=0; i<qMin(textPos, m_document->at(i_row).text.length()); i++) {
                            (*m_document)[i_row].selected[i] = !(*m_document)[i_row].selected[i];
                        }
                        i_column = m_document->indentAt(0)*2;
                    }
                    else {
                        int textPos = i_column - m_document->indentAt(i_row);
                        if (i_row<m_document->size()) {
                            for (int i=0; i<qMin(textPos, m_document->at(i_row).text.length()); i++) {
                                (*m_document)[i_row].selected[i] = !(*m_document)[i_row].selected[i];
                            }
                            i_row --;
                        }
                        else {
                            i_row = m_document->size()-1;
                        }
                        textPos = i_column - m_document->indentAt(i_row);
                        if (textPos<0)
                            textPos = 0;
                        for (int i=textPos; i<m_document->at(i_row).text.length(); i++) {
                            (*m_document)[i_row].selected[i] = !(*m_document)[i_row].selected[i];
                        }
                        (*m_document)[i_row].lineEndSelected = ! (*m_document)[i_row].lineEndSelected;
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
                if (i_row>=m_document->size())
                    i_column = 0;
                else
                    i_column = m_document->indentAt(i_row) * 2;
            }
            else if (m==MM_Select) {
                if (i_row<m_document->size()) {
                    int textPos = i_column - m_document->indentAt(i_row);
                    textPos = qMax(0, textPos);
                    textPos = qMin(m_document->at(i_row).text.length(), textPos);
                    for (int i=0; i<textPos; i++) {
                        (*m_document)[i_row].selected[i] = !(*m_document)[i_row].selected[i];
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
                if (i_row>=m_document->size()) {}
                else
                    i_column = m_document->indentAt(i_row) * 2 +
                            m_document->at(i_row).text.length();
            }
            else if (m==MM_Select) {
                if (i_row<m_document->size()) {
                    int textPos = i_column - m_document->indentAt(i_row);
                    textPos = qMax(0, textPos);
                    for (int i=textPos; i<m_document->at(i_row).text.length(); i++) {
                        (*m_document)[i_row].selected[i] = !(*m_document)[i_row].selected[i];
                    }
                    i_column = m_document->indentAt(i_row) * 2 +
                            m_document->at(i_row).text.length();
                }
            }
        }
        else if (op==QTextCursor::Start) {
            if (m==MM_Move) {
                i_row = 0;
                i_column = 0;
            }
            else if (m==MM_Select) {
                if (i_row<m_document->size()) {
                    int textPos = i_column - m_document->indentAt(i_row);
                    textPos = qMax(0, textPos);
                    textPos = qMin(m_document->at(i_row).text.length(), textPos);
                    for (int i=0; i<textPos; i++) {
                        (*m_document)[i_row].selected[i] = !(*m_document)[i_row].selected[i];
                    }
                    i_row--;
                }
                else {
                    i_row = m_document->size()-1;
                }
                for ( ; i_row>=0; i_row-- ) {
                    for (int i=0; i<m_document->at(i_row).selected.size(); i++) {
                        (*m_document)[i_row].selected[i] = !(*m_document)[i_row].selected[i];
                    }
                    (*m_document)[i_row].lineEndSelected = !(*m_document)[i_row].lineEndSelected;
                }
                i_column = 0;
                i_row = 0;
            }
        }
        else if (op==QTextCursor::End) {
            if (m==MM_Move) {
                i_row = m_document->size()-1;
                i_column = m_document->indentAt(i_row)*2 +
                        m_document->last().text.length();
            }
            else if (m==MM_Select) {
                if (i_row>=m_document->size()) {
                    i_row = m_document->size()-1;
                    i_column = m_document->indentAt(i_row)*2 +
                            m_document->last().text.length();
                }
                else {
                    int textPos = i_column - m_document->indentAt(i_row);
                    textPos = qMax(0, textPos);
                    for (int i=textPos; i<m_document->at(i_row).text.length(); i++) {
                        (*m_document)[i_row].selected[i] = !(*m_document)[i_row].selected[i];
                    }
                    (*m_document)[i_row].lineEndSelected = !(*m_document)[i_row].lineEndSelected;
                    i_row ++;
                    for ( ; i_row<m_document->size(); i_row++ ) {
                        for (int i=0; i<m_document->at(i_row).selected.size(); i++) {
                            (*m_document)[i_row].selected[i] = !(*m_document)[i_row].selected[i];
                        }
                        if (i_row<m_document->size()-1) {
                            (*m_document)[i_row].lineEndSelected = !(*m_document)[i_row].lineEndSelected;
                        }
                    }
                    i_row = m_document->size()-1;
                    i_column = m_document->indentAt(i_row)*2 +
                            m_document->last().text.length();
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
    // TODO Undo-redo stack!
    if (hasSelection())
        removeSelectedText();
    if (hasRectSelection())
        removeSelectedBlock();

    for (int y=i_row; y<i_row+block.size(); y++) {
        int indent = m_document->indentAt(y);
        int textStart = i_column - indent;
        int textEnd = i_column + block[y-i_row].length();
        if (y<m_document->size()) {
            TextLine line = m_document->at(y);
            QString text = line.text.mid(0, textStart).leftJustified(textStart, ' ');

            QList<Shared::LexemType> highlight =
                    line.highlight.mid(0, qMin(textStart, line.highlight.size()));
            QList<bool> selected =
                    line.selected.mid(0, qMin(textStart, line.selected.size()));
            for (int x=line.highlight.size(); x<textStart; x++) {
                highlight << Shared::LxTypeEmpty;
                selected << false;
            }

            text += block[y-i_row];
            for (int x=0; x<block[y-i_row].size(); x++) {
                highlight << Shared::LxTypeEmpty;
                selected << false;
            }
            if (textEnd+1<line.text.size()) {
                text += line.text.mid(textEnd);
            }

            if (textEnd+1<line.highlight.size()) {
                highlight += line.highlight.mid(textEnd);
            }
            if (textEnd+1<line.selected.size()) {
                selected += line.selected.mid(textEnd);
            }
            line.text = text;
            line.highlight = highlight;
            line.selected = selected;
            (*m_document)[y] = line;
        }
        else {
            TextLine line;
            line.indentStart = line.indentEnd = 0;
            line.lineEndSelected = false;
            line.text.fill(' ', textStart);
            for (int x=0; x<textStart; x++) {
                line.highlight << Shared::LxTypeEmpty;
                line.selected << false;
            }
            line.text += block[y-i_row];
            for (int x=0; x<block[y-i_row].length(); x++) {
                line.highlight << Shared::LxTypeEmpty;
                line.selected << false;
            }
            m_document->append(line);
        }
    }
    i_row += block.size()-1;
    i_column += block[0].length();
    emit updateRequest(-1, -1);
    emitPositionChanged();
}

void TextCursor::clearSelectedBlock()
{

}

void TextCursor::insertText(const QString &text)
{
    if (!b_enabled)
        return;
    // TODO Undo-redo stack!
    if (hasSelection()) {
        removeSelectedText();
    }
    if (hasRectSelection()) {
        removeSelectedBlock();
    }

    int fromLineUpdate = i_row;
    while (i_row>=m_document->size()) {
        TextLine textLine;
        textLine.indentStart = 0;
        textLine.indentEnd = 0;
        textLine.lineEndSelected = false;
        if (!l_newLines.contains(m_document->size()))
            l_newLines << m_document->size();
        m_document->append(textLine);
    }
    const int indent = m_document->indentAt(i_row);

    // move cursor from protected part
    i_column = qMax(i_column, indent * 2);

    // calculate next indent size in case of line insert
    int nextIndent = indent;
    if (i_row<m_document->size())
        nextIndent += m_document->at(i_row).indentEnd;

    int textPos = i_column - indent * 2;

    if (i_row<m_document->size() && !l_removedLines.contains(i_row))
        l_removedLines << i_row;
    if (i_row<m_document->size() && !l_newLines.contains(i_row))
        l_newLines << i_row;

    // fill with spaces if need
    if (m_document->at(i_row).text.length()<textPos) {
        TextLine line = m_document->at(i_row);
        int spacesCount = textPos - line.text.size();
        for (int i=0; i<spacesCount; i++) {
            line.text += " ";
            line.highlight << Shared::LxTypeEmpty;
            line.selected << false;
        }
        (*m_document)[i_row] = line;
    }

    QStringList lines = text.split("\n");
    for (int i=1; i<lines.count(); i++) {
        if (!l_newLines.contains(i_row+i)) {
            l_newLines << i_row+i;
        }
    }

    // detect highlight at current cursor position
    Shared::LexemType curType = Shared::LxTypeEmpty;
    if (textPos>0 && textPos-1 < m_document->at(i_row).highlight.size())
        curType = m_document->at(i_row).highlight[textPos-1];
    else if (textPos==0 && m_document->at(i_row).highlight.size()>0)
        curType = m_document->at(i_row).highlight[0];

    // cut and store line tail
    QString lastText;
    QList<Shared::LexemType> lastHighlight;

    if (textPos < m_document->at(i_row).text.length()) {
        TextLine textLine = m_document->at(i_row);
        lastText = textLine.text.mid(textPos);
        lastHighlight = textLine.highlight.mid(textPos);
        textLine.text = textLine.text.left(textPos);
        textLine.highlight = textLine.highlight.mid(0, textPos);
        textLine.selected = textLine.selected.mid(0, textPos);
        (*m_document)[i_row] = textLine;
    }

    // insert head line
    (*m_document)[i_row].text += lines[0];
    for (int i=0; i<lines[0].size(); i++) {
        (*m_document)[i_row].highlight << curType;
        (*m_document)[i_row].selected << false;
    }
    i_column += lines[0].length();

    // insert tail lines
    for (int i=1; i<lines.size(); i++) {
        i_row ++;
        i_column = nextIndent * 2 + lines[i].length();
        TextLine textLine;
        textLine.indentStart = 0;
        textLine.indentEnd = 0;
        textLine.lineEndSelected = false;
        textLine.text = lines[i];
        for (int j=0; j<lines[i].length(); j++) {
            textLine.highlight << Shared::LxTypeEmpty;
            textLine.selected << false;
        }
        m_document->insert(i_row, textLine);
    }

    // insert remainder
    (*m_document)[i_row].text += lastText;
    for (int i=0; i<lastHighlight.size(); i++) {
        (*m_document)[i_row].highlight << lastHighlight[i];
        (*m_document)[i_row].selected << false;
    }


    int toLineUpdate = lines.size()>0? -1 : i_row;

    emit updateRequest(fromLineUpdate, toLineUpdate);
    emitPositionChanged();
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
        if ( i_row<m_document->size() &&
                textPos <= m_document->at(i_row).text.length())
        {
            TextLine curTextLine = m_document->at(i_row);
            (*m_document)[i_row].text =
                    curTextLine.text.left(textPos-1) +
                    curTextLine.text.mid(textPos);

            (*m_document)[i_row].highlight =
                    curTextLine.highlight.mid(0,textPos-1) +
                    curTextLine.highlight.mid(textPos);

            (*m_document)[i_row].selected.pop_back();
            if (!l_removedLines.contains(i_row)) {
                l_removedLines << i_row;
            }
            if (!l_newLines.contains(i_row)) {
                l_newLines << i_row;
            }
        }
        i_column --;
    }
    else {
        // remove current line and set cursor to end of previous line
        if (i_row>0) {
            if (i_row<m_document->size()) {
                toLineUpdate = -1;
                TextLine curTextLine = m_document->at(i_row);
                i_column = m_document->indentAt(i_row-1)*2 +
                        m_document->at(i_row-1).text.length();
                (*m_document)[i_row-1].text += curTextLine.text;
                (*m_document)[i_row-1].highlight += curTextLine.highlight;
                (*m_document)[i_row-1].indentEnd += curTextLine.indentStart + curTextLine.indentEnd;
                m_document->removeAt(i_row);
                i_row--;
                if (!l_removedLines.contains(i_row)) {
                    l_removedLines << i_row;
                }
                if (!l_removedLines.contains(i_row+1)) {
                    l_removedLines << i_row+1;
                }
                if (!l_newLines.contains(i_row)) {
                    l_newLines << i_row;
                }
            }
            else {
                i_row--;
                i_column = m_document->indentAt(i_row)*2;
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

    if (i_row<m_document->size()) {
        if (!l_removedLines.contains(i_row)) {
            l_removedLines << i_row;
        }
        m_document->removeAt(i_row);
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

    if (i_row<m_document->size()) {
        if (!l_removedLines.contains(i_row)) {
            l_removedLines << i_row;
        }
        if (!l_newLines.contains(i_row)) {
            l_newLines << i_row;
        }
        int textPos = i_column - m_document->indentAt(i_row)*2;
        if (textPos<m_document->at(i_row).text.length()) {
            (*m_document)[i_row].text = (*m_document)[i_row].text.mid(0, textPos);
            (*m_document)[i_row].highlight = (*m_document)[i_row].highlight.mid(0, textPos);
            (*m_document)[i_row].selected = (*m_document)[i_row].selected.mid(0, textPos);
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
    if (i_row<m_document->size() && textPos<m_document->at(i_row).text.length()) {
        // remove current char in current line
        TextLine curTextLine = m_document->at(i_row);
        (*m_document)[i_row].text =
                curTextLine.text.left(textPos) +
                curTextLine.text.mid(textPos+1);

        (*m_document)[i_row].highlight =
                curTextLine.highlight.mid(0,textPos) +
                curTextLine.highlight.mid(textPos+1);

        (*m_document)[i_row].selected.pop_back();
        if (!l_removedLines.contains(i_row)) {
            l_removedLines << i_row;
        }
        if (!l_newLines.contains(i_row)) {
            l_newLines << i_row;
        }

    }
    else if (i_row<m_document->size()-1) {
        // remove line delimeter if exists
        toLineUpdate = -1;
        TextLine curTextLine = m_document->at(i_row);
        if (!l_removedLines.contains(i_row)) {
            l_removedLines << i_row;
        }
        if (!l_removedLines.contains(i_row+1)) {
            l_removedLines << i_row+1;
        }
        if (!l_newLines.contains(i_row)) {
            l_newLines << i_row;
        }
        // if cursor far away from end of line -- fill by spaces
        while (curTextLine.text.length() < textPos) {
            curTextLine.text += " ";
            curTextLine.highlight << Shared::LxTypeEmpty;
            curTextLine.selected << false;
        }
        TextLine nextTextLine = m_document->at(i_row+1);
        curTextLine.text += nextTextLine.text;
        curTextLine.highlight += nextTextLine.highlight;
        curTextLine.selected += nextTextLine.selected;
        curTextLine.indentEnd += nextTextLine.indentStart + nextTextLine.indentEnd;
        (*m_document)[i_row] = curTextLine;
        m_document->removeAt(i_row+1);
    }

    b_visible = true;
    emit updateRequest();
    emit updateRequest(fromLineUpdate, toLineUpdate);
    emitPositionChanged();
}

void TextCursor::clearUndoRedoStacks()
{
    // TODO implement me
}


bool TextCursor::isModified() const
{
    // TODO implement me
    return true;
}

void TextCursor::removeSelectedText()
{
    if (!b_enabled)
        return;

    if (!hasSelection())
        return;

    // Find where to place cursor after deletion

    int cursorStartLine = -1;
    int cursorTextPos = -1;

    for (int i=0; i<m_document->size(); i++) {
        const TextLine line = m_document->at(i);
        if (line.lineEndSelected && cursorStartLine==-1)
            cursorStartLine = i;
        for (int j=0; j<line.selected.size(); j++) {
            if (line.selected[j] && cursorTextPos==-1)
                cursorTextPos = j;
            if (line.selected[j] && cursorStartLine==-1)
                cursorStartLine = i;
        }
        if (cursorStartLine!=-1) {
            if (cursorTextPos==-1) {
                cursorTextPos = line.text.length();
            }
            break;
        }
    }

    // Remove selected text inside lines

    int selectionLineStart = -1;
    int selectionLineEnd = -1;

    for (int i=0; i<m_document->size(); i++) {
        int start = -1;
        int end = -1;
        for (int j=0; j<m_document->at(i).selected.size(); j++) {
            bool v = m_document->at(i).selected[j];
            if (v) {
                if (selectionLineStart==-1) {
                    selectionLineStart = i;
                }
                selectionLineEnd = qMax(selectionLineEnd, i);
                if (start==-1) {
                    start = j;
                }
                end = qMax(j, end);
            }
            if (m_document->at(i).lineEndSelected) {
                if (selectionLineStart==-1)
                    selectionLineStart = i;
                selectionLineEnd = qMax(i+1, selectionLineEnd);
            }
        }
        if (start!=-1) {
            (*m_document)[i].text = (*m_document)[i].text.mid(0, start)
                    + (*m_document)[i].text.mid(end+1);
            (*m_document)[i].highlight = (*m_document)[i].highlight.mid(0, start)
                    + (*m_document)[i].highlight.mid(end+1);
            (*m_document)[i].selected = (*m_document)[i].selected.mid(0, start)
                    + (*m_document)[i].selected.mid(end+1);
        }
    }


    // Remove lines

    QList<TextLine>::iterator it = m_document->begin();
    bool removeNextLine = false;
    while (it!=m_document->end()) {
        if (removeNextLine) {
            removeNextLine = (*it).lineEndSelected;
            QList<TextLine>::iterator prev = it - 1;
            (*prev).text += (*it).text;
            (*prev).highlight += (*it).highlight;
            (*prev).selected += (*it).selected;
            (*prev).indentEnd += (*it).indentStart + (*it).indentEnd;
            (*prev).lineEndSelected = false;
            it = m_document->erase(it);
        }
        else {
            removeNextLine = (*it).lineEndSelected;
            it ++;
        }
    }


    for (int i=selectionLineStart; i<=selectionLineEnd; i++) {
        if (!l_removedLines.contains(i)) {
            l_removedLines << i;
        }
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
    int lineEnd = rect_selection.bottom();
    int startPos = rect_selection.left();
    int endPos = rect_selection.right();

    for (int i=lineStart; i<=lineEnd; i++) {
        if (!l_removedLines.contains(i))
            l_removedLines << i;
    }

    lineEnd = qMin(lineEnd+1, m_document->size());

    for (int y=0; y<lineEnd; y++) {
        int indent = m_document->indentAt(y)*2;
        int textStart = startPos - indent;
        int textEnd = endPos - indent + 1;
        TextLine line = m_document->at(y);
        textStart = qMin(textStart, line.text.length());
        textEnd = qMin(textEnd, line.text.length());

        QString text = line.text.mid(0, textStart);
        if (textEnd < line.text.length())
            text += line.text.mid(textEnd);
        QList<Shared::LexemType> highlight = line.highlight.mid(0, textStart);
        if (textEnd < line.highlight.size())
            highlight += line.highlight.mid(textEnd);
        QList<bool> selected = line.selected.mid(0, textStart);
        if (textEnd < line.selected.size())
            selected += line.selected.mid(textEnd);

        line.text = text;
        line.highlight = highlight;
        line.selected = selected;
        if (line.text.trimmed().isEmpty())
            line.indentStart = line.indentEnd = 0;

        (*m_document)[y] = line;
    }
    i_row = lineStart;
    i_column = startPos;
    rect_selection = QRect(-1, -1, 0, 0);
    emit updateRequest(-1, -1);
    emit updateRequest();
    emitPositionChanged();
}

bool TextCursor::hasSelection() const
{
    for (int i=0; i<m_document->size(); i++) {
        if (m_document->at(i).lineEndSelected)
            return true;
        for (int j=0; j<m_document->at(i).selected.size(); j++) {
            if (m_document->at(i).selected[j])
                return true;
        }
    }
    return false;
}


QString TextCursor::selectedText() const
{
    QString result;
    if (hasSelection()) {
        for (int i=0; i<m_document->size(); i++) {
            for (int j=0; j<m_document->at(i).text.length(); j++) {
                if (m_document->at(i).selected[j])
                    result += m_document->at(i).text[j];
            }
            if (m_document->at(i).lineEndSelected)
                result += "\n";
        }
    }
    else if (hasRectSelection()) {
        int startLine = rect_selection.top();
        int endLine = qMin(m_document->size(), rect_selection.bottom()+1);
        int startPos = rect_selection.left();
        int endPos = rect_selection.right();
        for (int i=startLine; i<endLine; i++) {
            TextLine line = m_document->at(i);
            int indent = m_document->indentAt(i);
            int textStart = startPos - indent * 2;
            int textEnd = endPos - indent * 2;
            textStart = qMin(textStart, line.text.length());
            textEnd = qMin(textEnd, line.text.length());
            result += line.text.mid(textStart, textEnd-textStart);
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
        int endLine = qMin(m_document->size(), rect_selection.bottom()+1);
        int startPos = rect_selection.left();
        int endPos = rect_selection.right()+1;
        for (int i=startLine; i<endLine; i++) {
            TextLine line = m_document->at(i);
            int indent = m_document->indentAt(i);
            int textStart = startPos - indent * 2;
            int textEnd = endPos - indent * 2;
            textStart = qMin(textStart, line.text.length());
            textEnd = qMin(textEnd, line.text.length());
            textStart = qMax(0, textStart);
            textEnd = qMax(0, textEnd);
            QString text = line.text.mid(textStart, textEnd-textStart);
            int w = rect_selection.width();
            result << text.leftJustified(w, ' ');
        }
    }
    return result;
}

void TextCursor::removeSelection()
{
    bool wasSelection = false;
    for (int i=0; i<m_document->size(); i++) {
        wasSelection = wasSelection || (*m_document)[i].lineEndSelected;
        (*m_document)[i].lineEndSelected = false;
        for (int j=0; j<m_document->at(i).text.length(); j++) {
            wasSelection = wasSelection || (*m_document)[i].selected[j];
            (*m_document)[i].selected[j] = false;
        }
    }
    if (wasSelection) {
        emit updateRequest(-1, -1);
        emitPositionChanged();
    }
}

} // namespace Editor
