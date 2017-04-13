#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QUrl>
#include <QSharedPointer>

#ifdef DOCBOOKVIEWER_LIBRARY
#define EXTERN Q_DECL_EXPORT
#else
#define EXTERN Q_DECL_IMPORT
#endif

namespace DocBookViewer {

class DocBookModel;
typedef QSharedPointer<DocBookModel> ModelPtr;

class EXTERN Document
{
    friend class DocBookFactory;
    friend class DocBookViewImpl;
    friend class PrintDialog;
    friend class SidePanel;
public:
    const QUrl & url() const;
    QString title() const;
    ModelPtr documentRoot() const;

protected /* methods */:
    explicit Document(const QUrl & url, ModelPtr model);

protected /* fields */:
    QUrl url_;
    ModelPtr root_;
};

}

#endif
