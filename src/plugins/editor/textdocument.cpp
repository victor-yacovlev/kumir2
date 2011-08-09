#include "textdocument.h"
#include "textcursor.h"

namespace Editor
{

InsertCommand::InsertCommand(TextDocument *doc, class TextCursor * cursor, Shared::AnalizerInterface *analizer, int line, int pos, const QString &text)
{
    this->doc = doc;
    this->cursor = cursor;
    this->analizer = analizer;
    this->line = line;
    this->pos = pos;
    this->text = text;
    blankLines = blankChars = 0;
}

void InsertCommand::redo()
{
    cursorRow = cursor->row();
    cursorCol = cursor->column();
    doc->insertText(text, analizer, line, pos, blankLines, blankChars);
    QStringList lines = text.split("\n", QString::KeepEmptyParts);
    if (lines.size()>1) {
        cursor->setRow(cursor->row()+lines.size()-1);
        cursor->setColumn(lines.last().length() + doc->indentAt(cursor->row()));
    }
    else {
        cursor->setColumn(cursor->column()+text.length());
    }
    doc->checkForCompilationRequest(QPoint(cursor->row(), cursor->column()));
}

void InsertCommand::undo()
{
    QString txt;
    doc->removeText(txt, analizer, line, pos, blankLines, blankChars, text.length());
    Q_ASSERT(txt==text);
    cursor->setRow(cursorRow);
    cursor->setColumn(cursorCol);
    doc->checkForCompilationRequest(QPoint(cursor->row(), cursor->column()));
}

bool InsertCommand::mergeWith(const QUndoCommand *other)
{
    const InsertCommand * o = static_cast<const InsertCommand*>(other);
    Q_CHECK_PTR(o);
    if (o->line!=line)
        return false;
    if (o->pos-pos==text.length() || o->pos+o->text.length()==pos) {
        static const QString delimeters = QString::fromAscii(" |\n!@*()[],+-=:;/<>\"'{}");
        for (int i=0; i<text.length(); i++) {
            if (delimeters.contains(text[i]))
                return false;
        }
        for (int i=0; i<o->text.length(); i++) {
            if (delimeters.contains(o->text[i]))
                return false;
        }
        if (o->pos==pos+text.length()) {
            // Append other
            text += o->text;
        }
        else {
            // Prepend other
            blankLines = o->blankLines;
            blankChars = o->blankChars;
            pos = o->pos;
            text = o->text + text;
        }
        return true;
    }
    else {
        return false;
    }
}

RemoveCommand::RemoveCommand(
    TextDocument *doc,
    class TextCursor * cursor,
    Shared::AnalizerInterface *analizer,
    int line, int pos, int count, bool keepCursor)
{
    this->doc = doc;
    this->cursor = cursor;
    this->analizer = analizer;
    this->line = line;
    this->pos = pos;
    this->count = count;
    this->keepKursor = keepCursor;
}

void RemoveCommand::redo()
{
    cursorRow = cursor->row();
    cursorCol = cursor->column();
    removedText.clear();
    doc->removeText(removedText, analizer, line, pos, 0, 0, count);
    if (!keepKursor) {
        for (int i=0; i<count; i++) {
            int indent = doc->indentAt(cursor->row()) * 2;
            int textPos = cursor->column()-indent;
            if (textPos>0) {
                cursor->setColumn(cursor->column()-1);
            }
            else if (cursor->row()>0) {
                cursor->setRow(cursor->row()-1);
                cursor->setColumn(doc->indentAt(cursor->row())*2+doc->data[cursor->row()].text.length());
            }
        }
    }
    doc->checkForCompilationRequest(QPoint(cursor->row(), cursor->column()));
}

void RemoveCommand::undo()
{
    int blankLines, blankChars;
    doc->insertText(removedText, analizer, line, pos, blankLines, blankChars);
    cursor->setRow(cursorRow);
    cursor->setColumn(cursorCol);
    doc->checkForCompilationRequest(QPoint(cursor->row(), cursor->column()));
}

bool RemoveCommand::mergeWith(const QUndoCommand *other)
{
    const RemoveCommand * o = static_cast<const RemoveCommand*>(other);
    if (o->line!=line || keepKursor!=o->keepKursor)
        return false;
    if (pos+count==o->pos || o->pos+o->count==pos) {
        static const QString delimeters = QString::fromAscii(" |\n!@*()[],+-=:;/<>\"'{}");
        for (int i=0; i<removedText.length(); i++) {
            if (delimeters.contains(removedText[i]))
                return false;
        }
        for (int i=0; i<o->removedText.length(); i++) {
            if (delimeters.contains(o->removedText[i]))
                return false;
        }
        if (pos+count==o->pos) {
            // "Append"
            count += o->count;
            removedText += o->removedText;
        }
        else {
            // "Prepend"
            pos = o->pos;
            count += o->count;
            removedText = o->removedText + removedText;
        }
//        if (!keepKursor) {
//            cursorRow = qMin(cursorRow, o->cursorRow);
//            cursorCol = qMin(cursorCol, o->cursorCol);
//        }
        return true;
    }
    else {
        return false;
    }
}

TextDocument::TextDocument(QObject *parent)
    : QObject(parent)
    , documentId(-1)
    , m_undoStack(new QUndoStack(this))
{

}

void TextDocument::insertText(const QString &text, const Shared::AnalizerInterface * analizer, int line, int pos, int &blankLines, int &blankChars)
{
    blankLines = blankChars = 0;
    while (data.size()<=line) {
        blankLines ++;
        TextLine tl;
        data.append(tl);
        m_newLines.insert(data.size()-1);
    }
    TextLine tl = data[line];
    while (pos>tl.text.length()) {
        blankChars ++;
        tl.text.append(' ');
        tl.selected << false;
        tl.highlight << Shared::LxTypeEmpty;
    }
    data[line] = tl;
    m_removedLines.insert(line);
    m_newLines.insert(line);
    const QStringList lines = text.split("\n", QString::KeepEmptyParts);
    if (lines.size()==1) {
        // Insert text fragment into line
        data[line].text.insert(pos, lines[0]);
        while (data[line].selected.size() < data[line].text.length())
            data[line].selected << false;
        while (data[line].highlight.size() < data[line].text.length())
            data[line].highlight << Shared::LxTypeEmpty;
        if (analizer)
            data[line].highlight = analizer->lineProp(documentId, data[line].text).toList();

    }
    else {
        // 1. Append fragment to first line
        data[line].text += lines[0];
        while (data[line].selected.size() < data[line].text.length())
            data[line].selected << false;
        while (data[line].highlight.size() < data[line].text.length())
            data[line].highlight << Shared::LxTypeEmpty;
        if (analizer)
            data[line].highlight = analizer->lineProp(documentId, data[line].text).toList();

        // 2. Insert middle lines
        for (int i=lines.count()-2; i>=1; i--) {
            TextLine tl;
            tl.text = lines[i];
            for (int j=0; j<tl.text.length(); j++) {
                tl.selected << false;
                tl.highlight << Shared::LxTypeEmpty;
            }
            if (analizer)
                tl.highlight = analizer->lineProp(documentId, tl.text).toList();
            data.insert(line+1, tl);
            m_newLines.insert(i);
        }

        // 3. Prepend fragment to last line
        if (line+lines.count()-1 >= data.size()) {
            TextLine last;
            data.append(last);
        }
        m_removedLines.insert(line+1);
        m_newLines.insert(line);
        data[line+lines.count()-1].text.prepend(lines.last());
        while (data[line+lines.count()-1].selected.size() < data[line+lines.count()-1].text.length())
            data[line+lines.count()-1].selected << false;
        while (data[line+lines.count()-1].highlight.size() < data[line+lines.count()-1].text.length())
            data[line+lines.count()-1].highlight << Shared::LxTypeEmpty;
        if (analizer)
            data[line+lines.count()-1].highlight = analizer->lineProp(documentId, data[line+lines.count()-1].text).toList();
    }
}

InsertBlockCommand::InsertBlockCommand(
    TextDocument *doc,
    TextCursor *cursor,
    Shared::AnalizerInterface *analizer,
    int row, int column,
    const QStringList &block)
{
    this->block = block;
    this->row = row;
    this->column = column;
    this->doc = doc;
    this->cursor = cursor;
    this->analizer = analizer;
}

void InsertBlockCommand::redo()
{
    addedLines = 0;
    previousLines.clear();
    // 1. Ensure we have enought space
    while (doc->data.size()<row+block.size()) {
        addedLines ++;
        TextLine tl;
        doc->data.append(tl);
        doc->m_newLines.insert(doc->data.size()-1);
    }

    // 2. Save old lines
    for (int i=row; i<row+block.size(); i++) {
        previousLines.append(doc->data[i].text);
    }

    // 3. Insert block
    for (int i=0; i<block.size(); i++) {
        TextLine tl = doc->data[row+i];
        int textPos = column - 2 * doc->indentAt(row+i);
        while (textPos>tl.text.length())
            tl.text += " ";
        tl.text.insert(textPos, block[i]);
        tl.selected.clear();
        tl.highlight.clear();
        for (int j=0; j<tl.text.length(); j++) {
            tl.selected << false;
        }
        if (analizer) {
            tl.highlight = analizer->lineProp(doc->documentId, tl.text).toList();
        }
        else {
            for (int j=0; j<tl.text.length(); j++) {
                tl.highlight << Shared::LxTypeEmpty;
            }
        }
        doc->data[row+i] = tl;
        doc->m_newLines.insert(row+i);
        doc->m_removedLines.insert(row+i);
    }
    cursorRow = cursor->row();
    cursorCol = cursor->column();
    cursor->setRow(cursor->row()+block.size()-1);
    cursor->setColumn(cursor->column()+block[0].length());
    doc->checkForCompilationRequest(QPoint(cursor->row(), cursor->column()));
}

void InsertBlockCommand::undo()
{
    // 1. Restore old lines
    Q_ASSERT(block.size()==previousLines.size());
    for (int i=0; i<block.size(); i++) {
        TextLine tl = doc->data[row+i];
        tl.text = previousLines[i];
        tl.selected.clear();
        tl.highlight.clear();
        for (int j=0; j<tl.text.length(); j++) {
            tl.selected << false;
        }
        if (analizer) {
            tl.highlight = analizer->lineProp(doc->documentId, tl.text).toList();
        }
        else {
            for (int j=0; j<tl.text.length(); j++) {
                tl.highlight << Shared::LxTypeEmpty;
            }
        }
        doc->data[row+i] = tl;
        doc->m_newLines.insert(row+i);
        doc->m_removedLines.insert(row+i);
    }

    // 2. Remove added lines
    for (int i=0; i<addedLines; i++) {
        doc->data.pop_back();
        doc->m_removedLines.insert(doc->data.size());
    }

    // 3. Restore cursor position
    cursor->setRow(cursorRow);
    cursor->setColumn(cursorCol);

    doc->checkForCompilationRequest(QPoint(cursor->row(), cursor->column()));
}

RemoveBlockCommand::RemoveBlockCommand(TextDocument *doc,
                                       TextCursor *cursor,
                                       Shared::AnalizerInterface *analizer,
                                       const QRect &block)
{
    this->doc = doc;
    this->cursor = cursor;
    this->analizer = analizer;
    this->block = block;
}

void RemoveBlockCommand::redo()
{
    int top = block.top();
    int bottom = block.bottom()+1;
    int left = block.left();
    int right = block.right()+1;
    bottom = qMin(bottom, doc->data.size());
    previousLines.clear();
    for (int i=top; i<bottom; i++) {
        TextLine tl = doc->data[i];
        previousLines << tl.text;
        int textPos = left - doc->indentAt(i)*2;

        tl.text = tl.text.remove(textPos, right-left);

        tl.selected.clear();
        tl.highlight.clear();
        for (int j=0; j<tl.text.length(); j++)
            tl.selected << false;
        if (analizer)
            tl.highlight = analizer->lineProp(doc->documentId, tl.text).toList();
        else for (int j=0; j<tl.text.length(); j++)
            tl.highlight << Shared::LxTypeEmpty;
        doc->data[i] = tl;
        doc->m_newLines.insert(i);
        doc->m_removedLines.insert(i);
    }
    cursorRow = cursor->row();
    cursorCol = cursor->column();

    doc->checkForCompilationRequest(QPoint(cursor->row(), cursor->column()));
}

void RemoveBlockCommand::undo()
{
    int top = block.top();
    int bottom = block.bottom()+1;
    bottom = qMin(bottom, doc->data.size());
    for (int i=top; i<bottom; i++) {
        TextLine tl = doc->data[i];
        tl.text = previousLines[i-top];
        tl.selected.clear();
        tl.highlight.clear();
        for (int j=0; j<tl.text.length(); j++)
            tl.selected << false;
        if (analizer)
            tl.highlight = analizer->lineProp(doc->documentId, tl.text).toList();
        else for (int j=0; j<tl.text.length(); j++)
            tl.highlight << Shared::LxTypeEmpty;
        doc->data[i] = tl;
        doc->m_newLines.insert(i);
        doc->m_removedLines.insert(i);
    }

    doc->checkForCompilationRequest(QPoint(cursor->row(), cursor->column()));
}

void TextDocument::removeSelection()
{
    for (int i=0; i<data.size(); i++) {
        for (int j=0; j<data[i].selected.size(); j++) {
            data[i].selected[j] = false;
        }
        data[i].lineEndSelected = false;
    }
}

void TextDocument::removeText(QString &removedText, const Shared::AnalizerInterface *analizer, int line, int pos, int blankLines, int blankChars, int count)
{
    int cnt = count;
    int p = pos;
    m_removedLines.insert(line);
    m_newLines.insert(line);
    int counter = line;
    while (cnt>0) {
        TextLine tl = data[line];
        int thisLineRemoveCount = qMin(cnt, tl.text.length() - p);
        Q_ASSERT(thisLineRemoveCount>=0);
        removedText += tl.text.mid(p, thisLineRemoveCount);
        tl.text.remove(p, thisLineRemoveCount);
        for (int i=0; i<thisLineRemoveCount; i++) {
            tl.highlight.removeAt(p);
            tl.selected.removeAt(p);
        }
        cnt -= thisLineRemoveCount;
        if (cnt>0) {
            if (line+1 < data.size()) {
                TextLine next = data[line+1];
                data.removeAt(line+1);
                tl.text += next.text;
                tl.selected += next.selected;
                tl.highlight += next.highlight;
                removedText += "\n";
                m_removedLines.insert(counter);
            }
            else {
                data.removeAt(line);
            }
            counter ++;
            cnt --;
        }
        if (line < data.size()) {
            if (analizer)
                tl.highlight = analizer->lineProp(documentId, tl.text).toList();
            data[line] = tl;
        }
    }
    if (line < data.size()) {
        data[line].text.remove(0, blankChars);
        for (int i=0; i<blankChars; i++) {
            data[line].selected.pop_front();
            data[line].highlight.pop_front();
        }
    }
    for (int i=0; i<blankLines; i++) {
        data.pop_back();
    }
}

int TextDocument::indentAt(int lineNo) const
{
    int result = 0;
    for (int i=0; i<qMin(lineNo, data.size()); i++) {
        result += data[i].indentStart + data[i].indentEnd;
    }
    if (lineNo < data.size()) {
        result += data[lineNo].indentStart;
    }
    return qMax(result, 0);
}

void TextDocument::setPlainText(const QString &text)
{
    qDebug() << "TextDocument::setPlainText";
    QStringList lines = text.split("\n");
    foreach (const QString &line, lines) {
        TextLine textLine;
        textLine.text = line;
        textLine.indentStart = 0;
        textLine.indentEnd = 0;
        textLine.lineEndSelected = false;
        for (int j=0; j<line.length(); j++) {
            textLine.highlight << Shared::LxTypeEmpty;
            textLine.selected << false;
        }
        data.append(textLine);
    }
    qDebug() << "TextDocument::setPlainText -- OK";
}

QString TextDocument::toPlainText() const
{
    QString result;
    for (int i=0; i<data.size(); i++) {
        if (i>0)
            result += "\n";
        result += data[i].text;
    }
    return result;
}

void TextDocument::checkForCompilationRequest(const QPoint &cursorPosition)
{
    if (cursorPosition.y()!=lastCursorPos.y()) {
        if (!m_removedLines.isEmpty() || !m_newLines.isEmpty()) {
            lastCursorPos = cursorPosition;
            flushChanges();
        }
    }
}

void TextDocument::flushChanges()
{
    if (m_removedLines.isEmpty() || m_newLines.isEmpty())
        return;
    Shared::ChangeTextTransaction trans;
    trans.removedLineNumbers = m_removedLines;
    QList<int> ll = m_newLines.toList();
    for (int i=0; i<ll.size(); i++) {
        m_removedLines.insert(ll.at(i));
    }
    changes.push(trans);
    m_removedLines.clear();
    m_newLines.clear();
}

void TextDocument::flushTransaction()
{
    flushChanges();
    if (!changes.isEmpty())
        emit compilationRequest(changes);
    changes.clear();
}


}
