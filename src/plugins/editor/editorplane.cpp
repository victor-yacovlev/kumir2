#include "editorplane.h"
#include "textcursor.h"
#include "textdocument.h"
#include "editcommands.h"
#include "clipboard.h"
#include "settingspage.h"
#include "utils.h"
#include "autocompletewidget.h"


#define RECT_SELECTION_MODIFIER Qt::AltModifier

#define LOCK_SYMBOL_WIDTH 20

namespace Editor {

QString EditorPlane::MarginWidthKey = "MarginWidth";
int EditorPlane::MarginWidthDefault = 15;

EditorPlane::EditorPlane(TextDocument * doc
                         , Shared::AnalizerInterface * analizer
                         , TextCursor * cursor
                         , class Clipboard * clipboard
                         , const QList<QRegExp> &fileNamesToOpen
                         , QSettings * settings
                         , QScrollBar * horizontalSB
                         , QScrollBar * verticalSB
                         , bool hasAnalizer
                         , QWidget *parent) :
    QWidget(parent)
{
    e_backgroundMode = BgLines;
    m_analizer = analizer;
    i_highlightedLine = -1;
    i_grayLockSymbolLine = -1;
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    rxFilenamePattern = fileNamesToOpen;
    i_marginAlpha = 255;
    m_document = doc;
    m_cursor = cursor;
    m_clipboard = clipboard;
    m_settings = settings;
    m_horizontalScrollBar = horizontalSB;
    m_verticalScrollBar = verticalSB;
    b_hasAnalizer = hasAnalizer;
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
    setFocusPolicy(Qt::StrongFocus);
    pnt_delimeterPress = pnt_marginPress = pnt_textPress = pnt_dropPosMarker = pnt_dropPosCorner = QPoint(-1000, -1000);
    b_selectionInProgress = false;
    m_autocompleteWidget = new AutoCompleteWidget(this);
    m_autocompleteWidget->setVisible(false);
    connect(m_autocompleteWidget, SIGNAL(accepted(QString,QString)),
            this, SLOT(finishAutoCompletion(QString,QString)));
    qApp->installEventFilter(this);

    an_dontEdit = new QPropertyAnimation(this, "dontEditState", this);
    an_dontEdit->setDuration(1000);
    r_dontEditState = 0.0;
    img_dontEdit = QImage(qApp->property("sharePath").toString()+"/editor/dontedit.png");

}

void EditorPlane::addContextMenuAction(QAction *a)
{
    l_contextMenuActions << a;
}

void EditorPlane::setTeacherMode(bool v)
{
    b_teacherMode = v;
}

void EditorPlane::setLineHighlighted(int lineNo, const QColor &color)
{
    i_highlightedLine = lineNo;
    color_highlightedLine = color;
    if (lineNo>-1) {
        ensureHighlightedLineVisible();
    }
    update();
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

void EditorPlane::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu * menu = new QMenu(this);
    menu->addActions(l_contextMenuActions);
    menu->exec(e->globalPos());
    e->accept();
}

void EditorPlane::mousePressEvent(QMouseEvent *e)
{
    emit requestAutoScroll(0);
    if (m_autocompleteWidget->isVisible())
        m_autocompleteWidget->reject();
    if (e->button()==Qt::RightButton) {
        e->accept();
        return;
    }
    int lockSymbolWidth = b_teacherMode && b_hasAnalizer ? LOCK_SYMBOL_WIDTH : 0;
    int ln = charWidth() * 5 + lockSymbolWidth;
    int wc = widthInChars();
    int mn = (wc+5)* charWidth()+1;
//        qDebug() << "wc = " << ln;
//        qDebug() << "ln = " << ln;
//        qDebug() << "mn = " << mn;
//        qDebug() << "x  = " << e->pos().x();
    m_cursor->setViewMode(TextCursor::VM_Hidden);
    pnt_delimeterPress = pnt_marginPress = pnt_textPress = QPoint(-1000, -1000);
    if (b_hasAnalizer && b_teacherMode && e->pos().x() < lockSymbolWidth) {
        int realY = e->pos().y() - offset().y();
        int textY = qMax(0, realY/lineHeight());
        if (textY<m_document->linesCount()) {
            m_document->undoStack()->push(new ToggleLineProtectedCommand(m_document, textY));
            QApplication::restoreOverrideCursor();
        }
    }
    if (b_hasAnalizer && e->pos().x()>=(mn-2) && e->pos().x()<=(mn+2)) {
        // Begin drag margin line
        pnt_marginPress = e->pos();
    }
    else if (e->pos().x()>ln && e->pos().x()<(mn-2)) {
        bool moveDelimeterLine = false;
        if (b_hasAnalizer && b_teacherMode) {
            int hls = m_document->hiddenLineStart();
            QRect lr;
            if (hls==-1) {
                lr = QRect(charWidth()*5+lockSymbolWidth, height()-lineHeight(), widthInChars()*charWidth(), lineHeight());
            }
            else {
                lr = QRect(charWidth()*5+lockSymbolWidth, lineHeight()*hls-2, widthInChars()*charWidth(), 4);
            }
            moveDelimeterLine = lr.contains(e->pos());
        }
        if (moveDelimeterLine) {
            // Move line between visible and teacher text
            pnt_delimeterPress = e->pos();
            update();
        }
        else {
            // Move cursor and (possible) begin selection
            pnt_textPress = e->pos();
            int realX = e->pos().x() - offset().x();
            int realY = e->pos().y() - offset().y();
            int textX = realX/charWidth();
            int textY = realY/lineHeight();
            if (textY != m_cursor->row())
                m_document->flushTransaction();
            m_cursor->moveTo(textY, textX);
            pos_textPress = QPoint(textX, textY);
            update();
        }
    }
    e->accept();
}

void EditorPlane::mouseReleaseEvent(QMouseEvent *e)
{
    emit requestAutoScroll(0);
    if (pnt_marginPress.x()!=-1000 && pnt_marginPress.y()!=-1000) {
        int x = pnt_marginPress.x();
        if (b_teacherMode)
            x += LOCK_SYMBOL_WIDTH;
        int cw = charWidth();
        x = (x/cw)*cw;
        int marginAbsoluteWidth = width()-x;
        marginAbsoluteWidth = qMin(marginAbsoluteWidth, 450);
        marginAbsoluteWidth = qMax(marginAbsoluteWidth, cw);
        int marginWidth = marginAbsoluteWidth / cw;
        m_settings->setValue(MarginWidthKey, marginWidth);
        updateScrollBars();
        update();
        pnt_marginPress = QPoint(-1000, -1000);
    }
    if (pnt_delimeterPress.x()!=-1000 && pnt_delimeterPress.y()!=-1000) {
        int y = pnt_delimeterPress.y();
        int lh = lineHeight();
        int yy = (y/lh) ;
        if (y>height()-lh)
            yy = -1;
        m_document->undoStack()->push(new ChangeHiddenLineDelimeterCommand(
                                          m_document,
                                          yy));
        update();
        pnt_delimeterPress = QPoint(-1000, -1000);
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

bool EditorPlane::eventFilter(QObject *, QEvent *event)
{
    if (event->type()==QEvent::MouseMove) {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button()==Qt::LeftButton) {
            mouseMoveEvent(mouseEvent);
//            return true;
        }
    }
    else if (event->type()==QEvent::MouseButtonPress) {
        pnt_textPress = QPoint(-1000,-1000);
    }
    return false;
}

void EditorPlane::mouseMoveEvent(QMouseEvent *e)
{
    int lockSymbolWidth = b_teacherMode && b_hasAnalizer ? LOCK_SYMBOL_WIDTH : 0;
    int ln = charWidth() * 5 + lockSymbolWidth;
    int mn = ln + widthInChars() * charWidth() - lockSymbolWidth;
    i_grayLockSymbolLine = -1;
    bool moveDelimeterLine = false;
    if (b_hasAnalizer && b_teacherMode) {
        int hls = m_document->hiddenLineStart();
        QRect lr;
        if (hls==-1) {
            lr = QRect(charWidth()*5+lockSymbolWidth, height()-lineHeight(), widthInChars()*charWidth(), lineHeight());
        }
        else {
            lr = QRect(charWidth()*5+lockSymbolWidth, lineHeight()*hls-2, widthInChars()*charWidth(), 4);
        }
        moveDelimeterLine = lr.contains(e->pos());
    }

    if (b_teacherMode && b_hasAnalizer && e->pos().x()<lockSymbolWidth) {
        int lh = lineHeight();
        int realY = e->pos().y() - offset().y();
        int textY = realY/lh;
        textY = qMax(textY, 0);
        if (textY<m_document->linesCount()) {
            i_grayLockSymbolLine = textY;
//            QApplication::setOverrideCursor(Qt::PointingHandCursor);
        }
        else {
//            QApplication::restoreOverrideCursor();
        }

    }
    else if (e->pos().x()<ln) {
//        QApplication::setOverrideCursor(Qt::ArrowCursor);
        QApplication::restoreOverrideCursor();
    }
    else if (e->pos().x()<=mn-2) {
        QApplication::restoreOverrideCursor();
        if (moveDelimeterLine)
           QApplication::setOverrideCursor(Qt::SplitVCursor);
    }
    else if (b_hasAnalizer && e->pos().x()<=mn+2) {
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
    else if (pnt_delimeterPress.x()!=-1000 && pnt_delimeterPress.y()!=-1000) {
        pnt_delimeterPress = e->pos();
        update();
    }
    else if (b_selectionInProgress || (e->pos().x()>ln && e->pos().x()<mn-2 && e->buttons().testFlag(Qt::LeftButton))) {
        if (e->pos().y()<0) {
            emit requestAutoScroll(-1);
        }
        else if (e->pos().y()>height()) {
            emit requestAutoScroll(+1);
        }
        else {
            emit requestAutoScroll( 0);
        }
        QApplication::restoreOverrideCursor();
        int dX = e->pos().x() - pnt_textPress.x();
        int dY = e->pos().y() - pnt_textPress.y();
        qreal distance = sqrt(double(dX*dX+dY*dY));
        if (pnt_textPress.x()==-1000 && !b_selectionInProgress)
            distance = 0;
        qreal therehold = QApplication::startDragDistance();
        if (distance>=therehold) {
            bool sel = m_cursor->hasSelection();
            bool rsel = m_analizer==0 && m_cursor->hasRectSelection();
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
                if (m_analizer==0 && e->modifiers().testFlag(Qt::ShiftModifier)) {
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
                if (m_analizer==0 && rsel) {
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
    update();
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
    QPoint lockSymbolOffset (b_teacherMode? LOCK_SYMBOL_WIDTH : 0, 0);
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
    QPoint totalOffset = lineNumbersOffset + scrollOffset + lockSymbolOffset;
    return totalOffset;
}

void EditorPlane::updateScrollBars()
{
    QPoint prevOffset = offset();
    int w = 1;
    int h = 1;
    for (int i=0 ; i<m_document->linesCount(); i++) {
        int indent = m_document->indentAt(i) * 2;
        int tl = m_document->textAt(i).length();
        w = qMax(w, indent+tl+1);
    }
    w = qMax(w, m_cursor->column()+1);
    h = qMax(m_document->linesCount()+1, m_cursor->row()+2);

    QSize contentSize (w*charWidth(), h*lineHeight());
    QSize viewportSize (widthInChars() * charWidth(), height());
    if (contentSize.width()<=viewportSize.width()) {
        m_horizontalScrollBar->setEnabled(false);
        m_horizontalScrollBar->setVisible(false);
    }
    else {
        m_horizontalScrollBar->setEnabled(true);
        m_horizontalScrollBar->setVisible(true);
        m_horizontalScrollBar->setRange(0, contentSize.width()-viewportSize.width());
        m_horizontalScrollBar->setSingleStep(charWidth());
        m_horizontalScrollBar->setPageStep(charWidth() * 8);
    }
    if (contentSize.height()<=viewportSize.height()) {
        m_verticalScrollBar->setEnabled(false);
        m_verticalScrollBar->setVisible(false);
    }
    else {
        m_verticalScrollBar->setEnabled(true);
        m_verticalScrollBar->setVisible(true);
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

void EditorPlane::ensureHighlightedLineVisible()
{
    QRect cr(0,
             i_highlightedLine,
             2,
             2
                );
    QRect vr;
    vr.setLeft(m_horizontalScrollBar->isEnabled()? m_horizontalScrollBar->value()/charWidth() : 0);
    vr.setTop(m_verticalScrollBar->isEnabled()? m_verticalScrollBar->value()/lineHeight() : 0);
    vr.setSize(QSize(widthInChars(), height()/lineHeight()));

    if (cr.top()>vr.bottom()) {
        int v = i_highlightedLine;
        m_verticalScrollBar->setValue(v*lineHeight());
    }
    else if (cr.bottom()<vr.top()) {
        int v = i_highlightedLine;
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

    if (i_highlightedLine!=-1)
    {
        QRect highlightRect(0, lineHeight()*i_highlightedLine+1,
                            widthInChars()*charWidth(), lineHeight()+5);
        QLinearGradient gr(QPointF(0,0),QPointF(0,1));
        gr.setCoordinateMode(QGradient::ObjectBoundingMode);
        QColor c1 = color_highlightedLine.lighter();
        c1.setAlpha(32);
        QColor c2 = color_highlightedLine.lighter();

        gr.setColorAt(0, c1);
        gr.setColorAt(1, c2);
        p.setBrush(gr);
        p.setPen(Qt::NoPen);
        p.drawRect(highlightRect);
        p.setPen(color_highlightedLine);
        p.drawLine(highlightRect.topLeft(), highlightRect.topRight());
        p.drawLine(highlightRect.bottomLeft(), highlightRect.bottomRight());
        p.setPen(Qt::NoPen);
    }
    paintText(&p, e->rect());

    p.restore();
    paintLineNumbers(&p, e->rect());

    paintCursor(&p, e->rect());

    if (b_hasAnalizer) {
        paintMarginBackground(&p, e->rect());

        paintNewMarginLine(&p);
    }



    p.save();
    p.translate(offset());
    if (i_highlightedLine!=-1)
    {
        QRect highlightRect1(-offset().x(), lineHeight()*i_highlightedLine+1,
                            5*charWidth(), lineHeight()+5);
        QRect highlightRect2(charWidth()*widthInChars(), lineHeight()*i_highlightedLine+1,
                             marginCharactersCount()*widthInChars(), lineHeight()+5);
        QLinearGradient gr(QPointF(0,0),QPointF(0,1));
        gr.setCoordinateMode(QGradient::ObjectBoundingMode);
        QColor c1 = color_highlightedLine.lighter();
        c1.setAlpha(32);
        QColor c2 = color_highlightedLine.lighter();

        gr.setColorAt(0, c1);
        gr.setColorAt(1, c2);
        p.setBrush(gr);
        p.setPen(Qt::NoPen);
        p.drawRect(highlightRect1);
        p.drawRect(highlightRect2);
        p.setPen(color_highlightedLine);
        p.drawLine(highlightRect1.topLeft(), highlightRect1.topRight());
        p.drawLine(highlightRect1.bottomLeft(), highlightRect1.bottomRight());
        p.drawLine(highlightRect2.topLeft(), highlightRect2.topRight());
        p.drawLine(highlightRect2.bottomLeft(), highlightRect2.bottomRight());
    }
    p.restore();

    if (b_hasAnalizer) {
        paintMarginText(&p, e->rect());
    }

    paintDropPosition(&p);
    p.setBrush(Qt::NoBrush);
    const QBrush br = hasFocus()? palette().brush(QPalette::Highlight) : palette().brush(QPalette::Window);
    p.setPen(QPen(br,1));
    p.drawRect(0,0,width()-1,height()-1);
    if (b_teacherMode && b_hasAnalizer) {
        p.setPen(Qt::NoPen);
        QColor hidColor("#797979");
        hidColor.setAlpha(255);
        p.setBrush(hidColor);
//        p.drawRect(0, height()-lineHeight(), (widthInChars()+5)*charWidth(), lineHeight());
        p.drawRect(0, height()-lineHeight(), width(), lineHeight());
        paintHiddenTextDelimeterLine(&p);
        paintNewHiddenDelimeterLine(&p);
    }

    p.setOpacity(r_dontEditState);
    p.drawImage((width()-img_dontEdit.width())/2,
                (height()-img_dontEdit.height())/2,
                img_dontEdit);
    p.setOpacity(1.0);
    e->accept();

}

void EditorPlane::paintHiddenTextDelimeterLine(QPainter *p)
{
    if (b_teacherMode && b_hasAnalizer) {
        int hiddenLineStart = m_document->hiddenLineStart();

        int x1 = 5*charWidth()+LOCK_SYMBOL_WIDTH;
        int x2 = (widthInChars()+5)*charWidth();
        if (hiddenLineStart == -1) {
            p->setPen(QPen(QColor("white"), 2, Qt::DashLine));
            p->drawLine(x1, height()-lineHeight()/2, x2, height()-lineHeight()/2);
        }
        else {
            p->setPen(QPen(QColor("black"), 2, Qt::DashLine));
            int y = lineHeight()*hiddenLineStart;
            y += offset().y();
            p->drawLine(x1, y, x2, y);
        }
    }
}

void EditorPlane::paintNewMarginLine(QPainter *p)
{
    if (pnt_marginPress.x()!=-1000 && pnt_marginPress.y()!=-1000) {
        p->save();
        p->setPen(QColor(Qt::black));
        p->setBrush(Qt::NoBrush);
        int x = pnt_marginPress.x();
        int cw = charWidth();
        QRect marginLineRect(0,0,4,height());
        x = ( x / cw ) * cw;
        int marginAbsoluteWidth = width()-x;
//        int userWidth = marginAbsoluteWidth;
        marginAbsoluteWidth = qMin(marginAbsoluteWidth, 450);
        marginAbsoluteWidth = qMax(marginAbsoluteWidth, cw);

        marginLineRect.translate(width()-marginAbsoluteWidth-marginLineRect.width()/2,0);
        p->drawRect(marginLineRect);

//        if (userWidth!=marginAbsoluteWidth) {
//            p->setPen(QColor(Qt::gray));
//            p->setBrush(QColor(Qt::gray));
//            marginLineRect.setLeft(width()-userWidth-marginLineRect.width()/2);
//            marginLineRect.setRight(marginLineRect.left()+4);
//            p->drawRect(marginLineRect);
//        }

        p->restore();
    }
}

void EditorPlane::paintNewHiddenDelimeterLine(QPainter *p)
{
    p->save();
    p->setPen(QColor(Qt::black));
    p->setBrush(Qt::NoBrush);
    int y = pnt_delimeterPress.y();
    int lh = lineHeight();
    int x1 = 5*charWidth()+LOCK_SYMBOL_WIDTH;
    int x2 = (widthInChars()+5)*charWidth();
    QRect marginLineRect(x1,0,x2-x1,4);
    y = ( y / lh ) * lh;
    if (y>height()-lh) {
        y += lineHeight()/2-1;
        QColor cc(Qt::white);
        cc.setAlpha(64);
        p->setBrush(cc);
    }
    marginLineRect.translate(0,y);
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

bool EditorPlane::event(QEvent *e)
{
    if (e->type()==QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        if (ke->key()==Qt::Key_Tab || ke->key()==Qt::Key_Alt || ke->key()==Qt::Key_AltGr) {
            if (ke->type()==QEvent::KeyPress)
                keyPressEvent(ke);
            else
                keyReleaseEvent(ke);
            return true;
        }
    }
    return QWidget::event(e);
}

void EditorPlane::keyPressEvent(QKeyEvent *e)
{
    if (m_cursor->isEnabled()) {
        if (e->matches(QKeySequence::MoveToNextChar)) {
            m_cursor->evaluateCommand(KeyCommand::MoveToNextChar);
        }
        else if (e->matches(QKeySequence::SelectNextChar)) {
            m_cursor->evaluateCommand(KeyCommand::SelectNextChar);
        }
        else if (e->key()==Qt::Key_Right && e->modifiers().testFlag(RECT_SELECTION_MODIFIER) && m_analizer==0) {
            m_cursor->evaluateCommand(KeyCommand::SelectNextColumn);
        }
        else if (e->matches(QKeySequence::MoveToPreviousChar)) {
            m_cursor->evaluateCommand(KeyCommand::MoveToPreviousChar);
        }
        else if (e->matches(QKeySequence::SelectPreviousChar)) {
            m_cursor->evaluateCommand(KeyCommand::SelectPreviousChar);
        }
        else if (e->key()==Qt::Key_Left && e->modifiers().testFlag(RECT_SELECTION_MODIFIER) && m_analizer==0) {
            m_cursor->evaluateCommand(KeyCommand::SelectPreviousColumn);
        }
        else if (e->matches(QKeySequence::MoveToNextLine)) {
            m_cursor->evaluateCommand(KeyCommand::MoveToNextLine);
        }
        else if (e->matches(QKeySequence::SelectNextLine)) {
            m_cursor->evaluateCommand(KeyCommand::SelectNextLine);
        }
        else if (e->key()==Qt::Key_Down && e->modifiers().testFlag(RECT_SELECTION_MODIFIER) && m_analizer==0) {
            m_cursor->evaluateCommand(KeyCommand::SelectNextRow);
        }
        else if (e->matches(QKeySequence::MoveToPreviousLine)) {
            m_cursor->evaluateCommand(KeyCommand::MoveToPreviousLine);
        }
        else if (e->matches(QKeySequence::SelectPreviousLine)) {
            m_cursor->evaluateCommand(KeyCommand::SelectPreviousLine);
        }
        else if (e->key()==Qt::Key_Up && e->modifiers().testFlag(RECT_SELECTION_MODIFIER) && m_analizer==0) {
            m_cursor->evaluateCommand(KeyCommand::SelectPreviousRow);
        }
        else if (e->matches(QKeySequence::MoveToStartOfLine)) {
            m_cursor->evaluateCommand(KeyCommand::MoveToStartOfLine);
        }
        else if (e->matches(QKeySequence::SelectStartOfLine)) {
            m_cursor->evaluateCommand(KeyCommand::SelectStartOfLine);
        }
        else if (e->matches(QKeySequence::MoveToEndOfLine)) {
            m_cursor->evaluateCommand(KeyCommand::MoveToEndOfLine);
        }
        else if (e->matches(QKeySequence::SelectEndOfLine)) {
            m_cursor->evaluateCommand(KeyCommand::SelectEndOfLine);
        }
        else if (e->matches(QKeySequence::MoveToPreviousPage)) {
            m_cursor->evaluateCommand(KeyCommand::MoveToPreviousPage);
        }
        else if (e->matches(QKeySequence::SelectPreviousPage)) {
            m_cursor->evaluateCommand(KeyCommand::SelectPreviousPage);
        }
        else if (e->matches(QKeySequence::MoveToNextPage)) {
            m_cursor->evaluateCommand(KeyCommand::MoveToNextPage);
        }
        else if (e->matches(QKeySequence::SelectNextPage)) {
            m_cursor->evaluateCommand(KeyCommand::SelectNextPage);
        }
        else if (e->matches(QKeySequence::MoveToStartOfDocument)) {
            m_cursor->evaluateCommand(KeyCommand::MoveToStartOfDocument);
        }
        else if (e->matches(QKeySequence::SelectStartOfDocument)) {
            m_cursor->evaluateCommand(KeyCommand::SelectStartOfDocument);
        }
        else if (e->matches(QKeySequence::MoveToEndOfDocument)) {
            m_cursor->evaluateCommand(KeyCommand::MoveToEndOfDocument);
        }
        else if (e->matches(QKeySequence::SelectEndOfDocument)) {
            m_cursor->evaluateCommand(KeyCommand::SelectEndOfDocument);
        }
        else if (e->matches(QKeySequence::InsertParagraphSeparator)) {
            m_cursor->evaluateCommand("\n");
        }
        else if (e->key()==Qt::Key_Backspace && e->modifiers()==0) {
            m_cursor->evaluateCommand(KeyCommand::Backspace);
        }
        else if (e->matches(QKeySequence::Paste)) {
            m_cursor->evaluateCommand(KeyCommand::Paste);
        }
        else if (e->matches(QKeySequence::Copy)) {
            m_cursor->evaluateCommand(KeyCommand::Copy);
        }
        else if (e->matches(QKeySequence::Cut)) {
            m_cursor->evaluateCommand(KeyCommand::Cut);
        }
        else if (e->matches(QKeySequence::SelectAll)) {
            m_cursor->evaluateCommand(KeyCommand::SelectAll);
        }
        else if (e->key()==Qt::Key_Y && e->modifiers().testFlag(Qt::ControlModifier)) {
            m_cursor->evaluateCommand(KeyCommand::RemoveLine);
        }
        else if (e->key()==Qt::Key_K && e->modifiers().testFlag(Qt::ControlModifier)) {
            m_cursor->evaluateCommand(KeyCommand::RemoveTail);
        }
        else if (e->matches(QKeySequence::Delete)) {
            m_cursor->evaluateCommand(KeyCommand::Delete);
        }
        else if (e->matches(QKeySequence::Undo)) {
            m_cursor->undo();
        }
        else if (e->matches(QKeySequence::Redo)) {
            m_cursor->redo();
        }
        else if (e->key()==Qt::Key_Slash && e->modifiers().testFlag(Qt::ControlModifier)) {
            m_cursor->toggleComment();
        }
        else if (e->key()==Qt::Key_Tab || ( e->key()==Qt::Key_Space && e->modifiers().testFlag(Qt::ControlModifier) ) ) {
            if (b_hasAnalizer)
                doAutocomplete();
        }
        else if (!e->text().isEmpty()) {
            m_cursor->evaluateCommand(Utils::textByKey(Qt::Key(e->key())
                                                       , e->text()
                                                       , e->modifiers().testFlag(Qt::ShiftModifier)));
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
    if (e->key()>=Qt::Key_F1 && e->key()<=Qt::Key_F35)
        e->ignore();
    else
        e->accept();
}

void EditorPlane::doAutocomplete()
{
    QString source;
    QStringList algorhitms = m_analizer->algorhitmsAvailableFor(m_document->documentId, m_cursor->row());
    QStringList locals = m_analizer->localsAvailableFor(m_document->documentId, m_cursor->row());
    QStringList globals = m_analizer->globalsAvailableFor(m_document->documentId, m_cursor->row());

    m_cursor->removeSelection();
    m_cursor->removeRectSelection();

    if (m_cursor->row()<m_document->linesCount()) {
        QString line = m_document->textAt(m_cursor->row());
        int textPos = m_cursor->column() - 2 * m_document->indentAt(m_cursor->row());
        if (textPos <= line.length()) {
            for (int i=line.length(); i>0; i--) {
                QChar ch = line[i-1];
                if (ch.isLetterOrNumber() || ch=='_' || ch=='%')
                    source.prepend(ch);
                else
                    break;
            }
        }
    }

    m_autocompleteWidget->init(font(),
                               this,
                               source,
                               algorhitms,
                               locals,
                               globals
                               );
    if (m_autocompleteWidget->suggestionsCount()>0) {
        m_autocompleteWidget->move(cursorRect().topLeft()+QPoint(5*charWidth(), m_autocompleteWidget->offsetY()));
        m_autocompleteWidget->setVisible(true);
        m_autocompleteWidget->setFocus();
    }
}

void EditorPlane::finishAutoCompletion(const QString &source,
                                       const QString &newtext)
{
    for (int i=0; i<source.length(); i++) {
        m_cursor->evaluateCommand(KeyCommand::SelectPreviousChar);
    }
    m_cursor->evaluateCommand(KeyCommand(newtext));
}

void EditorPlane::selectAll()
{
    m_cursor->evaluateCommand(KeyCommand::SelectAll);
}

void EditorPlane::copy()
{
    m_cursor->evaluateCommand(KeyCommand::Copy);
}

void EditorPlane::paste()
{
    m_cursor->evaluateCommand(KeyCommand::Paste);
    findCursor();
}

void EditorPlane::cut()
{
    m_cursor->evaluateCommand(KeyCommand::Cut);
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
        if (e->mimeData()->hasUrls()) {
            i_marginAlpha = 255;
            pnt_dropPosMarker = pnt_dropPosCorner = QPoint(-1000, -1000);
        }
        else{
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
    m_document->undoStack()->beginMacro("dragndrop");
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
        QList<QUrl> urls = e->mimeData()->urls();
        QList<QUrl> validUrls;
        foreach (QUrl url, urls) {
            const QString fileName = url.toLocalFile();
            bool valid = false;
            foreach (QRegExp rx, rxFilenamePattern) {
                if (rx.exactMatch(fileName)) {
                    valid = true;
                    break;
                }
            }
            if (valid) {
                validUrls << url;
            }
        }
        if (!validUrls.isEmpty()) {
            emit urlsDragAndDropped(validUrls);
        }
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
                int afromRow, atoRow, afromCol, atoCol;
                m_cursor->selectionBounds(afromRow, afromCol, atoRow, atoCol);
                m_cursor->setRow(afromRow);
                m_cursor->setColumn(afromCol);
                if (row>=toRow) {
                    QString st = m_cursor->selectedText();
                    row -= st.count("\n");
                }
                m_cursor->removeSelectedText();
                m_cursor->setRow(toRow);
                m_cursor->setColumn(toCol);
//                if (row>=toRow) {
//                    row -= text.count("\n");
//                    if (row==toRow && col>=toCol) {
//                        col -= text.split("\n").last().size();
//                    }
//                }
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
    if (e->mimeData()->hasFormat(Clipboard::BlockMimeType)) {
        m_cursor->insertBlock(lines);
    }
    else if (e->mimeData()->hasText()) {
        m_cursor->insertText(text);
    }
    m_document->undoStack()->endMacro();
//    m_document->flushTransaction();
    m_document->forceCompleteRecompilation();

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

void EditorPlane::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    updateScrollBars();
}

void EditorPlane::focusInEvent(QFocusEvent *e)
{
    QWidget::focusInEvent(e);
    if (m_cursor->isEnabled()) {
        m_cursor->setViewMode(TextCursor::VM_Blinking);
    }
}

void EditorPlane::focusOutEvent(QFocusEvent *e)
{
    QWidget::focusOutEvent(e);
    if (m_cursor->isEnabled()) {
        m_cursor->setViewMode(TextCursor::VM_Hidden);
    }
}

void EditorPlane::removeLine()
{
    m_cursor->evaluateCommand(KeyCommand::RemoveLine);
    findCursor();
}

void EditorPlane::removeLineTail()
{
    m_cursor->evaluateCommand(KeyCommand::RemoveTail);
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
    if (e_backgroundMode==BgLines || e_backgroundMode==BgCells) {
        // draw horizontal lines
        QRect lineRect;
        for (int y=dY; y<height(); y += dY) {
            lineRect = QRect(0, y, width(), 1).intersected(rect);
            if (lineRect.width()>0 && lineRect.height()>0) {
                p->drawLine(lineRect.topLeft(), lineRect.topRight());
            }
        }
    }
    if (e_backgroundMode==BgCells) {
        // draw vertical lines
        QRect lineRect;
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
    Q_UNUSED(rect);
    p->save();
    p->setPen(Qt::NoPen);
    p->setBrush(palette().brush(QPalette::Highlight));
    int startLine = 0;
//    int startLine = qMax(0, rect.top() / lineHeight() - 1);
//    int endLine = rect.bottom() / lineHeight() + 1;
    int endLine = m_document->linesCount();
    int lh = lineHeight();
    int cw = charWidth();
    bool prevLineSelected = false;
    for (int i=startLine; i<endLine+1; i++) {
        if (i<m_document->linesCount()) {
            int indentSpace = 2 * cw * m_document->indentAt(i);
            if (prevLineSelected) {
                p->drawRect(0, i*lh, indentSpace, lh);
            }
            QList<bool> sm = m_document->selectionMaskAt(i);
            for (int j=0; j<sm.size(); j++) {
                if (sm[j])
                    p->drawRect(indentSpace+j*cw, i*lh, cw, lh);
            }
            if (m_document->lineEndSelectedAt(i)) {
                prevLineSelected = true;
                int textLength = m_document->textAt(i).length()*cw;
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
    int lockOffset = b_teacherMode? LOCK_SYMBOL_WIDTH : 0;
    for (int i=startLine; i<endLine+1; i++) {
        p->setPen(Qt::NoPen);
        p->setBrush(palette().brush(QPalette::Window));
        p->drawRect(0, i*lh, cw*4+cw/2+lockOffset, lh);
        p->setBrush(palette().brush(QPalette::Base));
        p->drawRect(cw*4+cw/2+lockOffset, i*lh, cw/2, lh);
        QColor textColor = QColor(palette().brush(QPalette::WindowText).color());
        if (i-1-offset().y()/lineHeight()>=m_document->linesCount()) {
            textColor = QColor(Qt::lightGray);
        }
        p->setPen(textColor);
        QString txt = QString::number(i - (offset().y()/lineHeight()));
        int tw = QFontMetrics(font()).width(txt);
        int xx = cw * 3 - tw + lockOffset;
        int yy = i * lh-2;
        p->drawText(xx, yy, txt);
        if (b_teacherMode) {
            if (i<m_document->linesCount() && m_document->isProtected(i))
                paintLockSymbol(p, true, QRect(0, i*lh, LOCK_SYMBOL_WIDTH, lh));
            if (i==i_grayLockSymbolLine && i<m_document->linesCount() && !m_document->isProtected(i))
                paintLockSymbol(p, false, QRect(0, i*lh, LOCK_SYMBOL_WIDTH, lh));
        }
    }
    p->restore();
}

void EditorPlane::paintLockSymbol(QPainter *p, bool colored, const QRect &r)
{
    p->save();
    p->setRenderHint(QPainter::Antialiasing, true);
    int W = r.width()-8;
    int H = r.height();
    int S = qMin(W, H);
    W = H = S;
    int X = r.left() + (r.width()-W)/2 + 2;
    int Y = r.top() + (r.height()-H)/2+2;
    p->setPen(QPen(QColor(colored? Qt::black : Qt::gray),1));

    if (colored) {
        p->setBrush(QColor(Qt::gray));
    }
    else {
        p->setBrush(Qt::NoBrush);
    }
    QPainterPath path;
    path.moveTo(X+W/8., Y+H/2.);
    path.arcTo(QRectF(X+W/8., Y, 0.75*W, H), 0, 180);
    path.lineTo(X+0.75*W, Y+H/2.);
    path.arcTo(QRectF(X+W/4., Y+H/8., .5*W, H), 0, 180);
    path.lineTo(X+W/8., Y+H/2.);
    path.closeSubpath();
    p->drawPath(path);
    if (colored) {
        p->setBrush(QColor(Qt::yellow));
    }
    else {
        p->setBrush(Qt::NoBrush);
    }
    p->drawRect(X,Y+H/2,W,H/2);

    p->drawPoint(QPoint(X+W/2, Y+H/4*3));
    p->restore();
}

void EditorPlane::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers().testFlag(Qt::ControlModifier)) {
        static const int minSize = 8;
        static const int maxSize = 36;
        QFont fnt = font();
        int currentSize = fnt.pointSize();
        int degrees = e->delta() / 8;
        int steps = degrees / 15;
        if (currentSize<=minSize && steps<0 ) {
            e->ignore();
            return;
        }
        if (currentSize>=maxSize && steps>0) {
            e->ignore();
            return;
        }
        currentSize += steps;
        currentSize = qMin(maxSize, qMax(minSize, currentSize));
        fnt.setPointSize(currentSize);
        m_settings->setValue(SettingsPage::KeyFontSize, currentSize);
        setFont(fnt);
        update();

    }
    if (!m_verticalScrollBar->isEnabled() && e->orientation()==Qt::Vertical) {
        e->ignore();
        return;
    }
    if (!m_horizontalScrollBar->isEnabled() && e->orientation()==Qt::Horizontal) {
        e->ignore();
        return;
    }
    int degrees = e->delta() / 8;
    int steps = degrees / 15;
    QScrollBar * sb = e->orientation()==Qt::Vertical? m_verticalScrollBar : m_horizontalScrollBar;
    sb->setValue(sb->value()-steps*sb->singleStep()*3);

}

void EditorPlane::paintMarginText(QPainter * p, const QRect &rect)
{
    int marginWidth = m_settings->value(MarginWidthKey, MarginWidthDefault).toInt();
    if (marginWidth<3)
        return;
    p->save();
    QColor errorColor(Qt::red);
    errorColor.setAlpha(i_marginAlpha);
    QColor marginColor(Qt::black);
    marginColor.setAlpha(i_marginAlpha);

    const int dX = charWidth();
    const int dY = lineHeight();
    int marginLeft = (widthInChars()+5)*dX+1;
    int startLine = rect.top()-offset().y() / lineHeight() - 1;
    int endLine = rect.bottom()-offset().y() / lineHeight() + 1;
    for (int i=qMax(startLine, 0); i<endLine+1; i++) {
        int y =  ( i + 1 )* dY;
        QString text;
        if (i<m_document->linesCount() && m_document->marginTextAt(i).length()>0) {
            text = m_document->marginTextAt(i);
            p->setPen(marginColor);
        }
        else if (i<m_document->linesCount() && m_document->errorsAt(i).size()>0) {
            text = m_document->errorsAt(i)[0];
            p->setPen(errorColor);
        }
        if (text.length()>0)
            p->drawText(marginLeft+4, y+offset().y(), text);
    }
    p->restore();
}

void EditorPlane::paintText(QPainter *p, const QRect &rect)
{
    p->save();
    int startLine = rect.top()-offset().y() / lineHeight() - 1;
    int endLine = rect.bottom()-offset().y() / lineHeight() + 1;
    int hiddenLineStart = -1;
    if (b_teacherMode && b_hasAnalizer)
    for (int i=0; i<m_document->linesCount(); i++) {
        if (m_document->isHidden(i)) {
            hiddenLineStart = i;
            break;
        }
    }
    for (int i=qMax(startLine, 0); i<endLine+1; i++) {
        int y =  ( i )* lineHeight();
        bool drawThisRect = false;
        QRect specialLineRect (0, y, width(), lineHeight());
        if (i<m_document->linesCount() && m_document->isProtected(i) && !m_document->isHidden(i)) {
            QColor protColor("#157962");
            protColor.setAlpha(64);
            p->setBrush(protColor);
            drawThisRect = true;
        }
        else if (hiddenLineStart!=-1 && i>=hiddenLineStart) {
            QColor hidColor("black");
            hidColor.setAlpha(64);
            p->setBrush(hidColor);
            drawThisRect = true;
        }
        if (drawThisRect) {
            p->setPen(Qt::NoPen);
            p->drawRect(specialLineRect);
        }
    }
    for (int i=qMax(startLine, 0); i<endLine+1; i++) {
        int indent = m_document->indentAt(i);
        int y =  ( i + 1 )* lineHeight();
        p->setBrush(QColor(Qt::black));
        p->setPen(Qt::NoPen);
        for (int j=0; j<indent; j++) {
            const int dotH = lineHeight()/5;
//            const int dotW = charWidth()/5;
            const int dotW = dotH;
            const int dotX = j * charWidth() * 2 + (charWidth()-dotW);
            const int dotY = y - lineHeight() + (lineHeight()-dotH);
            QRect dotRect(dotX, dotY, dotW, dotH);
            p->drawRect(dotRect);
        }
    }
    for (int i=qMax(startLine, 0); i<qMin(endLine+1, m_document->linesCount()); i++) {
        int indent = m_document->indentAt(i);
        int y =  ( i + 1 )* lineHeight();


        QList<Shared::LexemType> highlight = m_document->highlightAt(i);
        QString text = m_document->textAt(i);
        
        QList<bool> sm = m_document->selectionMaskAt(i);
        Shared::LexemType curType = Shared::LexemType(0);
        setProperFormat(p, curType, '.');
        for (int j=0; j<text.size(); j++) {
            int offset = ( indent * 2 + j ) * charWidth();
            if (j<highlight.size()) {
                curType = highlight[j];
            }
            setProperFormat(p, curType, text[j]);
            if (j<sm.size() && sm[j]) {
                p->setPen(palette().brush(QPalette::HighlightedText).color());
            }
            if (i_highlightedLine==i) {
                p->setPen(p->pen().color().darker());
            }

            int charW = QFontMetrics(p->font()).width(text[j]);
            if (charW<charWidth()) {
                offset += (charWidth()-charW)/2;
            }
            if (curType==LxTypeComment && text[j]=='|') {
                p->setPen(QPen(p->pen().brush(),2));
                p->drawLine(offset, y, offset, y-lineHeight()+2);
            }
            else {
                p->drawText(offset, y,  QString(text[j]));
            }
            if (curType & Shared::LxTypeError) {
                p->setPen(QPen(QColor(Qt::red),1));
                QPolygon pp = errorUnderline(offset, y+2, charWidth());
//                p->drawLine(offset, y+2, offset+charWidth(), y+2);
                p->drawPolyline(pp);
            }
        }

    }
    p->restore();
}

void EditorPlane::signalizeNotEditable()
{
    qDebug() << "AAAAA!!!!";
    an_dontEdit->stop();
    an_dontEdit->setStartValue(1.0);
    an_dontEdit->setEndValue(0.0);
    r_dontEditState = 1.0;
    an_dontEdit->start();
}

QPolygon EditorPlane::errorUnderline(int x, int y, int len)
{
    QVector<QPoint> points = QVector<QPoint>(5);
    points[0] = QPoint(x,y);
    points[1] = QPoint(x+len/4, y+2);
    points[2] = QPoint(x+len/2, y);
    points[3] = QPoint(x+len/4*3, y+2);
    points[4] = QPoint(x+len,y);
    return QPolygon(points);
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

    QFont::Capitalization caseInsensitiveKw = QFont::AllLowercase;
    QFont::Capitalization caseInsensitiveType = QFont::MixedCase;

    if (priKwd || secKwd) {
        c = m_settings->value(SettingsPage::KeyColorKw, SettingsPage::DefaultColorKw).toString();
        f.setBold(m_settings->value(SettingsPage::KeyBoldKw, SettingsPage::DefaultBoldKw).toBool());
        if (m_analizer->caseInsensitiveGrammatic()) {
            f.setCapitalization(caseInsensitiveKw);
        }
    }
    if (nameClass) {
        c = m_settings->value(SettingsPage::KeyColorType,  SettingsPage::DefaultColorType).toString();
        f.setBold(m_settings->value(SettingsPage::KeyBoldType, SettingsPage::DefaultBoldType).toBool());
        if (m_analizer->caseInsensitiveGrammatic()) {
            f.setCapitalization(caseInsensitiveType);
        }
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

    if (m_analizer && !m_analizer->primaryAlphabetIsLatin() && ch!='\0' && ch.isLetter() && ch.toAscii()!='\0') {
        f.setItalic(true);
    }
    p->setFont(f);
    p->setPen(c);
}

int EditorPlane::widthInChars() const
{
    const int cw = charWidth();
    int marginMinWidth = cw * m_settings->value(MarginWidthKey, MarginWidthDefault).toInt();;
    if (!b_hasAnalizer)
        marginMinWidth = 0;
    const int myWidth = width();
    const int lockSymbolWidth = (b_hasAnalizer && b_teacherMode)? LOCK_SYMBOL_WIDTH : 0;
    const int availableWidth = myWidth - marginMinWidth - lockSymbolWidth;
    const int result = availableWidth / cw - 5;
//    qDebug() << "myWidth = " << myWidth;
//    qDebug() << "marginMinWidth = " << marginMinWidth;
//    qDebug() << "availableWidth = " << availableWidth;
//    qDebug() << "charWidth = " << cw;
//    qDebug() << "widthInChars() = " << result;
    return result;
}

} // namespace Editor
