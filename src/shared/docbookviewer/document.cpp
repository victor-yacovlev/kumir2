#include "document.h"
#include "docbookmodel.h"

namespace DocBookViewer
{

Document::Document(const QUrl & url, ModelPtr model)
    : url_(url)
    , root_(model)
{
}


const QUrl& Document::url() const
{
    return url_;
}

}
