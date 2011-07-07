#include "terminal_plane.h"
#include "terminal.h"
#include "terminal_onesession.h"

namespace Terminal {

static int sessionMargin = 4;

Plane::Plane(Term *parent)
    : QWidget(parent)
    , m_terminal(parent)
{
    b_inputMode = false;
    i_inputPosition = 0;
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

void Plane::keyPressEvent(QKeyEvent *e)
{
    if (!b_inputMode) {
        e->ignore();
        return;
    }
    if (e->matches(QKeySequence::MoveToNextChar)) {
        i_inputPosition++;
        emit inputCursorPositionChanged(i_inputPosition);
        e->accept();
    }
    else if (e->matches(QKeySequence::MoveToPreviousChar)) {
        if (i_inputPosition>0) {
            i_inputPosition --;
            emit inputCursorPositionChanged(i_inputPosition);
            e->accept();
        }
    }
    else if (e->matches(QKeySequence::MoveToStartOfLine)
             || e->matches(QKeySequence::MoveToStartOfDocument)
             || e->matches(QKeySequence::MoveToPreviousLine)
             ) {
        i_inputPosition = 0;
        emit inputCursorPositionChanged(i_inputPosition);
        e->accept();
    }
    else if (e->matches(QKeySequence::MoveToEndOfLine)
             || e->matches(QKeySequence::MoveToEndOfDocument)
             || e->matches(QKeySequence::MoveToNextLine)
             ) {
        i_inputPosition = s_inputText.length();
        emit inputCursorPositionChanged(i_inputPosition);
        e->accept();
    }
    else if (e->key()==Qt::Key_Backspace) {
        if (i_inputPosition>0) {
            if (i_inputPosition>s_inputText.length()) {
                i_inputPosition = s_inputText.length();
                emit inputCursorPositionChanged(i_inputPosition);
            }
            else {
                s_inputText.remove(i_inputPosition-1, 1);
                i_inputPosition --;
                emit inputCursorPositionChanged(i_inputPosition);
                emit inputTextChanged(s_inputText);
            }
            e->accept();
        }
    }
    else if (e->key()==Qt::Key_Delete) {
        if (i_inputPosition<s_inputText.length()) {
            s_inputText.remove(i_inputPosition, 1);
            emit inputCursorPositionChanged(i_inputPosition);
            emit inputTextChanged(s_inputText);
            e->accept();
        }
    }
    else if (e->key()==Qt::Key_Return || e->key()==Qt::Key_Enter) {
        emit inputFinishRequest();
        e->accept();
    }
    else if (!e->text().isEmpty()) {
        while (i_inputPosition>s_inputText.length())
            s_inputText += " ";
        s_inputText.insert(i_inputPosition, e->text());
        i_inputPosition += e->text().length();
        emit inputTextChanged(s_inputText);
        emit inputCursorPositionChanged(i_inputPosition);
        e->accept();
    }
}

void Plane::mousePressEvent(QMouseEvent *e)
{
    setFocus();
    e->accept();
}

QPoint Plane::offset() const
{
    QPoint result(0,0);
    if (m_terminal->sb_horizontal->isEnabled()) {
        int valX = m_terminal->sb_horizontal->value();
        result.setX(-valX);
    }
    if (m_terminal->sb_vertical->isEnabled()) {
        int valY = m_terminal->sb_vertical->value();
        result.setY(-valY);
    }
    return result;
}


void Plane::updateScrollBars()
{
    QPoint prevOffset = offset();
    int w = 2 * sessionMargin;
    int h = 2 * sessionMargin;
    for (int i=0; i<m_terminal->l_sessions.size(); i++) {
        OneSession * s = m_terminal->l_sessions[i];
        QSize ss = s->visibleSize(width()-2*sessionMargin);
        w = qMax(w, sessionMargin*2 + ss.width());
        h += sessionMargin + ss.height();
        if (i==m_terminal->l_sessions.size()-1) {
            h += height()-ss.height()-2*sessionMargin;
        }
    }

    QScrollBar * hb = m_terminal->sb_horizontal;
    QScrollBar * vb = m_terminal->sb_vertical;

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
    if (!m_terminal->sb_vertical->isEnabled() && e->orientation()==Qt::Vertical) {
        e->ignore();
        return;
    }
    if (!m_terminal->sb_horizontal->isEnabled() && e->orientation()==Qt::Horizontal) {
        e->ignore();
        return;
    }
    int degrees = e->delta() / 8;
    int steps = degrees / 15;
    QScrollBar * sb = e->orientation()==Qt::Vertical? m_terminal->sb_vertical : m_terminal->sb_horizontal;
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
    for (int i=0; i<m_terminal->l_sessions.size(); i++) {
        OneSession * session = m_terminal->l_sessions[i];
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
