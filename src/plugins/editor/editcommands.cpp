#include "editcommands.h"
#include "textdocument.h"
#include "textcursor.h"

namespace Editor {

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

InsertCommand::InsertCommand(TextDocument *doc, TextCursor *cursor, Shared::AnalizerInterface *analizer)
{
    this->doc = doc;
    this->cursor = cursor;
    this->analizer = analizer;
    line = pos = blankLines = blankChars = 0;
}

void InsertCommand::redo()
{
    if (TextDocument::noUndoRedo)
        return;
    cursorRow = cursor->row();
    cursorCol = cursor->column();
    doc->insertText(text, analizer, line, pos, blankLines, blankChars);
    QStringList lines = text.split("\n", QString::KeepEmptyParts);
    if (lines.size()>1) {
        cursor->setRow(cursor->row()+lines.size()-1);
        cursor->setColumn(lines.last().length() + doc->indentAt(cursor->row())*2);
    }
    else {
        cursor->setColumn(cursor->column()+text.length());
    }
    doc->checkForCompilationRequest(QPoint(cursor->row(), cursor->column()));
}

void InsertCommand::undo()
{
    if (TextDocument::noUndoRedo)
        return;
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

RemoveCommand::RemoveCommand(TextDocument *doc, TextCursor *cursor, Shared::AnalizerInterface *analizer)
{
    this->doc = doc;
    this->cursor = cursor;
    this->analizer = analizer;
    line = pos = count = 0;
    keepKursor = true;
}

void RemoveCommand::redo()
{
    if (TextDocument::noUndoRedo)
        return;
    cursorRow = cursor->row();
    cursorCol = cursor->column();
    removedText.clear();

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
    doc->removeText(removedText, analizer, line, pos, 0, 0, count);
    doc->checkForCompilationRequest(QPoint(cursor->row(), cursor->column()));
}

void RemoveCommand::undo()
{
    if (TextDocument::noUndoRedo)
        return;
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

RemoveBlockCommand::RemoveBlockCommand(TextDocument *doc, TextCursor *cursor, Shared::AnalizerInterface *analizer)
{
    this->doc = doc;
    this->cursor = cursor;
    this->analizer = analizer;
}

void RemoveBlockCommand::redo()
{
    if (TextDocument::noUndoRedo)
        return;
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
        tl.changed = true;

        tl.selected.clear();
        tl.highlight.clear();
        for (int j=0; j<tl.text.length(); j++)
            tl.selected << false;
        if (analizer)
            tl.highlight = analizer->lineProp(doc->documentId, tl.text).toList();
        else for (int j=0; j<tl.text.length(); j++)
            tl.highlight << Shared::LxTypeEmpty;
        doc->data[i] = tl;

    }
    cursorRow = cursor->row();
    cursorCol = cursor->column();

    doc->checkForCompilationRequest(QPoint(cursor->row(), cursor->column()));
}

void RemoveBlockCommand::undo()
{
    if (TextDocument::noUndoRedo)
        return;
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
        tl.changed = true;
        doc->data[i] = tl;

    }

    doc->checkForCompilationRequest(QPoint(cursor->row(), cursor->column()));
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
    addedLines = 0;
}

InsertBlockCommand::InsertBlockCommand(TextDocument *doc, TextCursor *cursor, Shared::AnalizerInterface *analizer)
{
    this->doc = doc;
    this->cursor = cursor;
    this->analizer = analizer;
    addedLines =row = column = 0;
}

void InsertBlockCommand::redo()
{
    if (TextDocument::noUndoRedo)
        return;
    addedLines = 0;
    previousLines.clear();
    // 1. Ensure we have enought space
    while (doc->data.size()<row+block.size()) {
        addedLines ++;
        TextLine tl;
        tl.changed = true;
        tl.inserted = true;
        doc->data.append(tl);
    }

    // 2. Save old lines
    for (int i=row; i<row+block.size(); i++) {
        previousLines.append(doc->data[i].text);
    }

    // 3. Insert block
    for (int i=0; i<block.size(); i++) {
        TextLine tl = doc->data[row+i];
        tl.changed = true;
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
    }
    cursorRow = cursor->row();
    cursorCol = cursor->column();
    cursor->setRow(cursor->row()+block.size()-1);
    cursor->setColumn(cursor->column()+block[0].length());
    doc->checkForCompilationRequest(QPoint(cursor->row(), cursor->column()));
}

void InsertBlockCommand::undo()
{
    if (TextDocument::noUndoRedo)
        return;
    // 1. Restore old lines
    Q_ASSERT(block.size()==previousLines.size());
    for (int i=0; i<block.size(); i++) {
        TextLine tl = doc->data[row+i];
        tl.changed = true;
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


ToggleLineProtectedCommand::ToggleLineProtectedCommand(TextDocument *Doc, int lineNo)
{
    this->doc = Doc;
    this->lineNo = lineNo;
}

void ToggleLineProtectedCommand::redo()
{
    if (TextDocument::noUndoRedo)
        return;
    doc->setProtected(lineNo, !doc->isProtected(lineNo));
}

void ToggleLineProtectedCommand::undo()
{
    if (TextDocument::noUndoRedo)
        return;
    doc->setProtected(lineNo, !doc->isProtected(lineNo));
}

ChangeHiddenLineDelimeterCommand::ChangeHiddenLineDelimeterCommand(
    TextDocument *doc,
    int firstHiddenLineNo)
{
    this->doc = doc;
    this->firstHiddenLineNo = firstHiddenLineNo;
}

void ChangeHiddenLineDelimeterCommand::redo()
{
    if (TextDocument::noUndoRedo)
        return;
    prevData = doc->toKumFile();
    for (int i=0; i<doc->linesCount(); i++) {
        if (i<firstHiddenLineNo || firstHiddenLineNo==-1)
            doc->setHidden(i, false);
        else
            doc->setHidden(i, true);
    }
}

void ChangeHiddenLineDelimeterCommand::undo()
{
    if (TextDocument::noUndoRedo)
        return;
    doc->setKumFile(prevData);
}

QDataStream & operator<< (QDataStream & stream, const InsertCommand & command)
{
    stream << command.line;
    stream << command.pos;
    stream << command.blankLines;
    stream << command.blankChars;
    stream << command.cursorRow;
    stream << command.cursorCol;
    stream << command.text;
    return stream;
}

QDataStream & operator>> (QDataStream & stream, InsertCommand & command)
{
    stream >> command.line;
    stream >> command.pos;
    stream >> command.blankLines;
    stream >> command.blankChars;
    stream >> command.cursorRow;
    stream >> command.cursorCol;
    stream >> command.text;
    return stream;
}

QDataStream & operator<< (QDataStream & stream, const RemoveCommand & command)
{
    stream << command.line;
    stream << command.pos;
    stream << command.count;
    stream << command.keepKursor;
    stream << command.cursorRow;
    stream << command.cursorCol;
    stream << command.removedText;
    return stream;
}

QDataStream & operator>> (QDataStream & stream, RemoveCommand & command)
{
    stream >> command.line;
    stream >> command.pos;
    stream >> command.count;
    stream >> command.keepKursor;
    stream >> command.cursorRow;
    stream >> command.cursorCol;
    stream >> command.removedText;
    return stream;
}

QDataStream & operator<< (QDataStream & stream, const InsertBlockCommand & command)
{
    stream << command.row;
    stream << command.column;
    stream << command.addedLines;
    stream << command.cursorRow;
    stream << command.cursorCol;
    stream << command.block;
    stream << command.previousLines;
    return stream;
}

QDataStream & operator>> (QDataStream & stream, InsertBlockCommand & command)
{
    stream >> command.row;
    stream >> command.column;
    stream >> command.addedLines;
    stream >> command.addedLines;
    stream >> command.cursorRow;
    stream >> command.cursorCol;
    stream >> command.block;
    stream >> command.previousLines;
    return stream;
}

QDataStream & operator<< (QDataStream & stream, const RemoveBlockCommand & command)
{
    stream << command.cursorRow;
    stream << command.cursorCol;
    stream << command.block;
    stream << command.previousLines;
    return stream;
}

QDataStream & operator>> (QDataStream & stream, RemoveBlockCommand & command)
{
    stream >> command.cursorRow;
    stream >> command.cursorCol;
    stream >> command.block;
    stream >> command.previousLines;
    return stream;
}

QDataStream & operator<< (QDataStream & stream, const ToggleLineProtectedCommand & command)
{
    stream << command.lineNo;
    return stream;
}

QDataStream & operator>> (QDataStream & stream, ToggleLineProtectedCommand & command)
{
    stream >> command.lineNo;
    return stream;
}

QDataStream & operator<< (QDataStream & stream, const ChangeHiddenLineDelimeterCommand & command)
{
    stream << command.firstHiddenLineNo;
    stream << KumFile::toString(command.prevData);
    return stream;
}

QDataStream & operator>> (QDataStream & stream, ChangeHiddenLineDelimeterCommand & command)
{
    stream >> command.firstHiddenLineNo;
    QString s;
    stream >> s;
    command.prevData = KumFile::fromString(s);
    return stream;
}

}
