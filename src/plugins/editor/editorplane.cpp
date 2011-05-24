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
                         , const QList<QRegExp> &fileNamesToOpen
                         , QSettings * settings
                         , QScrollBar * horizontalSB
                         , QScrollBar * verticalSB
                         , QWidget *parent) :
    QWidget(parent)
{
    rxFilenamePattern = fileNamesToOpen;
    i_marginWidth = 15;
    i_marginAlpha = 255;
    m_document = doc;
    m_cursor = cursor;
    m_clipboard = clipboard;
    m_settings = settings;
    m_horizontalScrollBar = horizontalSB;
    m_verticalScrollBar = verticalSB;
    connect(m_cursor, SIGNAL(updateRequest()), this, SLOT(updateCursor()));
    connect(m_cursor, SIGNAL(updateRequest(int,int)), this, SLOT(updateText(int,int)));
    setFocusPolicy(Qt::StrongFocus);
    QFont defaultFont;
    defaultFont.setFamily(m_settings->value(SettingsPage::KeyFontName, SettingsPage::defaultFontFamily()).toString());
    defaultFont.setPointSize(m_settings->value(SettingsPage::KeyFontSize, SettingsPage::defaultFontSize).toInt());
    setFont(defaultFont);
    connect(horizontalSB, SIGNAL(valueChanged(int)), this, SLOT(update()));
    connect(verticalSB, SIGNAL(valueChanged(int)), this, SLOT(update()));
    updateScrollBars();
    initMouseCursor();
    setMouseTracking(true);
    setAcceptDrops(true);
    pnt_marginPress = pnt_textPress = pnt_dropPosMarker = pnt_dropPosCorner = QPoint(-1000, -1000);
    b_selectionInProgress = false;
}

void EditorPlane::paintDropPosition(QPainter *p)
{
    p->save();
    p->setPen(QPen(QColor(Qt::black), 2, Qt::SolidLine));
    if (pnt_dropPosMarker.x()!=-1000 && pnt_dropPosMarker.y()!=-1000) {
        int cw = charWidth();
        int lh = lineHeight();
        int x = offset().x() + cw * pnt_dropPosMarker.x();
        int y = offset().y() + lh * pnt_dropPosMarker.y();
        p->drawLine(x, y, x, y+lh);
    }
    if (pnt_dropPosCorner.x()!=-1000 && pnt_dropPosCorner.y()!=-1000) {
        int cw = charWidth();
        int lh = lineHeight();
        int x = offset().x() + cw * pnt_dropPosCorner.x();
        int y = offset().y() + lh * pnt_dropPosCorner.y();
        p->drawLine(x, y, x, y+lh);
        p->drawLine(x, y, x+cw, y);
    }
    p->restore();
}



void EditorPlane::mousePressEvent(QMouseEvent *e)
{
    int ln = charWidth() * 5;
    int mn = (widthInChars()+5)* charWidth();
    //    qDebug() << "ln = " << ln;
    //    qDebug() << "mn = " << mn;
    //    qDebug() << "x  = " << e->pos().x();
    m_cursor->setViewMode(TextCursor::VM_Hidden);
    pnt_marginPress = pnt_textPress = QPoint(-1000, -1000);
    if (e->pos().x()>=(mn-2) && e->pos().x()<=(mn+2)) {
        // Begin drag margin line
        pnt_marginPress = e->pos();
    }
    else if (e->pos().x()>ln && e->pos().x()<(mn-2)) {
        // Move cursor and (possible) begin selection
        pnt_textPress = e->pos();
        int realX = e->pos().x() - offset().x();
        int realY = e->pos().y() - offset().y();
        int textX = realX/charWidth();
        int textY = realY/lineHeight();
        m_cursor->moveTo(textY, textX);
        pos_textPress = QPoint(textX, textY);
        update();
    }
    e->accept();
}

void EditorPlane::mouseReleaseEvent(QMouseEvent *e)
{
    if (pnt_marginPress.x()!=-1000 && pnt_marginPress.y()!=-1000) {
        int x = pnt_marginPress.x();
        int cw = charWidth();
        x = (x/cw)*cw;
        int marginAbsoluteWidth = width()-x;
        i_marginWidth = marginAbsoluteWidth / cw;
        updateScrollBars();
        update();
        pnt_marginPress = QPoint(-1000, -1000);
    }
    if (b_selectionInProgress) {
        b_selectionInProgress = false;
    }
    else {
        m_cursor->removeSelection();
        m_cursor->removeRectSelection();
    }
    m_cursor->setViewMode(TextCursor::VM_Blinking);
    e->accept();
}

void EditorPlane::mouseMoveEvent(QMouseEvent *e)
{
    int ln = charWidth() * 5;
    int mn = ln + widthInChars() * charWidth();
    if (e->pos().x()<ln) {
//        QApplication::setOverrideCursor(Qt::ArrowCursor);
        QApplication::restoreOverrideCursor();
    }
    else if (e->pos().x()<=mn-2) {
        QApplication::restoreOverrideCursor();
    }
    else if (e->pos().x()<=mn+2) {
        QApplication::setOverrideCursor(Qt::SplitHCursor);
    }
    else {
//        QApplication::setOverrideCursor(Qt::ArrowCursor);
        QApplication::restoreOverrideCursor();
    }
    if (pnt_marginPress.x()!=-1000 && pnt_marginPress.y()!=-1000) {
        pnt_marginPress = e->pos();
        update();
    }
    else if (e->pos().x()>ln && e->pos().x()<mn-2 && e->buttons().testFlag(Qt::LeftButton)) {
        int dX = e->pos().x() - pnt_textPress.x();
        int dY = e->pos().y() - pnt_textPress.y();
        qreal distance = sqrt(dX*dX+dY*dY);
        qreal therehold = QApplication::startDragDistance();
        if (distance>=therehold) {
            bool sel = m_cursor->hasSelection();
            bool rsel = m_cursor->hasRectSelection();
//            qDebug() << "sel = " << sel;
//            qDebug() << "rsel = " << rsel;
            bool nothingSelected = !sel && !rsel;
//            qDebug() << "nothingSelected = " << nothingSelected;
            if (b_selectionInProgress || nothingSelected ) {
                b_selectionInProgress = true;
                int cw = charWidth();
                int lh = lineHeight();
                int realX = e->pos().x() - offset().x();
                int realY = e->pos().y() - offset().y();
                int textX = realX/cw;
                int textY = realY/lh;
                textX = qMax(textX, 0);
                textY = qMax(textY, 0);
                if (e->modifiers().testFlag(Qt::ShiftModifier)) {
                    QApplication::restoreOverrideCursor();
                    QApplication::setOverrideCursor(Qt::CrossCursor);
                    m_cursor->selectRangeBlock(pos_textPress.y(), pos_textPress.x(), textY, textX);
                }
                else {
                    QApplication::restoreOverrideCursor();
                    m_cursor->selectRangeText(pos_textPress.y(), pos_textPress.x(), textY, textX);
                }
                update();
            }
            else if (rsel || sel) {
                QMimeData * data = new QMimeData;
                if (sel) {
                    data->setText(m_cursor->selectedText());
                }
                if (rsel) {
                    data->setText(m_cursor->selectedText());
                    data->setData(
                                Clipboard::BlockMimeType
                                , m_cursor->rectSelectionText().join("\n").toUtf8());
                }
                QDrag * drag = new QDrag(this);
                drag->setMimeData(data);
                Qt::DropAction result = drag->exec(Qt::CopyAction|Qt::MoveAction, Qt::CopyAction);
                if (result==Qt::MoveAction && drag->target()!=this) {
                    if (sel) {
                        m_cursor->removeSelectedText();
                    }
                    else if (rsel) {
                        m_cursor->removeSelectedBlock();
                    }
                }
            }
        }
    }
    e->accept();
}

void EditorPlane::initMouseCursor()
{
//    QRect r0(0,0,charWidth(),lineHeight());
//    QRect r1(0,0,charWidth()-1,lineHeight()-1);
//    QImage img(r0.size(), QImage::Format_ARGB32);
//    img.fill(0);
//    QPainter p(&img);
//    p.setPen(QPen(QColor(Qt::black)));
//    p.drawRect(r1);
//    p.end();
//    QCursor c(QPixmap::fromImage(img));
    QCursor c(Qt::IBeamCursor);
    setCursor(c);
}

QPoint EditorPlane::offset() const
{
    QPoint lineNumbersOffset (charWidth()*5 , 0);
    QPoint scrollOffset(0,0);
    if (m_horizontalScrollBar->isEnabled()) {
        int valX = m_horizontalScrollBar->value();
        valX = ( valX / charWidth() ) * charWidth();
        scrollOffset.setX(-valX);
    }
    if (m_verticalScrollBar->isEnabled()) {
        int valY = m_verticalScrollBar->value();
        valY = ( valY / lineHeight() ) * lineHeight();
        scrollOffset.setY(-valY);
    }
    QPoint totalOffset = lineNumbersOffset + scrollOffset;
    return totalOffset;
}

void EditorPlane::updateScrollBars()
{
    QPoint prevOffset = offset();
    int w = 1;
    int h = 1;
    for (int i=0 ; i<m_document->size(); i++) {
        int indent = m_document->indentAt(i) * 2;
        int tl = m_document->at(i).text.length();
        w = qMax(w, indent+tl+1);
    }
    w = qMax(w, m_cursor->column()+1);
    h = qMax(m_document->size()+1, m_cursor->row()+2);

    QSize contentSize (w*charWidth(), h*lineHeight());
    QSize viewportSize (widthInChars() * charWidth(), height());
    if (contentSize.width()<=viewportSize.width()) {
        m_horizontalScrollBar->setEnabled(false);
    }
    else {
        m_horizontalScrollBar->setEnabled(true);
        m_horizontalScrollBar->setRange(0, contentSize.width()-viewportSize.width());
        m_horizontalScrollBar->setSingleStep(charWidth());
        m_horizontalScrollBar->setPageStep(charWidth() * 8);
    }
    if (contentSize.height()<=viewportSize.height()) {
        m_verticalScrollBar->setEnabled(false);
    }
    else {
        m_verticalScrollBar->setEnabled(true);
        m_verticalScrollBar->setRange(0, contentSize.height()-viewportSize.height());
        m_verticalScrollBar->setSingleStep(lineHeight());
        m_verticalScrollBar->setPageStep(lineHeight() * 8);
    }
    if (prevOffset!=offset())
        update();
}

void EditorPlane::ensureCursorVisible()
{
    QRect cr(5 + m_cursor->column(),
             m_cursor->row(),
             2,
             2
                );
    QRect vr;
    vr.setLeft(m_horizontalScrollBar->isEnabled()? m_horizontalScrollBar->value()/charWidth() : 0);
    vr.setTop(m_verticalScrollBar->isEnabled()? m_verticalScrollBar->value()/lineHeight() : 0);
    vr.setSize(QSize(widthInChars(), height()/lineHeight()));
//    qDebug() << "CR: " << cr;
//    qDebug() << "VR: " << vr;
    if (cr.left()>vr.right()) {
//        qDebug() << "A";
        int v = 6+m_cursor->column();
        m_horizontalScrollBar->setValue(v * charWidth());
    }
    else if (cr.right()<vr.left()) {
//        qDebug() << "B";
        int v = 4+m_cursor->column();
        m_horizontalScrollBar->setValue(v * charWidth());
    }
    if (cr.top()>vr.bottom()) {
//        qDebug() << "C";
        int v = m_cursor->row()+1;
        m_verticalScrollBar->setValue(v*lineHeight());
    }
    else if (cr.bottom()<vr.top()) {
//        qDebug() << "D";
        int v = m_cursor->row()-1;
        m_verticalScrollBar->setValue(v*lineHeight());
    }
}

void EditorPlane::findCursor()
{
    updateScrollBars();
    ensureCursorVisible();
}

void EditorPlane::paintEvent(QPaintEvent *e)
{
//    qDebug() << "My size : " << size();
    QPainter p(this);
    paintBackground(&p, e->rect());

    p.save();

    p.translate( offset() );

    paintSelection(&p, e->rect());
    paintRectSelection(&p, e->rect());
    paintText(&p, e->rect());

    p.restore();
    paintLineNumbers(&p, e->rect());
    paintCursor(&p, e->rect());

    paintMarginBackground(&p, e->rect());

    paintMarginText(&p, e->rect());

    paintNewMarginLine(&p);
    paintDropPosition(&p);

    e->accept();
}

void EditorPlane::paintNewMarginLine(QPainter *p)
{
    p->save();
    p->setPen(QColor(Qt::black));
    p->setBrush(Qt::NoBrush);
    int x = pnt_marginPress.x();
    int cw = charWidth();
    QRect marginLineRect(0,0,4,height());
    x = ( x / cw ) * cw;
    marginLineRect.translate(x,0);
    p->drawRect(marginLineRect);
    p->restore();
}

int EditorPlane::charWidth() const
{
    const QFontMetrics fm(font());
    return fm.width('M');
}

void EditorPlane::updateCursor()
{
//    update(cursorRect());
    update();
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
    cr.translate(offset());

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
        findCursor();
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
    findCursor();
}

void EditorPlane::cut()
{
    copy();
    if (m_cursor->hasSelection()) {
        m_cursor->removeSelectedText();
    }
    else if (m_cursor->hasRectSelection()) {
        m_cursor->removeRectSelection();
    }
    findCursor();
}

bool EditorPlane::canDrop(const QPoint &pos, const QMimeData *data) const
{
    if (!m_cursor->isEnabled()) {
        return false;
    }
    bool result = false;
    if (data->hasUrls()) {
        foreach (const QUrl &url, data->urls()) {
            const QString filename = url.toLocalFile();
            foreach (const QRegExp &rx, rxFilenamePattern) {
                if (rx.exactMatch(filename)) {
                    result = true;
                }
            }
        }
    }
    if (data->hasText() || data->hasFormat(Clipboard::BlockMimeType)) {
//        int ln = charWidth() * 5;
//        int mn = ln + widthInChars() * charWidth();
//        if (pos.x()>ln && pos.x()<mn) {
//            result = true;
//        }
        Q_UNUSED(pos);
        result = true;
    }

    return result;
}


void EditorPlane::dragEventHandler(QDragMoveEvent *e)
{
    m_cursor->setViewMode(TextCursor::VM_Hidden);
    if (canDrop(e->pos(), e->mimeData())) {
        if (e->source()==this) {
            e->setDropAction(Qt::MoveAction);
        }
        int col = (e->pos().x()-offset().x())/charWidth();
        int row = (e->pos().y()-offset().y())/lineHeight();
        col = qMax(col, 0);
        row = qMax(row, 0);
        if (e->mimeData()->hasFormat(Clipboard::BlockMimeType)) {
            pnt_dropPosCorner = QPoint(col, row);
            if (col>widthInChars()-1) {
                i_marginAlpha = 64;
            }
            else {
                i_marginAlpha = 255;
            }
        }
        else if (e->mimeData()->hasText()) {
            pnt_dropPosMarker = QPoint(col, row);
            if (col>widthInChars()-1) {
                i_marginAlpha = 64;
            }
            else {
                i_marginAlpha = 255;
            }
        }
        else {
            i_marginAlpha = 255;
            pnt_dropPosMarker = pnt_dropPosCorner = QPoint(-1000, -1000);
        }
        update();
        e->accept();
    }
    else {
        e->ignore();
    }
}

void EditorPlane::dropEvent(QDropEvent *e)
{
    m_cursor->setViewMode(TextCursor::VM_Blinking);
    pnt_dropPosMarker = pnt_dropPosCorner = QPoint(-1000, -1000);
    i_marginAlpha = 255;
    bool dropIntoSelection = false;
    int col = (e->pos().x()-offset().x())/charWidth();
    int row = (e->pos().y()-offset().y())/lineHeight();
    col = qMax(col, 0);
    row = qMax(row, 0);
    int fromRow, fromCol, toRow, toCol;
    if (m_cursor->hasSelection()) {
        m_cursor->selectionBounds(fromRow, fromCol, toRow, toCol);
        if (row>fromRow && row<toRow) {
            dropIntoSelection = true;
        }
        if (fromRow==toRow) {
            if (row==fromRow && col>=fromCol && col <toCol) {
                dropIntoSelection = true;
            }
        }
        else {
            if (row==fromRow && col>=fromCol) {
                dropIntoSelection = true;
            }
            if (row==toRow && col<toCol) {
                dropIntoSelection = true;
            }
        }
    }
    QRect r;
    if (m_cursor->hasRectSelection()) {
        r = m_cursor->selectionRect();
        dropIntoSelection = r.contains(col, row);
    }
    if (e->mimeData()->hasUrls()) {
        // TODO implement file open
        e->accept();
        return;
    }
    else if (e->mimeData()->hasFormat(Clipboard::BlockMimeType) || e->mimeData()->hasText()) {
        if (dropIntoSelection && e->source()==this) {
            // Do nothing - drop into dragged source
            e->accept();
            return;
        }
    }
    if (e->source()==this) {
        e->setDropAction(Qt::MoveAction);
    }
    e->accept();
    QStringList lines;
    QString text;
    m_cursor->setEmitCompilationBlocked(true);
    if (e->mimeData()->hasFormat(Clipboard::BlockMimeType)) {
        lines = QString::fromUtf8(e->mimeData()->data(Clipboard::BlockMimeType)).split("\n");
    }
    else if (e->mimeData()->hasText()) {
        text = e->mimeData()->text();
    }

    if (dropIntoSelection) {
        if (m_cursor->hasSelection()) {
            m_cursor->removeSelectedText();
        }
        if (m_cursor->hasRectSelection()) {
            m_cursor->removeSelectedBlock();
        }
    }
    else {
        if (e->source()==this) {
            if (m_cursor->hasSelection()) {
                m_cursor->removeSelectedText();
                if (row>=toRow) {
                    row -= text.count("\n");
                    if (row==toRow && col>=toCol) {
                        col -= text.split("\n").last().size();
                    }
                }
            }
            if (m_cursor->hasRectSelection()) {
                m_cursor->removeSelectedBlock();
//                if (row>=r.bottom()) {
//                    row -= r.height();
//                }
            }

        }
        else {
            if (m_cursor->hasSelection()) {
                m_cursor->removeSelection();
            }
            if (m_cursor->hasRectSelection()) {
                m_cursor->removeRectSelection();
            }
        }
    }
    m_cursor->moveTo(row, col);
    m_cursor->setEmitCompilationBlocked(false);
    if (e->mimeData()->hasFormat(Clipboard::BlockMimeType)) {
        m_cursor->insertBlock(lines);
    }
    else if (e->mimeData()->hasText()) {
        m_cursor->insertText(text);
    }
    update();
}

void EditorPlane::dragLeaveEvent(QDragLeaveEvent *e)
{
    m_cursor->setViewMode(TextCursor::VM_Blinking);
    pnt_dropPosMarker = pnt_dropPosCorner = QPoint(-1000, -1000);
    i_marginAlpha = 255;
    update();
    e->accept();
}

void EditorPlane::dragMoveEvent(QDragMoveEvent *e)
{
    dragEventHandler(e);
}

void EditorPlane::dragEnterEvent(QDragEnterEvent *e)
{
    dragEventHandler(e);
}

void EditorPlane::removeLine()
{
    m_cursor->removeCurrentLine();
    findCursor();
}

void EditorPlane::removeLineTail()
{
    m_cursor->removeLineTail();
    findCursor();
}

QRect EditorPlane::cursorRect() const
{
//    QPoint off = offset();
    int row = m_cursor->row();
    int col = m_cursor->column();
    int dX = charWidth();
    int dY = lineHeight();
    QRect result;
    if (m_cursor->mode()==TextCursor::EM_Overwrite)
        result = QRect(col*dX, row*dY, dX, dY);
    else
        result = QRect(col*dX, (row+1)*dY-1, dX, 2);
//    result.translate( off );
//    qDebug() << "Cursor rect: " << result;
    return result;
}

void EditorPlane::paintMarginBackground(QPainter *p, const QRect &rect)
{
    p->save();
    p->setPen(Qt::NoPen);
    QColor bgColor = palette().brush(QPalette::Base).color();
    bgColor.setAlpha(i_marginAlpha);
    p->setBrush(bgColor);

    const int dX = charWidth();
    const int dY = lineHeight();
    int marginLeft = (widthInChars()+5)*dX+1;
    QRect marginLine = QRect(marginLeft, 0, 2, height()).intersected(rect);
    p->drawRect(rect.intersected(QRect(marginLeft, 0, width()-marginLeft, height())));
    p->setPen(QPen(QColor(0,0,255,32*i_marginAlpha/256),1));
    p->setBrush(Qt::NoBrush);
    if (true) {
        // draw horizontal lines
        QRect lineRect;
        for (int y=dY; y<height(); y += dY) {
            lineRect = QRect(marginLeft, y, width(), 1).intersected(rect);
            if (lineRect.width()>0 && lineRect.height()>0) {
                p->drawLine(lineRect.topLeft(), lineRect.topRight());
            }
        }
        // draw vertical lines
//        for (int x=dX; x<width(); x+= dX) {
//            lineRect = QRect(x, 0, 1, height()).intersected(rect);
//            if (lineRect.width()>0 && lineRect.height()>0) {
//                p->drawLine(lineRect.topLeft(), lineRect.bottomLeft());
//            }
//        }
    }
    // draw margin line
    if (marginLine.width()>0 && marginLine.height()>0) {
        p->setPen(QPen(QColor(255,0,0,128*i_marginAlpha/256),2));
        p->drawLine((widthInChars()+5)*dX+1, 0, (widthInChars()+5)*dX+1, height());
    }
    p->restore();
}

void EditorPlane::paintBackground(QPainter *p, const QRect &rect)
{
    p->save();
    p->setPen(Qt::NoPen);
    p->setBrush(palette().brush(QPalette::Base).color());
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
    QRect r(selRect.left() * charWidth() + 1,
            selRect.top() * lineHeight() + 1,
            selRect.width() * charWidth() - 2,
            selRect.height() * lineHeight() -2 );

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
        if (i-1-offset().y()/lineHeight()>=m_document->size()) {
            textColor = QColor(Qt::lightGray);
        }
        p->setPen(textColor);
        QString txt = QString::number(i - (offset().y()/lineHeight()));
        int tw = QFontMetrics(font()).width(txt);
        int xx = cw * 3 - tw;
        int yy = i * lh;
        p->drawText(xx, yy, txt);
    }
    p->restore();
}

void EditorPlane::paintMarginText(QPainter * p, const QRect &rect)
{
    p->save();
    QColor textColor(Qt::red);
    textColor.setAlpha(i_marginAlpha);
    p->setPen(textColor);
    const int dX = charWidth();
    const int dY = lineHeight();
    int marginLeft = (widthInChars()+5)*dX+1;
    int startLine = rect.top()-offset().y() / lineHeight() - 1;
    int endLine = rect.bottom()-offset().y() / lineHeight() + 1;
    for (int i=qMax(startLine, 0); i<endLine+1; i++) {
        int y =  ( i + 1 )* dY;
        if (i<m_document->size() && m_document->at(i).errors.size()>0) {
            const QString errText = m_document->at(i).errors.size()>1
                        ? "> "+m_document->at(i).errors[0]
                        : m_document->at(i).errors[0];
            p->drawText(marginLeft+4, y, errText);
        }
    }
    p->restore();
}

void EditorPlane::paintText(QPainter *p, const QRect &rect)
{
    p->save();
    int startLine = rect.top()-offset().y() / lineHeight() - 1;
    int endLine = rect.bottom()-offset().y() / lineHeight() + 1;
    for (int i=qMax(startLine, 0); i<endLine+1; i++) {
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
    for (int i=qMax(startLine, 0); i<qMin(endLine+1, m_document->size()); i++) {
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
            if (curType & Shared::LxTypeError) {
                p->setPen(QColor(Qt::red));
                p->drawLine(offset, y+2, offset+charWidth(), y+2);
            }
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
