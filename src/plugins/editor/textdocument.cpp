#include "editor.h"
#include "textdocument.h"
#include "textcursor.h"
#include "settingspage.h"

#include <stdint.h>

#include <kumir2-libs/extensionsystem/pluginmanager.h>

namespace Editor
{

TextDocument::TextDocument(EditorInstance * editor)
    : QObject(editor)
    , editor_(editor)
    , undoStack_(new QUndoStack(this))
{
    wasHiddenTextFlag_ = false;
    if (editor->analizer()) {
        _syntaxHighlightBehaviour = editor->analizer()->plugin()->syntaxHighlightBehaviour();
    }
    else {
        _syntaxHighlightBehaviour = AnalizerInterface::IndependentLinesHighlight;
    }
}

bool TextDocument::noUndoRedo = false;

void TextDocument::insertText(const QString &text, const Shared::Analizer::InstanceInterface * analizer, int line, int pos, int &blankLines, int &blankChars)
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
    uint nlOffset = text == "\n" ? 1 : 0;
    while (pos>int(tl.text.length() + nlOffset)) {
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
            data_[line].selected.insert(pos, false);
        while (data_[line].highlight.size() < data_[line].text.length())
            data_[line].highlight << Shared::LxTypeEmpty;
        if (analizer && AnalizerInterface::IndependentLinesHighlight==_syntaxHighlightBehaviour)
            data_[line].highlight = analizer->lineProp(line, data_[line].text).toList();

    }
    else {
        // 1. Append fragment to first line
        QString remainder = data_[line].text.mid(pos);
        data_[line].text = data_[line].text.left(pos)+lines[0];
        while (data_[line].selected.size() < data_[line].text.length())
            data_[line].selected << false;
        while (data_[line].highlight.size() < data_[line].text.length())
            data_[line].highlight << Shared::LxTypeEmpty;
        if (analizer && AnalizerInterface::IndependentLinesHighlight==_syntaxHighlightBehaviour)
            data_[line].highlight = analizer->lineProp(line, data_[line].text).toList();

        // 2. Insert middle lines
        for (int i=lines.count()-1; i>=1; i--) {
            TextLine tl;
            tl.changed = true;
            tl.inserted = true;
            tl.text = lines[i];
            tl.hidden = data_[line].hidden;
            tl.protecteed = data_[line].protecteed;
            for (int j=0; j<tl.text.length(); j++) {
                tl.selected << false;
                tl.highlight << Shared::LxTypeEmpty;
            }
            if (analizer && AnalizerInterface::IndependentLinesHighlight==_syntaxHighlightBehaviour)
                tl.highlight = analizer->lineProp(i, tl.text).toList();
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
        if (analizer && AnalizerInterface::IndependentLinesHighlight==_syntaxHighlightBehaviour)
            data_[line+lines.count()-1].highlight = analizer->lineProp(line+lines.count()-1, data_[line+lines.count()-1].text).toList();
    }

    if (analizer && AnalizerInterface::IndependentLinesHighlight!=_syntaxHighlightBehaviour) {
        int rehighlightStart =
                AnalizerInterface::CompleteTextHighlight==_syntaxHighlightBehaviour
                ? 0
                : line;
        int rehighlightEnd = data_.size();
        for (int i=rehighlightStart; i<rehighlightEnd; ++i) {
            data_[i].highlight = analizer->lineProp(i, data_[i].text).toList();
        }
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

const TextLine::Margin & TextDocument::marginAt(uint index) const
{
    if (index < data_.size()) {
        return data_.at(index).margin;
    }
    else {
        static const TextLine::Margin dummy = TextLine::Margin();
        return dummy;
    }
}

TextLine::Margin & TextDocument::marginAt(uint index)
{
    if (index < data_.size()) {
        return data_[index].margin;
    }
    else {
        static TextLine::Margin dummy;
        dummy.text.clear();
        dummy.errors.clear();
        return dummy;
    }
}

void TextDocument::removeText(QString &removedText, const Shared::Analizer::InstanceInterface *analizer, int line, int pos, int blankLines, int blankChars, int count)
{
    if (data_.size()==0)
        return;
    int cnt = count;
    int p = pos;
    int removedCounter = line;
    while (cnt>0 && line < data_.size()) {
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
            if (analizer && AnalizerInterface::IndependentLinesHighlight==_syntaxHighlightBehaviour)
                tl.highlight = analizer->lineProp(line, tl.text).toList();
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
    if (analizer && AnalizerInterface::IndependentLinesHighlight!=_syntaxHighlightBehaviour) {
        int rehighlightStart =
                AnalizerInterface::CompleteTextHighlight==_syntaxHighlightBehaviour
                ? 0
                : line;
        int rehighlightEnd = data_.size();
        for (int i=rehighlightStart; i<rehighlightEnd; ++i) {
            data_[i].highlight = analizer->lineProp(i, data_[i].text).toList();
        }
    }
}

void TextDocument::insertLine(const QString &text, const uint beforeLineNo)
{
    TextLine textLine;
    textLine.text = text;
    textLine.inserted = true;
    if (editor_->analizerInstance_ && AnalizerInterface::IndependentLinesHighlight==_syntaxHighlightBehaviour) {
        textLine.highlight = editor_->analizerInstance_->lineProp(qMin(beforeLineNo, uint(data_.size())), text).toList();
    }
    for (uint i=0; i<text.length(); i++) {
        textLine.selected.push_back(false);
    }
    data_.insert(qMin(beforeLineNo, uint(data_.size())), textLine);
    if (editor_->analizerInstance_ && AnalizerInterface::IndependentLinesHighlight!=_syntaxHighlightBehaviour) {
        int rehighlightStart =
                AnalizerInterface::CompleteTextHighlight==_syntaxHighlightBehaviour
                ? 0
                : beforeLineNo;
        int rehighlightEnd = data_.size();
        for (int i=rehighlightStart; i<rehighlightEnd; ++i) {
            data_[i].highlight = editor_->analizerInstance_->lineProp(i, data_[i].text).toList();
        }
    }
}

void TextDocument::removeLine(const uint lineNo)
{
    data_.removeAt(lineNo);
    removedLines_.insert(lineNo);
}

uint TextDocument::indentAt(uint lineNo) const
{
    int result = 0;
    for (uint i=0; i<qMin(lineNo, uint(data_.size()) ); i++) {
        result += data_[i].indentStart;
        result = qMax(0, result);
        result += data_[i].indentEnd;
    }
    if (lineNo < data_.size()) {
        result += data_[lineNo].indentStart;
        result = qMax(0, result);
    }
    return uint( qMax(result, 0) );
}

void TextDocument::setKumFile(const Shared::Analizer::SourceFileInterface::Data &d, bool showHiddenLines)
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

Shared::Analizer::SourceFileInterface::Data TextDocument::toKumFile() const
{
    Shared::Analizer::SourceFileInterface::Data kumfile;
    kumfile.hasHiddenText = false;
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
    if (kumfile.hiddenText.length() > 0)
        kumfile.hasHiddenText = true;
    return kumfile;
}


void TextDocument::checkForCompilationRequest(const QPoint &cursorPosition)
{   
    if (cursorPosition.y()!=lastCursorPos_.y()) {
        bool hasChangedLines = false;
        for (int i=0; i<data_.size(); i++) {
            if (data_[i].changed || data_[i].inserted) {
                hasChangedLines = true;
                break;
            }
        }
        bool hasRemovedLines = !removedLines_.isEmpty();
        if (hasChangedLines || hasRemovedLines) {
            forceCompleteRecompilation(cursorPosition);
        }
    }
    lastCursorPos_ = cursorPosition;
}


void TextDocument::forceCompleteRecompilation(const QPoint &cursorPosition)
{
    lastCursorPos_ = cursorPosition;
    removedLines_.clear();
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
    if (hiddenText_.length() > 0) {
        hiddenText += hiddenText_;
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
        marginLines << ( data_[i].margin.text.isEmpty()
                         ? (data_[i].margin.errors.isEmpty()
                            ? "" : data_[i].margin.errors[0]
                            )
                         : data_[i].margin.text
                           );
    }
    result = QString::fromLatin1(
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
        bool nonLatin = text[i]!='\0' && text[i].isLetter() && text[i].toLatin1()!='\0';
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
            c = editor_->mySettings()->value(SettingsPage::KeyColorKw, SettingsPage::DefaultColorKw).toString();
            chunks[i].bold = editor_->mySettings()->value(SettingsPage::KeyBoldKw, SettingsPage::DefaultBoldKw).toBool();
        }
        if (nameClass) {
            c = editor_->mySettings()->value(SettingsPage::KeyColorType,  SettingsPage::DefaultColorType).toString();
            chunks[i].bold = editor_->mySettings()->value(SettingsPage::KeyBoldType, SettingsPage::DefaultBoldType).toBool();
        }
        else if (nameAlg) {
            c = editor_->mySettings()->value(SettingsPage::KeyColorAlg,  SettingsPage::DefaultColorAlg).toString();
            chunks[i].bold = editor_->mySettings()->value(SettingsPage::KeyBoldAlg, SettingsPage::DefaultBoldAlg).toBool();
        }
        else if (nameModule) {
            c = editor_->mySettings()->value(SettingsPage::KeyColorMod,  SettingsPage::DefaultColorMod).toString();
            chunks[i].bold = editor_->mySettings()->value(SettingsPage::KeyBoldMod, SettingsPage::DefaultBoldMod).toBool();
        }
        else if (literalConstant) {
            c = editor_->mySettings()->value(SettingsPage::KeyColorLiteral,  SettingsPage::DefaultColorLiteral).toString();
            chunks[i].bold = editor_->mySettings()->value(SettingsPage::KeyBoldLiteral, SettingsPage::DefaultBoldLiteral).toBool();
        }
        else if (constant)
        {
            c = editor_->mySettings()->value(SettingsPage::KeyColorNumeric,  SettingsPage::DefaultColorNumeric).toString();
            chunks[i].bold = editor_->mySettings()->value(SettingsPage::KeyBoldNumeric, SettingsPage::DefaultBoldNumeric).toBool();
        }
        else if (doc) {
            c = editor_->mySettings()->value(SettingsPage::KeyColorDoc,  SettingsPage::DefaultColorDoc).toString();
            chunks[i].bold = editor_->mySettings()->value(SettingsPage::KeyBoldDoc, SettingsPage::DefaultBoldDoc).toBool();
        }
        else if (comment) {
            c = editor_->mySettings()->value(SettingsPage::KeyColorComment,  SettingsPage::DefaultColorComment).toString();
            chunks[i].bold = editor_->mySettings()->value(SettingsPage::KeyBoldComment, SettingsPage::DefaultBoldComment).toBool();
        }
        chunks[i].color = c.name();
        chunks[i].html = QString::fromLatin1(
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


namespace RTF {

static QByteArray rtfColor(const ExtensionSystem::SettingsPtr s,
                           const QString & key,
                           const QString & defaultt)
{
    const QString colorName = s->value(key, defaultt).toString();
    const QColor color(colorName);
    QByteArray result;
    result.append("\\red" + QString::number(color.red()).toLatin1());
    result.append("\\green" + QString::number(color.green()).toLatin1());
    result.append("\\blue" + QString::number(color.blue()).toLatin1());
    result.append(";");
    return result;
}

struct Chunk {
    QString text;
    bool bold;
    bool italic;
    bool error;
    uint color;
    uint32_t format;
    QByteArray data;
};

static QString screenCharacter(const QChar &ch)
{
    switch (ch.unicode()) {
    case '{':
        return "\\u123 ";
    case '}':
        return "\\u125 ";
    case '\\':
        return "\\u92 ";
    default:
        return ch;
    }
}

static QList<Chunk> splitLineIntoChunks(const ExtensionSystem::SettingsPtr s,
                                        const TextLine & textLine,
                                        const bool primaryAlphabetIsLatin
                                        )
{
    using namespace Shared;
    QList<Chunk> result;
    const QString & text = textLine.text;
    const QList<LexemType> & highlight = textLine.highlight;

    // Split text into chunks of various formats
    for (uint i=0; i<text.length(); i++) {
        bool latin = text[i]!='\0' &&
                text[i].isLetter() &&
                text[i].toLatin1()!='\0';
        if (result.isEmpty())
        {
            // Create first empty chunk of text
            Chunk chunk;
            chunk.bold = chunk.error = false;
            chunk.italic = latin && !primaryAlphabetIsLatin;
            chunk.format = highlight[i];
            chunk.text += screenCharacter(text[i]);
            result.push_back(chunk);
        }
        else if (result.last().format==highlight[i] &&
                 result.last().italic!=(latin && !primaryAlphabetIsLatin))
        {
            // Continue the same format chunk of text
            Chunk & chunk = result.last();
            chunk.text += screenCharacter(text[i]);
        }
        else
        {
            // Make new format chunk of text
            Chunk chunk;
            chunk.bold = chunk.error = false;
            chunk.italic = latin && !primaryAlphabetIsLatin;
            chunk.format = highlight[i];
            chunk.text += screenCharacter(text[i]);
            result.push_back(chunk);
        }
    }

    // Set proper formats to chunks
    static const uint32_t PriKwd = LxTypePrimaryKwd;
    static const uint32_t SecKwd = LxTypeSecondaryKwd;
    static const uint32_t NameClass = LxNameClass;
    static const uint32_t NameAlg = LxNameAlg;
    static const uint32_t NameModule = LxNameModule;
    static const uint32_t ConstLiteral = LxConstLiteral;
    static const uint32_t TypeConstant = LxTypeConstant;
    static const uint32_t TypeDoc = LxTypeDoc;
    static const uint32_t TypeComment = LxTypeComment;

    for (uint i=0; i<result.size(); i++) {
        Chunk & chunk = result[i];
        chunk.error = ( chunk.format & LxTypeError ) > 0;
        chunk.format = ( chunk.format << 1 ) >> 1;

        const uint32_t priKwd = PriKwd & chunk.format;
        const uint32_t secKwd = SecKwd & chunk.format;
        const uint32_t nameClass = NameClass == chunk.format;
        const uint32_t nameAlg = NameAlg == chunk.format;
        const uint32_t nameModule = NameModule == chunk.format;
        const uint32_t literalConstant = ConstLiteral == chunk.format;
        const uint32_t constant = TypeConstant & chunk.format;
        const uint32_t doc = TypeDoc == chunk.format;
        const uint32_t comment = TypeComment == chunk.format;

        if (priKwd || secKwd) {
            chunk.color = 1;
            chunk.bold = s->value(SettingsPage::KeyBoldKw,
                                  SettingsPage::DefaultBoldKw).toBool();
        }
        else if (nameClass) {
            chunk.color = 2;
            chunk.bold = s->value(SettingsPage::KeyBoldType,
                                  SettingsPage::DefaultBoldType).toBool();
        }
        else if (nameAlg) {
            chunk.color = 3;
            chunk.bold = s->value(SettingsPage::KeyBoldAlg,
                                  SettingsPage::DefaultBoldAlg).toBool();
        }
        else if (nameModule) {
            chunk.color = 4;
            chunk.bold = s->value(SettingsPage::KeyBoldMod,
                                  SettingsPage::DefaultBoldMod).toBool();
        }
        else if (literalConstant) {
            chunk.color = 5;
            chunk.bold = s->value(SettingsPage::KeyBoldLiteral,
                                  SettingsPage::DefaultBoldLiteral).toBool();
        }
        else if (constant) {
            chunk.color = 6;
            chunk.bold = s->value(SettingsPage::KeyBoldNumeric,
                                  SettingsPage::DefaultBoldNumeric).toBool();
        }
        else if (doc) {
            chunk.color = 7;
            chunk.bold = s->value(SettingsPage::KeyBoldDoc,
                                  SettingsPage::DefaultBoldDoc).toBool();
        }
        else if (comment) {
            chunk.color = 8;
            chunk.bold = s->value(SettingsPage::KeyBoldComment,
                                  SettingsPage::DefaultBoldComment).toBool();
        }
        else {
            chunk.color = 0;
            chunk.bold = false;
        }
        if (chunk.error) {
            chunk.color = 9;
        }
    }

    // Make RTF data for chunks
    for (uint i=0; i<result.size(); i++) {
        QByteArray & d = result[i].data;
        const Chunk & chunk = result[i];
        d.append("{");
        if (chunk.color)
            d.append("\\cf"+QString::number(chunk.color).toLatin1());
        if (chunk.bold)
            d.append("\\b");
        if (chunk.italic)
            d.append("\\i");
        if (chunk.error)
            d.append("\\ul");
        if (chunk.bold || chunk.italic || chunk.error || chunk.color)
            d.append(" ");

        static QTextCodec * codec = QTextCodec::codecForName("CP1251");
        d.append(codec->fromUnicode(chunk.text));

        d.append("}");
    }

    return result;
}

} // end namespace RTF

/**
 * @brief Get formatted text (with margins) as RTF
 * @param fromLine begin line number (inclusive)
 * @param toLine end line number (not inclusive)
 * @return RTF file data
 */
QByteArray TextDocument::toRtf(uint fromLine, uint toLine) const
{
    using namespace RTF;

    QByteArray result;

    // Check if margin data exists
    bool containsMarginText = false;
    for (uint i=fromLine; i<qMin(toLine+1, linesCount()); i++) {
        QString text;
        const TextLine::Margin & margin = marginAt(i);
        if (margin.text.length() > 0) {
            containsMarginText = true;
            break;
        }
        else {
            if (margin.errors.size() > 0) {
                containsMarginText = true;
                break;
            }
        }
    }

    using Shared::PluginInterface;
    PluginInterface::GlobalState gs = ExtensionSystem::PluginManager::instance()->currentGlobalState();
    if (PluginInterface::GS_Unlocked != gs) {
        containsMarginText = true;
    }

    // RTF header
    result.append("{\\rtf1\\ansicpg1251\r\n");

    const QFont usedFont = editor_->editorFont();

//    const QString fontName = usedFont.family();
    const QString fontName = "Courier New"; // TODO embed Kumir font into RTF data
    const int fontSize = usedFont.pointSize();

    // Font table
    result.append(QString(
                      "{\\fonttbl{\\f0\\fmodern\\fcharset204 %1;}}\r\n"
                      )
                  .arg(fontName)
                  );

    // Begin color table
    result.append("{\\colortbl;");

    result.append(rtfColor(editor_->mySettings(), SettingsPage::KeyColorKw, SettingsPage::DefaultColorKw));
    result.append(rtfColor(editor_->mySettings(), SettingsPage::KeyColorType, SettingsPage::DefaultColorType));
    result.append(rtfColor(editor_->mySettings(), SettingsPage::KeyColorAlg, SettingsPage::DefaultColorAlg));
    result.append(rtfColor(editor_->mySettings(), SettingsPage::KeyColorMod, SettingsPage::DefaultColorMod));
    result.append(rtfColor(editor_->mySettings(), SettingsPage::KeyColorLiteral, SettingsPage::DefaultColorLiteral));
    result.append(rtfColor(editor_->mySettings(), SettingsPage::KeyColorNumeric, SettingsPage::DefaultColorNumeric));
    result.append(rtfColor(editor_->mySettings(), SettingsPage::KeyColorDoc, SettingsPage::DefaultColorDoc));
    result.append(rtfColor(editor_->mySettings(), SettingsPage::KeyColorComment, SettingsPage::DefaultColorComment));
    result.append("\\red255;\\green0\\blue0;"); // for errors

    // End color table
    result.append("}\r\n");

    // Begin code style
    result.append(QString("{\\f0\\fs%1\\lang1024\r\n").arg(fontSize * 2));

    // Begin table row

    if (containsMarginText) {

        result.append("\\trowd\r\n");
        result.append("\\trgraph144\r\n");
        result.append("\\trautofit1\r\n");
        result.append("\\intbl\r\n");
        result.append("\\trftsWidth2\\trwWidth5000\r\n");
        result.append("\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\r\n");
        result.append("\\clNoWrap\\clftsWidth2\\clwWidth3000\\cellx6000\r\n");
        result.append("\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\r\n");
        result.append("\\clNoWrap\\clftsWidth2\\clwWidth2000\\cellx10000\r\n");

    }

    // Main program cell
    for (uint i=fromLine; i<qMin(toLine+1, linesCount()); i++) {
        if (containsMarginText)
            result.append("\\intbl");
        // Indent spaces
        if (this->indentAt(i) > 0) {
            result.append("{");
            for (uint j=0; j<this->indentAt(i); j++) {
                result.append("{\\b .} ");
            }
            result.append("}");
        }
        const TextLine & textLine = data_[i];
        bool primaryAlphabetIsLatin = editor_->analizer() && editor_->analizerPlugin_->primaryAlphabetIsLatin();
        const QList<RTF::Chunk> chunks = splitLineIntoChunks(
                    editor_->mySettings(),
                    textLine,
                    primaryAlphabetIsLatin
                    );
        for (uint j=0; j<chunks.size(); j++) {
            result.append(chunks[j].data);
        }
        if (fromLine!=toLine && i<toLine) {
            result.append(containsMarginText ? "\\par\r\n" : "\\line\r\n");
        }
    }

    if (containsMarginText) {

        // End main proram cell
        result.append("\\cell\r\n");

        // Margin cell
        for (uint i=fromLine; i<qMin(toLine+1, linesCount()); i++) {
            QString text;
            bool red = false;
            const TextLine::Margin & margin = marginAt(i);
            if (margin.text.length() > 0) {
                text = margin.text;
                red = false;
            }
            else {
                if (margin.errors.size() > 0) {
                    text = margin.errors.first();
                    red = true;
                }
            }
            static QTextCodec * codec = QTextCodec::codecForName("CP1251");
            result.append("\\intbl");
            if (text.length()>0) {
                result.append(QString("{\\fs%1 ").arg(fontSize * 2));
                if (red) {
                    result.append("\\cf9 ");
                }
                result.append(codec->fromUnicode(text));
                result.append("}");
            }
            if (fromLine!=toLine && i<toLine) {
                result.append("\\par\r\n");
            }
        }

        // End margin cell
        result.append("\\cell\r\n");

        // End table row
        result.append("\\row\r\n");

    } // end if (containsMarginText)

    // Code style end
    result.append("}\r\n");

    // RTF end
    result.append("}\r\n");
    return result;
}

}
