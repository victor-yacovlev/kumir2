#include "docbookmodel.h"
#include "docbookfactory.h"

#include "sidepanel.h"
#include "ui_sidepanel.h"

#include <QtGlobal>
#include <QDebug>

namespace DocBookViewer {

SidePanel::SidePanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SidePanel)
{
    ui->setupUi(this);
    static const QList<QPushButton*> buttons = QList<QPushButton*>()
            << ui->contents << ui->algorithms << ui->examples << ui->tables;

    static const QList<QTreeWidget*> treeWidgets = QList<QTreeWidget*>()
            << ui->contentsNavigator << ui->algorithmsNavigator
            << ui->examplesNavigator << ui->tablesNavigator;

    static const QList<QLineEdit*> lineEdits = QList<QLineEdit*>()
            << ui->searchContents << ui->searchAlgorithms
            << ui->searchExamples << ui->searchTables;

    foreach (QPushButton* button, buttons) {
        connect(button, SIGNAL(clicked()), this, SLOT(hadleButtonPressed()));
    }

    foreach (QTreeWidget* widget, treeWidgets) {
        connect(widget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                this, SLOT(selectTreeWidgetItem(QTreeWidgetItem*)));
    }

    foreach (QLineEdit* lineEdit, lineEdits) {
        connect(lineEdit, SIGNAL(textChanged(QString)),
                this, SLOT(doFilter(QString)));
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
        createListOfAlgorithms(model);
        modelsOfItems_[item] = model;
        itemsOfModels_[model] = item;
    }
}

void SidePanel::hadleButtonPressed()
{
    QObject * who = sender();
    static const QList<QPushButton*> buttons = QList<QPushButton*>()
            << ui->contents << ui->algorithms << ui->examples << ui->tables;

    for (int index = 0; index < buttons.size() ; index ++) {
        if (who == buttons[index]) {
            ui->stackedWidget->setCurrentIndex(index);
        }
        else {
            buttons[index]->setChecked(false);
        }
    }
}

void SidePanel::saveState(ExtensionSystem::SettingsPtr settings, const QString &prefix)
{
    QString shown;
    if (ui->contents->isChecked())
        shown = "Contents";
    else if (ui->algorithms->isChecked())
        shown = "Algorithms";
    else if (ui->examples->isChecked())
        shown = "Examples";
    else if (ui->tables->isChecked())
        shown = "Tables";
    settings->setValue(prefix + "/ShowMode", shown);
}

void SidePanel::restoreState(ExtensionSystem::SettingsPtr settings, const QString &prefix)
{
    QString shown = settings->value(prefix + "/ShowMode").toString().toLower();
    if (shown == "contents") {
        ui->contents->setChecked(true);
        ui->algorithms->setChecked(false);
        ui->examples->setChecked(false);
        ui->tables->setChecked(false);
        ui->stackedWidget->setCurrentIndex(0);
    }
    else if (shown == "algorithms") {
        ui->contents->setChecked(false);
        ui->algorithms->setChecked(true);
        ui->examples->setChecked(false);
        ui->tables->setChecked(false);
        ui->stackedWidget->setCurrentIndex(1);
    }
    else if (shown == "examples") {
        ui->contents->setChecked(false);
        ui->algorithms->setChecked(false);
        ui->examples->setChecked(true);
        ui->tables->setChecked(false);
        ui->stackedWidget->setCurrentIndex(2);
    }
    else if (shown == "tables") {
        ui->contents->setChecked(false);
        ui->algorithms->setChecked(false);
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
                ui->algorithms->setChecked(false);
                ui->examples->setChecked(false);
                ui->tables->setChecked(false);
                ui->stackedWidget->setCurrentIndex(0);
            }
            else if (item->treeWidget() == ui->examplesNavigator) {
                ui->contents->setChecked(false);
                ui->algorithms->setChecked(false);
                ui->examples->setChecked(true);
                ui->tables->setChecked(false);
                ui->stackedWidget->setCurrentIndex(2);
            }
            else if (item->treeWidget() == ui->tablesNavigator) {
                ui->contents->setChecked(false);
                ui->algorithms->setChecked(false);
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

void SidePanel::createListOfAlgorithms(ModelPtr root)
{
    QMap<QString, ModelPtr> modules;
    static const QString stdlibName = tr("Standard Library functions");
    for (int i=0; i<ui->algorithmsNavigator->topLevelItemCount(); i++) {
        QString key;
        QTreeWidgetItem * topLevelItem = ui->algorithmsNavigator->topLevelItem(i);
        if (topLevelItem->text(0) != stdlibName)
            key = topLevelItem->text(0);
        ModelPtr topLevelModel = modelsOfItems_[topLevelItem];
        modules[key] = topLevelModel;
    }

    DocBookFactory::updateListOfAlgorithms(root, modules);

    foreach (const QString & key, modules.keys()) {
        ModelPtr module = modules[key];
        QTreeWidgetItem * moduleItem = nullptr;
        if (itemsOfModels_.contains(module)) {
            moduleItem = itemsOfModels_[module];
        }
        else {
            moduleItem = new QTreeWidgetItem(ui->algorithmsNavigator);
            moduleItem->setText(0, key.isEmpty() ? stdlibName : key);
            moduleItem->setExpanded(true);
            ui->algorithmsNavigator->addTopLevelItem(moduleItem);
            itemsOfModels_[module] = moduleItem;
            modelsOfItems_[moduleItem] = module;
        }

        foreach (ModelPtr algorithm, module->children()) {
            if (!itemsOfModels_.contains(algorithm)) {
                QTreeWidgetItem * algItem = new QTreeWidgetItem(moduleItem);
                moduleItem->addChild(algItem);
                itemsOfModels_[algorithm] = algItem;
                modelsOfItems_[algItem] = algorithm;
                algItem->setText(0, algorithm->title());
            }
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


void SidePanel::doFilter(const QString &text)
{
    static const QList<QTreeWidget*> treeWidgets = QList<QTreeWidget*>()
            << ui->contentsNavigator << ui->algorithmsNavigator
            << ui->examplesNavigator << ui->tablesNavigator;

    static const QList<QLineEdit*> lineEdits = QList<QLineEdit*>()
            << ui->searchContents << ui->searchAlgorithms
            << ui->searchExamples << ui->searchTables;

    QLineEdit * who = qobject_cast<QLineEdit*>(sender());
    int index = lineEdits.indexOf(who);

    QTreeWidget * tree = treeWidgets.at(index);

    QSet<QTreeWidgetItem*> allItems = findFilteredItems("", tree, nullptr);

    QSet<QTreeWidgetItem*> matchedItems = findFilteredItems(text.simplified(), tree, nullptr);

    QSet<QTreeWidgetItem*> unmatchedItems = allItems - matchedItems;

    foreach (QTreeWidgetItem* item, unmatchedItems) {
        item->setHidden(true);
    }

    foreach (QTreeWidgetItem* item, matchedItems) {
        item->setHidden(false);
        QTreeWidgetItem* p = item->parent();
        while (p) {
            p->setHidden(false);
            p = p->parent();
        }
    }

}

QSet<QTreeWidgetItem*>
SidePanel::findFilteredItems(const QString &text,
                             QTreeWidget *tree,
                             QTreeWidgetItem * root)
{
    Q_ASSERT(tree || root);
    QSet<QTreeWidgetItem*> result;
    QList<QTreeWidgetItem*> topLevelChilds;
    if (tree) {
        for (int i=0; i<tree->topLevelItemCount(); i++) {
            topLevelChilds << tree->topLevelItem(i);
        }
    }
    else {
        for (int i=0; i<root->childCount(); i++) {
            topLevelChilds << root->child(i);
        }
    }
    foreach (QTreeWidgetItem* item, topLevelChilds) {
        if (item->text(0).contains(text, Qt::CaseInsensitive)) {
            result.insert(item);
        }
        result += findFilteredItems(text, nullptr, item);
    }
    return result;
}

SidePanel::~SidePanel()
{
    delete ui;
}

} // namespace DocBookViewer
