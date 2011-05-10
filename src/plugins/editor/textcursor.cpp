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
    , p_selectionStart(QPoint(-1,-1))
    , p_selectionEnd(QPoint(-1,-1))
{
    i_timerId = startTimer(QApplication::cursorFlashTime()/2);
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

bool TextCursor::movePosition(QTextCursor::MoveOperation op, QTextCursor::MoveMode m, int n)
{
    b_visible = false;
    updateRequest();
    bool result = true;
    if (m==QTextCursor::KeepAnchor) {
        if (p_selectionStart.y()==-1 || p_selectionEnd.y()==-1) {

        }
    }
    else {
        p_selectionStart = QPoint(-1,-1);
        p_selectionEnd = QPoint(-1,-1);
    }
    for (int i=0; i<n; i++) {
        if (op==QTextCursor::NextCell) {
            i_column ++;
            if (m==QTextCursor::KeepAnchor) {

            }
        }
        else if (op==QTextCursor::PreviousCell) {
            if (i_column==0 && i_row==0)
                result = false;
            else if (i_column==0 && i_row>0) {
                i_row --;
                i_column = m_document->at(i_row).text.length() +
                        m_document->indentAt(i_row) * 2;
            }
            else
                i_column--;
        }
        else if (op==QTextCursor::NextRow) {
            i_row++;
        }
        else if (op==QTextCursor::PreviousRow) {
            if (i_row==0)
                result = false;
            else {
                i_row--;
            }
        }
        else if (op==QTextCursor::StartOfBlock) {
            if (i_row>=m_document->size())
                i_column = 0;
            else
                i_column = m_document->indentAt(i_row) * 2;
        }
        else if (op==QTextCursor::EndOfBlock) {
            if (i_row>=m_document->size())
                result = false;
            else
                i_column = m_document->indentAt(i_row) * 2 +
                        m_document->at(i_row).text.length();
        }
        else if (op==QTextCursor::Start) {
            i_row = 0;
            i_column = 0;
        }
        else if (op==QTextCursor::End) {
            i_row = m_document->size()-1;
            i_column = m_document->indentAt(i_row)*2 +
                    m_document->last().text.length();
        }
    }
    b_visible = true;
    emit updateRequest();
    if (m==QTextCursor::KeepAnchor) {
        emit updateRequest(-1, -1);
    }
    return result;
}

bool TextCursor::insertText(const QString &text)
{
    if (!b_enabled)
        return false;
    // TODO Undo-redo stack!
    bool result = true;
    int fromLineUpdate = i_row;
    while (i_row>=m_document->size()) {
        TextLine textLine;
        textLine.indentStart = 0;
        textLine.indentEnd = 0;
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

    // fill with spaces if need
    if (m_document->at(i_row).text.length()<textPos) {
        TextLine line = m_document->at(i_row);
        int spacesCount = textPos - line.text.size();
        for (int i=0; i<spacesCount; i++) {
            line.text += " ";
            line.highlight << Shared::LxTypeEmpty;
        }
        (*m_document)[i_row] = line;
    }

    QStringList lines = text.split("\n");

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
        (*m_document)[i_row] = textLine;
    }

    // insert head line
    (*m_document)[i_row].text += lines[0];
    for (int i=0; i<lines[0].size(); i++) {
        (*m_document)[i_row].highlight << curType;
    }
    i_column += lines[0].length();

    // insert tail lines
    for (int i=1; i<lines.size(); i++) {
        i_row ++;
        i_column = nextIndent * 2 + lines[i].length();
        TextLine textLine;
        textLine.indentStart = 0;
        textLine.indentEnd = 0;
        textLine.text = lines[i];
        for (int j=0; j<lines[i].length(); j++) {
            textLine.highlight << Shared::LxTypeEmpty;
        }
        m_document->insert(i_row, textLine);
    }

    // insert remainder
    (*m_document)[i_row].text += lastText;
    for (int i=0; i<lastHighlight.size(); i++) {
        (*m_document)[i_row].highlight << lastHighlight[i];
    }


    int toLineUpdate = lines.size()>0? -1 : i_row;

    emit updateRequest(fromLineUpdate, toLineUpdate);
    return result;
}

bool TextCursor::removePreviousChar()
{
    // TODO Undo-redo stack!
    if (!b_enabled)
        return false;
    b_visible = false;
    emit updateRequest();
    bool result = true;
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
            }
            else {
                i_row--;
                i_column = m_document->indentAt(i_row)*2;
            }
        }
        else {
            // can't backspace in (0, 0)
            result = false;
        }
    }

    b_visible = true;
    emit updateRequest();
    emit updateRequest(fromLineUpdate, toLineUpdate);
    return result;
}

bool TextCursor::removeCurrentChar()
{
    // TODO Undo-redo stack!
    if (!b_enabled)
        return false;
    b_visible = false;
    emit updateRequest();
    bool result = true;
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
    }
    else if (i_row<m_document->size()-1) {
        // remove line delimeter if exists
        toLineUpdate = -1;
        TextLine curTextLine = m_document->at(i_row);
        // if cursor far away from end of line -- fill by spaces
        while (curTextLine.text.length() < textPos) {
            curTextLine.text += " ";
            curTextLine.highlight << Shared::LxTypeEmpty;
        }
        TextLine nextTextLine = m_document->at(i_row+1);
        curTextLine.text += nextTextLine.text;
        curTextLine.highlight += nextTextLine.highlight;
        curTextLine.indentEnd += nextTextLine.indentStart + nextTextLine.indentEnd;
        (*m_document)[i_row] = curTextLine;
        m_document->removeAt(i_row+1);
    }
    else {
        // nothing to delete
        result = false;
    }

    b_visible = true;
    emit updateRequest();
    emit updateRequest(fromLineUpdate, toLineUpdate);
    return result;
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

QString TextCursor::selectedText() const
{
    if (!hasSelection())
        return "";
    return ""; // TODO implement me
}

} // namespace Editor
