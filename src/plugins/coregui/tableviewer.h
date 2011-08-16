#ifndef COREGUI_TABLEVIEWER_H
#define COREGUI_TABLEVIEWER_H

#include <QtCore>
#include <QtGui>

#include "abstractsyntaxtree/ast.h"

namespace CoreGUI {

class TableViewer
    : public QTabWidget
{
    Q_OBJECT
    friend class TableViewerPrivate;
public:
    explicit TableViewer(AST::Data * ast, int modId, int algId, int varId, QWidget *parent = 0);
    int moduleId() const;
    int variableId() const;
    ~ TableViewer();
public slots:
    void updateContent();
private:
    struct TableViewerPrivate * d;
};

} // namespace CoreGUI

#endif // COREGUI_TABLEVIEWER_H
