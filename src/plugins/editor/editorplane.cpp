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

void EditorPlane::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    paintBackground(&p, e->rect());
    paintSelection(&p, e->rect());
    paintText(&p, e->rect());
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
            m_cursor->movePosition(QTextCursor::NextCell,
                                   e->modifiers() & Qt::ShiftModifier
                                   ? QTextCursor::KeepAnchor
                                   : QTextCursor::MoveAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToPreviousChar)) {
            m_cursor->movePosition(QTextCursor::PreviousCell,
                                   e->modifiers() & Qt::ShiftModifier
                                   ? QTextCursor::KeepAnchor
                                   : QTextCursor::MoveAnchor);
            process = true;
        }
        if (e->matches(QKeySequence::MoveToNextLine)) {
            m_cursor->movePosition(QTextCursor::NextRow,
                                   e->modifiers() & Qt::ShiftModifier
                                   ? QTextCursor::KeepAnchor
                                   : QTextCursor::MoveAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToPreviousLine)) {
            m_cursor->movePosition(QTextCursor::PreviousRow,
                                   e->modifiers() & Qt::ShiftModifier
                                   ? QTextCursor::KeepAnchor
                                   : QTextCursor::MoveAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToStartOfLine)) {
            m_cursor->movePosition(QTextCursor::StartOfBlock,
                                   e->modifiers() & Qt::ShiftModifier
                                   ? QTextCursor::KeepAnchor
                                   : QTextCursor::MoveAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToEndOfLine)) {
            m_cursor->movePosition(QTextCursor::EndOfBlock,
                                   e->modifiers() & Qt::ShiftModifier
                                   ? QTextCursor::KeepAnchor
                                   : QTextCursor::MoveAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToPreviousPage)) {
            int lines = height()/lineHeight() - 1;
            m_cursor->movePosition(QTextCursor::PreviousRow,
                                   e->modifiers() & Qt::ShiftModifier
                                   ? QTextCursor::KeepAnchor
                                   : QTextCursor::MoveAnchor, lines);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToNextPage)) {
            int lines = height()/lineHeight() - 1;
            m_cursor->movePosition(QTextCursor::NextRow,
                                   e->modifiers() & Qt::ShiftModifier
                                   ? QTextCursor::KeepAnchor
                                   : QTextCursor::MoveAnchor, lines);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToStartOfDocument)) {
            m_cursor->movePosition(QTextCursor::Start,
                                   e->modifiers() & Qt::ShiftModifier
                                   ? QTextCursor::KeepAnchor
                                   : QTextCursor::MoveAnchor);
            process = true;
        }
        else if (e->matches(QKeySequence::MoveToEndOfDocument)) {
            m_cursor->movePosition(QTextCursor::End,
                                   e->modifiers() & Qt::ShiftModifier
                                   ? QTextCursor::KeepAnchor
                                   : QTextCursor::MoveAnchor);
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
    int row = m_cursor->row();
    int col = m_cursor->column();
    int dX = charWidth();
    int dY = lineHeight();
    if (m_cursor->mode()==TextCursor::EM_Overwrite)
        return QRect(col*dX, row*dY, dX, dY);
    else
        return QRect(col*dX, (row+1)*dY-1, dX, 2);
}

void EditorPlane::paintBackground(QPainter *p, const QRect &rect)
{
    p->save();
    p->setPen(Qt::NoPen);
    p->setBrush(QColor(Qt::white));
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
    QRegion selection = selectionRegion();
    if (!selection.isEmpty() && selection.intersects(rect)) {
        p->drawRects(selection.rects());
    }
    p->restore();
}

QRegion EditorPlane::selectionRegion() const
{
    QPoint start = m_cursor->selectionStart();
    QPoint end = m_cursor->selectionEnd();
    if (start.x()==-1 || end.x()==-1 || start.y()==-1 || end.y()==-1)
        return QRegion();
    QRegion result;
    for (int y=start.y() ; y<=end.y(); y++) {
        int yy = lineHeight() * y;
        int xx = 0;
        int ww = 0;
        if (y>start.y() && y<end.y()) {
            // middle lines of selection
            xx = 0;
            ww = 0;
            if (y<m_document->size() && !m_document->at(y).text.isEmpty()) {
                int indentSpace = charWidth() * 2 * m_document->indentAt(y);
                int textWidth = charWidth() * m_document->at(y).text.length();
                ww = indentSpace + textWidth;
            }
            else {
                ww = charWidth() / 2;
            }
        }
        else if (y==start.y() && y==end.y()) {
            // selection in single line
            xx = start.x() * charWidth();
            ww = (end.x()-start.x()) * charWidth();
        }
        else if (y==start.y()) {
            // first line of selection
            xx = start.x() * charWidth();
            ww = 0;
            if (y<m_document->size() && !m_document->at(y).text.isEmpty()) {
                int indentSpace = charWidth() * 2 * m_document->indentAt(y);
                int textWidth = charWidth() * m_document->at(y).text.length();
                ww = indentSpace + textWidth - xx;
            }
            else {
                ww = charWidth() / 2;
            }
        }
        else if (y==end.y()) {
            // last line of selection
            xx = 0;
            ww = end.x() * charWidth();
        }
        result += QRect(xx, yy, ww ,lineHeight());
    }
    return result;
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
