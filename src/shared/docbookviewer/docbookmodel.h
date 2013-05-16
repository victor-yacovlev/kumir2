#ifndef DOCBOOKMODEL_H
#define DOCBOOKMODEL_H

// Qt includes
#include <QtGlobal>
#include <QString>
#include <QObject>
#include <QSharedPointer>

namespace DocBookViewer {

static const quint8 MAX_SECTION_LEVEL_IN_TOC = 2u;
typedef QSharedPointer<class DocBookModel> ModelPtr;
typedef QList<ModelPtr>::iterator ModelIterator;

class DocBookModel
{
    friend class DocBookFactory;
public:
    enum ModelType {
        Text,
        Book,
        Article,
        Set,
        Chapter,
        Section,
        Preface,
        Reference,
        Abstract,
        Para,
        Example,
        ProgramListing,
        OrderedList,
        ItemizedList,
        ListItem,
        Xref,
        Emphasis,
        XInfo,
        KeywordSet,
        Keyword,
        Synopsis,
        KeyCombo,
        KeySym
    };

    quint8 sectionLevel() const;
    ModelType modelType() const;
    const QString & id() const;
    const QString & title() const;
    const QString & subtitle() const;
    const QString & text() const;
    const QString & role() const;
    const QString & xrefLinkEnd() const;
    const QString & xrefEndTerm() const;
    ModelPtr parent() const;
    const QList<ModelPtr>& children() const;
    void setParent(ModelPtr parent);
    bool isSectioningNode() const;

protected /*methods*/:
    explicit DocBookModel(ModelPtr parent, const ModelType modelType);
    void updateSectionLevel();

protected /*fields*/:
    ModelPtr parent_;
    QList<ModelPtr> children_;
    ModelType modelType_;
    unsigned short sectionLevel_;
    QString title_;
    QString subtitle_;
    QString text_;
    QString id_;
    QString os_;
    QString xrefLinkEnd_;
    QString xrefEndTerm_;
    QString role_;
};

}

#endif
