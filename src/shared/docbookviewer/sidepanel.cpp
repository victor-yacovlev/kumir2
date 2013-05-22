#include "docbookmodel.h"
#include "docbookfactory.h"

#include "sidepanel.h"
#include "ui_sidepanel.h"

namespace DocBookViewer {

SidePanel::SidePanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SidePanel)
{
    ui->setupUi(this);
    static const QList<QPushButton*> buttons = QList<QPushButton*>()
            << ui->contents << ui->index << ui->examples << ui->tables;

    static const QList<QTreeWidget*> treeWidgets = QList<QTreeWidget*>()
            << ui->contentsNavigator << ui->examplesNavigator
               << ui->tablesNavigator;

    foreach (QPushButton* button, buttons) {
        connect(button, SIGNAL(clicked()), this, SLOT(hadleButtonPressed()));
    }

    foreach (QTreeWidget* widget, treeWidgets) {
        connect(widget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                this, SLOT(selectTreeWidgetItem(QTreeWidgetItem*)));
    }

}

void SidePanel::addDocument(Document document)
{
    QList<ModelPtr> topLevelItems;
    if (document.root_) {
        if (document.root_->modelType() == DocBookModel::Set) {
            topLevelItems = document.root_->children();
        }
        else {
            topLevelItems.append(document.root_);
        }
        loadedDocuments_.append(document);
    }
    for (int i=0; i<topLevelItems.size(); i++) {
        ModelPtr model = topLevelItems[i];
        QTreeWidgetItem * item =
                new QTreeWidgetItem(ui->contentsNavigator);
        ui->contentsNavigator->addTopLevelItem(item);
        item->setText(0, model->title());
        item->setToolTip(0, model->subtitle());
        createNavigationItems(item, model);
        createListOfExamples(model);
        createListOfTables(model);
        modelsOfItems_[item] = model;
        itemsOfModels_[model] = item;
    }
}

void SidePanel::hadleButtonPressed()
{
    QObject * who = sender();
    static const QList<QPushButton*> buttons = QList<QPushButton*>()
            << ui->contents << ui->index << ui->examples << ui->tables;

    for (int index = 0; index < buttons.size() ; index ++) {
        if (who == buttons[index]) {
            ui->stackedWidget->setCurrentIndex(index);
        }
        else {
            buttons[index]->setChecked(false);
        }
    }
}

void SidePanel::saveState(QSettings *settings, const QString &prefix)
{
    QString shown;
    if (ui->contents->isChecked())
        shown = "Contents";
    else if (ui->index->isChecked())
        shown = "Index";
    else if (ui->examples->isChecked())
        shown = "Examples";
    else if (ui->tables->isChecked())
        shown = "Tables";
    settings->setValue(prefix + "/ShowMode", shown);
}

void SidePanel::restoreState(QSettings *settings, const QString &prefix)
{
    QString shown = settings->value(prefix + "/ShowMode").toString().toLower();
    if (shown == "contents") {
        ui->contents->setChecked(true);
        ui->index->setChecked(false);
        ui->examples->setChecked(false);
        ui->tables->setChecked(false);
        ui->stackedWidget->setCurrentIndex(0);
    }
    else if (shown == "index") {
        ui->contents->setChecked(false);
        ui->index->setChecked(true);
        ui->examples->setChecked(false);
        ui->tables->setChecked(false);
        ui->stackedWidget->setCurrentIndex(1);
    }
    else if (shown == "examples") {
        ui->contents->setChecked(false);
        ui->index->setChecked(false);
        ui->examples->setChecked(true);
        ui->tables->setChecked(false);
        ui->stackedWidget->setCurrentIndex(2);
    }
    else if (shown == "tables") {
        ui->contents->setChecked(false);
        ui->index->setChecked(false);
        ui->examples->setChecked(false);
        ui->tables->setChecked(true);
        ui->stackedWidget->setCurrentIndex(3);
    }
}

void SidePanel::selectItem(ModelPtr itemModel)
{
    const QList<ModelPtr> keys = itemsOfModels_.keys();
    for (int i=0; i<keys.size(); i++) {
        ModelPtr model = keys[i];
        if (model == itemModel) {
            QList<QTreeWidgetItem*> selection =
                    ui->contentsNavigator->selectedItems() +
                    ui->examplesNavigator->selectedItems() +
                    ui->tablesNavigator->selectedItems();
            foreach (QTreeWidgetItem* item, selection) {
                item->setSelected(false);
            }
            QTreeWidgetItem * item = itemsOfModels_[model];
            item->setSelected(true);
            item->treeWidget()->scrollToItem(item);
            if (item->treeWidget() == ui->contentsNavigator) {
                ui->contents->setChecked(true);
                ui->index->setChecked(false);
                ui->examples->setChecked(false);
                ui->tables->setChecked(false);
                ui->stackedWidget->setCurrentIndex(0);
            }
            else if (item->treeWidget() == ui->examplesNavigator) {
                ui->contents->setChecked(false);
                ui->index->setChecked(false);
                ui->examples->setChecked(true);
                ui->tables->setChecked(false);
                ui->stackedWidget->setCurrentIndex(2);
            }
            else if (item->treeWidget() == ui->tablesNavigator) {
                ui->contents->setChecked(false);
                ui->index->setChecked(false);
                ui->examples->setChecked(false);
                ui->tables->setChecked(true);
                ui->stackedWidget->setCurrentIndex(3);
            }
        }
    }
}

void SidePanel::createNavigationItems(QTreeWidgetItem *item,
                                            ModelPtr model)
{
    for (int i=0; i<model->children().size(); i++) {
        ModelPtr child = model->children()[i];
        if (child->isSectioningNode()) {
            QTreeWidgetItem * childItem = new QTreeWidgetItem(item);
            childItem->setText(0, child->title());
            childItem->setToolTip(0, child->subtitle());
            modelsOfItems_[childItem] = child;
            itemsOfModels_[child] = childItem;
            createNavigationItems(childItem, child);
            item->addChild(childItem);
            item->setExpanded(true);
        }
    }
}

void SidePanel::createListOfExamples(ModelPtr root)
{
    ModelPtr listOfExamples = DocBookFactory::createListOfEntries(
                root,
                DocBookModel::ListOfExamples,
                DocBookModel::Example
                );
    if (listOfExamples) {
        QTreeWidgetItem * topLevelItem =
                new QTreeWidgetItem(ui->examplesNavigator);
        topLevelItem->setText(0, listOfExamples->title());
        topLevelItem->setToolTip(0, tr("List of examples in \"%1\"")
                                 .arg(listOfExamples->title()));
        ui->examplesNavigator->addTopLevelItem(topLevelItem);
        itemsOfModels_[listOfExamples] = topLevelItem;
        modelsOfItems_[topLevelItem] = listOfExamples;
        foreach (ModelPtr example, listOfExamples->children()) {
            QTreeWidgetItem * item = new QTreeWidgetItem(topLevelItem);
            item->setText(0, example->title());
            item->setToolTip(0, example->subtitle());
            modelsOfItems_[item] = example;
            itemsOfModels_[example] = item;
            topLevelItem->addChild(item);
            topLevelItem->setExpanded(true);
        }
    }
}

void SidePanel::createListOfTables(ModelPtr root)
{
    ModelPtr listOfTables = DocBookFactory::createListOfEntries(
                root,
                DocBookModel::ListOfTables,
                DocBookModel::Table
                );
    if (listOfTables) {
        QTreeWidgetItem * topLevelItem =
                new QTreeWidgetItem(ui->tablesNavigator);
        topLevelItem->setText(0, listOfTables->title());
        topLevelItem->setToolTip(0, tr("List of tables in \"%1\"")
                                 .arg(listOfTables->title()));
        ui->tablesNavigator->addTopLevelItem(topLevelItem);
        itemsOfModels_[listOfTables] = topLevelItem;
        modelsOfItems_[topLevelItem] = listOfTables;
        foreach (ModelPtr table, listOfTables->children()) {
            QTreeWidgetItem * item = new QTreeWidgetItem(topLevelItem);
            item->setText(0, table->title());
            item->setToolTip(0, table->subtitle());
            modelsOfItems_[item] = table;
            itemsOfModels_[table] = item;
            topLevelItem->addChild(item);
            topLevelItem->setExpanded(true);
        }
    }
}



void SidePanel::selectTreeWidgetItem(QTreeWidgetItem *item)
{
    if (!item || !modelsOfItems_.contains(item)) {
        return;
    }
    ModelPtr model = modelsOfItems_[item];
    emit itemPicked(model);
}

SidePanel::~SidePanel()
{
    delete ui;
}

} // namespace DocBookViewer
