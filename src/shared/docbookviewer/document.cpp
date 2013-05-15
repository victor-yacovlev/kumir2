#include "document.h"
#include "docbookmodel.h"

namespace DocBookViewer
{

Document::Document(const QUrl & url, const DocBookModel * model)
    : url_(url)
    , root_(model)
{
}

const DocBookModel* Document::content() const
{
    return root_;
}

const QUrl& Document::url() const
{
    return url_;
}

}
