#ifndef COREGUI_DEBUGGERVIEW_H
#define COREGUI_DEBUGGERVIEW_H

#include "interfaces/runinterface.h"

#include <QTreeView>

namespace CoreGUI {

class DebuggerView : public QTreeView
{
    Q_OBJECT
public:
    explicit DebuggerView(Shared::RunInterface * runner, QWidget *parent = 0);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    
signals:
    
public slots:
    void setDebuggerEnabled(bool enabled);

private slots:
    void handleRowsInserted(const QModelIndex & index, int start, int end);
    void handleRowsRemoved(const QModelIndex & index, int start, int end);

private /*methods*/:
    void paintEvent(QPaintEvent *event);

private /*fields*/:
    Shared::RunInterface * runner_;
    bool debuggerEnabled_;
    
};

} // namespace CoreGUI

#endif // COREGUI_DEBUGGERVIEW_H
