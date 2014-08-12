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
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

namespace DocBookViewer {

DocBookViewImpl::DocBookViewImpl(DocBookView *pClass)
    : QObject(pClass)
    , pClass_(pClass)
    , settings_(nullptr)
    , compactModeFlag_(false)
{

    smallSizeContainer_ = new QWidget(pClass);
    smallSizeContainer_->move(0, 0);
    smallSizeContainer_->setLayout(new QHBoxLayout);
    smallSizeContainer_->layout()->setContentsMargins(0, 24, 0, 0);

    background_ = new QWidget(pClass);
    background_->move(0, 0);

    splitterRightWidget_  = new QStackedWidget(pClass);

    filler_ = new QWidget(pClass_);
    splitterRightWidget_->addWidget(filler_);

    enoughtSizeContainer_ = new QWidget(pClass);
    enoughtSizeContainer_->setLayout(new QHBoxLayout);
    enoughtSizeContainer_->layout()->setContentsMargins(0, 0, 0, 0);
    splitterRightWidget_->addWidget(enoughtSizeContainer_);


    splitter_ = new QSplitter(Qt::Horizontal, pClass_);
    sidePanel_ = new SidePanel(pClass_);
    splitter_->addWidget(sidePanel_);
    content_ = new ContentView(pClass_);
    splitter_->addWidget(splitterRightWidget_);

    connect(sidePanel_, SIGNAL(itemPicked(ModelPtr)),
            this, SLOT(showAnItem(ModelPtr)));

    connect(content_, SIGNAL(itemRequest(ModelPtr)),
            this, SLOT(showAnItem(ModelPtr)));

    connect(this, SIGNAL(itemSelected(ModelPtr)),
            sidePanel_, SLOT(selectItem(ModelPtr)));

    splitter_->setCollapsible(0, false);
    splitter_->setCollapsible(1, false);
    splitter_->move(0, 0);


    toggleSideBar_ = new QLabel(pClass_);
    toggleSideBar_->setFixedHeight(24);
    toggleSideBar_->move(2, 0);
    toggleSideBar_->setTextFormat(Qt::RichText);
    connect(toggleSideBar_, SIGNAL(linkActivated(QString)),
            this, SLOT(handleShowSideBarButton(QString)));

    createActions();

    background_->setStyleSheet("background-color: rgba(0,0,0,192);");
    sidePanel_->setAutoFillBackground(true);
    splitter_->handle(1)->setAutoFillBackground(true);
}


void DocBookViewImpl::updateToggleSideBarButton()
{
    static const QString Hide = tr("Hide side bar");
    static const QString Show = tr("Show side bar");

    const QString & text = isSideBarVisible() ? Hide : Show;
    const QString action = isSideBarVisible() ? "#hide" : "#show";

    const QString html = QString("<a href=\"%1\">%2</a>").arg(action).arg(text);

    const int width = toggleSideBar_->fontMetrics().width(text) + 16;

    toggleSideBar_->setText(html);
    toggleSideBar_->setFixedWidth(width);
}

bool DocBookViewImpl::isSideBarVisible() const
{
    return compactModeFlag_ ? splitter_->isVisible() : sidePanel_->isVisible();
}

void DocBookViewImpl::handleShowSideBarButton(const QString &action)
{
    if (action=="#show") {
        showSidePanel();
    }
    else if (action=="#hide") {
        hideSidePanel();
    }
}

void DocBookViewImpl::setSize(const QSize &size)
{
    int minWidth = content_->minimumSizeHint().width() +
            sidePanel_->minimumSizeHint().width();
    splitter_->move(0, 0);
    splitter_->resize(size);
    smallSizeContainer_->move(0, 0);
    smallSizeContainer_->resize(size);
    background_->move(0, 0);
    background_->resize(size);
    if (size.width() < minWidth) {
        switchToCompactMode();
    }
    else {
        switchToEnoughtSizeMode();
    }
    updateToggleSideBarButton();
}

void DocBookViewImpl::switchToCompactMode(bool force)
{
    if (compactModeFlag_ && !force)
        return;
    compactModeFlag_ = true;
    splitterRightWidget_->setCurrentIndex(0);
    smallSizeContainer_->layout()->addWidget(content_);
    splitter_->setVisible(false);
    updateToggleSideBarButton();
    background_->setVisible(isSideBarVisible());
}

void DocBookViewImpl::switchToEnoughtSizeMode(bool force)
{
    if (!compactModeFlag_ && !force)
        return;
    compactModeFlag_ = false;
    splitterRightWidget_->setCurrentIndex(1);
    enoughtSizeContainer_->layout()->addWidget(content_);
    splitter_->setVisible(true);
    updateToggleSideBarButton();
    background_->setVisible(false);
}

void DocBookViewImpl::hideSidePanel()
{
    if (compactModeFlag_)
        splitter_->setVisible(false);
    else
        sidePanel_->setVisible(false);
    updateToggleSideBarButton();
    background_->setVisible(false);
    enoughtSizeContainer_->layout()->setContentsMargins(0, 24, 0, 0);
}

void DocBookViewImpl::showSidePanel()
{
    splitter_->setVisible(true);
    sidePanel_->setVisible(true);
    updateToggleSideBarButton();
    background_->setVisible(compactModeFlag_);
    enoughtSizeContainer_->layout()->setContentsMargins(0, 0, 0, 0);
}

QSize DocBookViewImpl::minimumSizeHint() const
{
    int minW = qMax(sidePanel_->minimumSizeHint().width(),
                    content_->minimumSizeHint().width());
    int minH = qMax(sidePanel_->minimumSizeHint().height(),
                    content_->minimumSizeHint().height());
    return QSize(minW, minH);
}

QSize DocBookViewImpl::sizeHint() const
{
    return QSize(800, 600);
}

void DocBookViewImpl::updateSettings(ExtensionSystem::SettingsPtr settings, const QString &prefix)
{
    settings_ = settings;
    settingsPrefix_ = prefix;
}

void DocBookViewImpl::saveState(ExtensionSystem::SettingsPtr settings, const QString &prefix)
{
    settings->setValue(prefix + "/SplitterGeometry",
                       splitter_->saveGeometry());
    settings->setValue(prefix + "/SplitterState",
                       splitter_->saveState());
    sidePanel_->saveState(settings, prefix + "/SideBar");
    settings->setValue(prefix + "/CompactMode", compactModeFlag_);
}

void DocBookViewImpl::restoreState(ExtensionSystem::SettingsPtr settings, const QString &prefix)
{
    splitter_->restoreState(settings->value(prefix + "/SplitterState")
                            .toByteArray());
    splitter_->restoreGeometry(settings->value(prefix + "/SplitterGeometry")
                               .toByteArray());
    sidePanel_->restoreState(settings, prefix + "/SideBar");
    setSize(pClass_->size());
    if (settings->value(prefix + "/CompactMode").toBool())
        switchToCompactMode(true);
    else
        switchToEnoughtSizeMode(true);
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
    if (content_->isEmpty()) {
        content_->renderData(doc.root_);
    }
    return doc;
}

bool DocBookViewImpl::hasAlgorithm(const QString &name) const
{
    const ModelPtr algorithm = sidePanel_->findApiFunction(name);
    return ! algorithm.isNull();
}

void DocBookViewImpl::navigateToApiFunction(const QString & package, const QString &function)
{
    const ModelPtr algorithm = sidePanel_->findApiFunction(package, function);
    if (algorithm) {
        sidePanel_->selectItem(algorithm, function);
        showAnItem(algorithm);
    }
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

QStringList DocBookViewImpl::booksList() const
{
    QStringList result;
    if (sidePanel_) {
        QList<ModelPtr> docs = sidePanel_->loadedDocuments();
        for (int i=0; i<docs.size(); i++) {
            ModelPtr doc = docs[i];
            result.push_back(doc->title());
        }
    }
    return result;
}

void DocBookViewImpl::activateBookIndex(int index)
{
    ModelPtr target;
    if (sidePanel_ && index >= 0) {
        QList<ModelPtr> docs = sidePanel_->loadedDocuments();
        if (index < docs.size()) {
            target = docs[index];
        }
    }
    if (target) {
        showAnItem(target);
    }
}

}
