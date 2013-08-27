// Self includes
#include "findreplace.h"
#include "ui_findreplace.h"

// Kumir includes
#include "textdocument.h"
#include "textcursor.h"
#include "editor.h"

// Qt includes
#include <QPainter>
#include <QStyle>


namespace Editor {

FindReplace::FindReplace(Editor * editor)
    : QWidget(editor)
    , ui(new Ui::FindReplace)
    , editor_(editor)
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

    static const QString iconsRoot =
            qApp->property("sharePath").toString() + "/editor/";
    ui->btnNext->setIcon(QIcon(iconsRoot+"find-next.png"));
    ui->btnPrev->setIcon(QIcon(iconsRoot+"find-previous.png"));
    ui->btnClose->setIcon(QIcon(iconsRoot+"find-close.png"));
    ui->btnMore->setIcon(QIcon(iconsRoot+"find-options.png"));
    setCursor(Qt::ArrowCursor);
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
    if (editor_->cursor()->hasSelection()) {
        const QString currentSearch = editor_->cursor()->selectedText();
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
        if (editor_->cursor()->hasSelection()) {
            doReplace();
        }
        else {
            close();
        }
    }
}

void FindReplace::doFindFirst(const QString &text)
{
    editor_->cursor()->removeSelection();
    const bool patternFlag = ui->searchMode->currentIndex() == 1;
    const bool matchCaseFlag = ui->matchCase->isChecked();
    const QPoint start(0, 0);
    const QPoint end(0, editor_->document()->linesCount());
    findText(text, start, end, patternFlag, matchCaseFlag, 1);
}

void FindReplace::doFindNext()
{
    const bool patternFlag = ui->searchMode->currentIndex() == 1;
    const bool matchCaseFlag = ui->matchCase->isChecked();
    QPoint start(editor_->cursor()->column(), editor_->cursor()->row());
    if (editor_->cursor()->hasSelection()) {
        int fromRow, fromCol, toRow, toCol;
        editor_->cursor()->selectionBounds(fromRow, fromCol, toRow, toCol);
        start.ry() = qMax(fromRow, toRow);
        start.rx() = qMax(fromCol, toCol);
    }
    const QPoint end(0, editor_->document()->linesCount());
    findText(ui->find->text(), start, end, patternFlag, matchCaseFlag, 1);
}

void FindReplace::doFindPrevious()
{
    const bool patternFlag = ui->searchMode->currentIndex() == 1;
    const bool matchCaseFlag = ui->matchCase->isChecked();
    const QPoint start(0, 0);
    QPoint end(editor_->cursor()->column(), editor_->cursor()->row());
    if (editor_->cursor()->hasSelection()) {
        int fromRow, fromCol, toRow, toCol;
        editor_->cursor()->selectionBounds(fromRow, fromCol, toRow, toCol);
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
            const uint indent = 2 * editor_->document()->indentAt(line);
            const uint startPos = line==fromLine
                    ? qMax(0, int(fromPos) - int(indent))
                    : 0;
            const QString & lineText = editor_->document()->textAt(line);
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
            const uint indent = 2 * editor_->document()->indentAt(line);
            const QString & lineText = editor_->document()->textAt(line);
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
    editor_->cursor()->removeSelection();
    if (direction == 1) {
        // select from left to right
        editor_->cursor()->moveTo(lineNo, fromPos);
        editor_->cursor()->movePosition(
                    QTextCursor::NextCell,
                    TextCursor::MM_Select,
                    toPos - fromPos
                    );
    }
    else if (direction == -1) {
        // select from right to left
        editor_->cursor()->moveTo(lineNo, toPos);
        editor_->cursor()->movePosition(
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
    if (editor_->cursor()->hasSelection()) {
        editor_->cursor()->insertText(ui->replace->text());
    }
    doFindNext();
}

void FindReplace::doReplaceAll()
{
    doFindFirst(ui->find->text());
    if (!editor_->cursor()->hasSelection()) {
        return; // Prevent pushing anything to undo stack
    }
    editor_->document()->undoStack()->beginMacro("replaceAll");
    while (editor_->cursor()->hasSelection()) {
        doReplace();
    }
    editor_->document()->undoStack()->endMacro();
}


} // namespace Editor
