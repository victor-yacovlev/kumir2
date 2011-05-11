#include "editorplane.h"
#include "textcursor.h"
#include "textdocument.h"
#include "clipboard.h"


namespace Editor {

EditorPlane::EditorPlane(TextDocument * doc
                         , TextCursor * cursor
                         , class Clipboard * clipboard
                         , QWidget *parent) :
    QWidget(parent)
{
    i_marginWidth = 15;
    m_document = doc;
    m_cursor = cursor;
    m_clipboard = clipboard;
    connect(m_cursor, SIGNAL(updateRequest()), this, SLOT(updateCursor()));
    connect(m_cursor, SIGNAL(updateRequest(int,int)), this, SLOT(updateText(int,int)));
    setFocusPolicy(Qt::StrongFocus);
    QFont defaultFont("DejaVu Sans Mono");
    defaultFont.setPointSize(12);
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

void EditorPlane::keyPressEvent(QKeyEvent *e)
{
    bool process = false;
    if (m_cursor->isEnabled()) {
        if (e->matches(QKeySequence::MoveToNextChar)) {
            m_cursor->movePosition(QTextCursor::NextCell, QTextCursor::MoveAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectNextChar)) {
            m_cursor->movePosition(QTextCursor::NextCell, QTextCursor::KeepAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToPreviousChar)) {
            m_cursor->movePosition(QTextCursor::PreviousCell, QTextCursor::MoveAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectPreviousChar)) {
            m_cursor->movePosition(QTextCursor::PreviousCell, QTextCursor::KeepAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToNextLine)) {
            m_cursor->movePosition(QTextCursor::NextRow, QTextCursor::MoveAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectNextLine)) {
            m_cursor->movePosition(QTextCursor::NextRow, QTextCursor::KeepAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToPreviousLine)) {
            m_cursor->movePosition(QTextCursor::PreviousRow, QTextCursor::MoveAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectPreviousLine)) {
            m_cursor->movePosition(QTextCursor::PreviousRow, QTextCursor::KeepAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToStartOfLine)) {
            m_cursor->movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectStartOfLine)) {
            m_cursor->movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToEndOfLine)) {
            m_cursor->movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectEndOfLine)) {
            m_cursor->movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToPreviousPage)) {
            int lines = height()/lineHeight() - 1;
            m_cursor->movePosition(QTextCursor::PreviousRow, QTextCursor::MoveAnchor, lines);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectPreviousPage)) {
            int lines = height()/lineHeight() - 1;
            m_cursor->movePosition(QTextCursor::PreviousRow, QTextCursor::KeepAnchor, lines);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToNextPage)) {
            int lines = height()/lineHeight() - 1;
            m_cursor->movePosition(QTextCursor::NextRow, QTextCursor::MoveAnchor, lines);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectNextPage)) {
            int lines = height()/lineHeight() - 1;
            m_cursor->movePosition(QTextCursor::NextRow, QTextCursor::KeepAnchor, lines);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToStartOfDocument)) {
            m_cursor->movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectStartOfDocument)) {
            m_cursor->movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToEndOfDocument)) {
            m_cursor->movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::SelectEndOfDocument)) {
            m_cursor->movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
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
            if (m_clipboard->hasContent()) {
                m_cursor->insertText(m_clipboard->content());
                process = true;
            }
        }
        else if (e->matches(QKeySequence::Copy)) {
            if (m_cursor->hasSelection()) {
                process = true;
                m_clipboard->push(m_cursor->selectedText());
            }
        }
        else if (e->matches(QKeySequence::Cut)) {
            if (m_cursor->hasSelection()) {
                process = true;
                m_clipboard->push(m_cursor->selectedText());
                m_cursor->removeSelectedText();
            }
        }
        else if (e->matches(QKeySequence::SelectAll)) {
            process = true;
            m_cursor->movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
            m_cursor->movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        }
        else if (e->key()==Qt::Key_Y && e->modifiers().testFlag(Qt::ControlModifier)) {
            process = true;
            m_cursor->removeCurrentLine();
        }
        else if (e->key()==Qt::Key_K && e->modifiers().testFlag(Qt::ControlModifier)) {
            process = true;
            m_cursor->removeLineTail();
        }
        else if (e->matches(QKeySequence::Delete)) {
            m_cursor->removeCurrentChar();
            process = true;
        }
        else if (!e->text().isEmpty()) {
            // TODO language switch
            m_cursor->insertText(e->text());
            process = true;
        }
    }
    if (process)
        e->accept();
    else
        e->ignore();
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
    QRect marginLine = QRect(widthInChars()*dX+1, 0, 2, height()).intersected(rect);
    if (marginLine.width()>0 && marginLine.height()>0) {
        p->setPen(QPen(QColor(255,0,0,128),2));
        p->drawLine(widthInChars()*dX+1, 0, widthInChars()*dX+1, height());
    }

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
    if (type & Shared::LxTypePrimaryKwd || type & Shared::LxTypeSecondaryKwd || type == Shared::LxNameClass) {
        f.setBold(true);
    }
    if (type == Shared::LxNameClass) {
        c = Qt::magenta;
    }
    else if (type == Shared::LxNameAlg) {
        c = Qt::blue;
    }
    else if (type == Shared::LxNameModule) {
        c = Qt::green;
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
    const int result = availableWidth / cw;
    return result;
}

} // namespace Editor
