// Self includes
#include "docbookview_impl.h"
#include "contentview.h"
#include "document.h"
#include "docbookfactory.h"
#include "docbookmodel.h"
#include "printdialog.h"
#include "printrenderer.h"
#include "sidepanel.h"

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
    sidePanel_ = new SidePanel(pClass_);
    splitter_->addWidget(sidePanel_);
    content_ = new ContentView(pClass_);
    splitter_->addWidget(content_);
    pClass_->layout()->addWidget(splitter_);

    connect(sidePanel_, SIGNAL(itemPicked(ModelPtr)),
            this, SLOT(showAnItem(ModelPtr)));

    connect(content_, SIGNAL(itemRequest(ModelPtr)),
            this, SLOT(showAnItem(ModelPtr)));

    connect(this, SIGNAL(itemSelected(ModelPtr)),
            sidePanel_, SLOT(selectItem(ModelPtr)));

    createActions();
}


QSize DocBookViewImpl::minimumSizeHint() const
{
    return QSize(800, 500);
}

QSize DocBookViewImpl::sizeHint() const
{
    return QSize(1000, 600);
}

void DocBookViewImpl::updateSettings(QSettings *settings, const QString &prefix)
{
    settings_ = settings;
    settingsPrefix_ = prefix;
}

void DocBookViewImpl::saveState(QSettings *settings, const QString &prefix)
{
    settings->setValue(prefix + "/SplitterGeometry",
                       splitter_->saveGeometry());
    settings->setValue(prefix + "/SplitterState",
                       splitter_->saveState());
    sidePanel_->saveState(settings, prefix + "/SideBar");
}

void DocBookViewImpl::restoreState(QSettings *settings, const QString &prefix)
{
    splitter_->restoreState(settings->value(prefix + "/SplitterState")
                            .toByteArray());
    splitter_->restoreGeometry(settings->value(prefix + "/SplitterGeometry")
                               .toByteArray());
    sidePanel_->restoreState(settings, prefix + "/SideBar");
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
    sidePanel_->addDocument(doc);
    return doc;
}



void DocBookViewImpl::removeDocument(const Document & existingDocument)
{

}

void DocBookViewImpl::showAnItem(ModelPtr model)
{
    if (model) {
        content_->reset();
        content_->renderData(model);
    }
    if (sender() == content_) {
        emit itemSelected(model);
    }
}

void DocBookViewImpl::showPrintDialog()
{

}

}
