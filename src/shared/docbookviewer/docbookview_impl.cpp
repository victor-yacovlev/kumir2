// Self includes
#include "docbookview_impl.h"
#include "contentview.h"
#include "document.h"
#include "docbookfactory.h"
#include "docbookmodel.h"
#include "printdialog.h"
#include "printrenderer.h"

// Qt includes
#include <QtCore>
#include <QtGui>

namespace DocBookViewer {

DocBookViewImpl::DocBookViewImpl(DocBookView *pClass)
    : QObject(pClass)
    , pClass_(pClass)
    , settings_(nullptr)
{
    pClass_->setLayout(new QHBoxLayout);
    splitter_ = new QSplitter(Qt::Horizontal, pClass_);
    navigator_ = new QTreeWidget(pClass_);
    navigator_->setHeaderHidden(true);
    navigator_->setFixedWidth(250);
    splitter_->addWidget(navigator_);
    content_ = new ContentView(pClass_);
    splitter_->addWidget(content_);
    pClass_->layout()->addWidget(splitter_);
    connect(navigator_, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(selectAnItem(QTreeWidgetItem*)));
    connect(content_, SIGNAL(requestModelLoad(quintptr)),
            this, SLOT(loadAModelByPtr(quintptr)));
    createActions();
}


QSize DocBookViewImpl::minimumSizeHint() const
{
    return QSize(800, 300);
}

QSize DocBookViewImpl::sizeHint() const
{
    return QSize(800, 600);
}


void DocBookViewImpl::updateSettings(QSettings *settings, const QString &prefix)
{
    settings_ = settings;
    settingsPrefix_ = prefix;
}

void DocBookViewImpl::createActions()
{
    actionToggleNavigationBar_ = new QAction(tr("Toggle sidebar visible"), this);
    actionToggleNavigationBar_->setCheckable(true);

    actionShowPrintDialog_ = new QAction(tr("Print..."), this);
    connect(actionShowPrintDialog_, SIGNAL(triggered()),
            this, SLOT(showPrintDialog()));
}

QAction * DocBookViewImpl::viewerAction(const DocBookView::DocBookViewAction type) const
{
    if (type==DocBookView::ToggleNavigationPane) {
        return actionToggleNavigationBar_;
    }
    else if (type==DocBookView::ShowPrintDialog) {
        return actionShowPrintDialog_;
    }
    return 0;
}

Document DocBookViewImpl::addDocument(const QUrl &url, QString *error, int index)
{
    DocBookFactory * factory = DocBookFactory::self();
    Document doc = factory->parseDocument(url, error);
    QList<ModelPtr> topLevelItems;
    if (doc.root_) {
        if (doc.root_->modelType() == DocBookModel::Set) {
            topLevelItems = doc.root_->children();
        }
        else {
            topLevelItems.append(doc.root_);
        }
        loadedDocuments_.append(doc);
    }
    for (int i=0; i<topLevelItems.size(); i++) {
        ModelPtr model = topLevelItems[i];
        QTreeWidgetItem * item = new QTreeWidgetItem(navigator_);
        navigator_->addTopLevelItem(item);
        item->setText(0, model->title());
        item->setToolTip(0, model->subtitle());
        createNavigationItems(item, model);
        modelsOfItems_[item] = model;
        itemsOfModels_[model] = item;
    }

    return doc;
}

void DocBookViewImpl::createNavigationItems(QTreeWidgetItem *item,
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

void DocBookViewImpl::removeDocument(const Document & existingDocument)
{

}

void DocBookViewImpl::selectAnItem(QTreeWidgetItem *item)
{
    if (!item || !modelsOfItems_.contains(item)) {
        return;
    }
    ModelPtr model = modelsOfItems_[item];
    content_->reset();
    content_->addData(model);

}

void DocBookViewImpl::loadAModelByPtr(quintptr dataPtr)
{
    if (dataPtr) {
        const DocBookModel * modelWeakPointer =
                reinterpret_cast<const DocBookModel*>(dataPtr);
        const QList<ModelPtr> keys = itemsOfModels_.keys();
        for (int i=0; i<keys.size(); i++) {
            ModelPtr model = keys[i];
            if (model.data() == modelWeakPointer) {
                QList<QTreeWidgetItem*> selection = navigator_->selectedItems();
                foreach (QTreeWidgetItem* item, selection) {
                    item->setSelected(false);
                }
                QTreeWidgetItem * item = itemsOfModels_[model];
                item->setSelected(true);
                navigator_->scrollToItem(item);
                content_->reset();
                content_->addData(model);
            }
        }
    }
}

void DocBookViewImpl::showPrintDialog()
{
    PrintDialog chooseItemsDialog(pClass_);
    foreach (const Document & doc, loadedDocuments_) {
        chooseItemsDialog.addDocument(doc);
    }
    if (chooseItemsDialog.exec()) {
        QPrintDialog actualPrintDialog(pClass_);
        if (actualPrintDialog.exec()) {
            PrintRenderer * renderer = PrintRenderer::self();
            renderer->reset();
            QList<ModelPtr> selectedModels = chooseItemsDialog.selectedModels();
            foreach (ModelPtr model, selectedModels)
            {
                renderer->addData(model);
            }
            renderer->render(actualPrintDialog.printer());
        }
    }
}

}
