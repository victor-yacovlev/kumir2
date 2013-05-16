#include "printdialog.h"
#include "ui_printdialog.h"

#include "docbookmodel.h"

#include <QTreeWidgetItem>

namespace DocBookViewer {

PrintDialog::PrintDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintDialog)
{
    ui->setupUi(this);
}

PrintDialog::~PrintDialog()
{
    delete ui;
}

void PrintDialog::addDocument(const Document &doc)
{
    QTreeWidgetItem * item = new QTreeWidgetItem(ui->itemsChooser);
    ui->itemsChooser->addTopLevelItem(item);
    item->setText(0, doc.root_->title());
    item->setToolTip(0, doc.root_->subtitle());
    item->setCheckState(0, Qt::Unchecked);
    createNavigationItems(item, doc.root_);
    modelsOfItems_[item] = doc.root_;
}

void PrintDialog::createNavigationItems(QTreeWidgetItem *item, ModelPtr model)
{
    foreach (ModelPtr child, model->children()) {
        if (child->isSectioningNode()) {
            QTreeWidgetItem * childItem = new QTreeWidgetItem(item);
            childItem->setCheckState(0, Qt::Unchecked);
            childItem->setText(0, child->title());
            childItem->setToolTip(0, child->subtitle());
            modelsOfItems_[childItem] = child;
            createNavigationItems(childItem, child);
            item->addChild(childItem);
            item->setExpanded(true);
        }
    }
}

QList<ModelPtr> PrintDialog::selectedModels() const
{
    QList<ModelPtr> result;
    for (int i=0; i<ui->itemsChooser->topLevelItemCount(); i++) {
        result += selectedModels(ui->itemsChooser->topLevelItem(i));
    }
    return result;
}

QList<ModelPtr> PrintDialog::selectedModels(QTreeWidgetItem * root) const
{
    QList<ModelPtr> result;
    if (root->checkState(0) == Qt::Checked) {
        result.append(modelsOfItems_[root]);
    }
    else if (root->checkState(0) == Qt::PartiallyChecked) {
        for (int i=0; i<root->childCount(); i++) {
            result += selectedModels(root->child(i));
        }
    }
    return result;
}

} // namespace DocBookViewer
