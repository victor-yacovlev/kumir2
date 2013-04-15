#include "textdocument.h"
#include "textcursor.h"
#include "settingspage.h"

namespace Editor
{



TextDocument::TextDocument(QObject *parent, QSettings * settings)
    : QObject(parent)
    , id_(-1)
    , undoStack_(new QUndoStack(this))
    , settings_(settings)
{
    analizer_ = 0;
    wasHiddenTextFlag_ = false;
}

bool TextDocument::noUndoRedo = false;

void TextDocument::insertText(const QString &text, const Shared::AnalizerInterface * analizer, int line, int pos, int &blankLines, int &blankChars)
{

    blankLines = blankChars = 0;
    bool blankLinesAreHidden = false;
    if (data_.size()>0)
            blankLinesAreHidden = data_.last().hidden;
    while (data_.size()<=line) {
        blankLines ++;
        TextLine tl;
        tl.inserted = true;
        tl.hidden = blankLinesAreHidden;
        data_.append(tl);
    }
    TextLine tl = data_[line];
    while (pos>tl.text.length()) {
        blankChars ++;
        tl.text.append(' ');
        tl.selected << false;
        tl.highlight << Shared::LxTypeEmpty;
    }
    tl.changed = true;
    data_[line] = tl;

    const QStringList lines = text.split("\n", QString::KeepEmptyParts);
    if (lines.size()==1) {
        // Insert text fragment into line
        data_[line].text.insert(pos, lines[0]);
        while (data_[line].selected.size() < data_[line].text.length())
            data_[line].selected << false;
        while (data_[line].highlight.size() < data_[line].text.length())
            data_[line].highlight << Shared::LxTypeEmpty;
        if (analizer)
            data_[line].highlight = analizer->lineProp(id_, data_[line].text).toList();

    }
    else {
        // 1. Append fragment to first line
        QString remainder = data_[line].text.mid(pos);
        data_[line].text = data_[line].text.left(pos)+lines[0];
        while (data_[line].selected.size() < data_[line].text.length())
            data_[line].selected << false;
        while (data_[line].highlight.size() < data_[line].text.length())
            data_[line].highlight << Shared::LxTypeEmpty;
        if (analizer)
            data_[line].highlight = analizer->lineProp(id_, data_[line].text).toList();

        // 2. Insert middle lines
        for (int i=lines.count()-1; i>=1; i--) {
            TextLine tl;
            tl.changed = true;
            tl.inserted = true;
            tl.text = lines[i];
            tl.hidden = data_[line].hidden;
            for (int j=0; j<tl.text.length(); j++) {
                tl.selected << false;
                tl.highlight << Shared::LxTypeEmpty;
            }
            if (analizer)
                tl.highlight = analizer->lineProp(id_, tl.text).toList();
            data_.insert(line+1, tl);
        }

//        // 3. Prepend fragment to last line
//        if (line+lines.count()-1 >= data.size()) {
//            TextLine last;
//            data.append(last);
//        }
//        m_removedLines.insert(line+1);
//        m_newLines.insert(line);
        data_[line+lines.count()-1].text.append(remainder);
        data_[line+lines.count()-1].changed = true;
        while (data_[line+lines.count()-1].selected.size() < data_[line+lines.count()-1].text.length())
            data_[line+lines.count()-1].selected << false;
        while (data_[line+lines.count()-1].highlight.size() < data_[line+lines.count()-1].text.length())
            data_[line+lines.count()-1].highlight << Shared::LxTypeEmpty;
        if (analizer)
            data_[line+lines.count()-1].highlight = analizer->lineProp(id_, data_[line+lines.count()-1].text).toList();
    }
}




void TextDocument::removeSelection()
{
    for (int i=0; i<data_.size(); i++) {
        for (int j=0; j<data_[i].selected.size(); j++) {
            data_[i].selected[j] = false;
        }
        data_[i].lineEndSelected = false;
    }
}

void TextDocument::removeText(QString &removedText, const Shared::AnalizerInterface *analizer, int line, int pos, int blankLines, int blankChars, int count)
{
    if (data_.size()==0)
        return;
    int cnt = count;
    int p = pos;
    int removedCounter = line;
    while (cnt>0) {
        TextLine tl = data_[line];
        tl.changed = true;
        int thisLineRemoveCount = qMin(cnt, tl.text.length() - p);
//        Q_ASSERT(thisLineRemoveCount>=0);
        removedText += tl.text.mid(p, thisLineRemoveCount);
        tl.text.remove(p, thisLineRemoveCount);
        for (int i=0; i<thisLineRemoveCount; i++) {
            tl.highlight.removeAt(p);
            tl.selected.removeAt(p);
        }
        cnt -= thisLineRemoveCount;
        if (cnt>0) {
            if (line+1 < data_.size()) {
                TextLine next = data_[line+1];
                data_.removeAt(line+1);
                if (next.hidden)
                    removedHiddenLines_.insert(removedCounter);
                else
                    removedLines_.insert(removedCounter);
                tl.text += next.text;
                tl.selected += next.selected;
                tl.highlight += next.highlight;
                removedText += "\n";
                removedCounter ++;
            }
            else {
                data_.removeAt(line);
                removedLines_.insert(removedCounter);
                removedCounter ++;
            }

            cnt --;
        }
        if (line < data_.size()) {
            if (analizer)
                tl.highlight = analizer->lineProp(id_, tl.text).toList();
            data_[line] = tl;
            removedLines_.insert(removedCounter);
        }
    }
    if (line < data_.size()) {
        int blanks = blankChars;
        TextLine & tl = data_[line];
        bool changed = false;
        for (int i=tl.text.length()-1; i>=0; i--) {
            if (blanks<=0) break;
            if (tl.text[i].isSpace()) {
                tl.text.resize(tl.text.length()-1);
                tl.selected.pop_back();
                tl.highlight.pop_back();
                changed = true;
                blanks -= 1;
            }
        }
        if (changed) {
            removedLines_.insert(line);
            tl.changed = true;
        }
    }
    for (int i=0; i<blankLines; i++) {
        data_.pop_back();
    }
}

void TextDocument::insertLine(const QString &text, const uint beforeLineNo)
{
    TextLine textLine;
    textLine.text = text;
    textLine.inserted = true;
    if (analizer_) {
        textLine.highlight = analizer_->lineProp(id_, text).toList();
    }
    for (uint i=0; i<text.length(); i++) {
        textLine.selected.push_back(false);
    }
    data_.insert(qMin(beforeLineNo, uint(data_.size())), textLine);
    forceCompleteRecompilation();
}

void TextDocument::removeLine(const uint lineNo)
{
    data_.removeAt(lineNo);
    removedLines_.insert(lineNo);
    forceCompleteRecompilation();
}

uint TextDocument::indentAt(uint lineNo) const
{
    int result = 0;
    for (uint i=0; i<qMin(lineNo, uint(data_.size()) ); i++) {
        result += data_[i].indentStart + data_[i].indentEnd;
    }
    if (lineNo>=0 && lineNo < data_.size()) {
        result += data_[lineNo].indentStart;
    }
    return uint( qMax(result, 0) );
}

void TextDocument::setKumFile(const KumFile::Data &d, bool showHiddenLines)
{
    data_.clear();
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
        data_.append(textLine);
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
            data_.append(textLine);
        }
    }
    else if (d.hasHiddenText) {
        hiddenText_ = d.hiddenText;
        wasHiddenTextFlag_ = true;
    }
    forceCompleteRecompilation();
}

void TextDocument::setPlainText(const QString &t)
{
    data_.clear();
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
        data_.append(textLine);
    }
    forceCompleteRecompilation();
}

int TextDocument::hiddenLineStart() const
{
    int result = -1;
    for (int i=0; i<data_.size(); i++) {
        if (data_[i].hidden) {
            result = i;
            break;
        }
    }
    return result;
}

KumFile::Data TextDocument::toKumFile() const
{
    KumFile::Data kumfile;
    for (int i=0; i<data_.size(); i++) {
        if (data_[i].hidden) {
            kumfile.hiddenText += data_[i].text;
            if (i<data_.size()-1)
                kumfile.hiddenText += "\n";
        }
        else {
            kumfile.visibleText += data_[i].text;
            if (data_[i].protecteed)
                kumfile.protectedLineNumbers.insert(i);
            if (i<data_.size()-1 && !data_[i+1].hidden)
                kumfile.visibleText += "\n";
        }
    }
    if (wasHiddenTextFlag_) {
        kumfile.hasHiddenText = true;
        kumfile.hiddenText = hiddenText_;
    }
    return kumfile;
}


void TextDocument::checkForCompilationRequest(const QPoint &cursorPosition)
{
    if (cursorPosition.y()!=lastCursorPos_.y()) {
        bool hasChangedLines = false;
        for (int i=0; i<data_.size(); i++) {
            if (data_[i].changed) {
                hasChangedLines = true;
                break;
            }
        }
        bool hasRemovedLines = !removedLines_.isEmpty();
        if (!hasChangedLines || !hasRemovedLines) {
            lastCursorPos_ = cursorPosition;
            flushChanges();
        }
    }
}

void TextDocument::flushChanges()
{
    Shared::ChangeTextTransaction trans;
    trans.removedLineNumbers = removedLines_;
    for (int i=0; i<data_.size(); i++) {
        if (!data_[i].hidden) {
            if (data_[i].inserted) {
                trans.newLines.append(data_[i].text);
            }
            else if (data_[i].changed) {
                trans.removedLineNumbers.insert(i);
                trans.newLines.append(data_[i].text);
            }
            data_[i].changed = false;
            data_[i].inserted = false;
        }
    }
    if (!trans.removedLineNumbers.isEmpty() || !trans.newLines.isEmpty())
        changes_.push(trans);
    removedLines_.clear();

}

void TextDocument::flushTransaction()
{
    if (!analizer_)
        return;
    bool hiddenChanged = !removedHiddenLines_.isEmpty();
    if (!hiddenChanged)
    for (int i=0; i<data_.size(); i++) {
        if (data_[i].changed || data_[i].inserted) {
            if (data_[i].hidden) {
                hiddenChanged = true;
                break;
            }
        }
    }
    flushChanges();
    if (hiddenChanged)
        forceCompleteRecompilation();
    if (!changes_.isEmpty()) {
        if (analizer_->supportPartialCompiling())
            emit compilationRequest(changes_);
        else
            emit forceCompleteRecompilation();
    }
    changes_.clear();
}

void TextDocument::forceCompleteRecompilation()
{
    changes_.clear();
    removedLines_.clear();
    removedHiddenLines_.clear();
    QStringList visibleText, hiddenText;
    int hiddenBaseLine = -1;
    for (int i=0; i<data_.size(); i++) {
        data_[i].changed = data_[i].inserted = false;
        if (data_[i].hidden && hiddenBaseLine==-1)
            hiddenBaseLine = i;
        if (data_[i].hidden)
            hiddenText += data_[i].text;
        else
            visibleText += data_[i].text;
    }
    emit completeCompilationRequest(visibleText, hiddenText, hiddenBaseLine);
}

QString TextDocument::toHtml(int fromLine, int toLine) const
{
    if (fromLine==-1 || toLine==-1) {
        for (int i=0; i<data_.size(); i++) {
            if (data_[i].selected.contains(true)) {
                fromLine = i;
                break;
            }
        }
        for (int i=data_.size()-1; i>=0; i--) {
            if (data_[i].selected.contains(true)) {
                toLine = i;
                break;
            }
        }
    }
    fromLine = qMax(0, fromLine);
    toLine = qMin(data_.size()-1, toLine);
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
        marginLines << ( data_[i].marginText.isEmpty()? (data_[i].errors.isEmpty()? "" : data_[i].errors[0]) : data_[i].marginText );
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
    if (lineNo<0 || lineNo>=data_.size())
        return "";
    int indent = indentAt(lineNo);
    QString result;
    for (int i=0; i<indent; i++) {
        result += ". ";
    }
    QList<Shared::LexemType> highlight = data_[lineNo].highlight;
    QString text = data_[lineNo].text;
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
            c = settings_->value(SettingsPage::KeyColorKw, SettingsPage::DefaultColorKw).toString();
            chunks[i].bold = settings_->value(SettingsPage::KeyBoldKw, SettingsPage::DefaultBoldKw).toBool();
        }
        if (nameClass) {
            c = settings_->value(SettingsPage::KeyColorType,  SettingsPage::DefaultColorType).toString();
            chunks[i].bold = settings_->value(SettingsPage::KeyBoldType, SettingsPage::DefaultBoldType).toBool();
        }
        else if (nameAlg) {
            c = settings_->value(SettingsPage::KeyColorAlg,  SettingsPage::DefaultColorAlg).toString();
            chunks[i].bold = settings_->value(SettingsPage::KeyBoldAlg, SettingsPage::DefaultBoldAlg).toBool();
        }
        else if (nameModule) {
            c = settings_->value(SettingsPage::KeyColorMod,  SettingsPage::DefaultColorMod).toString();
            chunks[i].bold = settings_->value(SettingsPage::KeyBoldMod, SettingsPage::DefaultBoldMod).toBool();
        }
        else if (literalConstant) {
            c = settings_->value(SettingsPage::KeyColorLiteral,  SettingsPage::DefaultColorLiteral).toString();
            chunks[i].bold = settings_->value(SettingsPage::KeyBoldLiteral, SettingsPage::DefaultBoldLiteral).toBool();
        }
        else if (constant)
        {
            c = settings_->value(SettingsPage::KeyColorNumeric,  SettingsPage::DefaultColorNumeric).toString();
            chunks[i].bold = settings_->value(SettingsPage::KeyBoldNumeric, SettingsPage::DefaultBoldNumeric).toBool();
        }
        else if (doc) {
            c = settings_->value(SettingsPage::KeyColorDoc,  SettingsPage::DefaultColorDoc).toString();
            chunks[i].bold = settings_->value(SettingsPage::KeyBoldDoc, SettingsPage::DefaultBoldDoc).toBool();
        }
        else if (comment) {
            c = settings_->value(SettingsPage::KeyColorComment,  SettingsPage::DefaultColorComment).toString();
            chunks[i].bold = settings_->value(SettingsPage::KeyBoldComment, SettingsPage::DefaultBoldComment).toBool();
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
