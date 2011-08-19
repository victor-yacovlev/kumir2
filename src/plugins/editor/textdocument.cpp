#include "textdocument.h"
#include "textcursor.h"
#

namespace Editor
{



TextDocument::TextDocument(QObject *parent)
    : QObject(parent)
    , documentId(-1)
    , m_undoStack(new QUndoStack(this))
{

}

bool TextDocument::noUndoRedo = false;

void TextDocument::insertText(const QString &text, const Shared::AnalizerInterface * analizer, int line, int pos, int &blankLines, int &blankChars)
{
    blankLines = blankChars = 0;
    while (data.size()<=line) {
        blankLines ++;
        TextLine tl;
        tl.inserted = true;
        data.append(tl);
    }
    TextLine tl = data[line];
    while (pos>tl.text.length()) {
        blankChars ++;
        tl.text.append(' ');
        tl.selected << false;
        tl.highlight << Shared::LxTypeEmpty;
    }
    tl.changed = true;
    data[line] = tl;
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
        QString remainder = data[line].text.mid(pos);
        data[line].text = data[line].text.left(pos)+lines[0];
        while (data[line].selected.size() < data[line].text.length())
            data[line].selected << false;
        while (data[line].highlight.size() < data[line].text.length())
            data[line].highlight << Shared::LxTypeEmpty;
        if (analizer)
            data[line].highlight = analizer->lineProp(documentId, data[line].text).toList();

        // 2. Insert middle lines
        for (int i=lines.count()-1; i>=1; i--) {
            TextLine tl;
            tl.changed = true;
            tl.inserted = true;
            tl.text = lines[i];
            for (int j=0; j<tl.text.length(); j++) {
                tl.selected << false;
                tl.highlight << Shared::LxTypeEmpty;
            }
            if (analizer)
                tl.highlight = analizer->lineProp(documentId, tl.text).toList();
            data.insert(line+1, tl);
        }

//        // 3. Prepend fragment to last line
//        if (line+lines.count()-1 >= data.size()) {
//            TextLine last;
//            data.append(last);
//        }
//        m_removedLines.insert(line+1);
//        m_newLines.insert(line);
        data[line+lines.count()-1].text.prepend(remainder);
        data[line+lines.count()-1].changed = true;
        while (data[line+lines.count()-1].selected.size() < data[line+lines.count()-1].text.length())
            data[line+lines.count()-1].selected << false;
        while (data[line+lines.count()-1].highlight.size() < data[line+lines.count()-1].text.length())
            data[line+lines.count()-1].highlight << Shared::LxTypeEmpty;
        if (analizer)
            data[line+lines.count()-1].highlight = analizer->lineProp(documentId, data[line+lines.count()-1].text).toList();
    }
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
    int removedCounter = line;
    while (cnt>0) {
        TextLine tl = data[line];
        tl.changed = true;
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
                removedCounter ++;
                TextLine next = data[line+1];
                data.removeAt(line+1);
                m_removedLines.insert(removedCounter);
                tl.text += next.text;
                tl.selected += next.selected;
                tl.highlight += next.highlight;
                removedText += "\n";
            }
            else {
                data.removeAt(line);
            }

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
        data[line].changed = true;
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

void TextDocument::setKumFile(const KumFile::Data &d)
{
    data.clear();
    QStringList lines = d.visibleText.split("\n");
    for (int i=0; i<lines.count(); i++) {
        const QString line = lines[i];
        TextLine textLine;
        textLine.text = line;
        textLine.indentStart = 0;
        textLine.indentEnd = 0;
        textLine.lineEndSelected = false;
        for (int j=0; j<line.length(); j++) {
            textLine.highlight << Shared::LxTypeEmpty;
            textLine.selected << false;
        }
        textLine.protecteed = d.protectedLineNumbers.contains(i);
        textLine.hidden = false;
        data.append(textLine);
    }
    if (d.hasHiddenText) {
        lines = d.hiddenText.split("\n");
        for (int i=0; i<lines.count(); i++) {
            const QString line = lines[i];
            TextLine textLine;
            textLine.text = line;
            textLine.indentStart = 0;
            textLine.indentEnd = 0;
            textLine.lineEndSelected = false;
            for (int j=0; j<line.length(); j++) {
                textLine.highlight << Shared::LxTypeEmpty;
                textLine.selected << false;
            }
            textLine.protecteed = false;
            textLine.hidden = true;
            data.append(textLine);
        }
    }

}

int TextDocument::hiddenLineStart() const
{
    int result = -1;
    for (int i=0; i<data.size(); i++) {
        if (data[i].hidden) {
            result = i;
            break;
        }
    }
    return result;
}

KumFile::Data TextDocument::toKumFile() const
{
    KumFile::Data kumfile;
    for (int i=0; i<data.size(); i++) {
        if (data[i].hidden) {
            kumfile.hiddenText += data[i].text;
            if (i<data.size()-1)
                kumfile.hiddenText += "\n";
        }
        else {
            kumfile.visibleText += data[i].text;
            if (data[i].protecteed)
                kumfile.protectedLineNumbers.insert(i);
            if (i<data.size()-1 && !data[i+1].hidden)
                kumfile.visibleText += "\n";
        }
    }
    return kumfile;
}


void TextDocument::checkForCompilationRequest(const QPoint &cursorPosition)
{
    if (cursorPosition.y()!=lastCursorPos.y()) {
        bool hasChangedLines = false;
        for (int i=0; i<data.size(); i++) {
            if (data[i].changed) {
                hasChangedLines = true;
                break;
            }
        }
        bool hasRemovedLines = !m_removedLines.isEmpty();
        if (!hasChangedLines || !hasRemovedLines) {
            lastCursorPos = cursorPosition;
            flushChanges();
        }
    }
}

void TextDocument::flushChanges()
{
    Shared::ChangeTextTransaction trans;
    trans.removedLineNumbers = m_removedLines;
    for (int i=0; i<data.size(); i++) {
        if (data[i].inserted) {
            trans.newLines.append(data[i].text);
        }
        else if (data[i].changed) {
            trans.removedLineNumbers.insert(i);
            trans.newLines.append(data[i].text);
        }
        data[i].changed = false;
        data[i].inserted = false;
    }
    if (!trans.removedLineNumbers.isEmpty() || !trans.newLines.isEmpty())
        changes.push(trans);
    m_removedLines.clear();

}

void TextDocument::flushTransaction()
{
    flushChanges();
    if (!changes.isEmpty())
        emit compilationRequest(changes);
    changes.clear();
}




}
