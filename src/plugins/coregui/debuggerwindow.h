#ifndef COREGUI_DEBUGGERWINDOW_H
#define COREGUI_DEBUGGERWINDOW_H

#include <QtCore>
#include <QtGui>

#include "interfaces/runinterface.h"

namespace CoreGUI {

class DebuggerWindow : public QTreeWidget
{
    Q_OBJECT
public:
    explicit DebuggerWindow(Shared::RunInterface * runner, QWidget *parent = 0);
    
signals:
    
public slots:
    /** Clear debugger window */
    void reset();

    /** Set globals frame */
    void setGlobals(
            /** module name */           const QString & moduleName,
            /** variable names */         const QStringList & names,
            /** variable base types */const QStringList & baseTypes,
            /** variable dimensions */ const QList<int> & dimensions
            );

    /** Push a new stack frame */
    void pushContext(
            /** context header */       const QString & contextName,
            /** variable names */         const QStringList & names,
            /** variable base types */const QStringList & baseTypes,
            /** variable dimensions */ const QList<int> & dimensions
            );

    /** Pop a stack frame */
    void popContext();

    /** Force update values */
    void updateAllValues();

    /** Update scalar variable value */
    void updateLocalVariable(
            /** variable name */       const QString & name,
            /** value */              const QString & value
            );
    void updateGlobalVariable(
            /** module qualified name */const QString & moduleName,
            /** variable name */       const QString & name,
            /** value */              const QString & value
            );

    /** Set table variable bounds */
    void updateLocalTableBounds(
            /** variable name */             const QString & name,
            /** a list of bound pairs */ const QList<int> & bounds
            );

    void updateGlobalTableBounds(
            /** module name */         const QString & moduleName,
            /** variable name */             const QString & name,
            /** a list of bound pairs */ const QList<int> & bounds
            );

    void setLocalReference(
            /** variable name */             const QString & name,
            /** target name */         const QString & targetName,
            /** target array indeces */const QList<int> & indeces,
            /** stack frames back, or -1 for global */   int back,
            /** global value module name */ const QString & moduleName
            );

    /** Marks a table to redraw shown values */
    void updateLocalTableValue(
            /** variable name */            const QString & name,
            /** indeces */             const QList<int> & indeces
            );
    void updateGlobalTableValue(
            /** module name */        const QString & moduleName,
            /** variable name */            const QString & name,
            /** indeces */             const QList<int> & indeces
            );

private slots:
    void handleItemExpanded(QTreeWidgetItem * item);
    void handleItemCollapsed(QTreeWidgetItem * item);
private:
    void paintEvent(QPaintEvent *event);
    void findExpandedTableItem(
            const QList<int> & indeces,
            QTreeWidgetItem * & item
            );
    static QString boundsToText(const QList<int> & bounds);
    static int optimalGroupSize(int size);
    static void valuesListToString(
            const QVariant & vals,
            int maxItems,
            int & counter,
            QString & result
            );
    void setupTableValue(QTreeWidgetItem * item);
    QMap<QString,QTreeWidgetItem *> globals;
    Shared::RunInterface * runner;

};

} // namespace CoreGUI

#endif // COREGUI_DEBUGGERWINDOW_H
