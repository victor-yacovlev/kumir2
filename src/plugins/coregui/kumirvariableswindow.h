#ifndef COREGUI_KUMIRVARIABLESWINDOW_H
#define COREGUI_KUMIRVARIABLESWINDOW_H

#include <QtCore>
#include <QtGui>

#include "abstractsyntaxtree/ast.h"

#include "ui_kumirvariableswindow.h"

namespace CoreGUI {

class KumirVariablesWindow
        : public QWidget
        , Ui::KumirVariablesWindow

{
public:
    Q_OBJECT

public:
    explicit KumirVariablesWindow ( QWidget* parent = 0 );
    ~KumirVariablesWindow();
    void init( AST::Data * ast );

public slots:
    void refreshVariables();
    void reset();

protected slots:
    void handleItemDoubleClicked(QTreeWidgetItem * item, int column);
    void handleTableViewerClosed();
    void handleItemHovered(QTreeWidgetItem * item, int column);

protected:
    static QString unserializeArray(const QVariant &data);
    static QString unserialize1D(const QList<QVariant> l, int size, int &index);

    void createSeparateTableViewer(int moduleId, int variableId);

    void showEvent(QShowEvent *event);
    void closeEvent ( QCloseEvent * event );
    void setHeaders();
    bool isFunction(const QString& name);
    QVariant getValue(int mod, int alg, int id) const;
    AST::Data * m_ast;
    QMap<QString, class TableViewer*> l_tables;
    QMap<QString, class QWidget*> l_childWindows;
};

} // namespace CoreGUI

#endif // COREGUI_KUMIRVARIABLESWINDOW_H
