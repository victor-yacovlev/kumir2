#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QUrl>
#include <QSharedPointer>

namespace DocBookViewer {

class DocBookModel;
typedef QSharedPointer<DocBookModel> ModelPtr;

class Document
{
    friend class DocBookFactory;
    friend class DocBookViewImpl;
    friend class PrintDialog;
public:
    const QUrl & url() const;

protected /* methods */:
    explicit Document(const QUrl & url, ModelPtr model);

protected /* fields */:
    QUrl url_;
    ModelPtr root_;
};

}

#endif
