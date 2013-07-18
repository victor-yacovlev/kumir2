#include "debuggerview.h"

namespace CoreGUI {

DebuggerView::DebuggerView(Shared::RunInterface * runner, QWidget *parent)
    : QTreeView(parent)
    , runner_(runner)
    , debuggerEnabled_(false)
{
    setHeaderHidden(true);
}

void DebuggerView::setDebuggerEnabled(bool enabled)
{
    debuggerEnabled_ = enabled;
    if (enabled && runner_ && runner_->debuggerVariablesViewModel()) {
        QAbstractItemModel * newModel = runner_->debuggerVariablesViewModel();
        if (model() != newModel) {
            setModel(runner_->debuggerVariablesViewModel());
            connect(model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                    this, SLOT(handleRowsInserted(QModelIndex,int,int)));
            for (int column = 0; column < model()->columnCount(); ++column)
                resizeColumnToContents(column);
            handleRowsInserted(QModelIndex(),
                               model()->rowCount() - 1,
                               model()->rowCount() - 1);
        }
    }
    else {
        if (model()) {
            disconnect(model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                       this, SLOT(handleRowsInserted(QModelIndex,int,int)));
        }
        setModel(0);
    }
}

void DebuggerView::paintEvent(QPaintEvent *event)
{
    if (debuggerEnabled_ && model()) {
        QTreeView::paintEvent(event);
    }
    else {
        static const QString message =
                tr("Current values available only while running program in step-by-step mode");
        event->accept();
        QPainter p(viewport());
        QStyleOptionFrame opt;
        opt.initFrom(viewport());
        opt.rect = viewport()->rect();
        opt.palette.setCurrentColorGroup(QPalette::Disabled);
        style()->drawPrimitive(QStyle::PE_Frame, &opt, &p, viewport());
        QTextOption textOpt;
        textOpt.setAlignment(Qt::AlignCenter);
        textOpt.setWrapMode(QTextOption::WordWrap);
        p.setPen(QPen(opt.palette.buttonText().color()));
        p.drawText(viewport()->rect().adjusted(5,5,-10,-10),
                   message,
                   textOpt
                   );


    }
}

void DebuggerView::handleRowsInserted(const QModelIndex &index, int start, int end)
{
    if (!index.isValid() && start == end) { // top level context
        const QModelIndex elementIndex = model()->index(start, 0, index);
        if (elementIndex.isValid()) {
            setExpanded(elementIndex, true);
        }
    }
}

void DebuggerView::handleRowsRemoved(const QModelIndex &index, int start, int end)
{

}

} // namespace CoreGUI
