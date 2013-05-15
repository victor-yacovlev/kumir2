#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QUrl>

namespace DocBookViewer {

class Document
{
    friend class DocBookFactory;
public:

    const QUrl & url() const;
    const class DocBookModel* content() const;

protected /* methods */:
    explicit Document(const QUrl & url,
                      const class DocBookModel * model);


protected /* fields */:
    QUrl url_;
    const class DocBookModel* root_;
};

}

#endif
