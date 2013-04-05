#include "terminal_plane.h"
#include "terminal.h"
#include "terminal_onesession.h"

#include <algorithm>

namespace Terminal {

static int sessionMargin = 4;

Plane::Plane(Term *parent)
    : QWidget(parent)
    , terminal_(parent)
    , inputMode_(false)
    , inputPosition_(0)
    , selectedSession_(nullptr)
    , actionCopyToClipboard_(new QAction(this))
    , actionPasteFromClipboard_(new QAction(this))
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    actionCopyToClipboard_->setText(tr("Copy to clipboard"));
    connect(actionCopyToClipboard_, SIGNAL(triggered()),
            this, SLOT(copyToClipboard()));

    actionPasteFromClipboard_->setText(tr("Paste from clipboard"));
    connect(actionPasteFromClipboard_, SIGNAL(triggered()),
            this, SLOT(pasteFromClipboard()));

}

void Plane::keyPressEvent(QKeyEvent *e)
{
    if (!inputMode_) {
        e->ignore();
        return;
    }
    if (e->matches(QKeySequence::MoveToNextChar)) {
        inputPosition_++;
        emit inputCursorPositionChanged(inputPosition_);
        e->accept();
    }
    else if (e->matches(QKeySequence::MoveToPreviousChar)) {
        if (inputPosition_>0) {
            inputPosition_ --;
            emit inputCursorPositionChanged(inputPosition_);
            e->accept();
        }
    }
    else if (e->matches(QKeySequence::MoveToStartOfLine)
             || e->matches(QKeySequence::MoveToStartOfDocument)
             || e->matches(QKeySequence::MoveToPreviousLine)
             ) {
        inputPosition_ = 0;
        emit inputCursorPositionChanged(inputPosition_);
        e->accept();
    }
    else if (e->matches(QKeySequence::MoveToEndOfLine)
             || e->matches(QKeySequence::MoveToEndOfDocument)
             || e->matches(QKeySequence::MoveToNextLine)
             ) {
        inputPosition_ = inputText_.length();
        emit inputCursorPositionChanged(inputPosition_);
        e->accept();
    }
    else if (e->key()==Qt::Key_Backspace) {
        if (inputPosition_>0) {
            if (inputPosition_>inputText_.length()) {
                inputPosition_ = inputText_.length();
                emit inputCursorPositionChanged(inputPosition_);
            }
            else {
                inputText_.remove(inputPosition_-1, 1);
                inputPosition_ --;
                emit inputCursorPositionChanged(inputPosition_);
                emit inputTextChanged(inputText_);
            }
            e->accept();
        }
    }
    else if (e->key()==Qt::Key_Delete) {
        if (inputPosition_<inputText_.length()) {
            inputText_.remove(inputPosition_, 1);
            emit inputCursorPositionChanged(inputPosition_);
            emit inputTextChanged(inputText_);
            e->accept();
        }
    }
    else if (e->key()==Qt::Key_Return || e->key()==Qt::Key_Enter) {
        emit inputFinishRequest();
        e->accept();
    }
    else if (!e->text().isEmpty()) {
        while (inputPosition_>inputText_.length())
            inputText_ += " ";
        inputText_.insert(inputPosition_, e->text());
        inputPosition_ += e->text().length();
        emit inputTextChanged(inputText_);
        emit inputCursorPositionChanged(inputPosition_);
        e->accept();
    }
}

OneSession * Plane::sessionByPos(const QPoint &pos) const
{
    OneSession * result = nullptr;
    foreach (OneSession * session, terminal_->sessions_) {
        if (sessionRect(session).contains(pos)) {
            result = session;
            break;
        }
    }
    return result;
}

QRect Plane::sessionRect(const OneSession *session) const
{
    QRect result(offset() + QPoint(sessionMargin, 0), QSize(0,0));
    foreach (const OneSession * s, terminal_->sessions_) {
        result.setSize(s->visibleSize(width()-2*sessionMargin));
        if (session==s)
            break;
        result.translate(0, result.height()+sessionMargin);
    }
    result.setWidth(qMax(result.width(), width()-2*sessionMargin));
    return result;
}

void Plane::mousePressEvent(QMouseEvent *e)
{
    setFocus();
    e->accept();
    selectedSession_ = sessionByPos(e->pos());
    mousePressPosition_ = e->pos();
    if (e->button()!=Qt::RightButton) {
        for (int i=0; i<terminal_->sessions_.size(); i++) {
            terminal_->sessions_.at(i)->clearSelection();
        }
    }
    update();
}

void Plane::mouseMoveEvent(QMouseEvent *e)
{
    e->accept();
    if (selectedSession_) {
        const QPoint sessionOffset = sessionRect(selectedSession_).topLeft();
        const QPoint fromPos = mousePressPosition_ - sessionOffset;
        const QPoint toPos   = e->pos() - sessionOffset;
        selectedSession_->triggerTextSelection(fromPos, toPos);
        update();
    }
}

void Plane::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();
}

QPoint Plane::offset() const
{
    QPoint result(0,0);
    if (terminal_->sb_horizontal->isEnabled()) {
        int valX = terminal_->sb_horizontal->value();
        result.setX(-valX);
    }
    if (terminal_->sb_vertical->isEnabled()) {
        int valY = terminal_->sb_vertical->value();
        result.setY(-valY);
    }
    return result;
}

void Plane::contextMenuEvent(QContextMenuEvent * event)
{
    event->accept();
    bool canCopyToClipboard = false;
    foreach (const OneSession * s, terminal_->sessions_) {
        canCopyToClipboard = canCopyToClipboard || s->hasSelectedText();
    }
    const QClipboard * clipboard = QApplication::clipboard();
    bool canPasteFromClipboard = inputMode_ &&
            clipboard->text().length()>0;
    bool hasAnyAction =
            canCopyToClipboard ||
            canPasteFromClipboard;

    if (hasAnyAction) {
        QMenu * menu = new QMenu(this);
        if (canCopyToClipboard)
            menu->addAction(actionCopyToClipboard_);
        if (canPasteFromClipboard)
            menu->addAction(actionPasteFromClipboard_);
        menu->exec(mapToGlobal(event->pos()));
    }
}

void Plane::copyToClipboard()
{
    QClipboard * clipboard = QApplication::clipboard();
    QString text;
    foreach (const OneSession * s, terminal_->sessions_) {
        text += s->selectedText();
    }
    clipboard->setText(text);
}

void Plane::pasteFromClipboard()
{
    const QClipboard * clipboard = QApplication::clipboard();
    const QString text = clipboard->text().replace("\n", " ");
    inputText_ += text;
    inputPosition_ += text.length();
    emit inputTextChanged(inputText_);
    emit inputCursorPositionChanged(inputPosition_);
}

void Plane::updateScrollBars()
{
    QPoint prevOffset = offset();
    int w = 2 * sessionMargin;
    int h = 2 * sessionMargin;
    for (int i=0; i<terminal_->sessions_.size(); i++) {
        OneSession * s = terminal_->sessions_[i];
        QSize ss = s->visibleSize(width()-2*sessionMargin);
        w = qMax(w, sessionMargin*2 + ss.width());
        h += sessionMargin + ss.height();
        if (i==terminal_->sessions_.size()-1) {
            h += qMax(0, height()-ss.height()-2*sessionMargin);
        }
    }

    QScrollBar * hb = terminal_->sb_horizontal;
    QScrollBar * vb = terminal_->sb_vertical;

    if (w<=width()) {
        hb->setEnabled(false);
        hb->setVisible(false);
    }
    else {
        hb->setEnabled(true);
        hb->setVisible(true);
        hb->setRange(0, w - width());
        hb->setSingleStep(width()/3);
        hb->setPageStep(width());
    }

    if (h<=height()) {
        vb->setEnabled(false);
        vb->setVisible(false);
    }
    else {
        QFontMetrics fm(font());
        int dH = fm.height();
        vb->setEnabled(true);
        vb->setVisible(true);
        vb->setRange(0, h - height());
        vb->setSingleStep(dH);
        vb->setPageStep(height());
    }
    if (prevOffset!=offset())
        update();

}

void Plane::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    updateScrollBars();
}

void Plane::wheelEvent(QWheelEvent *e)
{
    if (!terminal_->sb_vertical->isEnabled() && e->orientation()==Qt::Vertical) {
        e->ignore();
        return;
    }
    if (!terminal_->sb_horizontal->isEnabled() && e->orientation()==Qt::Horizontal) {
        e->ignore();
        return;
    }
    int degrees = e->delta() / 8;
    int steps = degrees / 15;
    QScrollBar * sb = e->orientation()==Qt::Vertical? terminal_->sb_vertical : terminal_->sb_horizontal;
    sb->setValue(sb->value()-steps*sb->singleStep()*3);
}

void Plane::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
//    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    const QBrush br = hasFocus()? palette().brush(QPalette::Highlight) : palette().brush(QPalette::Window);
    p.setPen(QPen(br,1));
    p.setBrush(palette().brush(QPalette::Base));
    p.drawRect(0,0,width(), height());

    QPoint off = offset();
    int y = sessionMargin;
    for (int i=0; i<terminal_->sessions_.size(); i++) {
        OneSession * session = terminal_->sessions_[i];
        const QSize sessionSize = session->visibleSize(width()-2*sessionMargin);
//        const QRect sessionRect = QRect(QPoint(sessionMargin, y), sessionSize).translated(off);
//        const QRect myRect(off, size());
//        if (myRect.intersects(sessionRect)) {
        {

            p.save();
            p.translate(sessionMargin+off.x(), y+off.y());
            session->draw(&p, width()-2*sessionMargin);
            p.restore();
        }
        y += sessionSize.height() + sessionMargin;
    }

    p.end();
    e->accept();
}

} // namespace Terminal
