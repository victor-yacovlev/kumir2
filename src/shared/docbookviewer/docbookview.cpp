#include "docbookview.h"
#include "docbookview_impl.h"

namespace DocBookViewer {

DocBookView::DocBookView(QWidget *parent)
    : QWidget(parent)
    , pImpl_(new DocBookViewImpl(this))
{

}

void DocBookView::updateSettings(QSettings *settings, const QString & prefix)
{
    pImpl_->updateSettings(settings, prefix);
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

}
