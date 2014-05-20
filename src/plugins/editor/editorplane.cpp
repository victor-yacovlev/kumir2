#include "editorplane.h"
#include "textcursor.h"
#include "textdocument.h"
#include "editcommands.h"
#include "clipboard.h"
#include "settingspage.h"
#include "utils.h"
#include "suggestionswindow.h"
#include "editor.h"
#ifdef Q_OS_UNIX
#include <unistd.h>
#endif
#include <stdint.h>


namespace Editor {

static const Qt::KeyboardModifier RECT_SELECTION_MODIFIER  = Qt::AltModifier;
static const uint LOCK_SYMBOL_WIDTH = 20u /*px*/;
static const uint HIDDEN_DELIMETER_LINE_HEIGHT = 4u /*px*/;
static const uint LEFT_MARGIN_SIZE = 5u /*symbols*/;
static const uint HIGHTLIGHT_LINE_VERTICAL_PADDING = 5u /*px*/;
static const QColor PROTECTED_LINE_BACKGROUND(0x15, 0x79, 0x63, 0x40);
static const QColor HIDDEN_LINE_BACKGROUND(0x00, 0x00, 0x00, 0x40);
static const uint MARGIN_LINE_WIDTH = 3u /*px*/;

QString EditorPlane::MarginWidthKey = "MarginWidth";
uint EditorPlane::MarginWidthDefault = 15u /*px*/;

EditorPlane::EditorPlane(EditorInstance * editor)
    : QWidget(editor)
    , editor_(editor)
    , highlightedTextLineNumber_(-1)
    , highlightedTextColumnStartNumber_(0u)
    , highlightedTextColumnEndNumber_(0u)
    , highlightedLockSymbolLineNumber_ (-1)
    , marginBackgroundAlpha_(255)
    , delimeterRuleMousePressedPoint_(QPoint(-1000, -1000))
    , marginMousePressedPoint_(QPoint(-1000, -1000))
    , textMousePressedPoint_(QPoint(-1000, -1000))
    , pnt_dropPosMarker(QPoint(-1000, -1000))
    , pnt_dropPosCorner(QPoint(-1000, -1000))
    , selectionInProgressFlag_(false)
    , marginHintBox_(new QLabel(this, Qt::ToolTip))
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setFocusPolicy(Qt::StrongFocus);

    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);
    setAcceptDrops(true);          
    initMouseCursor();

}

void EditorPlane::updateSettings(const QStringList & keys)
{
    if (keys.isEmpty() || keys.contains(SettingsPage::KeyFontName) || keys.contains(SettingsPage::KeyFontSize)) {
        QFont defaultFont;
        defaultFont.setFamily(editor_->mySettings()->value(SettingsPage::KeyFontName, SettingsPage::defaultFontFamily()).toString());
        defaultFont.setPointSize(editor_->mySettings()->value(SettingsPage::KeyFontSize, SettingsPage::defaultFontSize).toInt());
        setFont(defaultFont);
        marginHintBox_->setFont(defaultFont);
    }
}

void EditorPlane::addContextMenuAction(QAction *a)
{
    contextMenuActions_ << a;
}


void EditorPlane::setLineHighlighted(int lineNo, const QColor &color, quint32 colStart, quint32 colEnd)
{
    highlightedTextLineNumber_ = lineNo;
    highlightedTextLineColor_ = color;
    highlightedTextColumnStartNumber_ = colStart;
    highlightedTextColumnEndNumber_ = colEnd;
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
    menu->addActions(contextMenuActions_);
    menu->exec(e->globalPos());
    e->accept();
}

/** Handles mouse press event
 * @param e an event gone throught event system
 */
void EditorPlane::mousePressEvent(QMouseEvent *e)
{
    emit message(QString());
    // Ensure auto scrolling by timer is stopped
    emit requestAutoScroll(0);
    emit requestAutoScrollX(0);

    // Auto complete widget is a tool tip, so it must be hidden if
    // use clicks somewhere in editor
    if (editor_->autocompleteWidget_->isVisible())
        editor_->autocompleteWidget_->hide();

    // Do nothing on middle or right mouse button.
    // Right mouse button is used for context menu and processed by
    // separate handler contextMenuEvent
    if (e->button()!=Qt::LeftButton) {
        e->accept();
        return;
    }

    // The width of symbol "line locked" in teacher mode
    const uint lockSymbolWidth = editor_->plugin_->teacherMode_ &&
            editor_->analizerInstance_ ? LOCK_SYMBOL_WIDTH : 0;

    // Left border of editable area
    const uint editableAreaLeftBorder =
            charWidth() * LEFT_MARGIN_SIZE +
            lockSymbolWidth;

    // Right border of editable area
    const uint editableAreaRightBorder =
            editableAreaLeftBorder +
            widthInChars() * charWidth() -
            lockSymbolWidth;

    // Force text cursor (managed primarily from keyboard) to temporary hide
    editor_->cursor()->setViewMode(TextCursor::VM_Hidden);

    // Unset values for mouse press points
    // TODO implement it using boost::optional !
    delimeterRuleMousePressedPoint_ =
            marginMousePressedPoint_ =
            textMousePressedPoint_ = QPoint(-1000, -1000);


    // Perform an action depending on position where mouse clicked

    if (editor_->analizerInstance_ && editor_->plugin_->teacherMode_ &&
            e->pos().x() < lockSymbolWidth)
    {
        // Toggle 'locked' line status

        // Take into consideration possible scrolled coordinates
        const uint realY = qMax(0, e->pos().y() - offset().y());
        // Translate visible coordinates into text coordinates
        const uint textY = qMax(0u, realY/lineHeight());

        if (textY < editor_->document()->linesCount()) {
            // Toggle status only if line exists
            editor_->document()->undoStack()->push(
                        new ToggleLineProtectedCommand(editor_->document(), textY)
                        );
        }
    }
    else if (editor_->analizerInstance_ &&
             // Pointer position near margin line
             marginLineRect().contains(e->pos()))
    {
        // Begin drag margin line
        marginMousePressedPoint_ = e->pos();
    }
    else if ( editableAreaLeftBorder < e->pos().x() &&
              e->pos().x() < editableAreaRightBorder-2 )
    {
        // There are two actions possible:
        //   1) click on a text;
        //   2) drag a ruler

        // Flag to mark an action as move delimeter line between "visible"
        // and "hidden" part of editable text. False by default
        bool moveDelimeterRulerBetweenVisibleAndHiddenTextFlag = false;
        if (editor_->analizerInstance_ && editor_->plugin_->teacherMode_) {
            // The line number, where hidden text starts, or -1 if
            // there is no hidden text
            const int hiddenLineStart = editor_->document()->hiddenLineStart();

            // Rectangle region for delimeter "line"
            const QRect delimeterLineRect = hiddenLineStart==-1
                    ? QRect(editableAreaLeftBorder, // left border
                            height()-lineHeight(), // topBorder
                            widthInChars()*charWidth(), // width of area
                            lineHeight() // line thickness is line height
                          )
                    : QRect(editableAreaLeftBorder, // left border
                            lineHeight()*hiddenLineStart-2, // top border
                            widthInChars()*charWidth(), // width of area
                            HIDDEN_DELIMETER_LINE_HEIGHT // height of line (4px)
                          )
                    ;

            // If mouse coordinated in delimeterLineRect,
            // then move the line
            moveDelimeterRulerBetweenVisibleAndHiddenTextFlag =
                    delimeterLineRect.contains(e->pos());
        }
        if (moveDelimeterRulerBetweenVisibleAndHiddenTextFlag) {
            // Move line between visible and teacher text
            delimeterRuleMousePressedPoint_ = e->pos();
        }
        else {
            // Move cursor and (possible) begin selection
            textMousePressedPoint_ = e->pos();

            // Calculate text position
            const uint realX = qMax(0, e->pos().x() - offset().x());
            const uint realY = qMax(0, e->pos().y() - offset().y());
            const uint textX = realX / charWidth();
            const uint textY = realY / lineHeight();



            // Move text cursor into clicked position
            editor_->cursor()->moveTo(textY, textX);
            editor_->document()->checkForCompilationRequest(QPoint(editor_->cursor()->column(), editor_->cursor()->row()));

            // Store text clicked position for possible
            // selection handling
            textPressedPosition_ = QPoint(textX, textY);
        }
    }

    // Set repaint flag due to something may be changed and accept event
    update();
    e->accept();
}

/** Handles mouse button release event
 * @param e an event passed throught an event system
 */
void EditorPlane::mouseReleaseEvent(QMouseEvent *e)
{
#ifdef Q_OS_WIN32
    if (Qt::RightButton == e->button()) {
        e->ignore();
        return;
    }
#endif
    // Ensure auto scrolling by timer is stopped
    emit requestAutoScroll(0);
    emit requestAutoScrollX(0);

    if (marginMousePressedPoint_.x()!=-1000 &&
            marginMousePressedPoint_.y()!=-1000)
    {
        // In this case there was pairing "mousePress" action on this widget
        // inside a margin region

        // New margin position in pixels (normalized and bounded)
        uint x = normalizedNewMarginLinePosition(marginMousePressedPoint_.x());

        // New margin width in characters
        const uint marginCharWidth = (width() - x) / charWidth();

        // Store a settings value, this will be used on paint event
        editor_->mySettings()->setValue(MarginWidthKey, marginCharWidth);

        // Update scrollbars due to editable region size changed
        updateScrollBars();

        // Unset value
        // TODO use boost::optional here
        marginMousePressedPoint_ = QPoint(-1000, -1000);
    }

    if (delimeterRuleMousePressedPoint_.x()!=-1000 &&
            delimeterRuleMousePressedPoint_.y()!=-1000)
    {
        // In this case there was pairing "mousePress" action on this widget
        // inside a visible/hidden rule region

        const uint y = qMax(0, delimeterRuleMousePressedPoint_.y());
        int textY = y / lineHeight();
        if (y > uint(height()) - lineHeight())
            textY = -1;
        editor_->document()->undoStack()->push(new ChangeHiddenLineDelimeterCommand(
                                          editor_->document(),
                                          textY));
        update();
        delimeterRuleMousePressedPoint_ = QPoint(-1000, -1000);
    }

    if (selectionInProgressFlag_) {
        // Stop selection, so the next mouse press will initiate
        // a new selection progress
        selectionInProgressFlag_ = false;
    }
    else {
        // If not selection in progress, remove all selections
        editor_->cursor()->removeSelection();
        editor_->cursor()->removeRectSelection();
        updateScrollBars();
    }

    // Restore cursor blink behaviour, changed while mousePressEvent
    editor_->cursor()->setViewMode(TextCursor::VM_Blinking);

    // Set repaint flag due to something may be changed and accept event
    update();
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
        textMousePressedPoint_ = QPoint(-1000,-1000);
    }
    return false;
}

void EditorPlane::leaveEvent(QEvent *e)
{
    if (e->type() == QEvent::HoverLeave) {
        marginHintBox_->setVisible(false);
    }
    QWidget::leaveEvent(e);
}


/** Handles mouse move (while button pressed or not) enent.
 * The reason to handle the event while button is not pressed is
 * to set proper mouse pointer shape
 * @param e an event passed throught an event system
 */
void EditorPlane::mouseMoveEvent(QMouseEvent *e)
{
    // The width of symbol "line locked" in teacher mode
    const uint lockSymbolWidth = editor_->plugin_->teacherMode_ &&
            editor_->analizerInstance_ ? LOCK_SYMBOL_WIDTH : 0;

    // Left border of editable area
    const uint editableAreaLeftBorder =
            charWidth() * LEFT_MARGIN_SIZE +
            lockSymbolWidth;

    // Right border of editable area
    const uint editableAreaRightBorder =
            editableAreaLeftBorder +
            widthInChars() * charWidth() -
            lockSymbolWidth;

    // Line number of highlighted (by mouseover) 'lock' symbol or -1 if none
    highlightedLockSymbolLineNumber_ = -1;

    // The action is move delimeter line between "visible"
    // and "hidden" part of editable text. False by default
    bool moveDelimeterRulerBetweenVisibleAndHiddenTextFlag = false;

    if (editor_->analizerInstance_ && editor_->plugin_->teacherMode_) {
        // Programs in teacher mode have visible delimeter line,
        // so it is possible to move that line

        // The line number, where hidden text starts, or -1 if
        // there is no hidden text
        const int hiddenLineStart = editor_->document()->hiddenLineStart();

        // Rectangle region for delimeter "line" (of course, non-zero thickness)
        const QRect delimeterLineRect = hiddenLineStart==-1
                ? QRect(editableAreaLeftBorder, // left border
                        height()-lineHeight(), // topBorder
                        widthInChars()*charWidth(), // width of editable area
                        lineHeight() // line thickness is line height this case
                      )
                : QRect(editableAreaLeftBorder, // left border
                        lineHeight()*hiddenLineStart-2, // top border
                        widthInChars()*charWidth(), // width of editable area
                        HIDDEN_DELIMETER_LINE_HEIGHT // height of line (4px)
                      )
                ;

        // If mouse coordinated in delimeterLineRect,
        // then move the line
        moveDelimeterRulerBetweenVisibleAndHiddenTextFlag =
                delimeterLineRect.contains(e->pos());
    }

    if (editor_->plugin_->teacherMode_ && editor_->analizerInstance_ && e->pos().x()<lockSymbolWidth) {
        // Programs in teacher mode have visible 'lock' symbols to the left
        // of line numbers. Mouseover these symbols causes to highlight them

        // Take into consideration possible scrolled coordinates
        const uint realY = qMax(0, e->pos().y() - offset().y());

        // Translate visible coordinates into text coordinates
        const uint textY = qMax(0u, realY/lineHeight());

        if (textY<editor_->document()->linesCount()) {
            // Mark corresponding highlight 'lock' symbol only if
            // there is real text line exists
            highlightedLockSymbolLineNumber_ = textY;
        }

    }

    // Set proper mouse cursor shape depending on mouse pointer position

    if (editor_->plugin_->teacherMode_ && editor_->analizerInstance_ &&
            ( 0 <= e->pos().x() && e->pos().x() <= lockSymbolWidth ) &&
            highlightedLockSymbolLineNumber_ != -1
            )
    {
        // Pointer is over 'lock' symbol and real text line exists near it.
        // It is possible to toggle 'locked' status, so pointer shape should
        // be something like 'hand' of 'finger'

        setCursor(Qt::PointingHandCursor);
    }
    else if (e->pos().x()<editableAreaLeftBorder) {
        // Pointer is out of editable text, so there should be
        // regular 'arrow' shape instead of this widget default
        // 'text beam' shape

        setCursor(Qt::ArrowCursor);
    }
    else if (// Pointer position in visible/hidden ruler region
             moveDelimeterRulerBetweenVisibleAndHiddenTextFlag
             ||
             // Visible/hidden ruler movement in progress
             ( delimeterRuleMousePressedPoint_.x()!=-1000 &&
               delimeterRuleMousePressedPoint_.y()!=-1000 )
             )
    {
        // The mouse points to delimeter ruler between visible and hidden
        // text, so there should be a pointer shape as for vertical resize

        setCursor(Qt::SplitVCursor);
    }
    else if (editor_->analizerInstance_ &&
             (
                 // Pointer position near margin line
                 marginLineRect().contains(e->pos())
                 ||
                 // Margin line movement in progress
                 ( marginMousePressedPoint_.x()!=-1000 &&
                   marginMousePressedPoint_.y()!=-1000 )
                 )
             )
    {
        // The mouse pointer is about red line delimeter between editable area
        // and the margin (+- 2 pixels), so there should be a pointer shape as
        // for horizontal resize

        setCursor(Qt::SplitHCursor);
    }
    else {
        // Just a simple regular case: mouse pointer is over editable area
        // or text margin, so restore cursor shape to its regular "text beam"

        if (e->pos().x() < editableAreaRightBorder )
            setCursor(Qt::IBeamCursor);
        else
            unsetCursor();
    }


    // Begin processing of move event.
    // There is three actions possible to perform:
    //  1) move margin line;
    //  2) move visible/hidden ruler;
    //  3) perform text selection or start drag'n'drop

    // There are special values (-1000, -1000) to indicate "has value" flag
    // for storing positions of mouse press to margin or delimeter rule.
    // Update them if requred

    // TODO switch to using of boost::optional instead to reduce flags usage!

    if (marginMousePressedPoint_.x()!=-1000 &&
            marginMousePressedPoint_.y()!=-1000) {
        marginMousePressedPoint_ = e->pos();
        update();
    }
    else if (delimeterRuleMousePressedPoint_.x()!=-1000 &&
             delimeterRuleMousePressedPoint_.y()!=-1000) {
        delimeterRuleMousePressedPoint_ = e->pos();
        update();
    }

    // The last important case is selecting text by mouse.
    // This case is matched by already set 'selectionInProgressFlag_'
    // or if left mouse button pressed while pointer is in editable area
    else if (selectionInProgressFlag_ || (
                 e->pos().x()>editableAreaLeftBorder &&
                 e->pos().x()<editableAreaRightBorder-2 &&
                 e->buttons().testFlag(Qt::LeftButton)
                 )
             )
    {
        // Start autoscrolling while selection if mouse pointer
        // is out of the widget area

        // Check for vertival scrolling
        if (e->pos().y()<0) {
            emit requestAutoScroll(-1);   // scroll up by timer ticks
        }
        else if (e->pos().y()>height()) {
            emit requestAutoScroll(+1);   // scroll down by timer ticks
        }
        else {
            emit requestAutoScroll( 0);   // stop vertical autoscrolling
        }

        // Check for horizontal scrolling
        if (e->pos().x()<0) {
            emit requestAutoScrollX(-1);  // scroll left by timer ticks
        }
        else if (e->pos().x()>editableAreaRightBorder-2) {
            emit requestAutoScrollX(+1);  // scroll right by timer ticks
        }
        else {
            emit requestAutoScrollX( 0);  // stop horizontal autoscrolling
        }

        // Calculate distance for this mouse movement to protect user from
        // unawaited text selection
        int dX = e->pos().x() - textMousePressedPoint_.x();
        int dY = e->pos().y() - textMousePressedPoint_.y();
        qreal distance = sqrt(double(dX*dX+dY*dY));
        qreal thereshold = QApplication::startDragDistance();
        bool reallyMovingMouse = distance >= thereshold;

        // Check if selection really performing
        bool mousePressedInTextOrSelectionInProgress =
                textMousePressedPoint_.x()!=-1000 || selectionInProgressFlag_;

        if (reallyMovingMouse && mousePressedInTextOrSelectionInProgress) {
            // Ok, let's perform a text selection

            // Is something selected?
            bool cursorHasTextSelection = editor_->cursor()->hasSelection();
            bool cursorHasRectSelection =
                    editor_->analizerInstance_==0 && editor_->cursor()->hasRectSelection();

            bool nothingSelected =
                    !cursorHasTextSelection && !cursorHasRectSelection;

            if (selectionInProgressFlag_ || nothingSelected ) {
                // Perform a text selection

                // Set selection in progress flag in case if nothing was
                // selected before
                selectionInProgressFlag_ = true;

                // Translate coordinates considering scroll values
                const uint realX = qMax(0, e->pos().x() - offset().x());
                const uint realY = qMax(0, e->pos().y() - offset().y());

                // Translate pointer pixel-coordinates into text coordinates
                const uint textX = realX / charWidth();
                const uint textY = realY / lineHeight();

                if (editor_->analizerInstance_==0 &&
                        e->modifiers().testFlag(Qt::ShiftModifier))
                {
                    // Rectangle selection (Using Shift+mouse button),
                    // which available only for plain text editors

                    // Set cursor shape like spreadsheet cross
                    QApplication::restoreOverrideCursor();
                    QApplication::setOverrideCursor(Qt::CrossCursor);

                    // Perform rectangle selection
                    editor_->cursor()->selectRangeBlock(
                                textPressedPosition_.y(),
                                textPressedPosition_.x(),
                                textY,
                                textX);
                }
                else {
                    // Regular text selection

                    // Ensure using regular mouse pointer shape
                    QApplication::restoreOverrideCursor();

                    // Perform regular text selection
                    editor_->cursor()->selectRangeText(
                                textPressedPosition_.y(),
                                textPressedPosition_.x(),
                                textY,
                                textX);
                }
            } // end if (selectionInProgressFlag_ || nothingSelected )

            else if (cursorHasRectSelection || cursorHasTextSelection) {
                // Have a finished selection. Perform drag'n'drop

                // Prepare drag data
                QMimeData * data = new QMimeData;

                if (cursorHasTextSelection) {
                    // If regular selection mode, store selected text
                    data->setText(editor_->cursor()->selectedText());
                }

                if (editor_->analizerInstance_==0 && cursorHasRectSelection) {
                    // In case of rectangle selection, store rectangle
                    // block data ...
                    data->setData(
                                Clipboard::BlockMimeType,
                                editor_->cursor()->rectSelectionText().
                                join("\n").toUtf8());

                    // ... and plain text data to use in non-Kumir
                    // drop source
                    data->setText(editor_->cursor()->selectedText());
                }

                // Perform a drag action
                QDrag * drag = new QDrag(this);
                drag->setMimeData(data);
                Qt::DropAction result = drag->exec(
                            Qt::CopyAction|Qt::MoveAction,
                            Qt::CopyAction
                            );

                // If drag'n'drop performed as 'Move' but not 'Copy',
                // then delete selected text
                if (result==Qt::MoveAction && drag->target()!=this) {
                    if (cursorHasTextSelection) {
                        editor_->cursor()->removeSelectedText();
                    }
                    else if (cursorHasRectSelection) {
                        editor_->cursor()->removeSelectedBlock();
                    }
                }
            } // end else if (cursorHasRectSelection || cursorHasTextSelection)
        } // end if (reallyMovingMouse && mousePressedInText...)
    }

    // Check if there is must be shown tooltlip for a margin line
    bool visibleMarginBox = false;
    if (e->pos().x() >= editableAreaRightBorder
            && e->button() == Qt::NoButton)
    {
            // Translate coordinates considering scroll values
            const uint realY = qMax(0, e->pos().y() - offset().y());

            // Translate pointer pixel-coordinates into text coordinates
            const uint textY = realY / lineHeight();

            static const QString empty;
            const TextLine::Margin & margin = editor_->document()->marginAt(textY);
            const QString & text = margin.text.length() > 0
                    ? margin.text : margin.errors.size() > 0
                      ? margin.errors.first() : empty;

            if (text.length() > 0) {
                QFontMetrics fm(font());
                const uint w = fm.width(text);
                const QRect marginRect = marginBackgroundRect();
                if (w > marginRect.width()) {
                    visibleMarginBox = true;
                    marginHintBox_->setText(text);
                    const uint y = ( e->pos().y() / lineHeight() + 1)* lineHeight();
                    const uint x = width() - marginHintBox_->width();
                    marginHintBox_->move(mapToGlobal(QPoint(x, y)));
                }
            }
    }
    marginHintBox_->setVisible(visibleMarginBox);


    // Set repaint flag due to something may be changed and accept event
    update();
    e->accept();
}


void EditorPlane::initMouseCursor()
{
    QCursor c(Qt::IBeamCursor);
    setCursor(c);
}

int EditorPlane::marginCharactersCount() const
{
    return editor_->mySettings()->value(MarginWidthKey, MarginWidthDefault).toInt();
}

QPoint EditorPlane::offset() const
{
    QPoint lineNumbersOffset(charWidth() * LEFT_MARGIN_SIZE , 0);
    QPoint lockSymbolOffset (editor_->plugin_->teacherMode_ ? LOCK_SYMBOL_WIDTH : 0, 0);
    QPoint scrollOffset(0, 0);
    if (editor_->scrollBar(Qt::Horizontal)->isEnabled()) {
        int valX = editor_->scrollBar(Qt::Horizontal)->value();
        valX = ( valX / charWidth() ) * charWidth();
        scrollOffset.setX(-valX);
    }
    if (editor_->scrollBar(Qt::Vertical)->isEnabled()) {
        int valY = editor_->scrollBar(Qt::Vertical)->value();
        valY = ( valY / lineHeight() ) * lineHeight();
        scrollOffset.setY(-valY);
    }
    QPoint totalOffset = lineNumbersOffset + scrollOffset + lockSymbolOffset;
    return totalOffset;
}

void EditorPlane::updateScrollBars()
{
    QPoint prevOffset = offset();
    uint w = 1;
    uint h = 1;
    for (int i=0 ; i<editor_->document()->linesCount(); i++) {
        uint indent = editor_->document()->indentAt(i) * 2;
        uint textLength = editor_->document()->textAt(i).length();
        w = qMax(w, indent + textLength + 1);
    }
    w = qMax(w, editor_->cursor()->column()+1);
    h = qMax(editor_->document()->linesCount()+1, editor_->cursor()->row()+2);

    QSize contentSize (w*charWidth(), h*lineHeight());
    QSize viewportSize (widthInChars() * charWidth(), height());
    if (contentSize.width()<=viewportSize.width()) {
        editor_->scrollBar(Qt::Horizontal)->setEnabled(false);
        editor_->scrollBar(Qt::Horizontal)->setVisible(false);
    }
    else {
        editor_->scrollBar(Qt::Horizontal)->setEnabled(true);
        editor_->scrollBar(Qt::Horizontal)->setVisible(true);
        editor_->scrollBar(Qt::Horizontal)->setRange(0, contentSize.width()-viewportSize.width());
        editor_->scrollBar(Qt::Horizontal)->setSingleStep(charWidth());
        editor_->scrollBar(Qt::Horizontal)->setPageStep(charWidth() * 8);
    }
    if (contentSize.height()<=viewportSize.height()) {
        editor_->scrollBar(Qt::Vertical)->setEnabled(false);
        editor_->scrollBar(Qt::Vertical)->setVisible(false);
    }
    else {
        editor_->scrollBar(Qt::Vertical)->setEnabled(true);
        editor_->scrollBar(Qt::Vertical)->setVisible(true);
        editor_->scrollBar(Qt::Vertical)->setRange(0, contentSize.height()-viewportSize.height());
        editor_->scrollBar(Qt::Vertical)->setSingleStep(lineHeight());
        editor_->scrollBar(Qt::Vertical)->setPageStep(lineHeight() * 8);
    }
    editor_->scrollBar(Qt::Horizontal)->setFixedWidth(marginLeftBound() + MARGIN_LINE_WIDTH);
    if (prevOffset!=offset())
        update();
}

void EditorPlane::ensureCursorVisible()
{    
    const int lineNoWidth = 5;
    uint row = editor_->cursor()->row();
    uint column = editor_->cursor()->column();
    QRect cr(5 + column,
             row,
             2,
             2
                );
    if (column == 2u * editor_->document()->indentAt(row)) {
        cr.setLeft(0);
        cr.setRight(2);
    }
    QRect vr;
    vr.setLeft(editor_->scrollBar(Qt::Horizontal)->isEnabled()? editor_->scrollBar(Qt::Horizontal)->value()/charWidth() : 0);
    vr.setTop(editor_->scrollBar(Qt::Vertical)->isEnabled()? editor_->scrollBar(Qt::Vertical)->value()/lineHeight() : 0);
    vr.setSize(QSize(widthInChars(), height()/lineHeight()));
    vr.translate(QPoint(lineNoWidth, 0));
//    qDebug() << "CR: " << cr;
//    qDebug() << "VR: " << vr;
    if (cr.left()>vr.right()) {
//        qDebug() << "A";
        int v = editor_->cursor()->column() - vr.width()-1 + lineNoWidth;
        editor_->scrollBar(Qt::Horizontal)->setValue(v * charWidth());
    }
    else if (cr.left()<vr.left()) {
//        qDebug() << "B";
        int v = editor_->cursor()->column();
        if (editor_->cursor()->column() == 2u * editor_->document()->indentAt(editor_->cursor()->row()))
            v = 0;
        editor_->scrollBar(Qt::Horizontal)->setValue(v * charWidth());
    }
    if (cr.top()>vr.bottom()) {
//        qDebug() << "C";
        int v = editor_->cursor()->row()-vr.height()+1;
//        qDebug() << "v0: " << m_verticalScrollBar->value();
        editor_->scrollBar(Qt::Vertical)->setValue(v*lineHeight());
//        qDebug() << "v1: " << m_verticalScrollBar->value();
    }
    else if (cr.top()<vr.top()) {
//        qDebug() << "D";
        int v = cr.top();
//        qDebug() << "v0: " << m_verticalScrollBar->value();
        editor_->scrollBar(Qt::Vertical)->setValue(v*lineHeight());
//        qDebug() << "v1: " << m_verticalScrollBar->value();
    }
}

void EditorPlane::ensureHighlightedLineVisible()
{
    QRect cr(0,
             highlightedTextLineNumber_,
             2,
             2
                );
    QRect vr;
    vr.setLeft(editor_->scrollBar(Qt::Horizontal)->isEnabled()? editor_->scrollBar(Qt::Horizontal)->value()/charWidth() : 0);
    vr.setTop(editor_->scrollBar(Qt::Vertical)->isEnabled()? editor_->scrollBar(Qt::Vertical)->value()/lineHeight() : 0);
    vr.setSize(QSize(widthInChars(), height()/lineHeight()));

    if (cr.top()>vr.bottom()) {
        int v = highlightedTextLineNumber_;
        editor_->scrollBar(Qt::Vertical)->setValue(v*lineHeight());
    }
    else if (cr.bottom()<vr.top()) {
        int v = highlightedTextLineNumber_;
        editor_->scrollBar(Qt::Vertical)->setValue(v*lineHeight());
    }
}


void EditorPlane::findCursor()
{
    updateScrollBars();
    ensureCursorVisible();
}

void EditorPlane::paintEvent(QPaintEvent *e)
{
    // Create a painter
    QPainter p(this);

//    // Paint a plain background
    paintBackground(&p, e->rect());

    // Save state before translating scroll offsets
    p.save();

    // Translate coordinate system to match scroll values
    p.translate( offset() );

    // Paint selections before text
    paintSelection(&p, e->rect().translated(-offset()));
    paintRectSelection(&p, e->rect().translated(-offset()));

    QBrush highlightLineBrush;
    if (highlightedTextLineNumber_!=-1)
    {
        const QColor bgColor = palette().color(QPalette::Base);
        int darkness = bgColor.red() + bgColor.green() + bgColor.blue();
        if (darkness / 3 <= 127) {
            highlightLineBrush = QBrush(highlightedTextLineColor_);
        }
        else {
            QLinearGradient gr(QPointF(0,0),QPointF(0,1));
            gr.setCoordinateMode(QGradient::ObjectBoundingMode);
            QColor c1 = highlightedTextLineColor_.lighter();
            c1.setAlpha(32);
            QColor c2 = highlightedTextLineColor_.lighter();
            gr.setColorAt(0, c1);
            gr.setColorAt(1, c2);
            highlightLineBrush = QBrush(gr);
        }
    }

    // Paint line highlight if need before text itself
    if (highlightedTextLineNumber_!=-1)
    {
        const QRect highlightRect(
                    0,
                    lineHeight() * highlightedTextLineNumber_+1,
                    widthInChars() * charWidth(),
                    lineHeight() + HIGHTLIGHT_LINE_VERTICAL_PADDING
                    );        

        // Draw a rect
        p.setBrush(highlightLineBrush);
        p.setPen(Qt::NoPen);
        p.drawRect(highlightRect);

        // Draw rect borders
        p.setPen(highlightedTextLineColor_);        
        p.drawLine(highlightRect.topLeft(), highlightRect.topRight());
        p.drawLine(highlightRect.bottomLeft(), highlightRect.bottomRight());

        // Restore a pen
        p.setPen(Qt::NoPen);
    }

    // Paint a text
    paintText(&p, e->rect().translated(-offset()));

    // Paint a cursor
    paintCursor(&p, e->rect().translated(-offset()));

    // Restore coordinate system due to line numbers must be always
    // visible without reference to scroll values
    p.restore();
    paintLineNumbers(&p, e->rect());

    if (editor_->analizerInstance_) {
        // Fill a margin with empty backround and draw margin line
        paintMarginBackground(&p, e->rect());

        // Draw a new margin rect in case of moving margin line by mouse
        paintNewMarginLine(&p);
    }

    // If there is highlighted line, draw it's parts at left anf right margin
    if (highlightedTextLineNumber_!=-1)
    {
        // Save & translate coordinate system again
        p.save();
        p.translate(offset());

        const QRect highlightLeftRect(
                    -offset().x(),
                    lineHeight() * highlightedTextLineNumber_ + 1,
                    LEFT_MARGIN_SIZE * charWidth(),
                    lineHeight() + HIGHTLIGHT_LINE_VERTICAL_PADDING
                    );

        const QRect highlightRightRect(
                    charWidth() * widthInChars(),
                    lineHeight() * highlightedTextLineNumber_ + 1,
                    marginCharactersCount() * widthInChars(),
                    lineHeight() + HIGHTLIGHT_LINE_VERTICAL_PADDING
                    );

        // Draw a rect
        p.setBrush(highlightLineBrush);
        p.setPen(Qt::NoPen);
        p.drawRect(highlightLeftRect);
        p.drawRect(highlightRightRect);

        // Draw borders
        p.setPen(highlightedTextLineColor_);

        p.drawLine(highlightLeftRect.topLeft(),
                   highlightLeftRect.topRight());
        p.drawLine(highlightLeftRect.bottomLeft(),
                   highlightLeftRect.bottomRight());
        p.drawLine(highlightRightRect.topLeft(),
                   highlightRightRect.topRight());
        p.drawLine(highlightRightRect.bottomLeft(),
                   highlightRightRect.bottomRight());

        if (highlightedTextColumnStartNumber_ != highlightedTextColumnEndNumber_) {
            // Draw a rect around statement
            QPen pen;
            pen.setColor(highlightedTextLineColor_);
            const QColor bgColor = palette().color(QPalette::Base);
            int darkness = bgColor.red() + bgColor.green() + bgColor.blue();
            if (darkness / 3 <= 127) {
                pen.setColor(highlightedTextLineColor_.darker());
            }
            pen.setStyle(Qt::SolidLine);
            pen.setWidth(2);
            p.setPen(pen);
            p.setBrush(Qt::NoBrush);
            uint cw = charWidth();
            uint left = cw * highlightedTextColumnStartNumber_;
            uint right = cw * highlightedTextColumnEndNumber_;
            left += cw * 2 * editor_->document()->indentAt(highlightedTextLineNumber_);
            right += cw * 2 * editor_->document()->indentAt(highlightedTextLineNumber_);
            p.drawRoundedRect(left, highlightLeftRect.top(),
                              int(right) - int(left), highlightLeftRect.height(),
                              2, 2);
        }

        // Restore a pen
        p.setPen(Qt::NoPen);

        // Restore a coordinate system
        p.restore();
    }

    if (editor_->analizerInstance_) {
        // Paint margin text
        // This function translates coordinate system itself due to
        // margin text always visible and so has no reference on X-scroll value
        paintMarginText(&p, e->rect());
    }

    // What is it???? TODO comment me!
    paintDropPosition(&p);

    // Draw a widget border to show its focus state
    p.setBrush(Qt::NoBrush);
    const QBrush br = hasFocus()
            ? palette().brush(QPalette::Highlight)
            : palette().brush(QPalette::Mid);
    p.setPen(QPen(br, 3));
    p.drawLine(0, 0, width()-1, 0);
    p.drawLine(0, height()-1, width()-1, height()-1);
    p.drawLine(0, 0, 0, height()-1);
    p.drawLine(width()-1, 0, width()-1, height());


    // Draw a delimeter ruler between visible/hidden text if need
    if (editor_->plugin_->teacherMode_ && editor_->analizerInstance_) {

        // Draw a place where ruler stays if no hidden text
        p.setPen(Qt::NoPen);
        QColor hidColor("#797979");
        hidColor.setAlpha(255);
        p.setBrush(hidColor);
        p.drawRect(0, height()-lineHeight(), width(), lineHeight());

        // Draw a ruler
        paintHiddenTextDelimeterLine(&p);

        // Draw a new ruler rect in case of moving ruler by mouse
        paintNewHiddenDelimeterLine(&p);
    }    

    // Accept an event
    e->accept();
}

void EditorPlane::paintHiddenTextDelimeterLine(QPainter *p)
{
    if (editor_->plugin_->teacherMode_ && editor_->analizerInstance_) {
        int hiddenLineStart = editor_->document()->hiddenLineStart();

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

uint EditorPlane::normalizedNewMarginLinePosition(uint x) const
{
    // Bound x value
    uint minimumLeftPosition = (LEFT_MARGIN_SIZE + 1) * charWidth() +
            (editor_->plugin_->teacherMode_ && editor_->analizerInstance_
             ? LOCK_SYMBOL_WIDTH : 0 );

    uint maximumLeftPosition = width() - charWidth() - MARGIN_LINE_WIDTH;
    x = qMax(minimumLeftPosition, qMin(maximumLeftPosition, x));

    // Make x-coordinate aligned to character matrix
    x = ( x / charWidth() ) * charWidth();
//    qDebug() << "X = " << x;
    return x;
}

void EditorPlane::paintNewMarginLine(QPainter *p)
{
    if (marginMousePressedPoint_.x() >= 0 &&
            marginMousePressedPoint_.y() >= 0)
    {
        // Draw only in case of moving margin line

        uint x = normalizedNewMarginLinePosition(marginMousePressedPoint_.x());

        const QRect newMarginLineRect(
                    x,
                    0,
                    MARGIN_LINE_WIDTH,
                    height()
                    );

        // Draw a transparent bordered rect
        p->setPen(QColor(Qt::black));
        p->setBrush(Qt::NoBrush);
        p->drawRect(newMarginLineRect);
    }
}

void EditorPlane::paintNewHiddenDelimeterLine(QPainter *p)
{
    p->save();
    p->setPen(QColor(Qt::black));
    p->setBrush(Qt::NoBrush);
    int y = delimeterRuleMousePressedPoint_.y();
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

uint EditorPlane::charWidth() const
{
    const QFontMetrics fm(font());
    return qMax(0, fm.width('M'));
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

uint EditorPlane::lineHeight() const
{
    const QFontMetrics fm(font());
    return qMax(0, fm.lineSpacing());
}



void EditorPlane::paintCursor(QPainter *p, const QRect &rect)
{
    // Get cursor rect (in absolute coordinated)
    const QRect cr = cursorRect();

    if (rect.intersects(cr) // prevent painting of non-updated region
            &&
            editor_->cursor()->isVisible() // determined by blink timer
            )
    {
        p->setPen(Qt::NoPen);
        p->setBrush(QColor(Qt::black));
        if (editor_->mySettings()->value(SettingsPage::KeyInvertColorsIfDarkSystemTheme,
                            SettingsPage::DefaultInvertColorsIfDarkSystemTheme)
                .toBool()
                )
        {
            // Check for dark background
            const QColor bgColor = palette().color(QPalette::Base);
            int darkness = bgColor.red() + bgColor.green() + bgColor.blue();
            if (darkness / 3 <= 127) {
                // Invert color for dark backround
                p->setBrush(Qt::white);
            }
        }
        p->drawRect(cr);
    }
}



void EditorPlane::keyReleaseEvent(QKeyEvent *e)
{
    Qt::Key tempSwichLayoutKey = Qt::Key(
                editor_->mySettings()->value(
                    SettingsPage::KeyTempSwitchLayoutButton
                    , SettingsPage::DefaultTempSwitchLayoutButton)
                .toUInt()
                );
    if (e->key()==tempSwichLayoutKey) {
        Utils::altKeyPressed = false;
    }
    if (e->key()==Qt::Key_Shift || (e->key()==-1 && e->modifiers() & Qt::ShiftModifier)) {
        Utils::shiftKeyPressed = false;
    }
    if (editor_->cursor()->isEnabled()) {
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
    else if (e->type() == QEvent::HoverLeave) {
        marginHintBox_->setVisible(false);
    }
    else if (e->type()==QEvent::FocusOut) {
        Utils::altKeyPressed = false;
        Utils::shiftKeyPressed = false;
        marginHintBox_->setVisible(false);
    }
    return QWidget::event(e);
}

void EditorPlane::keyPressEvent(QKeyEvent *e)
{
    bool MoveToNextChar = e->matches(QKeySequence::MoveToNextChar);
    bool SelectNextChar = e->matches(QKeySequence::SelectNextChar);
    bool MoveToNextWord = e->matches(QKeySequence::MoveToNextWord);
    bool SelectNextWord = e->matches(QKeySequence::SelectNextWord);
    bool MoveToPreviousChar = e->matches(QKeySequence::MoveToPreviousChar);
    bool SelectPreviousChar = e->matches(QKeySequence::SelectPreviousChar);
    bool MoveToPreviousWord = e->matches(QKeySequence::MoveToPreviousWord);
    bool SelectPreviousWord = e->matches(QKeySequence::SelectPreviousWord);
    bool MoveToNextLine = e->matches(QKeySequence::MoveToNextLine);
    bool SelectNextLine = e->matches(QKeySequence::SelectNextLine);
    bool MoveToPreviousLine = e->matches(QKeySequence::MoveToPreviousLine);
    bool SelectPreviousLine = e->matches(QKeySequence::SelectPreviousLine);

    bool ignoreTextEvent = e->key()==Qt::Key_Escape ||
            e->key()==Qt::Key_Backspace ||
            ( (e->key()==Qt::Key_Enter||e->key()==Qt::Key_Return) && e->modifiers()!=0 );
#ifdef Q_WS_X11
    // VIM-style navigation using Super key :
    //   {h,j,k,l} -> {left, down, up, right }
    if (e->modifiers().testFlag(Qt::MetaModifier)) {
        MoveToNextChar = e->key()==Qt::Key_L
                && !e->modifiers().testFlag(Qt::ShiftModifier)
                && !e->modifiers().testFlag(Qt::ControlModifier);
        SelectNextChar = e->key()==Qt::Key_L
                && e->modifiers().testFlag(Qt::ShiftModifier)
                && !e->modifiers().testFlag(Qt::ControlModifier);
        MoveToNextWord = e->key()==Qt::Key_L
                && !e->modifiers().testFlag(Qt::ShiftModifier)
                && e->modifiers().testFlag(Qt::ControlModifier);
        SelectNextWord = e->key()==Qt::Key_L
                && e->modifiers().testFlag(Qt::ShiftModifier)
                && e->modifiers().testFlag(Qt::ControlModifier);

        MoveToPreviousChar = e->key()==Qt::Key_H
                && !e->modifiers().testFlag(Qt::ShiftModifier)
                && !e->modifiers().testFlag(Qt::ControlModifier);
        SelectPreviousChar = e->key()==Qt::Key_H
                && e->modifiers().testFlag(Qt::ShiftModifier)
                && !e->modifiers().testFlag(Qt::ControlModifier);
        MoveToPreviousWord = e->key()==Qt::Key_H
                && !e->modifiers().testFlag(Qt::ShiftModifier)
                && e->modifiers().testFlag(Qt::ControlModifier);
        SelectPreviousWord = e->key()==Qt::Key_H
                && e->modifiers().testFlag(Qt::ShiftModifier)
                && e->modifiers().testFlag(Qt::ControlModifier);

        MoveToNextLine = e->key()==Qt::Key_J
                && !e->modifiers().testFlag(Qt::ShiftModifier)
                && !e->modifiers().testFlag(Qt::ControlModifier);
        SelectNextLine = e->key()==Qt::Key_J
                && e->modifiers().testFlag(Qt::ShiftModifier)
                && !e->modifiers().testFlag(Qt::ControlModifier);

        MoveToPreviousLine = e->key()==Qt::Key_K
                && !e->modifiers().testFlag(Qt::ShiftModifier)
                && !e->modifiers().testFlag(Qt::ControlModifier);
        SelectPreviousLine = e->key()==Qt::Key_K
                && e->modifiers().testFlag(Qt::ShiftModifier)
                && !e->modifiers().testFlag(Qt::ControlModifier);
    }

#endif
    if (editor_->cursor()->isEnabled() && hasFocus()) {
        emit message(QString());
        if (MoveToNextChar) {
            editor_->cursor()->evaluateCommand(KeyCommand::MoveToNextChar);
        }
        else if (SelectNextChar) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectNextChar);
        }
        else if (MoveToNextWord) {
            editor_->cursor()->evaluateCommand(KeyCommand::MoveToNextLexem);
        }
        else if (SelectNextWord) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectNextLexem);
        }
        else if (e->key()==Qt::Key_Right && e->modifiers().testFlag(RECT_SELECTION_MODIFIER) && editor_->analizerInstance_==0) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectNextColumn);
        }
        else if (MoveToPreviousChar) {
            editor_->cursor()->evaluateCommand(KeyCommand::MoveToPreviousChar);
        }
        else if (SelectPreviousChar) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectPreviousChar);
        }
        else if (MoveToPreviousWord) {
            editor_->cursor()->evaluateCommand(KeyCommand::MoveToPreviousLexem);
        }
        else if (SelectPreviousWord) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectPreviousLexem);
        }
        else if (e->key()==Qt::Key_Left && e->modifiers().testFlag(RECT_SELECTION_MODIFIER) && editor_->analizerInstance_==0) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectPreviousColumn);
        }
        else if (MoveToNextLine) {
            editor_->cursor()->evaluateCommand(KeyCommand::MoveToNextLine);
        }
        else if (SelectNextLine) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectNextLine);
        }
        else if (e->key()==Qt::Key_Down && e->modifiers().testFlag(RECT_SELECTION_MODIFIER) && editor_->analizerInstance_==0) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectNextRow);
        }
        else if (MoveToPreviousLine) {
            editor_->cursor()->evaluateCommand(KeyCommand::MoveToPreviousLine);
        }
        else if (SelectPreviousLine) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectPreviousLine);
        }
        else if (e->key()==Qt::Key_Up && e->modifiers().testFlag(RECT_SELECTION_MODIFIER) && editor_->analizerInstance_==0) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectPreviousRow);
        }
        else if (e->matches(QKeySequence::MoveToStartOfLine)) {
            editor_->cursor()->evaluateCommand(KeyCommand::MoveToStartOfLine);
        }
        else if (e->matches(QKeySequence::SelectStartOfLine)) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectStartOfLine);
        }
        else if (e->matches(QKeySequence::MoveToEndOfLine)) {
            editor_->cursor()->evaluateCommand(KeyCommand::MoveToEndOfLine);
        }
        else if (e->matches(QKeySequence::SelectEndOfLine)) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectEndOfLine);
        }
        else if (e->matches(QKeySequence::MoveToPreviousPage)) {
            editor_->cursor()->evaluateCommand(KeyCommand::MoveToPreviousPage);
        }
        else if (e->matches(QKeySequence::SelectPreviousPage)) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectPreviousPage);
        }
        else if (e->matches(QKeySequence::MoveToNextPage)) {
            editor_->cursor()->evaluateCommand(KeyCommand::MoveToNextPage);
        }
        else if (e->matches(QKeySequence::SelectNextPage)) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectNextPage);
        }
        else if (e->matches(QKeySequence::MoveToStartOfDocument)) {
            editor_->cursor()->evaluateCommand(KeyCommand::MoveToStartOfDocument);
        }
        else if (e->matches(QKeySequence::SelectStartOfDocument)) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectStartOfDocument);
        }
        else if (e->matches(QKeySequence::MoveToEndOfDocument)) {
            editor_->cursor()->evaluateCommand(KeyCommand::MoveToEndOfDocument);
        }
        else if (e->matches(QKeySequence::SelectEndOfDocument)) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectEndOfDocument);
        }
        else if (e->matches(QKeySequence::InsertParagraphSeparator)) {
            const bool protecteed = editor_->cursor()->modifiesProtectedLiines();
            if (!protecteed) {
                bool addIndent = editor_->analizerPlugin_ && editor_->analizerPlugin_->indentsSignificant();
                if (!addIndent) {
                    editor_->cursor()->evaluateCommand("\n");
                }
                else {
                    const QString & curText = editor_->cursor()->row() < editor_->document()->linesCount()
                            ? editor_->document()->at(editor_->cursor()->row()).text : QString();

                    int indentSpaces = 0;
                    for (int i=0; i<curText.length(); i++) {
                        if (curText.at(i) == ' ') {
                            indentSpaces += 1;
                        }
                        else {
                            break;
                        }
                    }
                    bool moveToEnd = false;
                    for (uint i=editor_->cursor()->column(); i<curText.length(); i++) {
                        if (curText.at(i) == ' ') {
                            moveToEnd = true;
                        }
                        else {
                            moveToEnd = false;
                            break;
                        }
                    }
                    if (moveToEnd)
                        editor_->cursor()->moveTo(editor_->cursor()->row(), curText.length());
                    QString indent;
                    indent.fill(' ', indentSpaces);
                    editor_->cursor()->evaluateCommand("\n" + indent);
                }
            }
        }
        else if (e->key()==Qt::Key_Backspace && e->modifiers()==0) {
            bool checkForIndent = !editor_->cursor()->hasSelection() &&
                    editor_->analizerPlugin_ && editor_->analizerPlugin_->indentsSignificant();
            if (!checkForIndent) {
                editor_->cursor()->evaluateCommand(KeyCommand::Backspace);
            }
            else {
                const QString & curText = editor_->cursor()->row() < editor_->document()->linesCount()
                        ? editor_->document()->at(editor_->cursor()->row()).text : QString();
                bool onlySpacesBefore = curText.left(editor_->cursor()->column()).trimmed().isEmpty();
                uint bsCount = 1u;
                if (onlySpacesBefore && editor_->cursor()->column() > 0) {
                    bsCount = qMin(4u, editor_->cursor()->column());
                }
                for (uint i=0u; i<bsCount; i++) {
                    editor_->cursor()->evaluateCommand(KeyCommand::Backspace);
                }
            }
        }
        else if (e->matches(QKeySequence::Paste)) {
            editor_->cursor()->evaluateCommand(KeyCommand::Paste);
        }
        else if (e->matches(QKeySequence::Copy)) {
            editor_->cursor()->evaluateCommand(KeyCommand::Copy);
        }
        else if (e->matches(QKeySequence::Cut)) {
            editor_->cursor()->evaluateCommand(KeyCommand::Cut);
        }
        else if (e->matches(QKeySequence::SelectAll)) {
            editor_->cursor()->evaluateCommand(KeyCommand::SelectAll);
        }
        else if (e->key()==Qt::Key_Y && e->modifiers().testFlag(Qt::ControlModifier)) {
            editor_->cursor()->evaluateCommand(KeyCommand::RemoveLine);
        }
        else if (e->key()==Qt::Key_K && e->modifiers().testFlag(Qt::ControlModifier)) {
            editor_->cursor()->evaluateCommand(KeyCommand::RemoveTail);
        }
        else if (e->matches(QKeySequence::Delete)) {
            editor_->cursor()->evaluateCommand(KeyCommand::Delete);
        }
        else if (e->matches(QKeySequence::Undo)) {
            editor_->cursor()->undo();
        }
        else if (e->matches(QKeySequence::Redo)) {
            editor_->cursor()->redo();
        }
        else if (e->key()==Qt::Key_Slash && e->modifiers().testFlag(Qt::ControlModifier)) {
            editor_->cursor()->toggleComment();
        }
        else if (e->key()==Qt::Key_M && e->modifiers().testFlag(Qt::ControlModifier)) {
            editor_->recordMacro_->trigger();
        }
        else if (e->key()==Qt::Key_Space && e->modifiers().testFlag(Qt::ControlModifier)) {
            if (editor_->analizerInstance_)
                doAutocomplete();
        }
        else if (e->key()==Qt::Key_Tab) {
            if (editor_->analizerPlugin_ && editor_->analizerPlugin_->indentsSignificant()) {
                editor_->cursor()->evaluateCommand("    ");
            }
            else if (editor_->analizerInstance_ && editor_->analizerInstance_->helper()) {
                doAutocomplete();
            }
        }
        else if (!e->text().isEmpty() &&
                 !e->modifiers().testFlag(Qt::ControlModifier) &&
                 !ignoreTextEvent
                 ) {
            editor_->cursor()->evaluateCommand(Utils::textByKey(Qt::Key(e->key())
                                                       , e->text()
                                                       , e->modifiers().testFlag(Qt::ShiftModifier)
                                                       , editor_->isTeacherMode() && editor_->analizer()
                                                      ));
        }

        Qt::Key tempSwichLayoutKey = Qt::Key(
                    editor_->mySettings()->value(
                        SettingsPage::KeyTempSwitchLayoutButton
                        , SettingsPage::DefaultTempSwitchLayoutButton)
                    .toUInt()
                    );
        if (e->key()==tempSwichLayoutKey) {
            Utils::altKeyPressed = true;
        }
        if (e->key()==Qt::Key_Shift) {
            Utils::shiftKeyPressed = true;
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
    if (!editor_->analizerInstance_ ||
            !editor_->analizerInstance_->helper())
        return;
    QString before, after;
    if (editor_->cursor()->row()<editor_->document()->linesCount()) {
        QString line = editor_->document()->textAt(editor_->cursor()->row());
        int textPos = editor_->cursor()->column() - 2 * editor_->document()->indentAt(editor_->cursor()->row());
        textPos = qMax(0, textPos);
        before = line.mid(0, textPos);
        if (textPos<line.length()) {
            after = line.mid(textPos);
        }
    }
    QList<Shared::Analizer::Suggestion> suggestions =
            editor_->analizerInstance_->helper()->suggestAutoComplete(editor_->cursor()->row(), before, after);
    if (suggestions.isEmpty()) {
        emit message(tr("Can't suggest autocomplete"));
    }
    else {
        emit disableInsertActions();
        editor_->cursor()->removeSelection();
        editor_->cursor()->removeRectSelection();
        editor_->autocompleteWidget_->init(before, suggestions, nullptr);
        editor_->autocompleteWidget_->move(mapToGlobal(cursorRect().topLeft()+offset()));
        editor_->autocompleteWidget_->setVisible(true);
        editor_->autocompleteWidget_->activateWindow();
        editor_->autocompleteWidget_->setFocus();
    }
}

void EditorPlane::finishAutoCompletion(const QString &suggestion)
{
#ifdef QT_DEBUG
    editor_->autocompleteWidget_->hide();
    QApplication::processEvents();
#endif
    static const QString Delimeters = QString::fromLatin1(
                " ;:=()!,.@-+*/[]{}"
                );
    QString before, after;
    if (editor_->cursor()->row()<editor_->document()->linesCount()) {
        QString line = editor_->document()->textAt(editor_->cursor()->row());
        int textPos = editor_->cursor()->column() - 2 * editor_->document()->indentAt(editor_->cursor()->row());
        before = line.mid(0, textPos);
        if (textPos<line.length()) {
            after = line.mid(textPos);
        }
    }
    int leftPart = 0;
    QString text;
    if (!suggestion.startsWith(' ')) {
        for (int i=before.length()-1; i>=0; i--) {
            if (Delimeters.contains(before[i]))
                break;
            else
                leftPart += 1;
        }
        text = suggestion;
    }
    else if (before.length()>0) {
        QString endText;
        int removeCount = 0;
        for (int i=0; i<before.length(); i++) {
            endText = before.mid(before.length()-i-1);
            if (suggestion.startsWith(endText)) {
                break;
            }
        }
        if (endText.length()!=before.length()) {
            removeCount = endText.length();
            text = suggestion.mid(removeCount);
        }
        else {
            text = suggestion;
            bool textBeforeEndsWithDelimeter =
                    before.length() > 0 &&
                    Delimeters.contains(before.at(before.length()-1));
            if (text.startsWith(' ') &&
                    (before.length()==0 || textBeforeEndsWithDelimeter)) {
                while (text.startsWith(' '))
                    text.remove(0,1);
            }
        }
    }
    for (int i=0; i<leftPart; i++) {
        editor_->cursor()->evaluateCommand(KeyCommand::SelectPreviousChar);
    }
    editor_->cursor()->evaluateCommand(KeyCommand(text));
    emit message(QString());
}

void EditorPlane::selectAll()
{
    editor_->cursor()->evaluateCommand(KeyCommand::SelectAll);
}

void EditorPlane::copy()
{
    editor_->cursor()->evaluateCommand(KeyCommand::Copy);
}

void EditorPlane::paste()
{
    editor_->cursor()->evaluateCommand(KeyCommand::Paste);
    findCursor();
}

void EditorPlane::cut()
{
    editor_->cursor()->evaluateCommand(KeyCommand::Cut);
    findCursor();
}

bool EditorPlane::canDrop(const QPoint &pos, const QMimeData *data) const
{
    if (!editor_->cursor()->isEnabled()) {
        return false;
    }
    bool result = false;
//    if (data->hasUrls()) {
//        foreach (const QUrl &url, data->urls()) {
//            const QString filename = url.toLocalFile();
//            foreach (const QRegExp &rx, rxFilenamePattern_) {
//                if (rx.exactMatch(filename)) {
//                    result = true;
//                }
//            }
//        }
//    }
    if ( (!data->hasUrls() && data->hasText())
            || data->hasFormat(Clipboard::BlockMimeType))
    {
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
    editor_->cursor()->setViewMode(TextCursor::VM_Hidden);
    if (canDrop(e->pos(), e->mimeData())) {
        if (e->source()==this) {
            e->setDropAction(Qt::MoveAction);
        }
        if (e->mimeData()->hasUrls()) {
            marginBackgroundAlpha_ = 255;
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
                    marginBackgroundAlpha_ = 64;
                }
                else {
                    marginBackgroundAlpha_ = 255;
                }
            }
            else if (e->mimeData()->hasText()) {
                pnt_dropPosMarker = QPoint(col, row);
                if (col>widthInChars()-1) {
                    marginBackgroundAlpha_ = 64;
                }
                else {
                    marginBackgroundAlpha_ = 255;
                }
            }
            else {
                marginBackgroundAlpha_ = 255;
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
    editor_->cursor()->setViewMode(TextCursor::VM_Blinking);
    pnt_dropPosMarker = pnt_dropPosCorner = QPoint(-1000, -1000);
    marginBackgroundAlpha_ = 255;
    bool dropIntoSelection = false;
    int col = (e->pos().x()-offset().x())/charWidth();
    int row = (e->pos().y()-offset().y())/lineHeight();
    col = qMax(col, 0);
    row = qMax(row, 0);
    int fromRow, fromCol, toRow, toCol;
    editor_->document()->undoStack()->beginMacro("dragndrop");
    if (editor_->cursor()->hasSelection()) {
        editor_->cursor()->selectionBounds(fromRow, fromCol, toRow, toCol);
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
    if (editor_->cursor()->hasRectSelection()) {
        r = editor_->cursor()->selectionRect();
        dropIntoSelection = r.contains(col, row);
    }
    if (e->mimeData()->hasUrls()) {
        QList<QUrl> urls = e->mimeData()->urls();
        QList<QUrl> validUrls;
        foreach (QUrl url, urls) {
            const QString fileName = url.toLocalFile();
            const QString filenameSuffix =
                    editor_->analizerPlugin_ ? editor_->analizerPlugin_->defaultDocumentFileNameSuffix()
                              : ".txt";
            if (fileName.endsWith(filenameSuffix)) {
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
        if (editor_->cursor()->hasSelection()) {
            editor_->cursor()->removeSelectedText();
        }
        if (editor_->cursor()->hasRectSelection()) {
            editor_->cursor()->removeSelectedBlock();
        }
    }
    else {
        if (e->source()==this) {
            if (editor_->cursor()->hasSelection()) {
                int afromRow, atoRow, afromCol, atoCol;
                editor_->cursor()->selectionBounds(afromRow, afromCol, atoRow, atoCol);
                editor_->cursor()->setRow(afromRow);
                editor_->cursor()->setColumn(afromCol);
                if (row>=toRow) {
                    QString st = editor_->cursor()->selectedText();
                    row -= st.count("\n");
                }
                if (row == toRow && atoCol <= col) {
                    QString st = editor_->cursor()->selectedText();
                    col -= st.length();
                }
                editor_->cursor()->removeSelectedText();
                editor_->cursor()->setRow(toRow);
                editor_->cursor()->setColumn(toCol);
//                if (row>=toRow) {
//                    row -= text.count("\n");
//                    if (row==toRow && col>=toCol) {
//                        col -= text.split("\n").last().size();
//                    }
//                }
            }
            if (editor_->cursor()->hasRectSelection()) {
                editor_->cursor()->removeSelectedBlock();
//                if (row>=r.bottom()) {
//                    row -= r.height();
//                }
            }

        }
        else {
            if (editor_->cursor()->hasSelection()) {
                editor_->cursor()->removeSelection();
            }
            if (editor_->cursor()->hasRectSelection()) {
                editor_->cursor()->removeRectSelection();
            }
        }
    }
    editor_->cursor()->moveTo(row, col);
    if (e->mimeData()->hasFormat(Clipboard::BlockMimeType)) {
        editor_->cursor()->insertBlock(lines);
    }
    else if (e->mimeData()->hasText()) {
        editor_->cursor()->insertText(text);
    }
    editor_->document()->undoStack()->endMacro();
//    m_document->flushTransaction();
    editor_->document()->forceCompleteRecompilation(QPoint(editor_->cursor()->column(), editor_->cursor()->row()));

    update();
}

void EditorPlane::dragLeaveEvent(QDragLeaveEvent *e)
{
    editor_->cursor()->setViewMode(TextCursor::VM_Blinking);
    pnt_dropPosMarker = pnt_dropPosCorner = QPoint(-1000, -1000);
    marginBackgroundAlpha_ = 255;
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
    if (editor_->cursor()->isEnabled()) {
        editor_->cursor()->setViewMode(TextCursor::VM_Blinking);
    }
}

void EditorPlane::focusOutEvent(QFocusEvent *e)
{
    QWidget::focusOutEvent(e);
    if (editor_->cursor()->isEnabled()) {
        editor_->cursor()->setViewMode(TextCursor::VM_Hidden);
    }
}

void EditorPlane::removeLine()
{
    editor_->cursor()->evaluateCommand(KeyCommand::RemoveLine);
    findCursor();
}

void EditorPlane::removeLineTail()
{
    editor_->cursor()->evaluateCommand(KeyCommand::RemoveTail);
    findCursor();
}

QRect EditorPlane::cursorRect() const
{
    // Text coordinates
    uint row = editor_->cursor()->row();
    uint col = editor_->cursor()->column();

    // Sizes
    uint dX = charWidth();
    uint dY = lineHeight();



    if (editor_->cursor()->mode()==TextCursor::EM_Overwrite) {
        // Return a cursor rect the same height as line in this case
        return QRect(col*dY, row*dY, dX, dY);
    }
    else {
        // Return an "underline" or "vline" cursor rect
        return editor_->cursor()->isFreeCursorMovement()
                ? QRect(col*dX, (row+1)*dY-1, dX, 2)
                : QRect(col*dX, row*dY+4, 2, dY-2);
    }
}

uint EditorPlane::marginLeftBound() const
{
    return (widthInChars() + LEFT_MARGIN_SIZE) * charWidth() +
            (editor_->plugin_->teacherMode_ && editor_->analizerInstance_
             ? LOCK_SYMBOL_WIDTH : 0 ) - MARGIN_LINE_WIDTH / 2 ;
}

QRect EditorPlane::marginBackgroundRect() const
{
    return QRect(
                marginLeftBound() + MARGIN_LINE_WIDTH, // left
                1, // top
                width() - marginLeftBound() - MARGIN_LINE_WIDTH -3, // width
                height() - 2 // height
                );
}

QRect EditorPlane::marginLineRect() const
{
    return QRect(
                marginLeftBound(), // left
                0, // top
                MARGIN_LINE_WIDTH, // width
                height() // height
                );
}

/** Draws margin background and margin line
 * @param p the initialized painter
 * @param rect the rect to be repainted
 */
void EditorPlane::paintMarginBackground(QPainter *p, const QRect &rect)
{
    // Draw a background

    // No border for the rect
    p->setPen(Qt::NoPen);

    // Use the same color as for editable area
    QColor marginBacgroundColor = palette().brush(QPalette::Base).color();
    // It may be semi-transparent in case of text dragging
    marginBacgroundColor.setAlpha(marginBackgroundAlpha_);
    p->setBrush(marginBacgroundColor);
    p->drawRect(marginBackgroundRect().intersected(rect));

    // Draw margin line
    unsigned errorsCount = editor_->analizer() ? editor_->analizer()->errors().size() : 0u;
    QColor marginLineColor = palette().color(hasFocus()? QPalette::Highlight : QPalette::Mid);
    if (errorsCount) {
        const QColor bgColor = palette().color(QPalette::Base);
        int darkness = bgColor.red() + bgColor.green() + bgColor.blue();
        if (darkness / 3 <= 127) {
            // Invert color for dark backround
            marginLineColor = QColor("orangered");
        }
        else {
            marginLineColor = QColor("red");
        }
    }
    marginLineColor.setAlpha(marginBackgroundAlpha_);
    p->setBrush(marginLineColor);
    p->drawRect(marginLineRect().intersected(rect));
}

/** Draws main background
 * @param p the initialized painter
 * @param rect the rect to be repainted
 */
void EditorPlane::paintBackground(QPainter *p, const QRect &rect)
{
    p->setPen(Qt::NoPen);
    p->setBrush(palette().brush(QPalette::Base));
    p->drawRect(rect);
}

/** Draws rectangle text selection
 * @param p the initialized painter
 * @param rect the rect to be repainted
 */
void EditorPlane::paintRectSelection(QPainter *p, const QRect &)
{
    if (!editor_->cursor()->hasRectSelection())
        return;
    p->save();
    p->setPen(QPen(palette().brush(hasFocus()? QPalette::Active : QPalette::Inactive, QPalette::Highlight)
                   , 2
                   , Qt::SolidLine));
    p->setBrush(Qt::NoBrush);
    QRect selRect = editor_->cursor()->selectionRect();
    QRect r(selRect.left() * charWidth() + 1,
            selRect.top() * lineHeight() + 1,
            selRect.width() * charWidth() - 2,
            selRect.height() * lineHeight() -2 );

    p->drawRect(r);

    p->restore();
}

/** Draws regular text selection
 * @param p the initialized painter
 * @param rect the rect to be repainted
 */
void EditorPlane::paintSelection(QPainter *p, const QRect &rect)
{
    Q_UNUSED(rect);
    p->save();
    p->setPen(Qt::NoPen);
    p->setBrush(palette().brush(hasFocus()? QPalette::Active : QPalette::Inactive, QPalette::Highlight));
    int startLine = 0;
//    int startLine = qMax(0, rect.top() / lineHeight() - 1);
//    int endLine = rect.bottom() / lineHeight() + 1;
    int endLine = editor_->document()->linesCount();
    int lh = lineHeight();
    int cw = charWidth();
    bool prevLineSelected = false;
    for (int i=startLine; i<endLine+1; i++) {
        if (i<editor_->document()->linesCount()) {
            int indentSpace = 2 * cw * editor_->document()->indentAt(i);
            if (prevLineSelected) {
                p->drawRect(0, i*lh, indentSpace, lh);
            }
            QList<bool> sm = editor_->document()->selectionMaskAt(i);
            for (int j=0; j<sm.size(); j++) {
                if (sm[j])
                    p->drawRect(indentSpace+j*cw, i*lh, cw, lh);
            }
            if (editor_->document()->lineEndSelectedAt(i)) {
                prevLineSelected = true;
                int textLength = editor_->document()->textAt(i).length()*cw;
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

/** Draws a block to the left of editable area
 * @param p the initialized painter
 * @param rect the rect to be repainted
 */
void EditorPlane::paintLineNumbers(QPainter *p, const QRect &rect)
{
    // Determine which text lines actually required to be repainted (inclusive)
    const uint startLine = qMax(0, rect.top() / int(lineHeight()) - 1);
    const uint endLine = qMax(0, rect.bottom() / int(lineHeight()) + 1);

    uint lockSymbolOffset = editor_->plugin_->teacherMode_? LOCK_SYMBOL_WIDTH : 0;
    for (uint i=startLine; i<=endLine; i++) {

        // Visible line number accounting Y-scroll offset
        uint realLineNumber = i + qMax(0, -offset().y()) / lineHeight();

        // Draw a background

        // The line number backround is not editable, so use theme backround
        // color and theme text color
        p->setPen(Qt::NoPen);
        p->setBrush(palette().brush(QPalette::Window));
        p->drawRect(0,
                    i * lineHeight(),
                    charWidth() * 4+ charWidth() / 2 + lockSymbolOffset,
                    lineHeight()
                    );

        // Draw a small white border to the right
        p->setBrush(palette().brush(QPalette::Base));
        p->drawRect(charWidth() * 4 + charWidth() / 2 + lockSymbolOffset,
                    i * lineHeight(),
                    charWidth() / 2,
                    lineHeight()
                    );


        const QColor textColor = realLineNumber <= editor_->document()->linesCount()
                  // If line exists, draw number using regular fg color
                ? QColor(palette().brush(QPalette::WindowText).color())
                  // else draw using lighter color
                : QColor(palette().brush(QPalette::Disabled, QPalette::WindowText).color());
//                : QColor(Qt::lightGray);

        p->setPen(textColor);
        const QColor bgColor = palette().color(QPalette::Base);
        int darkness = bgColor.red() + bgColor.green() + bgColor.blue();
        if ((darkness / 3 <= 127) && (highlightedTextLineNumber_ + 1== realLineNumber)) {
            p->setPen(QColor(Qt::black));
        }

        // Calculate number width to align it centered
        const QString txt = QString::number(realLineNumber);
        int textWidth = QFontMetrics(font()).width(txt);
        int xx = charWidth() * 3 - textWidth + lockSymbolOffset;
        int yy = i * lineHeight() - 2;
        p->drawText(xx, yy, txt);

        if (editor_->plugin_->teacherMode_) {
            // Paint 'lock' symbol to the left of line number
            const QRect lockSymbolRect(
                        0,
                        i * lineHeight(),
                        LOCK_SYMBOL_WIDTH,
                        lineHeight()
                        );
            if (realLineNumber<editor_->document()->linesCount() &&
                    editor_->document()->isProtected(realLineNumber))
            {
                // Draw 'locked' symbol if line is protected
                paintLockSymbol(p, true, lockSymbolRect);
            }

            if (realLineNumber==highlightedLockSymbolLineNumber_ &&
                    realLineNumber<editor_->document()->linesCount() &&
                    !editor_->document()->isProtected(realLineNumber) )
            {
                // Draw 'locked' symbol outline in case of mouse over
                paintLockSymbol(p, false, lockSymbolRect);
            }
        }
    }
}

/** Draws a 'lock' symbol in teacher mode
 * @param p the initialized painter
 * @param rect the rect to be repainted
 */
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

    p->setPen(QPen(QColor(colored
                       ? Qt::black
                       : palette().brush(QPalette::WindowText).color()
                         ), 1));

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
        editor_->mySettings()->setValue(SettingsPage::KeyFontSize, currentSize);
        setFont(fnt);
        update();

    }
    if (!editor_->scrollBar(Qt::Vertical)->isEnabled() && e->orientation()==Qt::Vertical) {
        e->ignore();
        return;
    }
    if (!editor_->scrollBar(Qt::Horizontal)->isEnabled() && e->orientation()==Qt::Horizontal) {
        e->ignore();
        return;
    }
    int degrees = e->delta() / 8;
    int steps = degrees / 15;
    QScrollBar * sb = e->orientation()==Qt::Vertical? editor_->scrollBar(Qt::Vertical) : editor_->scrollBar(Qt::Horizontal);
    sb->setValue(sb->value()-steps*sb->singleStep()*3);

}

/** Draws margin text
 * @param p the initialized painter
 * @param rect the rect to be repainted
 */
void EditorPlane::paintMarginText(QPainter * p, const QRect &rect)
{
    // Test if there is enought space for margin text
    uint marginWidth =
            editor_->mySettings()->value(MarginWidthKey, MarginWidthDefault).toUInt();
    if (marginWidth<3)
        return;

    // Determine which text lines actually required to be repainted (inclusive)
//    const uint startLine = qMax(0, rect.top() / int(lineHeight()) - 1);
//    const uint endLine =
//            qMax(0, qMin(
//                     int(rect.bottom() / lineHeight() + 1),
//                     int(editor_->document()->linesCount())-1
//                     )
//                 );

    const uint startLine = 0;
    const uint endLine = editor_->document()->linesCount();

    // Prepare colors
    QColor errorColor(Qt::red);
    errorColor.setAlpha(marginBackgroundAlpha_);
    QColor marginColor(Qt::black);
    marginColor.setAlpha(marginBackgroundAlpha_);
    const QColor bgColor = palette().color(QPalette::Base);
    int darkness = bgColor.red() + bgColor.green() + bgColor.blue();
    if (darkness / 3 <= 127) {
        // Invert color for dark backround
        errorColor = QColor("orangered");
        marginColor = QColor(Qt::white);
    }

    // Calculate text position
    uint marginTextLeft = marginLeftBound() +
            MARGIN_LINE_WIDTH +
            + charWidth() / 2;

    for (uint i=startLine; i<endLine; i++) {
        if (editor_->document()->marginAt(i).text.length() > 0 ||
                editor_->document()->marginAt(i).errors.size() > 0)
        {
            // If something exists on margin

            // Text bottom position
            const uint y =  ( i + 1 ) * lineHeight();

            // Text to show
            const QString & text =
                    editor_->document()->marginAt(i).text.length() > 0
                    ? editor_->document()->marginAt(i).text
                    : editor_->document()->marginAt(i).errors.first();

            // Set corresponding text color
            QColor color = editor_->document()->marginAt(i).text.length() > 0
                    ? marginColor : errorColor ;
            if (darkness / 3 <= 127 && highlightedTextLineNumber_ == i) {
                color = QColor(Qt::black);
            }
            p->setPen(color);

            // Draw a text line
            p->drawText(marginTextLeft, y+offset().y(), text);
        }
    }
}

/** Draws editable text
 * @param p the initialized painter
 * @param rect the rect to be repainted
 */
void EditorPlane::paintText(QPainter *p, const QRect &rect)
{
    // Determine which text lines actually required to be repainted (inclusive)
    const uint startLine = qMax(0, rect.top() / int(lineHeight()) - 1);
    const uint endLine =
            qMax(0, qMin(
                     int(rect.bottom() / lineHeight() + 1),
                     int(editor_->document()->linesCount())-1
                     )
                 );

    // Find a line in the document from which hidden text begins,
    // -1 value means 'not found'
    // TODO implement using std::algorithm with predicate (requires C++11)
    int hiddenLineStart = -1;
    if (editor_->plugin_->teacherMode_ && editor_->analizerInstance_)
    for (uint i=0; i<editor_->document()->linesCount(); i++) {
        if (editor_->document()->isHidden(i)) {
            hiddenLineStart = i;
            break;
        }
    }

    // Draw protection/hidden background
    for (uint i=startLine; i<endLine+1; i++) {

        // Top bound
        const uint y =  i * lineHeight();

        // Whether to draw a rect or not
        bool drawThisSpecialRectFlag = false;

        // The rect itself
        const QRect specialLineRect (0, y, width(), lineHeight());

        bool protectedLine = editor_->document()->isProtected(i) && !editor_->document()->isHidden(i);

        if (protectedLine) {
            // Line is protected
            p->setBrush(PROTECTED_LINE_BACKGROUND);
            drawThisSpecialRectFlag = true;
        }
        else if (hiddenLineStart!=-1 && int(i)>=hiddenLineStart) {
            // Line is hidden
            p->setBrush(HIDDEN_LINE_BACKGROUND);
            drawThisSpecialRectFlag = true;
        }

        if (drawThisSpecialRectFlag) {
            // Perform drawing a rect
            p->setPen(Qt::NoPen);
            p->drawRect(specialLineRect);
        }
    }

    // Draw remaining text area in case of last line is protected
    if (!editor_->isTeacherMode() && editor_->document()->linesCount() > 0) {
        const TextLine & lastLine =
                editor_->document()->at(editor_->document()->linesCount()-1);
        if (lastLine.protecteed) {
            const uint y = lineHeight() * editor_->document()->linesCount();
            const QRect specialLineRect (0, y, width(), height() - y - offset().y());
            p->setBrush(PROTECTED_LINE_BACKGROUND);
            p->setPen(Qt::NoPen);
            p->drawRect(specialLineRect);
        }
    }

    // Draw text lines themselves
    for (uint i=startLine; i<=endLine; i++)
    {
        // Indent count (in logical levels)
        uint indent = editor_->document()->indentAt(i);

        // Bottom text bound (so 'i + 1' instead of 'i')
        const uint y =  ( i + 1 )* lineHeight();

        // Draw black 'dots' at start of lines
        p->setBrush(QColor(Qt::black));
        if (editor_->mySettings()->value(SettingsPage::KeyInvertColorsIfDarkSystemTheme,
                            SettingsPage::DefaultInvertColorsIfDarkSystemTheme)
                .toBool()
                )
        {
            // Check for dark background
            const QColor bgColor = palette().color(QPalette::Base);
            int darkness = bgColor.red() + bgColor.green() + bgColor.blue();
            if (darkness / 3 <= 127) {
                // Invert color for dark backround
                if (i != highlightedTextLineNumber_)
                    p->setBrush(Qt::white);
            }
        }
        p->setPen(Qt::NoPen);
        for (uint j=0; j<indent; j++) {
            const uint dotSize = qMin(lineHeight() / 3u,
                                      charWidth() / 3u);
            const uint dotX = j * charWidth() * 2 + (charWidth()-dotSize);
            const uint dotY = y - lineHeight() + (lineHeight()-dotSize);
            const QRect dotRect(dotX, dotY, dotSize, dotSize);
            p->drawRect(dotRect);
        }

        // Requires lexem types for propertly highlighting
        const QList<Shared::LexemType> & highlight = editor_->document()->highlightAt(i);
        const QString& text = editor_->document()->textAt(i);

        // Calculate trailing spaces to show them in special way if need
        uint trailingSpaces = 0;
        for (int j=0; j<text.length(); j++) {
            if (text.at(text.length()-j-1).isSpace())
                trailingSpaces += 1;
            else break;
        }
        
        // Requires selection mask due to selected text color differs
        const QList<bool>& sm = editor_->document()->selectionMaskAt(i);

        // Current lexem type, by default -- regular text
        Shared::LexemType curType = Shared::LexemType(0);

        // Set current proper format for default type and non-letter
        setProperFormat(p, curType, '.');

        // Draw line letters
        for (uint j=0; j<uint(qMax(0, text.size()-int(trailingSpaces))); j++) {

            // Offet by indent
            uint offset = ( indent * 2 + j ) * charWidth();

            // Get current lexem type
            if (j<uint(highlight.size()))
                curType = highlight[j];

            // Set proper format for lexem type and current character
            setProperFormat(p, curType, text[j]);
            if (i == highlightedTextLineNumber_) {
                const QColor bgColor = palette().color(QPalette::Base);
                int darkness = bgColor.red() + bgColor.green() + bgColor.blue();
                if (darkness / 3 <= 127) {
                    p->setPen(QColor(Qt::black));
                }
            }

            // If this character is selected, then set proper text color
            if (j<uint(sm.size()) && sm[j]) {
                p->setPen(palette().brush(QPalette::HighlightedText).color());
            }

            // If line is highlighted, then make text some darker
            // for better accessibility
            if (highlightedTextLineNumber_==i) {
                p->setPen(p->pen().color().darker());
            }

            // Align character horizontally to it's position in case
            // if various characters have different width
            const int charW = QFontMetrics(p->font()).width(text[j]);
            if (charW<charWidth()) {
                offset += (charWidth()-charW)/2;
            }

            if (curType==LxTypeComment && text[j]=='|') {
                // A comment symbol '|' must be drawn as accessible as possible
                p->setPen(QPen(p->pen().brush(), 2));
                p->drawLine(offset+charWidth()/2, y, offset+charWidth()/2, y-lineHeight()+2);
            }
            else {
                // Draw a symbol using obtained format
                p->drawText(offset, y,  QString(text[j]));
            }

            // If there is an error then draw underline
            if (curType & Shared::LxTypeError && !(curType & Shared::LxTypeComment)) {
                p->setPen(QPen(QColor(Qt::red),1));
                const QColor bgColor = palette().color(QPalette::Base);
                int darkness = bgColor.red() + bgColor.green() + bgColor.blue();
                if (darkness / 3 <= 127) {
                    // Invert color for dark backround
                    p->setPen(QColor("orangered"));
                }
                QPolygon pp = errorUnderline(offset, y+2, charWidth());
                p->drawPolyline(pp);
            }
        }

        // Draw trailing spaces if they are visible by user settings
        if (trailingSpaces && editor_->mySettings()->value(
                    SettingsPage::KeyShowTrailingSpaces,
                    SettingsPage::DefaultShowTrailingSpaces
                    ).toBool())
        {
            // Trainling spaces are lighter then primary text
            p->setPen(QColor("lightgray"));
            // Paint trailing spaces
            QString dots;
            dots.fill('.', trailingSpaces);
            uint offset =
                    (indent*2 + text.length() - trailingSpaces) * charWidth();
            p->drawText(offset, y, dots);
        }

    } // end for (uint i=startLine; i<=endLine; i++)
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

/** Set painter's format for specified lexem type and character
 * @param p the painter to setup
 * @param type lexem type
 * @param ch a character used to resolve italic font usage
 */
void EditorPlane::setProperFormat(
        QPainter *p,
        Shared::LexemType type,
        const QChar &ch
        )
{
    // Current font to modify
    QFont f = font();

    // Text color, by default is black
    QColor c = Qt::black;

    // Remove error flag (first bit in mask) and make it uint32_t
    const uint32_t t = (type << 1) >> 1;

    // Bit masks for certain groups
    static const uint32_t PriKwd = Shared::LxTypePrimaryKwd;
    static const uint32_t SecKwd = Shared::LxTypeSecondaryKwd;
    static const uint32_t NameClass = Shared::LxNameClass;
    static const uint32_t NameAlg = Shared::LxNameAlg;
    static const uint32_t NameModule = Shared::LxNameModule;
    static const uint32_t ConstLiteral = Shared::LxConstLiteral;
    static const uint32_t TypeConstant = Shared::LxTypeConstant;
    static const uint32_t TypeDoc = Shared::LxTypeDoc;
    static const uint32_t TypeComment = Shared::LxTypeComment;

    // Test if provided lexem type matches these groups
    const uint32_t priKwd = PriKwd & t;
    const uint32_t secKwd = SecKwd & t;
    const uint32_t nameClass = NameClass == t;
    const uint32_t nameAlg = NameAlg == t;
    const uint32_t nameModule = NameModule == t;
    const uint32_t literalConstant = ConstLiteral == t;
    const uint32_t constant = TypeConstant & t;
    const uint32_t doc = TypeDoc == t;
    const uint32_t comment = TypeComment == t;

    // Flags for case-insensitive grammatics (like Pascal language)
    static const QFont::Capitalization caseInsensitiveKw = QFont::AllLowercase;
    static const QFont::Capitalization caseInsensitiveType = QFont::MixedCase;

    // The code below uses settings values
    if (priKwd || secKwd) {
        // Matched keyword
        c = editor_->mySettings()->value(SettingsPage::KeyColorKw,
                             SettingsPage::DefaultColorKw).toString();
        f.setBold(editor_->mySettings()->value(SettingsPage::KeyBoldKw,
                                   SettingsPage::DefaultBoldKw).toBool());
        if (editor_->analizerPlugin_->caseInsensitiveGrammatic()) {
            f.setCapitalization(caseInsensitiveKw);
        }
    }
    if (nameClass) {
        // Matched name of type/class
        c = editor_->mySettings()->value(SettingsPage::KeyColorType,
                             SettingsPage::DefaultColorType).toString();
        f.setBold(editor_->mySettings()->value(SettingsPage::KeyBoldType,
                                   SettingsPage::DefaultBoldType).toBool());
        if (editor_->analizerPlugin_->caseInsensitiveGrammatic()) {
            f.setCapitalization(caseInsensitiveType);
        }
    }
    else if (nameAlg) {
        // Matched name of algorithm/procedure/function
        c = editor_->mySettings()->value(SettingsPage::KeyColorAlg,
                             SettingsPage::DefaultColorAlg).toString();
        f.setBold(editor_->mySettings()->value(SettingsPage::KeyBoldAlg,
                                   SettingsPage::DefaultBoldAlg).toBool());
    }
    else if (nameModule) {
        // Matched name of unit/module
        c = editor_->mySettings()->value(SettingsPage::KeyColorMod,
                             SettingsPage::DefaultColorMod).toString();
        f.setBold(editor_->mySettings()->value(SettingsPage::KeyBoldMod,
                                   SettingsPage::DefaultBoldMod).toBool());
    }
    else if (literalConstant) {
        // Matched literal constant
        c = editor_->mySettings()->value(SettingsPage::KeyColorLiteral,
                             SettingsPage::DefaultColorLiteral).toString();
        f.setBold(editor_->mySettings()->value(SettingsPage::KeyBoldLiteral,
                                   SettingsPage::DefaultBoldLiteral).toBool());
    }
    else if (constant)
    {
        // Matched any other constant
        c = editor_->mySettings()->value(SettingsPage::KeyColorNumeric,
                             SettingsPage::DefaultColorNumeric).toString();
        f.setBold(editor_->mySettings()->value(SettingsPage::KeyBoldNumeric,
                                   SettingsPage::DefaultBoldNumeric).toBool());
    }
    else if (doc) {
        // Matched documentation string
        c = editor_->mySettings()->value(SettingsPage::KeyColorDoc,
                             SettingsPage::DefaultColorDoc).toString();
        f.setBold(editor_->mySettings()->value(SettingsPage::KeyBoldDoc,
                                   SettingsPage::DefaultBoldDoc).toBool());
    }
    else if (comment) {
        // Matched comment
        c = editor_->mySettings()->value(SettingsPage::KeyColorComment,
                             SettingsPage::DefaultColorComment).toString();
        f.setBold(editor_->mySettings()->value(SettingsPage::KeyBoldComment,
                                   SettingsPage::DefaultBoldComment).toBool());
        f.setItalic(ch.isLetter() || ch.isDigit());
    }

    // Make letter italic if latin-italization available
    if (editor_->analizerPlugin_ && // it is source code editor
            !editor_->analizerPlugin_->primaryAlphabetIsLatin() &&  // italization possible
            ch!='\0' && // char is valid
            ch.isLetter() &&  // char is a letter
            ch.toLatin1()!='\0' //char is valid (see above), but its ASCII is not
            )
    {
        f.setItalic(true);
    }

    // Update a painter
    p->setFont(f);

    if (editor_->mySettings()->value(SettingsPage::KeyInvertColorsIfDarkSystemTheme,
                        SettingsPage::DefaultInvertColorsIfDarkSystemTheme)
            .toBool()
            )
    {
        // Check for dark background
        const QColor bgColor = palette().color(QPalette::Base);
        int darkness = bgColor.red() + bgColor.green() + bgColor.blue();
        if (darkness / 3 <= 127) {
            // Invert color for dark backround
            c.setRed(255 - c.red());
            c.setGreen(255 - c.green());
            c.setBlue(255 - c.blue());
        }
    }

    p->setPen(c);
}

/** Width of editable area
 * @return winth (in characters) of editable area
 */
uint EditorPlane::widthInChars() const
{
    ExtensionSystem::SettingsPtr sett = editor_->mySettings();
    if (!sett)
        return 0u;
    const uint cw = charWidth();
    uint marginWidthInPixels =
            cw * sett->value(MarginWidthKey, MarginWidthDefault).toUInt();
    if (!editor_->analizerInstance_)
        marginWidthInPixels = 0;
    const uint myWidth = width();
    const uint lockSymbolWidth = (editor_->analizerInstance_ && editor_->plugin_->teacherMode_)
            ? LOCK_SYMBOL_WIDTH
            : 0;
    const int availableWidth = myWidth - marginWidthInPixels - lockSymbolWidth;
    const int result = availableWidth / cw - 5;
    return uint( qMax(0, result) );
}

} // namespace Editor
