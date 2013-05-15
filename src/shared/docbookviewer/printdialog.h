#ifndef DOCBOOKVIEWER_PRINTDIALOG_H
#define DOCBOOKVIEWER_PRINTDIALOG_H

#include "document.h"

#include <QDialog>
#include <QTreeWidgetItem>

namespace DocBookViewer {

namespace Ui {
class PrintDialog;
}

class PrintDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PrintDialog(QWidget *parent = 0);
    void addDocument(const Document & document);
    ~PrintDialog();
    QList<const DocBookModel*> selectedModels() const;
    
private:
    QList<const DocBookModel*> selectedModels(
            QTreeWidgetItem * root) const;
    QMap<QTreeWidgetItem*, const DocBookModel*> modelsOfItems_;
    void createNavigationItems(QTreeWidgetItem * item,
                               const DocBookModel * model);
    Ui::PrintDialog *ui;
};


} // namespace DocBookViewer
#endif // DOCBOOKVIEWER_PRINTDIALOG_H
