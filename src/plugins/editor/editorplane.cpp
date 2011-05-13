#include "editorplane.h"
#include "textcursor.h"
#include "textdocument.h"
#include "clipboard.h"
#include "settingspage.h"
#include "utils.h"

#define RECT_SELECTION_MODIFIER Qt::AltModifier


namespace Editor {

EditorPlane::EditorPlane(TextDocument * doc
                         , TextCursor * cursor
                         , class Clipboard * clipboard
                         , QSettings * settings
                         , QWidget *parent) :
    QWidget(parent)
{
    i_marginWidth = 15;
    m_document = doc;
    m_cursor = cursor;
    m_clipboard = clipboard;
    m_settings = settings;
    connect(m_cursor, SIGNAL(updateRequest()), this, SLOT(updateCursor()));
    connect(m_cursor, SIGNAL(updateRequest(int,int)), this, SLOT(updateText(int,int)));
    setFocusPolicy(Qt::StrongFocus);
    QFont defaultFont;
    defaultFont.setFamily(m_settings->value(SettingsPage::KeyFontName, SettingsPage::defaultFontFamily()).toString());
    defaultFont.setPointSize(m_settings->value(SettingsPage::KeyFontSize, SettingsPage::defaultFontSize).toInt());
    setFont(defaultFont);
}

QPoint EditorPlane::offset() const
{
    QPoint lineNumbersOffset(charWidth()*5, 0);
    QPoint totalOffset = lineNumbersOffset;
    return totalOffset;
}

void EditorPlane::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    paintBackground(&p, e->rect());

    p.save();

    p.translate(offset());

    paintSelection(&p, e->rect());
    paintRectSelection(&p, e->rect());
    paintText(&p, e->rect());

    p.restore();
    paintLineNumbers(&p, e->rect());
    paintCursor(&p, e->rect());
    e->accept();
}

int EditorPlane::charWidth() const
{
    const QFontMetrics fm(font());
    return fm.width('M');
}

void EditorPlane::updateCursor()
{
    update(cursorRect());
}

void EditorPlane::updateText(int fromLine, int toLine)
{
    QRect r(
                (fromLine-1) * lineHeight(),
                0,
                width(),
                (toLine+2) * lineHeight()
                );
    if (fromLine==-1 || toLine==-1)
        update(QRect(0,0,width(),height()));
    else
        update(r);
}

int EditorPlane::lineHeight() const
{
    const QFontMetrics fm(font());
    return fm.lineSpacing();
}



void EditorPlane::paintCursor(QPainter *p, const QRect &rect)
{
    p->save();
    QRect cr = cursorRect();
//    qDebug() << "Paint rect: " << rect;
    if (rect.intersects(cr) && m_cursor->isVisible()) {
        p->setPen(Qt::NoPen);
        p->setBrush(QColor(Qt::black));
        p->drawRect(cr);
    }
    p->restore();
}

void EditorPlane::keyReleaseEvent(QKeyEvent *e)
{
    Qt::Key tempSwichLayoutKey = Qt::Key(
                m_settings->value(
                    SettingsPage::KeyTempSwitchLayoutButton
                    , SettingsPage::DefaultTempSwitchLayoutButton)
                .toUInt()
                );
    if (e->key()==tempSwichLayoutKey) {
        Utils::temporaryLayoutSwitch = false;
    }
    if (m_cursor->isEnabled()) {
        e->accept();
    }
    else {
        e->ignore();
    }
}

void EditorPlane::keyPressEvent(QKeyEvent *e)
{
    bool process = false;
    if (m_cursor->isEnabled()) {
        if (e->matches(QKeySequence::MoveToNextChar)) {
            m_cursor->movePosition(QTextCursor::NextCell, TextCursor::MM_Move);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectNextChar)) {
            m_cursor->movePosition(QTextCursor::NextCell, TextCursor::MM_Select);
            process = true;
        }
        else if (e->key()==Qt::Key_Right && e->modifiers().testFlag(RECT_SELECTION_MODIFIER)) {
            m_cursor->movePosition(QTextCursor::NextCell, TextCursor::MM_RectSelect);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToPreviousChar)) {
            m_cursor->movePosition(QTextCursor::PreviousCell, TextCursor::MM_Move);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectPreviousChar)) {
            m_cursor->movePosition(QTextCursor::PreviousCell, TextCursor::MM_Select);
            process = true;
        }
        else if (e->key()==Qt::Key_Left && e->modifiers().testFlag(RECT_SELECTION_MODIFIER)) {
            m_cursor->movePosition(QTextCursor::PreviousCell, TextCursor::MM_RectSelect);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToNextLine)) {
            m_cursor->movePosition(QTextCursor::NextRow, TextCursor::MM_Move);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectNextLine)) {
            m_cursor->movePosition(QTextCursor::NextRow, TextCursor::MM_Select);
            process = true;
        }
        else if (e->key()==Qt::Key_Down && e->modifiers().testFlag(RECT_SELECTION_MODIFIER)) {
            m_cursor->movePosition(QTextCursor::NextRow, TextCursor::MM_RectSelect);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToPreviousLine)) {
            m_cursor->movePosition(QTextCursor::PreviousRow, TextCursor::MM_Move);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectPreviousLine)) {
            m_cursor->movePosition(QTextCursor::PreviousRow, TextCursor::MM_Select);
            process = true;
        }
        else if (e->key()==Qt::Key_Up && e->modifiers().testFlag(RECT_SELECTION_MODIFIER)) {
            m_cursor->movePosition(QTextCursor::PreviousRow, TextCursor::MM_RectSelect);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToStartOfLine)) {
            m_cursor->movePosition(QTextCursor::StartOfBlock, TextCursor::MM_Move);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectStartOfLine)) {
            m_cursor->movePosition(QTextCursor::StartOfBlock, TextCursor::MM_Select);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToEndOfLine)) {
            m_cursor->movePosition(QTextCursor::EndOfBlock, TextCursor::MM_Move);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectEndOfLine)) {
            m_cursor->movePosition(QTextCursor::EndOfBlock, TextCursor::MM_Select);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToPreviousPage)) {
            int lines = height()/lineHeight() - 1;
            m_cursor->movePosition(QTextCursor::PreviousRow, TextCursor::MM_Move, lines);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectPreviousPage)) {
            int lines = height()/lineHeight() - 1;
            m_cursor->movePosition(QTextCursor::PreviousRow, TextCursor::MM_Select, lines);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToNextPage)) {
            int lines = height()/lineHeight() - 1;
            m_cursor->movePosition(QTextCursor::NextRow, TextCursor::MM_Move, lines);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectNextPage)) {
            int lines = height()/lineHeight() - 1;
            m_cursor->movePosition(QTextCursor::NextRow, TextCursor::MM_Select, lines);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToStartOfDocument)) {
            m_cursor->movePosition(QTextCursor::Start, TextCursor::MM_Move);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectStartOfDocument)) {
            m_cursor->movePosition(QTextCursor::Start, TextCursor::MM_Select);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToEndOfDocument)) {
            m_cursor->movePosition(QTextCursor::End, TextCursor::MM_Move);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectEndOfDocument)) {
            m_cursor->movePosition(QTextCursor::End, TextCursor::MM_Select);
            process = true;
        }
        else if (e->matches(QKeySequence::InsertParagraphSeparator)) {
            m_cursor->insertText("\n");
            process = true;
        }
        else if (e->key()==Qt::Key_Backspace && e->modifiers()==0) {
            m_cursor->removePreviousChar();
            process = true;
        }
        else if (e->matches(QKeySequence::Paste)) {
            process = true;
            paste();
        }
        else if (e->matches(QKeySequence::Copy)) {
            process = true;
            copy();
        }
        else if (e->matches(QKeySequence::Cut)) {
            process = true;
            cut();
        }
        else if (e->matches(QKeySequence::SelectAll)) {
            process = true;
            selectAll();
        }
        else if (e->key()==Qt::Key_Y && e->modifiers().testFlag(Qt::ControlModifier)) {
            process = true;
            removeLine();
        }
        else if (e->key()==Qt::Key_K && e->modifiers().testFlag(Qt::ControlModifier)) {
            process = true;
            removeLineTail();
        }
        else if (e->matches(QKeySequence::Delete)) {
            m_cursor->removeCurrentChar();
            process = true;
        }
        else if (!e->text().isEmpty()) {
            m_cursor->insertText(Utils::textByKey(Qt::Key(e->key())
                                                  , e->text()
                                                  , e->modifiers().testFlag(Qt::ShiftModifier)));
            process = true;
        }

        Qt::Key tempSwichLayoutKey = Qt::Key(
                    m_settings->value(
                        SettingsPage::KeyTempSwitchLayoutButton
                        , SettingsPage::DefaultTempSwitchLayoutButton)
                    .toUInt()
                    );
        if (e->key()==tempSwichLayoutKey) {
            Utils::temporaryLayoutSwitch = true;
        }

    }
    if (process)
        e->accept();
    else
        e->ignore();
}

void EditorPlane::selectAll()
{
    m_cursor->movePosition(QTextCursor::Start, TextCursor::MM_Move);
    m_cursor->movePosition(QTextCursor::End, TextCursor::MM_Select);
}

void EditorPlane::copy()
{
    if (m_cursor->hasSelection()) {
        ClipboardData data;
        data.text = m_cursor->selectedText();
        data.type = ClipboardData::Text;
        m_clipboard->push(data);
    }
    else if (m_cursor->hasRectSelection()) {
        ClipboardData data;
        data.text = m_cursor->selectedText();
        data.type = ClipboardData::Block;
        data.block = m_cursor->rectSelectionText();
        m_clipboard->push(data);
    }
}

void EditorPlane::paste()
{
    if (m_clipboard->hasContent()) {
        ClipboardData data = m_clipboard->content();
        if (data.type == ClipboardData::Block) {
            m_cursor->insertBlock(data.block);
        }
        else if (data.type == ClipboardData::Text) {
            m_cursor->insertText(data.text);
        }
    }
}

void EditorPlane::cut()
{
    copy();
    if (m_cursor->hasSelection()) {
        m_cursor->removeSelectedText();
    }
    else if (m_cursor->hasRectSelection()) {
        m_cursor->clearSelectedBlock();
    }

}

void EditorPlane::removeLine()
{
    m_cursor->removeCurrentLine();
}

void EditorPlane::removeLineTail()
{
    m_cursor->removeLineTail();
}

QRect EditorPlane::cursorRect() const
{
    QPoint off = offset();
    int row = m_cursor->row();
    int col = m_cursor->column();
    int dX = charWidth();
    int dY = lineHeight();
    QRect result;
    if (m_cursor->mode()==TextCursor::EM_Overwrite)
        result = QRect(col*dX, row*dY, dX, dY);
    else
        result = QRect(col*dX, (row+1)*dY-1, dX, 2);
    result.translate(off);
//    qDebug() << "Cursor rect: " << result;
    return result;
}

void EditorPlane::paintBackground(QPainter *p, const QRect &rect)
{
    p->save();
    p->setPen(Qt::NoPen);
    p->setBrush(palette().brush(QPalette::Base));
    p->drawRect(rect);
    p->setPen(QPen(QColor(0,0,255,32),1));
    p->setBrush(Qt::NoBrush);
    const int dX = charWidth();
    const int dY = lineHeight();
    if (true) {
        // draw horizontal lines
        QRect lineRect;
        for (int y=dY; y<height(); y += dY) {
            lineRect = QRect(0, y, width(), 1).intersected(rect);
            if (lineRect.width()>0 && lineRect.height()>0) {
                p->drawLine(lineRect.topLeft(), lineRect.topRight());
            }
        }
        // draw vertical lines
        for (int x=dX; x<width(); x+= dX) {
            lineRect = QRect(x, 0, 1, height()).intersected(rect);
            if (lineRect.width()>0 && lineRect.height()>0) {
                p->drawLine(lineRect.topLeft(), lineRect.bottomLeft());
            }
        }
    }
    // draw margin line
    QRect marginLine = QRect((widthInChars()+5)*dX+1, 0, 2, height()).intersected(rect);
    if (marginLine.width()>0 && marginLine.height()>0) {
        p->setPen(QPen(QColor(255,0,0,128),2));
        p->drawLine((widthInChars()+5)*dX+1, 0, (widthInChars()+5)*dX+1, height());
    }

    p->restore();
}

void EditorPlane::paintRectSelection(QPainter *p, const QRect &)
{
    if (!m_cursor->hasRectSelection())
        return;
    p->save();
    p->setPen(QPen(palette().brush(QPalette::Highlight)
                   , 2
                   , Qt::SolidLine));
    p->setBrush(Qt::NoBrush);
    QRect selRect = m_cursor->selectionRect();
    QRect r(selRect.left() * charWidth(),
            selRect.top() * lineHeight(),
            selRect.width() * charWidth(),
            selRect.height() * lineHeight());

    p->drawRect(r);

    p->restore();
}

void EditorPlane::paintSelection(QPainter *p, const QRect &rect)
{
    p->save();
    p->setPen(Qt::NoPen);
    p->setBrush(palette().brush(QPalette::Highlight));
    int startLine = qMax(0, rect.top() / lineHeight() - 1);
    int endLine = rect.bottom() / lineHeight() + 1;
    int lh = lineHeight();
    int cw = charWidth();
    bool prevLineSelected = false;
    for (int i=startLine; i<endLine+1; i++) {
        if (i<m_document->size()) {
            int indentSpace = 2 * cw * m_document->indentAt(i);
            if (prevLineSelected) {
                p->drawRect(0, i*lh, indentSpace, lh);
            }
            for (int j=0; j<m_document->at(i).selected.size(); j++) {
                if (m_document->at(i).selected[j])
                    p->drawRect(indentSpace+j*cw, i*lh, cw, lh);
            }
            if (m_document->at(i).lineEndSelected) {
                prevLineSelected = true;
                int textLength = m_document->at(i).text.length()*cw;
                int xx = indentSpace + textLength;
                int ww = widthInChars()*cw-xx;
                p->drawRect(xx,
                            i*lh,
                            ww, lh);
//                qDebug() << "End line selected at " << i << " x  = " << xx << " w = " << ww;
            }
            else {
                prevLineSelected = false;
            }
        }
    }
    p->restore();
}

void EditorPlane::paintLineNumbers(QPainter *p, const QRect &rect)
{
    p->save();
    int startLine = rect.top() / lineHeight();
    int endLine = rect.bottom() / lineHeight() + 1;
    int lh = lineHeight();
    int cw = charWidth();
    for (int i=startLine; i<endLine+1; i++) {
        p->setPen(Qt::NoPen);
        p->setBrush(palette().brush(QPalette::Window));
        p->drawRect(0, i*lh, cw*4+cw/2, lh);
        p->setBrush(palette().brush(QPalette::Base));
        p->drawRect(cw*4+cw/2, i*lh, cw/2, lh);
        QColor textColor = QColor(palette().brush(QPalette::WindowText).color());
        if (i-1>=m_document->size()) {
            textColor = QColor(Qt::lightGray);
        }
        p->setPen(textColor);
        QString txt = QString::number(i);
        int tw = QFontMetrics(font()).width(txt);
        int xx = cw * 3 - tw;
        int yy = i * lh;
        p->drawText(xx, yy, txt);
    }
    p->restore();
}

void EditorPlane::paintText(QPainter *p, const QRect &rect)
{
    p->save();
    int startLine = rect.top() / lineHeight();
    int endLine = rect.bottom() / lineHeight() + 1;
    for (int i=startLine; i<endLine+1; i++) {
        int indent = m_document->indentAt(i);
        int y =  ( i + 1 )* lineHeight();
        p->setBrush(QColor(Qt::black));
        for (int j=0; j<indent; j++) {
            const int dotH = lineHeight()/5;
            const int dotW = charWidth()/5;
            const int dotX = j * charWidth() * 2 + (charWidth()-dotW);
            const int dotY = y - lineHeight() + (lineHeight()-dotH);
            QRect dotRect(dotX, dotY, dotW, dotH);
            p->drawRect(dotRect);
        }
    }
    for (int i=startLine; i<qMin(endLine+1, m_document->size()); i++) {
        int indent = m_document->indentAt(i);
        int y =  ( i + 1 )* lineHeight();
        QList<Shared::LexemType> highlight = m_document->at(i).highlight;
        Shared::LexemType curType = Shared::LexemType(0);
        setProperFormat(p, curType, '.');
        for (int j=0; j<m_document->at(i).text.size(); j++) {
            int offset = ( indent * 2 + j ) * charWidth();
            if (j<highlight.size()) {
                curType = highlight[j];
            }
            setProperFormat(p, curType, m_document->at(i).text[j]);
            int charW = QFontMetrics(p->font()).width(m_document->at(i).text[j]);
            if (charW<charWidth()) {
                offset += (charWidth()-charW)/2;
            }
            p->drawText(offset, y,  QString(m_document->at(i).text[j]));
        }

    }
    p->restore();
}

void EditorPlane::setProperFormat(QPainter *p, Shared::LexemType type, const QChar &ch)
{
    QFont f = font();
    QColor c = Qt::black;

    const quint32 t = (type << 1) >> 1; // remove error flag

    static const quint32 PriKwd = Shared::LxTypePrimaryKwd;
    static const quint32 SecKwd = Shared::LxTypeSecondaryKwd;
    static const quint32 NameClass = Shared::LxNameClass;
    static const quint32 NameAlg = Shared::LxNameAlg;
    static const quint32 NameModule = Shared::LxNameModule;
    static const quint32 ConstLiteral = Shared::LxConstLiteral;
    static const quint32 TypeConstant = Shared::LxTypeConstant;
    static const quint32 TypeDoc = Shared::LxTypeDoc;
    static const quint32 TypeComment = Shared::LxTypeComment;

    const quint32 priKwd = PriKwd & t;
    const quint32 secKwd = SecKwd & t;
    const quint32 nameClass = NameClass == t;
    const quint32 nameAlg = NameAlg == t;
    const quint32 nameModule = NameModule == t;
    const quint32 literalConstant = ConstLiteral == t;
    const quint32 constant = TypeConstant & t;
    const quint32 doc = TypeDoc == t;
    const quint32 comment = TypeComment == t;

    if (priKwd || secKwd) {
        c = m_settings->value(SettingsPage::KeyColorKw, SettingsPage::DefaultColorKw).toString();
        f.setBold(m_settings->value(SettingsPage::KeyBoldKw, SettingsPage::DefaultBoldKw).toBool());
    }
    if (nameClass) {
        c = m_settings->value(SettingsPage::KeyColorType,  SettingsPage::DefaultColorType).toString();
        f.setBold(m_settings->value(SettingsPage::KeyBoldType, SettingsPage::DefaultBoldType).toBool());
    }
    else if (nameAlg) {
        c = m_settings->value(SettingsPage::KeyColorAlg,  SettingsPage::DefaultColorAlg).toString();
        f.setBold(m_settings->value(SettingsPage::KeyBoldAlg, SettingsPage::DefaultBoldAlg).toBool());
    }
    else if (nameModule) {
        c = m_settings->value(SettingsPage::KeyColorMod,  SettingsPage::DefaultColorMod).toString();
        f.setBold(m_settings->value(SettingsPage::KeyBoldMod, SettingsPage::DefaultBoldMod).toBool());
    }
    else if (literalConstant) {
        c = m_settings->value(SettingsPage::KeyColorLiteral,  SettingsPage::DefaultColorLiteral).toString();
        f.setBold(m_settings->value(SettingsPage::KeyBoldLiteral, SettingsPage::DefaultBoldLiteral).toBool());
    }
    else if (constant)
    {
        c = m_settings->value(SettingsPage::KeyColorNumeric,  SettingsPage::DefaultColorNumeric).toString();
        f.setBold(m_settings->value(SettingsPage::KeyBoldNumeric, SettingsPage::DefaultBoldNumeric).toBool());
    }
    else if (doc) {
        c = m_settings->value(SettingsPage::KeyColorDoc,  SettingsPage::DefaultColorDoc).toString();
        f.setBold(m_settings->value(SettingsPage::KeyBoldDoc, SettingsPage::DefaultBoldDoc).toBool());
    }
    else if (comment) {
        c = m_settings->value(SettingsPage::KeyColorComment,  SettingsPage::DefaultColorComment).toString();
        f.setBold(m_settings->value(SettingsPage::KeyBoldComment, SettingsPage::DefaultBoldComment).toBool());
    }

    if (ch!='\0' && ch.isLetter() && ch.toAscii()!='\0') {
        f.setItalic(true);
    }
    p->setFont(f);
    p->setPen(c);
}

int EditorPlane::widthInChars() const
{
    const int cw = charWidth();
    const int marginMinWidth = cw * i_marginWidth;
    const int myWidth = width();
    const int availableWidth = myWidth - marginMinWidth;
    const int result = availableWidth / cw - 5;
    return result;
}

} // namespace Editor
