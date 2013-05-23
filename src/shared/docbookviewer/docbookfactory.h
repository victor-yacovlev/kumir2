#ifndef DOCBOOKFACTORY_H
#define DOCBOOKFACTORY_H

#include "document.h"
#include "docbookmodel.h"

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
    static ModelPtr createListOfEntries(ModelPtr root,
                                        DocBookModel::ModelType resType,
                                        DocBookModel::ModelType findType
                                        );
    static QMap<QString,ModelPtr> &
    updateListOfAlgorithms(ModelPtr root, QMap<QString,ModelPtr> &result);



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
    bool skippedEntity(const QString &name);
    bool endElement(const QString &namespaceURI,
                    const QString &localName,
                    const QString &qName);

    bool error(const QXmlParseException &exception);
    bool fatalError(const QXmlParseException &exception);
    bool warning(const QXmlParseException &exception);

    void filterByOs(ModelPtr root) const;

    static QList<ModelPtr> findEntriesOfType(ModelPtr root,
                                             DocBookModel::ModelType findType
                                             );


private /*fields*/:
    mutable QXmlSimpleReader* reader_;
    mutable ModelPtr doc_;
    mutable QUrl url_;

    ModelPtr root_;
    QString buffer_;

};

}

#endif
