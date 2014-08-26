#ifndef DOCBOOKFACTORY_H
#define DOCBOOKFACTORY_H

#include "docbookview.h"
#include "document.h"
#include "docbookmodel.h"

// Qt includes
#include <QXmlDefaultHandler>
#include <QXmlSimpleReader>
#include <QUrl>


#ifdef LIBRARY_BUILD
#define EXTERN Q_DECL_EXPORT
#else
#define EXTERN Q_DECL_IMPORT
#endif

namespace DocBookViewer {

class EXTERN DocBookFactory
        : public QXmlDefaultHandler
{

public:
    static DocBookFactory* self();
    Document parseDocument(const QMap<ModelType,QString> roleValues, const QUrl & url, QString * error = 0) const;
    static ModelPtr createListOfEntries(ModelPtr root,
                                        ModelType resType,
                                        ModelType findType
                                        );
    static QMap<QString,ModelPtr> &
    updateListOfAlgorithms(ModelPtr root, QMap<QString,ModelPtr> &result);



private /*methods*/:
    ModelPtr parseDocument(
            const QMap<ModelType,QString> & roles,
            QIODevice * stream,
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
    void filterByRoles(const QMap<ModelType,QString> & roles,
                       ModelPtr root) const;
    void filterByConfiguration(ModelPtr root) const;

    static QList<ModelPtr> findEntriesOfType(ModelPtr root,
                                             ModelType findType
                                             );

    static QByteArray loadAndPreprocessSvg(const QString & fileName);
    static QImage loadAndPreprocessPng(const QString & fileName);


private /*fields*/:
    mutable QXmlSimpleReader* reader_;
    mutable ModelPtr doc_;
    mutable QUrl url_;
    mutable QMap<ModelType,QString> roles_;

    ModelPtr root_;
    QString buffer_;

};

}

#endif
