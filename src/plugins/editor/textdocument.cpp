#include "textdocument.h"
#include "textcursor.h"
#include "settingspage.h"

namespace Editor
{



TextDocument::TextDocument(QObject *parent, QSettings * settings)
    : QObject(parent)
    , documentId(-1)
    , m_undoStack(new QUndoStack(this))
    , m_settings(settings)
{
    m_analizer = 0;
    b_wasHiddenText = false;
}

bool TextDocument::noUndoRedo = false;

void TextDocument::insertText(const QString &text, const Shared::AnalizerInterface * analizer, int line, int pos, int &blankLines, int &blankChars)
{

    blankLines = blankChars = 0;
    bool blankLinesAreHidden = data.last().hidden;
    while (data.size()<=line) {
        blankLines ++;
        TextLine tl;
        tl.inserted = true;
        tl.hidden = blankLinesAreHidden;
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
            tl.hidden = data[line].hidden;
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
                if (next.hidden)
                    m_removedHiddenLines.insert(removedCounter);
                else
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

void TextDocument::setKumFile(const KumFile::Data &d, bool showHiddenLines)
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
    if (d.hasHiddenText && showHiddenLines) {
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
    else if (d.hasHiddenText) {
        s_hiddenText = d.hiddenText;
        b_wasHiddenText = true;
    }
    forceCompleteRecompilation();
}

void TextDocument::setPlainText(const QString &t)
{
    data.clear();
    QStringList lines = t.split("\n");
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
        textLine.hidden = false;
        data.append(textLine);
    }
    forceCompleteRecompilation();
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
    if (b_wasHiddenText) {
        kumfile.hasHiddenText = true;
        kumfile.hiddenText = s_hiddenText;
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
        if (!data[i].hidden) {
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
    }
    if (!trans.removedLineNumbers.isEmpty() || !trans.newLines.isEmpty())
        changes.push(trans);
    m_removedLines.clear();

}

void TextDocument::flushTransaction()
{
    if (!m_analizer)
        return;
    bool hiddenChanged = !m_removedHiddenLines.isEmpty();
    if (!hiddenChanged)
    for (int i=0; i<data.size(); i++) {
        if (data[i].changed || data[i].inserted) {
            if (data[i].hidden) {
                hiddenChanged = true;
                break;
            }
        }
    }
    flushChanges();
    if (hiddenChanged)
        forceCompleteRecompilation();
    if (!changes.isEmpty()) {
        if (m_analizer->supportPartialCompiling())
            emit compilationRequest(changes);
        else
            emit forceCompleteRecompilation();
    }
    changes.clear();
}

void TextDocument::forceCompleteRecompilation()
{
    changes.clear();
    m_removedLines.clear();
    m_removedHiddenLines.clear();
    QStringList visibleText, hiddenText;
    int hiddenBaseLine = -1;
    for (int i=0; i<data.size(); i++) {
        data[i].changed = data[i].inserted = false;
        if (data[i].hidden && hiddenBaseLine==-1)
            hiddenBaseLine = i;
        if (data[i].hidden)
            hiddenText += data[i].text;
        else
            visibleText += data[i].text;
    }
    emit completeCompilationRequest(visibleText, hiddenText, hiddenBaseLine);
}

QString TextDocument::toHtml(int fromLine, int toLine) const
{
    if (fromLine==-1 || toLine==-1) {
        for (int i=0; i<data.size(); i++) {
            if (data[i].selected.contains(true)) {
                fromLine = i;
                break;
            }
        }
        for (int i=data.size()-1; i>=0; i--) {
            if (data[i].selected.contains(true)) {
                toLine = i;
                break;
            }
        }
    }
    fromLine = qMax(0, fromLine);
    toLine = qMin(data.size()-1, toLine);
    QString result;
    QStringList programLines;
    QStringList marginLines;
    QStringList p_style;
    p_style << "font-family:'Droid Sans Mono','Consolas','Courier New','Courier','Monospace'";
    p_style << QString("font-size:10pt");
    p_style << "margin:0px";
    p_style << "padding:0px";

    for (int i=fromLine; i<=toLine; i++) {
        programLines << lineToHtml(i);
        marginLines << ( data[i].marginText.isEmpty()? (data[i].errors.isEmpty()? "" : data[i].errors[0]) : data[i].marginText );
    }
    result = QString::fromAscii(
                "<table width=\"100%\">"
                "  <tr>"
                "    <td width=\"60%\">"
                "      <p style=\"%1\">%2</p>"
                "    </td>"
                "    <td>"
                "      <p style=\"%1\">%3</p>"
                "    </td>"
                "  </tr>"
                "</table>"
                )
            .arg(p_style.join(";")+";")
            .arg(programLines.join("<br>\n"))
            .arg(marginLines.join("<br>\n"))
            ;
    return result;
}

struct Chunk {
    QString text;
    bool bold;
    bool italic;
    bool error;
    QString color;
    quint32 format;
    QString html;
};

QString TextDocument::lineToHtml(int lineNo) const
{
    if (lineNo<0 || lineNo>=data.size())
        return "";
    int indent = indentAt(lineNo);
    QString result;
    for (int i=0; i<indent; i++) {
        result += ". ";
    }
    QList<Shared::LexemType> highlight = data[lineNo].highlight;
    QString text = data[lineNo].text;
    Q_ASSERT(text.length()==highlight.size());

    QList<Chunk> chunks;
    for (int i=0; i<text.length(); i++) {
        bool nonLatin = text[i]!='\0' && text[i].isLetter() && text[i].toAscii()!='\0';
        if (chunks.isEmpty()) {
            Chunk ch;
            ch.bold = ch.error = false;
            ch.italic = nonLatin;
            ch.format = highlight[i];
            ch.text += text[i];
            chunks << ch;
        }
        else {
            if (chunks.last().format==highlight[i] && chunks.last().italic==nonLatin) {
                chunks.last().text += text[i];
            }
            else {
                Chunk ch;
                ch.bold = ch.error = false;
                ch.italic = nonLatin;
                ch.format = highlight[i];
                ch.text += text[i];
                chunks << ch;
            }
        }
    }

    static const quint32 PriKwd = Shared::LxTypePrimaryKwd;
    static const quint32 SecKwd = Shared::LxTypeSecondaryKwd;
    static const quint32 NameClass = Shared::LxNameClass;
    static const quint32 NameAlg = Shared::LxNameAlg;
    static const quint32 NameModule = Shared::LxNameModule;
    static const quint32 ConstLiteral = Shared::LxConstLiteral;
    static const quint32 TypeConstant = Shared::LxTypeConstant;
    static const quint32 TypeDoc = Shared::LxTypeDoc;
    static const quint32 TypeComment = Shared::LxTypeComment;

    for (int i=0; i<chunks.size() ;i++) {
        chunks[i].error = ( chunks[i].format & Shared::LxTypeError ) > 0;
        chunks[i].format = ( chunks[i].format << 1 ) >> 1;
        const quint32 priKwd = PriKwd & chunks[i].format;
        const quint32 secKwd = SecKwd & chunks[i].format;
        const quint32 nameClass = NameClass == chunks[i].format;
        const quint32 nameAlg = NameAlg == chunks[i].format;
        const quint32 nameModule = NameModule == chunks[i].format;
        const quint32 literalConstant = ConstLiteral == chunks[i].format;
        const quint32 constant = TypeConstant & chunks[i].format;
        const quint32 doc = TypeDoc == chunks[i].format;
        const quint32 comment = TypeComment == chunks[i].format;
        QColor c = Qt::black;
        if (priKwd || secKwd) {
            c = m_settings->value(SettingsPage::KeyColorKw, SettingsPage::DefaultColorKw).toString();
            chunks[i].bold = m_settings->value(SettingsPage::KeyBoldKw, SettingsPage::DefaultBoldKw).toBool();
        }
        if (nameClass) {
            c = m_settings->value(SettingsPage::KeyColorType,  SettingsPage::DefaultColorType).toString();
            chunks[i].bold = m_settings->value(SettingsPage::KeyBoldType, SettingsPage::DefaultBoldType).toBool();
        }
        else if (nameAlg) {
            c = m_settings->value(SettingsPage::KeyColorAlg,  SettingsPage::DefaultColorAlg).toString();
            chunks[i].bold = m_settings->value(SettingsPage::KeyBoldAlg, SettingsPage::DefaultBoldAlg).toBool();
        }
        else if (nameModule) {
            c = m_settings->value(SettingsPage::KeyColorMod,  SettingsPage::DefaultColorMod).toString();
            chunks[i].bold = m_settings->value(SettingsPage::KeyBoldMod, SettingsPage::DefaultBoldMod).toBool();
        }
        else if (literalConstant) {
            c = m_settings->value(SettingsPage::KeyColorLiteral,  SettingsPage::DefaultColorLiteral).toString();
            chunks[i].bold = m_settings->value(SettingsPage::KeyBoldLiteral, SettingsPage::DefaultBoldLiteral).toBool();
        }
        else if (constant)
        {
            c = m_settings->value(SettingsPage::KeyColorNumeric,  SettingsPage::DefaultColorNumeric).toString();
            chunks[i].bold = m_settings->value(SettingsPage::KeyBoldNumeric, SettingsPage::DefaultBoldNumeric).toBool();
        }
        else if (doc) {
            c = m_settings->value(SettingsPage::KeyColorDoc,  SettingsPage::DefaultColorDoc).toString();
            chunks[i].bold = m_settings->value(SettingsPage::KeyBoldDoc, SettingsPage::DefaultBoldDoc).toBool();
        }
        else if (comment) {
            c = m_settings->value(SettingsPage::KeyColorComment,  SettingsPage::DefaultColorComment).toString();
            chunks[i].bold = m_settings->value(SettingsPage::KeyBoldComment, SettingsPage::DefaultBoldComment).toBool();
        }
        chunks[i].color = c.name();
        chunks[i].html = QString::fromAscii(
                    "<span style=\""
                    "color: %1;"
                    "font-weight: %2;"
                    "font-style: %3;"
                    "text-decoration: %4;"
                    "\">"
                    "%5"
                    "</span>"
                    )
                .arg(chunks[i].color)
                .arg(chunks[i].bold? "bold" : "normal")
                .arg(chunks[i].italic? "italic" : "normal")
                .arg(chunks[i].error? "underline" : "none")
                .arg(chunks[i].text)
                ;
    }
    for (int i=0; i<chunks.size(); i++) {
        result += chunks[i].html;
    }
    return result;
}

}
