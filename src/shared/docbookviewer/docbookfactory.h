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
    static ModelPtr createListOfExamples(ModelPtr root);



private /*methods*/:
    ModelPtr parseDocument(QIODevice * stream,
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

    bool error(const QXmlParseException &exception);
    bool fatalError(const QXmlParseException &exception);
    bool warning(const QXmlParseException &exception);

    void filterByOs(ModelPtr root) const;

    static QList<ModelPtr> findExamples(ModelPtr root);


private /*fields*/:
    mutable QXmlSimpleReader* reader_;
    mutable ModelPtr doc_;
    mutable QUrl url_;

    ModelPtr root_;
    QString buffer_;

};

}

#endif
