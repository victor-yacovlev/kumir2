#ifndef DOCBOOKFACTORY_H
#define DOCBOOKFACTORY_H

#include "document.h"

// Qt includes
#include <QXmlDefaultHandler>
#include <QXmlSimpleReader>
#include <QUrl>

namespace DocBookViewer {

class DocBookFactory
        : public QXmlDefaultHandler
{

public:
    static DocBookFactory* self();
    Document parseDocument(const QUrl & url, QString * error = 0) const;



private /*methods*/:
    class DocBookModel* parseDocument(QIODevice * stream,
                                      const QUrl & url,
                                      QString * error = 0) const;

    explicit DocBookFactory();

    bool startDocument();
    bool startElement(const QString &namespaceURI,
                      const QString &localName,
                      const QString &qName,
                      const QXmlAttributes &atts);
    bool characters(const QString &ch);
    bool endElement(const QString &namespaceURI,
                    const QString &localName,
                    const QString &qName);

    void filterByOs(DocBookModel * root) const;

private /*fields*/:
    mutable QXmlSimpleReader* reader_;
    mutable DocBookModel* doc_;
    mutable QUrl url_;

    DocBookModel* root_;
    QString buffer_;

};

}

#endif
