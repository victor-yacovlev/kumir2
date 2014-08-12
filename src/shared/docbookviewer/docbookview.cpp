#include "docbookview.h"
#include "docbookview_impl.h"

#include <QResizeEvent>

namespace DocBookViewer {

DocBookView::DocBookView(QWidget *parent)
    : QWidget(parent)
    , pImpl_(new DocBookViewImpl(this))
{

}

void DocBookView::updateSettings(ExtensionSystem::SettingsPtr settings, const QString &prefix)
{
    pImpl_->updateSettings(settings, prefix);
}

void DocBookView::closeEvent(QCloseEvent * event)
{
    if (pImpl_->settings_) {
        pImpl_->saveState(pImpl_->settings_, pImpl_->settingsPrefix_);
    }
    QWidget::closeEvent(event);
}

void DocBookView::hideEvent(QHideEvent * event)
{
    if (pImpl_->settings_) {
        pImpl_->saveState(pImpl_->settings_, pImpl_->settingsPrefix_);
    }
    QWidget::hideEvent(event);
}

void DocBookView::showEvent(QShowEvent * event)
{
    if (pImpl_->settings_) {
        pImpl_->restoreState(pImpl_->settings_, pImpl_->settingsPrefix_);
    }
    QWidget::showEvent(event);
}

void DocBookView::resizeEvent(QResizeEvent * event)
{
    if (event->size().width() < minimumWidth() ||
            event->size().height() < minimumHeight()) {
        event->ignore();
    }
    else {
        event->accept();
        pImpl_->setSize(event->size());
    }
}


DocBookView::~DocBookView()
{
    delete pImpl_;
}

QAction * DocBookView::viewerAction(const DocBookViewAction type) const
{
    return pImpl_->viewerAction(type);
}

Document DocBookView::addDocument(const QUrl &url, QString *error, int index)
{
    return pImpl_->addDocument(url, error, index);
}

void DocBookView::removeDocument(const Document & existingDocument)
{
    pImpl_->removeDocument(existingDocument);
}

QSize DocBookView::minimumSizeHint() const
{
    return pImpl_->minimumSizeHint();
}

QSize DocBookView::sizeHint() const
{
    return pImpl_->sizeHint();
}

bool DocBookView::hasAlgorithm(const QString &name) const
{
    return pImpl_->hasAlgorithm(name);
}

void DocBookView::navigateToApiFunction(const QString &package, const QString & function)
{
    pImpl_->navigateToApiFunction(package, function);
}

QStringList DocBookView::booksList() const
{
    return pImpl_->booksList();
}

void DocBookView::activateBookIndex(int index)
{
    pImpl_->activateBookIndex(index);
}

void DocBookView::setRole(ModelType category, const QString &value)
{
    pImpl_->setRole(category, value);
}

QString DocBookView::role(ModelType category) const
{
    return pImpl_->role(category);
}

}
