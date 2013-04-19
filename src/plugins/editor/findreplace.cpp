// Self includes
#include "findreplace.h"
#include "ui_findreplace.h"

// Kumir includes
#include "textdocument.h"
#include "textcursor.h"

// Qt includes
#include <QPainter>
#include <QStyle>


namespace Editor {

FindReplace::FindReplace(
        TextDocument * document,
        TextCursor * cursor,
        QWidget * parent
        )
    : QWidget(parent)
    , ui(new Ui::FindReplace)
    , document_(document)
    , cursor_(cursor)
{
    ui->setupUi(this);
    connect(ui->btnMore, SIGNAL(clicked(bool)),
            this, SLOT(handleMoreButtonChecked(bool)));

    connect(ui->btnClose, SIGNAL(clicked()),
            this, SLOT(close()));

    setFont(QApplication::font());
    ui->find->setFont(parentWidget()->font());
    ui->find->setFont(parentWidget()->font());

    connect(ui->find, SIGNAL(textEdited(QString)),
            this, SLOT(doFindFirst(QString)));

    connect(ui->btnNext, SIGNAL(clicked()),
            this, SLOT(doFindNext()));

    connect(ui->btnPrev, SIGNAL(clicked()),
            this, SLOT(doFindPrevious()));

    connect(ui->searchMode, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleSearchParameterChanged()));

    connect(ui->matchCase, SIGNAL(clicked()),
            this, SLOT(handleSearchParameterChanged()));

    connect(ui->btnReplace, SIGNAL(clicked()),
            this, SLOT(doReplace()));

    connect(ui->btnReplaceAll, SIGNAL(clicked()),
            this, SLOT(doReplaceAll()));

    connect(ui->find, SIGNAL(returnPressed()),
            this, SLOT(handleReturnPressed()));

    connect(ui->replace, SIGNAL(returnPressed()),
            this, SLOT(handleReturnPressed()));

    setVisible(false);
}

void FindReplace::updateLayout(bool replaceMode)
{
    ui->optionsContainer->setVisible(replaceMode);
    ui->replaceContainer->setVisible(replaceMode);
    const uint height = replaceMode
            ? ui->findContainer->minimumHeight() +
              ui->replaceContainer->minimumHeight() +
              ui->optionsContainer->minimumHeight() + 8
            : ui->findContainer->minimumHeight() + 8;
    QPoint position = parentWidget()->rect().topRight();
    position.rx() -= width();
    move(position);
    setFixedHeight(height);
}

void FindReplace::handleMoreButtonChecked(bool v)
{
    updateLayout(v);
}

void FindReplace::show()
{
    QWidget::show();
}

void FindReplace::closeEvent(QCloseEvent *e)
{
    QWidget::closeEvent(e);
    parentWidget()->setFocus();
}

void FindReplace::showFind()
{
    ui->btnMore->setChecked(false);
    updateLayout(false);
    ui->find->clear();
    ui->find->setFocus();
    show();
}

void FindReplace::showReplace()
{
    ui->btnMore->setChecked(true);
    updateLayout(true);
    if (ui->find->text().isEmpty())
        ui->find->setFocus();
    else
        ui->replace->setFocus();
    show();
}

void FindReplace::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
    if (!e->isAccepted()) {
        e->accept();
    }
}

void FindReplace::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
    if (!e->isAccepted()) {
        e->accept();
    }
}


void FindReplace::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    const QBrush background = palette().brush(
                isActiveWindow() && parentWidget()->hasFocus()
                ? QPalette::Active : QPalette::Inactive,
                QPalette::Window
                );
    painter.setBrush(background);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());


    QWidget::paintEvent(event);

    const QBrush borderBrush = parentWidget()->hasFocus()
            ? palette().brush(QPalette::Highlight)
            : palette().brush(QPalette::Dark);
    painter.setPen(QPen(borderBrush, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(0, 0, width(), height()-1);
}

void FindReplace::keyPressEvent(QKeyEvent *e)
{
    QWidget::keyPressEvent(e);
    if (!e->isAccepted())
        e->accept();
}

void FindReplace::keyReleaseEvent(QKeyEvent *e)
{
    QWidget::keyReleaseEvent(e);
    if (!e->isAccepted())
        e->accept();
}

FindReplace::~FindReplace()
{
    delete ui;
}

void FindReplace::handleSearchParameterChanged()
{
    bool currentMatch = false;
    if (cursor_->hasSelection()) {
        const QString currentSearch = cursor_->selectedText();
        const QString text = ui->find->text();
        const bool patternFlag = ui->searchMode->currentIndex() == 1;
        const bool matchCaseFlag = ui->matchCase->isChecked();
        if (patternFlag) {
            QRegExp rx = makeAPatternRegExp(
                        text,
                        matchCaseFlag ? Qt::CaseSensitive : Qt::CaseInsensitive
                                        );
            currentMatch = rx.exactMatch(currentSearch);
        }
        else {
            currentMatch = currentSearch==text;
        }
    }
    if (!currentMatch) {
        doFindFirst(ui->find->text());
    }
}

void FindReplace::handleReturnPressed()
{
    if (sender() == ui->find) {
        if (ui->replaceContainer->isVisible()) {
            ui->replace->setFocus();
        }
        else {
            close();
        }
    }
    else if (sender() == ui->replace) {
        if (cursor_->hasSelection()) {
            doReplace();
        }
        else {
            close();
        }
    }
}

void FindReplace::doFindFirst(const QString &text)
{
    cursor_->removeSelection();
    const bool patternFlag = ui->searchMode->currentIndex() == 1;
    const bool matchCaseFlag = ui->matchCase->isChecked();
    const QPoint start(0, 0);
    const QPoint end(0, document_->linesCount());
    findText(text, start, end, patternFlag, matchCaseFlag, 1);
}

void FindReplace::doFindNext()
{
    const bool patternFlag = ui->searchMode->currentIndex() == 1;
    const bool matchCaseFlag = ui->matchCase->isChecked();
    QPoint start(cursor_->column(), cursor_->row());
    if (cursor_->hasSelection()) {
        int fromRow, fromCol, toRow, toCol;
        cursor_->selectionBounds(fromRow, fromCol, toRow, toCol);
        start.ry() = qMax(fromRow, toRow);
        start.rx() = qMax(fromCol, toCol);
    }
    const QPoint end(0, document_->linesCount());
    findText(ui->find->text(), start, end, patternFlag, matchCaseFlag, 1);
}

void FindReplace::doFindPrevious()
{
    const bool patternFlag = ui->searchMode->currentIndex() == 1;
    const bool matchCaseFlag = ui->matchCase->isChecked();
    const QPoint start(0, 0);
    QPoint end(cursor_->column(), cursor_->row());
    if (cursor_->hasSelection()) {
        int fromRow, fromCol, toRow, toCol;
        cursor_->selectionBounds(fromRow, fromCol, toRow, toCol);
        end.ry() = qMin(fromRow, toRow);
        end.rx() = qMin(fromCol, toCol);
    }
    findText(ui->find->text(), start, end, patternFlag, matchCaseFlag, -1);
}

void FindReplace::findText(
        const QString &text,
        const QPoint &from,
        const QPoint &to,
        bool patternFlag,
        bool matchCaseFlag,
        char direction
        )
{
    const uint fromLine = qMax(0, from.y());
    const uint toLine = qMax(0, to.y());

    const uint fromPos = qMax(0, from.x());
    const uint toPos = qMax(0, to.x());

    if (direction == 1) {
        for (uint line=fromLine; line<toLine; line++) {
            const uint indent = 2 * document_->indentAt(line);
            const uint startPos = line==fromLine
                    ? qMax(0, int(fromPos) - int(indent))
                    : 0;
            const QString & lineText = document_->textAt(line);
            int spos = -1;
            uint ssize = 0;
            if (!patternFlag) {
                spos = lineText.indexOf(
                       text,
                       startPos,
                       matchCaseFlag ? Qt::CaseSensitive : Qt::CaseInsensitive
                                       );
                ssize = text.length();
            }
            else {
                const QRegExp rx = makeAPatternRegExp(text, matchCaseFlag);
                spos = rx.indexIn(lineText, startPos);
                ssize = rx.matchedLength();

            }
            if (spos != -1) {
                uint foundStart = indent + spos;
                uint foundLength = ssize;
                uint foundEnd = foundStart + foundLength;
                markFoundText(line, foundStart, foundEnd, 1);
                return;
            }
        }
    }
    else if (direction == -1) {
        for (uint line=toLine + 1; line-- > fromLine; ) {
            const uint indent = 2 * document_->indentAt(line);
            const QString & lineText = document_->textAt(line);
            const uint endPos = line==toLine
                    ? qMax(0, int(toPos) - int(indent))
                    : qMax(0, lineText.length()-1);
            int spos = -1;
            uint ssize = 0;
            if (!patternFlag) {
                spos = lineText.lastIndexOf(
                       text,
                       int(endPos),
                       matchCaseFlag ? Qt::CaseSensitive : Qt::CaseInsensitive
                                       );
                ssize = text.length();
            }
            else {
                const QRegExp rx = makeAPatternRegExp(text, matchCaseFlag);
                spos = rx.lastIndexIn(lineText, endPos);
                ssize = rx.matchedLength();

            }
            if (spos != -1 && spos < endPos) {
                uint foundStart = indent + spos;
                uint foundLength = ssize;
                uint foundEnd = foundStart + foundLength;
                markFoundText(line, foundStart, foundEnd, -1);
                return;
            }
        }
    }
}

void FindReplace::markFoundText(
        const uint lineNo,
        const uint fromPos,
        const uint toPos,
        const char direction
        )
{
    cursor_->removeSelection();
    if (direction == 1) {
        // select from left to right
        cursor_->moveTo(lineNo, fromPos);
        cursor_->movePosition(
                    QTextCursor::NextCell,
                    TextCursor::MM_Select,
                    toPos - fromPos
                    );
    }
    else if (direction == -1) {
        // select from right to left
        cursor_->moveTo(lineNo, toPos);
        cursor_->movePosition(
                    QTextCursor::PreviousCell,
                    TextCursor::MM_Select,
                    toPos - fromPos
                    );
    }
}

QRegExp FindReplace::makeAPatternRegExp(QString s, const bool matchCaseFlag)
{
    s.replace('\\', "\\\\");
    s.replace('[', "\\[");
    s.replace(']', "\\]");
    s.replace(')', "\\)");
    s.replace('.', "\\.");
    s.replace(' ', "\\s");
    s.replace('+', "\\+");
    s.replace('*', ".*");
    s.replace('?', ".");
    QRegExp result(s, matchCaseFlag? Qt::CaseSensitive : Qt::CaseInsensitive);
    result.setMinimal(true);
    return result;
}

void FindReplace::doReplace()
{
    if (cursor_->hasSelection()) {
        cursor_->insertText(ui->replace->text());
    }
    doFindNext();
}

void FindReplace::doReplaceAll()
{
    doFindFirst(ui->find->text());
    if (!cursor_->hasSelection()) {
        return; // Prevent pushing anything to undo stack
    }
    document_->undoStack()->beginMacro("replaceAll");
    while (cursor_->hasSelection()) {
        doReplace();
    }
    document_->undoStack()->endMacro();
}


} // namespace Editor
